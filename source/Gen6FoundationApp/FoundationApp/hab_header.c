#include <stddef.h>
#include "hab_header.h"
#include "db_hdw.h"
#define COUNT_DCD 4			// dcd ahb clk div =1
#define MAX_EXP_SIZE 4		// security max exponent size
#include "HABtypes.h"
#include "mx25spi.h"
#define BOOT_START_ADDR 0x78006000 // where to jump after loading our boot
#define RAM_LOAD_ADDR (BOOT_START_ADDR-BOOTCODE_ROM_ADDR) 	// this is where  freescale boot puts first part of spi flash
#define ROM_HEADER_ADDRESS (0x400-offsetof(CompleteHeader_t,FlashHeader))
#define RAM_HDR_ADDR (RAM_LOAD_ADDR + ROM_HEADER_ADDRESS)
#define LOAD_SIZE (BOOTCODE_ROM_ADDR + 0x6000)

typedef struct
{
	DCD_T DeviveConfigData;
	FLASH_CFG_PARMS_T app_length_config;
	hab_rsa_public_key pubkey;
	FLASH_HDR_T FlashHeader;
}
CompleteHeader_t;

CompleteHeader_t HeaderComplete =
    {
        {
            {
                DCD_BARKER,
                COUNT_DCD* sizeof(DCD_TYPE_ADDR_DATA_T),
            },
            {
                {
                    0x00000004, // 4 byte write
                    (uint32_t *)0x53f80008, // crm
                    0xc0034000,		// ahb clock /1
                },
                {
                    0x00000004, // 4 byte write
                    (uint32_t *)0x43fac214, // green led vstbyreq green led
                    0x00000005,		// mode 5
                },
                {
                    0x00000004, // 4 byte write
                    (uint32_t *)0x43fac218, // green led vstbyreq red led
                    0x00000005,		// mode 5
                },
                {
                    0x00000004, // 4 byte write
                    (uint32_t *)0x43fac21c, // green led vstbyreq vibrate
                    0x00000005,		// mode 5
                },
            }
        },
        {LOAD_SIZE},				// load 16k for now
        {{0,0,0,0},0,4,256,0}, // srk key for secure modes
        {
            //            (uint32_t * )(RAM_HDR_ADDR + offsetof(CompleteHeader_t,BranchInstuct)),
            (uint32_t * )BOOT_START_ADDR,
            APP_BARKER,
            0x0,			// no csf data
            (DCD_T **)(RAM_HDR_ADDR+ offsetof(CompleteHeader_t,FlashHeader)+offsetof(FLASH_HDR_T,dcd_ptr)),
            (hab_rsa_public_key *)0,
            (DCD_T *)RAM_HDR_ADDR,	// dcd data is first
            (uint32_t * )RAM_LOAD_ADDR
        },
    };

unsigned int GetHeaderSize(void)
{
	return sizeof(HeaderComplete);
}

void * GetHeaderAddress(void)
{
	return (void *)&HeaderComplete;
}
void * GetRomHeaderAddress(void)
{
	return (void *)(ROM_HEADER_ADDRESS);
}

void * GetRamHeaderAddress(void)
{
	return (void *)(RAM_HDR_ADDR);
}
unsigned int SetBootCodeSize(unsigned int sz)
{
	return (HeaderComplete.app_length_config.length=sz+0x800);
}
//int SpiFlashBootCode(char * BootCode,unsigned int BootSize)
// flash boot codew into block 0 of spi flash
//		inputs:
//			BootCode pointer to buffer with boot code image
//			BootSize boot code size in bytes
//		returns 0 if sucsess other if failed
// Both the HAB header (see above) and boot code are flashed into bllock 0
int SpiFlashBootCode(unsigned char * BootCode,unsigned int BootSize)
{
	unsigned int SerRomAddr =(unsigned int)GetRomHeaderAddress(); // header start in rom
	char * pHeadertoWrite =(char *)GetHeaderAddress();			  // pointer to header data to write
	int hdrsize=GetHeaderSize();								// size of header data to write
	SetBootCodeSize(BootSize); 				// set boot code size in header
	SpiEraseSector(SerRomAddr);
	SpiWriteMemory( 0x800 ,BootSize,(char *)BootCode); // write boot code
	int result =SpiMemVerify( 0x800 ,BootSize,(char *)BootCode); // verify boot code
	if (result==0)
	{
		SpiWriteMemory( SerRomAddr ,hdrsize,pHeadertoWrite);		// write the hab header
		result =SpiMemVerify( SerRomAddr ,hdrsize,pHeadertoWrite);
	}
	return result;
}
