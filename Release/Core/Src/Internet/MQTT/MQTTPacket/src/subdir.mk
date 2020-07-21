################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/Internet/MQTT/MQTTPacket/src/MQTTConnectClient.c \
../Core/Src/Internet/MQTT/MQTTPacket/src/MQTTConnectServer.c \
../Core/Src/Internet/MQTT/MQTTPacket/src/MQTTDeserializePublish.c \
../Core/Src/Internet/MQTT/MQTTPacket/src/MQTTFormat.c \
../Core/Src/Internet/MQTT/MQTTPacket/src/MQTTPacket.c \
../Core/Src/Internet/MQTT/MQTTPacket/src/MQTTSerializePublish.c \
../Core/Src/Internet/MQTT/MQTTPacket/src/MQTTSubscribeClient.c \
../Core/Src/Internet/MQTT/MQTTPacket/src/MQTTSubscribeServer.c \
../Core/Src/Internet/MQTT/MQTTPacket/src/MQTTUnsubscribeClient.c \
../Core/Src/Internet/MQTT/MQTTPacket/src/MQTTUnsubscribeServer.c 

OBJS += \
./Core/Src/Internet/MQTT/MQTTPacket/src/MQTTConnectClient.o \
./Core/Src/Internet/MQTT/MQTTPacket/src/MQTTConnectServer.o \
./Core/Src/Internet/MQTT/MQTTPacket/src/MQTTDeserializePublish.o \
./Core/Src/Internet/MQTT/MQTTPacket/src/MQTTFormat.o \
./Core/Src/Internet/MQTT/MQTTPacket/src/MQTTPacket.o \
./Core/Src/Internet/MQTT/MQTTPacket/src/MQTTSerializePublish.o \
./Core/Src/Internet/MQTT/MQTTPacket/src/MQTTSubscribeClient.o \
./Core/Src/Internet/MQTT/MQTTPacket/src/MQTTSubscribeServer.o \
./Core/Src/Internet/MQTT/MQTTPacket/src/MQTTUnsubscribeClient.o \
./Core/Src/Internet/MQTT/MQTTPacket/src/MQTTUnsubscribeServer.o 

