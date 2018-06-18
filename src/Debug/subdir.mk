################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../Appoggio.cpp \
../Imagewatermark.cpp \
../PipeImagewatermark.cpp 

OBJS += \
./Appoggio.o \
./Imagewatermark.o \
./PipeImagewatermark.o 

CPP_DEPS += \
./Appoggio.d \
./Imagewatermark.d \
./PipeImagewatermark.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


