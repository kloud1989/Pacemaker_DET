/******************************************************************************

  Copyright (C), 2001-2012, Texas Instrument.

 ******************************************************************************
  File Name     : KeyStone_DDR_Init.c
  Version       : Initial Draft
  Author        : Brighton Feng
  Created       : 2010/12/6
  Last Modified :
  Description   : KeyStone DDR configuration
  Function List :
              C6670_EVM_DDR_Init
              C6678_EVM_DDR_Init
              DDR_simple_test
              Dual_Nyquist_EVM_DDR_Init
              KeyStone_DDR_full_leveling
              KeyStone_DDR_incremental_leveling
              KeyStone_DDR_ECC_init
              KeyStone_DDR_ECC_INT_enable
              KeyStone_DDR_init
              TCI6614_EVM_DDR_Init
  History       :
  1.Date        : 2010/12/6
    Author      : Brighton Feng
    Modification: Created file

  2.Date         : 2011/4/17
    Author       : Brighton Feng
    Modification: for C6678 EVM

  3.Date         : 2011/7/12
    Author       : Brighton Feng
    Modification: for dual C6670 EVM    

  4.Date         : 2012/5/8
    Author       : Brighton Feng
    Modification: C6670 EVM and TCI6614 EVM

  5.Date         : 2012/11/28
    Author       : Brighton Feng
    Modification: Modify configuration according to update document
    	"sprabl2a_KeyStone DDR3 Initialization.pdf"
******************************************************************************/
#include <stdio.h>
#include <csl_bootcfgaux.h>
#include <cslr_xmc.h>
#include "KeyStone_DDR_Init.h"
#include "KeyStone_common.h"

#define DDR_INIT_MAX_RETRY_NUM 	10

#pragma DATA_SECTION(DDR_REGS_MPAX_cfg_table,".far:Core_MPAX")
MPAX_Config DDR_REGS_MPAX_cfg_table[]=
{
    /*BADDR         RADDR           SegementSize      AccessPermisionMask
    32-bit virtual  36-bit physical in byte, must     Access types allowed
    base address    address right   be power of 2     in this address range
                    shift by 4                                            */
    {0x21000000,    0x100000000>>4,  4*1024,         MP_SR|MP_SW|MP_UR|MP_UW}/*map DDR2 configuration registers at physical address 0x1:00000000 to logical address 0x21000000*/
};

//check the if the DDR3 registers are mapped. If not, map it
void DDR3_registers_adress_map()
{
	int i=0;
	for(i=0; i<16; i++)
	{
		if(0x10000000==(gpXMC_regs->XMPAX[i].XMPAXL&CSL_XMC_XMPAXL_RADDR_MASK))
		{//DDR3 registers at 0x1:00000000 is mapped in the XMPAX
			break;
		}
	}
	if(i==16) //not mapped
	{
		KeyStone_XMC_MPAX_setup(DDR_REGS_MPAX_cfg_table, 15,
			sizeof(DDR_REGS_MPAX_cfg_table)/sizeof(MPAX_Config));
	}		
}

/*****************************************************************************
 Prototype    : KeyStone_DDR_full_leveling
 Description  : execute DDR full leveling
 Input        : None
 Output       : None
 Return Value : 
 
  History        :
  1.Date         : 2010/12/6
    Author       : Brighton Feng
    Modification : Created function

*****************************************************************************/
int KeyStone_DDR_full_leveling()
{
	Uint32 uwStatus;
	
	/*enable full leveling*/
	gpDDR_regs->RDWR_LVL_RMP_CTRL = 
		(unsigned int)(1<<CSL_EMIF4F_RDWR_LVL_RMP_CTRL_REG_RDWRLVL_EN_SHIFT);

	/*start full leveling*/
	gpDDR_regs->RDWR_LVL_CTRL = 
		(unsigned int)(1<<CSL_EMIF4F_RDWR_LVL_CTRL_REG_RDWRLVLFULL_START_SHIFT);

	/*Read back any of the DDR3 controller registers.
	This ensures full leveling is complete because this step is executed 
	only after full	leveling completes.*/
	uwStatus= gpDDR_regs->RDWR_LVL_RMP_CTRL; 	//dummy read

	//Wait 3ms for leveling to complete
	TSC_delay_ms(3); 	

	if(gpDDR_regs->STATUS&(CSL_EMIF4F_STATUS_REG_RDLVLGATETO_MASK
		|CSL_EMIF4F_STATUS_REG_RDLVLTO_MASK
		|CSL_EMIF4F_STATUS_REG_WRLVLTO_MASK))
	{
		printf("DDR3 leveling has failed, STATUS = 0x%x\n", gpDDR_regs->STATUS);
        uwStatus = 1;
		return uwStatus;
	}
    uwStatus = 0;
	return uwStatus;
}


/*****************************************************************************
 Prototype    : KeyStone_DDR_read_incremental_leveling
 Description  : DDR incremental leveling for data read, 
 				can only be executed after full leveling.
		Execute incremenatl lelveing for 64 times with init_interval_us,
		and then, setup the incremental leveling inteval to maximum.
 Input        : initial incermental leveling inteval in us  
 Output       : None
 Return Value : 
 
  History        :
  1.Date         : 2012/11/29
    Author       : Brighton Feng
    Modification : Created function

*****************************************************************************/
void KeyStone_DDR_read_incremental_leveling(Uint32 init_interval_us)
{
	Uint32 init_interval;

	//prescaler is 2*7.8=15.6us
	init_interval= (int)((float)init_interval_us/15.6);

	if(0xFF<init_interval)
		init_interval= 0xFF;

	/*set initial incremental leveling inteval*/
	gpDDR_regs->RDWR_LVL_CTRL = 
		(1<<CSL_EMIF4F_RDWR_LVL_CTRL_REG_RDWRLVLINC_PRE_SHIFT)|
		(init_interval<<CSL_EMIF4F_RDWR_LVL_CTRL_REG_RDLVLINC_INT_SHIFT)|
		(init_interval<<CSL_EMIF4F_RDWR_LVL_CTRL_REG_RDLVLGATEINC_INT_SHIFT);

	//make sure at least 64 initial incremental leveling complete
	TSC_delay_us((init_interval+1)*15.6*64);	

	/*after initial incremental leveling, set incrmental leveling interval to maximum value
	to minmize the impact on performance*/
	gpDDR_regs->RDWR_LVL_CTRL = 
		(CSL_EMIF4F_RDWR_LVL_CTRL_REG_RDWRLVLINC_PRE_MASK)|
		(CSL_EMIF4F_RDWR_LVL_CTRL_REG_RDLVLINC_INT_MASK)|
		(CSL_EMIF4F_RDWR_LVL_CTRL_REG_RDLVLGATEINC_INT_MASK);

	if(gpDDR_regs->STATUS&(CSL_EMIF4F_STATUS_REG_RDLVLGATETO_MASK
		|CSL_EMIF4F_STATUS_REG_RDLVLTO_MASK))
	{
		printf("DDR3 leveling has failed, STATUS = 0x%x\n", gpDDR_regs->STATUS);
	}
}

/*****************************************************************************
 Prototype    : KeyStone_DDR_ECC_INT_enable
 Description  : Enable DDR ECC error interrupt
 Input        : none
 Output       : None
 Return Value : 
 
  History        :
  1.Date         : 2012/11/2
    Author       : Brighton Feng
    Modification : Created function

*****************************************************************************/
void KeyStone_DDR_ECC_INT_enable()
{
	//clear the status
	gpDDR_regs->IRQSTATUS_SYS= CSL_EMIF4F_IRQSTATUS_SYS_REG_RD_ECC_ERR_SYS_MASK
		|CSL_EMIF4F_IRQSTATUS_SYS_REG_WR_ECC_ERR_SYS_MASK;

	//enable error interrupt
	gpDDR_regs->IRQENABLE_SET_SYS= CSL_EMIF4F_IRQENABLE_SET_SYS_REG_EN_RD_ECC_ERR_SYS_MASK;
}
/*****************************************************************************
 Prototype    : KeyStone_DDR_ECC_init
 Description  : DDR ECC configuration
	Note��If ECC is disabled, the ECC byte lane is held in reset to save power.
	Hence, full-leveling must be triggered after enabling ECC to ensure that the
	ECC byte lane is leveled.
 Input        : ecc_cfg, pointer to the structure with configuration parameters
 Output       : None
 Return Value : 
 
  History        :
  1.Date         : 2012/11/2
    Author       : Brighton Feng
    Modification : Created function

*****************************************************************************/
void KeyStone_DDR_ECC_init (DDR_ECC_Config * ecc_cfg)
{
	int i;
	Uint32 ECC_EN= 0;
	Uint32 ECC_RANGE_PROT= 0;
	Uint32 ADDR_RNG_EN[2]= {0, 0};
	Uint32 START_ADDR[2]= {0, 0};
	Uint32 END_ADDR[2]= {0, 0};

	if(ecc_cfg)
	{
		ECC_RANGE_PROT= (Uint32)ecc_cfg->rangeMode;

		for(i= 0; i<2; i++)
		{
			if(ecc_cfg->addressRange[i].byteCnt)
			{
				ADDR_RNG_EN[i]= 1;
				START_ADDR[i]= (ecc_cfg->addressRange[i].startAddr>>17)&0xFFFF;
				END_ADDR[i]= ((ecc_cfg->addressRange[i].startAddr
					+ ecc_cfg->addressRange[i].byteCnt-1)>>17)&0xFFFF;
			}
		}

		if(ecc_cfg->addressRange[0].byteCnt+
			ecc_cfg->addressRange[1].byteCnt)
		{
			ECC_EN= 1;

			//set DDR performance conter 2 to count number of 1 bit error
			gpDDR_regs->PERF_CNT_CFG=
				(0xB<<CSL_EMIF4F_PERF_CNT_CFG_REG_CNTR2_CFG_SHIFT);

			KeyStone_DDR_ECC_INT_enable();
		}
	}

	gpDDR_regs->ECC_CTRL = 
		((ECC_EN)<<CSL_EMIF4F_ECC_CTRL_REG_ECC_EN_SHIFT)|
		((ECC_RANGE_PROT)<<CSL_EMIF4F_ECC_CTRL_REG_ECC_ADDR_RNG_PROT_SHIFT)|
		((ADDR_RNG_EN[1])<<CSL_EMIF4F_ECC_CTRL_REG_ECC_ADDR_RNG_2_EN_SHIFT)|
		((ADDR_RNG_EN[0])<<CSL_EMIF4F_ECC_CTRL_REG_ECC_ADDR_RNG_1_EN_SHIFT);

	gpDDR_regs->ECC_ADDR_RNG_1= 
		((START_ADDR[0])<<CSL_EMIF4F_ECC_ADDR_RNG_1_REG_ECC_STRT_ADDR_1_SHIFT)|
		((END_ADDR[0])<<CSL_EMIF4F_ECC_ADDR_RNG_1_REG_ECC_END_ADDR_1_SHIFT);

	gpDDR_regs->ECC_ADDR_RNG_2= 
		((START_ADDR[1])<<CSL_EMIF4F_ECC_ADDR_RNG_2_REG_ECC_STRT_ADDR_2_SHIFT)|
		((END_ADDR[1])<<CSL_EMIF4F_ECC_ADDR_RNG_2_REG_ECC_END_ADDR_2_SHIFT);

}

