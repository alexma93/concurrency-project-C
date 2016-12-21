################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/accepter.c \
../src/accepterTest.c \
../src/dispatcher.c \
../src/dispatcherTest.c \
../src/main.c \
../src/poison_pill.c \
../src/provider.c \
../src/providerTest.c \
../src/reader.c \
../src/readerTest.c \
../src/test.c 

OBJS += \
./src/accepter.o \
./src/accepterTest.o \
./src/dispatcher.o \
./src/dispatcherTest.o \
./src/main.o \
./src/poison_pill.o \
./src/provider.o \
./src/providerTest.o \
./src/reader.o \
./src/readerTest.o \
./src/test.o 

C_DEPS += \
./src/accepter.d \
./src/accepterTest.d \
./src/dispatcher.d \
./src/dispatcherTest.d \
./src/main.d \
./src/poison_pill.d \
./src/provider.d \
./src/providerTest.d \
./src/reader.d \
./src/readerTest.d \
./src/test.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


