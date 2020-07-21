################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/Modbus/Modbus.c 

OBJS += \
./Core/Src/Modbus/Modbus.o 

C_DEPS += \
./Core/Src/Modbus/Modbus.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/Modbus/Modbus.o: ../Core/Src/Modbus/Modbus.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -DUSE_HAL_DRIVER -DSTM32F401xC -c -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I../Drivers/CMSIS/Include -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Core/Inc -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I"D:/Projekty/Suncontroller/Suncontroller software/Suncontroller CPU/Core/Src/Ethernet" -Os -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Core/Src/Modbus/Modbus.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

