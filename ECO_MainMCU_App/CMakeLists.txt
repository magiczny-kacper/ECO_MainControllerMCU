cmake_minimum_required (VERSION 3.10)

# set the build type
if(NOT CMAKE_BUILD_TYPE) 
    set(CMAKE_BUILD_TYPE Release)
endif(NOT CMAKE_BUILD_TYPE)

if(CMAKE_BUILD_TYPE MATCHES Debug)
  message("Debug build.")
elseif(CMAKE_BUILD_TYPE MATCHES Release)
  message("Release build.")
endif()

# set search path for CMake modules
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/CMake/Modules)
# set(CMAKE_TOOLCHAIN_FILE GNU-ARM-Toolchain.cmake)
include(GNU-ARM-Toolchain.cmake)

# Project specific settings
set(application_name "ECO_MainMCU_App")

project (${application_name} C ASM)

# Used linker file (just for testing)
get_filename_component(LINKER_SCRIPT STM32G0B1RETx_.ld ABSOLUTE)

# Use LTO? WARNING: This might break the code
#set(ADDITIONAL_CORE_FLAGS -flto)

# OpenOCD config file
get_filename_component(OPENOCD_CONFIG tools/stm32f4discovery.cfg ABSOLUTE)

include(file_list.cmake)

set(FREERTOS_CONFIG_FILE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/Application/RTOS CACHE STRING  "")
set(FREERTOS_PORT GCC_ARM_CM0 CACHE STRING "")

add_subdirectory(Application/RTOS/FreeRTOS)

# GCC optimization level: use -O0 in debug build, otherwise -O2
if(CMAKE_BUILD_TYPE MATCHES Debug)
  set(opt_level -Og)
else()
  set(opt_level -Os)
endif()

# set some project constants
set(elf_file ${application_name}.elf)
set(bin_file ${application_name}.bin)
set(signed_bin_file ${application_name}_signed.bin)
set(hex_file ${application_name}.hex)
set(signed_hex_file ${application_name}_signed.hex)
set(map_file ${application_name}.map)
set(lss_file ${application_name}.lss)

include_directories(${includes})
# add sources to elf file
add_executable(${elf_file} ${sources})

# target_link_libraries(${elf_file} PUBLIC )
# set additional for compiler and linker: optimization and generate map file
set(additional_compiler_flags ${opt_level})
set(additional_linker_flags -Wl,-Map=${map_file},--cref,--no-warn-mismatch)

add_definitions(
    -DMBEDTLS_NO_PLATFORM_ENTROPY
    -DMBEDTLS_SHA256_SMALLER
    -DUSE_HAL_DRIVER
    -DSTM32G0B1xx
    -DPRINTF_INCLUDE_CONFIG_H
)

target_link_libraries(${elf_file} PUBLIC mbedtls)
target_link_libraries(${elf_file} PUBLIC bootutil)
target_link_libraries(${elf_file} PUBLIC freertos_kernel)

target_compile_definitions(${elf_file} PUBLIC USE_HAL_DRIVER)
target_compile_definitions(${elf_file} PUBLIC STM32G0B1xx)
target_compile_definitions(${elf_file} PUBLIC MBEDTLS_NO_PLATFORM_ENTROPY)
target_compile_definitions(${elf_file} PUBLIC MBEDTLS_SHA256_SMALLER)
target_compile_definitions(${elf_file} PUBLIC PRINTF_INCLUDE_CONFIG_H)

target_compile_options(${elf_file} PRIVATE ${additional_compiler_flags})
target_link_libraries(${elf_file} PRIVATE ${additional_linker_flags})

# remove unused sections
target_link_libraries(${elf_file} PUBLIC "-g -Wl,--gc-sections")

# link with linker file
target_link_libraries(${elf_file} PUBLIC -T${LINKER_SCRIPT})

# show size of resulting firmware image
add_custom_target(${elf_file}-size DEPENDS ${elf_file} COMMAND ${ARM_SIZE_EXECUTABLE} -B ${elf_file})

# generate extended listing
add_custom_target(${lss_file} DEPENDS ${elf_file} COMMAND ${ARM_OBJDUMP_EXECUTABLE} -S ${elf_file} > ${lss_file})

# create binary and hex files
add_custom_target(${hex_file}
    DEPENDS ${elf_file}
    COMMAND ${ARM_OBJCOPY_EXECUTABLE} -Oihex ${elf_file} ${hex_file}
)

add_custom_target(${bin_file}
    DEPENDS ${elf_file}
    COMMAND ${ARM_OBJCOPY_EXECUTABLE} -Obinary ${elf_file} ${bin_file}
)

add_custom_target(${signed_hex_file}
    DEPENDS ${hex_file}
    COMMAND ${PYTHON} ${mcuboot_root}/scripts/imgtool.py sign
    --header-size 0x200
    --align 8
    --slot-size 0x60000
    --version 1.0.0
    --pad-header
    ${hex_file}
    ${signed_hex_file}
)

add_custom_target(${signed_bin_file}
    DEPENDS ${signed_hex_file}
    COMMAND ${ARM_OBJCOPY_EXECUTABLE} 
    --input-target=ihex --output-target=binary
    ${signed_hex_file} ${signed_bin_file}
)

add_custom_target(${application_name} ALL
    DEPENDS ${elf_file}-size ${bin_file} ${hex_file} ${lss_file} ${signed_hex_file} ${signed_bin_file}
)

# # OpenOCD targets
# add_custom_target(flash DEPENDS ${elf_file} COMMAND ${OPENOCD_EXECUTABLE}
#   -f ${OPENOCD_CONFIG}
#   --command "program ${elf_file} reset exit")

#   # The port that OpenOCD will serve a gdb interface on
# if(NOT GDB_DEBUG_PORT)
#   set(GDB_DEBUG_PORT "4242")
# endif()

# # Connect OCD to the device and serve for an incoming GDB connection
# add_custom_target(ocd_serve DEPENDS ${elf_file} COMMAND ${OPENOCD_EXECUTABLE}
#   -f ${OPENOCD_CONFIG}
#   --command "gdb_port ${GDB_DEBUG_PORT}")

# # Run GDB against the target on the device, via OpenOCD
# add_custom_target(gdb_connect DEPENDS ${elf_file} COMMAND ${ARM_GDB_EXECUTABLE}
#   -ex "target remote localhost:${GDB_DEBUG_PORT}"
#   -ex "monitor reset halt"
#   ${elf_file})
