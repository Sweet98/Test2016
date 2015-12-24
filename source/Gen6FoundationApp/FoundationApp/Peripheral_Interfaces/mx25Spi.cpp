//======================================================================================
//	Revision information
//======================================================================================
// $RCSfile: Gen6FoundationApp/FoundationApp/Peripheral_Interfaces/mx25Spi.cpp $
// $Revision: 1.23 $
// $Date: 2010/05/13 01:42:37EDT $
//======================================================================================
#include <stdlib.h>
#include <string.h>

#include "iMX25_CSPI.h"
#include "iMX25_CRM.h"
#include "iMX25_IOMUX.h"
#include "iMX25_GPIO.h"
#include "mx25spi.h"
#ifndef MATRIX_BOOT_CODE	// enable /disable interrupts for app nothing for boot
#include "interrupts.h"
#endif
#include "platform.h"		//watch dog
#ifndef BIT
#define BIT(a) (1U<<a)
#endif

#define NEED_SPI1 1    // to use spi 1 uncomment this to include port init data
#define NEED_SPI2 1		// include init data for spi2
//#define USE_DUAL_SPI // UNCOMMENT TO USE DUAL CHANNEL SPI

#define SPI_DATA_HI do {(*(volatile unsigned int *)GPIO2_DR0)|=BIT(23);}while (0)
#define	SPI_CLK_HI  do {(*(volatile unsigned int *)GPIO2_DR0)|=BIT(25);}while (0)
#define SPI_DATA_LO do {(*(volatile unsigned int *)GPIO2_DR0)&=~BIT(23);}while (0)
#define	SPI_CLK_LO  do {(*(volatile unsigned int *)GPIO2_DR0)&=~BIT(25);}while (0)
#define	SPI_CS_HI   do {(*(volatile unsigned int *)GPIO2_DR0)|=BIT(28);}while (0)
#define	SPI_CS_LO   do {(*(volatile unsigned int *)GPIO2_DR0)&=~BIT(28);}while (0)
#define SPI_DATA_IN ((*(volatile unsigned int *)GPIO2_PSR0) &BIT(24))
#pragma push
#pragma O3



static int SpiWritePage(unsigned int address,unsigned int size,char *buff);


static spiregs_t  *  spiports[]=
    {
        (spiregs_t * )CSPI1_BASE_ADDR,
        (spiregs_t *)CSPI2_BASE_ADDR,
        (spiregs_t *)CSPI3_BASE_ADDR
    };
// pin configuration table pad register address then value to write end with 0 address
// spi 1,2,3
static const unsigned int  spi1_io[] =
    {
#ifdef NEED_SPI1
        IOMUXC_SW_MUX_CTL_PAD_CSPI1_MOSI,
        MUX_MODE_0,
        IOMUXC_SW_MUX_CTL_PAD_CSPI1_MISO,
        MUX_MODE_0,
        IOMUXC_SW_MUX_CTL_PAD_NF_CE0,
        MUX_MODE_1,
        IOMUXC_SW_MUX_CTL_PAD_CSPI1_SCLK,
        MUX_MODE_0,
        IOMUXC_SW_PAD_CTL_PAD_CSPI1_MISO,
        0x0,									//Disable the internal pull-up 
        IOMUXC_SW_PAD_CTL_PAD_CSPI1_SCLK,
        0x0,									//Disable the internal pull-up 
        
#endif
        NULL
    };
static const unsigned int  spi2_io[] =

    {
#ifdef NEED_SPI2
        IOMUXC_SW_MUX_CTL_PAD_SD1_CMD,	//MOSI
        MUX_MODE_1,
        IOMUXC_SW_MUX_CTL_PAD_SD1_CLK,	//MISO
        MUX_MODE_1,
        IOMUXC_SW_MUX_CTL_PAD_SD1_DATA0,	//SCLK
        MUX_MODE_1,
        IOMUXC_SW_PAD_CTL_PAD_SD1_DATA0,
        0x0,								//Disable the internal pull-up        
        //     IOMUXC_SW_MUX_CTL_PAD_SD1_DATA1,   //READY not used
        //     MUX_MODE_1,
        IOMUXC_SW_MUX_CTL_PAD_SD1_DATA3,   // slave select 1
        MUX_MODE_1,
        //    IOMUXC_SW_MUX_CTL_PAD_SD1_DATA2,	//SS0 used as gpio for write protect
        //    MUX_MODE_1,
        IOMUXC_CSPI2_IPP_CSPI_CLK_IN_SELECT_INPUT, // input mux values
        0x01,
        //   IOMUXC_CSPI2_IPP_IND_DATAREADY_B_SELECT_INPUT,
        //   0x01,
        IOMUXC_CSPI2_IPP_IND_MISO_SELECT_INPUT,
        0x01,
        IOMUXC_CSPI2_IPP_IND_MOSI_SELECT_INPUT,
        0x01,
        //   IOMUXC_CSPI2_IPP_IND_SS0_B_SELECT_INPUT,
        //  0x01,
        IOMUXC_CSPI2_IPP_IND_SS1_B_SELECT_INPUT,
        0x01,
#endif
        NULL
    };
