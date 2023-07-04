/*
 * flash_map_backend.c
 *
 *  Created on: Mar 3, 2023
 *      Author: kapib
 */

#include <string.h>
#include <stdlib.h>

#include "mcuboot_config/mcuboot_logging.h"

#include "flash_map_backend/flash_map_backend.h"
#include "sysflash/sysflash.h"

#include "stm32g0xx_hal.h"
#include "W25Q64_Flash.h"

#define BOOTLOADER_SIZE (128 * 1024)
#define APPLICATION_SIZE (384 * 1024)
#define SCRATCH_SIZE (128 * 1024)

#define BOOTLOADER_START_ADDRESS 0x00000000
#define APPLICATION_PRIMARY_START_ADDRESS (BOOTLOADER_START_ADDRESS + BOOTLOADER_SIZE)
#define APPLICATION_SECONDARY_START_ADDRESS (0x00000000)

#define SCRATCH_START_ADDRESS (APPLICATION_SECONDARY_START_ADDRESS + APPLICATION_SIZE)

static const struct flash_area bootloader =
{
    .fa_id = FLASH_AREA_BOOTLOADER,
    .fa_device_id = FLASH_DEVICE_INTERNAL_FLASH,
    .fa_off = BOOTLOADER_START_ADDRESS,
    .fa_size = BOOTLOADER_SIZE
};

static const struct flash_area primary_img0 =
{
    .fa_id = FLASH_AREA_IMAGE_PRIMARY(0),
    .fa_device_id = FLASH_DEVICE_INTERNAL_FLASH,
    .fa_off = APPLICATION_PRIMARY_START_ADDRESS,
    .fa_size = APPLICATION_SIZE
};

static const struct flash_area secondary_img0 =
{
    .fa_id = FLASH_AREA_IMAGE_SECONDARY(0),
    .fa_device_id = FLASH_DEVICE_EXTENRAL_SPI,
    .fa_off = APPLICATION_SECONDARY_START_ADDRESS,
    .fa_size = APPLICATION_SIZE
};

static const struct flash_area scratch =
{
    .fa_id = FLASH_AREA_IMAGE_SCRATCH,
    .fa_device_id = FLASH_DEVICE_EXTENRAL_SPI,
    .fa_off = SCRATCH_START_ADDRESS,
    .fa_size = SCRATCH_SIZE
};

static const struct flash_area event_log_area =
{
    .fa_id = FLASH_AREA_EVENT_LOG,
    .fa_device_id = FLASH_DEVICE_EXTENRAL_SPI,
    .fa_off = SCRATCH_START_ADDRESS + SCRATCH_SIZE,
    .fa_size = 0x800000 - (SCRATCH_START_ADDRESS + SCRATCH_SIZE)
};

static const struct flash_area *s_flash_areas[] =
{ 
    &bootloader, 
    &primary_img0, 
    &secondary_img0, 
    &scratch,
    &event_log_area
};

static const struct flash_area* prv_lookup_flash_area(uint8_t id)
{
#define ARRAY_SIZE(arr) sizeof(arr)/sizeof(arr[0])
    for (size_t i = 0; i < ARRAY_SIZE(s_flash_areas); i++)
    {
        const struct flash_area *area = s_flash_areas[i];
        if (id == area->fa_id)
        {
            return area;
        }
    }
    return NULL;
}

int flash_area_get_id(const struct flash_area *fa)
{
    if (fa != NULL)
    {
        return (int) fa->fa_id;
    }
    else
    {
        return -1;
    }
}

int flash_area_get_device_id(const struct flash_area *fa)
{
    if (fa != NULL)
    {
        return (int) fa->fa_device_id;
    }
    else
    {
        return -1;
    }
}

uint32_t flash_area_get_off(const struct flash_area *fa)
{
    if (fa != NULL)
    {
        return fa->fa_off;
    }
    else
    {
        return 0;
    }
}

uint32_t flash_area_get_size(const struct flash_area *fa)
{
    if (fa != NULL)
    {
        return fa->fa_size;
    }
    else
    {
        return 0;
    }
}

int flash_area_open(uint8_t id, const struct flash_area **area_outp)
{
    MCUBOOT_LOG_DBG("%s: ID=%d", __func__, (int )id);

    const struct flash_area *area = prv_lookup_flash_area(id);
    *area_outp = area;

    if (NULL != area)
    {
        return 0;
    }
    return -1;
}

void flash_area_close(const struct flash_area *area)
{
    (void)area;
}

