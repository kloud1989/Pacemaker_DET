################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

CG_TOOL_ROOT := C:/Program Files/Texas Instruments/C6000 Code Generation Tools 7.4.6

ORDERED_OBJS += \
$(GEN_CMDS__FLAG) \
"./src/resourcemgr.obj" \
"./src/platform_osal.obj" \
"./src/main.obj" \
"./src/Utils.obj" \
"./src/Thread_NetworkInit.obj" \
"./src/Thread_ADS1298Init.obj" \
"./src/Services.obj" \
"./src/SPI_Intc.obj" \
"./src/KeyStone_common.obj" \
"./src/KeyStone_UART_Init_drv.obj" \
"./src/KeyStone_SPI_Init_drv.obj" \
"./src/KeyStone_DDR_Init.obj" \
"./src/GPIO_drv.obj" \
"./src/ADS1298_drv.obj" \
-l"libc.a" \
-l"D:\ti\pdk_C6678_1_1_2_5\packages\ti\platform\evmc6678l\platform_lib\lib\release\ti.platform.evm6678l.ae66" \
-l"D:\ti\bios_6_33_06_50\packages\ti\sysbios\family\c66\tci66xx\lib\debug\ti.sysbios.family.c66.tci66xx.ae66" \
-l"D:\ti\pdk_C6678_1_1_2_5\packages\ti\csl\lib\ti.csl.ae66" \
-l"D:\ti\pdk_C6678_1_1_2_5\packages\ti\csl\lib\ti.csl.intc.ae66" \

-include ../makefile.init

RM := DEL /F
RMDIR := RMDIR /S/Q

# All of the sources participating in the build are defined here
-include sources.mk
-include subdir_vars.mk
-include src/subdir_vars.mk
-include subdir_rules.mk
-include src/subdir_rules.mk
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
ifneq ($(strip $(S55_DEPS)),)
-include $(S55_DEPS)
endif
ifneq ($(strip $(C67_DEPS)),)
-include $(C67_DEPS)
endif
ifneq ($(strip $(C??_DEPS)),)
-include $(C??_DEPS)
endif
ifneq ($(strip $(CLA_DEPS)),)
-include $(CLA_DEPS)
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

# All Target
all: Pacemaker_DET.out

# Tool invocations
Pacemaker_DET.out: $(OBJS) $(GEN_CMDS)
	@echo 'Building target: $@'
	@echo 'Invoking: C6000 Linker'
	"C:/Program Files/Texas Instruments/C6000 Code Generation Tools 7.4.6/bin/cl6x" -mv6600 --abi=eabi -g --diag_warning=225 -z -m"Pacemaker_DET.map" -i"C:/Program Files/Texas Instruments/C6000 Code Generation Tools 7.4.6/lib" -i"C:/Program Files/Texas Instruments/C6000 Code Generation Tools 7.4.6/include" --reread_libs --warn_sections --rom_model -o "Pacemaker_DET.out" $(ORDERED_OBJS)
	@echo 'Finished building target: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(GEN_SRCS__QUOTED)$(GEN_MISC_FILES__QUOTED)$(C6000_EXECUTABLE_OUTPUTS__QUOTED)$(GEN_OPTS__QUOTED)$(GEN_CMDS__QUOTED) "Pacemaker_DET.out"
	-$(RMDIR) $(GEN_MISC_DIRS__QUOTED)
	-$(RM) "src\ADS1298_drv.pp" "src\GPIO_drv.pp" "src\KeyStone_DDR_Init.pp" "src\KeyStone_SPI_Init_drv.pp" "src\KeyStone_UART_Init_drv.pp" "src\KeyStone_common.pp" "src\SPI_Intc.pp" "src\Services.pp" "src\Thread_ADS1298Init.pp" "src\Thread_NetworkInit.pp" "src\Utils.pp" "src\main.pp" "src\platform_osal.pp" "src\resourcemgr.pp" 
	-$(RM) "src\ADS1298_drv.obj" "src\GPIO_drv.obj" "src\KeyStone_DDR_Init.obj" "src\KeyStone_SPI_Init_drv.obj" "src\KeyStone_UART_Init_drv.obj" "src\KeyStone_common.obj" "src\SPI_Intc.obj" "src\Services.obj" "src\Thread_ADS1298Init.obj" "src\Thread_NetworkInit.obj" "src\Utils.obj" "src\main.obj" "src\platform_osal.obj" "src\resourcemgr.obj" 
	-@echo 'Finished clean'
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
