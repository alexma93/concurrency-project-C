################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/list/TestDriver.c \
../src/list/list.c \
../src/list/listDomainAssertion.c \
../src/list/testList.c 

OBJS += \
./src/list/TestDriver.o \
./src/list/list.o \
./src/list/listDomainAssertion.o \
./src/list/testList.o 

C_DEPS += \
./src/list/TestDriver.d \
./src/list/list.d \
./src/list/listDomainAssertion.d \
./src/list/testList.d 


# Each subdirectory must supply rules for building sources it contributes
src/list/%.o: ../src/list/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


