################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Src/CANProcess.c \
../Src/UserTasks.c \
../Src/freertos.c \
../Src/main.c \
../Src/stm32f4xx_hal_msp.c \
../Src/stm32f4xx_hal_timebase_TIM.c \
../Src/stm32f4xx_it.c 

OBJS += \
./Src/CANProcess.o \
./Src/UserTasks.o \
./Src/freertos.o \
./Src/main.o \
./Src/stm32f4xx_hal_msp.o \
./Src/stm32f4xx_hal_timebase_TIM.o \
./Src/stm32f4xx_it.o 

C_DEPS += \
./Src/CANProcess.d \
./Src/UserTasks.d \
./Src/freertos.d \
./Src/main.d \
./Src/stm32f4xx_hal_msp.d \
./Src/stm32f4xx_hal_timebase_TIM.d \
./Src/stm32f4xx_it.d 


# Each subdirectory must supply rules for building sources it contributes
Src/%.o: ../Src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo %cd%
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -D__weak="__attribute__((weak))" -D__packed="__attribute__((__packed__))" -DUSE_HAL_DRIVER -DSTM32F407xx -I"C:/Users/Kai Strubel/Documents/Extra Curricular/Purdue Electric Racing/STM32Mx/Pedal Box/Inc" -I"C:/Users/Kai Strubel/Documents/Extra Curricular/Purdue Electric Racing/STM32Mx/Pedal Box/Drivers/STM32F4xx_HAL_Driver/Inc" -I"C:/Users/Kai Strubel/Documents/Extra Curricular/Purdue Electric Racing/STM32Mx/Pedal Box/Drivers/STM32F4xx_HAL_Driver/Inc/Legacy" -I"C:/Users/Kai Strubel/Documents/Extra Curricular/Purdue Electric Racing/STM32Mx/Pedal Box/Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F" -I"C:/Users/Kai Strubel/Documents/Extra Curricular/Purdue Electric Racing/STM32Mx/Pedal Box/Drivers/CMSIS/Device/ST/STM32F4xx/Include" -I"C:/Users/Kai Strubel/Documents/Extra Curricular/Purdue Electric Racing/STM32Mx/Pedal Box/Middlewares/Third_Party/FreeRTOS/Source/include" -I"C:/Users/Kai Strubel/Documents/Extra Curricular/Purdue Electric Racing/STM32Mx/Pedal Box/Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS" -I"C:/Users/Kai Strubel/Documents/Extra Curricular/Purdue Electric Racing/STM32Mx/Pedal Box/Drivers/CMSIS/Include"  -Os -g3 -Wall -fmessage-length=0 -ffunction-sections -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


