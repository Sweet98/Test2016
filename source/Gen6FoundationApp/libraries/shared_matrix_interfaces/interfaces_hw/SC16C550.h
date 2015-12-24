/*=================================================================================
  class: SC16C550 - Phillips 16C550 UART chip

//=================================================================================
   $Source: interfaces_hw/SC16C550.h $
   $Date: 2004/04/02 03:18:41EST $
   $Revision:


//=================================================================================*/

#ifndef SC16C550_H
#define SC16C550_H
#define       RHRI             1
#define       THRI             2
#define       RLSII            4
#define       MSII             8
#define       SLEEPMODEI       0x10
#define       XOFFINTI         0x20
#define       RTSINTI          0x40
#define       CTSINTI          0x80



/* ==============================================================================
    UART definitions
   ============================================================================== */




/* Phillips UART SC16C550 Registers - This chip is hooked up to a Bluetooth chip */
typedef struct BaseBTUart_T
{
	// UART Address 0
    union
    {
        volatile unsigned char  RX;     // Recieve Holding Register
        volatile unsigned char  TX;     // Transmit Holding Register
        volatile unsigned char  DLLSB;  // Divisor Latch LSB (when LCR[7]=1)
        volatile unsigned char   i;
        volatile unsigned int   indexfix;
    } Loc0;
    
    // Address 1  base +4
    union
    {
        // Interrupt Enable Register
        union
        {
            struct
            {
                volatile unsigned char      RHR             : 1;
                volatile unsigned char      THR             : 1;
                volatile unsigned char      RLSI            : 1;
                volatile unsigned char      MSI             : 1;
                volatile unsigned char      SLEEPMODE       : 1;
                volatile unsigned char      XOFFINT         : 1;
                volatile unsigned char      RTSINT          : 1;
                volatile unsigned char      CTSINT          : 1;
            } b;
        } Ier;
        
        volatile unsigned char  DLMSB;    // Divisor Latch MSB (when LCR[7]=1)
        volatile unsigned char   i;
        volatile unsigned int   indexfix;
    } Loc1;

    // Address 2  base +8
    union
    {
        // FIFO Control Register
        union
        {
            struct
            {
                volatile unsigned char      FIFOENAB        : 1;
                volatile unsigned char      RXFIFORST       : 1;
                volatile unsigned char      TXFIFORST       : 1;
                volatile unsigned char      DMAMODESEL      : 1;
                volatile unsigned char      unused0         : 1;
                volatile unsigned char      unused1         : 1;
                volatile unsigned char      RXTRIGLVL       : 2;
            } b;
        } Fcr;
        
        // Interrupt Status Register        
        union
        {
            struct
            {
                volatile unsigned char      INTSTATUS       : 1;
                volatile unsigned char      INTPRIORITY     : 5;
                volatile unsigned char      FIFOSENABL      : 2;
            } b;
        } Isr;
        
        // Enhanced Feature Register        
        union
        {
            struct
            {
                volatile unsigned char      unused0         : 6;
                volatile unsigned char      AUTORTS         : 1;
                volatile unsigned char      AUTOCTS         : 1;
            } b;
        } Efr;
        volatile unsigned char i;
        volatile unsigned int   indexfix;
   } Loc2;

    // Address 3 - Line Control Register base +0x0c
    union
    {
        struct
        {
            volatile unsigned char      WORDLEN             : 2;
            volatile unsigned char      STOPBITS            : 1;
            volatile unsigned char      PARITYENAB          : 1;
            volatile unsigned char      EVENPAR             : 1;
            volatile unsigned char      SETPAR              : 1;
            volatile unsigned char      SETBRK              : 1;
            volatile unsigned char      DLEE                : 1;
        } b;
        volatile unsigned char i;
        volatile unsigned int   indexfix;
    } Lcr;

    // Address 4 - Modem Control Register base +0x10
    union
    {
        struct
        {
            volatile unsigned char      DTR                 : 1;
            volatile unsigned char      RTS                 : 1;
            volatile unsigned char      OUT1                : 1;
            volatile unsigned char      OUT2                : 1;
            volatile unsigned char      LOOPBACK            : 1;
            volatile unsigned char      unused5             : 1;
            volatile unsigned char      IRENAB              : 1;
            volatile unsigned char      unused7             : 1;
        } b;
        volatile unsigned char i;
        volatile unsigned int   indexfix;
    } Mcr;

    // Address 5 - Line Status Register
    union
    {
        struct
        {
            volatile unsigned char      RXDATARDY           : 1;
            volatile unsigned char      OVERRUNERR          : 1;
            volatile unsigned char      PARITYERR           : 1;
            volatile unsigned char      FRAMINGERR          : 1;
            volatile unsigned char      BREAKINT            : 1;
            volatile unsigned char      TXHOLDINGEMPTY      : 1;
            volatile unsigned char      TXEMPTY             : 1;
            volatile unsigned char      FIFODATAERR         : 1;
        } b;
        volatile unsigned char i;
        volatile unsigned int   indexfix;
    } Lsr;

    // Address 6 - Modem Status Register
    union
    {
        struct
        {
            volatile unsigned char      CTSDELTA            : 1;
            volatile unsigned char      DSRDELTA            : 1;
            volatile unsigned char      RIDELTA             : 1;
            volatile unsigned char      DCDDELTA            : 1;
            volatile unsigned char      CTS                 : 1;
            volatile unsigned char      DSR                 : 1;
            volatile unsigned char      RI                  : 1;
            volatile unsigned char      DCD                 : 1;
        } b;
        volatile unsigned char i;
        volatile unsigned int   indexfix;
    } Msr;

    // Address 7 - Scratchpad Register
    union
    {
        struct
        {
            volatile unsigned char      TEMPDATA            : 8;
        } b;
        volatile unsigned char i;
        volatile unsigned int   indexfix;
    } Spr;

} BLUEUART_T;

#define pBlueUart           ((BaseBTUart_T *) 0x15000000)
#define BlueUart            (*pBlueUart)

#endif // SC16C550_H