/*****************************************************************************
 Prototype    : KeyStone_DDR_clock_phase_init
 Description  : DDR clock phase initialization
	whenever the board routing delay for the clock is not longer than the 
	data routing by at least one-quarter of a clock period,	leveling may fail. 
	To solve this problem, the DDR3 clock output can be inverted to add	an 
	apparent extension of one-half the clock period to the clock net and the 
	other associated fly-by routes.
 Input        : bInvert: TRUE->invert clock; FALSE-> not invert
 Output       : None
 Return Value : None
 
  History        :
  1.Date         : 2012/11/28
    Author       : Brighton Feng
    Modification : Created function

*****************************************************************************/
void KeyStone_DDR_clock_phase_init (Bool bInvert)
{

	/*when we set INVERT_CLKOUT to 1, the CTRL_SLAVE_RATIO field in 
	DDR3_CONFIG_REG_0 should be programmed to 0x100. If the 
	INVERT_CLKOUT bit remains at 0, the default value of the
	CTRL_SLAVE_RATIO field must be programmed to 0x80.*/

	// clear ctrl_slave_ratio field
	gpBootCfgRegs->DDR3_CONFIG_REG[0] &= 
		~(CSL_BOOTCFG_DDR3_CONFIG_REG_0_CMD_REG_PHY_CTRL_SLAVE_RATIO_MASK);  

	if(bInvert)
	{
		// set ctrl_slave_ratio to 0x100
		gpBootCfgRegs->DDR3_CONFIG_REG[0] |= 
			(0x100<<CSL_BOOTCFG_DDR3_CONFIG_REG_0_CMD_REG_PHY_CTRL_SLAVE_RATIO_SHIFT);

		// Set invert_clkout = 1
		gpBootCfgRegs->DDR3_CONFIG_REG[12] |= 
			CSL_BOOTCFG_DDR3_CONFIG_REG_12_CMD_REG_PHY_INVERT_CLKOUT_MASK;
	}
	else
	{
		// set ctrl_slave_ratio to 0x80
		gpBootCfgRegs->DDR3_CONFIG_REG[0] |= 
			(0x80<<CSL_BOOTCFG_DDR3_CONFIG_REG_0_CMD_REG_PHY_CTRL_SLAVE_RATIO_SHIFT);

		// clear invert_clkout = 0
		gpBootCfgRegs->DDR3_CONFIG_REG[12] &= 
			~CSL_BOOTCFG_DDR3_CONFIG_REG_12_CMD_REG_PHY_INVERT_CLKOUT_MASK;
	}

	// set dll_lock_diff to 15
	gpBootCfgRegs->DDR3_CONFIG_REG[0] |= 
		CSL_BOOTCFG_DDR3_CONFIG_REG_0_CMD_REQ_PHY_DLL_LOCK_DIFF_MASK; 	

}

/*****************************************************************************
 Prototype    : KeyStone_DDR_latch_leveling_configuration
 Description  : KeyStone_DDR_latch_leveling_configuration
	the PHY_RESET is pulsed (0 -> 1 -> 0) to latch 
	leveling configuration values into the PHY logic.
 Input        : None
 Output       : None
 Return Value : None
 
  History        :
  1.Date         : 2012/11/28
    Author       : Brighton Feng
    Modification : Created function

*****************************************************************************/
void KeyStone_DDR_latch_leveling_configuration ()
{
	/*the PHY_RESET is pulsed (0 -> 1 -> 0) to latch 
	leveling configuration values into the PHY logic.*/
	gpDDR_regs->DDR_PHY_CTRL_1 &= ~(0x00008000);
	gpDDR_regs->DDR_PHY_CTRL_1 |= (0x00008000);
	gpDDR_regs->DDR_PHY_CTRL_1 &= ~(0x00008000);
}

/*****************************************************************************
 Prototype    : C6678_EVM_DDR_Init
 Description  : configure DDR according to the clock speed
                please note, clock_MHz is the clock speed in MHz, not data
                rate. 
                For example, clock speed for 1333.333M data rate is 666.667-
                MHz
 Input        : float clock_MHz  
 Output       : None
 Return Value : 
 
  History        :
  1.Date         : 2011/4/17
    Author       : Brighton Feng
    Modification : Created function

*****************************************************************************/
void C6678_EVM_DDR_Init(float clock_MHz, DDR_ECC_Config * ecc_cfg)
{
	Uint32 uiRev= gpBootCfgRegs->DEVICE_ID_REG0&0xF0000000;
	
	CSL_BootCfgUnlockKicker();

	/*Invert Clock Out*/
	KeyStone_DDR_clock_phase_init(TRUE);

	if(0==uiRev)
	{//Rev 1.0 does not support read eye leveling
		//Set bit 9 = 1 to use forced ratio leveling for read DQS
		gpBootCfgRegs->DDR3_CONFIG_REG[23] |= 0x00000200; 
	}
	
	//initial vale for leveling
	/*WRLVL_INIT_RATIO*/
	gpBootCfgRegs->DDR3_CONFIG_REG[2] = 0x20;
	gpBootCfgRegs->DDR3_CONFIG_REG[3] = 0x24;
	gpBootCfgRegs->DDR3_CONFIG_REG[4] = 0x3A;
	gpBootCfgRegs->DDR3_CONFIG_REG[5] = 0x38;
	gpBootCfgRegs->DDR3_CONFIG_REG[6] = 0x51;
	gpBootCfgRegs->DDR3_CONFIG_REG[7] = 0x5E;
	gpBootCfgRegs->DDR3_CONFIG_REG[8] = 0x5E;
	gpBootCfgRegs->DDR3_CONFIG_REG[9] = 0x5E;
	gpBootCfgRegs->DDR3_CONFIG_REG[10] = 0x44;

	/*GTLVL_INIT_RATIO*/
	gpBootCfgRegs->DDR3_CONFIG_REG[14] = 0xA1;
	gpBootCfgRegs->DDR3_CONFIG_REG[15] = 0x9E;
	gpBootCfgRegs->DDR3_CONFIG_REG[16] = 0xA7;
	gpBootCfgRegs->DDR3_CONFIG_REG[17] = 0xA9;
	gpBootCfgRegs->DDR3_CONFIG_REG[18] = 0xCA;
	gpBootCfgRegs->DDR3_CONFIG_REG[19] = 0xBE;
	gpBootCfgRegs->DDR3_CONFIG_REG[20] = 0xDD;
	gpBootCfgRegs->DDR3_CONFIG_REG[21] = 0xDD;
	gpBootCfgRegs->DDR3_CONFIG_REG[22] = 0xBA;

	// Customized board setting
//	gpBootCfgRegs->DDR3_CONFIG_REG[2] = 0x7D;
//	gpBootCfgRegs->DDR3_CONFIG_REG[3] = 0x7E;
//	gpBootCfgRegs->DDR3_CONFIG_REG[4] = 0x7C;
//	gpBootCfgRegs->DDR3_CONFIG_REG[5] = 0x76;
//	gpBootCfgRegs->DDR3_CONFIG_REG[6] = 0x6E;
//	gpBootCfgRegs->DDR3_CONFIG_REG[7] = 0x70;
//	gpBootCfgRegs->DDR3_CONFIG_REG[8] = 0x60;
//	gpBootCfgRegs->DDR3_CONFIG_REG[9] = 0x5C;
//	gpBootCfgRegs->DDR3_CONFIG_REG[10] = 0x70;
//
//	/*GTLVL_INIT_RATIO*/
//	gpBootCfgRegs->DDR3_CONFIG_REG[14] = 0xB6;
//	gpBootCfgRegs->DDR3_CONFIG_REG[15] = 0xB6;
//	gpBootCfgRegs->DDR3_CONFIG_REG[16] = 0xA2;
//	gpBootCfgRegs->DDR3_CONFIG_REG[17] = 0xA9;
//	gpBootCfgRegs->DDR3_CONFIG_REG[18] = 0x9C;
//	gpBootCfgRegs->DDR3_CONFIG_REG[19] = 0x9A;
//	gpBootCfgRegs->DDR3_CONFIG_REG[20] = 0x95;
//	gpBootCfgRegs->DDR3_CONFIG_REG[21] = 0x99;
//	gpBootCfgRegs->DDR3_CONFIG_REG[22] = 0x9F;

	/*the PHY_RESET is pulsed (0 -> 1 -> 0) to latch these 
	leveling configuration values into the PHY logic.*/
	KeyStone_DDR_latch_leveling_configuration ();

	/*Drives CKE low.
	This is a JEDEC requirement that we have 500us delay between reset de-assert 
	and cke assert and then program the correct refresh rate
	The DDR internal clock is divide by 16 before SDCFG write*/
	gpDDR_regs->SDRAM_REF_CTRL = CSL_EMIF4F_SDRAM_REF_CTRL_REG_INITREF_DIS_MASK
		|(unsigned int)(500.f*clock_MHz/16.f);

	gpDDR_regs->SDRAM_TIM_1 =
		((unsigned int)(13.5*clock_MHz/1000.f-0.0001f)<<CSL_EMIF4F_SDRAM_TIM_1_REG_T_RP_SHIFT)|
		((unsigned int)(13.5*clock_MHz/1000.f-0.0001f)<<CSL_EMIF4F_SDRAM_TIM_1_REG_T_RCD_SHIFT)|
		((unsigned int)(15*clock_MHz/1000.f-0.0001f)<<CSL_EMIF4F_SDRAM_TIM_1_REG_T_WR_SHIFT)|
		((unsigned int)(36*clock_MHz/1000.f-0.0001f)<<CSL_EMIF4F_SDRAM_TIM_1_REG_T_RAS_SHIFT)|
		((unsigned int)(49.5*clock_MHz/1000.f-0.0001f)<<CSL_EMIF4F_SDRAM_TIM_1_REG_T_RC_SHIFT)|
		((unsigned int)(45*clock_MHz/4000.f-0.0001f)<<CSL_EMIF4F_SDRAM_TIM_1_REG_T_RRD_SHIFT)| 	/*T_RRD = (tFAW/(4*tCK)) �C 1*/
		((unsigned int)(7.5*clock_MHz/1000.f-0.0001f)<<CSL_EMIF4F_SDRAM_TIM_1_REG_T_WTR_SHIFT);
	gpDDR_regs->SDRAM_TIM_2   = 
		((unsigned int)(6*clock_MHz/1000.f-0.0001f)<<CSL_EMIF4F_SDRAM_TIM_2_REG_T_XP_SHIFT)|
		((unsigned int)(120*clock_MHz/1000.f-0.0001f)<<CSL_EMIF4F_SDRAM_TIM_2_REG_T_XSNR_SHIFT)| 	/*T_XSNR = (tXS /tCK)�C 1*/
		((512-1)<<CSL_EMIF4F_SDRAM_TIM_2_REG_T_XSRD_SHIFT)| 	/*T_XSRD =tXSDLL�C 1*/
		((unsigned int)(7.5*clock_MHz/1000.f-0.0001f)<<CSL_EMIF4F_SDRAM_TIM_2_REG_T_RTP_SHIFT)|
		((unsigned int)(5.625*clock_MHz/1000.f-0.0001f)<<CSL_EMIF4F_SDRAM_TIM_2_REG_T_CKE_SHIFT);
	gpDDR_regs->SDRAM_TIM_3   = 
		(5<<CSL_EMIF4F_SDRAM_TIM_3_REG_T_PDLL_UL_SHIFT)| 	/*This field must always be programmed to 0x5.*/
		((5)<<CSL_EMIF4F_SDRAM_TIM_3_REG_T_CSTA_SHIFT)| 	/*This field should be set according to PHY requirements as 0x5.*/
		((unsigned int)(5.625*clock_MHz/1000.f+0.9999f)<<CSL_EMIF4F_SDRAM_TIM_3_REG_T_CKESR_SHIFT)|
		((64-1)<<CSL_EMIF4F_SDRAM_TIM_3_REG_ZQ_ZQCS_SHIFT)|
		((unsigned int)(110*clock_MHz/1000.f-0.0001f)<<CSL_EMIF4F_SDRAM_TIM_3_REG_T_RFC_SHIFT)|
		(15<<CSL_EMIF4F_SDRAM_TIM_3_REG_T_RAS_MAX_SHIFT); 	/*This field must always be programmed to 0xF.*/

	gpDDR_regs->DDR_PHY_CTRL_1  = 0x00100100|
		(12<<CSL_EMIF4F_DDR_PHY_CTRL_1_REG_READ_LATENCY_SHIFT); 	/*between CAS Latency + 1 and CAS Latency + 7*/

	gpDDR_regs->ZQ_CONFIG = 
		((0)<<CSL_EMIF4F_ZQ_CONFIG_REG_ZQ_CS1EN_SHIFT)|
		((1)<<CSL_EMIF4F_ZQ_CONFIG_REG_ZQ_CS0EN_SHIFT)|
		((1)<<CSL_EMIF4F_ZQ_CONFIG_REG_ZQ_DUALCALEN_SHIFT)| 	/*This bit should always be set to 1.*/
		((1)<<CSL_EMIF4F_ZQ_CONFIG_REG_ZQ_SFEXITEN_SHIFT)|
		((512/256-1)<<CSL_EMIF4F_ZQ_CONFIG_REG_ZQ_ZQINIT_MULT_SHIFT)| 	/*T_ZQ_ZQINIT_MULT = (tZQinit/tZQoper �C 1)*/
		((256/64-1)<<CSL_EMIF4F_ZQ_CONFIG_REG_ZQ_ZQCL_MULT_SHIFT)| 	/*T_ZQ_ZQCL_MULT = (tZQoper/tZQCS �C 1)*/
		/*interval between ZQCS commands = 0.5%/((TSens x Tdriftrate) + (VSens x Vdriftrate))
		=0.5%/((max (dRTTdT, dRONdTM) x Tdriftrate in C/second) + (max(dRTTdV, dRONdVM) x Vdriftrate in mV/second))
		this time need be converted to refresh period number*/
		(((unsigned int)(1000000000*0.5/(1.5*1.2+0.15*15))/(64000000/8192))
			<<CSL_EMIF4F_ZQ_CONFIG_REG_ZQ_REFINTERVAL_SHIFT);

	/*map priority 0,1,2,3 to COS0,
	map priority 3,5,6,7 to COS1*/
	gpDDR_regs->PRI_COS_MAP = 
		((1)<<CSL_EMIF4F_PRI_COS_MAP_REG_PRI_COS_MAP_EN_SHIFT)|
		((1)<<CSL_EMIF4F_PRI_COS_MAP_REG_PRI_7_COS_SHIFT)|
		((1)<<CSL_EMIF4F_PRI_COS_MAP_REG_PRI_6_COS_SHIFT)|
		((1)<<CSL_EMIF4F_PRI_COS_MAP_REG_PRI_5_COS_SHIFT)|
		((1)<<CSL_EMIF4F_PRI_COS_MAP_REG_PRI_4_COS_SHIFT)|
		((0)<<CSL_EMIF4F_PRI_COS_MAP_REG_PRI_3_COS_SHIFT)|
		((0)<<CSL_EMIF4F_PRI_COS_MAP_REG_PRI_2_COS_SHIFT)|
		((0)<<CSL_EMIF4F_PRI_COS_MAP_REG_PRI_1_COS_SHIFT)|
		((0)<<CSL_EMIF4F_PRI_COS_MAP_REG_PRI_0_COS_SHIFT);

	/*master based COS map is disabled*/
	gpDDR_regs->MSTID_COS_1_MAP= 0;
	gpDDR_regs->MSTID_COS_2_MAP= 0;

	/*LAT_CONFIG*/
	gpDDR_regs->VBUSM_CONFIG= 
		(8<<CSL_EMIF4F_VBUSM_CONFIG_REG_COS_COUNT_1_SHIFT)|
		(16<<CSL_EMIF4F_VBUSM_CONFIG_REG_COS_COUNT_2_SHIFT)|
		(32<<CSL_EMIF4F_VBUSM_CONFIG_REG_PR_OLD_COUNT_SHIFT);

	/*Read Write Execution Threshold*/
	gpDDR_regs->RD_WR_EXEC_THRSH= 
		((1024/8/8-1)<<CSL_EMIF4F_RD_WR_EXEC_THRSH_REG_RD_THRSH_SHIFT)
		|((512/8/8-1)<<CSL_EMIF4F_RD_WR_EXEC_THRSH_REG_WR_THRSH_SHIFT);

	KeyStone_DDR_ECC_init(ecc_cfg);

	/* enables DRAM configuration.  It still has the refresh interval 
	programmed to the longer number needed during DRAM initialization.*/
	gpDDR_regs->SDRAM_REF_CTRL = (unsigned int)(500.f*clock_MHz/16.f); 

	gpDDR_regs->SDRAM_CONFIG = 
		(3<<CSL_EMIF4F_SDRAM_CONFIG_REG_SDRAM_TYPE_SHIFT)| 	/*Set to 3 for DDR3. All other values reserved.*/
		(0<<CSL_EMIF4F_SDRAM_CONFIG_REG_IBANK_POS_SHIFT)|
		(DDR_TERM_RZQ_OVER_6<<CSL_EMIF4F_SDRAM_CONFIG_REG_DDR_TERM_SHIFT)|
		(DDR_DYN_ODT_DISABLED<<CSL_EMIF4F_SDRAM_CONFIG_REG_DYN_ODT_SHIFT)|
		(0<<CSL_EMIF4F_SDRAM_CONFIG_REG_DDR_DISABLE_DLL_SHIFT)|
		(SDRAM_DRIVE_RZQ_OVER_7<<CSL_EMIF4F_SDRAM_CONFIG_REG_SDRAM_DRIVE_SHIFT)|
		(DDR_CWL_7<<CSL_EMIF4F_SDRAM_CONFIG_REG_CWL_SHIFT)|
		(DDR_BUS_WIDTH_64<<CSL_EMIF4F_SDRAM_CONFIG_REG_NARROW_MODE_SHIFT)|
		(DDR_CL_9<<CSL_EMIF4F_SDRAM_CONFIG_REG_CL_SHIFT)|
		(DDR_ROW_SIZE_13_BIT<<CSL_EMIF4F_SDRAM_CONFIG_REG_ROWSIZE_SHIFT)|
		(DDR_BANK_NUM_8<<CSL_EMIF4F_SDRAM_CONFIG_REG_IBANK_SHIFT)|
		(0<<CSL_EMIF4F_SDRAM_CONFIG_REG_EBANK_SHIFT)|
		(DDR_PAGE_SIZE_10_BIT_1024_WORD<<CSL_EMIF4F_SDRAM_CONFIG_REG_PAGESIZE_SHIFT);

	TSC_delay_us(600); 	//Wait 600us for HW init to complete

//	gpDDR_regs->SDRAM_REF_CTRL    = 64000000/8192/(1000/clock_MHz);
	gpDDR_regs->SDRAM_REF_CTRL    = (unsigned int)64000.f*clock_MHz/8192.f;

	KeyStone_DDR_full_leveling();

	if(uiRev)
	{
		/*Rev 2.0, 
		read data eye training and read gate training are all triggered for 
		initial convergence. However, the read eye sample point may still be 
		invalid. Incremental leveling will then force the read eye sample point 
		to a good starting value and then it will robustly optimize the read 
		eye sample point after multiple successive iterations.*/
		KeyStone_DDR_read_incremental_leveling(100);
	}
}

