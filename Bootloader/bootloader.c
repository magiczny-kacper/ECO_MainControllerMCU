#include <stdint.h>
#include "memorymap.h"

#pragma section ("boot_flash")

static void __attribute__((naked)) start_app(uint32_t pc, uint32_t sp);

int boot_main(void) {
  uint32_t *app_code = (uint32_t *)__app_flash_start__;
  uint32_t app_sp = app_code[0];
  uint32_t app_start = app_code[1];
  start_app(app_start, app_sp);
  /* Not Reached */
  while (1) {}
}

static void __attribute__((naked)) start_app(uint32_t pc, uint32_t sp){
    __asm("           \n\
          msr msp, r1 /* load r1 into MSP */\n\
          bx r0       /* branch to the address at r0 */\n\
    ");
}
