/*
 * Do not modify this file; it is automatically generated from the template
 * linkcmd.xdt in the ti.targets.elf package and will be overwritten.
 */

/*
 * put '"'s around paths because, without this, the linker
 * considers '-' as minus operator, not a file name character.
 */


-l"D:\CCS_PRJ\helloworld_test\Debug\configPkg\package\cfg\helloWorld_pe66.oe66"
-l"D:\ti\ndk_2_21_01_38\packages\ti\ndk\miniPrintf\lib\miniPrintf.ae66"
-l"D:\ti\ndk_2_21_01_38\packages\ti\ndk\hal\ser_stub\lib\hal_ser_stub.ae66"
-l"D:\ti\ndk_2_21_01_38\packages\ti\ndk\hal\timer_bios\lib\hal_timer_bios.ae66"
-l"D:\ti\ndk_2_21_01_38\packages\ti\ndk\os\lib\os.ae66"
-l"D:\ti\ndk_2_21_01_38\packages\ti\ndk\hal\userled_stub\lib\hal_userled_stub.ae66"
-l"D:\ti\ndk_2_21_01_38\packages\ti\ndk\hal\eth_stub\lib\hal_eth_stub.ae66"
-l"D:\ti\ndk_2_21_01_38\packages\ti\ndk\tools\cgi\lib\cgi.ae66"
-l"D:\ti\ndk_2_21_01_38\packages\ti\ndk\tools\hdlc\lib\hdlc.ae66"
-l"D:\ti\ndk_2_21_01_38\packages\ti\ndk\tools\console\lib\console_min.ae66"
-l"D:\ti\ndk_2_21_01_38\packages\ti\ndk\netctrl\lib\netctrl.ae66"
-l"D:\ti\ndk_2_21_01_38\packages\ti\ndk\nettools\lib\nettool.ae66"
-l"D:\ti\ndk_2_21_01_38\packages\ti\ndk\tools\servers\lib\servers_min.ae66"
-l"D:\ti\ndk_2_21_01_38\packages\ti\ndk\stack\lib\stk6_ppp_pppoe.ae66"
-l"D:\ti\pdk_C6678_1_1_2_5\packages\ti\transport\ndk\nimu\lib\debug\ti.transport.ndk.nimu.ae66"
-l"D:\ti\pdk_C6678_1_1_2_5\packages\ti\platform\evmc6678l\platform_lib\lib\debug\ti.platform.evm6678l.ae66"
-l"D:\ti\pdk_C6678_1_1_2_5\packages\ti\drv\pa\lib\ti.drv.pa.ae66"
-l"D:\ti\pdk_C6678_1_1_2_5\packages\ti\drv\cppi\lib\ti.drv.cppi.ae66"
-l"D:\ti\pdk_C6678_1_1_2_5\packages\ti\drv\qmss\lib\ti.drv.qmss.ae66"
-l"D:\ti\pdk_C6678_1_1_2_5\packages\ti\csl\lib\ti.csl.ae66"
-l"D:\ti\bios_6_33_06_50\packages\ti\sysbios\lib\instrumented_e66\sysbios\sysbios.lib"
-l"D:\ti\xdctools_3_23_04_60\packages\ti\targets\rts6000\lib\ti.targets.rts6000.ae66"
-l"D:\ti\xdctools_3_23_04_60\packages\ti\targets\rts6000\lib\boot.ae66"

--retain="*(xdc.meta)"


--args 0x0
-heap  0x0
-stack 0x1000

MEMORY
{
    L2SRAM (RWX) : org = 0x800000, len = 0x80000
    MSMCSRAM (RWX) : org = 0xc000000, len = 0x400000
    DDR3 : org = 0x80000000, len = 0x20000000
}

/*
 * Linker command file contributions from all loaded packages:
 */

/* Content from xdc.services.global (null): */

/* Content from xdc (null): */

/* Content from xdc.corevers (null): */

/* Content from xdc.shelf (null): */

/* Content from xdc.services.spec (null): */

