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
-include Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS/subdir.mk
-include Middlewares/Third_Party/FreeRTOS/Source/subdir.mk
-include Drivers/STM32F4xx_HAL_Driver/Src/subdir.mk
-include Core/Startup/subdir.mk
-include Core/Src/nRF24/subdir.mk
-include Core/Src/Tasks/RegulationTask/subdir.mk
-include Core/Src/Tasks/RadioTask/subdir.mk
-include Core/Src/Tasks/EthernetTask/subdir.mk
-include Core/Src/Tasks/DisplayTask/subdir.mk
-include Core/Src/Tasks/DataLogTask/subdir.mk
-include Core/Src/RuntimeStats/subdir.mk
-include Core/Src/RingBuffer/subdir.mk
-include Core/Src/Modbus/subdir.mk
-include Core/Src/Flash/subdir.mk
-include Core/Src/Expander/subdir.mk
-include Core/Src/Ethernet/W5500/subdir.mk
-include Core/Src/Ethernet/subdir.mk
-include Core/Src/ConfigEEPROM/subdir.mk
-include Core/Src/CLI/subdir.mk
-include Core/Src/AnalogOut/subdir.mk
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
all: Suncontroller\ CPU.elf secondary-outputs

# Tool invocations
Suncontroller\ CPU.elf: $(OBJS) $(USER_OBJS) /mnt/Data/Projekty/Suncontroller/Suncontroller\ software/ECO_MainControllerMCU/STM32F401CCUX_FLASH.ld
	arm-none-eabi-gcc -o "Suncontroller CPU.elf" @"objects.list" $(USER_OBJS) $(LIBS) -mcpu=cortex-m4 -T"/mnt/Data/Projekty/Suncontroller/Suncontroller software/ECO_MainControllerMCU/STM32F401CCUX_FLASH.ld" --specs=nosys.specs -Wl,-Map="Suncontroller CPU.map" -Wl,--gc-sections -static --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -Wl,--start-group -lc -lm -Wl,--end-group
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

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
