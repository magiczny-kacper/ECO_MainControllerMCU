################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/Flash/dataLog.c \
../Core/Src/Flash/w25qxx.c 

OBJS += \
./Core/Src/Flash/dataLog.o \
./Core/Src/Flash/w25qxx.o 

C_DEPS += \
./Core/Src/Flash/dataLog.d \
./Core/Src/Flash/w25qxx.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/Flash/dataLog.o: ../Core/Src/Flash/dataLog.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DUSE_HAL_DRIVER -DSTM32F401xC -DDEBUG -c -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I../Drivers/CMSIS/Include -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Core/Inc -I"/mnt/Data/Projekty/Suncontroller/Suncontroller software/ECO_MainControllerMCU/Core/Src/Ethernet" -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I"/mnt/Data/Projekty/Suncontroller/Suncontroller software/ECO_MainControllerMCU/Core/Src/Internet" -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS -I"/mnt/Data/Projekty/Suncontroller/Suncontroller software/ECO_MainControllerMCU/Middlewares/Third_Party/FreeRTOS_CLI" -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I"/mnt/Data/Projekty/Suncontroller/Suncontroller software/ECO_MainControllerMCU/Middlewares/Third_Party/printf" -Og -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Core/Src/Flash/dataLog.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Core/Src/Flash/w25qxx.o: ../Core/Src/Flash/w25qxx.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DUSE_HAL_DRIVER -DSTM32F401xC -DDEBUG -c -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I../Drivers/CMSIS/Include -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Core/Inc -I"/mnt/Data/Projekty/Suncontroller/Suncontroller software/ECO_MainControllerMCU/Core/Src/Ethernet" -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I"/mnt/Data/Projekty/Suncontroller/Suncontroller software/ECO_MainControllerMCU/Core/Src/Internet" -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS -I"/mnt/Data/Projekty/Suncontroller/Suncontroller software/ECO_MainControllerMCU/Middlewares/Third_Party/FreeRTOS_CLI" -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I"/mnt/Data/Projekty/Suncontroller/Suncontroller software/ECO_MainControllerMCU/Middlewares/Third_Party/printf" -Og -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Core/Src/Flash/w25qxx.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