/*****************************************************************************
 Prototype    : Dual_Nyquist_EVM_DDR_Init
 Description  : configure DDR according to the clock speed
                please note, clock_MHz is the clock speed in MHz, not data
                rate. 
                For example, clock speed for 1333.333M data rate is 666.667-
                MHz
 Input        : float clock_MHz  
 Output       : None
 Return Value : 
 
  History        :
  1.Date         : 2011/7/12
    Author       : Brighton Feng
    Modification : Created function

*****************************************************************************/
void Dual_Nyquist_EVM_DDR_Init(float clock_MHz, DDR_ECC_Config * ecc_cfg)
{
	Uint32 uiDSP_Num= KeyStone_Get_DSP_Number();

	if(0!=uiDSP_Num&&clock_MHz>533.333)
		clock_MHz= 533.333;
		
	CSL_BootCfgUnlockKicker();

	/*Invert Clock Out*/
	KeyStone_DDR_clock_phase_init(TRUE);
	
	/*different DSP on the board has different trace length to the DDR device
	so, the initial leveling value is different*/
	if(0==uiDSP_Num)
	{
		//initial vale for leveling                
		/*WRLVL_INIT_RATIO*/                       
		gpBootCfgRegs->DDR3_CONFIG_REG[2]  = 0x0F; 
		gpBootCfgRegs->DDR3_CONFIG_REG[3]  = 0x0F; 
		gpBootCfgRegs->DDR3_CONFIG_REG[4]  = 0x20; 
		gpBootCfgRegs->DDR3_CONFIG_REG[5]  = 0x24; 
		gpBootCfgRegs->DDR3_CONFIG_REG[6]  = 0x33; 
		gpBootCfgRegs->DDR3_CONFIG_REG[7]  = 0x38; 
		gpBootCfgRegs->DDR3_CONFIG_REG[8]  = 0x21; 
		gpBootCfgRegs->DDR3_CONFIG_REG[9]  = 0x29; 
		gpBootCfgRegs->DDR3_CONFIG_REG[10] = 0x0 ; 
                                                           
		/*GTLVL_INIT_RATIO*/                       
		gpBootCfgRegs->DDR3_CONFIG_REG[14] = 0x7B; 
		gpBootCfgRegs->DDR3_CONFIG_REG[15] = 0x7B; 
		gpBootCfgRegs->DDR3_CONFIG_REG[16] = 0x8D; 
		gpBootCfgRegs->DDR3_CONFIG_REG[17] = 0x89; 
		gpBootCfgRegs->DDR3_CONFIG_REG[18] = 0xC0; 
		gpBootCfgRegs->DDR3_CONFIG_REG[19] = 0xBC; 
		gpBootCfgRegs->DDR3_CONFIG_REG[20] = 0xC3; 
		gpBootCfgRegs->DDR3_CONFIG_REG[21] = 0xBB; 
		gpBootCfgRegs->DDR3_CONFIG_REG[22] = 0x0 ; 
	}
	else
	{
		//initial vale for leveling
		/*GTLVL_INIT_RATIO*/
		gpBootCfgRegs->DDR3_CONFIG_REG[22] = 0;
		gpBootCfgRegs->DDR3_CONFIG_REG[21] = 0xB8;
		gpBootCfgRegs->DDR3_CONFIG_REG[20] = 0xC0;
		gpBootCfgRegs->DDR3_CONFIG_REG[19] = 0xBE;
		gpBootCfgRegs->DDR3_CONFIG_REG[18] = 0xC2;
		gpBootCfgRegs->DDR3_CONFIG_REG[17] = 0x89;
		gpBootCfgRegs->DDR3_CONFIG_REG[16] = 0x8C;
		gpBootCfgRegs->DDR3_CONFIG_REG[15] = 0x7B;
		gpBootCfgRegs->DDR3_CONFIG_REG[14] = 0x7E;

		/*WRLVL_INIT_RATIO*/
		gpBootCfgRegs->DDR3_CONFIG_REG[10] = 0;
		gpBootCfgRegs->DDR3_CONFIG_REG[9]  = 0x25;
		gpBootCfgRegs->DDR3_CONFIG_REG[8]  = 0x1D;
		gpBootCfgRegs->DDR3_CONFIG_REG[7]  = 0x38;
		gpBootCfgRegs->DDR3_CONFIG_REG[6]  = 0x34;
		gpBootCfgRegs->DDR3_CONFIG_REG[5]  = 0x21;
		gpBootCfgRegs->DDR3_CONFIG_REG[4]  = 0x1E;
		gpBootCfgRegs->DDR3_CONFIG_REG[3]  = 0x0E;
		gpBootCfgRegs->DDR3_CONFIG_REG[2]  = 0x0B;
	}
	
	/*the PHY_RESET is pulsed (0 -> 1 -> 0) to latch these 
	leveling configuration values into the PHY logic.*/
	KeyStone_DDR_latch_leveling_configuration ();

	/*Drives CKE low.
	This is a JEDEC requirement that we have 500us delay between reset de-assert 
	and cke assert and then program the correct refresh rate
	The DDR internal clock is divide by 16 before SDCFG write*/
	gpDDR_regs->SDRAM_REF_CTRL = CSL_EMIF4F_SDRAM_REF_CTRL_REG_INITREF_DIS_MASK
		|(unsigned int)(500.f*clock_MHz/16.f);

	gpDDR_regs->SDRAM_TIM_1 =
		((unsigned int)(13.5*clock_MHz/1000.f-0.0001f)<<CSL_EMIF4F_SDRAM_TIM_1_REG_T_RP_SHIFT)|
		((unsigned int)(13.5*clock_MHz/1000.f-0.0001f)<<CSL_EMIF4F_SDRAM_TIM_1_REG_T_RCD_SHIFT)|
		((unsigned int)(15*clock_MHz/1000.f-0.0001f)<<CSL_EMIF4F_SDRAM_TIM_1_REG_T_WR_SHIFT)|
		((unsigned int)(36*clock_MHz/1000.f-0.0001f)<<CSL_EMIF4F_SDRAM_TIM_1_REG_T_RAS_SHIFT)|
		((unsigned int)(49.5*clock_MHz/1000.f-0.0001f)<<CSL_EMIF4F_SDRAM_TIM_1_REG_T_RC_SHIFT)|
		((unsigned int)(45*clock_MHz/4000.f-0.0001f)<<CSL_EMIF4F_SDRAM_TIM_1_REG_T_RRD_SHIFT)| 	/*T_RRD = (tFAW/(4*tCK)) �C 1*/
		(_max2(4-1, (unsigned int)(7.5*clock_MHz/1000.f-0.0001f))<<CSL_EMIF4F_SDRAM_TIM_1_REG_T_WTR_SHIFT);
	gpDDR_regs->SDRAM_TIM_2   = 
		(_max2(3-1, (unsigned int)(6*clock_MHz/1000.f-0.0001f))<<CSL_EMIF4F_SDRAM_TIM_2_REG_T_XP_SHIFT)|
		(_max2(5-1, (unsigned int)((160+10)*clock_MHz/1000.f-0.0001f))<<CSL_EMIF4F_SDRAM_TIM_2_REG_T_XSNR_SHIFT)| 	/*T_XSNR = (tXS /tCK)�C 1*/
		((512-1)<<CSL_EMIF4F_SDRAM_TIM_2_REG_T_XSRD_SHIFT)| 	/*T_XSRD =tXSDLL�C 1*/
		(_max2(4-1, (unsigned int)(7.5*clock_MHz/1000.f-0.0001f))<<CSL_EMIF4F_SDRAM_TIM_2_REG_T_RTP_SHIFT)|
		(_max2(3-1, (unsigned int)(5.625*clock_MHz/1000.f-0.0001f))<<CSL_EMIF4F_SDRAM_TIM_2_REG_T_CKE_SHIFT);
	gpDDR_regs->SDRAM_TIM_3   = 
		(5<<CSL_EMIF4F_SDRAM_TIM_3_REG_T_PDLL_UL_SHIFT)| 	/*This field must always be programmed to 0x5.*/
		((5)<<CSL_EMIF4F_SDRAM_TIM_3_REG_T_CSTA_SHIFT)| 	/*This field should be set according to PHY requirements as 0x5.*/
		(_max2(3, (unsigned int)(5.625*clock_MHz/1000.f+0.9999f))<<CSL_EMIF4F_SDRAM_TIM_3_REG_T_CKESR_SHIFT)|
		((64-1)<<CSL_EMIF4F_SDRAM_TIM_3_REG_ZQ_ZQCS_SHIFT)|
		((unsigned int)(160*clock_MHz/1000.f-0.0001f)<<CSL_EMIF4F_SDRAM_TIM_3_REG_T_RFC_SHIFT)|
		(15<<CSL_EMIF4F_SDRAM_TIM_3_REG_T_RAS_MAX_SHIFT); 	/*This field must always be programmed to 0xF.*/

	gpDDR_regs->DDR_PHY_CTRL_1  = 0x00100100|
		(12<<CSL_EMIF4F_DDR_PHY_CTRL_1_REG_READ_LATENCY_SHIFT); 	/*between CAS Latency + 1 and CAS Latency + 7*/

	gpDDR_regs->ZQ_CONFIG = 
		((0)<<CSL_EMIF4F_ZQ_CONFIG_REG_ZQ_CS1EN_SHIFT)|
		((1)<<CSL_EMIF4F_ZQ_CONFIG_REG_ZQ_CS0EN_SHIFT)|
		((1)<<CSL_EMIF4F_ZQ_CONFIG_REG_ZQ_DUALCALEN_SHIFT)| 	/*This bit should always be set to 1.*/
		((1)<<CSL_EMIF4F_ZQ_CONFIG_REG_ZQ_SFEXITEN_SHIFT)|
		((512/256-1)<<CSL_EMIF4F_ZQ_CONFIG_REG_ZQ_ZQINIT_MULT_SHIFT)| 	/*T_ZQ_ZQINIT_MULT = (tZQinit/tZQoper �C 1)*/
		((256/64-1)<<CSL_EMIF4F_ZQ_CONFIG_REG_ZQ_ZQCL_MULT_SHIFT)| 	/*T_ZQ_ZQCL_MULT = (tZQoper/tZQCS �C 1)*/
		/*interval between ZQCS commands = 0.5%/((TSens x Tdriftrate) + (VSens x Vdriftrate))
		=0.5%/((max (dRTTdT, dRONdTM) x Tdriftrate in C/second) + (max(dRTTdV, dRONdVM) x Vdriftrate in mV/second))
		this time need be converted to refresh period number*/
		(((unsigned int)(1000000000*0.5/(1.5*1.2+0.15*15))/(64000000/8192))
			<<CSL_EMIF4F_ZQ_CONFIG_REG_ZQ_REFINTERVAL_SHIFT);

	/*map priority 0,1,2,3 to COS0,
	map priority 3,5,6,7 to COS1*/
	gpDDR_regs->PRI_COS_MAP = 
		((1)<<CSL_EMIF4F_PRI_COS_MAP_REG_PRI_COS_MAP_EN_SHIFT)|
		((1)<<CSL_EMIF4F_PRI_COS_MAP_REG_PRI_7_COS_SHIFT)|
		((1)<<CSL_EMIF4F_PRI_COS_MAP_REG_PRI_6_COS_SHIFT)|
		((1)<<CSL_EMIF4F_PRI_COS_MAP_REG_PRI_5_COS_SHIFT)|
		((1)<<CSL_EMIF4F_PRI_COS_MAP_REG_PRI_4_COS_SHIFT)|
		((0)<<CSL_EMIF4F_PRI_COS_MAP_REG_PRI_3_COS_SHIFT)|
		((0)<<CSL_EMIF4F_PRI_COS_MAP_REG_PRI_2_COS_SHIFT)|
		((0)<<CSL_EMIF4F_PRI_COS_MAP_REG_PRI_1_COS_SHIFT)|
		((0)<<CSL_EMIF4F_PRI_COS_MAP_REG_PRI_0_COS_SHIFT);

	/*master based COS map is disabled*/
	gpDDR_regs->MSTID_COS_1_MAP= 0;
	gpDDR_regs->MSTID_COS_2_MAP= 0;

	/*LAT_CONFIG*/
	gpDDR_regs->VBUSM_CONFIG= 
		(8<<CSL_EMIF4F_VBUSM_CONFIG_REG_COS_COUNT_1_SHIFT)|
		(16<<CSL_EMIF4F_VBUSM_CONFIG_REG_COS_COUNT_2_SHIFT)|
		(32<<CSL_EMIF4F_VBUSM_CONFIG_REG_PR_OLD_COUNT_SHIFT);

	/*Read Write Execution Threshold*/
	gpDDR_regs->RD_WR_EXEC_THRSH= 
		((1024/8/8-1)<<CSL_EMIF4F_RD_WR_EXEC_THRSH_REG_RD_THRSH_SHIFT)
		|((512/8/8-1)<<CSL_EMIF4F_RD_WR_EXEC_THRSH_REG_WR_THRSH_SHIFT);

	KeyStone_DDR_ECC_init(ecc_cfg);

	/* enables DRAM configuration.  It still has the refresh interval 
	programmed to the longer number needed during DRAM initialization.*/
	gpDDR_regs->SDRAM_REF_CTRL = (unsigned int)(500.f*clock_MHz/16.f); 

	gpDDR_regs->SDRAM_CONFIG = 
		(3<<CSL_EMIF4F_SDRAM_CONFIG_REG_SDRAM_TYPE_SHIFT)| 	/*Set to 3 for DDR3. All other values reserved.*/
		(0<<CSL_EMIF4F_SDRAM_CONFIG_REG_IBANK_POS_SHIFT)|
		(DDR_TERM_RZQ_OVER_6<<CSL_EMIF4F_SDRAM_CONFIG_REG_DDR_TERM_SHIFT)|
		(DDR_DYN_ODT_DISABLED<<CSL_EMIF4F_SDRAM_CONFIG_REG_DYN_ODT_SHIFT)|
		(0<<CSL_EMIF4F_SDRAM_CONFIG_REG_DDR_DISABLE_DLL_SHIFT)|
		(SDRAM_DRIVE_RZQ_OVER_7<<CSL_EMIF4F_SDRAM_CONFIG_REG_SDRAM_DRIVE_SHIFT)|
		(DDR_CWL_7<<CSL_EMIF4F_SDRAM_CONFIG_REG_CWL_SHIFT)|
		(DDR_BUS_WIDTH_64<<CSL_EMIF4F_SDRAM_CONFIG_REG_NARROW_MODE_SHIFT)|
		(DDR_CL_9<<CSL_EMIF4F_SDRAM_CONFIG_REG_CL_SHIFT)|
		(DDR_ROW_SIZE_14_BIT<<CSL_EMIF4F_SDRAM_CONFIG_REG_ROWSIZE_SHIFT)|
		(DDR_BANK_NUM_8<<CSL_EMIF4F_SDRAM_CONFIG_REG_IBANK_SHIFT)|
		(0<<CSL_EMIF4F_SDRAM_CONFIG_REG_EBANK_SHIFT)|
		(DDR_PAGE_SIZE_10_BIT_1024_WORD<<CSL_EMIF4F_SDRAM_CONFIG_REG_PAGESIZE_SHIFT);

	TSC_delay_us(600); 	//Wait 600us for HW init to complete

//	gpDDR_regs->SDRAM_REF_CTRL    = 64000000/8192/(1000/clock_MHz);
	gpDDR_regs->SDRAM_REF_CTRL    = (unsigned int)64000.f*clock_MHz/8192.f;

	KeyStone_DDR_full_leveling();
	KeyStone_DDR_read_incremental_leveling(100);
}

