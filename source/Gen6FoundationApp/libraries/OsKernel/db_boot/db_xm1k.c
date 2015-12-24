/************************************************************************/
//
// HHP 4480 embedded firmware
//
//  Xmodem 1k C code for boot code
//  Date:       2/24/02
//  Tools:      ARM SDT 2.5 for starters... will be ARM ADS 1.1 or Metaware
//  Target:     D-ball 4480
//
//======================================================================================
//	Revision information
//======================================================================================
// $RCSfile: db_boot/db_xm1k.c $
// $Revision: 1.3 $
// $Date: 2009/10/08 13:41:32EDT $
//======================================================================================
/************************************************************************/
/************************************************************************/
/*  Starting point for this code was code off the net at Digital in the	*/
/* SAAPs for Brutus.  Code was modified to fit in system memory	for 	*/
/* our handle-board but remains essentially Digital's code... with their*/
/* copywrites et all...													*/
/*																		*/
/* Goal here was to allow a Binary file to be download and then run from*/
/* RAM.																	*/
/*																		*/
/* The entire process is initiated by the download monitor command (ZU)	*/
/* From there, xmodem loop starts running and upon completion decides	*/
/* whether to run or copy to Flash for safe keeping (and then run).	   	*/
/************************************************************************/
/*|=======================================================================
  |			MODIFICATION HISTORY
  |=======================================================================
  |
*************************************************************************/
#include "db_xmod.h"
#include "db_crc.h"             /* function to calculate CCITT-16 CRC                       */
#include "db_chrio.h"       // Character input / output routine definitions
#include "db_time.h"
#pragma thumb


unsigned char *block;

/*
	Try to get a char for ticks milliseconds.
*/
int readChar(int ticks, int *status)
{
	int	c;

	*status = TIMEOUT_ERROR;
	set_time_limit(ticks);

	do
	{
		RESET_WATCHDOG_TIMER();
		poll_getchar_from_rx_fifo();
		if(time_limit_expired())
		{
			*status = TIMEOUT_ERROR;
			return BADNEWS;
		}

	}
	while (!(kbhit())); // || poll_getchar_from_rx_fifo())); /* wait until something there to receive */

	c = get_char();               // get char from queue

	*status = GOODNEWS;

	return c;
}


/*
	readblock - read size chars into block
	status - GOODNEWS - block read successfully
			BADNEWS - an error occurred
*/
int readBlock(int size, int *status)
{
	unsigned long	crc;
	unsigned long	rcrc;
	int		c, i, blockn, invblockn;

	*status=BADNEWS;

	crc=0;
	blockn=readChar(WAIT_PERIOD,status);
	if(GOODNEWS!=*status)
	{
		//xputstring("Timeout reading block number\n");
		return(BADNEWS);
	}

	invblockn=readChar(WAIT_PERIOD,status);
	if(GOODNEWS!=*status)
	{
		//xputstring("Timeout reading inverse block number\n");
		return(BADNEWS);
	}

	for(i=0;i<size;++i)
	{
		c=readChar(WAIT_PERIOD,status);
		if(GOODNEWS!=*status)
		{
			//xputstring("Timeout reading packet data byte\n");
			return(BADNEWS);
		}
		else
			block[i]=((UCHAR) (c));
	}

	c=readChar(WAIT_PERIOD,status);
	if(GOODNEWS!=*status)
	{
		//xputstring("Timeout reading CRC #1\n");
		return(BADNEWS);
	}

	rcrc=(unsigned long)(c<<8);
	c=readChar(WAIT_PERIOD,status);
	if(GOODNEWS!=*status)
	{
		//xputstring("Timeout reading CRC #2\n");
		return(BADNEWS);
	}

	rcrc|=c;
	crc=((unsigned long) (calc_crc(block, size)));
	if(((invblockn^0xff)&0xff)!=blockn)
	{
		//xputstring("Block number and its' inverse don't match\n");
		*status=INVBLOCK_ERROR;
		return(BADNEWS);
	}
	if(crc!=rcrc)
	{
		//xputstring("CRC mismatch.\n");
		xputhex (blockn,4);
		xputchar (' ');
		xputhex (size,4);
		*status=CRC_ERROR;
		return(BADNEWS);
	}

	*status = GOODNEWS;

	return(blockn);
}

/*
	Copy incoming file to dataArea
	Doesn't check to see if the area is large enough
	Caters for xmodem 1k (STX) and 128 (SOH)
*/

