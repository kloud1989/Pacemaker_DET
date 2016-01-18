################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
src/platform/platform_osal.obj: ../src/platform/platform_osal.c $(GEN_OPTS) $(GEN_SRCS)
	@echo 'Building file: $<'
	@echo 'Invoking: C6000 Compiler'
	"C:/Program Files/Texas Instruments/C6000 Code Generation Tools 7.4.6/bin/cl6x" -mv6600 --abi=eabi -g --include_path="C:/Program Files/Texas Instruments/C6000 Code Generation Tools 7.4.6/include" --include_path="D:/ti/fftlib_c66x_2_0_0_2/packages" --include_path="D:/ti/dsplib_c66x_3_1_0_0" --include_path="D:/ti/pdk_C6678_1_1_2_5/packages/ti/csl" --include_path="D:/ti/pdk_C6678_1_1_2_5/packages/ti/platform" --include_path="D:/ti/pdk_C6678_1_1_2_5/packages/ti/drv/qmss" --include_path="D:/ti/pdk_C6678_1_1_2_5/packages/ti/drv/cppi" --include_path="D:/CCS_PRJ/Pacemaker_DET/include" --diag_warning=225 --preproc_with_compile --preproc_dependency="src/platform/platform_osal.pp" --obj_directory="src/platform" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/platform/resourcemgr.obj: ../src/platform/resourcemgr.c $(GEN_OPTS) $(GEN_SRCS)
	@echo 'Building file: $<'
	@echo 'Invoking: C6000 Compiler'
	"C:/Program Files/Texas Instruments/C6000 Code Generation Tools 7.4.6/bin/cl6x" -mv6600 --abi=eabi -g --include_path="C:/Program Files/Texas Instruments/C6000 Code Generation Tools 7.4.6/include" --include_path="D:/ti/fftlib_c66x_2_0_0_2/packages" --include_path="D:/ti/dsplib_c66x_3_1_0_0" --include_path="D:/ti/pdk_C6678_1_1_2_5/packages/ti/csl" --include_path="D:/ti/pdk_C6678_1_1_2_5/packages/ti/platform" --include_path="D:/ti/pdk_C6678_1_1_2_5/packages/ti/drv/qmss" --include_path="D:/ti/pdk_C6678_1_1_2_5/packages/ti/drv/cppi" --include_path="D:/CCS_PRJ/Pacemaker_DET/include" --diag_warning=225 --preproc_with_compile --preproc_dependency="src/platform/resourcemgr.pp" --obj_directory="src/platform" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '


