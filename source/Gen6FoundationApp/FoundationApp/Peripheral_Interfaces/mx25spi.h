#ifndef MX25SPI_H_
#define MX25SPI_H_
//======================================================================================
//	Revision information
//======================================================================================
// $RCSfile: Gen6FoundationApp/FoundationApp/Peripheral_Interfaces/mx25spi.h $
// $Revision: 1.6 $
// $Date: 2009/07/06 12:47:31EDT $
//======================================================================================

//======================================================================================

typedef struct spiregs_tag
{
	volatile unsigned long RXDATA;
	volatile unsigned long TXDATA;
	volatile unsigned long CONREG;
	volatile unsigned long INTREG;
	volatile unsigned long DMAREG;
	volatile unsigned long STATREG;
	volatile unsigned long PERIODREG;
	volatile unsigned long TESTREG;
}
spiregs_t;

#define BADSPINUM -1
#define BADSPIPARAM -2
#define NO_VERIFY -3
#define SPICTYP_WRITE 0
#define SPICTYP_READ 1
#define SPICTYP_VERIFY 2


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

int InitSpi(unsigned int spi_num,spiregs_t  *  spi_reg_init);

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
//======================================================================================

int SpiXfr(unsigned int spi_num,unsigned int devnum,unsigned int  *cmd,unsigned int numbytes, unsigned int cmdtype,char *buffer);

//======================================================================================
// verify memoy with data in buffer
// input address start address in spi flash to verify
//		size  size in bytes to verify
//		buff pointer to bytes to verify
// retuns 0 succsess non zero for error
//======================================================================================
int SpiMemVerify(unsigned int address,unsigned int size,char *buff);

//======================================================================================
// read memory from spi memory into buffer
// input address start address in spi flash to read from
//		size  size in bytes to read
//		buff pointer to bytes filled with data from read
// retuns 0 succsess non zero for error contents pointed to by buff modified with data read
//======================================================================================
int SpiMemRead(unsigned int address,unsigned int size,char *buff);

//======================================================================================
// writes memory using page writes
// assumes data already in flash to be suitable to write i.e erased or 1->0 change
// input address start address in spi flash to write to
//		size  size in bytes to write
//		buff pointer to bytes to write
//======================================================================================
int SpiWriteMemory(unsigned int address,unsigned int size,char *buff);

//======================================================================================
// erase a block (64k usually) in spi flasch
// input: address an address in the block to erase
// return 0 if succsess  non zero on error
//======================================================================================

int SpiEraseSector(unsigned int address);


//======================================================================================
// erase entire spi flasch
// input: N/A
// return 0 if succsess  non zero on error
//======================================================================================
int SpiEraseAllFlash(void);


#endif /*MX25SPI_H_*/