#if 0        // possible alternate when using etm cant boot from spi

{
	IOMUXC_SW_MUX_CTL_PAD_LD12,	//MOSI
	MUX_MODE_2,
	IOMUXC_SW_MUX_CTL_PAD_LD13,	//MISO
	MUX_MODE_2,
	IOMUXC_SW_MUX_CTL_PAD_LD14,	//SCLK
	MUX_MODE_2,
	IOMUXC_SW_MUX_CTL_PAD_LD15,   //READY not used
	MUX_MODE_2,
	IOMUXC_SW_MUX_CTL_PAD_CONTRAST,
	MUX_MODE_2,
	IOMUXC_SW_MUX_CTL_PAD_OE_ACD,	//SS0 used as gpio for write protect
	MUX_MODE_2,
	NULL
}
#endif
static const unsigned int  spi3_io[] =
    {
        NULL
    };

static   const unsigned int * const Spi_Io_Pin_Init[] =
    {
        spi1_io,
        spi2_io,
        spi3_io
    };

static const spiregs_t SpiDefault =					// default vaules when null is passed for spi_reg_init
    {
        0,			//dont care
        0, 			//dont care
        BIT(CSPI_CONREG_EN_LSH)|BIT(CSPI_CONREG_MODE_LSH) |BIT(CSPI_CONREG_SMC_LSH), // control register enabled master mode
        0,			// interrrupt reg
        0,			// dma reg
        0,			// status register dont care
        0,			// period register
        BIT(15)			// test register swap recieve byte order
    };


static const spiregs_t SpiSalve =					// default vaules when null is passed for spi_reg_init
    {
        0,			//dont care
        0, 			//dont care
        BIT(CSPI_CONREG_EN_LSH),// |(3<<CSPI_CONREG_CHIPSELECT_LSH), // control register enabled slave ss3 mode
        0,			// interrrupt reg
        0,			// dma reg
        0,			// status register dont care
        0,			// period register
        BIT(15)			// test register swap recieve byte order
    };


//======================================================================================
// bool InitSpi(unsigned int spi_num,spiregs_t*spi_reg_init)
// initializes spi 1 2 or 3 for further use as a spi interface
// inputs: spi number 1 2 or 3
//			spi_reg_init * pointer to spi register structure with initial values
//			use NULL to use defaults
// Defaults:	master mode 0,0 baud rate ipg clock /4 (max)
//				no irqs no dma default byte order no inter trasaction delay
// Modifies: sets pin controls for spi requested
//			 gates clock on
//======================================================================================
static short sprdtbl[256];
int InitSpixx(unsigned int spi_num,const spiregs_t  *  spi_reg_init)
{
	const unsigned int clockmask[3]=
	    {
	        BIT(CRM_CGCR1_IPG_CLK_CSPI1_LSH),
	        BIT(CRM_CGCR1_IPG_CLK_CSPI2_LSH),
	        BIT(CRM_CGCR1_IPG_CLK_CSPI3_LSH),
	    };
	const unsigned int * spiPadTbl;
	spi_num--;
	if (spi_num>2 || Spi_Io_Pin_Init[spi_num][0]==NULL)
	{
		return BADSPINUM;
	}
	spiPadTbl=	&(Spi_Io_Pin_Init[spi_num][0]);
#ifdef DISABLE_INTERRUPTS

	DISABLE_INTERRUPTS();
#endif

	*((volatile unsigned int *)CRM_CGR1 )|=clockmask[spi_num];	// clocks on
	spiports[spi_num]->CONREG=0;

	do													// load pin configuration
	{
		volatile unsigned int * pDest =(volatile unsigned int *)(*spiPadTbl++);
		if (pDest==NULL)							// pDest is address of pin config register
		{
			break;
		}
		*pDest=*spiPadTbl++;					// write the pin config value to register
	}
	while (true);
	if (spi_reg_init ==NULL)
	{
		spi_reg_init=(spiregs_t  * )&SpiDefault;
	}
	spiports[spi_num]->CONREG=1;
	spiports[spi_num]->CONREG=spi_reg_init->CONREG;
	spiports[spi_num]->INTREG=spi_reg_init->INTREG;
	spiports[spi_num]->DMAREG=spi_reg_init->DMAREG;
	spiports[spi_num]->PERIODREG=spi_reg_init->PERIODREG;
	spiports[spi_num]->CONREG=spi_reg_init->CONREG;
	spiports[spi_num]->TESTREG=spi_reg_init->TESTREG;
#ifdef RESTORE_INTERRUPTS

	RESTORE_INTERRUPTS();
#endif

	return 0;
}


