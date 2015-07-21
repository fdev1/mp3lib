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
all: mp3lib_test.eclipse

dependents:
	-cd /home/fernan/mp3lib/mp3lib.eclipse/Debug && $(MAKE) all

# Tool invocations
mp3lib_test.eclipse: $(OBJS) $(USER_OBJS) /home/fernan/mp3lib/mp3lib.eclipse/Debug/libmp3lib.eclipse.a
	@echo 'Building target: $@'
	@echo 'Invoking: GCC C Linker'
	gcc -L"/home/fernan/mp3lib/mp3lib.eclipse/Debug" -o "mp3lib_test.eclipse" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(EXECUTABLES)$(OBJS)$(C_DEPS) mp3lib_test.eclipse
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:
/home/fernan/mp3lib/mp3lib.eclipse/Debug/libmp3lib.eclipse.a:

-include ../makefile.targets