/*****************************************************************************
 Prototype    : C6670_EVM_DDR_Init
 Description  : configure DDR according to the clock speed
                please note, clock_MHz is the clock speed in MHz, not data
                rate. 
                For example, clock speed for 1333.333M data rate is 666.667-
                MHz
 Input        : float clock_MHz  
 Output       : None
 Return Value : 
 
  History        :
  1.Date         : 2011/7/12
    Author       : Brighton Feng
    Modification : Created function

*****************************************************************************/
void C6670_EVM_DDR_Init(float clock_MHz, DDR_ECC_Config * ecc_cfg)
{
	CSL_BootCfgUnlockKicker();

	/*Invert Clock Out*/
	KeyStone_DDR_clock_phase_init(TRUE);
	
	//initial vale for leveling
	/*WRLVL_INIT_RATIO*/
	gpBootCfgRegs->DDR3_CONFIG_REG[2]  = 0x5E;
	gpBootCfgRegs->DDR3_CONFIG_REG[3]  = 0x5E;
	gpBootCfgRegs->DDR3_CONFIG_REG[4]  = 0x5E;
	gpBootCfgRegs->DDR3_CONFIG_REG[5]  = 0x51;
	gpBootCfgRegs->DDR3_CONFIG_REG[6]  = 0x38;
	gpBootCfgRegs->DDR3_CONFIG_REG[7]  = 0x3A;
	gpBootCfgRegs->DDR3_CONFIG_REG[8]  = 0x24;
	gpBootCfgRegs->DDR3_CONFIG_REG[9]  = 0x20;
	gpBootCfgRegs->DDR3_CONFIG_REG[10] = 0x44;

	/*GTLVL_INIT_RATIO*/
	gpBootCfgRegs->DDR3_CONFIG_REG[14] = 0xDD;
	gpBootCfgRegs->DDR3_CONFIG_REG[15] = 0xDD;
	gpBootCfgRegs->DDR3_CONFIG_REG[16] = 0xBE;
	gpBootCfgRegs->DDR3_CONFIG_REG[17] = 0xCA;
	gpBootCfgRegs->DDR3_CONFIG_REG[18] = 0xA9;
	gpBootCfgRegs->DDR3_CONFIG_REG[19] = 0xA7;
	gpBootCfgRegs->DDR3_CONFIG_REG[20] = 0x9E;
	gpBootCfgRegs->DDR3_CONFIG_REG[21] = 0xA1;
	gpBootCfgRegs->DDR3_CONFIG_REG[22] = 0xBA;

	/*the PHY_RESET is pulsed (0 -> 1 -> 0) to latch these 
	leveling configuration values into the PHY logic.*/
	KeyStone_DDR_latch_leveling_configuration ();

	/*Drives CKE low.
	This is a JEDEC requirement that we have 500us delay between reset de-assert 
	and cke assert and then program the correct refresh rate
	The DDR internal clock is divide by 16 before SDCFG write*/
	gpDDR_regs->SDRAM_REF_CTRL = CSL_EMIF4F_SDRAM_REF_CTRL_REG_INITREF_DIS_MASK
		|(unsigned int)(500.f*clock_MHz/16.f);

	gpDDR_regs->SDRAM_TIM_1 =
		((unsigned int)(13.5*clock_MHz/1000.f-0.0001f)<<CSL_EMIF4F_SDRAM_TIM_1_REG_T_RP_SHIFT)|
		((unsigned int)(13.5*clock_MHz/1000.f-0.0001f)<<CSL_EMIF4F_SDRAM_TIM_1_REG_T_RCD_SHIFT)|
		((unsigned int)(15*clock_MHz/1000.f-0.0001f)<<CSL_EMIF4F_SDRAM_TIM_1_REG_T_WR_SHIFT)|
		((unsigned int)(36*clock_MHz/1000.f-0.0001f)<<CSL_EMIF4F_SDRAM_TIM_1_REG_T_RAS_SHIFT)|
		((unsigned int)(49.5*clock_MHz/1000.f-0.0001f)<<CSL_EMIF4F_SDRAM_TIM_1_REG_T_RC_SHIFT)|
		((unsigned int)(45*clock_MHz/4000.f-0.0001f)<<CSL_EMIF4F_SDRAM_TIM_1_REG_T_RRD_SHIFT)| 	/*T_RRD = (tFAW/(4*tCK)) �C 1*/
		((unsigned int)(7.5*clock_MHz/1000.f-0.0001f)<<CSL_EMIF4F_SDRAM_TIM_1_REG_T_WTR_SHIFT);
	gpDDR_regs->SDRAM_TIM_2   = 
		((unsigned int)(6*clock_MHz/1000.f-0.0001f)<<CSL_EMIF4F_SDRAM_TIM_2_REG_T_XP_SHIFT)|
		((unsigned int)(120*clock_MHz/1000.f-0.0001f)<<CSL_EMIF4F_SDRAM_TIM_2_REG_T_XSNR_SHIFT)| 	/*T_XSNR = (tXS /tCK)�C 1*/
		((512-1)<<CSL_EMIF4F_SDRAM_TIM_2_REG_T_XSRD_SHIFT)| 	/*T_XSRD =tXSDLL�C 1*/
		((unsigned int)(7.5*clock_MHz/1000.f-0.0001f)<<CSL_EMIF4F_SDRAM_TIM_2_REG_T_RTP_SHIFT)|
		((unsigned int)(5.625*clock_MHz/1000.f-0.0001f)<<CSL_EMIF4F_SDRAM_TIM_2_REG_T_CKE_SHIFT);
	gpDDR_regs->SDRAM_TIM_3   = 
		(5<<CSL_EMIF4F_SDRAM_TIM_3_REG_T_PDLL_UL_SHIFT)| 	/*This field must always be programmed to 0x5.*/
		((5)<<CSL_EMIF4F_SDRAM_TIM_3_REG_T_CSTA_SHIFT)| 	/*This field should be set according to PHY requirements as 0x5.*/
		((unsigned int)(5.625*clock_MHz/1000.f+0.9999f)<<CSL_EMIF4F_SDRAM_TIM_3_REG_T_CKESR_SHIFT)|
		((64-1)<<CSL_EMIF4F_SDRAM_TIM_3_REG_ZQ_ZQCS_SHIFT)|
		((unsigned int)(110*clock_MHz/1000.f-0.0001f)<<CSL_EMIF4F_SDRAM_TIM_3_REG_T_RFC_SHIFT)|
		(15<<CSL_EMIF4F_SDRAM_TIM_3_REG_T_RAS_MAX_SHIFT); 	/*This field must always be programmed to 0xF.*/

	gpDDR_regs->DDR_PHY_CTRL_1  = 0x00100100|
		(13<<CSL_EMIF4F_DDR_PHY_CTRL_1_REG_READ_LATENCY_SHIFT); 	/*between CAS Latency + 1 and CAS Latency + 7*/

	gpDDR_regs->ZQ_CONFIG = 
		((0)<<CSL_EMIF4F_ZQ_CONFIG_REG_ZQ_CS1EN_SHIFT)|
		((1)<<CSL_EMIF4F_ZQ_CONFIG_REG_ZQ_CS0EN_SHIFT)|
		((1)<<CSL_EMIF4F_ZQ_CONFIG_REG_ZQ_DUALCALEN_SHIFT)| 	/*This bit should always be set to 1.*/
		((1)<<CSL_EMIF4F_ZQ_CONFIG_REG_ZQ_SFEXITEN_SHIFT)|
		((512/256-1)<<CSL_EMIF4F_ZQ_CONFIG_REG_ZQ_ZQINIT_MULT_SHIFT)| 	/*T_ZQ_ZQINIT_MULT = (tZQinit/tZQoper �C 1)*/
		((256/64-1)<<CSL_EMIF4F_ZQ_CONFIG_REG_ZQ_ZQCL_MULT_SHIFT)| 	/*T_ZQ_ZQCL_MULT = (tZQoper/tZQCS �C 1)*/
		/*interval between ZQCS commands = 0.5%/((TSens x Tdriftrate) + (VSens x Vdriftrate))
		=0.5%/((max (dRTTdT, dRONdTM) x Tdriftrate in C/second) + (max(dRTTdV, dRONdVM) x Vdriftrate in mV/second))
		this time need be converted to refresh period number*/
		(((unsigned int)(1000000000*0.5/(1.5*1.2+0.15*15))/(64000000/8192))
			<<CSL_EMIF4F_ZQ_CONFIG_REG_ZQ_REFINTERVAL_SHIFT);

	/*map priority 0,1,2,3 to COS0,
	map priority 3,5,6,7 to COS1*/
	gpDDR_regs->PRI_COS_MAP = 
		((1)<<CSL_EMIF4F_PRI_COS_MAP_REG_PRI_COS_MAP_EN_SHIFT)|
		((1)<<CSL_EMIF4F_PRI_COS_MAP_REG_PRI_7_COS_SHIFT)|
		((1)<<CSL_EMIF4F_PRI_COS_MAP_REG_PRI_6_COS_SHIFT)|
		((1)<<CSL_EMIF4F_PRI_COS_MAP_REG_PRI_5_COS_SHIFT)|
		((1)<<CSL_EMIF4F_PRI_COS_MAP_REG_PRI_4_COS_SHIFT)|
		((0)<<CSL_EMIF4F_PRI_COS_MAP_REG_PRI_3_COS_SHIFT)|
		((0)<<CSL_EMIF4F_PRI_COS_MAP_REG_PRI_2_COS_SHIFT)|
		((0)<<CSL_EMIF4F_PRI_COS_MAP_REG_PRI_1_COS_SHIFT)|
		((0)<<CSL_EMIF4F_PRI_COS_MAP_REG_PRI_0_COS_SHIFT);

	/*master based COS map is disabled*/
	gpDDR_regs->MSTID_COS_1_MAP= 0;
	gpDDR_regs->MSTID_COS_2_MAP= 0;

	/*LAT_CONFIG*/
	gpDDR_regs->VBUSM_CONFIG= 
		(8<<CSL_EMIF4F_VBUSM_CONFIG_REG_COS_COUNT_1_SHIFT)|
		(16<<CSL_EMIF4F_VBUSM_CONFIG_REG_COS_COUNT_2_SHIFT)|
		(32<<CSL_EMIF4F_VBUSM_CONFIG_REG_PR_OLD_COUNT_SHIFT);

	/*Read Write Execution Threshold*/
	gpDDR_regs->RD_WR_EXEC_THRSH= 
		((1024/8/8-1)<<CSL_EMIF4F_RD_WR_EXEC_THRSH_REG_RD_THRSH_SHIFT)
		|((512/8/8-1)<<CSL_EMIF4F_RD_WR_EXEC_THRSH_REG_WR_THRSH_SHIFT);

	KeyStone_DDR_ECC_init(ecc_cfg);

	/* enables DRAM configuration.  It still has the refresh interval 
	programmed to the longer number needed during DRAM initialization.*/
	gpDDR_regs->SDRAM_REF_CTRL = (unsigned int)(500.f*clock_MHz/16.f); 

	gpDDR_regs->SDRAM_CONFIG = 
		(3<<CSL_EMIF4F_SDRAM_CONFIG_REG_SDRAM_TYPE_SHIFT)| 	/*Set to 3 for DDR3. All other values reserved.*/
		(0<<CSL_EMIF4F_SDRAM_CONFIG_REG_IBANK_POS_SHIFT)|
		(DDR_TERM_RZQ_OVER_6<<CSL_EMIF4F_SDRAM_CONFIG_REG_DDR_TERM_SHIFT)|
		(DDR_DYN_ODT_DISABLED<<CSL_EMIF4F_SDRAM_CONFIG_REG_DYN_ODT_SHIFT)|
		(0<<CSL_EMIF4F_SDRAM_CONFIG_REG_DDR_DISABLE_DLL_SHIFT)|
		(SDRAM_DRIVE_RZQ_OVER_7<<CSL_EMIF4F_SDRAM_CONFIG_REG_SDRAM_DRIVE_SHIFT)|
		(DDR_CWL_7<<CSL_EMIF4F_SDRAM_CONFIG_REG_CWL_SHIFT)|
		(DDR_BUS_WIDTH_64<<CSL_EMIF4F_SDRAM_CONFIG_REG_NARROW_MODE_SHIFT)|
		(DDR_CL_9<<CSL_EMIF4F_SDRAM_CONFIG_REG_CL_SHIFT)|
		(DDR_ROW_SIZE_13_BIT<<CSL_EMIF4F_SDRAM_CONFIG_REG_ROWSIZE_SHIFT)|
		(DDR_BANK_NUM_8<<CSL_EMIF4F_SDRAM_CONFIG_REG_IBANK_SHIFT)|
		(0<<CSL_EMIF4F_SDRAM_CONFIG_REG_EBANK_SHIFT)|
		(DDR_PAGE_SIZE_10_BIT_1024_WORD<<CSL_EMIF4F_SDRAM_CONFIG_REG_PAGESIZE_SHIFT);

	TSC_delay_us(600); 	//Wait 600us for HW init to complete

//	gpDDR_regs->SDRAM_REF_CTRL    = 64000000/8192/(1000/clock_MHz);
	gpDDR_regs->SDRAM_REF_CTRL    = (unsigned int)64000.f*clock_MHz/8192.f;

	KeyStone_DDR_full_leveling();
	KeyStone_DDR_read_incremental_leveling(100);
}

