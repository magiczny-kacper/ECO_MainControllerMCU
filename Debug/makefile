################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include Middlewares/Third_Party/printf/subdir.mk
-include Middlewares/Third_Party/FreeRTOS_CLI/subdir.mk
-include Middlewares/Third_Party/FreeRTOS/Source/portable/MemMang/subdir.mk
-include Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F/subdir.mk
-include Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2/subdir.mk
-include Middlewares/Third_Party/FreeRTOS/Source/subdir.mk
-include Drivers/STM32F4xx_HAL_Driver/Src/subdir.mk
-include Core/Startup/subdir.mk
-include Core/Src/Utils/RuntimeStats/subdir.mk
-include Core/Src/Utils/RingBuffer/unity/fixture/subdir.mk
-include Core/Src/Utils/RingBuffer/unity/core/subdir.mk
-include Core/Src/Utils/RingBuffer/Src/subdir.mk
-include Core/Src/Utils/Modbus/subdir.mk
-include Core/Src/Utils/DataLog/subdir.mk
-include Core/Src/Utils/Config/subdir.mk
-include Core/Src/Internet/httpServer/subdir.mk
-include Core/Src/Internet/TFTP/subdir.mk
-include Core/Src/Internet/SNTP/subdir.mk
-include Core/Src/Internet/SNMP/subdir.mk
-include Core/Src/Internet/MQTT/MQTTPacket/src/subdir.mk
-include Core/Src/Internet/MQTT/subdir.mk
-include Core/Src/Internet/FTPServer/subdir.mk
-include Core/Src/Internet/FTPClient/subdir.mk
-include Core/Src/Internet/DNS/subdir.mk
-include Core/Src/Internet/DHCP/subdir.mk
-include Core/Src/Ethernet/W5500/subdir.mk
-include Core/Src/Ethernet/subdir.mk
-include Core/Src/DeviceDrivers/TemperatureSensors/subdir.mk
-include Core/Src/DeviceDrivers/Nextion/subdir.mk
-include Core/Src/DeviceDrivers/Flash/subdir.mk
-include Core/Src/DeviceDrivers/Expander/src/subdir.mk
-include Core/Src/DeviceDrivers/EnergyMeter/subdir.mk
-include Core/Src/DeviceDrivers/EEPROM/subdir.mk
-include Core/Src/DeviceDrivers/AnalogOut/subdir.mk
-include Core/Src/Application/Tasks/RegulationTask/subdir.mk
-include Core/Src/Application/Tasks/RadioTask/subdir.mk
-include Core/Src/Application/Tasks/EthernetTask/subdir.mk
-include Core/Src/Application/Tasks/DisplayTask/subdir.mk
-include Core/Src/Application/Tasks/DataLogTask/subdir.mk
-include Core/Src/Application/CLI/subdir.mk
-include Core/Src/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

BUILD_ARTIFACT_NAME := Suncontroller CPU
BUILD_ARTIFACT_EXTENSION := elf
BUILD_ARTIFACT_PREFIX := 
BUILD_ARTIFACT := $(BUILD_ARTIFACT_PREFIX)$(BUILD_ARTIFACT_NAME).$(BUILD_ARTIFACT_EXTENSION)

# Add inputs and outputs from these tool invocations to the build variables 
EXECUTABLES += \
Suncontroller\ CPU.elf \

SIZE_OUTPUT += \
default.size.stdout \

OBJDUMP_LIST += \
Suncontroller\ CPU.list \

OBJCOPY_BIN += \
Suncontroller\ CPU.bin \


# All Target
all: main-build

# Main-build Target
main-build: Suncontroller\ CPU.elf secondary-outputs

# Tool invocations
Suncontroller\ CPU.elf: $(OBJS) $(USER_OBJS) D:\Projekty\ElektronikaiOprogramowanie\Suncontroller\SuncontrollerSoftware\ECO_MainControllerMCU\STM32F401CCUX_FLASH.ld
	arm-none-eabi-gcc -o "Suncontroller CPU.elf" @"objects.list" $(USER_OBJS) $(LIBS) -mcpu=cortex-m4 -T"D:\Projekty\ElektronikaiOprogramowanie\Suncontroller\SuncontrollerSoftware\ECO_MainControllerMCU\STM32F401CCUX_FLASH.ld" --specs=nosys.specs -Wl,-Map="Suncontroller CPU.map" -Wl,--gc-sections -Wl,--verbose -static --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -Wl,--start-group -lc -lm -Wl,--end-group
	@echo 'Finished building target: $@'
	@echo ' '

default.size.stdout: $(EXECUTABLES)
	arm-none-eabi-size  $(EXECUTABLES)
	@echo 'Finished building: $@'
	@echo ' '

Suncontroller\ CPU.list: $(EXECUTABLES)
	arm-none-eabi-objdump -h -S $(EXECUTABLES) > "Suncontroller CPU.list"
	@echo 'Finished building: $@'
	@echo ' '

Suncontroller\ CPU.bin: $(EXECUTABLES)
	arm-none-eabi-objcopy  -O binary $(EXECUTABLES) "Suncontroller CPU.bin"
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) *
	-@echo ' '

secondary-outputs: $(SIZE_OUTPUT) $(OBJDUMP_LIST) $(OBJCOPY_BIN)

fail-specified-linker-script-missing:
	@echo 'Error: Cannot find the specified linker script. Check the linker settings in the build configuration.'
	@exit 2

warn-no-linker-script-specified:
	@echo 'Warning: No linker script specified. Check the linker settings in the build configuration.'

.PHONY: all clean dependents fail-specified-linker-script-missing warn-no-linker-script-specified
.SECONDARY:

-include ../makefile.targets
