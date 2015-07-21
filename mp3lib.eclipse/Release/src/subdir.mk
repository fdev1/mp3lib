################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
/home/fernan/projects/mp3lib/mp3.c \
/home/fernan/projects/mp3lib/mpeg_2.c \
/home/fernan/projects/mp3lib/mpeg_2__1.c \
/home/fernan/projects/mp3lib/mpeg_2__2.c \
/home/fernan/projects/mp3lib/mpeg_2__3.c \
/home/fernan/projects/mp3lib/mpeg_2__3_huff.c \
/home/fernan/projects/mp3lib/mpeg_2__3_tbl.c \
/home/fernan/projects/mp3lib/mpeg_2_fractional.c \
/home/fernan/projects/mp3lib/mpeg_2_streamreader.c \
/home/fernan/projects/mp3lib/mpeg_2_synth.c 

OBJS += \
./src/mp3.o \
./src/mpeg_2.o \
./src/mpeg_2__1.o \
./src/mpeg_2__2.o \
./src/mpeg_2__3.o \
./src/mpeg_2__3_huff.o \
./src/mpeg_2__3_tbl.o \
./src/mpeg_2_fractional.o \
./src/mpeg_2_streamreader.o \
./src/mpeg_2_synth.o 

C_DEPS += \
./src/mp3.d \
./src/mpeg_2.d \
./src/mpeg_2__1.d \
./src/mpeg_2__2.d \
./src/mpeg_2__3.d \
./src/mpeg_2__3_huff.d \
./src/mpeg_2__3_tbl.d \
./src/mpeg_2_fractional.d \
./src/mpeg_2_streamreader.d \
./src/mpeg_2_synth.d 


# Each subdirectory must supply rules for building sources it contributes
src/mp3.o: /home/fernan/projects/mp3lib/mp3.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O3 -Wall -c -fmessage-length=0 -fPIC -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/mpeg_2.o: /home/fernan/projects/mp3lib/mpeg_2.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O3 -Wall -c -fmessage-length=0 -fPIC -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/mpeg_2__1.o: /home/fernan/projects/mp3lib/mpeg_2__1.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O3 -Wall -c -fmessage-length=0 -fPIC -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/mpeg_2__2.o: /home/fernan/projects/mp3lib/mpeg_2__2.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O3 -Wall -c -fmessage-length=0 -fPIC -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/mpeg_2__3.o: /home/fernan/projects/mp3lib/mpeg_2__3.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O3 -Wall -c -fmessage-length=0 -fPIC -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/mpeg_2__3_huff.o: /home/fernan/projects/mp3lib/mpeg_2__3_huff.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O3 -Wall -c -fmessage-length=0 -fPIC -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/mpeg_2__3_tbl.o: /home/fernan/projects/mp3lib/mpeg_2__3_tbl.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O3 -Wall -c -fmessage-length=0 -fPIC -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/mpeg_2_fractional.o: /home/fernan/projects/mp3lib/mpeg_2_fractional.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O3 -Wall -c -fmessage-length=0 -fPIC -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/mpeg_2_streamreader.o: /home/fernan/projects/mp3lib/mpeg_2_streamreader.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O3 -Wall -c -fmessage-length=0 -fPIC -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/mpeg_2_synth.o: /home/fernan/projects/mp3lib/mpeg_2_synth.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O3 -Wall -c -fmessage-length=0 -fPIC -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


