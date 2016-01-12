#include "ADS1298_drv.h"

// command definition
#define WAKEUP	(0x02)
#define STANDBY	(0x04)
#define RESET	(0x06)
#define START	(0x08)
#define STOP	(0x0A)
#define RDATAC	(0x10)
#define SDATAC	(0x11)
#define RDATA	(0x12)

// For RREG and WREG:
// r rrrr = starting register address for read/write opcodes.
// n nnnn ) number of registers to be read/written – 1
#define RREG	(0x20)	// FIRST BYTE: 001r rrrr (2xh); SECOND BYTE: 000n nnnn
#define WREG	(0x40)	// FIRST BYTE: 010r rrrr (4xh); SECOND BYTE: 000n nnnn


// register address definition
#define ID			(0x00)
#define CONFIG1		(0x01)
#define CONFIG2		(0x02)
#define CONFIG3		(0x03)
#define LOFF		(0x04)
#define CH1SET		(0x05)
#define CH2SET		(0x06)
#define CH3SET		(0x07)
#define CH4SET		(0x08)
#define CH5SET		(0x09)
#define CH6SET		(0x0A)
#define CH7SET		(0x0B)
#define CH8SET		(0x0C)
#define RLD_SENSP	(0x0D)
#define RLD_SENSN	(0x0E)
#define LOFF_SENSP	(0x0F)
#define LOFF_SENSN	(0x10)
#define LOFF_FLIP	(0x11)
#define LOFF_STATP	(0x12)
#define LOFF_STATN	(0x13)
#define GPIO		(0x14)
#define PACE		(0x15)
#define RESP		(0x16)
#define CONFIG4		(0x17)
#define WCT1		(0x18)
#define WCT2		(0x19)

SPI_Data_Format ADS1298DataFormat =
{
	/*.delayBetweenTrans_ns = */0,
	/*.ShifDirection        = */SPI_MSB_SHIFT_FIRST,
	/*.disable_CS_timing    = */0,
	/*.clockPolarity        = */SPI_CLOCK_LOW_INACTIVE,
	/*.clockPhase           = */0,
	/*.clockSpeedKHz        = */1000,	// 为保证多字节读写寄存器操作能连续完成，不能超过2MHz
	/*.wordLength           = */8
};

SPI_Transfer_Param ADS1298TransferParam =
{
	// CS=0, CS0/CS1都响应
	// CS=1, CS1响应，CS0不响应
	// CS=2, CS0响应，CS1不响应
	// CS=3, 都不响应
	1,     /*Chip select number*/
	0,  /*select one of the 4 SPI formats*/
	SPI_CS_ALWAYS_HOLD, /*hold CS between multiple words*/
	FALSE,  /*Enable the delay counter at the end of the current transaction*/
	1   /*number of bytes per SPI word*/
};

void Check_Reg_State();

void ADS1298_config_init()
{
	Uint8 command;

	/*ADS1298配置寄存器*/
	Uint8 Config_Info1[19] =
//	{
//			0x41, 0x10, 0x85, 0x12, 0xCC, 0x03, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60,
//			0x60, 0x60, 0x06, 0x02, 0xFF, 0xFF, 0x00
//
//	};
			// 0x41,0x10,0x86 500Hz
			// 0x41,0x10,0x85 1000Hz
	{
			0x41,0x10,0x85,0x07,0xCC,0x03,0x10,0x10,0x10,0x10,0x10,0x10,
			0x10,0x10,0x06,0x02,0xFF,0xFF,0x00

	};

	Uint8 Config_Info2[5] =	{ 0x57, 0x02, 0x02, 0x0A, 0xDC };

	// Send SDATAC Command, Device Wakes Up in RDATAC Mode, so Send
	// SDATAC Command so Registers can be Written
	command = SDATAC;
	KeyStone_SPI_TxRx(&command, 0, 1, NULL, 0, 0, &ADS1298TransferParam);

	// Write Certain Registers
	KeyStone_SPI_TxRx(Config_Info1, 0, 19, NULL, 0, 0, &ADS1298TransferParam);
	KeyStone_SPI_TxRx(Config_Info2, 0, 5, NULL, 0, 0, &ADS1298TransferParam);

	Check_Reg_State();

	// Put the Device Back in RDATAC Mode
	command = RDATAC;
	KeyStone_SPI_TxRx(&command, 0, 1, NULL, 0, 0, &ADS1298TransferParam);
}

void Check_Reg_State()
{
//	Uint8 command;
	Uint8 regs[26];
	Int8 i;

	Uint8 readAllRegCmd[2] =
	{
			0x20, 0x19
	};

//	command = SDATAC;
//	KeyStone_SPI_TxRx(&command, 0, 1, NULL, 0, 0, &ADS1298TransferParam);
	KeyStone_SPI_TxRx(readAllRegCmd, 0, 2, NULL, 0, 0, &ADS1298TransferParam);
	KeyStone_SPI_TxRx(NULL, 0, 0, regs, 0, 26, &ADS1298TransferParam);

//	command = RDATAC;
//	KeyStone_SPI_TxRx(&command, 0, 1, NULL, 0, 0, &ADS1298TransferParam);

	platform_write(" ADS1298 Register States) \n");
	for(i=0; i<25; i++)
	{
		platform_write("0x%02x, ", regs[i]);
	}
	platform_write("0x%02x \n", regs[25]);
}
