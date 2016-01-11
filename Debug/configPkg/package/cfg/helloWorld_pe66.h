/*
 *  Do not modify this file; it is automatically 
 *  generated and any modifications will be overwritten.
 *
 * @(#) xdc-y30
 */

#include <xdc/std.h>

#include <ti/sysbios/heaps/HeapMem.h>
extern const ti_sysbios_heaps_HeapMem_Handle heap0;

#include <ti/sysbios/knl/Task.h>
extern const ti_sysbios_knl_Task_Handle tskNetworkInit;

#include <ti/sysbios/knl/Mailbox.h>
extern const ti_sysbios_knl_Mailbox_Handle mailbox1298;

#include <ti/sysbios/knl/Semaphore.h>
extern const ti_sysbios_knl_Semaphore_Handle semDRDY;

#include <ti/sysbios/knl/Task.h>
extern const ti_sysbios_knl_Task_Handle tskMailData;

#include <ti/sysbios/hal/Timer.h>
extern const ti_sysbios_hal_Timer_Handle timer200ms;

#include <ti/sysbios/knl/Task.h>
extern const ti_sysbios_knl_Task_Handle tskEthernetSend;

#include <ti/sysbios/knl/Event.h>
extern const ti_sysbios_knl_Event_Handle eventServer;

extern int xdc_runtime_Startup__EXECFXN__C;

extern int xdc_runtime_Startup__RESETFXN__C;

#ifndef ti_sysbios_knl_Task__include
#ifndef __nested__
#define __nested__
#include <ti/sysbios/knl/Task.h>
#undef __nested__
#else
#include <ti/sysbios/knl/Task.h>
#endif
#endif

extern ti_sysbios_knl_Task_Struct TSK_idle;

