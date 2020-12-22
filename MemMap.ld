MEMORY
{
  RTOS_STACK	(xrw)	: ORIGIN = 0x20000000,	LENGTH = 8K
  RAM    		(xrw)   : ORIGIN = 0x20002000,	LENGTH = 56K
  BOOT_FLASH 	(rx) 	: ORIGIN = 0x8000000, 	LENGTH = 32K
  APP_FLASH    	(rx)    : ORIGIN = 0x8008000,   LENGTH = 112K
  BACKUP_FLASH  (rx)    : ORIGIN = 0x8020000,   LENGTH = 112K
}

__boot_flash_start__ = ORIGIN(BOOT_FLASH);
__boot_flash_end__ = ORIGIN(BOOT_FLASH) + LENGTH(BOOT_FLASH);
__app_flash_start__ = ORIGIN(APP_FLASH);
__app_flash_end__ = ORIGIN(APP_FLASH) + LENGTH(APP_FLASH);
__backup_flash_start__ = ORIGIN(BACKUP_FLASH);
__backup_flash_end__ = ORIGIN(BACKUP_FLASH) + LENGTH(BACKUP_FLASH);
__ram_start__ = ORIGIN(RAM);
__ram_end__ = ORIGIN(RAM) + LENGTH(RAM);