int InitSpi(unsigned int, spiregs_t  * )
{
	//	*((volatile unsigned int *)GPIO2_GDIR0) &=~(BIT(23)|BIT(25) |BIT(28));
	int ret =InitSpixx(2,NULL);
	ret |= InitSpixx(1,&SpiSalve);
	spiports[0]->CONREG=0;
	SPI_CS_HI;
	(*(volatile unsigned int *)GPIO2_GDIR0)|=BIT(28);
	(*(volatile unsigned int *)GPIO2_GDIR0)&=~BIT(23);
	for (int i=0;i<256;i++)
	{
		sprdtbl[i]=0;
		int temp=i;
		for (int j=8;j>0;j--)
		{
			sprdtbl[i]<<=2;
			if (temp &0x80U)
				sprdtbl[i] |=1;
			temp<<=1;
		}
	}


	return ret;
}



typedef union char_int_tag
{
	unsigned int wrd;
	char byt[sizeof(unsigned int)];
}char_int_t;


// merge bits from spi1 and spi2 into a single bit stream
// input pointer to output buffer
// returns updated pointer
// bits alternate beween spi channels
// uses table built in spiinit to spread bits then combines them together
unsigned int * MergeRead(unsigned int *p)
{
	char_int_t val1,val2;
	char_int_t answer;
	unsigned char *t=(unsigned char *)p;
	val2.wrd=(spiports[1]->RXDATA);
	val1.wrd=(spiports[0]->RXDATA);
	for (int i = 0;i<4;i++)
	{
		answer.wrd= sprdtbl[val2.byt[i]]<<1;
		answer.wrd|=sprdtbl[val1.byt[i]];
		*t++=answer.byt[1];
		*t++=answer.byt[0];
	}
	return (unsigned int *)t;
}

//======================================================================================
//int SpiXfr(unsigned int spi_num,unsigned int devnum,unsigned int  *cmd,unsigned int numbytes, unsigned int cmdtype,char *buffer)
// Do a spi transfer of max 512 bytes
// Inputs:
//	spi_num 	spi channel to use 1..3
//	devnum 		device select number 0..3
//	cmd 		pointer to first 4 bytes of transfer as an int usually a command *cmd modified on return
//	numbytes 	number of bytes in transfer including command
//	cmdtype		0 send data in buffer dont update buffer with read data
//				1 read command fill buffer with read data
//				2 verify command read data and verify to buffer
//	buffer 		data used for command
// Output:
//	*cmd 		updated with data read from up to first 4 bytes of transfer
//	*buffer		possibly updated with read data see above
// returns		0 if succsess non 0 if error including no verify

// TODO:  break this up into separate functions (read(), write(), etc.).  When we do this
//        use a const unsigned char * for buffer when reading.  We should considder using
//        a class.
//======================================================================================