/* Content from xdc.services.intern.xsr (null): */

/* Content from xdc.services.intern.gen (null): */

/* Content from xdc.services.intern.cmd (null): */

/* Content from xdc.bld (null): */

/* Content from ti.targets (null): */

/* Content from ti.targets.elf (null): */

/* Content from xdc.rov (null): */

/* Content from xdc.runtime (null): */

/* Content from ti.targets.rts6000 (null): */

/* Content from ti.sysbios.interfaces (null): */

/* Content from ti.sysbios.family (null): */

/* Content from ti.sysbios (null): */

/* Content from xdc.services.getset (null): */

/* Content from ti.sysbios.hal (null): */

/* Content from ti.sysbios.knl (null): */

/* Content from ti.csl (null): */

/* Content from ti.drv.qmss (null): */

/* Content from ti.drv.cppi (null): */

/* Content from ti.drv.pa (null): */

/* Content from ti.platform.evmc6678l (null): */

/* Content from ti.transport.ndk (null): */

/* Content from ti.ndk.rov (null): */

/* Content from ti.sysbios.family.c66 (null): */

/* Content from ti.sysbios.family.c64p (null): */

/* Content from ti.sysbios.family.c66.tci66xx (null): */

/* Content from ti.ndk.config (null): */

/* Content from xdc.runtime.knl (null): */

/* Content from ti.sysbios.gates (null): */

/* Content from ti.sysbios.heaps (null): */

/* Content from ti.sysbios.xdcruntime (null): */

/* Content from ti.sysbios.family.c62 (null): */

/* Content from ti.sysbios.timers.timer64 (null): */

/* Content from ti.sysbios.family.c64p.tci6488 (null): */

/* Content from ti.ndk.config.family (null): */

/* Content from ti.sysbios.utils (null): */

/* Content from ti.catalog.c6000 (null): */

/* Content from ti.catalog (null): */

/* Content from ti.catalog.peripherals.hdvicp2 (null): */

/* Content from xdc.platform (null): */

/* Content from xdc.cfg (null): */

/* Content from ti.platforms.evm6678 (null): */

/* Content from configPkg (null): */


/*
 * symbolic aliases for static instance objects
 */
xdc_runtime_Startup__EXECFXN__C = 1;
xdc_runtime_Startup__RESETFXN__C = 1;
TSK_idle = ti_sysbios_knl_Task_Object__table__V + 288;

SECTIONS
{
    .text: load >> DDR3
    .ti.decompress: load > L2SRAM
    .stack: load > L2SRAM
    GROUP: load > DDR3
    {
        .bss:
        .neardata:
        .rodata:
    }
    .cinit: load > DDR3
    .pinit: load >> L2SRAM
    .init_array: load > DDR3
    .const: load >> DDR3
    .data: load >> DDR3
    .fardata: load >> DDR3
    .switch: load >> DDR3
    .sysmem: load > DDR3
    .far: load >> DDR3
    .args: load > DDR3 align = 0x4, fill = 0 {_argsize = 0x0; }
    .cio: load >> DDR3
    .ti.handler_table: load > L2SRAM
    sharedL2: load >> DDR3
    systemHeap: load >> DDR3
    .cppi: load >> DDR3
    .qmss: load >> DDR3
    .code: load >> DDR3
    .vecs: load >> DDR3
    platform_lib: load >> DDR3
    .far:taskStackSection: load >> L2SRAM
    .nimu_eth_ll2: load >> L2SRAM
    .resmgr_memregion: load >> L2SRAM align = 0x80
    .resmgr_handles: load >> L2SRAM align = 0x10
    .resmgr_pa: load >> L2SRAM align = 0x8
    .far:IMAGEDATA: load >> L2SRAM align = 0x8
    .far:NDK_OBJMEM: load >> L2SRAM align = 0x8
    .far:NDK_PACKETMEM: load >> L2SRAM align = 0x80
    xdc.meta: load >> DDR3, type = COPY

}
