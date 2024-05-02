################################################################################
# Automatically-generated file. Do not edit!
################################################################################

CG_TOOL_ROOT := /home/tessin/ti/ccsv6/tools/compiler/gcc-arm-none-eabi-4_9-2015q3

GEN_OPTS__FLAG := 
GEN_CMDS__FLAG := 

ORDERED_OBJS += \
"./tm4c123gh6pm_startup_ccs_gcc.o" \
"./uartstdio.o" \
"./Source/App/AppMain.o" \
"./Source/Devices/ADC.o" \
"./Source/Devices/Clock.o" \
"./Source/Devices/DigitalInputPort.o" \
"./Source/Devices/DigitalOutputPort.o" \
"./Source/Devices/KeyBoard.o" \
"./Source/Devices/LED.o" \
"./Source/Devices/MatrixKBD.o" \
"./Source/Devices/PWM.o" \
"./Source/Devices/SysTimer.o" \
"./Source/Devices/Timer.o" \
"./Source/Devices/UART.o" \
"./Source/Genlib/Command.o" \
"./Source/Genlib/GenLib.o" \
"./Source/Genlib/HeapMem.o" \
"./Source/Genlib/MemOps.o" \
-Wl,-T"../tm4c123gh6pm.lds" \
$(GEN_CMDS__FLAG) \
-Wl,--start-group \
-lc \
-Wl,--end-group \

-include ../makefile.init

RM := rm -rf
RMDIR := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include subdir_vars.mk
-include Source/App/subdir_vars.mk
-include Source/Devices/subdir_vars.mk
-include Source/Genlib/subdir_vars.mk
-include subdir_rules.mk
-include Source/App/subdir_rules.mk
-include Source/Devices/subdir_rules.mk
-include Source/Genlib/subdir_rules.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(S62_DEPS)),)
-include $(S62_DEPS)
endif
ifneq ($(strip $(C64_DEPS)),)
-include $(C64_DEPS)
endif
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(CC_DEPS)),)
-include $(CC_DEPS)
endif
ifneq ($(strip $(SV7A_DEPS)),)
-include $(SV7A_DEPS)
endif
ifneq ($(strip $(S55_DEPS)),)
-include $(S55_DEPS)
endif
ifneq ($(strip $(C67_DEPS)),)
-include $(C67_DEPS)
endif
ifneq ($(strip $(CLA_DEPS)),)
-include $(CLA_DEPS)
endif
ifneq ($(strip $(C??_DEPS)),)
-include $(C??_DEPS)
endif
ifneq ($(strip $(CPP_DEPS)),)
-include $(CPP_DEPS)
endif
ifneq ($(strip $(S??_DEPS)),)
-include $(S??_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(C62_DEPS)),)
-include $(C62_DEPS)
endif
ifneq ($(strip $(CXX_DEPS)),)
-include $(CXX_DEPS)
endif
ifneq ($(strip $(C++_DEPS)),)
-include $(C++_DEPS)
endif
ifneq ($(strip $(ASM_UPPER_DEPS)),)
-include $(ASM_UPPER_DEPS)
endif
ifneq ($(strip $(K_DEPS)),)
-include $(K_DEPS)
endif
ifneq ($(strip $(C43_DEPS)),)
-include $(C43_DEPS)
endif
ifneq ($(strip $(INO_DEPS)),)
-include $(INO_DEPS)
endif
ifneq ($(strip $(S67_DEPS)),)
-include $(S67_DEPS)
endif
ifneq ($(strip $(SA_DEPS)),)
-include $(SA_DEPS)
endif
ifneq ($(strip $(S43_DEPS)),)
-include $(S43_DEPS)
endif
ifneq ($(strip $(OPT_DEPS)),)
-include $(OPT_DEPS)
endif
ifneq ($(strip $(PDE_DEPS)),)
-include $(PDE_DEPS)
endif
ifneq ($(strip $(S64_DEPS)),)
-include $(S64_DEPS)
endif
ifneq ($(strip $(C_UPPER_DEPS)),)
-include $(C_UPPER_DEPS)
endif
ifneq ($(strip $(C55_DEPS)),)
-include $(C55_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
EXE_OUTPUTS += \
ECSDLabProject.out \

EXE_OUTPUTS__QUOTED += \
"ECSDLabProject.out" \

BIN_OUTPUTS += \
ECSDLabProject.hex \

BIN_OUTPUTS__QUOTED += \
"ECSDLabProject.hex" \


# All Target
all: ECSDLabProject.out

# Tool invocations
ECSDLabProject.out: $(OBJS) $(LDS_SRCS) $(A_SRCS) $(GEN_CMDS)
	@echo 'Building target: $@'
	@echo 'Invoking: GNU Linker'
	"/home/tessin/ti/ccsv6/tools/compiler/gcc-arm-none-eabi-4_9-2015q3/bin/arm-none-eabi-gcc" -mcpu=cortex-m4 -march=armv7e-m -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -DPART_TM4C123GH6PM -ffunction-sections -fdata-sections -g -gdwarf-3 -gstrict-dwarf -Wall -specs="nosys.specs" -Wl,-Map,"ECSDLabProject.map" -o"ECSDLabProject.out" $(ORDERED_OBJS) $(A_SRCS)
	@echo 'Finished building target: $@'
	@echo ' '

ECSDLabProject.hex: $(EXE_OUTPUTS)
	@echo 'Invoking: GNU Objcopy Utility'
	"/home/tessin/ti/ccsv6/tools/compiler/gcc-arm-none-eabi-4_9-2015q3/bin/arm-none-eabi-objcopy"  $(EXE_OUTPUTS__QUOTED) "ECSDLabProject.hex"
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(EXE_OUTPUTS__QUOTED)$(BIN_OUTPUTS__QUOTED)
	-$(RM) "tm4c123gh6pm_startup_ccs_gcc.d" "uartstdio.d" "Source/App/AppMain.d" "Source/Devices/ADC.d" "Source/Devices/Clock.d" "Source/Devices/DigitalInputPort.d" "Source/Devices/DigitalOutputPort.d" "Source/Devices/KeyBoard.d" "Source/Devices/LED.d" "Source/Devices/MatrixKBD.d" "Source/Devices/PWM.d" "Source/Devices/SysTimer.d" "Source/Devices/Timer.d" "Source/Devices/UART.d" "Source/Genlib/Command.d" "Source/Genlib/GenLib.d" "Source/Genlib/HeapMem.d" "Source/Genlib/MemOps.d" 
	-$(RM) "tm4c123gh6pm_startup_ccs_gcc.o" "uartstdio.o" "Source/App/AppMain.o" "Source/Devices/ADC.o" "Source/Devices/Clock.o" "Source/Devices/DigitalInputPort.o" "Source/Devices/DigitalOutputPort.o" "Source/Devices/KeyBoard.o" "Source/Devices/LED.o" "Source/Devices/MatrixKBD.o" "Source/Devices/PWM.o" "Source/Devices/SysTimer.o" "Source/Devices/Timer.o" "Source/Devices/UART.o" "Source/Genlib/Command.o" "Source/Genlib/GenLib.o" "Source/Genlib/HeapMem.o" "Source/Genlib/MemOps.o" 
	-@echo 'Finished clean'
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets

