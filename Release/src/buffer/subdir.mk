################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/buffer/buffer.c \
../src/buffer/bufferTest.c \
../src/buffer/msg.c 

OBJS += \
./src/buffer/buffer.o \
./src/buffer/bufferTest.o \
./src/buffer/msg.o 

C_DEPS += \
./src/buffer/buffer.d \
./src/buffer/bufferTest.d \
./src/buffer/msg.d 


# Each subdirectory must supply rules for building sources it contributes
src/buffer/%.o: ../src/buffer/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