/*****************************************************************************
 Prototype    : TCI6614_EVM_DDR_Init
 Description  : configure DDR according to the clock speed
                please note, clock_MHz is the clock speed in MHz, not data
                rate. 
                For example, clock speed for 1333.333M data rate is 666.667-
                MHz
 Input        : float clock_MHz  
 Output       : None
 Return Value : 
 
  History        :
  1.Date         : 2012/5/8
    Author       : Brighton Feng
    Modification : Created function

*****************************************************************************/
void TCI6614_EVM_DDR_Init(float clock_MHz, DDR_ECC_Config * ecc_cfg)
{
	Uint32 uiRev= gpBootCfgRegs->DEVICE_ID_REG0&0xF0000000;
	
	CSL_BootCfgUnlockKicker();

	/*Invert Clock Out*/
	KeyStone_DDR_clock_phase_init(TRUE);

	//initial vale for leveling
	/*WRLVL_INIT_RATIO*/                                                      
	gpBootCfgRegs->DDR3_CONFIG_REG[2] = 0x20;                                 
	gpBootCfgRegs->DDR3_CONFIG_REG[3] = 0x24;                                 
	gpBootCfgRegs->DDR3_CONFIG_REG[4] = 0x3A;                                 
	gpBootCfgRegs->DDR3_CONFIG_REG[5] = 0x38;                                 
	gpBootCfgRegs->DDR3_CONFIG_REG[6] = 0x51;                                 
	gpBootCfgRegs->DDR3_CONFIG_REG[7] = 0x5E;                                 
	gpBootCfgRegs->DDR3_CONFIG_REG[8] = 0x5E;                                 
	gpBootCfgRegs->DDR3_CONFIG_REG[9] = 0x5E;                                 
	gpBootCfgRegs->DDR3_CONFIG_REG[10]= 0x44;                                
                                                                                  
	/*GTLVL_INIT_RATIO*/                                                      
	gpBootCfgRegs->DDR3_CONFIG_REG[14] = 0xA1;                                
	gpBootCfgRegs->DDR3_CONFIG_REG[15] = 0xA0;                                
	gpBootCfgRegs->DDR3_CONFIG_REG[16] = 0xA7;                                
	gpBootCfgRegs->DDR3_CONFIG_REG[17] = 0xA9;                                
	gpBootCfgRegs->DDR3_CONFIG_REG[18] = 0xCA;                                
	gpBootCfgRegs->DDR3_CONFIG_REG[19] = 0xBE;                                
	gpBootCfgRegs->DDR3_CONFIG_REG[20] = 0xDD;                                
	gpBootCfgRegs->DDR3_CONFIG_REG[21] = 0xDD;                                
	gpBootCfgRegs->DDR3_CONFIG_REG[22] = 0xBA;                                

	/*the PHY_RESET is pulsed (0 -> 1 -> 0) to latch these 
	leveling configuration values into the PHY logic.*/
	KeyStone_DDR_latch_leveling_configuration ();

	/*Drives CKE low.
	This is a JEDEC requirement that we have 500us delay between reset de-assert 
	and cke assert and then program the correct refresh rate
	The DDR internal clock is divide by 16 before SDCFG write*/
	gpDDR_regs->SDRAM_REF_CTRL = CSL_EMIF4F_SDRAM_REF_CTRL_REG_INITREF_DIS_MASK
		|(unsigned int)(500.f*clock_MHz/16.f);

	gpDDR_regs->SDRAM_TIM_1 =
		((unsigned int)(13.75*clock_MHz/1000.f-0.0001f)<<CSL_EMIF4F_SDRAM_TIM_1_REG_T_RP_SHIFT)|
		((unsigned int)(13.75*clock_MHz/1000.f-0.0001f)<<CSL_EMIF4F_SDRAM_TIM_1_REG_T_RCD_SHIFT)|
		((unsigned int)(15*clock_MHz/1000.f-0.0001f)<<CSL_EMIF4F_SDRAM_TIM_1_REG_T_WR_SHIFT)|
		((unsigned int)(36*clock_MHz/1000.f-0.0001f)<<CSL_EMIF4F_SDRAM_TIM_1_REG_T_RAS_SHIFT)|
		((unsigned int)(49.5*clock_MHz/1000.f-0.0001f)<<CSL_EMIF4F_SDRAM_TIM_1_REG_T_RC_SHIFT)|
		((unsigned int)(45*clock_MHz/4000.f-0.0001f)<<CSL_EMIF4F_SDRAM_TIM_1_REG_T_RRD_SHIFT)| 	/*T_RRD = (tFAW/(4*tCK)) �C 1*/
		(_max2(4-1, (unsigned int)(7.5*clock_MHz/1000.f-0.0001f))<<CSL_EMIF4F_SDRAM_TIM_1_REG_T_WTR_SHIFT);
	gpDDR_regs->SDRAM_TIM_2   = 
		(_max2(3-1, (unsigned int)(6*clock_MHz/1000.f-0.0001f))<<CSL_EMIF4F_SDRAM_TIM_2_REG_T_XP_SHIFT)|
		(_max2(5-1, (unsigned int)((128+10)*clock_MHz/1000.f-0.0001f))<<CSL_EMIF4F_SDRAM_TIM_2_REG_T_XSNR_SHIFT)| 	/*T_XSNR = (tXS /tCK)�C 1*/
		((512-1)<<CSL_EMIF4F_SDRAM_TIM_2_REG_T_XSRD_SHIFT)| 	/*T_XSRD =tXSDLL�C 1*/
		(_max2(4-1, (unsigned int)(7.5*clock_MHz/1000.f-0.0001f))<<CSL_EMIF4F_SDRAM_TIM_2_REG_T_RTP_SHIFT)|
		(_max2(3-1, (unsigned int)(5.625*clock_MHz/1000.f-0.0001f))<<CSL_EMIF4F_SDRAM_TIM_2_REG_T_CKE_SHIFT);
	gpDDR_regs->SDRAM_TIM_3   = 
		(5<<CSL_EMIF4F_SDRAM_TIM_3_REG_T_PDLL_UL_SHIFT)| 	/*This field must always be programmed to 0x5.*/
		((5)<<CSL_EMIF4F_SDRAM_TIM_3_REG_T_CSTA_SHIFT)| 	/*This field should be set according to PHY requirements as 0x5.*/
		((unsigned int)(5.625*clock_MHz/1000.f+0.9999f)<<CSL_EMIF4F_SDRAM_TIM_3_REG_T_CKESR_SHIFT)|
		((64-1)<<CSL_EMIF4F_SDRAM_TIM_3_REG_ZQ_ZQCS_SHIFT)|
		((unsigned int)(128*clock_MHz/1000.f-0.0001f)<<CSL_EMIF4F_SDRAM_TIM_3_REG_T_RFC_SHIFT)|
		(15<<CSL_EMIF4F_SDRAM_TIM_3_REG_T_RAS_MAX_SHIFT); 	/*This field must always be programmed to 0xF.*/

	gpDDR_regs->DDR_PHY_CTRL_1  = 0x00100100|
		(15<<CSL_EMIF4F_DDR_PHY_CTRL_1_REG_READ_LATENCY_SHIFT); 	/*between CAS Latency + 1 and CAS Latency + 7*/

	gpDDR_regs->ZQ_CONFIG = 
		((0)<<CSL_EMIF4F_ZQ_CONFIG_REG_ZQ_CS1EN_SHIFT)|
		((1)<<CSL_EMIF4F_ZQ_CONFIG_REG_ZQ_CS0EN_SHIFT)|
		((1)<<CSL_EMIF4F_ZQ_CONFIG_REG_ZQ_DUALCALEN_SHIFT)| 	/*This bit should always be set to 1.*/
		((1)<<CSL_EMIF4F_ZQ_CONFIG_REG_ZQ_SFEXITEN_SHIFT)|
		((512/256-1)<<CSL_EMIF4F_ZQ_CONFIG_REG_ZQ_ZQINIT_MULT_SHIFT)| 	/*T_ZQ_ZQINIT_MULT = (tZQinit/tZQoper �C 1)*/
		((256/64-1)<<CSL_EMIF4F_ZQ_CONFIG_REG_ZQ_ZQCL_MULT_SHIFT)| 	/*T_ZQ_ZQCL_MULT = (tZQoper/tZQCS �C 1)*/
		/*interval between ZQCS commands = 0.5%/((TSens x Tdriftrate) + (VSens x Vdriftrate))
		=0.5%/((max (dRTTdT, dRONdTM) x Tdriftrate in C/second) + (max(dRTTdV, dRONdVM) x Vdriftrate in mV/second))
		this time need be converted to refresh period number*/
		(((unsigned int)(1000000000*0.5/(1.5*1.2+0.15*15))/(64000000/8192))
			<<CSL_EMIF4F_ZQ_CONFIG_REG_ZQ_REFINTERVAL_SHIFT);

	/*map priority 0,1,2,3 to COS0,
	map priority 3,5,6,7 to COS1*/
	gpDDR_regs->PRI_COS_MAP = 
		((1)<<CSL_EMIF4F_PRI_COS_MAP_REG_PRI_COS_MAP_EN_SHIFT)|
		((1)<<CSL_EMIF4F_PRI_COS_MAP_REG_PRI_7_COS_SHIFT)|
		((1)<<CSL_EMIF4F_PRI_COS_MAP_REG_PRI_6_COS_SHIFT)|
		((1)<<CSL_EMIF4F_PRI_COS_MAP_REG_PRI_5_COS_SHIFT)|
		((1)<<CSL_EMIF4F_PRI_COS_MAP_REG_PRI_4_COS_SHIFT)|
		((0)<<CSL_EMIF4F_PRI_COS_MAP_REG_PRI_3_COS_SHIFT)|
		((0)<<CSL_EMIF4F_PRI_COS_MAP_REG_PRI_2_COS_SHIFT)|
		((0)<<CSL_EMIF4F_PRI_COS_MAP_REG_PRI_1_COS_SHIFT)|
		((0)<<CSL_EMIF4F_PRI_COS_MAP_REG_PRI_0_COS_SHIFT);

	/*master based COS map is disabled*/
	gpDDR_regs->MSTID_COS_1_MAP= 0;
	gpDDR_regs->MSTID_COS_2_MAP= 0;

	if(0==uiRev)
	{//workaround for "Advisory 19 ARM Memory Corruption Issue" for rev1.0 and 1.1
		/*LAT_CONFIG*/
		gpDDR_regs->VBUSM_CONFIG= 0;
	}
	else
	{
		/*LAT_CONFIG*/
		gpDDR_regs->VBUSM_CONFIG= 
			(8<<CSL_EMIF4F_VBUSM_CONFIG_REG_COS_COUNT_1_SHIFT)|
			(16<<CSL_EMIF4F_VBUSM_CONFIG_REG_COS_COUNT_2_SHIFT)|
			(32<<CSL_EMIF4F_VBUSM_CONFIG_REG_PR_OLD_COUNT_SHIFT);
	}

	/*Read Write Execution Threshold*/
	gpDDR_regs->RD_WR_EXEC_THRSH= 
		((1024/8/8-1)<<CSL_EMIF4F_RD_WR_EXEC_THRSH_REG_RD_THRSH_SHIFT)
		|((512/8/8-1)<<CSL_EMIF4F_RD_WR_EXEC_THRSH_REG_WR_THRSH_SHIFT);

	KeyStone_DDR_ECC_init(ecc_cfg);

	/* enables DRAM configuration.  It still has the refresh interval 
	programmed to the longer number needed during DRAM initialization.*/
	gpDDR_regs->SDRAM_REF_CTRL = (unsigned int)(500.f*clock_MHz/16.f); 

	gpDDR_regs->SDRAM_CONFIG = 
		(3<<CSL_EMIF4F_SDRAM_CONFIG_REG_SDRAM_TYPE_SHIFT)| 	/*Set to 3 for DDR3. All other values reserved.*/
		(0<<CSL_EMIF4F_SDRAM_CONFIG_REG_IBANK_POS_SHIFT)|
		(DDR_TERM_RZQ_OVER_6<<CSL_EMIF4F_SDRAM_CONFIG_REG_DDR_TERM_SHIFT)|
		(DDR_DYN_ODT_DISABLED<<CSL_EMIF4F_SDRAM_CONFIG_REG_DYN_ODT_SHIFT)|
		(0<<CSL_EMIF4F_SDRAM_CONFIG_REG_DDR_DISABLE_DLL_SHIFT)|
		(SDRAM_DRIVE_RZQ_OVER_7<<CSL_EMIF4F_SDRAM_CONFIG_REG_SDRAM_DRIVE_SHIFT)|
		(DDR_CWL_8<<CSL_EMIF4F_SDRAM_CONFIG_REG_CWL_SHIFT)|
		(DDR_BUS_WIDTH_64<<CSL_EMIF4F_SDRAM_CONFIG_REG_NARROW_MODE_SHIFT)|
		(DDR_CL_11<<CSL_EMIF4F_SDRAM_CONFIG_REG_CL_SHIFT)|
		(DDR_ROW_SIZE_14_BIT<<CSL_EMIF4F_SDRAM_CONFIG_REG_ROWSIZE_SHIFT)|
		(DDR_BANK_NUM_8<<CSL_EMIF4F_SDRAM_CONFIG_REG_IBANK_SHIFT)|
		(0<<CSL_EMIF4F_SDRAM_CONFIG_REG_EBANK_SHIFT)|
		(DDR_PAGE_SIZE_10_BIT_1024_WORD<<CSL_EMIF4F_SDRAM_CONFIG_REG_PAGESIZE_SHIFT);

	TSC_delay_us(600); 	//Wait 600us for HW init to complete

//	gpDDR_regs->SDRAM_REF_CTRL    = 64000000/8192/(1000/clock_MHz);
	gpDDR_regs->SDRAM_REF_CTRL    = (unsigned int)64000.f*clock_MHz/8192.f;

	KeyStone_DDR_full_leveling();
	KeyStone_DDR_read_incremental_leveling(100);
}

