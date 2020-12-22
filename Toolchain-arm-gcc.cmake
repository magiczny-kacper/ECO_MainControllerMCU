set(CMAKE_GENERATOR "MinGW Makefiles")

set(MCU_LINKER_SCRIPT STM32F401CCUX_FLASH.ld)
set(MCU_ARCH cortex-m4)
set(MCU_FLOAT_ABI hard)
set(MCU_FPU fpv4-sp-d16)

set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_VERSION 1)

set(CMAKE_C_COMPILER arm-none-eabi-gcc)
set(CMAKE_ASM_COMPILER arm-none-eabi-gcc)
set(CMAKE_CXX_COMPILER arm-none-eabi-g++)
set(CMAKE_SZ arm-none-eabi-size)

set(CMAKE_C_COMPILER_WORKS ON)
set(CMAKE_CXX_COMPILER_WORKS ON)

set(COMMON_FLAGS "-mcpu=${MCU_ARCH} -mthumb -mthumb-interwork -mfloat-abi=${MCU_FLOAT_ABI} -ffunction-sections -fdata-sections -Og -g3 -Wall -Wextra -Wswitch-default -fstack-usage --specs=nano.specs")

if (MCU_FLOAT_ABI STREQUAL hard)
    set(COMMON_FLAGS "${COMMON_FLAGS} -mfpu=${MCU_FPU}")
endif ()

set(CMAKE_CXX_FLAGS "${COMMON_FLAGS} -std=c++11")
set(CMAKE_C_FLAGS "${COMMON_FLAGS} -std=gnu11")
set(LD_FLAGS "-Wl,--gc-sections -Wl,--print-memory-usage")

macro(add_arm_executable target_name)
# Output files
set(elf_file ${target_name}.elf)
set(map_file ${target_name}.map)
set(hex_file ${target_name}.hex)
set(bin_file ${target_name}.bin)
set(lss_file ${target_name}.lss)
set(dmp_file ${target_name}.dmp)
add_executable(${elf_file} ${ARGN})
#generate hex file
add_custom_command(
  OUTPUT ${hex_file}
  COMMAND
    ${CMAKE_OBJCOPY} -O ihex ${elf_file} ${hex_file}
  DEPENDS ${elf_file}
)
# #generate bin file
add_custom_command(
  OUTPUT ${bin_file}
  COMMAND
    ${CMAKE_OBJCOPY} -O binary ${elf_file} ${bin_file}
  DEPENDS ${elf_file}
)
# #generate extended listing
add_custom_command(
  OUTPUT ${lss_file}
  COMMAND
    ${CMAKE_OBJDUMP} -h -S ${elf_file} > ${lss_file}
  DEPENDS ${elf_file}
)
# #generate memory dump
add_custom_command(
  OUTPUT ${dmp_file}
  COMMAND
    ${CMAKE_OBJDUMP} -x --syms ${elf_file} > ${dmp_file}
  DEPENDS ${elf_file}
)
#postprocessing from elf file - generate hex bin etc.
add_custom_target(
  ${target_name}
  ALL
  DEPENDS ${hex_file} ${bin_file} ${lss_file} ${dmp_file}
)
set_target_properties(
  ${target_name}
  PROPERTIES
    OUTPUT_NAME ${elf_file}
)
endmacro(add_arm_executable)