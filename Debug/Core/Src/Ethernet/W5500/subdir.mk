################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/Ethernet/W5500/w5500.c 

OBJS += \
./Core/Src/Ethernet/W5500/w5500.o 

C_DEPS += \
./Core/Src/Ethernet/W5500/w5500.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/Ethernet/W5500/w5500.o: ../Core/Src/Ethernet/W5500/w5500.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DUSE_HAL_DRIVER -DSTM32F401xC -DDEBUG -c -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I../Drivers/CMSIS/Include -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Core/Inc -I"D:/Projekty/ElektronikaiOprogramowanie/Suncontroller/SuncontrollerSoftware/ECO_MainControllerMCU/Core/Src/Ethernet" -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I"D:/Projekty/ElektronikaiOprogramowanie/Suncontroller/SuncontrollerSoftware/ECO_MainControllerMCU/Core/Src/Internet" -I"D:/Projekty/ElektronikaiOprogramowanie/Suncontroller/SuncontrollerSoftware/ECO_MainControllerMCU/Middlewares/Third_Party/FreeRTOS_CLI" -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I"D:/Projekty/ElektronikaiOprogramowanie/Suncontroller/SuncontrollerSoftware/ECO_MainControllerMCU/Middlewares/Third_Party/printf" -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -Og -ffunction-sections -fdata-sections -Wall -Wextra -Wswitch-default -fstack-usage -MMD -MP -MF"Core/Src/Ethernet/W5500/w5500.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