int xmodem(unsigned char *dataArea, int *nbytes, int MaximumBufferSize)
{
	int		i, j, ok, c;
	unsigned char	blocknum;
	int		bsize;
	int		status;
	unsigned char	ScratchBuffer[2048];	// Used to collect data from the host when the caller specified buffer has overflowed.


	//	toggle_io_pin(GR_LED&SCAN_LED);

	// initialise block pointer
	block=dataArea;

	ok=BADNEWS;
	*nbytes=0;
	bsize=0;
	for(i=0;i<400;i++)
	{
		xputchar('C');
		c=readChar(200,&status);
		if (BADNEWS==status)
		{
			break;
		}
		if(c==SOH)
		{
			bsize=128;
			ok=GOODNEWS;
			break;
		}
		else if(c==STX)
		{
			bsize=1024;
			ok=GOODNEWS;
			break;
		}
		else if(c==ESC)
		{
			return(XMOD_ESC_RECEIVED);
		}


		// Check to see if the buffer will be overrun.  If yes, receive the block to a scratch buffer, send an escape and an
		// error message to the host, and return an error to the caller.
		if((*nbytes + bsize) > MaximumBufferSize)
		{
			block=ScratchBuffer;
			c=readBlock(bsize,&status);
			xputchar(ESC);
			//xputstring("Download buffer overrun\015\012");
			return(DOWNLOAD_BUFFER_OVERRUN);
		}
	}
	if(BADNEWS==ok)
	{
		return(NO_SOHSTX_ERROR);
	}


	blocknum=1;
	for(i=0;i<100;++i)
	{
		c=readBlock(bsize,&status);
		if((GOODNEWS==status) && (c==blocknum))
		{
			i=0;

			/* Put memory writing code here */
			*nbytes+=bsize;
			block=&dataArea[*nbytes];
			blocknum=((UCHAR) ((blocknum+1)&0xff));
			xputchar(ACK);
		}
		else if(GOODNEWS!=status)
		{ /* Empty buffer */

			do
			{
				readChar(WAIT_PERIOD, &status);
			}
			while(GOODNEWS==status);
			xputchar(NAK);
		}
		else if(c<blocknum) // Sent block again
			xputchar(ACK);
		else
		{
			/* We've missed a block */
			for(j=0;j<10;++j)
				xputchar(CAN);
			return(MISSED_BLOCK_ERROR);
		}

		/* Wait for new block, ESC to terminate, or end of Rx (EOT) */
		do
		{
			c=readChar(WAIT_PERIOD,&status);
		}
		while((c!=ESC)&&(c!=SOH)&&(c!=STX)&&(c!=EOT)&&(c!=CAN)&&(status==GOODNEWS));

		if(c==EOT)
		{
			xputchar(ACK);
			//xputstring("\nTransfer successful.\n");
			return(0);
		}
		if(c==SOH)
			bsize=128;
		else if(c==STX)
			bsize=1024;
		else if(c==ESC)
		{
			return(XMOD_ESC_RECEIVED);
		}
		else
		{
			if(GOODNEWS!=status)
			{
				return (NO_SOHSTX_ERROR);
			}
			else
			{
				return(SPURIOUSCHAR_ERROR);
			}
		}

		// Check to see if the buffer will be overrun.  If yes, receive the block to a scratch buffer, send an escape and an
		// error message to the host, and return an error to the caller.
		if((*nbytes + bsize) > MaximumBufferSize)
		{
			block=ScratchBuffer;
			c=readBlock(bsize,&status);
			xputchar(ESC);
			//xputstring("Download buffer overrun\015\012");
			return(DOWNLOAD_BUFFER_OVERRUN);
		}
	}
	//xputstring("Retry limit exceeded.\n");


	return(RETRY_ERROR);
}













