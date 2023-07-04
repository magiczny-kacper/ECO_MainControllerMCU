/*
 * flash_map_backend.h
 *
 *  Created on: Mar 3, 2023
 *      Author: kapib
 */

#ifndef FLASH_MAP_BACKEND_H_
#define FLASH_MAP_BACKEND_H_

#include <inttypes.h>

struct flash_area {
  uint8_t  fa_id;         /** The slot/scratch identification */
  uint8_t  fa_device_id;  /** The device id (usually there's only one) */
  uint16_t pad16;
  uint32_t fa_off;        /** The flash offset from the beginning */
  uint32_t fa_size;       /** The size of this sector */
};

//! Structure describing a sector within a flash area.
struct flash_sector {
  //! Offset of this sector, from the start of its flash area (not device).
  uint32_t fs_off;

  //! Size of this sector, in bytes.
  uint32_t fs_size;
};

/*< Obtains ID of the flash area characterized by `fa` */
int     flash_area_get_id(const struct flash_area *fa);
/*< Obtains ID of a device the flash area `fa` described region resides on */
int     flash_area_get_device_id(const struct flash_area *fa);
/*< Obtains offset, from the beginning of a device, the flash area described
 * region starts at */
uint32_t flash_area_get_off(const struct flash_area *fa);
/*< Obtains size, from the offset, of the flash area `fa` characterized region */
uint32_t flash_area_get_size(const struct flash_area *fa);

//! Opens the area for use. id is one of the `fa_id`s */
int flash_area_open(uint8_t id, const struct flash_area **area_outp);
void flash_area_close(const struct flash_area *fa);

//! Reads `len` bytes of flash memory at `off` to the buffer at `dst`
int flash_area_read(const struct flash_area *fa, uint32_t off,
                    void *dst, uint32_t len);
//! Writes `len` bytes of flash memory at `off` from the buffer at `src`
int flash_area_write(const struct flash_area *fa, uint32_t off,
                     const void *src, uint32_t len);
//! Erases `len` bytes of flash memory at `off`
int flash_area_erase(const struct flash_area *fa,
                     uint32_t off, uint32_t len);

//! Returns this `flash_area`s alignment
size_t flash_area_align(const struct flash_area *area);
//! Returns the value read from an erased flash area byte
uint8_t flash_area_erased_val(const struct flash_area *area);

//! Given flash area ID, return info about sectors within the area
int flash_area_get_sectors(int fa_id, uint32_t *count, struct flash_sector *sectors);

uint32_t flash_sector_get_size(struct flash_sector* fs);

uint32_t flash_sector_get_off(struct flash_sector* fs);

//! Returns the `fa_id` for slot, where slot is 0 (primary) or 1 (secondary).
//!
//! `image_index` (0 or 1) is the index of the image. Image index is
//!  relevant only when multi-image support support is enabled
int flash_area_id_from_multi_image_slot(int image_index, int slot);
int flash_area_id_from_image_slot(int slot);

#endif /* FLASH_MAP_BACKEND_H_ */