C_DEPS += \
./Core/Src/Internet/MQTT/MQTTPacket/src/MQTTConnectClient.d \
./Core/Src/Internet/MQTT/MQTTPacket/src/MQTTConnectServer.d \
./Core/Src/Internet/MQTT/MQTTPacket/src/MQTTDeserializePublish.d \
./Core/Src/Internet/MQTT/MQTTPacket/src/MQTTFormat.d \
./Core/Src/Internet/MQTT/MQTTPacket/src/MQTTPacket.d \
./Core/Src/Internet/MQTT/MQTTPacket/src/MQTTSerializePublish.d \
./Core/Src/Internet/MQTT/MQTTPacket/src/MQTTSubscribeClient.d \
./Core/Src/Internet/MQTT/MQTTPacket/src/MQTTSubscribeServer.d \
./Core/Src/Internet/MQTT/MQTTPacket/src/MQTTUnsubscribeClient.d \
./Core/Src/Internet/MQTT/MQTTPacket/src/MQTTUnsubscribeServer.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/Internet/MQTT/MQTTPacket/src/MQTTConnectClient.o: ../Core/Src/Internet/MQTT/MQTTPacket/src/MQTTConnectClient.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -DUSE_HAL_DRIVER -DSTM32F401xC -c -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I../Drivers/CMSIS/Include -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Core/Inc -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I"D:/Projekty/Suncontroller/Suncontroller software/Suncontroller CPU/Core/Src/Ethernet" -Os -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Core/Src/Internet/MQTT/MQTTPacket/src/MQTTConnectClient.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Core/Src/Internet/MQTT/MQTTPacket/src/MQTTConnectServer.o: ../Core/Src/Internet/MQTT/MQTTPacket/src/MQTTConnectServer.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -DUSE_HAL_DRIVER -DSTM32F401xC -c -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I../Drivers/CMSIS/Include -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Core/Inc -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I"D:/Projekty/Suncontroller/Suncontroller software/Suncontroller CPU/Core/Src/Ethernet" -Os -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Core/Src/Internet/MQTT/MQTTPacket/src/MQTTConnectServer.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Core/Src/Internet/MQTT/MQTTPacket/src/MQTTDeserializePublish.o: ../Core/Src/Internet/MQTT/MQTTPacket/src/MQTTDeserializePublish.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -DUSE_HAL_DRIVER -DSTM32F401xC -c -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I../Drivers/CMSIS/Include -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Core/Inc -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I"D:/Projekty/Suncontroller/Suncontroller software/Suncontroller CPU/Core/Src/Ethernet" -Os -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Core/Src/Internet/MQTT/MQTTPacket/src/MQTTDeserializePublish.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Core/Src/Internet/MQTT/MQTTPacket/src/MQTTFormat.o: ../Core/Src/Internet/MQTT/MQTTPacket/src/MQTTFormat.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -DUSE_HAL_DRIVER -DSTM32F401xC -c -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I../Drivers/CMSIS/Include -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Core/Inc -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I"D:/Projekty/Suncontroller/Suncontroller software/Suncontroller CPU/Core/Src/Ethernet" -Os -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Core/Src/Internet/MQTT/MQTTPacket/src/MQTTFormat.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Core/Src/Internet/MQTT/MQTTPacket/src/MQTTPacket.o: ../Core/Src/Internet/MQTT/MQTTPacket/src/MQTTPacket.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -DUSE_HAL_DRIVER -DSTM32F401xC -c -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I../Drivers/CMSIS/Include -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Core/Inc -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I"D:/Projekty/Suncontroller/Suncontroller software/Suncontroller CPU/Core/Src/Ethernet" -Os -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Core/Src/Internet/MQTT/MQTTPacket/src/MQTTPacket.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Core/Src/Internet/MQTT/MQTTPacket/src/MQTTSerializePublish.o: ../Core/Src/Internet/MQTT/MQTTPacket/src/MQTTSerializePublish.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -DUSE_HAL_DRIVER -DSTM32F401xC -c -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I../Drivers/CMSIS/Include -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Core/Inc -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I"D:/Projekty/Suncontroller/Suncontroller software/Suncontroller CPU/Core/Src/Ethernet" -Os -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Core/Src/Internet/MQTT/MQTTPacket/src/MQTTSerializePublish.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Core/Src/Internet/MQTT/MQTTPacket/src/MQTTSubscribeClient.o: ../Core/Src/Internet/MQTT/MQTTPacket/src/MQTTSubscribeClient.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -DUSE_HAL_DRIVER -DSTM32F401xC -c -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I../Drivers/CMSIS/Include -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Core/Inc -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I"D:/Projekty/Suncontroller/Suncontroller software/Suncontroller CPU/Core/Src/Ethernet" -Os -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Core/Src/Internet/MQTT/MQTTPacket/src/MQTTSubscribeClient.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Core/Src/Internet/MQTT/MQTTPacket/src/MQTTSubscribeServer.o: ../Core/Src/Internet/MQTT/MQTTPacket/src/MQTTSubscribeServer.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -DUSE_HAL_DRIVER -DSTM32F401xC -c -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I../Drivers/CMSIS/Include -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Core/Inc -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I"D:/Projekty/Suncontroller/Suncontroller software/Suncontroller CPU/Core/Src/Ethernet" -Os -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Core/Src/Internet/MQTT/MQTTPacket/src/MQTTSubscribeServer.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Core/Src/Internet/MQTT/MQTTPacket/src/MQTTUnsubscribeClient.o: ../Core/Src/Internet/MQTT/MQTTPacket/src/MQTTUnsubscribeClient.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -DUSE_HAL_DRIVER -DSTM32F401xC -c -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I../Drivers/CMSIS/Include -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Core/Inc -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I"D:/Projekty/Suncontroller/Suncontroller software/Suncontroller CPU/Core/Src/Ethernet" -Os -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Core/Src/Internet/MQTT/MQTTPacket/src/MQTTUnsubscribeClient.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Core/Src/Internet/MQTT/MQTTPacket/src/MQTTUnsubscribeServer.o: ../Core/Src/Internet/MQTT/MQTTPacket/src/MQTTUnsubscribeServer.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -DUSE_HAL_DRIVER -DSTM32F401xC -c -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I../Drivers/CMSIS/Include -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Core/Inc -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I"D:/Projekty/Suncontroller/Suncontroller software/Suncontroller CPU/Core/Src/Ethernet" -Os -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Core/Src/Internet/MQTT/MQTTPacket/src/MQTTUnsubscribeServer.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

