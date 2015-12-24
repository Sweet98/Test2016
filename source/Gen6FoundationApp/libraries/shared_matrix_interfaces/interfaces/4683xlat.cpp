/* ==============================================================================
ST3800 IBM-4680 Cash Register Network Command To Scanner Command Translator Source
=================================================================================

   $RCSfile: interfaces/4683xlat.cpp $
   $Revision: 2.20.1.4 $
   $Date: 2011/06/30 15:25:51EDT $

=================================================================================

===============================================================================*/

/**********************************************************
*
*		4683xlat.cpp
*	This module is the IBM-4680 (RS-485) Interface between
*	 the network commands and the scanner commands.  It
*	 translates the commands from the network into menu
*	 commands the scanner will understand.
*
**********************************************************/

#include <stdio.h>
#include "stdInclude.h"
#include "def4683.h"
#include "CommandHandler4683.h"


/****************************************************************************
 *
 *  Function:   Translate4680Command
 *              Process the incoming frame and translate messages to menu
 *              commands if needed
 *
 *  Arguments:  unsigned char * pucCommandString    Pointer to the incoming frame
 *
 * **************************************************************************/
void CommandHandler4683::Translate4680Command(unsigned char *pucCommandString)
{
	const char* pMenu=NULL;
	char Temp[100];

	if ( ACK == *pucCommandString)
	{
		ClearTxBuffer();
		return;
	}

	switch ( *pucCommandString)		/* Sort the scanner command */
	{
		case 0x11 :			/* Enable Scanner */
		{
			EnableScanner4683();
	  		break;
		}
		case 0x12 :			/* Disable Scanner */
  	  	{
  	  		DisableScanner4683();

  	  		break;
  	  	}
  	  	case 0x14 :			/* Enable Beeper */
  	  	{
            sprintf((char *)Temp,"=at:ae;05021D%d!",mBeeperVolume);
            pMenu = (const char *)&Temp[0];
  	  		break;
  	  	}
  	  	case 0x18 :			/* Disable Beeper */
  	  	{
  	  		pMenu = "=at:ae;05021D0!";
  	  		break;
  	  	}
	}		/* Command handling complete! */

	if ( (PORT_9B1_ID == retailPort || PORT_9B2_ID == retailPort)) // && !scannerStatus.IgnoreConfig )
	{
		if (0x21 == *pucCommandString)		/* Sort the Length Limit command */
		{
			int iLengthLimit;
			pucCommandString++;
			iLengthLimit = *pucCommandString;
			if ( iLengthLimit )
			{
				sprintf( (char*)Temp,"=at:ae;020206%2d;020308%2d;020404%2d;020603%2d;020A03%2d;020D03%2d!",
							iLengthLimit,iLengthLimit,iLengthLimit,iLengthLimit,iLengthLimit,iLengthLimit);
				pMenu = (const char*)&Temp[0];
			}
			else
			{
				pMenu = "=at:ae;02020660;02030848;02040480;02060348;020A0380;020D0380!";
			}
		}	/* End of case 'Command 0x21' */
	}

	if ( PORT_9B2_ID == retailPort) // && !scannerStatus.IgnoreConfig)
	{
		if (0x20 == *pucCommandString)		/* Sort the Mode Select command */
		{
			pucCommandString++;		/* Point to the second byte - Mode Selection */
			switch ((*pucCommandString)>>4)
			{
  				case 1 :
  				{
  					pMenu = "=at:ae;0202011;0203010;0204010;0206010;020A010;020D010;      \
  						0211011;0212011;0212021;0213011;0214011;0211050;0211040;0211060;0212070;0212060;      \
  						0212080;0213040;0213030;0213050;0214040;0214030;0214050!";
  					break;
  				}
  		   	case 2 :
  		   	{
  		   		pMenu = "=at:ae;0202010;0203010;0204010;0206010;020A010;020D010;      \
  		   			0211011;0212011;0212021;0213011;0214011;0211050;0211040;0211060;0212070;0212060;      \
  		   			0212080;0213040;0213030;0213050;0214040;0214030;0214050!";
  		   		break;
  		   	}
  		   	case 3 :
  		   	{
  		   		pMenu = "=at:ae;0202010;0203011;0204010;0206010;020A010;020D010;      \
  		   			0211011;0212011;0212021;0213011;0214011;0211050;0211040;0211060;0212070;0212060;      \
  		   			0212080;0213040;0213030;0213050;0214040;0214030;0214050!";
  		   		break;
  		   	}
  		   	case 4 :
  		   	{
  		   		pMenu = "=at:ae;0202010;0203010;0204011;0206010;020A010;020D010;      \
  		   			0211011;0212011;0212021;0213011;0214011;0211050;0211040;0211060;0212070;0212060;      \
  		   			0212080;0213040;0213030;0213050;0214040;0214030;0214050!";
  		   		break;
  		   	}
  		   	case 5 :
  		   	{
  		   		pMenu = "=at:ae;0202010;0203010;0204010;0206010;020A011;020D010;      \
  		   			0211011;0212011;0212021;0213011;0214011;0211050;0211040;0211060;0212070;0212060;      \
  		   			0212080;0213040;0213030;0213050;0214040;0214030;0214050!";
  		   		break;
  		   	}
  		   	case 6 :
  		   	{
  		   		pMenu = "=at:ae;0202010;0203010;0204010;0206010;020A010;020D011;      \
  		   			0211011;0212011;0212021;0213011;0214011;0211050;0211040;0211060;0212070;0212060;      \
  		   			0212080;0213040;0213030;0213050;0214040;0214030;0214050!";
  		   		break;
  		   	}
  		   	case 7 :
  		   	{
  		   		pMenu = "=at:ae;0202010;0203010;0204010;0206010;020A010;020D010;      \
  		   			0211011;0212011;0212021;0213011;0214011;0211051;0211041;0211060;0212071;0212061;      \
  		   			0212080;0213041;0213031;0213050;0214041;0214031;0214050!";
  		   		break;
  		   	}
  		   	case 8 :
  		   	{
  		   		pMenu = "=at:ae;0202010;0203010;0204010;0206011;020A010;020D010;      \
  		   			0211010;0212010;0212020;0213010;0214010;0211050;0211040;0211060;0212070;0212060;      \
  		   			0212080;0213040;0213030;0213050;0214040;0214030;0214050!";
  		   		break;
  		   	}
  		   	case 9 :
  		   	{
  		   		pMenu = "=at:ae;0202010;0203010;0204011;0206010;020A010;020D010;      \
  		   			0211010;0212010;0212020;0213010;0214010;0211050;0211040;0211060;0212070;0212060;      \
  		   			0212080;0213040;0213030;0213050;0214040;0214030;0214050!";
  					break;
  		   	}
  		   	case 0x0A :
  		   	{
  		   		pMenu = "=at:ae;0202010;0203010;0204010;0206010;020A011;020D010;      \
  		   			0211010;0212010;0212020;0213010;0214010;0211050;0211040;0211060;0212070;0212060;      \
  		   			0212080;0213040;0213030;0213050;0214040;0214030;0214050!";
  				  break;
  		   	}
  		   	case 0x0B :
  		   	{
  		   		pMenu = "=at:ae;0202010;0203010;0204010;0206010;020A010;020D011;      \
  		   			0211010;0212010;0212020;0213010;0214010;0211050;0211040;0211060;0212070;0212060;      \
  		   			0212080;0213040;0213030;0213050;0214040;0214030;0214050!";
  		   		break;
  		   	}
  		   	case 0x0C :
  		   	{
  		   		pMenu = "=at:ae;0202010;0203010;0204010;0206010;020A010;020D010;      \
  		   			0211011;0212011;0212021;0213011;0214011;0211051;0211041;0211061;0212071;0212061;      \
  		   			0212081;0213041;0213031;0213051;0214041;0214031;0214051!";
  		   		break;
  		   	}
  		   	case 0 :
  		   	default :
  				{
  		 			pMenu = "=at:ae;0202011;0203011;0204011;0206011;020A011;020D011;      \
  		   			0211011;0212011;0212021;0213011;0214011;0211051;0211041;0211060;0212071;0212061;      \
  		   			0212080;0213041;0213031;0213050;0214041;0214031;0214050!";
  					break;
  				}
			}	/* End of case 'Mode' */
		}	/* End of 'Command 0x20' */
	}	/* End of 'HHBCR2' */
	if (pMenu!=NULL && !m_IgnoreConfigFlag)
	{
		RxFifo->Write((bchar_t *)"\x02m\x0d",(size_t)3);
		RxFifo->Write((bchar_t *)pMenu,(size_t)strlen(pMenu));
	}
	return;
}

