################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Middlewares/Third_Party/FreeRTOS_CLI/FreeRTOS_CLI.c 

OBJS += \
./Middlewares/Third_Party/FreeRTOS_CLI/FreeRTOS_CLI.o 

C_DEPS += \
./Middlewares/Third_Party/FreeRTOS_CLI/FreeRTOS_CLI.d 


# Each subdirectory must supply rules for building sources it contributes
Middlewares/Third_Party/FreeRTOS_CLI/FreeRTOS_CLI.o: ../Middlewares/Third_Party/FreeRTOS_CLI/FreeRTOS_CLI.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DUSE_HAL_DRIVER -DSTM32F401xC -DDEBUG -c -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I../Drivers/CMSIS/Include -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Core/Inc -I"D:/Projekty/Suncontroller/Suncontroller software/ECO_MainControllerMCU/Core/Src/Ethernet" -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I"D:/Projekty/Suncontroller/Suncontroller software/ECO_MainControllerMCU/Core/Src/Internet" -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS -I"D:/Projekty/Suncontroller/Suncontroller software/ECO_MainControllerMCU/Middlewares/Third_Party/FreeRTOS_CLI" -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -Og -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Middlewares/Third_Party/FreeRTOS_CLI/FreeRTOS_CLI.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

