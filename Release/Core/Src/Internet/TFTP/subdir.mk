################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/Internet/TFTP/netutil.c \
../Core/Src/Internet/TFTP/tftp.c 

OBJS += \
./Core/Src/Internet/TFTP/netutil.o \
./Core/Src/Internet/TFTP/tftp.o 

C_DEPS += \
./Core/Src/Internet/TFTP/netutil.d \
./Core/Src/Internet/TFTP/tftp.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/Internet/TFTP/netutil.o: ../Core/Src/Internet/TFTP/netutil.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -DUSE_HAL_DRIVER -DSTM32F401xC -c -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I../Drivers/CMSIS/Include -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Core/Inc -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I"D:/Projekty/Suncontroller/Suncontroller software/Suncontroller CPU/Core/Src/Ethernet" -Os -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Core/Src/Internet/TFTP/netutil.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Core/Src/Internet/TFTP/tftp.o: ../Core/Src/Internet/TFTP/tftp.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -DUSE_HAL_DRIVER -DSTM32F401xC -c -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I../Drivers/CMSIS/Include -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Core/Inc -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I"D:/Projekty/Suncontroller/Suncontroller software/Suncontroller CPU/Core/Src/Ethernet" -Os -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Core/Src/Internet/TFTP/tftp.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

