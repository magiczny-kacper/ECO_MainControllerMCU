set(PRJ_NAME SuncontrollerCPU)
set(MCU_FAMILY STM32F4xx)
set(MCU_LINE STM32F401CCUx)
set(MCU_APP_LINKER_SCRIPT STM32F401CCUX_FLASH.ld)
set(MCU_BOOT_LINKER_SCRIPT STM32F401CCUX_BOOT.ld)
cmake_minimum_required(VERSION 3.6)

set(CMAKE_C_COMPILER arm-none-eabi-gcc)
set(CMAKE_ASM_COMPILER arm-none-eabi-gcc)
set(CMAKE_CXX_COMPILER arm-none-eabi-g++)

project(${PRJ_NAME} C ASM)

add_definitions(-DSTM32F401xC)
add_definitions(-DUSE_HAL_LIBRARY)

file(GLOB_RECURSE USER_SOURCES Core/Src/*.c)
file(GLOB_RECURSE HAL_SOURCES Drivers/${MCU_FAMILY}_HAL_DRIVER/Src/*.c)
file(GLOB_RECURSE CMSIS_SYSTEM Core/Src/system_stm32f4xx.c)
file(GLOB_RECURSE MIDLLEWARE_SYSTEM Middlewares/Third_Party/*.c)
file(GLOB_RECURSE CMSIS_STARTUP Core/Startup/startup_stm32f401ccux.s)

set(APP_SOURCE_FILES ${CMSIS_STARTUP} ${USER_SOURCES} ${HAL_SOURCES} ${CMSIS_SYSTEM} ${MIDLLEWARE_SYSTEM} ${MCU_APP_LINKER_SCRIPT})

file(GLOB_RECURSE BOOTLOADER_SOURCES BootloaderSrc/*.c)

set(BOOT_SOURCE_FILES ${BOOTLOADER_SOURCES} ${CMSIS_STARTUP} ${CMSIS_SYSTEM} ${MCU_BOOT_LINKER_SCRIPT})

# Application includes
include_directories(Drivers/CMSIS/Device/ST/${MCU_FAMILY}/Include)
include_directories(Drivers/CMSIS/Include)
include_directories(Drivers/${MCU_FAMILY}_HAL_Driver/Inc)
include_directories(Middlewares/Third_Party/FreeRTOS/Source/include)
include_directories(Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2)
include_directories(Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F)
include_directories(Middlewares/Third_Party/FreeRTOS_CLI)
include_directories(Middlewares/Third_Party/printf)
include_directories(Core/Inc)
include_directories(Core/Src/Ethernet)
include_directories(Core/Src/Internet)

add_executable(${PROJECT_NAME}_app ${APP_SOURCE_FILES})
#set_target_properties(${CMAKE_PROJECT_NAME}_app PROPERTIES COMPILE_FLAGS ${MODULES_FLAGS})

add_executable(${PROJECT_NAME}_boot ${BOOT_SOURCE_FILES})

set_target_properties(${CMAKE_PROJECT_NAME}_app PROPERTIES
	SUFFIX ".elf"
  LINK_FLAGS "${LD_FLAGS} -T ${CMAKE_SOURCE_DIR}/${MCU_APP_LINKER_SCRIPT} -Wl,-Map=${CMAKE_SOURCE_DIR}/app.map" )

set_target_properties(${CMAKE_PROJECT_NAME}_boot PROPERTIES
	SUFFIX ".elf"
  LINK_FLAGS "${LD_FLAGS} -T ${CMAKE_SOURCE_DIR}/${MCU_BOOT_LINKER_SCRIPT} -Wl,-Map=${CMAKE_SOURCE_DIR}/boot.map")

add_custom_command(TARGET ${CMAKE_PROJECT_NAME}_app POST_BUILD
  COMMAND ${CMAKE_OBJCOPY} -O binary ${CMAKE_PROJECT_NAME}_app.elf ${CMAKE_PROJECT_NAME}_app.bin
  COMMAND ${CMAKE_SZ} ${CMAKE_PROJECT_NAME}_app.elf
  COMMENT "Converting the elf output to a binary file"
  )

add_custom_command(TARGET ${CMAKE_PROJECT_NAME}_boot POST_BUILD
  COMMAND ${CMAKE_OBJCOPY} --pad-to=0x4000 --gap-fill=0xFF -O binary ${CMAKE_PROJECT_NAME}_boot.elf ${CMAKE_PROJECT_NAME}_boot.bin
  COMMAND ${CMAKE_SZ} ${CMAKE_PROJECT_NAME}_boot.elf
  COMMENT "Converting the elf output to a binary file"
  )

add_custom_command(
  OUTPUT ${CMAKE_PROJECT_NAME}.bin
  COMMAND cat ${CMAKE_PROJECT_NAME}_boot.bin ${CMAKE_PROJECT_NAME}_app.bin > ${CMAKE_PROJECT_NAME}.bin
  DEPENDS ${CMAKE_PROJECT_NAME}_boot.bin ${CMAKE_PROJECT_NAME}_app.bin
)
