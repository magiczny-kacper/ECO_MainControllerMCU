################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/Tasks/DataLogTask/DataLogTask.c 

OBJS += \
./Core/Src/Tasks/DataLogTask/DataLogTask.o 

C_DEPS += \
./Core/Src/Tasks/DataLogTask/DataLogTask.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/Tasks/DataLogTask/DataLogTask.o: ../Core/Src/Tasks/DataLogTask/DataLogTask.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DUSE_HAL_DRIVER -DSTM32F401xC -DDEBUG -c -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I../Drivers/CMSIS/Include -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Core/Inc -I"D:/Projekty/Suncontroller/Suncontroller software/Suncontroller CPU/Core/Src/Ethernet" -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I"D:/Projekty/Suncontroller/Suncontroller software/Suncontroller CPU/Core/Src/Internet" -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -Og -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Core/Src/Tasks/DataLogTask/DataLogTask.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