int SpiXfr(unsigned int ,unsigned int devnum,unsigned int  *cmd,unsigned int numbytes, unsigned int cmdtype,char *buffer)
{
	RESET_WATCHDOG_TIMER();
	if ((devnum>3)||numbytes >512 ||cmdtype >2)
	{
		return BADSPIPARAM;
	}
	static unsigned int buff[512/sizeof(unsigned int)];
	spiregs_t * pSpiRegs=spiports[1];													// set up chip select and bit length feilds
	unsigned int CtrlRegVal= pSpiRegs->CONREG;
	// write the control register with BL and SS values
	CtrlRegVal &= (unsigned int)(~(0xfffU <<CSPI_CONREG_BURST_LSH)|(3U<< CSPI_CONREG_CHIPSELECT_LSH));
	CtrlRegVal |=((((numbytes*8-1)) << CSPI_CONREG_BURST_LSH)| (devnum<<CSPI_CONREG_CHIPSELECT_LSH));
	pSpiRegs->CONREG= CtrlRegVal;
	pSpiRegs->STATREG =BIT(CSPI_STATREG_TC_LSH);
	int ret =0;
	unsigned int num_words =(numbytes+3)/4;
	buff[0]=__rev(*cmd);
	numbytes=(numbytes<4)?0:numbytes-4;
	if (cmdtype == SPICTYP_WRITE)
	{
		memcpy(&buff[1],buffer,numbytes);
	}
	for (unsigned int * p=buff;num_words>0;num_words--)															// while there are still bytes in burst
	{
		while ((pSpiRegs->STATREG & BIT(CSPI_STATREG_TF_LSH))) 									// wait for space
		{
		}
		pSpiRegs->TXDATA=__rev(*p);

		// write the data
		while (!(pSpiRegs->STATREG & BIT(CSPI_STATREG_RR_LSH))) 								// wait for rx
		{
		}
		*p=(pSpiRegs->RXDATA);
		p++;
	}
	while (!(pSpiRegs->STATREG & BIT(CSPI_STATREG_TC_LSH))) 								// wait for rx
	{
	}

	switch (cmdtype)
	{
	case SPICTYP_WRITE:
		break;
	case SPICTYP_READ:
		memcpy(buffer,&buff[1],numbytes);
		break;
	case SPICTYP_VERIFY:
		ret = memcmp(buffer,&buff[1],numbytes);
		break;
	}
	*cmd=__rev(buff[0]);
	return ret;
}



int SpiReadDbl(unsigned int address,unsigned int numbytes,char *buffer)
#ifdef USE_DUAL_SPI
{
	unsigned int size =numbytes;
	numbytes=(numbytes+1)/2;
	numbytes+=8;
	numbytes&=~3;
	RESET_WATCHDOG_TIMER();
	if (numbytes >(512-4))
	{
		return BADSPIPARAM;
	}
	*((volatile unsigned int *)IOMUXC_SW_MUX_CTL_PAD_SD1_DATA3)=MUX_MODE_5;
	static unsigned int buff[1024/sizeof(unsigned int)];

	spiports[0]->CONREG=(((((4)*8-1)) << CSPI_CONREG_BURST_LSH)| (3<<CSPI_CONREG_CHIPSELECT_LSH)|BIT(0));
	spiports[0]->TESTREG=BIT(15);
	// write the control register with BL and SS values
	unsigned int CtrlRegVal= spiports[1]->CONREG;
	CtrlRegVal &= (unsigned int)(~(0xfffU <<CSPI_CONREG_BURST_LSH)|(3U<< CSPI_CONREG_CHIPSELECT_LSH));
	CtrlRegVal |=(((((numbytes)*8)-1) << CSPI_CONREG_BURST_LSH)| (1<<CSPI_CONREG_CHIPSELECT_LSH));
	spiports[1]->CONREG= CtrlRegVal;
	spiports[1]->STATREG =BIT(CSPI_STATREG_TC_LSH);

	unsigned int num_words =(numbytes)/4;
	address |=0x3b000000;
	{
#ifdef DISABLE_INTERRUPTS

		DISABLE_INTERRUPTS();
#endif

		SPI_CS_LO;
#ifdef RESTORE_INTERRUPTS

		RESTORE_INTERRUPTS();
#endif

	}

	spiports[1]->TXDATA=address;

	unsigned int stat=spiports[1]->STATREG;
	while (!(stat & BIT(CSPI_STATREG_RR_LSH))) 								// wait for rx
	{
		stat=spiports[1]->STATREG;
	}
	address=(spiports[1]->RXDATA);
	volatile unsigned int t1=(spiports[0]->RXDATA);
	*((volatile unsigned int *)IOMUXC_SW_MUX_CTL_PAD_SD1_CMD)=MUX_MODE_5;
	num_words--;
	unsigned int wordstoread=num_words;
	unsigned int *p=buff;
	stat=spiports[1]->STATREG;
	while (true)
	{
		if  ((wordstoread>=4)&& (stat & BIT(4))) 								
		{
			p=MergeRead(p);
			p=MergeRead(p);
			p=MergeRead(p);
			p=MergeRead(p);
			wordstoread-=4;
		}
		else if (wordstoread && (stat & BIT(3)))
		{
			p=MergeRead(p);
			wordstoread--;
		}
		if (wordstoread==0)
			break;
		if ((num_words >=8) && (stat & BIT(0)) && ((wordstoread-num_words)==0))
		{
			spiports[1]->TXDATA=0;
			spiports[1]->TXDATA=0;
			spiports[1]->TXDATA=0;
			spiports[1]->TXDATA=0;
			spiports[1]->TXDATA=0;
			spiports[1]->TXDATA=0;
			spiports[1]->TXDATA=0;
			spiports[1]->TXDATA=0;
			num_words-=8;
		}
		else if ((num_words >=4) && (stat & BIT(1)) && ((wordstoread-num_words)<=4))
		{
			spiports[1]->TXDATA=0;
			spiports[1]->TXDATA=0;
			spiports[1]->TXDATA=0;
			spiports[1]->TXDATA=0;
			num_words-=4;
		}
		else if (num_words  && !(stat & BIT(2)) && ((wordstoread-num_words)<=7))
		{
			spiports[1]->TXDATA=0;
			num_words--;
			
			
		}
		stat=spiports[1]->STATREG;		
	}
	{
#ifdef DISABLE_INTERRUPTS

		DISABLE_INTERRUPTS();
#endif

		SPI_CS_HI;
#ifdef RESTORE_INTERRUPTS

		RESTORE_INTERRUPTS();
#endif

	}
	*((volatile unsigned int *)IOMUXC_SW_MUX_CTL_PAD_SD1_DATA3)=MUX_MODE_1;
	*((volatile unsigned int *)IOMUXC_SW_MUX_CTL_PAD_SD1_CMD)=MUX_MODE_1;

	spiports[0]->CONREG=0;
	char * t =(char *)buff;
	memcpy(buffer,&t[2],size);
	return 0;

}