/****************************************************************************
 *
 *  Function:   DisableScanner4683
 *              Disable the scanner if IgnoreDisable is off
 *
 * **************************************************************************/
void CommandHandler4683::DisableScanner4683(void)
{
    if ( scannerStatus.IgnoreDisable)        /* if scanner disabled on a reset enable */
         {
            EnableScanner4683();
            return;	// exit here 
         }
  	scannerStatus.ScannerDisabled = 1;

	const char* pMenu;
	unsigned int MenuLen;

  	pMenu = "\x02m\x0d=at:ae;060F0B0!";
	MenuLen = (unsigned int)strlen(pMenu);
	RxFifo->Write((bchar_t *)pMenu,(size_t)MenuLen );

}

/****************************************************************************
 *
 *  Function:   EnableScanner4683
 *              Enable the scanner
 *
 * **************************************************************************/
void CommandHandler4683::EnableScanner4683(void)
{
    scannerStatus.ScannerDisabled = 0;
    scannerStatus.IgnoreDisable = 0;


	const char* pMenu;
	unsigned int MenuLen;

  	pMenu = "\x02m\x0d=at:ae;060F0B1!";
	MenuLen = (unsigned int)strlen(pMenu);
	RxFifo->Write((bchar_t *)pMenu,(size_t)MenuLen );


}