/*****************************************************************************
 Prototype    : KeyStone_DDR_init
 Description  : Initialize the DDR3
 Input        : float ref_clock_MHz    
                unsigned int DDR_PLLM  
                unsigned int DDR_PLLD  
 Output       : None
 Return Value : 
 
  History        :
  1.Date         : 2010/12/6
    Author       : Brighton Feng
    Modification : Created function

*****************************************************************************/
void KeyStone_DDR_init(float ref_clock_MHz, unsigned int DDR_PLLM, 
	unsigned int DDR_PLLD, DDR_ECC_Config * ecc_cfg)
{
	Uint32 uiRetryCount=0, uiFailCount;
	TDSP_Board_Type DSP_Board_Type;
	float DDR_Speed_MHz, DDR_Clock_MHz;

	//check the if the DDR3 registers are mapped. If not, map it
	DDR3_registers_adress_map();
	
	DSP_Board_Type= KeyStone_Get_dsp_board_type();

	DDR_Speed_MHz= ref_clock_MHz*DDR_PLLM/DDR_PLLD;
	DDR_Clock_MHz= DDR_Speed_MHz/2; 	//data speed is double of clock speed

	while(DDR_INIT_MAX_RETRY_NUM>uiRetryCount)
	{
		KeyStone_DDR_PLL_init(ref_clock_MHz, DDR_PLLM, DDR_PLLD);
		
		if(DUAL_NYQUIST_EVM==DSP_Board_Type)
			Dual_Nyquist_EVM_DDR_Init(DDR_Clock_MHz, ecc_cfg);
		else if(C6678_EVM==DSP_Board_Type)
			C6678_EVM_DDR_Init(DDR_Clock_MHz, ecc_cfg);
		else if(C6670_EVM==DSP_Board_Type)
			C6670_EVM_DDR_Init(DDR_Clock_MHz, ecc_cfg);
		else if(TCI6614_EVM==DSP_Board_Type)
			TCI6614_EVM_DDR_Init(DDR_Clock_MHz, ecc_cfg);
		else
		{
			puts("unknown DSP board type!");
			break;
		}

		//simple test to verify the basic function, return if it pass, retry if it fail
		uiFailCount  = Memory_Fill_Test(0x80000000,256, 0x00000000, 8, 8);
		uiFailCount += Memory_Fill_Test(0x80000000,256, 0xFFFFFFFF, 8, 8);
		uiFailCount += Memory_Address_Test(0x80000000,256, 8, 8);

		if(NULL==ecc_cfg)
		{//only continuous filling test can be done when ECC is enabled.
			uiFailCount += Memory_Data_Bus_Test(0x80000000, 64);
			uiFailCount += Memory_Address_Bus_Test(0x80000000, 512*1024*1024, 1);
		}

		if(0==uiFailCount)
			return;
			
		uiRetryCount++;
		printf("DDR reinitialization %d\n", uiRetryCount);
	}
}

