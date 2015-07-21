################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include src/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: mp3lib-test

dependents:
	-cd /home/fernan/projects/mp3lib/mp3lib.eclipse/Release && $(MAKE) clean all

# Tool invocations
mp3lib-test: $(OBJS) $(USER_OBJS) /home/fernan/projects/mp3lib/mp3lib.eclipse/Release/libmp3.so
	@echo 'Building target: $@'
	@echo 'Invoking: GCC C Linker'
	gcc -L"/home/fernan/projects/mp3lib/mp3lib.eclipse/Release" -o "mp3lib-test" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(EXECUTABLES)$(OBJS)$(C_DEPS) mp3lib-test
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:
/home/fernan/projects/mp3lib/mp3lib.eclipse/Release/libmp3.so:

-include ../makefile.targets