#else
{
	RESET_WATCHDOG_TIMER();
	if (numbytes >(512-4))
	{
		return BADSPIPARAM;
	}
	static unsigned int buff[512/sizeof(unsigned int)];

	unsigned int CtrlRegVal= spiports[1]->CONREG;
	// write the control register with BL and SS values
	CtrlRegVal &= (unsigned int)(~(0xfffU <<CSPI_CONREG_BURST_LSH)|(3U<< CSPI_CONREG_CHIPSELECT_LSH));
	CtrlRegVal |=(((((numbytes+4)*8-1)) << CSPI_CONREG_BURST_LSH)| (1<<CSPI_CONREG_CHIPSELECT_LSH));
	spiports[1]->CONREG= CtrlRegVal;
	spiports[1]->STATREG =BIT(CSPI_STATREG_TC_LSH);
	unsigned int num_words =(numbytes+3)/4;
	address |=0x03000000;
	spiports[1]->TXDATA=address;
	while (!((spiports[1]->STATREG & BIT(CSPI_STATREG_RR_LSH))) ) 								// wait for rx
	{
	}
	address=(spiports[1]->RXDATA);
	unsigned int *p=buff;
	while (num_words>=4)
	{
		spiports[1]->TXDATA=0;
		spiports[1]->TXDATA=0;
		spiports[1]->TXDATA=0;
		spiports[1]->TXDATA=0;
		while (!((spiports[1]->STATREG & BIT(4))) ) 								// wait for rx
		{
		}

		*p++ =(spiports[1]->RXDATA);
		*p++ =(spiports[1]->RXDATA);
		*p++ =(spiports[1]->RXDATA);
		*p++ =(spiports[1]->RXDATA);
		num_words-=4;

	}
	for (int i=num_words;i>0;i--)
	{
		spiports[1]->TXDATA=0;
	}
	while (num_words)
	{
		if ((spiports[1]->STATREG  & BIT(3)))
		{
			*p++ =(spiports[1]->RXDATA);
			num_words--;
		}
	}
	memcpy(buffer,buff,numbytes);
	return 0;
}
#endif
// memory specific fuctions
#define PAGSIZ 256		// spi flash page size
#define RDSIZ 64*7		// spi flash read size
// Poll serial flash status register for write or erase done
static void WriteErasePoll(void)
{
	RESET_WATCHDOG_TIMER();
	while (true)
	{
		unsigned int wren =0x0500;
		SpiXfr(2,1,&wren,2,SPICTYP_WRITE,NULL);
		if ((wren &0x00000003)==0)
		{
			break;
		}
	}
}