int flash_area_read(const struct flash_area *fa, uint32_t off, void *dst, uint32_t len)
{
    if (fa->fa_device_id == FLASH_DEVICE_INTERNAL_FLASH)
    {
        const uint32_t end_offset = off + len;
        if (end_offset > fa->fa_size)
        {
            MCUBOOT_LOG_ERR("%s: Out of Bounds (0x%x vs 0x%x)", __func__,
                    end_offset, fa->fa_size);
            __NOP();
            return -1;
        }

        // internal flash is memory mapped so just dereference the address
        void *addr = (void*) (fa->fa_off + off + 0x08000000);
        memcpy(dst, addr, len);

        return 0;
    }
    else if (fa->fa_device_id == FLASH_DEVICE_EXTENRAL_SPI)
    {
        const uint32_t end_offset = off + len;
        if (end_offset > fa->fa_size)
        {
            MCUBOOT_LOG_ERR("%s: Out of Bounds (0x%x vs 0x%x)", __func__,
                    end_offset, fa->fa_size);
            return -1;
        }

        // internal flash is memory mapped so just dereference the address
        uint32_t addr = (void*) (fa->fa_off + off + 0x08000000);
        if (HAL_OK == W25Q64_ReadDataBytes(addr, dst, len))
        {
            return 0;
        }
        else
        {
            return -1;
        }
    }
    else
    {
        return -1;
    }
}

int flash_area_write(const struct flash_area *fa, uint32_t off, const void *src, uint32_t len)
{
    if ((fa->fa_device_id != FLASH_DEVICE_INTERNAL_FLASH) && (fa->fa_device_id != FLASH_DEVICE_EXTENRAL_SPI))
    {
        return -1;
    }

    const uint32_t end_offset = off + len;
    if ((end_offset) > fa->fa_size)
    {
        MCUBOOT_LOG_ERR("%s: Out of Bounds (0x%x vs 0x%x)", __func__,
                end_offset, fa->fa_size);
        return -1;
    }

    if (fa->fa_device_id == FLASH_DEVICE_INTERNAL_FLASH)
    {
        uint32_t addr = fa->fa_off + off + 0x08000000;
        MCUBOOT_LOG_DBG("%s: Addr: 0x%08x Length: %d", __func__, (int)addr, (int)len);

        HAL_FLASH_Unlock();
        while (len)
        {
            if (len >= 256)
            {
                HAL_FLASH_Program(FLASH_TYPEPROGRAM_FAST, addr, src);
                addr += 256;
                src += 256;
                len -= 256;
            }
            else 
            {
                HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, addr, src);
                addr += 8;
                src += 8;
                len -= 8;
            }
        }
        HAL_FLASH_Lock();
    }
    else if (fa->fa_device_id == FLASH_DEVICE_EXTENRAL_SPI)
    {
        uint32_t addr = fa->fa_off + off;
        MCUBOOT_LOG_DBG("%s: Addr: 0x%08x Length: %d", __func__, (int)addr, (int)len);

        if (0 != (addr % 256))
        {
            return -1;
        }

        W25Q64_WriteEnable_and_WaitForWriteEnableLatch();
        while (len)
        {
            W25Q64_PageProgram(addr, (uint8_t*)src, 256);
            while (W25Q64_ReadStatusRegister1() & 1)
            {
                HAL_Delay(5);
            }
            addr += 256;
            src += 256;
            len -= 256;
        }
        W25Q64_WriteDisable();
    }
    else
    {
        return -1;
    }

    return 0;
}