int xmodem_transmit(unsigned char *dataArea, int NumberOfBytesToTransmit)
{
	int					HostReadyToReceive;			// Indicates that the host is ready to receive xmodem data.
	int					DownloadTerminatedByHost;	// Indicates that the host has sent an ESC character to terminate the
	//	download.
	int					c;							// Character read from host.
	unsigned int        BlockNum;               	// Next packet to be sent to the scanner.
	unsigned int        crc;                    	// Computed CRC of a packet to be sent to
	//  the scanner.
	unsigned long       OffsetOfNextFileByte;   	// Offset of next byte in download file
	//  that is to be sent to the scanner
	//  as part of an XModem packet.
	unsigned long       i;                      	// Indices
	unsigned char       OutputBuffer[1100];     	// Serial write buffer, for writing data
	//  packets to the scanner.
	unsigned long       BytesRemaining;         	// Number of file bytes remaining to be
	//  sent to the host.
	unsigned long		NewBytesRemaining;			// New number of file bytes remaining to be sent
	//	to the host if the command has been successful.
	unsigned long		BlockSize;					// Size of next block to be sent
	int					FatalDownloadError;     	// Fatal error occurred while trying to send
	//  data to the scanner.





	HostReadyToReceive = FALSE;		// By default, host is assumed to be not ready.
	DownloadTerminatedByHost=FALSE;	// By default, host has not asked for a termination yet.
	FatalDownloadError=FALSE;		// By default, no fatal errors yet.


	// Wait for a response from the host.  'C' means that the host is
	// ready to receive data and ESC means that the host wants to quit.
	// Any other condition is an error.
	set_time_limit(5000);
	while((!time_limit_expired()) && (!HostReadyToReceive) &&
	        (!DownloadTerminatedByHost))
	{
		c = 0;
		c=get_char();
		if(c == 'C')
		{
			HostReadyToReceive = TRUE;
			break;
		}
		else if(c == ESC)
		{
			clear_tx_queue();
			DownloadTerminatedByHost = TRUE;
		}
	}


	// If the host said it's ready to receive, start sending packets.
	if(HostReadyToReceive)
	{
		// First packet being sent.
		BlockNum = 1;
		OffsetOfNextFileByte = 0;
		BytesRemaining = NumberOfBytesToTransmit;

		// While there is still data to send and we haven't encountered a
		// termination condition...
		while((BytesRemaining) && (!DownloadTerminatedByHost) && (!FatalDownloadError))
		{
			// Build next output buffer

			// Zero fill the output buffer so that it will be zero-padded if there is not enough data to make up a full packet.
			for(i=0; i<1024; ++i)
			{
				OutputBuffer[i] = 0;
			}

			// If this is the last block, determine if it should be 128 or 1024 bytes
			// long.  The block has already been padded out with an extra 1024 zeros.
			// Even so, why send 1024 if 128 will do?  The block must be 128 or 1024
			// bytes in length for the scanner
			if(BytesRemaining <= 128)
			{
				BlockSize = 128;
				xputchar(1);
				for(i=0; i<BytesRemaining; ++i)
				{
					OutputBuffer[i] = dataArea[OffsetOfNextFileByte+i];
				}
				NewBytesRemaining = 0;				// Set BytesRemaining to this value if packet send was successful.
				//	Don't modify BytesRemaining directly, because we may have to resend.
			}
			else
			{
				BlockSize = 1024;
				xputchar(2);

				i=0;
				while((i < 1024) && (i < BytesRemaining))
				{
					OutputBuffer[i] = dataArea[OffsetOfNextFileByte+i];
					i++;
				}

				if(BytesRemaining <= 1024)
				{
					NewBytesRemaining = 0;			// Set BytesRemaining to this value if packet send was successful.
					//	Don't modify BytesRemaining directly, because we may have to resend.
				}
				else
				{
					NewBytesRemaining = BytesRemaining - 1024;		// Set BytesRemaining to this value if packet send was
					//	successful.  Don't modify BytesRemaining directly,
					//	because we may have to resend.
				}
			}


			// Check for an ESC and quit if one has been sent.
			if(get_char() == ESC)
			{
				clear_tx_queue();
				DownloadTerminatedByHost = TRUE;
			}


			if(!DownloadTerminatedByHost)
			{
				// Write the block number and inverse.
				xputchar(((UCHAR) (BlockNum & 0xFF)));
				xputchar(((UCHAR) ((~BlockNum) & 0xFF)));


				// Check for an ESC and quit if one has been sent.
				if(get_char() == ESC)
				{
					clear_tx_queue();
					DownloadTerminatedByHost = TRUE;
				}


				// Write the block.
				for(i=0; (i<BlockSize) && (!DownloadTerminatedByHost); ++i)
				{
					xputchar(OutputBuffer[i]);


					// Check for an ESC and quit if one has been sent.
					if(get_char() == ESC)
					{
						clear_tx_queue();
						DownloadTerminatedByHost = TRUE;
					}
				}


				if(!DownloadTerminatedByHost)
				{
					// Build and send the CRC.
					crc=((unsigned long) (calc_crc(OutputBuffer, ((int) BlockSize))));
					crc &= 0xffff;

					xputchar(((UCHAR) ((crc / 256) & 0xFF)));
					xputchar(((UCHAR) (crc & 0xFF)));


					// Wait for acknowledgement from host.
					set_time_limit(5000);
					while((!time_limit_expired()) && (!DownloadTerminatedByHost) && (!FatalDownloadError))
					{
						//						c=readChar(100,&Status);
						c = 0;
						c=get_char();

						// If the scanner ACKed, keep going if this isn't the last packet.  Otherwise...
						if(c == ACK)
						{
							++BlockNum;						// Update to next packet number
							OffsetOfNextFileByte += BlockSize;	// Packet sent successfully, so these counters can be updated.
							BytesRemaining = NewBytesRemaining;

							if(!BytesRemaining)
							{
								// This is the LAST PACKET!!
								// Write ctrl-D (EOT) to tell the host that we're done
								xputchar(EOT);
							}
							break;
						}
						else if(c == ESC)
						{
							clear_tx_queue();
							DownloadTerminatedByHost = TRUE;
						}
						else if(c == NAK)
						{
							break;
						}
					}


					// If the host is unresponsive, notify the user and stop the current download request.
					if(time_limit_expired())
					{
						FatalDownloadError = TRUE;
						//xputstring("XMODEM Transmit->Host failed to respond after packet transmission!\015\012");
					}
				}
			}
		}
	}

	else
	{
		//xputstring("XMODEM Transmit->Host failed to respond with Continue!\015\012");
	}

	return(FatalDownloadError);
}