// Internal function enable writing on spi flash
static void WriteEnableCommand(void)
{
	unsigned int wren =0x06060606;
	SpiXfr(2,1,&wren,1,SPICTYP_WRITE,NULL);
}

//======================================================================================
// verify memoy with data in buffer
// input address start address in spi flash to verify
//		size  size in bytes to verify
//		buff pointer to bytes to verify
// retuns 0 succsess non zero for error
//======================================================================================
int SpiMemVerify(unsigned int address,unsigned int size,char *buff)
{
	int ret=0;
	if (address >0xffffff)
	{
		return BADSPIPARAM;
	}
	address |=0x03000000;
	while (ret==0 && size >=PAGSIZ)
	{
		unsigned int addr=address;
		ret=SpiXfr(2,1,&addr,PAGSIZ+4,SPICTYP_VERIFY,buff);
		buff+=PAGSIZ;
		address+=PAGSIZ;
		size-=PAGSIZ;
	}
	if (ret ==0 && size)
	{
		ret = SpiXfr(2,1,&address,size+4,SPICTYP_READ,buff);
	}
	return ret;
}

//======================================================================================
// read memory from spi memory into buffer
// input address start address in spi flash to read from
//		size  size in bytes to read
//		buff pointer to bytes filled with data from read
// retuns 0 succsess non zero for error contents pointed to by buff modified with data read
//======================================================================================
int SpiMemRead(unsigned int address,unsigned int size,char *buff)
{
	if (address >0xffffff)
	{
		return BADSPIPARAM;
	}

	while (size >=RDSIZ)
	{
		SpiReadDbl(address,  RDSIZ,buff);
		buff+=RDSIZ;
		address+=RDSIZ;
		size-=RDSIZ;
	}
	if (size)
	{
		SpiReadDbl(address,size,buff);
	}
	return 0;
}

//======================================================================================
// internal page write function writes a page or partial page
// assumes data already in flash to be suitable to write i.e erased or 1->0 change
// input address start address in spi flash to write to
//		size  size in bytes to write
//		buff pointer to bytes to write
// retuns 0 succsess non zero for error
//======================================================================================
static int SpiWritePage(unsigned int address,unsigned int size,char *buff)
{
	if (address >0xffffff)
	{
		return BADSPIPARAM;
	}
	WriteEnableCommand();
	address |=0x02000000;
	SpiXfr(2,1,&address,size+4,SPICTYP_WRITE,buff);
	WriteErasePoll();
	return 0;
}
//======================================================================================
// writes memory using page writes
// assumes data already in flash to be suitable to write i.e erased or 1->0 change
// input address start address in spi flash to write to
//		size  size in bytes to write
//		buff pointer to bytes to write
//======================================================================================
int SpiWriteMemory(unsigned int address,unsigned int size,char *buff)
{
	int sz = PAGSIZ- (address &(PAGSIZ-1));		// sz is number of bytes to program in partial page
	sz=(size<sz)?size:sz;						// choose smaller of total size and partial page size
	int ret =SpiWritePage(address,sz,buff);
	buff+=sz;
	address+=sz;
	size-=sz;
	while (ret==0 && (size>=PAGSIZ))				// do full pages
	{
		ret =SpiWritePage(address,PAGSIZ,buff);
		buff+=PAGSIZ;
		address+=PAGSIZ;
		size-=PAGSIZ;
	}
	if (ret==0 && size>0)
	{
		ret =SpiWritePage(address,size,buff);
	}
	return ret;
}
//======================================================================================
// erase a block (64k usually) in spi flasch
// input: address an address in the block to erase
// return 0 if succsess  non zero on error
//======================================================================================
int SpiEraseSector(unsigned int address)
{
	if (address >0xffffff)
	{
		return BADSPIPARAM;
	}
	WriteEnableCommand();
	address |=0xd8000000;
	SpiXfr(2,1,&address,4,SPICTYP_WRITE,NULL);
	WriteErasePoll();
	return 0;
}

int SpiEraseAllFlash(void)
{
	WriteEnableCommand();
	unsigned int address =0xc7c7c7c7;
	int ret =SpiXfr(2,1,&address,1,SPICTYP_WRITE,NULL);
	if (!ret)
		WriteErasePoll();
	return ret;
}

#pragma pop


//======================================================================================
