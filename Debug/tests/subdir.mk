################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../tests/test_charsmap.cpp \
../tests/test_filters.cpp \
../tests/tests.cpp 

OBJS += \
./tests/test_charsmap.o \
./tests/test_filters.o \
./tests/tests.o 

CPP_DEPS += \
./tests/test_charsmap.d \
./tests/test_filters.d \
./tests/tests.d 


# Each subdirectory must supply rules for building sources it contributes
tests/%.o: ../tests/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


