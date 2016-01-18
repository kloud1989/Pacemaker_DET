#
_XDCBUILDCOUNT = 
ifneq (,$(findstring path,$(_USEXDCENV_)))
override XDCPATH = D:/ti/pdk_C6678_1_1_2_5/packages;D:/ti/ndk_2_21_01_38/packages;D:/ti/bios_6_33_06_50/packages;D:/ti/mcsdk_2_01_02_05/demos;D:/ti/ccsv5/ccs_base;D:/ti/dsplib_c66x_3_1_0_0/packages
override XDCROOT = D:/ti/xdctools_3_23_04_60
override XDCBUILDCFG = ./config.bld
endif
ifneq (,$(findstring args,$(_USEXDCENV_)))
override XDCARGS = 
override XDCTARGETS = 
endif
#
ifeq (0,1)
PKGPATH = D:/ti/pdk_C6678_1_1_2_5/packages;D:/ti/ndk_2_21_01_38/packages;D:/ti/bios_6_33_06_50/packages;D:/ti/mcsdk_2_01_02_05/demos;D:/ti/ccsv5/ccs_base;D:/ti/dsplib_c66x_3_1_0_0/packages;D:/ti/xdctools_3_23_04_60/packages;..
HOSTOS = Windows
endif