/*****************************************************************************
 Prototype    : KeyStone_DDR_reinit
 Description  : re-initialize the DDR3
 Input        : float ref_clock_MHz
                unsigned int DDR_PLLM
                unsigned int DDR_PLLD
 Output       : None
 Return Value :

  History        :
  1.Date         : 2015/5/5
    Author       : Wayne
    Modification : Created function

*****************************************************************************/
void KeyStone_DDR_reinit(float ref_clock_MHz, unsigned int DDR_PLLM,
	unsigned int DDR_PLLD, DDR_ECC_Config * ecc_cfg)
{
	TDSP_Board_Type DSP_Board_Type;
	float DDR_Speed_MHz, DDR_Clock_MHz;

	//check the if the DDR3 registers are mapped. If not, map it
	DDR3_registers_adress_map();

	DSP_Board_Type= KeyStone_Get_dsp_board_type();

	DDR_Speed_MHz= ref_clock_MHz*DDR_PLLM/DDR_PLLD;
	DDR_Clock_MHz= DDR_Speed_MHz/2; 	//data speed is double of clock speed


	KeyStone_DDR_PLL_init(ref_clock_MHz, DDR_PLLM, DDR_PLLD);

	if(DUAL_NYQUIST_EVM==DSP_Board_Type)
		Dual_Nyquist_EVM_DDR_Init(DDR_Clock_MHz, ecc_cfg);
	else if(C6678_EVM==DSP_Board_Type)
		C6678_EVM_DDR_Init(DDR_Clock_MHz, ecc_cfg);
	else if(C6670_EVM==DSP_Board_Type)
		C6670_EVM_DDR_Init(DDR_Clock_MHz, ecc_cfg);
	else if(TCI6614_EVM==DSP_Board_Type)
		TCI6614_EVM_DDR_Init(DDR_Clock_MHz, ecc_cfg);
	else
	{
		puts("unknown DSP board type!");
	}
}
