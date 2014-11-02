################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/OpenCL-Common/common.cpp \
../src/OpenCL-Common/image.cpp 

OBJS += \
./src/OpenCL-Common/common.o \
./src/OpenCL-Common/image.o 

CPP_DEPS += \
./src/OpenCL-Common/common.d \
./src/OpenCL-Common/image.d 


# Each subdirectory must supply rules for building sources it contributes
src/OpenCL-Common/%.o: ../src/OpenCL-Common/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I/usr/local/include/opencv2 -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