int flash_area_erase(const struct flash_area *fa, uint32_t off, uint32_t len)
{
    uint32_t PageError;
    HAL_StatusTypeDef erase_status;
    if (fa->fa_device_id == FLASH_DEVICE_INTERNAL_FLASH)
    {
        if ((len % FLASH_PAGE_SIZE) != 0 || (off % FLASH_PAGE_SIZE) != 0)
        {
            MCUBOOT_LOG_ERR("%s: Not aligned on sector Offset: 0x%x Length: 0x%x",
                    __func__, (int )off, (int )len);
            return -1;
        }

        const uint32_t start_addr = fa->fa_off + off + 0x08000000;
        MCUBOOT_LOG_DBG("%s: Addr: 0x%08x Length: %d", __func__, (int )start_addr,
                (int )len);

        for (size_t i = 0; i < len; i += FLASH_PAGE_SIZE)
        {
            const uint32_t addr = start_addr + i;
            FLASH_EraseInitTypeDef erase =
            {
                .TypeErase = FLASH_TYPEERASE_PAGES,
                .Page = addr / FLASH_PAGE_SIZE,
                .NbPages = 1,
                .Banks = addr < (256 * 1024) ? FLASH_BANK_1 : FLASH_BANK_2 
            };
            erase_status = HAL_FLASHEx_Erase(&erase, &PageError);
            if (erase_status != HAL_OK)
            {
                return -1;
            }
            MCUBOOT_LOG_DBG("%s: Erased %d of %d blocks", __func__,
                    i / FLASH_PAGE_SIZE, len / FLASH_PAGE_SIZE);
        }

    #if VALIDATE_PROGRAM_OP
        for (size_t i = 0; i < len; i++)
        {
            uint8_t *val = (void *)(start_addr + i);
            if (*val != 0xff)
            {
                MCUBOOT_LOG_ERR("%s: Erase at 0x%x Failed", __func__, (int)val);
                assert(0);
            }
        }
    #endif
    }
    else if (fa->fa_device_id == FLASH_DEVICE_EXTENRAL_SPI)
    {
        if ((len % 4096) != 0 || (off % 4096) != 0)
        {
            MCUBOOT_LOG_ERR("%s: Not aligned on sector Offset: 0x%x Length: 0x%x",
                    __func__, (int )off, (int )len);
            return -1;
        }

        const uint32_t start_addr = fa->fa_off + off;
        MCUBOOT_LOG_DBG("%s: Addr: 0x%08x Length: %d", __func__, (int )start_addr,
                (int )len);

        for (size_t i = 0; i < len; i += 4096)
        {
            const uint32_t addr = start_addr + i;
            const uint32_t sector = addr / 4096;

            erase_status = W25Q64_SectorErase(sector);
//            if (erase_status != HAL_OK)
//            {
//                return -1;
//            }
            while(W25Q64_ReadStatusRegister1() & 1)
            {
                HAL_Delay(5);
            }
            MCUBOOT_LOG_DBG("%s: Erased %d of %d blocks", __func__,
                    i / 4096, len / 4096);
        }
    }
    else
    {
        return -1;
    }

    return 0;
}

size_t flash_area_align(const struct flash_area *area)
{
    if (area == NULL) return 0;
    else if (area -> fa_device_id == FLASH_DEVICE_INTERNAL_FLASH) return 8;
    else if (area -> fa_device_id == FLASH_DEVICE_EXTENRAL_SPI) return 256;
    else return 0;
}

uint8_t flash_area_erased_val(const struct flash_area *area)
{
    (void)area;
    return 0xff;
}

int flash_area_get_sectors(int fa_id, uint32_t *count, struct flash_sector *sectors)
{
    const struct flash_area *fa = prv_lookup_flash_area(fa_id);
    if ((fa->fa_device_id != FLASH_DEVICE_INTERNAL_FLASH) && (fa->fa_device_id != FLASH_DEVICE_EXTENRAL_SPI))
    {
        return -1;
    }

    // All sectors for the G0B1RE are the same size
    const size_t sector_size = fa->fa_device_id == FLASH_DEVICE_INTERNAL_FLASH ? FLASH_PAGE_SIZE : 4096;
    uint32_t total_count = 0;
    for (size_t off = 0; off < fa->fa_size; off += sector_size)
    {
        // Note: Offset here is relative to flash area, not device
        sectors[total_count].fs_off = off;
        sectors[total_count].fs_size = sector_size;
        total_count++;
    }

    *count = total_count;
    return 0;
}

int flash_area_id_from_multi_image_slot(int image_index, int slot)
{
    switch (slot)
    {
        case 0:
            return FLASH_AREA_IMAGE_PRIMARY(image_index);
        case 1:
            return FLASH_AREA_IMAGE_SECONDARY(image_index);
    }

    MCUBOOT_LOG_ERR("Unexpected Request: image_index=%d, slot=%d", image_index,
            slot);
    return -1; /* flash_area_open will fail on that */
}

uint32_t flash_sector_get_size(struct flash_sector* fs)
{
    if(fs != NULL)
    {
        return fs -> fs_size;
    }
    else return 0;
}

uint32_t flash_sector_get_off(struct flash_sector* fs)
{
    if(fs != NULL)
    {
        return fs -> fs_off;
    }
    else return 0;
}

int flash_area_id_from_image_slot(int slot)
{
    return flash_area_id_from_multi_image_slot(0, slot);
}

void example_assert_handler(const char *file, int line)
{
    (void)file;
    (void)line;
}
