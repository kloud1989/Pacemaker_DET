################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
configPkg/compiler.opt: ../Pacemaker_DET.cfg
	@echo 'Building file: $<'
	@echo 'Invoking: XDCtools'
	"D:/ti/xdctools_3_23_04_60/xs" --xdcpath="D:/ti/pdk_C6678_1_1_2_5/packages;D:/ti/ndk_2_21_01_38/packages;D:/ti/bios_6_33_06_50/packages;D:/ti/mcsdk_2_01_02_05/demos;D:/ti/ccsv5/ccs_base;D:/ti/dsplib_c66x_3_1_0_0/packages;" xdc.tools.configuro -o configPkg -t ti.targets.elf.C66 -p ti.platforms.evm6678 -r debug -c "C:/Program Files/Texas Instruments/C6000 Code Generation Tools 7.4.6" "$<"
	@echo 'Finished building: $<'
	@echo ' '

configPkg/linker.cmd: configPkg/compiler.opt
configPkg/: configPkg/compiler.opt


