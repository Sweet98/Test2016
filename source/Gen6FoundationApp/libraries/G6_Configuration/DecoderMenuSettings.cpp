//======================================================================================
// DecoderMenuSettings.cpp
//======================================================================================
// $RCSfile: DecoderMenuSettings.cpp $
// $Revision: 1.93 $
// $Date: 2011/11/10 04:37:33EST $
//======================================================================================

//======================================================================================


//---------------------------------------------------------------------------
//
//  Module Description:
//      This file contains a routine that creates decoder menu settings as part
//		of the overall reader configuration for a Matrix device.
//
//---------------------------------------------------------------------------


#include "stdInclude.h"
#include "ReaderConfiguration.h"
#include "DecoderMenuSettings.h"			// DecoderMenuSettings interface definition
#include "HardwareToken.h"
#ifndef RFBASE
#include "DecoderInterface.h"
#endif

extern bool TerminateSeqBlock(void);
extern bool TerminateNonPreferredBlock(void);
extern bool TerminateCodePreferredBlock(void);

bool DisplayDecoderSetup(void);
bool DisplayDecoderData(void);
bool ResetDecoderWorkspace(void);
bool ChangeDecoderSetting(void);


int PostalCriticalSection = 0;


#ifndef RFBASE
#include "ioman.h"
#include "menu.h"

bool ChangeAusPost(void) {

	if( PostalCriticalSection == 0 ) {
		PostalCriticalSection = 1;
		if( ReaderConfiguration->MenuCommandResponse->Char[0] == '1' ) {
			if( !GET_SETTING(netEnable) &&
				!GET_SETTING(plnEnable) &&
				!GET_SETTING(bpoEnable) &&
				!GET_SETTING(japEnable) &&
				!GET_SETTING(canEnable) &&
				!GET_SETTING(kixEnable) &&
				!GET_SETTING(uspsS18TagEnable) &&
				!GET_SETTING(usps4cbEnable) ) { //invalid combinations

					HSTRING menuc("021D051",true);
					menuc.AppendChar(ReaderConfiguration->CurrentTable);
					ReaderConfiguration->Menu(&menuc);
					PostalCriticalSection = 0;
					return true;
			}
		}
		else {
			HSTRING menuc("021D050",true);
			menuc.AppendChar(ReaderConfiguration->CurrentTable);
			ReaderConfiguration->Menu(&menuc);
			PostalCriticalSection = 0;
			return true;
		}
		PostalCriticalSection = 0;
		return false;
	}
	PostalCriticalSection = 0;
	return true;
}

bool ChangePostnet(void) {

	if( PostalCriticalSection == 0 ) {
		PostalCriticalSection = 1;
		if( !GET_SETTING(ausEnable) &&
			!GET_SETTING(bpoEnable) &&
			!GET_SETTING(japEnable) &&
			!GET_SETTING(canEnable) &&
			!GET_SETTING(kixEnable) ) { //invalid combinations

				HSTRING menuc("021D05",true);
				unsigned int combo = 0;
				if( ReaderConfiguration->MenuCommandResponse->Char[0] == '1' ) {
					combo += DEC_SYMBOLOGY_POSTNET;
				}
				combo = combo +	(GET_SETTING(plnEnable)			*DEC_SYMBOLOGY_PLANETCODE) +
								(GET_SETTING(usps4cbEnable)		*DEC_SYMBOLOGY_USPS_4_STATE) +
								(GET_SETTING(uspsS18TagEnable)	*DEC_SYMBOLOGY_UPU_4_STATE);

				int index = DecodeSet(DEC_POSTAL_ENABLED_DIRECT,(void *)combo);

				if( index >= 0 ) {
					if( index >= 20 ) {
						menuc.AppendChar('2');
						menuc.AppendChar(index+28);
					}
					else if( index >= 10 ) {
						menuc.AppendChar('1');
						menuc.AppendChar(index+38);
					}
					else {
						menuc.AppendChar(index+48);
					}
					menuc.AppendChar(ReaderConfiguration->CurrentTable);
					ReaderConfiguration->Menu(&menuc);
					PostalCriticalSection = 0;
					return true;
				}
		}
		PostalCriticalSection = 0;
		return false;
	}
	PostalCriticalSection = 0;
	return true;
}

bool ChangePlanetCode(void) {

	if( PostalCriticalSection == 0 ) {
		PostalCriticalSection = 1;
		if( !GET_SETTING(ausEnable) &&
			!GET_SETTING(bpoEnable) &&
			!GET_SETTING(japEnable) &&
			!GET_SETTING(canEnable) &&
			!GET_SETTING(kixEnable) ) { //invalid combinations

				HSTRING menuc("021D05",true);
				unsigned int combo = 0;
				if( ReaderConfiguration->MenuCommandResponse->Char[0] == '1' ) {
					combo += DEC_SYMBOLOGY_PLANETCODE;
				}
				combo = combo +	(GET_SETTING(netEnable)			*DEC_SYMBOLOGY_POSTNET) +
								(GET_SETTING(usps4cbEnable)		*DEC_SYMBOLOGY_USPS_4_STATE) +
								(GET_SETTING(uspsS18TagEnable)	*DEC_SYMBOLOGY_UPU_4_STATE);

				int index = DecodeSet(DEC_POSTAL_ENABLED_DIRECT,(void *)combo);

				if( index >= 0 ) {
					if( index >= 20 ) {
						menuc.AppendChar('2');
						menuc.AppendChar(index+28);
					}
					else if( index >= 10 ) {
						menuc.AppendChar('1');
						menuc.AppendChar(index+38);
					}
					else {
						menuc.AppendChar(index+48);
					}
					menuc.AppendChar(ReaderConfiguration->CurrentTable);
					ReaderConfiguration->Menu(&menuc);
					PostalCriticalSection = 0;
					return true;
				}
		}
		PostalCriticalSection = 0;
		return false;
	}
	PostalCriticalSection = 0;
	return true;
}

bool ChangeBritishPost(void) {

	if( PostalCriticalSection == 0 ) {
		PostalCriticalSection = 1;
		if( ReaderConfiguration->MenuCommandResponse->Char[0] == '1' ) {
			if( !GET_SETTING(netEnable) &&
				!GET_SETTING(plnEnable) &&
				!GET_SETTING(ausEnable) &&
				!GET_SETTING(japEnable) &&
				!GET_SETTING(canEnable) &&
				!GET_SETTING(kixEnable) &&
				!GET_SETTING(uspsS18TagEnable) &&
				!GET_SETTING(usps4cbEnable) ) {

					HSTRING menuc("021D057",true);
					menuc.AppendChar(ReaderConfiguration->CurrentTable);
					ReaderConfiguration->Menu(&menuc);
					PostalCriticalSection = 0;
					return true;
			}
		}
		else {
			HSTRING menuc("021D050",true);
			menuc.AppendChar(ReaderConfiguration->CurrentTable);
			ReaderConfiguration->Menu(&menuc);
			PostalCriticalSection = 0;
			return true;
		}
		PostalCriticalSection = 0;
		return false;
	}
	PostalCriticalSection = 0;
	return true;
}

bool ChangeJapanPost(void) {

	if( PostalCriticalSection == 0 ) {
		PostalCriticalSection = 1;
		if( ReaderConfiguration->MenuCommandResponse->Char[0] == '1' ) {
			if( !GET_SETTING(netEnable) &&
				!GET_SETTING(plnEnable) &&
				!GET_SETTING(bpoEnable) &&
				!GET_SETTING(ausEnable) &&
				!GET_SETTING(canEnable) &&
				!GET_SETTING(kixEnable) &&
				!GET_SETTING(uspsS18TagEnable) &&
				!GET_SETTING(usps4cbEnable) ) {

					HSTRING menuc("021D053",true);
					menuc.AppendChar(ReaderConfiguration->CurrentTable);
					ReaderConfiguration->Menu(&menuc);
					PostalCriticalSection = 0;
					PostalCriticalSection = 0;
					return true;
			}
		}
		else {
			HSTRING menuc("021D050",true);
			menuc.AppendChar(ReaderConfiguration->CurrentTable);
			ReaderConfiguration->Menu(&menuc);
			PostalCriticalSection = 0;
			return true;
		}
		PostalCriticalSection = 0;
		return false;
	}
	PostalCriticalSection = 0;
	return true;
}

bool ChangeCanadianPost(void) {

	if( PostalCriticalSection == 0 ) {
		PostalCriticalSection = 1;
		if( (ReaderConfiguration->MenuCommandResponse->Char[0] == '1' ) ||
		    (ReaderConfiguration->MenuCommandResponse->Char[0] == '2' )) {
			if( !GET_SETTING(netEnable) &&
				!GET_SETTING(plnEnable) &&
				!GET_SETTING(bpoEnable) &&
				!GET_SETTING(japEnable) &&
				!GET_SETTING(ausEnable) &&
				!GET_SETTING(kixEnable) &&
				!GET_SETTING(uspsS18TagEnable) &&
				!GET_SETTING(usps4cbEnable) ) {

					HSTRING menuc("021D0530",true); //doesn't have an enable yet.
					menuc.AppendChar(ReaderConfiguration->CurrentTable);
					ReaderConfiguration->Menu(&menuc);
					PostalCriticalSection = 0;
					return true;
			}
		}
		else {
			HSTRING menuc("021D050",true);
			menuc.AppendChar(ReaderConfiguration->CurrentTable);
			ReaderConfiguration->Menu(&menuc);
			PostalCriticalSection = 0;
			return true;
		}
		PostalCriticalSection = 0;
		return false;
	}
	PostalCriticalSection = 0;
	return true;
}

bool ChangeKix(void) {

	if( PostalCriticalSection == 0 ) {
		PostalCriticalSection = 1;
		if( ReaderConfiguration->MenuCommandResponse->Char[0] == '1' ) {
			if( !GET_SETTING(netEnable) &&
				!GET_SETTING(plnEnable) &&
				!GET_SETTING(bpoEnable) &&
				!GET_SETTING(japEnable) &&
				!GET_SETTING(canEnable) &&
				!GET_SETTING(ausEnable) &&
				!GET_SETTING(uspsS18TagEnable) &&
				!GET_SETTING(usps4cbEnable) ) {

					HSTRING menuc("021D054",true);
					menuc.AppendChar(ReaderConfiguration->CurrentTable);
					ReaderConfiguration->Menu(&menuc);
					PostalCriticalSection = 0;
					return true;
			}
		}
		else {
			HSTRING menuc("021D050",true);
			menuc.AppendChar(ReaderConfiguration->CurrentTable);
			ReaderConfiguration->Menu(&menuc);
			PostalCriticalSection = 0;
			return true;
		}
		PostalCriticalSection = 0;
		return false;
	}
	PostalCriticalSection = 0;
	return true;
}

bool ChangeUSPS(void) {

	if( PostalCriticalSection == 0 ) {
		PostalCriticalSection = 1;
		if( !GET_SETTING(ausEnable) &&
			!GET_SETTING(bpoEnable) &&
			!GET_SETTING(japEnable) &&
			!GET_SETTING(canEnable) &&
			!GET_SETTING(kixEnable) ) { //invalid combinations

				HSTRING menuc("021D05",true);
				unsigned int combo = 0;
				if( ReaderConfiguration->MenuCommandResponse->Char[0] == '1' ) {
					combo += DEC_SYMBOLOGY_USPS_4_STATE;
				}
				combo = combo +	(GET_SETTING(plnEnable)			*DEC_SYMBOLOGY_PLANETCODE) +
								(GET_SETTING(netEnable)			*DEC_SYMBOLOGY_POSTNET) +
								(GET_SETTING(uspsS18TagEnable)	*DEC_SYMBOLOGY_UPU_4_STATE);

				int index = DecodeSet(DEC_POSTAL_ENABLED_DIRECT,(void *)combo);

				if( index >= 0 ) {
					if( index >= 20 ) {
						menuc.AppendChar('2');
						menuc.AppendChar(index+28);
					}
					else if( index >= 10 ) {
						menuc.AppendChar('1');
						menuc.AppendChar(index+38);
					}
					else {
						menuc.AppendChar(index+48);
					}
					menuc.AppendChar(ReaderConfiguration->CurrentTable);
					ReaderConfiguration->Menu(&menuc);
					PostalCriticalSection = 0;
					return true;
				}
		}
		PostalCriticalSection = 0;
		return false;
	}
	PostalCriticalSection = 0;
	return true;
}

bool ChangeUPU(void) {

	if( PostalCriticalSection == 0 ) {
		PostalCriticalSection = 1;
		if( !GET_SETTING(ausEnable) &&
			!GET_SETTING(bpoEnable) &&
			!GET_SETTING(japEnable) &&
			!GET_SETTING(canEnable) &&
			!GET_SETTING(kixEnable) ) { //invalid combinations

				HSTRING menuc("021D05",true);
				unsigned int combo = 0;
				if( ReaderConfiguration->MenuCommandResponse->Char[0] == '1' ) {
					combo += DEC_SYMBOLOGY_UPU_4_STATE;
				}
				combo = combo +	(GET_SETTING(plnEnable)			*DEC_SYMBOLOGY_PLANETCODE) +
								(GET_SETTING(netEnable)			*DEC_SYMBOLOGY_POSTNET) +
								(GET_SETTING(usps4cbEnable)		*DEC_SYMBOLOGY_USPS_4_STATE);

				int index = DecodeSet(DEC_POSTAL_ENABLED_DIRECT,(void *)combo);

				if( index >= 0 ) {
					if( index >= 20 ) {
						menuc.AppendChar('2');
						menuc.AppendChar(index+28);
					}
					else if( index >= 10 ) {
						menuc.AppendChar('1');
						menuc.AppendChar(index+38);
					}
					else {
						menuc.AppendChar(index+48);
					}
					menuc.AppendChar(ReaderConfiguration->CurrentTable);
					ReaderConfiguration->Menu(&menuc);
					PostalCriticalSection = 0;
					return true;
				}
		}
		PostalCriticalSection = 0;
		return false;
	}
	PostalCriticalSection = 0;
	return true;
}
#endif

bool ChangePostal( void ) {
	if( PostalCriticalSection == 0 ) {
		PostalCriticalSection = 1;
		HSTRING menuc(20,true);
		int used = 0;
		int value = ReaderConfiguration->MenuCommandResponse->ConvertToNumber(0,&used);
		switch(value) {
			case 1:
				menuc.Copy("0240001");
				menuc.Append(";023D000");
				menuc.Append(";023F000");
				menuc.Append(";023B000");
				menuc.Append(";023A000");
				menuc.Append(";023C000");
				menuc.Append(";023E000");
				menuc.Append(";0241000");
				menuc.Append(";0242000");
				break;
			case 2:
				menuc.Copy("0240000");
				menuc.Append(";023D000");
				menuc.Append(";023F000");
				menuc.Append(";023B000");
				menuc.Append(";023A000");
				menuc.Append(";023C000");
				menuc.Append(";023E000");
				menuc.Append(";0241000");
				menuc.Append(";0242000");
				return true;
			case 3:
				menuc.Copy("0240000");
				menuc.Append(";023D001");
				menuc.Append(";023F000");
				menuc.Append(";023B000");
				menuc.Append(";023A000");
				menuc.Append(";023C000");
				menuc.Append(";023E000");
				menuc.Append(";0241000");
				menuc.Append(";0242000");
				break;
			case 4:
				menuc.Copy("0240000");
				menuc.Append(";023D000");
				menuc.Append(";023F001");
				menuc.Append(";023B000");
				menuc.Append(";023A000");
				menuc.Append(";023C000");
				menuc.Append(";023E000");
				menuc.Append(";0241000");
				menuc.Append(";0242000");
				break;
			case 5:
				menuc.Copy("0240000");
				menuc.Append(";023D000");
				menuc.Append(";023F000");
				menuc.Append(";023B001");
				menuc.Append(";023A000");
				menuc.Append(";023C000");
				menuc.Append(";023E000");
				menuc.Append(";0241000");
				menuc.Append(";0242000");
				break;
			case 6:
				menuc.Copy("0240000");
				menuc.Append(";023D000");
				menuc.Append(";023F000");
				menuc.Append(";023B000");
				menuc.Append(";023A001");
				menuc.Append(";023C000");
				menuc.Append(";023E000");
				menuc.Append(";0241000");
				menuc.Append(";0242000");
				break;
			case 7:
				menuc.Copy("0240000");
				menuc.Append(";023D000");
				menuc.Append(";023F000");
				menuc.Append(";023B000");
				menuc.Append(";023A000");
				menuc.Append(";023C001");
				menuc.Append(";023E000");
				menuc.Append(";0241000");
				menuc.Append(";0242000");
				break;
			case 8:
				menuc.Copy("0240000");
				menuc.Append(";023D000");
				menuc.Append(";023F000");
				menuc.Append(";023B000");
				menuc.Append(";023A000");
				menuc.Append(";023C001");
				menuc.Append(";023E000");
				menuc.Append(";0241000");
				menuc.Append(";0242000");
				break;
			case 9:
				menuc.Copy("0240000");
				menuc.Append(";023D000");
				menuc.Append(";023F000");
				menuc.Append(";023B000");
				menuc.Append(";023A000");
				menuc.Append(";023C000");
				menuc.Append(";023E000");
				menuc.Append(";0241001");
				menuc.Append(";0242000");
				break;
			case 10:
				menuc.Copy("0240000");
				menuc.Append(";023D000");
				menuc.Append(";023F000");
				menuc.Append(";023B000");
				menuc.Append(";023A000");
				menuc.Append(";023C000");
				menuc.Append(";023E000");
				menuc.Append(";0241000");
				menuc.Append(";0242001");
				break;
			case 11:
				menuc.Copy("0240000");
				menuc.Append(";023D000");
				menuc.Append(";023F000");
				menuc.Append(";023B000");
				menuc.Append(";023A001");
				menuc.Append(";023C000");
				menuc.Append(";023E000");
				menuc.Append(";0241000");
				menuc.Append(";0242000");
				break;
			case 12:
				menuc.Copy("0240000");
				menuc.Append(";023D000");
				menuc.Append(";023F000");
				menuc.Append(";023B001");
				menuc.Append(";023A001");
				menuc.Append(";023C000");
				menuc.Append(";023E000");
				menuc.Append(";0241000");
				menuc.Append(";0242000");
				break;
			case 13:
				menuc.Copy("0240000");
				menuc.Append(";023D000");
				menuc.Append(";023F000");
				menuc.Append(";023B001");
				menuc.Append(";023A000");
				menuc.Append(";023C000");
				menuc.Append(";023E000");
				menuc.Append(";0241001");
				menuc.Append(";0242000");
				break;
			case 14:
				menuc.Copy("0240000");
				menuc.Append(";023D000");
				menuc.Append(";023F000");
				menuc.Append(";023B000");
				menuc.Append(";023A001");
				menuc.Append(";023C000");
				menuc.Append(";023E000");
				menuc.Append(";0241001");
				menuc.Append(";0242000");
				break;
			case 15:
				menuc.Copy("0240000");
				menuc.Append(";023D000");
				menuc.Append(";023F000");
				menuc.Append(";023B001");
				menuc.Append(";023A000");
				menuc.Append(";023C000");
				menuc.Append(";023E000");
				menuc.Append(";0241000");
				menuc.Append(";0242001");
				break;
			case 16:
				menuc.Copy("0240000");
				menuc.Append(";023D000");
				menuc.Append(";023F000");
				menuc.Append(";023B000");
				menuc.Append(";023A001");
				menuc.Append(";023C000");
				menuc.Append(";023E000");
				menuc.Append(";0241000");
				menuc.Append(";0242001");
				break;
			case 17:
				menuc.Copy("0240000");
				menuc.Append(";023D000");
				menuc.Append(";023F000");
				menuc.Append(";023B000");
				menuc.Append(";023A000");
				menuc.Append(";023C000");
				menuc.Append(";023E000");
				menuc.Append(";0241001");
				menuc.Append(";0242001");
				break;
			case 18:
				menuc.Copy("0240000");
				menuc.Append(";023D000");
				menuc.Append(";023F000");
				menuc.Append(";023B001");
				menuc.Append(";023A001");
				menuc.Append(";023C000");
				menuc.Append(";023E000");
				menuc.Append(";0241000");
				menuc.Append(";0242000");
				break;
			case 19:
				menuc.Copy("0240000");
				menuc.Append(";023D000");
				menuc.Append(";023F000");
				menuc.Append(";023B000");
				menuc.Append(";023A001");
				menuc.Append(";023C000");
				menuc.Append(";023E000");
				menuc.Append(";0241001");
				menuc.Append(";0242000");
				break;
			case 20:
				menuc.Copy("0240000");
				menuc.Append(";023D000");
				menuc.Append(";023F000");
				menuc.Append(";023B000");
				menuc.Append(";023A001");
				menuc.Append(";023C000");
				menuc.Append(";023E000");
				menuc.Append(";0241000");
				menuc.Append(";0242001");
				break;
			case 21:
				menuc.Copy("0240000");
				menuc.Append(";023D000");
				menuc.Append(";023F000");
				menuc.Append(";023B001");
				menuc.Append(";023A001");
				menuc.Append(";023C000");
				menuc.Append(";023E000");
				menuc.Append(";0241001");
				menuc.Append(";0242000");
				break;
			case 22:
				menuc.Copy("0240000");
				menuc.Append(";023D000");
				menuc.Append(";023F000");
				menuc.Append(";023B001");
				menuc.Append(";023A001");
				menuc.Append(";023C000");
				menuc.Append(";023E000");
				menuc.Append(";0241000");
				menuc.Append(";0242001");
				break;
			case 23:
				menuc.Copy("0240000");
				menuc.Append(";023D000");
				menuc.Append(";023F000");
				menuc.Append(";023B001");
				menuc.Append(";023A000");
				menuc.Append(";023C000");
				menuc.Append(";023E000");
				menuc.Append(";0241001");
				menuc.Append(";0242001");
				break;
			case 24:
				menuc.Copy("0240000");
				menuc.Append(";023D000");
				menuc.Append(";023F000");
				menuc.Append(";023B000");
				menuc.Append(";023A001");
				menuc.Append(";023C000");
				menuc.Append(";023E000");
				menuc.Append(";0241001");
				menuc.Append(";0242001");
				break;
			case 25:
				menuc.Copy("0240000");
				menuc.Append(";023D000");
				menuc.Append(";023F000");
				menuc.Append(";023B001");
				menuc.Append(";023A001");
				menuc.Append(";023C000");
				menuc.Append(";023E000");
				menuc.Append(";0241001");
				menuc.Append(";0242000");
				break;
			case 26:
				menuc.Copy("0240000");
				menuc.Append(";023D000");
				menuc.Append(";023F000");
				menuc.Append(";023B001");
				menuc.Append(";023A001");
				menuc.Append(";023C000");
				menuc.Append(";023E000");
				menuc.Append(";0241000");
				menuc.Append(";0242001");
				break;
			case 27:
				menuc.Copy("0240000");
				menuc.Append(";023D000");
				menuc.Append(";023F000");
				menuc.Append(";023B000");
				menuc.Append(";023A001");
				menuc.Append(";023C000");
				menuc.Append(";023E000");
				menuc.Append(";0241001");
				menuc.Append(";0242001");
				break;
			case 28:
				menuc.Copy("0240000");
				menuc.Append(";023D000");
				menuc.Append(";023F000");
				menuc.Append(";023B001");
				menuc.Append(";023A001");
				menuc.Append(";023C000");
				menuc.Append(";023E000");
				menuc.Append(";0241001");
				menuc.Append(";0242001");
				break;
			case 29:
				menuc.Copy("0240000");
				menuc.Append(";023D000");
				menuc.Append(";023F000");
				menuc.Append(";023B001");
				menuc.Append(";023A001");
				menuc.Append(";023C000");
				menuc.Append(";023E000");
				menuc.Append(";0241001");
				menuc.Append(";0242001");
				break;
			case 30:
				menuc.Copy("0240000");
				menuc.Append(";023D000");
				menuc.Append(";023F000");
				menuc.Append(";023B000");
				menuc.Append(";023A000");
				menuc.Append(";023C000");
				menuc.Append(";023E001");
				menuc.Append(";0241000");
				menuc.Append(";0242000");
				break;
			default:
				menuc.Copy("0240000");
				menuc.Append(";023D000");
				menuc.Append(";023F000");
				menuc.Append(";023B000");
				menuc.Append(";023A000");
				menuc.Append(";023C000");
				menuc.Append(";023E000");
				menuc.Append(";0241000");
				menuc.Append(";0242000");
				break;
		}

		menuc.AppendChar(ReaderConfiguration->CurrentTable);
		ReaderConfiguration->Menu(&menuc);
	}
	PostalCriticalSection = 0;
	return true;
}

#ifndef RFBASE
#define MCR ReaderConfiguration->MenuCommandResponse
unsigned char parsedOCRTemplate[SIZE_OF_OCR_TEMPLATE + 1];

#define OCR_DEBUG 0

#if OCR_DEBUG
	#include "db_chrio.h"
	#include "ioman.h"
	#define OCPt(format, ... ) do {theCurrentInterface.printf(format, ## __VA_ARGS__); wait_til_tx_is_empty();} while (0)
#else
	#define OCPt(format, ... ) do{}while(0)
#endif

bool ParseOCRTemplate(void) {
	bool ret = TRUE;
	int menuIndex = 0;
	int menuStop = 0;
	int templateIndex = 0;
	int numUsed = 0;
	HSTRING *testString;

	memset(parsedOCRTemplate,0,SIZE_OF_OCR_TEMPLATE);

	if( MCR->Char[0] == 0x22 ) { //if quotes to not start this string
		testString = new HSTRING(MCR);
		menuStop = MCR->Size-1;
	}
	else {
		testString = new HSTRING(MCR->Size/2);
		menuIndex = -1;
		int jkc = 0;
		for( int jk = 0; jk<MCR->Size; ) {
			testString->Char[jkc++] = MCR->ConvertHexToNumber(jk, jk+1, &numUsed);
			OCPt("ts[%d]:%c \r\n",jkc,testString->Char[jkc-1]);
			menuStop++;
			jk += numUsed;
		}
		testString->Size = jkc-1;
	}

	OCPt("testString:%s-%d\r\n\r\n",testString->Char,testString->Size); //string to be tested

	while( ret && ++menuIndex < menuStop ) {
		if( testString->Char[menuIndex] == 'x' || testString->Char[menuIndex] == 'X' ) {
			parsedOCRTemplate[templateIndex++] = testString->ConvertHexToNumber(menuIndex+1,menuIndex+2,&numUsed);
			OCPt("ASCII:%c%c%c-%d\r\n",testString->Char[menuIndex],testString->Char[menuIndex+1],testString->Char[menuIndex+2],parsedOCRTemplate[templateIndex-1]);
			menuIndex += numUsed;
		}
		else {
			parsedOCRTemplate[templateIndex++] = (char)testString->ConvertHexToNumber(menuIndex,menuIndex,&numUsed); //this should only be each control code
			OCPt("control:%c-%d\r\n",testString->Char[menuIndex],parsedOCRTemplate[templateIndex-1]);

			switch( testString->Char[menuIndex] ) {
				case '0': break;	//End template
				case '1':			//New template
					menuIndex += numUsed;
					parsedOCRTemplate[templateIndex++] = (char)testString->ConvertHexToNumber(menuIndex,menuIndex,&numUsed); //font
					OCPt(" 1:%c-%d\r\n", testString->Char[menuIndex],parsedOCRTemplate[templateIndex-1]);
					break;
				case '2': break;	//New line
				case '3':			//Define group start ID[001-255]
					menuIndex += numUsed;
					parsedOCRTemplate[templateIndex++] = testString->ConvertToNumber(menuIndex,menuIndex+2,&numUsed); //3 digit ID
					OCPt(" 3:%c%c%c-%d  ID\r\n",testString->Char[menuIndex],testString->Char[menuIndex+1],testString->Char[menuIndex+2],parsedOCRTemplate[templateIndex-1]);
					menuIndex += numUsed;
					while( ret && testString->Char[menuIndex] != '4' ) { //put in each until the end group marker
						OCPt("group:%c\r\n",testString->Char[menuIndex]);
						if( testString->Char[menuIndex] >= '5' && testString->Char[menuIndex] <= '8' ) {
							parsedOCRTemplate[templateIndex++] = testString->ConvertToNumber(menuIndex,menuIndex,&numUsed);
							OCPt(" 3:%c-%d  Wildcard\r\n",testString->Char[menuIndex],parsedOCRTemplate[templateIndex-1]);
						}
						else if( testString->Char[menuIndex] == 'x' || testString->Char[menuIndex] == 'X' ) {
							parsedOCRTemplate[templateIndex++] = testString->ConvertHexToNumber(menuIndex+1,menuIndex+2,&numUsed);
							OCPt(" 3:%c-%d  ASCII\r\n",testString->Char[menuIndex],parsedOCRTemplate[templateIndex-1]);
							menuIndex++;
						}
						else {
							ret=false;
						}
						menuIndex += numUsed;
					}
					parsedOCRTemplate[templateIndex++] = 4; //mark the end of the group
				case '4': break;	//Define group End
				case '5': break;	//Wildcard: Numeric
				case '6': break;	//Wildcard: Alpha
				case '7': break;	//Wildcard: Alphanumeric
				case '8': break;	//Wildcard: Any
				case '9':			//9 is the only other value and has no meaning
					ret = false; break;
				case 'a':
				case 'A':			//Use defined group ID[001-255]
					menuIndex += numUsed;
					parsedOCRTemplate[templateIndex++] = testString->ConvertToNumber(menuIndex,menuIndex+2,&numUsed); //3 digit ID
					OCPt(" A:%c%c%c-%d\r\n",testString->Char[menuIndex],testString->Char[menuIndex+1],testString->Char[menuIndex+2],parsedOCRTemplate[templateIndex-1]);
					menuIndex += (numUsed-1);
					break;
				case 'b':
				case 'B':			//In-line group start
					menuIndex += numUsed;
					while( testString->Char[menuIndex++] != 'C' ) { //put in each until the end group marker
						if( testString->Char[menuIndex] >= '5' && testString->Char[menuIndex] <= '8' ) {
							parsedOCRTemplate[templateIndex++] = testString->ConvertToNumber(menuIndex,menuIndex,&numUsed);
							OCPt(" B:%c-%d  Wildcard\r\n",testString->Char[menuIndex],parsedOCRTemplate[templateIndex-1]);
						}
						else {
							parsedOCRTemplate[templateIndex++] = testString->Char[menuIndex];
							OCPt(" B:%c-%d  ASCII\r\n",testString->Char[menuIndex],parsedOCRTemplate[templateIndex-1]);
						}
					}
					parsedOCRTemplate[templateIndex++] = 12;
				case 'c':
				case 'C': break;	//In-line group end
				case 'd':
				case 'D':			//Checksum
					menuIndex += numUsed;
					parsedOCRTemplate[templateIndex++] = testString->ConvertHexToNumber(menuIndex,menuIndex+1,&numUsed);
					OCPt(" D:%c%c-%d\r\n",testString->Char[menuIndex],testString->Char[menuIndex+1],parsedOCRTemplate[templateIndex-1]);
					menuIndex += (numUsed-1);
					break;
				case 'e':
				case 'E':			//Fixed repeat
					menuIndex += numUsed;
					parsedOCRTemplate[templateIndex++] = testString->ConvertToNumber(menuIndex,menuIndex+1,&numUsed); //2 digit repeat number
					OCPt(" E:%c%c-%d\r\n",testString->Char[menuIndex],testString->Char[menuIndex+1],parsedOCRTemplate[templateIndex-1]);
					menuIndex += (numUsed-1);
					break;
				case 'f':
				case 'F':
					menuIndex += numUsed;
					parsedOCRTemplate[templateIndex++] = testString->ConvertToNumber(menuIndex,menuIndex+1,&numUsed); //2 digit min repeat
					OCPt(" F:%c%c-%d  min\r\n",testString->Char[menuIndex],testString->Char[menuIndex+1],parsedOCRTemplate[templateIndex-1]);
					menuIndex += numUsed;
					parsedOCRTemplate[templateIndex++] = testString->ConvertToNumber(menuIndex,menuIndex+1,&numUsed); //2 digit max repeat
					OCPt(" F:%c%c-%d  max\r\n",testString->Char[menuIndex],testString->Char[menuIndex+1],parsedOCRTemplate[templateIndex-1]);
					menuIndex += (numUsed-1);
					break;
				default: //Any other char should be fine, as an ASCII code
					break;
			}//switch(char)
		}//else
	}//while
	if( ret ) {
		OCPt( "Parse good, trying to set template: ");
		for( int jk=0; jk<templateIndex; jk++ ) {
			OCPt("%d,",parsedOCRTemplate[jk]);
		}
		OCPt("\r\n");
		ret = DecodeSet(DEC_OCR_TEMPLATE,(void *)parsedOCRTemplate);
		OCPt("decode return:%d \r\n",ret);

		if( ret == 0 ) {
			if( GetLastDecoderError() == DEC_ERR_INVALID_HANDLE ) { //needed because at system start, the decoder is not established
				ret = true;											//so this prevents the revert of the menu command to default. NG2D-604
			}
			OCPt("error:%d\r\n",GetLastDecoderError());
		}
		else {
			ret = true;
		}
	}
	delete( testString );
	return ret;
}

#endif


void READER_CONFIGURATION::CreateDecoderMenuSettings( int ControlMask, UINT BC412SymbologyCommandModifier, UINT CodeZSymbologyCommandModifier,
        UINT DpcCirrusSymbologyCommandModifier, UINT SecureCodeSymbologyCommandModifier, UINT Two_DCommandModifier,
        UINT VericodeSymbologyCommandModifier, UINT GmxMatrixSymbologyCommandModifier, UINT LinearSymbologyCommandModifier )
{
	AddCommand("020100", SUB_TAG_TABLE::FULLY_VISIBLE | SUB_TAG_TABLE::RETAIN_EXTRANEOUS_TEXT_AFTER_COMMAND, NULL, NULL,
	           SUB_TAG_TABLE::ENABLE_SYMBOLOGIES);


	//***************************************************************************************************************************
	/*Linear Decoders*/


	//	Codabar Settings
	AddCommand("020200", LinearSymbologyCommandModifier | WORKGROUP, NULL, NULL, SUB_TAG_TABLE::SET_WORKING_AND_ACTIVE_VALUES_TO_DEFAULT_FOR_THIS_TAG);


	if(ControlMask & MAKE_LINEAR_SYMBOLOGIES_AVAILABLE)
		AddBoolean("020201", SUB_TAG_TABLE::FULLY_VISIBLE | SUB_TAG_TABLE::SYMBOLOGY_ENABLE_COMMAND | WORKGROUP, NULL, NULL, &CbrEnable, true, true);
	else
		AddBoolean("020201", LinearSymbologyCommandModifier | WORKGROUP, NULL, NULL, &CbrEnable, false, false);


	AddBoolean("020202", LinearSymbologyCommandModifier | WORKGROUP, NULL, NULL, &CbrSSXmit, false, false);

	AddNumericList("020203", LinearSymbologyCommandModifier | WORKGROUP, NULL, NULL, &CbrChkChar, CHK_NONE, CHK_NONE,
	               CHK_NONE, CHK_REQ, CHK_REQ_XMIT);


	AddNumericList("020204", LinearSymbologyCommandModifier | WORKGROUP, NULL, NULL, &CbrConCat, CONCAT_DISABLED,  CONCAT_DISABLED,
	               CONCAT_DISABLED, CONCAT_ENABLED, CONCAT_REQUIRED);


	AddNumber("020205", LinearSymbologyCommandModifier | WORKGROUP, NULL, NULL, &CbrMinLength, 4, 4, 2, 60, &CbrMaxLength);


	AddNumber("020206", LinearSymbologyCommandModifier | WORKGROUP, NULL, NULL, &CbrMaxLength, 60, 60, 2, &CbrMinLength, 60);


	//	Code 39 Settings
	AddCommand("020300", LinearSymbologyCommandModifier | WORKGROUP, NULL, NULL, SUB_TAG_TABLE::SET_WORKING_AND_ACTIVE_VALUES_TO_DEFAULT_FOR_THIS_TAG);


	if(ControlMask & MAKE_LINEAR_SYMBOLOGIES_AVAILABLE)
		AddBoolean("020301", SUB_TAG_TABLE::FULLY_VISIBLE | SUB_TAG_TABLE::SYMBOLOGY_ENABLE_COMMAND | WORKGROUP, NULL, NULL, &C39Enable, true, true);
	else
		AddBoolean("020301", LinearSymbologyCommandModifier | WORKGROUP, NULL, NULL, &C39Enable, false, false);


	AddBoolean("020302", LinearSymbologyCommandModifier | WORKGROUP, NULL, NULL, &C39FullAscii, false, false);
	AddBoolean("020303", LinearSymbologyCommandModifier | WORKGROUP, NULL, NULL, &C39Append, false, false);

	AddNumericList("020304", LinearSymbologyCommandModifier | WORKGROUP, NULL, NULL, &C39ChkChar, CHK_NONE, CHK_NONE,
	               CHK_NONE, CHK_REQ, CHK_REQ_XMIT);

	AddBoolean("020305", LinearSymbologyCommandModifier | WORKGROUP, NULL, NULL, &C39SSXmit, false, false);
	AddBoolean("020306", LinearSymbologyCommandModifier | WORKGROUP, NULL, NULL, &C39Base32, false, false);

	AddNumber("020307", LinearSymbologyCommandModifier | WORKGROUP, NULL, NULL, &C39MinLength, 0, 0, 0, 48, &C39MaxLength);
	AddNumber("020308", LinearSymbologyCommandModifier | WORKGROUP, NULL, NULL, &C39MaxLength, 48, 48, 0, &C39MinLength, 48);
	AddNumericList("020309", LinearSymbologyCommandModifier | WORKGROUP, NULL, NULL, &C39DfltCodePage, 2, 2, 2, 2);


	//	Interleaved 2 of 5 Settings
	AddCommand("020400", LinearSymbologyCommandModifier | WORKGROUP, NULL, NULL, SUB_TAG_TABLE::SET_WORKING_AND_ACTIVE_VALUES_TO_DEFAULT_FOR_THIS_TAG);


	if(ControlMask & MAKE_LINEAR_SYMBOLOGIES_AVAILABLE)
		AddBoolean("020401", SUB_TAG_TABLE::FULLY_VISIBLE | SUB_TAG_TABLE::SYMBOLOGY_ENABLE_COMMAND | WORKGROUP, NULL, NULL, &I25Enable, true, true);
	else
		AddBoolean("020401", LinearSymbologyCommandModifier | WORKGROUP, NULL, NULL, &I25Enable, false, false);

	AddNumericList("020402", LinearSymbologyCommandModifier | WORKGROUP, NULL, NULL, &I25ChkChar, CHK_NONE, CHK_NONE,
	               CHK_NONE, CHK_REQ, CHK_REQ_XMIT, CHK_REQ_W49M10, CHK_REQ_XMIT_W49M10);


	AddNumber("020403", LinearSymbologyCommandModifier | WORKGROUP, NULL, NULL, &I25MinLength, 4, 4, 2, 80, &I25MaxLength);


	AddNumber("020404", LinearSymbologyCommandModifier | WORKGROUP, NULL, NULL, &I25MaxLength, 80, 80, 2, &I25MinLength, 80);

#if (DECODER_ID) || defined(RFBASE)

	AddCommand("020500", LinearSymbologyCommandModifier | WORKGROUP, NULL, NULL, SUB_TAG_TABLE::SET_WORKING_AND_ACTIVE_VALUES_TO_DEFAULT_FOR_THIS_TAG);

	if(ControlMask & MAKE_LINEAR_SYMBOLOGIES_AVAILABLE)
		AddBoolean("020501", SUB_TAG_TABLE::FULLY_VISIBLE | SUB_TAG_TABLE::SYMBOLOGY_ENABLE_COMMAND | WORKGROUP, NULL, NULL, &N25Enable, true, true);
	else
		AddBoolean("020501", LinearSymbologyCommandModifier | WORKGROUP, NULL, NULL, &N25Enable, false, false);

	AddNumericList("020502", LinearSymbologyCommandModifier | WORKGROUP, NULL, NULL, &N25ChkChar, CHK_NONE, CHK_NONE,
	               CHK_NONE, CHK_REQ, CHK_REQ_XMIT);


	AddNumber("020503", LinearSymbologyCommandModifier | WORKGROUP, NULL, NULL, &N25MinLength, 4, 4, 2, 80, &N25MaxLength);


	AddNumber("020504", LinearSymbologyCommandModifier | WORKGROUP, NULL, NULL, &N25MaxLength, 80, 80, 2, &N25MinLength, 80);
#endif

	//	Industrial Code 2 of 5 Settings
	AddCommand("020600", LinearSymbologyCommandModifier | WORKGROUP, NULL, NULL, SUB_TAG_TABLE::SET_WORKING_AND_ACTIVE_VALUES_TO_DEFAULT_FOR_THIS_TAG);
	if(ControlMask & MAKE_LINEAR_SYMBOLOGIES_AVAILABLE)
		AddBoolean("020601", SUB_TAG_TABLE::FULLY_VISIBLE | SUB_TAG_TABLE::SYMBOLOGY_ENABLE_COMMAND | WORKGROUP, NULL, NULL, &R25Enable, false, false);
	else
		AddBoolean("020601", LinearSymbologyCommandModifier | WORKGROUP, NULL, NULL, &R25Enable, false, false);

	AddNumber("020602", LinearSymbologyCommandModifier | WORKGROUP, NULL, NULL, &R25MinLength, 4, 4, 1, 48, &R25MaxLength);
	AddNumber("020603", LinearSymbologyCommandModifier | WORKGROUP, NULL, NULL, &R25MaxLength, 48, 48, 1, &R25MinLength, 48);


	//	IATA Code 2 of 5 Settings
	AddCommand("020700", LinearSymbologyCommandModifier | WORKGROUP, NULL, NULL, SUB_TAG_TABLE::SET_WORKING_AND_ACTIVE_VALUES_TO_DEFAULT_FOR_THIS_TAG);
	if(ControlMask & MAKE_LINEAR_SYMBOLOGIES_AVAILABLE)
		AddBoolean("020701", SUB_TAG_TABLE::FULLY_VISIBLE | SUB_TAG_TABLE::SYMBOLOGY_ENABLE_COMMAND | WORKGROUP, NULL, NULL, &A25Enable, false, false);
	else
		AddBoolean("020701", LinearSymbologyCommandModifier | WORKGROUP, NULL, NULL, &A25Enable, false, false);

	AddNumber("020702", LinearSymbologyCommandModifier | WORKGROUP, NULL, NULL, &A25MinLength, 4, 4, 1, 48, &A25MaxLength);
	AddNumber("020703", LinearSymbologyCommandModifier | WORKGROUP, NULL, NULL, &A25MaxLength, 48, 48, 1, &A25MinLength, 48);

	//	Matrix 2 of 5 Settings
	AddCommand("020800", LinearSymbologyCommandModifier | WORKGROUP, NULL, NULL, SUB_TAG_TABLE::SET_WORKING_AND_ACTIVE_VALUES_TO_DEFAULT_FOR_THIS_TAG);
	if(ControlMask & MAKE_LINEAR_SYMBOLOGIES_AVAILABLE)
		AddBoolean("020801", SUB_TAG_TABLE::FULLY_VISIBLE | SUB_TAG_TABLE::SYMBOLOGY_ENABLE_COMMAND | WORKGROUP, NULL, NULL, &X25Enable, false, false);
	else
		AddBoolean("020801", LinearSymbologyCommandModifier | WORKGROUP, NULL, NULL, &X25Enable, false, false);

	AddNumber("020802", LinearSymbologyCommandModifier | WORKGROUP, NULL, NULL, &X25MinLength, 4, 4, 1, 80, &X25MaxLength);
	AddNumber("020803", LinearSymbologyCommandModifier | WORKGROUP, NULL, NULL, &X25MaxLength, 80, 80, 1, &X25MinLength, 80);

	//	Code 11 Settings
	AddCommand("020900", LinearSymbologyCommandModifier | WORKGROUP, NULL, NULL, SUB_TAG_TABLE::SET_WORKING_AND_ACTIVE_VALUES_TO_DEFAULT_FOR_THIS_TAG);


	if(ControlMask & MAKE_LINEAR_SYMBOLOGIES_AVAILABLE)
		AddBoolean("020901", SUB_TAG_TABLE::FULLY_VISIBLE | SUB_TAG_TABLE::SYMBOLOGY_ENABLE_COMMAND | WORKGROUP, NULL, NULL, &C11Enable, false, false);
	else
		AddBoolean("020901", LinearSymbologyCommandModifier | WORKGROUP, NULL, NULL, &C11Enable, false, false);



	AddNumber("020902", LinearSymbologyCommandModifier | WORKGROUP, NULL, NULL, &C11MinLength, 4, 4, 1, 80, &C11MaxLength);
	AddNumber("020903", LinearSymbologyCommandModifier | WORKGROUP, NULL, NULL, &C11MaxLength, 80, 80, 1, &C11MinLength, 80);

	AddBoolean("020904", LinearSymbologyCommandModifier | WORKGROUP, NULL, NULL, &C11DoubleCheck, true, true);



	//	Code 128 Settings
	AddCommand("020A00", LinearSymbologyCommandModifier | WORKGROUP, NULL, NULL, SUB_TAG_TABLE::SET_WORKING_AND_ACTIVE_VALUES_TO_DEFAULT_FOR_THIS_TAG);

	if(ControlMask & MAKE_LINEAR_SYMBOLOGIES_AVAILABLE){
		AddBoolean("020A01", SUB_TAG_TABLE::FULLY_VISIBLE | SUB_TAG_TABLE::SYMBOLOGY_ENABLE_COMMAND | WORKGROUP, NULL, NULL, &C128Enable, true, true);
		AddBoolean("020B00", SUB_TAG_TABLE::FULLY_VISIBLE | SUB_TAG_TABLE::SYMBOLOGY_ENABLE_COMMAND | WORKGROUP, NULL, NULL, &G128Enable, true, true); }
	else{
		AddBoolean("020A01", LinearSymbologyCommandModifier | WORKGROUP, NULL, NULL, &C128Enable, false, false);
		AddBoolean("020B00", LinearSymbologyCommandModifier | WORKGROUP, NULL, NULL, &G128Enable, false, false);}
	//AddBoolean("GS1ENA", NULL | WORKGROUP, NULL, NULL, &G128Enable, false, false);

	AddCommand("020B01", LinearSymbologyCommandModifier | WORKGROUP, NULL, NULL, SUB_TAG_TABLE::SET_WORKING_AND_ACTIVE_VALUES_TO_DEFAULT_FOR_THIS_TAG);

	AddNumber("020A02", LinearSymbologyCommandModifier | WORKGROUP, NULL, NULL, &C128MinLength, 0, 0, 0, 80, &C128MaxLength);  // **** 80 is greatest min???
	AddNumber("020A03", LinearSymbologyCommandModifier | WORKGROUP, NULL, NULL, &C128MaxLength, 80, 80, 0, &C128MinLength, 90);
	AddNumber("020B02", LinearSymbologyCommandModifier | WORKGROUP, NULL, NULL, &G128MinLength, 0, 0, 0, 80, &G128MaxLength);  // **** 80 is greatest min???
	AddNumber("020B03", LinearSymbologyCommandModifier | WORKGROUP, NULL, NULL, &G128MaxLength, 80, 80, 0, &G128MinLength, 90);
	AddBoolean("020C00", LinearSymbologyCommandModifier | WORKGROUP, NULL, NULL, &C128ISBTEnabled, false, false);
	AddBoolean("020A04", LinearSymbologyCommandModifier | WORKGROUP , NULL, NULL, &C128ShowFNCs, false, false);
	if(ControlMask & MAKE_LINEAR_SYMBOLOGIES_AVAILABLE)
		AddNumber("020A05", SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG | WORKGROUP, NULL, NULL, &C128CharSubFunc1, 29, 29, 1, 128);
	else
		AddNumber("020A05", LinearSymbologyCommandModifier | WORKGROUP, NULL, NULL, &C128CharSubFunc1, 29, 29, 1, 128);
	AddNumericList("020A06", LinearSymbologyCommandModifier | WORKGROUP, NULL, NULL, &C128DfltCodePage, 2, 2, 2, 2);

#if (DECODER_AD)
	if(ControlMask & MAKE_LINEAR_SYMBOLOGIES_AVAILABLE)
		AddBoolean("020A07", SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG | WORKGROUP , NULL, NULL, &C128FarReading, false, false);
	else
		AddBoolean("020A07", NULL | WORKGROUP , NULL, NULL, &C128FarReading, false, false);
#endif

	AddBoolean("020A08", LinearSymbologyCommandModifier | WORKGROUP , NULL, NULL, &C128AppendEnable, true, true);

	//	Code 93 Settings
	AddCommand("020D00", LinearSymbologyCommandModifier | WORKGROUP, NULL, NULL, SUB_TAG_TABLE::SET_WORKING_AND_ACTIVE_VALUES_TO_DEFAULT_FOR_THIS_TAG);


	if(ControlMask & MAKE_LINEAR_SYMBOLOGIES_AVAILABLE)
		AddBoolean("020D01", SUB_TAG_TABLE::FULLY_VISIBLE | SUB_TAG_TABLE::SYMBOLOGY_ENABLE_COMMAND | WORKGROUP, NULL, NULL, &C93Enable, true, true);
	else
		AddBoolean("020D01", LinearSymbologyCommandModifier | WORKGROUP, NULL, NULL, &C93Enable, false, false);


	AddNumber("020D02", LinearSymbologyCommandModifier | WORKGROUP, NULL, NULL, &C93MinLength, 0, 0, 0, 80, &C93MaxLength);
	AddNumber("020D03", LinearSymbologyCommandModifier | WORKGROUP, NULL, NULL, &C93MaxLength, 80, 80, 0, &C93MinLength, 80);
	AddNumericList("020D04", LinearSymbologyCommandModifier | WORKGROUP, NULL, NULL, &C93DfltCodePage, 2, 2, 2, 2);
	AddBoolean("020D05", LinearSymbologyCommandModifier | WORKGROUP , NULL, NULL, &C93AppendEnable, false, false);

	// MSI
	AddCommand("020E00", LinearSymbologyCommandModifier | WORKGROUP, NULL, NULL, SUB_TAG_TABLE::SET_WORKING_AND_ACTIVE_VALUES_TO_DEFAULT_FOR_THIS_TAG);
	if(ControlMask & MAKE_LINEAR_SYMBOLOGIES_AVAILABLE)
		AddBoolean("020E01", SUB_TAG_TABLE::FULLY_VISIBLE | SUB_TAG_TABLE::SYMBOLOGY_ENABLE_COMMAND | WORKGROUP, NULL, NULL, &MSIEnable, false, false);
	else
		AddBoolean("020E01", LinearSymbologyCommandModifier | WORKGROUP, NULL, NULL, &MSIEnable, false, false);

#if (DECODER_ID) || defined(RFBASE)
	AddNumericList("020E02", LinearSymbologyCommandModifier | WORKGROUP, NULL, NULL, &MSIChkChar, MSI_CHK_10, MSI_CHK_10,
	               MSI_CHK_10, MSI_CHK_10_XMIT,MSI_CHK_10_10,MSI_CHK_10_10_XMIT,MSI_CHK_11_10,MSI_CHK_11_10_XMIT,MSI_CHK_NONE);
#else
	AddNumericList("020E02", LinearSymbologyCommandModifier | WORKGROUP, NULL, NULL, &MSIChkChar, MSI_CHK_10, MSI_CHK_10,
	               MSI_CHK_10, MSI_CHK_10_XMIT);
#endif
	AddNumber("020E03", LinearSymbologyCommandModifier | WORKGROUP, NULL, NULL, &MSIMinLength, 4, 4, 4, 48, &MSIMaxLength);
	AddNumber("020E04", LinearSymbologyCommandModifier | WORKGROUP, NULL, NULL, &MSIMaxLength, 48, 48, 4, &MSIMinLength, 48);

#if (DECODER_AD)
	//	Plessey Settings
	AddCommand("020F00", LinearSymbologyCommandModifier | WORKGROUP, NULL, NULL, SUB_TAG_TABLE::SET_WORKING_AND_ACTIVE_VALUES_TO_DEFAULT_FOR_THIS_TAG);
	if(ControlMask & MAKE_LINEAR_SYMBOLOGIES_AVAILABLE)
		AddBoolean("020F01", SUB_TAG_TABLE::FULLY_VISIBLE | SUB_TAG_TABLE::SYMBOLOGY_ENABLE_COMMAND | WORKGROUP, NULL, NULL, &PlsyEnable, false, false);
	else
		AddBoolean("020F01", LinearSymbologyCommandModifier | WORKGROUP, NULL, NULL, &PlsyEnable, false, false);

	AddBoolean("020F02", LinearSymbologyCommandModifier | WORKGROUP, NULL, NULL, &PlsyChkChar, false,false);
	AddNumber("020F03", LinearSymbologyCommandModifier | WORKGROUP, NULL, NULL, &PlsyMinLength, 4, 4, 4, 48, &PlsyMaxLength);
	AddNumber("020F04", LinearSymbologyCommandModifier | WORKGROUP, NULL, NULL, &PlsyMaxLength, 48, 48, 4, &PlsyMinLength, 48);
#endif

	//	Telepen Settings
	AddCommand("021000", LinearSymbologyCommandModifier | WORKGROUP, NULL, NULL, SUB_TAG_TABLE::SET_WORKING_AND_ACTIVE_VALUES_TO_DEFAULT_FOR_THIS_TAG);


	if(ControlMask & MAKE_LINEAR_SYMBOLOGIES_AVAILABLE)
		AddBoolean("021001", SUB_TAG_TABLE::FULLY_VISIBLE | SUB_TAG_TABLE::SYMBOLOGY_ENABLE_COMMAND | WORKGROUP, NULL, NULL, &TELEnable, false, false);
	else
		AddBoolean("021001", LinearSymbologyCommandModifier | WORKGROUP, NULL, NULL, &TELEnable, false, false);



	AddNumericList("021002", LinearSymbologyCommandModifier | WORKGROUP, NULL, NULL, &TELOldStyle, 0, 0, 0, 1);
	AddNumber("021003", LinearSymbologyCommandModifier | WORKGROUP, NULL, NULL, &TELMinLength, 1, 1, 1, 60, &TELMaxLength);
	AddNumber("021004", LinearSymbologyCommandModifier | WORKGROUP, NULL, NULL, &TELMaxLength, 60, 60, 1, &TELMinLength, 60);


	//	UPC-A Settings
	AddCommand("021100", LinearSymbologyCommandModifier | WORKGROUP, NULL, NULL, SUB_TAG_TABLE::SET_WORKING_AND_ACTIVE_VALUES_TO_DEFAULT_FOR_THIS_TAG);

	if(ControlMask & MAKE_LINEAR_SYMBOLOGIES_AVAILABLE)
		AddBoolean("021101", SUB_TAG_TABLE::FULLY_VISIBLE | SUB_TAG_TABLE::SYMBOLOGY_ENABLE_COMMAND | WORKGROUP, NULL, NULL, &UPCAEnable, true, true);
	else
		AddBoolean("021101", LinearSymbologyCommandModifier | WORKGROUP, NULL, NULL, &UPCAEnable, false, false);


	AddBoolean("021102", LinearSymbologyCommandModifier | WORKGROUP, NULL, NULL, &UPCAChkXmit, true, true);
	AddBoolean("021103", LinearSymbologyCommandModifier | WORKGROUP, NULL, NULL, &UPCANumXmit, true, true);


	AddBoolean("021104", LinearSymbologyCommandModifier | WORKGROUP, NULL, NULL, &UPCAAddenda2, false, false);
	AddBoolean("021105", LinearSymbologyCommandModifier | WORKGROUP, NULL, NULL, &UPCAAddenda5, false, false);
	AddBoolean("021106", LinearSymbologyCommandModifier | WORKGROUP, NULL, NULL, &UPCAAddendaReq, false, false);


	AddBoolean("021107", LinearSymbologyCommandModifier | WORKGROUP, NULL, NULL, &UPCAAddendaSep, true, true);


	//	UPC-E Settings
	AddCommand("021200", LinearSymbologyCommandModifier | WORKGROUP, NULL, NULL, SUB_TAG_TABLE::SET_WORKING_AND_ACTIVE_VALUES_TO_DEFAULT_FOR_THIS_TAG);


	if(ControlMask & MAKE_LINEAR_SYMBOLOGIES_AVAILABLE)
		AddBoolean("021201", SUB_TAG_TABLE::FULLY_VISIBLE | SUB_TAG_TABLE::SYMBOLOGY_ENABLE_COMMAND | WORKGROUP, NULL, NULL, &UPCE0Enable, true, true);
	else
		AddBoolean("021201", LinearSymbologyCommandModifier | WORKGROUP, NULL, NULL, &UPCE0Enable, false, false);


	if(ControlMask & MAKE_LINEAR_SYMBOLOGIES_AVAILABLE)
		AddBoolean("021202", SUB_TAG_TABLE::FULLY_VISIBLE | SUB_TAG_TABLE::SYMBOLOGY_ENABLE_COMMAND | WORKGROUP, NULL, NULL, &UPCE1Enable, false, false);
	else
		AddBoolean("021202", LinearSymbologyCommandModifier | WORKGROUP, NULL, NULL, &UPCE1Enable, false, false);

	AddBoolean("021203", LinearSymbologyCommandModifier | WORKGROUP, NULL, NULL, &UPCEExpand, false, false);
	AddBoolean("021204", LinearSymbologyCommandModifier | WORKGROUP, NULL, NULL, &UPCEChkXmit, true, true);
	AddBoolean("021205", LinearSymbologyCommandModifier | WORKGROUP, NULL, NULL, &UPCENumXmit, true, true);


	AddBoolean("021206", LinearSymbologyCommandModifier | WORKGROUP, NULL, NULL, &UPCEAddenda2, false, false);
	AddBoolean("021207", LinearSymbologyCommandModifier | WORKGROUP, NULL, NULL, &UPCEAddenda5, false, false);
	AddBoolean("021208", LinearSymbologyCommandModifier | WORKGROUP, NULL, NULL, &UPCEAddendaReq, false, false);


	AddBoolean("021209", LinearSymbologyCommandModifier | WORKGROUP, NULL, NULL, &UPCEAddendaSep, true, true);


	//	EAN-13 Settings
	AddCommand("021300", LinearSymbologyCommandModifier | WORKGROUP, NULL, NULL, SUB_TAG_TABLE::SET_WORKING_AND_ACTIVE_VALUES_TO_DEFAULT_FOR_THIS_TAG);


	if(ControlMask & MAKE_LINEAR_SYMBOLOGIES_AVAILABLE)
		AddBoolean("021301", SUB_TAG_TABLE::FULLY_VISIBLE | SUB_TAG_TABLE::SYMBOLOGY_ENABLE_COMMAND | WORKGROUP, NULL, NULL, &EAN13Enable, true, true);
	else
		AddBoolean("021301", LinearSymbologyCommandModifier | WORKGROUP, NULL, NULL, &EAN13Enable, false, false);


	AddBoolean("021302", LinearSymbologyCommandModifier | WORKGROUP, NULL, NULL, &EAN13ChkXmit, true, true);


	AddBoolean("021303", LinearSymbologyCommandModifier | WORKGROUP, NULL, NULL, &EAN13Addenda2, false, false);
	AddBoolean("021304", LinearSymbologyCommandModifier | WORKGROUP, NULL, NULL, &EAN13Addenda5, false, false);
	AddBoolean("021305", LinearSymbologyCommandModifier | WORKGROUP, NULL, NULL, &EAN13AddendaReq, false, false);


	AddBoolean("021306", LinearSymbologyCommandModifier | WORKGROUP, NULL, NULL, &EAN13AddendaSep, true, true);
	AddBoolean("021307", LinearSymbologyCommandModifier | WORKGROUP, NULL, NULL, &EAN13ISBN, false, false);


	//	EAN-8 Settings
	AddCommand("021400", LinearSymbologyCommandModifier | WORKGROUP, NULL, NULL, SUB_TAG_TABLE::SET_WORKING_AND_ACTIVE_VALUES_TO_DEFAULT_FOR_THIS_TAG);


	if(ControlMask & MAKE_LINEAR_SYMBOLOGIES_AVAILABLE)
		AddBoolean("021401", SUB_TAG_TABLE::FULLY_VISIBLE | SUB_TAG_TABLE::SYMBOLOGY_ENABLE_COMMAND | WORKGROUP, NULL, NULL, &EAN8Enable, true, true);
	else
		AddBoolean("021401", LinearSymbologyCommandModifier | WORKGROUP, NULL, NULL, &EAN8Enable, false, false);


	AddBoolean("021402", LinearSymbologyCommandModifier | WORKGROUP, NULL, NULL, &EAN8ChkXmit, true, true);

	AddBoolean("021403", LinearSymbologyCommandModifier | WORKGROUP, NULL, NULL, &EAN8Addenda2, false, false);
	AddBoolean("021404", LinearSymbologyCommandModifier | WORKGROUP, NULL, NULL, &EAN8Addenda5, false, false);
	AddBoolean("021405", LinearSymbologyCommandModifier | WORKGROUP, NULL, NULL, &EAN8AddendaReq, false, false);


	AddBoolean("021406", LinearSymbologyCommandModifier | WORKGROUP, NULL, NULL, &EAN8AddendaSep, true, true);


	// Coupon codes

	//FIXME	if( CHal::WhatBoardAmI() == IT3800G_1_TOKEN )
	//FIXME		if(ControlMask & MAKE_LINEAR_SYMBOLOGIES_AVAILABLE)
	//FIXME			AddNumber("CPNENA", SUB_TAG_TABLE::FULLY_VISIBLE | SUB_TAG_TABLE::SYMBOLOGY_ENABLE_COMMAND | WORKGROUP, NULL, NULL, &CouponCodes, 0, 0, 0, 2);
	//FIXME		else
	//FIXME			AddNumber("CPNENA", LinearSymbologyCommandModifier | WORKGROUP, NULL, NULL, &CouponCodes, 0, 0, 0, 2);
	//FIXME	else
	if(ControlMask & MAKE_LINEAR_SYMBOLOGIES_AVAILABLE)
		AddNumber("021500", SUB_TAG_TABLE::FULLY_VISIBLE | SUB_TAG_TABLE::SYMBOLOGY_ENABLE_COMMAND | WORKGROUP, NULL, NULL, &CouponCodes, 0, 0, 0, 2);
	else
		AddNumber("021500", LinearSymbologyCommandModifier | WORKGROUP, NULL, NULL, &CouponCodes, 0, 0, 0, 2);

	AddBoolean("021501", LinearSymbologyCommandModifier | WORKGROUP , NULL, NULL, &CouponSecondaryOutput, false, false);



	// EAN.UCC Emulation
	AddNumber("021600", LinearSymbologyCommandModifier | WORKGROUP, NULL, NULL, &EanUccEmulation, 0, 0, 0, 4);

#if (DECODER_AD)
	// Enabling MLD for UPC/EAN
	if(ControlMask & MAKE_LINEAR_SYMBOLOGIES_AVAILABLE)
		AddBoolean("021700", SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG | WORKGROUP , NULL, NULL, &UpcFarReading, true, true);
	else
		AddBoolean("021700", NULL | WORKGROUP , NULL, NULL, &UpcFarReading, true, true);
#endif


	//	China Postal Code Settings
	AddCommand("021800", MAKE_HIDDEN_UNTIL_LICENSED | WORKGROUP, NULL, NULL, SUB_TAG_TABLE::SET_WORKING_AND_ACTIVE_VALUES_TO_DEFAULT_FOR_THIS_TAG); //Note this is a Licensable feature and is updated in the ConfigureLicensableCommands()
	AddBoolean("021801", MAKE_HIDDEN_UNTIL_LICENSED | WORKGROUP, NULL, NULL, &ChinaEnable, false, false); //Note this is a Licensable feature and is updated in the ConfigureLicensableCommands()

	AddNumber("021802", MAKE_HIDDEN_UNTIL_LICENSED | WORKGROUP, NULL, NULL, &ChinaMinLength, 4, 4, 2, 80, &ChinaMaxLength); //Note this is a Licensable feature and is updated in the ConfigureLicensableCommands()
	AddNumber("021803", MAKE_HIDDEN_UNTIL_LICENSED | WORKGROUP, NULL, NULL, &ChinaMaxLength, 80, 80, 2, &ChinaMinLength, 80); //Note this is a Licensable feature and is updated in the ConfigureLicensableCommands()


	//	RSS-14 Code Settings
	AddCommand("021900", LinearSymbologyCommandModifier | WORKGROUP, NULL, NULL, SUB_TAG_TABLE::SET_WORKING_AND_ACTIVE_VALUES_TO_DEFAULT_FOR_THIS_TAG);


	if(ControlMask & MAKE_LINEAR_SYMBOLOGIES_AVAILABLE)
		AddBoolean("021901", SUB_TAG_TABLE::FULLY_VISIBLE | SUB_TAG_TABLE::SYMBOLOGY_ENABLE_COMMAND | WORKGROUP, NULL, NULL, &RSS14Enable, true, true);
	else
		AddBoolean("021901", LinearSymbologyCommandModifier | WORKGROUP, NULL, NULL, &RSS14Enable, false, false);



	//	RSS-Limited Code Settings
	AddCommand("021A00", LinearSymbologyCommandModifier | WORKGROUP, NULL, NULL, SUB_TAG_TABLE::SET_WORKING_AND_ACTIVE_VALUES_TO_DEFAULT_FOR_THIS_TAG);


	if(ControlMask & MAKE_LINEAR_SYMBOLOGIES_AVAILABLE)
		AddBoolean("021A01", SUB_TAG_TABLE::FULLY_VISIBLE | SUB_TAG_TABLE::SYMBOLOGY_ENABLE_COMMAND | WORKGROUP, NULL, NULL, &RSSLimEnable, true, true);
	else
		AddBoolean("021A01", LinearSymbologyCommandModifier | WORKGROUP, NULL, NULL, &RSSLimEnable, false, false);



	//	RSS-Expanded Code Settings
	AddCommand("021B00", LinearSymbologyCommandModifier | WORKGROUP, NULL, NULL, SUB_TAG_TABLE::SET_WORKING_AND_ACTIVE_VALUES_TO_DEFAULT_FOR_THIS_TAG);


	if(ControlMask & MAKE_LINEAR_SYMBOLOGIES_AVAILABLE)
		AddBoolean("021B01", SUB_TAG_TABLE::FULLY_VISIBLE | SUB_TAG_TABLE::SYMBOLOGY_ENABLE_COMMAND | WORKGROUP, NULL, NULL, &RSSExEnable, true, true);
	else
		AddBoolean("021B01", LinearSymbologyCommandModifier | WORKGROUP, NULL, NULL, &RSSExEnable, false, false);


	AddNumber("021B02", LinearSymbologyCommandModifier | WORKGROUP, NULL, NULL, &RSSExMinLength, 4, 4, 4, 74, &RSSExMaxLength);
	AddNumber("021B03", LinearSymbologyCommandModifier | WORKGROUP, NULL, NULL, &RSSExMaxLength, 74, 74, 4, &RSSExMinLength, 74);

#if (DECODER_AD)
	// LabelCode
	if(ControlMask & MAKE_LINEAR_SYMBOLOGIES_AVAILABLE)
		AddBoolean("021C00", SUB_TAG_TABLE::FULLY_VISIBLE | SUB_TAG_TABLE::SYMBOLOGY_ENABLE_COMMAND | WORKGROUP, NULL, NULL, &LabelCodeEnable, true, true);
	else
		AddBoolean("021C00", LinearSymbologyCommandModifier | WORKGROUP, NULL, NULL, &LabelCodeEnable, false, false);


	// Posicode
	AddCommand("021D00", LinearSymbologyCommandModifier | WORKGROUP, NULL, NULL, SUB_TAG_TABLE::SET_WORKING_AND_ACTIVE_VALUES_TO_DEFAULT_FOR_THIS_TAG);
	if(ControlMask & MAKE_LINEAR_SYMBOLOGIES_AVAILABLE)
		AddBoolean("021D01", SUB_TAG_TABLE::FULLY_VISIBLE | SUB_TAG_TABLE::SYMBOLOGY_ENABLE_COMMAND | WORKGROUP, NULL, NULL, &posiEnable, true, true);
	else
		AddBoolean("021D01", LinearSymbologyCommandModifier | WORKGROUP, NULL, NULL, &posiEnable, false, false);

	AddNumber("021D02", LinearSymbologyCommandModifier | WORKGROUP, NULL, NULL, &posiMinLength, 4, 4, 2, 80, &posiMaxLength);
	AddNumber("021D03", LinearSymbologyCommandModifier | WORKGROUP, NULL, NULL, &posiMaxLength, 48, 48, 2, &posiMinLength, 80);
	AddNumericList("021D04", LinearSymbologyCommandModifier | WORKGROUP, NULL, NULL, &posiLimited, POSILIMITED_B, POSILIMITED_B, POSILIMITED_OFF, POSILIMITED_A, POSILIMITED_B);
#endif

	// trioptic
	if(ControlMask & MAKE_LINEAR_SYMBOLOGIES_AVAILABLE)
		AddBoolean("021E00", SUB_TAG_TABLE::FULLY_VISIBLE | SUB_TAG_TABLE::SYMBOLOGY_ENABLE_COMMAND | WORKGROUP, NULL, NULL, &triopticEnable, false, false);
	else
		AddBoolean("021E00", LinearSymbologyCommandModifier | WORKGROUP, NULL, NULL, &triopticEnable, false, false);


	//***************************************************************************************************************************
	/*Stacked Linear Code Decoders*/


	// PDF Settings and Decoder Options
	AddCommand("021F00", MAKE_HIDDEN_UNTIL_LICENSED | WORKGROUP, NULL, NULL, SUB_TAG_TABLE::SET_WORKING_AND_ACTIVE_VALUES_TO_DEFAULT_FOR_THIS_TAG); //Note this is a Licensable feature and is updated in the ConfigureLicensableCommands()

	AddBoolean("021F01", MAKE_HIDDEN_UNTIL_LICENSED | WORKGROUP, NULL, NULL, &PDFEnable, false, false);  //Note this is a Licensable feature and is updated in the ConfigureLicensableCommands()


	AddBoolean("021F02", SUB_TAG_TABLE::FULLY_VISIBLE | WORKGROUP, NULL, NULL, &PDFMacroEnabled, true, true);

#if (DECODER_AD)
	//AddBoolean("PDFDIA", NULL | WORKGROUP, NULL, NULL, &PDFDiagnostics, false, false);
	AddBoolean("021F03", MAKE_HIDDEN_UNTIL_LICENSED | WORKGROUP, NULL, NULL, &PDFShowGLI, false, false); //Note this is a Licensable feature and is updated in the ConfigureLicensableCommands()
	//AddBoolean("PDFMAC", NULL | WORKGROUP, NULL, NULL, &PDFMacroEnabled, true, true);
	//AddBoolean("PDFSMB", NULL | WORKGROUP, NULL, NULL, &PDFShowMacro, false, false);
	//AddBoolean("PDFLRN", NULL | WORKGROUP, NULL, NULL, &PDFLearnMode, false, false);
	if(FALSE == (ControlMask & MAKE_2D_COMMANDS_AVAILABLE))
		AddBoolean("021F04", SUB_TAG_TABLE::FULLY_VISIBLE | WORKGROUP, NULL, NULL, &PDFTicks, true, true); //PDF tick is only for linear readers.
	else
		AddBoolean("021F04", NULL | WORKGROUP, NULL, NULL, &PDFTicks, false, false); //PDF tick is only for linear readers.
#endif
	AddNumber("021F05", MAKE_HIDDEN_UNTIL_LICENSED | WORKGROUP, NULL, NULL, &PDFMinLength, 1, 1, 1, 2750, &PDFMaxLength); //Note this is a Licensable feature and is updated in the ConfigureLicensableCommands()
	AddNumber("021F06", MAKE_HIDDEN_UNTIL_LICENSED | WORKGROUP, NULL, NULL, &PDFMaxLength, 2750, 2750, 1, &PDFMinLength, 2750); //Note this is a Licensable feature and is updated in the ConfigureLicensableCommands()
	AddNumericList("021F07", MAKE_HIDDEN_UNTIL_LICENSED | WORKGROUP, NULL, NULL, &PDFDfltCodePage, 30, 30, 30, 30); //Note this is a Licensable feature and is updated in the ConfigureLicensableCommands()



	// MicroPDF Settings
	AddCommand("022000", MAKE_HIDDEN_UNTIL_LICENSED | WORKGROUP, NULL, NULL, SUB_TAG_TABLE::SET_WORKING_AND_ACTIVE_VALUES_TO_DEFAULT_FOR_THIS_TAG); //Note this is a Licensable feature and is updated in the ConfigureLicensableCommands()
	AddBoolean("022001", MAKE_HIDDEN_UNTIL_LICENSED | WORKGROUP, NULL, NULL, &MPDEnable, false, false); //Note this is a Licensable feature and is updated in the ConfigureLicensableCommands()

	AddNumber("022002", MAKE_HIDDEN_UNTIL_LICENSED | WORKGROUP, NULL, NULL, &MPDMinLength, 1, 1, 1, 366, &MPDMaxLength); //Note this is a Licensable feature and is updated in the ConfigureLicensableCommands()
	AddNumber("022003", MAKE_HIDDEN_UNTIL_LICENSED | WORKGROUP, NULL, NULL, &MPDMaxLength, 366, 366, 1, &MPDMinLength, 366); //Note this is a Licensable feature and is updated in the ConfigureLicensableCommands()
	AddNumericList("022004", MAKE_HIDDEN_UNTIL_LICENSED | WORKGROUP, NULL, NULL, &MPDDfltCodePage, 30, 30, 30, 30); //Note this is a Licensable feature and is updated in the ConfigureLicensableCommands()


	// Composites Settings
	AddCommand("022100", MAKE_HIDDEN_UNTIL_LICENSED | WORKGROUP, NULL, NULL, SUB_TAG_TABLE::SET_WORKING_AND_ACTIVE_VALUES_TO_DEFAULT_FOR_THIS_TAG); //Note this is a Licensable feature and is updated in the ConfigureLicensableCommands()
	AddBoolean("022101", MAKE_HIDDEN_UNTIL_LICENSED | WORKGROUP, NULL, NULL, &CompositeEnable, false, false); //Note this is a Licensable feature and is updated in the ConfigureLicensableCommands()

	AddBoolean("022102", MAKE_HIDDEN_UNTIL_LICENSED | WORKGROUP, NULL, NULL, &CompsOnUpcEan, false, false); //Note this is a Licensable feature and is updated in the ConfigureLicensableCommands()
	AddNumber("022103", MAKE_HIDDEN_UNTIL_LICENSED | WORKGROUP, NULL, NULL, &CompMinLength, 1, 1, 1, 2435, &CompMaxLength); //Note this is a Licensable feature and is updated in the ConfigureLicensableCommands()
	AddNumber("022104", MAKE_HIDDEN_UNTIL_LICENSED | WORKGROUP, NULL, NULL, &CompMaxLength, 2435, 2435, 1, &CompMinLength, 2435); //Note this is a Licensable feature and is updated in the ConfigureLicensableCommands()
	AddNumericList("022105", MAKE_HIDDEN_UNTIL_LICENSED | WORKGROUP, NULL, NULL, &COMDfltCodePage, 30, 30, 30, 30); //Note this is a Licensable feature and is updated in the ConfigureLicensableCommands()


	// TLC39
	AddBoolean("022200", MAKE_HIDDEN_UNTIL_LICENSED | WORKGROUP, NULL, NULL, &tlc39Enable, false, false); //Note this is a Licensable feature and is updated in the ConfigureLicensableCommands()

#if (DECODER_ID) || (DECODER_FLD) || defined(RFBASE)
	// Codablock A settings
	AddCommand("022300", MAKE_HIDDEN_UNTIL_LICENSED | WORKGROUP, NULL, NULL, SUB_TAG_TABLE::SET_WORKING_AND_ACTIVE_VALUES_TO_DEFAULT_FOR_THIS_TAG); //Note this is a Licensable feature and is updated in the ConfigureLicensableCommands()
	AddBoolean("022301", MAKE_HIDDEN_UNTIL_LICENSED | WORKGROUP, NULL, NULL, &cblkaEnable, false, false); //Note this is a Licensable feature and is updated in the ConfigureLicensableCommands()

	AddNumber("022302", MAKE_HIDDEN_UNTIL_LICENSED | WORKGROUP, NULL, NULL, &cblkaMinLength, 1, 1, 1, 600, &cblkaMaxLength); //Note this is a Licensable feature and is updated in the ConfigureLicensableCommands()
	AddNumber("022303", MAKE_HIDDEN_UNTIL_LICENSED | WORKGROUP, NULL, NULL, &cblkaMaxLength, 600, 600, 1, &cblkaMinLength, 600); //Note this is a Licensable feature and is updated in the ConfigureLicensableCommands()
#endif
	// Codablock F settings
	AddCommand("022400", MAKE_HIDDEN_UNTIL_LICENSED | WORKGROUP, NULL, NULL, SUB_TAG_TABLE::SET_WORKING_AND_ACTIVE_VALUES_TO_DEFAULT_FOR_THIS_TAG); //Note this is a Licensable feature and is updated in the ConfigureLicensableCommands()
	AddBoolean("022401", MAKE_HIDDEN_UNTIL_LICENSED | WORKGROUP, NULL, NULL, &cblkfEnable, false, false);//Note this is a Licensable feature and is updated in the ConfigureLicensableCommands()

	AddNumber("022402", MAKE_HIDDEN_UNTIL_LICENSED | WORKGROUP, NULL, NULL, &cblkfMinLength, 1, 1, 1, 2048, &cblkfMaxLength); //Note this is a Licensable feature and is updated in the ConfigureLicensableCommands()
	AddNumber("022403", MAKE_HIDDEN_UNTIL_LICENSED | WORKGROUP, NULL, NULL, &cblkfMaxLength, 2048, 2048, 1, &cblkfMinLength, 2048); //Note this is a Licensable feature and is updated in the ConfigureLicensableCommands()

#if (DECODER_AD)
	//	Code 16 K Settings
	AddCommand("022500", MAKE_HIDDEN_UNTIL_LICENSED | WORKGROUP, NULL, NULL, SUB_TAG_TABLE::SET_WORKING_AND_ACTIVE_VALUES_TO_DEFAULT_FOR_THIS_TAG); //Note this is a Licensable feature and is updated in the ConfigureLicensableCommands()
	AddBoolean("022501", MAKE_HIDDEN_UNTIL_LICENSED | WORKGROUP, NULL, NULL, &c16KEnable, false, false);//Note this is a Licensable feature and is updated in the ConfigureLicensableCommands()
	AddNumber("022502", MAKE_HIDDEN_UNTIL_LICENSED | WORKGROUP, NULL, NULL, &c16KMinLength, 1, 1, 0, 160, &c16KMaxLength); //Note this is a Licensable feature and is updated in the ConfigureLicensableCommands()
	AddNumber("022503", MAKE_HIDDEN_UNTIL_LICENSED | WORKGROUP, NULL, NULL, &c16KMaxLength, 160, 160, 0, &c16KMinLength, 160); //Note this is a Licensable feature and is updated in the ConfigureLicensableCommands()
	// Code 49 Settings
	AddCommand("022600", MAKE_HIDDEN_UNTIL_LICENSED | WORKGROUP, NULL, NULL, SUB_TAG_TABLE::SET_WORKING_AND_ACTIVE_VALUES_TO_DEFAULT_FOR_THIS_TAG); //Note this is a Licensable feature and is updated in the ConfigureLicensableCommands()
	AddBoolean("022601", MAKE_HIDDEN_UNTIL_LICENSED | WORKGROUP, NULL, NULL, &c49Enable, false, false);//Note this is a Licensable feature and is updated in the ConfigureLicensableCommands()
	AddNumber("022602", MAKE_HIDDEN_UNTIL_LICENSED | WORKGROUP, NULL, NULL, &c49MinLength, 1, 1, 1, 81, &c49MaxLength); //Note this is a Licensable feature and is updated in the ConfigureLicensableCommands()
	AddNumber("022603", MAKE_HIDDEN_UNTIL_LICENSED | WORKGROUP, NULL, NULL, &c49MaxLength, 81, 81, 1, &c49MinLength, 81); //Note this is a Licensable feature and is updated in the ConfigureLicensableCommands()
#endif

	//***************************************************************************************************************************
	/*Custom Decoders, have seperate configuration masks */

#if (DECODER_AD)
	// Code Z Settings
	AddCommand("022700", CodeZSymbologyCommandModifier | WORKGROUP, NULL, NULL, SUB_TAG_TABLE::SET_WORKING_AND_ACTIVE_VALUES_TO_DEFAULT_FOR_THIS_TAG);
	if(ControlMask & MAKE_CODE_Z_SYMBOLOGY_AVAILABLE)
		AddBoolean("022701", SUB_TAG_TABLE::FULLY_VISIBLE | SUB_TAG_TABLE::SYMBOLOGY_ENABLE_COMMAND | WORKGROUP, NULL, NULL, &codezEnable, false, false);
	else
		AddBoolean("022701", CodeZSymbologyCommandModifier | WORKGROUP, NULL, NULL, &codezEnable, false, false);

	AddNumber("022702", CodeZSymbologyCommandModifier | WORKGROUP, NULL, NULL, &codezMinLength, 1, 1, 1, 80, &codezMaxLength);
	AddNumber("022703", CodeZSymbologyCommandModifier | WORKGROUP, NULL, NULL, &codezMaxLength, 80, 80, 1, &codezMinLength, 80);


	// DPC CIRRUS Settings
	if(ControlMask & MAKE_DPC_CIRRUS_SYMBOLOGY_AVAILABLE)
		AddBoolean("022800", SUB_TAG_TABLE::FULLY_VISIBLE | SUB_TAG_TABLE::SYMBOLOGY_ENABLE_COMMAND | WORKGROUP, NULL, NULL, &DpcCirrusEnable, false, false);
	else
		AddBoolean("022800", DpcCirrusSymbologyCommandModifier | WORKGROUP, NULL, NULL, &DpcCirrusEnable, false, false);


	//GMX settings
	AddCommand("022900", GmxMatrixSymbologyCommandModifier | WORKGROUP, NULL, NULL, SUB_TAG_TABLE::SET_WORKING_AND_ACTIVE_VALUES_TO_DEFAULT_FOR_THIS_TAG);
	if(ControlMask & MAKE_GRID_MATRIX_SYMBOLOGY_AVAILABLE)
		AddBoolean("022901", SUB_TAG_TABLE::FULLY_VISIBLE | SUB_TAG_TABLE::SYMBOLOGY_ENABLE_COMMAND | WORKGROUP, NULL, NULL, &gmxEnable, false, false);
	else
		AddBoolean("022901", GmxMatrixSymbologyCommandModifier | WORKGROUP, NULL, NULL, &gmxEnable, false, false);

	AddNumber("022902", GmxMatrixSymbologyCommandModifier | WORKGROUP, NULL, NULL, &gmxMinLength, 1, 1, 1, 2751, &gmxMaxLength);
	AddNumber("022903", GmxMatrixSymbologyCommandModifier | WORKGROUP, NULL, NULL, &gmxMaxLength, 2751, 2751, 1, &gmxMinLength, 2751);
	AddBoolean("022904", LinearSymbologyCommandModifier | WORKGROUP , NULL, NULL, &gmxAppendEnable, true, true);


	//VeriCode settings													/* These options only available in Vericode build. */
	AddCommand("022A00", VericodeSymbologyCommandModifier | WORKGROUP, NULL, NULL, SUB_TAG_TABLE::SET_WORKING_AND_ACTIVE_VALUES_TO_DEFAULT_FOR_THIS_TAG);
	if(ControlMask & MAKE_VERICODE_SYMBOLOGY_AVAILABLE)
		AddBoolean("022A01", SUB_TAG_TABLE::FULLY_VISIBLE | SUB_TAG_TABLE::SYMBOLOGY_ENABLE_COMMAND | WORKGROUP, NULL, NULL, &verEnable, false, false);
	else
		AddBoolean("022A01", VericodeSymbologyCommandModifier | WORKGROUP, NULL, NULL, &verEnable, false, false);

	AddNumber("022A02", VericodeSymbologyCommandModifier | WORKGROUP, NULL, NULL, &verMinLength, 1, 1, 1, 150, &verMaxLength);
	AddNumber("022A03", VericodeSymbologyCommandModifier | WORKGROUP, NULL, NULL, &verMaxLength, 150, 150, 1, &verMinLength, 150);
	AddNumber("022A04", VericodeSymbologyCommandModifier | WORKGROUP, NULL, NULL, &verSizeA, 0, 0, 0, 48);
	AddNumber("022A05", VericodeSymbologyCommandModifier | WORKGROUP, NULL, NULL, &verSizeB, 0, 0, 0, 48);
	AddNumber("022A06", VericodeSymbologyCommandModifier | WORKGROUP, NULL, NULL, &verSizeC, 0, 0, 0, 48);
	AddNumber("022A07", VericodeSymbologyCommandModifier | WORKGROUP, NULL, NULL, &verSizeD, 0, 0, 0, 48);
	AddNumber("022A08", VericodeSymbologyCommandModifier | WORKGROUP, NULL, NULL, &verSizeE, 0, 0, 0, 48);


	// SecureCode Settings
	AddCommand("022B00", SUB_TAG_TABLE::FIXED | WORKGROUP, NULL, NULL, SUB_TAG_TABLE::SET_WORKING_AND_ACTIVE_VALUES_TO_DEFAULT_FOR_THIS_TAG);
	if(ControlMask & MAKE_SECURECODE_SYMBOLOGY_AVAILABLE)
		AddBoolean("022B01", SUB_TAG_TABLE::FULLY_VISIBLE | SUB_TAG_TABLE::SYMBOLOGY_ENABLE_COMMAND | WORKGROUP, NULL, NULL, &SecureCodeEnable, false, false);
	else
		AddBoolean("022B01", NULL | WORKGROUP, NULL, NULL, &SecureCodeEnable, false, false);

	AddNumber("022B02", SecureCodeSymbologyCommandModifier | WORKGROUP, NULL, NULL, &SecureCodeMinLength, 1, 1, 1, 4000, &SecureCodeMaxLength);
	AddNumber("022B03", SecureCodeSymbologyCommandModifier | WORKGROUP, NULL, NULL, &SecureCodeMaxLength, 4000, 4000, 1, &SecureCodeMinLength, 40000);
	if(ControlMask & MAKE_SECURECODE_SYMBOLOGY_AVAILABLE)
		AddString("022B04", SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG | SUB_TAG_TABLE::STRING_MUST_BE_QUOTED | WORKGROUP, NULL, NULL, SecureCodeKey,
		          SIZE_OF_SECURECODE_APP_KEY, "KEY", "KEY");
	else
		AddString("022B04", NULL | WORKGROUP, NULL, NULL, SecureCodeKey, SIZE_OF_SECURECODE_APP_KEY, "KEY", "KEY");


	// BC-412 Settings
	AddCommand("022C00", BC412SymbologyCommandModifier | WORKGROUP, NULL, NULL, SUB_TAG_TABLE::SET_WORKING_AND_ACTIVE_VALUES_TO_DEFAULT_FOR_THIS_TAG);
	if(ControlMask & MAKE_BC_412_SYMBOLOGY_AVAILABLE)
		AddBoolean("022C01", SUB_TAG_TABLE::FULLY_VISIBLE | SUB_TAG_TABLE::SYMBOLOGY_ENABLE_COMMAND | WORKGROUP, NULL, NULL, &bc412Enable, false, false);
	else
		AddBoolean("022C01", BC412SymbologyCommandModifier | WORKGROUP, NULL, NULL, &bc412Enable, false, false);

	AddNumber("022C02", BC412SymbologyCommandModifier | WORKGROUP, NULL, NULL, &bc412MinLength, 0, 0, 0, 80, &bc412MaxLength);
	AddNumber("022C03", BC412SymbologyCommandModifier | WORKGROUP, NULL, NULL, &bc412MaxLength, 80, 80, 0, &bc412MinLength, 80);
#endif

	//***************************************************************************************************************************
	/*Matrix Symbologies Decoders*/

#if (DECODER_AD)
	// Mesa settings
	AddBoolean("022D00", MAKE_HIDDEN_UNTIL_LICENSED | WORKGROUP, NULL, NULL, &i25MesaEnable, false, false); //Note this is a Licensable feature and is updated in the ConfigureLicensableCommands()
	AddBoolean("022E00", MAKE_HIDDEN_UNTIL_LICENSED | WORKGROUP, NULL, NULL, &c128MesaEnable, false, false); //Note this is a Licensable feature and is updated in the ConfigureLicensableCommands()
	AddBoolean("022F00", MAKE_HIDDEN_UNTIL_LICENSED | WORKGROUP, NULL, NULL, &c39MesaEnable, false, false); //Note this is a Licensable feature and is updated in the ConfigureLicensableCommands()
	AddBoolean("023000", MAKE_HIDDEN_UNTIL_LICENSED | WORKGROUP, NULL, NULL, &c93MesaEnable, false, false); //Note this is a Licensable feature and is updated in the ConfigureLicensableCommands()
	AddBoolean("023100", MAKE_HIDDEN_UNTIL_LICENSED | WORKGROUP, NULL, NULL, &upcaMesaEnable, false, false); //Note this is a Licensable feature and is updated in the ConfigureLicensableCommands()


	// Please don't change this!!!!  I have changed this 3 times and it always returns to what it was before... I think 'ENS' is incorrect
	//	AddBoolean("ENSENA", SUB_TAG_TABLE::FULLY_VISIBLE | SUB_TAG_TABLE::SYMBOLOGY_ENABLE_COMMAND | WORKGROUP, NULL, NULL, &ean13MesaEnable, false, false);
	AddBoolean("023200", MAKE_HIDDEN_UNTIL_LICENSED | WORKGROUP, NULL, NULL, &ean13MesaEnable, false, false); //Note this is a Licensable feature and is updated in the ConfigureLicensableCommands()
#endif

	// Aztec settings
	AddCommand("023300", MAKE_HIDDEN_UNTIL_LICENSED | WORKGROUP, NULL, NULL, SUB_TAG_TABLE::SET_WORKING_AND_ACTIVE_VALUES_TO_DEFAULT_FOR_THIS_TAG); //Note this is a Licensable feature and is updated in the ConfigureLicensableCommands()
	AddBoolean("023301", MAKE_HIDDEN_UNTIL_LICENSED | WORKGROUP, NULL, NULL, &aztEnable, false, false); //Note this is a Licensable feature and is updated in the ConfigureLicensableCommands()
#if (DECODER_ID) || defined(RFBASE)
	AddBoolean("023302", MAKE_HIDDEN_UNTIL_LICENSED | WORKGROUP, NULL, NULL, &aztSmallSymbolSize, false, false); //Note this is a Licensable feature and is updated in the ConfigureLicensableCommands()
	AddBoolean("023303", MAKE_HIDDEN_UNTIL_LICENSED | WORKGROUP, NULL, NULL, &PresenationDecodeAztecSymbolSize, false, false);
#else
	AddBoolean("023304", MAKE_HIDDEN_UNTIL_LICENSED | WORKGROUP, NULL, NULL, &aztRunes, false, false); //Note this is a Licensable feature and is updated in the ConfigureLicensableCommands()
#endif
	AddNumber("023305", MAKE_HIDDEN_UNTIL_LICENSED | WORKGROUP, NULL, NULL, &aztMinLength, 1, 1, 1, 3832, &aztMaxLength); //Note this is a Licensable feature and is updated in the ConfigureLicensableCommands()
	AddNumber("023306", MAKE_HIDDEN_UNTIL_LICENSED | WORKGROUP, NULL, NULL, &aztMaxLength, 3832, 3832, 1, &aztMinLength, 3832); //Note this is a Licensable feature and is updated in the ConfigureLicensableCommands()
	AddNumericList("023307", MAKE_HIDDEN_UNTIL_LICENSED | WORKGROUP, NULL, NULL, &aztDfltCodePage, 51, 51, 51, 51); //Note this is a Licensable feature and is updated in the ConfigureLicensableCommands()

	AddBoolean("023308", LinearSymbologyCommandModifier | WORKGROUP , NULL, NULL, &aztAppendEnable, true, true);
	AddBoolean("023309", LinearSymbologyCommandModifier | WORKGROUP , NULL, NULL, &aztAppendStripInfo, true, true);


	// Maxicode settings
	AddCommand("023400", MAKE_HIDDEN_UNTIL_LICENSED | WORKGROUP, NULL, NULL, SUB_TAG_TABLE::SET_WORKING_AND_ACTIVE_VALUES_TO_DEFAULT_FOR_THIS_TAG); //Note this is a Licensable feature and is updated in the ConfigureLicensableCommands()
	AddBoolean("023401", MAKE_HIDDEN_UNTIL_LICENSED | WORKGROUP, NULL, NULL, &maxEnable, true, true); //Note this is a Licensable feature and is updated in the ConfigureLicensableCommands()
	AddNumber("023402", MAKE_HIDDEN_UNTIL_LICENSED | WORKGROUP, NULL, NULL, &maxMinLength, 1, 1, 1, 150, &maxMaxLength); //Note this is a Licensable feature and is updated in the ConfigureLicensableCommands()
	AddNumber("023403", MAKE_HIDDEN_UNTIL_LICENSED | WORKGROUP, NULL, NULL, &maxMaxLength, 150, 150, 1, &maxMinLength, 150); //Note this is a Licensable feature and is updated in the ConfigureLicensableCommands()

#if (DECODER_ID) || defined(RFBASE)
	AddBoolean("023404", MAKE_HIDDEN_UNTIL_LICENSED | WORKGROUP, NULL, NULL, &maxSmallSymbolSize, false, false); //Note this is a Licensable feature and is updated in the ConfigureLicensableCommands()
	AddBoolean("023405", MAKE_HIDDEN_UNTIL_LICENSED | WORKGROUP, NULL, NULL, &PresenationDecodeMaxicodeSymbolSize, false, false);
#else
	AddBoolean("023500", SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG | WORKGROUP, NULL, NULL, &refTargetEnable, false, false);
#endif

	// Data Matrix settings
	AddCommand("023600", MAKE_HIDDEN_UNTIL_LICENSED | WORKGROUP, NULL, NULL, SUB_TAG_TABLE::SET_WORKING_AND_ACTIVE_VALUES_TO_DEFAULT_FOR_THIS_TAG); //Note this is a Licensable feature and is updated in the ConfigureLicensableCommands()
	AddBoolean("023601", MAKE_HIDDEN_UNTIL_LICENSED | WORKGROUP, NULL, NULL, &idmEnable, false, false); //Note this is a Licensable feature and is updated in the ConfigureLicensableCommands()

	AddNumber("023602", MAKE_HIDDEN_UNTIL_LICENSED | WORKGROUP, NULL, NULL, &idmMinLength, 1, 1, 1, 3116, &idmMaxLength); //Note this is a Licensable feature and is updated in the ConfigureLicensableCommands()
	AddNumber("023603", MAKE_HIDDEN_UNTIL_LICENSED | WORKGROUP, NULL, NULL, &idmMaxLength, 3116, 3116, 1, &idmMinLength, 3116); //Note this is a Licensable feature and is updated in the ConfigureLicensableCommands()
	AddNumericList("023604", MAKE_HIDDEN_UNTIL_LICENSED | WORKGROUP, NULL, NULL, &idmDfltCodePage, 51, 51, 51, 51); //Note this is a Licensable feature and is updated in the ConfigureLicensableCommands()
#if (DECODER_ID) || defined(RFBASE)
	AddBoolean("023605", MAKE_HIDDEN_UNTIL_LICENSED | WORKGROUP, NULL, NULL, &idmShiftedTiles, false, false); //Note this is a Licensable feature and is updated in the ConfigureLicensableCommands()
	AddNumber("023606", MAKE_HIDDEN_UNTIL_LICENSED | WORKGROUP, NULL, NULL, &idmLowContrast, 1, 1, 0, 3); //Note this is a Licensable feature and is updated in the ConfigureLicensableCommands()
	AddNumber("023607", MAKE_HIDDEN_UNTIL_LICENSED | WORKGROUP, NULL, NULL, &PresenationDecodeDataMatrixLowContrast, 0, 0, 0, 3);
	AddBoolean("023608", MAKE_HIDDEN_UNTIL_LICENSED | WORKGROUP, NULL, NULL, &idmNonSquareModules, false, false); //Note this is a Licensable feature and is updated in the ConfigureLicensableCommands()
	AddNumber("023609", MAKE_HIDDEN_UNTIL_LICENSED | WORKGROUP, NULL, NULL, &idmSmallSymbolSize, 2, 2, 0, 2); //Note this is a Licensable feature and is updated in the ConfigureLicensableCommands()
	AddNumber("02360A", MAKE_HIDDEN_UNTIL_LICENSED | WORKGROUP, NULL, NULL, &PresenationDecodeDataMatrixSymbolSize, 0, 0, 0, 2);
	AddNumber("02360B", MAKE_HIDDEN_UNTIL_LICENSED | WORKGROUP, NULL, NULL, &idmSearchTimeOut, 200, 200, 1, 2000); //Note this is a Licensable feature and is updated in the ConfigureLicensableCommands()

#endif
	//removing until decoder has functionality
	//AddBoolean("IDMAPP", LinearSymbologyCommandModifier | WORKGROUP , NULL, NULL, &idmAppendEnable, true, true);

	// QR settings
	AddCommand("023700", MAKE_HIDDEN_UNTIL_LICENSED | WORKGROUP, NULL, NULL, SUB_TAG_TABLE::SET_WORKING_AND_ACTIVE_VALUES_TO_DEFAULT_FOR_THIS_TAG); //Note this is a Licensable feature and is updated in the ConfigureLicensableCommands()
	AddBoolean("023701", MAKE_HIDDEN_UNTIL_LICENSED | WORKGROUP, NULL, NULL, &qrEnable, false, false); //Note this is a Licensable feature and is updated in the ConfigureLicensableCommands()

	AddNumber("023702", MAKE_HIDDEN_UNTIL_LICENSED | WORKGROUP, NULL, NULL, &qrMinLength, 1, 1, 1, 7089, &qrMaxLength); //Note this is a Licensable feature and is updated in the ConfigureLicensableCommands()
	AddNumber("023703", MAKE_HIDDEN_UNTIL_LICENSED | WORKGROUP, NULL, NULL, &qrMaxLength, 7089, 7089, 1, &qrMinLength, 7089); //Note this is a Licensable feature and is updated in the ConfigureLicensableCommands()
	AddNumericList("023704", MAKE_HIDDEN_UNTIL_LICENSED | WORKGROUP, NULL, NULL, &qrDfltCodePage, 3, 3, 3, 3); //Note this is a Licensable feature and is updated in the ConfigureLicensableCommands()
#if (DECODER_ID) || defined(RFBASE)
	AddBoolean("023705", MAKE_HIDDEN_UNTIL_LICENSED | WORKGROUP, NULL, NULL, &qrcNonSquareModules, false, false); //Note this is a Licensable feature and is updated in the ConfigureLicensableCommands()
	AddNumber("023706", MAKE_HIDDEN_UNTIL_LICENSED | WORKGROUP, NULL, NULL, &qrcSmallSymbolSize, 1, 1, 0, 2); //Note this is a Licensable feature and is updated in the ConfigureLicensableCommands()
	AddNumber("023707", MAKE_HIDDEN_UNTIL_LICENSED | WORKGROUP, NULL, NULL, &PresenationDecodeQRCodeSymbolSize, 0, 0, 0, 2);
#endif
	AddBoolean("023708", LinearSymbologyCommandModifier | WORKGROUP , NULL, NULL, &qrcAppendEnable, true, true);

	// Han Xin Code settings
	AddCommand("023800", MAKE_HIDDEN_UNTIL_LICENSED | WORKGROUP, NULL, NULL, SUB_TAG_TABLE::SET_WORKING_AND_ACTIVE_VALUES_TO_DEFAULT_FOR_THIS_TAG); //Note this is a Licensable feature and is updated in the ConfigureLicensableCommands()
	AddBoolean("023801", MAKE_HIDDEN_UNTIL_LICENSED | WORKGROUP, NULL, NULL, &hxEnable, false, false); //Note this is a Licensable feature and is updated in the ConfigureLicensableCommands()

	AddNumber("023802", MAKE_HIDDEN_UNTIL_LICENSED | WORKGROUP, NULL, NULL, &hxMinLength, 1, 1, 1, 7833, &hxMaxLength); //Note this is a Licensable feature and is updated in the ConfigureLicensableCommands()
	AddNumber("023803", MAKE_HIDDEN_UNTIL_LICENSED | WORKGROUP, NULL, NULL, &hxMaxLength, 7833, 7833, 1, &hxMinLength, 7833); //Note this is a Licensable feature and is updated in the ConfigureLicensableCommands()


	//***************************************************************************************************************************
	/*Postal Code Decoders*/

	// Korean Post settings
	AddCommand("023900", MAKE_HIDDEN_UNTIL_LICENSED | WORKGROUP, NULL, NULL, SUB_TAG_TABLE::SET_WORKING_AND_ACTIVE_VALUES_TO_DEFAULT_FOR_THIS_TAG); //Note this is a Licensable feature and is updated in the ConfigureLicensableCommands()
	AddBoolean("023901", MAKE_HIDDEN_UNTIL_LICENSED | WORKGROUP, NULL, NULL, &koreanPostEnable, false, false); //Note this is a Licensable feature and is updated in the ConfigureLicensableCommands()
	AddNumber("023902", MAKE_HIDDEN_UNTIL_LICENSED | WORKGROUP, NULL, NULL, &koreanPostMinLength, 4, 4, 2, 80, &koreanPostMaxLength); //Note this is a Licensable feature and is updated in the ConfigureLicensableCommands()
	AddNumber("023903", MAKE_HIDDEN_UNTIL_LICENSED | WORKGROUP, NULL, NULL, &koreanPostMaxLength, 48, 48, 2, &koreanPostMinLength, 80); //Note this is a Licensable feature and is updated in the ConfigureLicensableCommands()
#if (DECODER_ID) || defined(RFBASE)
	AddBoolean("023904", MAKE_HIDDEN_UNTIL_LICENSED | WORKGROUP, NULL, NULL, &koreanPostCheckTransmit, false, false); //Note this is a Licensable feature and is updated in the ConfigureLicensableCommands()
	AddBoolean("023905", MAKE_HIDDEN_UNTIL_LICENSED | WORKGROUP, NULL, NULL, &koreanPostReverse, true, true); //Note this is a Licensable feature and is updated in the ConfigureLicensableCommands()
#endif

	// PostNet settings
	AddBoolean("023A00", MAKE_HIDDEN_UNTIL_LICENSED | WORKGROUP, &ChangePostnet, NULL, &netEnable, false, false); //Note this is a Licensable feature and is updated in the ConfigureLicensableCommands()
	AddBoolean("023A01", MAKE_HIDDEN_UNTIL_LICENSED | WORKGROUP, NULL, NULL, &netChkXmit, false, false); //Note this is a Licensable feature and is updated in the ConfigureLicensableCommands()

	// Planet Code settings
	AddBoolean("023B00", MAKE_HIDDEN_UNTIL_LICENSED | WORKGROUP, &ChangePlanetCode, NULL, &plnEnable, false, false); //Note this is a Licensable feature and is updated in the ConfigureLicensableCommands()
	AddBoolean("023B01", MAKE_HIDDEN_UNTIL_LICENSED | WORKGROUP, NULL, NULL, &plnChkXmit, false, false); //Note this is a Licensable feature and is updated in the ConfigureLicensableCommands()

	// British Post settings
	AddBoolean("023C00", MAKE_HIDDEN_UNTIL_LICENSED | WORKGROUP, &ChangeBritishPost, NULL, &bpoEnable, false, false); //Note this is a Licensable feature and is updated in the ConfigureLicensableCommands()

	// Japnaese Post settings
	AddBoolean("023D00", MAKE_HIDDEN_UNTIL_LICENSED | WORKGROUP, &ChangeJapanPost, NULL, &japEnable, false, false); //Note this is a Licensable feature and is updated in the ConfigureLicensableCommands()

	//Canadian Post settings
	AddNumber("023E00", MAKE_HIDDEN_UNTIL_LICENSED | WORKGROUP, &ChangeCanadianPost, NULL, &canEnable, 0, 0, 0, 2); //Note this is a Licensable feature and is updated in the ConfigureLicensableCommands()

	// Kix Post settings
	AddBoolean("023F00", MAKE_HIDDEN_UNTIL_LICENSED | WORKGROUP, &ChangeKix, NULL, &kixEnable, false, false); //Note this is a Licensable feature and is updated in the ConfigureLicensableCommands()

	//Australian Post settings
	AddBoolean("024000", MAKE_HIDDEN_UNTIL_LICENSED | WORKGROUP, &ChangeAusPost, NULL, &ausEnable, false, false); //Note this is a Licensable feature and is updated in the ConfigureLicensableCommands()

#if (DECODER_ID) || defined(RFBASE)
	AddBoolean("024001", MAKE_HIDDEN_UNTIL_LICENSED | WORKGROUP, NULL, NULL, &ausFCCenable, false, false); //Note this is a Licensable feature and is updated in the ConfigureLicensableCommands()
#endif
	AddBoolean("024002", MAKE_HIDDEN_UNTIL_LICENSED | WORKGROUP, NULL, NULL, &ausBarOutput, false, false); //Note this is a Licensable feature and is updated in the ConfigureLicensableCommands()
	AddNumber("024003", MAKE_HIDDEN_UNTIL_LICENSED | WORKGROUP, NULL, NULL, &ausCustIntrpt, 0, 0, 0, 3); //Note this is a Licensable feature and is updated in the ConfigureLicensableCommands()

	/* usps 4 state */
	AddBoolean("024100", MAKE_HIDDEN_UNTIL_LICENSED | WORKGROUP, &ChangeUPU, NULL, &uspsS18TagEnable, false, false); //Note this is a Licensable feature and is updated in the ConfigureLicensableCommands()

	AddBoolean("024200", MAKE_HIDDEN_UNTIL_LICENSED | WORKGROUP, &ChangeUSPS, NULL, &usps4cbEnable, false, false); //Note this is a Licensable feature and is updated in the ConfigureLicensableCommands()

	AddNumber("024300", MAKE_HIDDEN_UNTIL_LICENSED | WORKGROUP, NULL, NULL, &CBQAMode, 0, 0, 0, 2); //Note this is a Licensable feature and is updated in the ConfigureLicensableCommands()

#if (DECODER_ID) || defined(RFBASE)
	AddNumber("021D05", SUB_TAG_TABLE::FULLY_VISIBLE | WORKGROUP, &ChangePostal, NULL, &postalEnable, 0, 0, 0, (POSTAL_OPTIONS-1));
	AddNumber("024400", SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG | WORKGROUP, NULL, NULL, &royalMailMinLengthNoFormatCheck, 0, 0, 0, 11);
#endif

	//***************************************************************************************************************************
	/*OCR decoders*/
#if 0

#else
#if (DECODER_ID)|| defined(RFBASE)
	AddCommand("024500", SUB_TAG_TABLE::FULLY_VISIBLE, NULL, NULL, SUB_TAG_TABLE::SET_WORKING_AND_ACTIVE_VALUES_TO_DEFAULT_FOR_THIS_TAG); //Note this is a Licensable feature and is updated in the ConfigureLicensableCommands()
	AddNumber("024501", SUB_TAG_TABLE::FULLY_VISIBLE, NULL, NULL, &ocrEnable, 0, 0, 0, 3); //Note this is a Licensable feature and is updated in the ConfigureLicensableCommands()
#if defined(RFBASE)
	AddString("024502", MAKE_HIDDEN_UNTIL_LICENSED | SUB_TAG_TABLE::STRING_MUST_BE_QUOTED | WORKGROUP, NULL, NULL, ocrTemplate, SIZE_OF_OCR_TEMPLATE, "13777777770", "13777777770"); //Note this is a Licensable feature and is updated in the ConfigureLicensableCommands()
#else
	AddString("024502", SUB_TAG_TABLE::FULLY_VISIBLE, &ParseOCRTemplate, NULL, ocrTemplate, SIZE_OF_OCR_TEMPLATE, "13777777770", "13777777770"); //Note this is a Licensable feature and is updated in the ConfigureLicensableCommands()
#endif // endof # else if defined(RFBASE)

	AddNumericList("024503", SUB_TAG_TABLE::FULLY_VISIBLE, NULL, NULL, &ocrActiveTemplates, OCR_TMP_USER, OCR_TMP_USER, OCR_TMP_NONE, OCR_TMP_USER, OCR_TMP_PASSPORT, //Note this is a Licensable feature and is updated in the ConfigureLicensableCommands()
		OCR_TMP_ISBN, OCR_TMP_U_I, OCR_TMP_PRICE_FIELD, OCR_TMP_PF_U, OCR_TMP_PF_I, OCR_TMP_PF_U_I, OCR_TMP_MICR, OCR_TMP_M_U, OCR_TMP_M_I, OCR_TMP_M_U_I, OCR_TMP_M_PF, OCR_TMP_M_U_PF,
		OCR_TMP_M_I_PF, OCR_TMP_M_U_I_PF );
	AddBoolean("024504", SUB_TAG_TABLE::FULLY_VISIBLE, NULL, NULL, &ocrBusyBackground, false, false); //Note this is a Licensable feature and is updated in the ConfigureLicensableCommands()
#else
	AddCommand("024500", MAKE_HIDDEN_UNTIL_LICENSED | WORKGROUP, NULL, NULL, SUB_TAG_TABLE::SET_WORKING_AND_ACTIVE_VALUES_TO_DEFAULT_FOR_THIS_TAG); //Note this is a Licensable feature and is updated in the ConfigureLicensableCommands()

	AddNumericList("024501", MAKE_HIDDEN_UNTIL_LICENSED | WORKGROUP, NULL, NULL, &ocrEnable, OCR_OFF, OCR_OFF, OCR_OFF, OCR_A, OCR_B, OCR_CURRENCY, E13B_MICR, OCR_SEMI); //Note this is a Licensable feature and is updated in the ConfigureLicensableCommands()

	AddString("024502", MAKE_HIDDEN_UNTIL_LICENSED | SUB_TAG_TABLE::STRING_MUST_BE_QUOTED | WORKGROUP, NULL, NULL, ocrTemplate, SIZE_OF_OCR_TEMPLATE, "dddddddd", "dddddddd"); //Note this is a Licensable feature and is updated in the ConfigureLicensableCommands()
	AddString("024505", MAKE_HIDDEN_UNTIL_LICENSED | SUB_TAG_TABLE::STRING_MUST_BE_QUOTED | WORKGROUP, NULL, NULL, ocrGroupg, SIZE_OF_OCR_GROUP_G, "", ""); //Note this is a Licensable feature and is updated in the ConfigureLicensableCommands()
	AddString("024506", MAKE_HIDDEN_UNTIL_LICENSED | SUB_TAG_TABLE::STRING_MUST_BE_QUOTED | WORKGROUP, NULL, NULL, ocrGrouph, SIZE_OF_OCR_GROUP_H, "", ""); //Note this is a Licensable feature and is updated in the ConfigureLicensableCommands()
	AddString("024507", MAKE_HIDDEN_UNTIL_LICENSED | SUB_TAG_TABLE::STRING_MUST_BE_QUOTED | WORKGROUP, NULL, NULL, ocrCheckSet, SIZE_OF_OCR_CHECKSET, "", ""); //Note this is a Licensable feature and is updated in the ConfigureLicensableCommands()

#endif
#endif


#if (DECODER_AD)
	//BarNoBar
	AddBoolean("024600", MAKE_HIDDEN_UNTIL_LICENSED | WORKGROUP, NULL, NULL, &BarNoBarEnable, false, false); //Note this is a Licensable feature and is updated in the ConfigureLicensableCommands()

	//hhr format represents the 1/2 hour period of the day in 2-digit hour and even/odd modifier
	//even = 00-29min = r = 0, odd = 30-59min = r = 3
	//Example:
	//  6:30pm = 37th half hour of the day
	//  6 = 18th hour, 30 is the odd part of that hour
	//  HHR1 = 183 output, HHR0 = 37 output, BNB spec says to output HHR1, so it is default
	AddBoolean("024601", SUB_TAG_TABLE::FULLY_VISIBLE | SUB_TAG_TABLE::SYMBOLOGY_ENABLE_COMMAND | WORKGROUP, NULL, NULL, &BarNoBarHhrFormat, true, true);
	//machine id as hex (1) or decimal (0), spec say hex, so it is default
	AddBoolean("024602", SUB_TAG_TABLE::FULLY_VISIBLE | SUB_TAG_TABLE::SYMBOLOGY_ENABLE_COMMAND | WORKGROUP, NULL, NULL, &BarNoBarHexFormat, true, true);
#endif
	//***************************************************************************************************************************
	/*General Decoder Menu Items*/

#if (DECODER_ID) || defined(RFBASE)
	if(ControlMask & MAKE_2D_COMMANDS_AVAILABLE) AddNumber("024700", SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG | WORKGROUP, NULL, NULL, &PrintWeight, 4, 4, 1, 7);
	else AddNumber("024700", NULL | WORKGROUP, NULL, NULL, &PrintWeight, 4, 4, 1, 7);
	DDDiag = 0;
#else
	AddNumber("024700", Two_DCommandModifier | WORKGROUP, NULL, NULL, &PrintWeight, 4, 4, 1, 7);
	AddBoolean("024701", Two_DCommandModifier | WORKGROUP, NULL, NULL, &PrintLocalization, false, false);
	AddNumber("024800", Two_DCommandModifier | WORKGROUP, NULL, NULL, &DDDiag, 0, 0, 0, 5);
#endif
	AddBoolean("024900", SUB_TAG_TABLE::FULLY_VISIBLE | WORKGROUP, NULL, NULL, &ShowMenuSymbols, false, false); 		//put menu symbols out the interface

	if(ControlMask & MAKE_2D_COMMANDS_AVAILABLE)
	{
#if defined(RFBASE) || defined(RFSCAN)
		AddNumber	("024A00", SUB_TAG_TABLE::FULLY_VISIBLE | WORKGROUP, NULL, NULL, &ScanDriverTimeout, 1, 1, 0, 999999);
#else
		//FIXME
		//FIXME		if ((CHal::WhatBoardAmI() >= 0x20) && (CHal::WhatBoardAmI() < 0x30)) // if 4010/80 we want to save power by default
		//FIXME			AddNumber	("SDRTIM", SUB_TAG_TABLE::FULLY_VISIBLE | WORKGROUP, NULL, NULL, &ScanDriverTimeout, 1, 1, 0, 999999);
		//FIXME		else
		AddNumber	("024A00", SUB_TAG_TABLE::FULLY_VISIBLE | WORKGROUP, NULL, NULL, &ScanDriverTimeout, 1, 1, 0, 999999);
#endif

	}
	else
		AddNumber	("024A00", NULL | WORKGROUP, NULL, NULL, &ScanDriverTimeout, 1, 1, 1, 1);  /*FOR 1d THIS IS NOT VISIBLE AND SET TO 1ms*/

	AddNumber("024B00", SUB_TAG_TABLE::FULLY_VISIBLE | WORKGROUP, NULL, NULL, &VideoReverse, 0, 0, 0, 2);

#if (DECODER_AD)
	if(ControlMask & MAKE_2D_COMMANDS_AVAILABLE)
	{
		AddBoolean("024C00", SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG | WORKGROUP, NULL, NULL, &AutoAdaptive, true, true);
		AddString("024C01", SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG | SUB_TAG_TABLE::STRING_MUST_BE_QUOTED | WORKGROUP,
		          NULL, NULL, PriorityCodeIds, SIZE_OF_PRIORITY_IDS, "", "");
	}
	else
	{
		AddBoolean("024C00", NULL | WORKGROUP, NULL, NULL, &AutoAdaptive, true, true);
		AddString("024C01", NULL | WORKGROUP, NULL, NULL, PriorityCodeIds, SIZE_OF_PRIORITY_IDS, "", "");
	}
#endif


#if (DECODER_ID) || defined(RFBASE)
		AddNumber("024D00", SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG | WORKGROUP, NULL, NULL, &DecoderECIHandling, 2, 2, 0, 2);
		AddBoolean("024D01", Two_DCommandModifier | WORKGROUP, NULL, NULL, &CellPhoneMode, false, false);
		AddBoolean("024D02", Two_DCommandModifier | WORKGROUP, NULL, NULL, &ImproveBounds, false, false);
		if(ControlMask & MAKE_2D_COMMANDS_AVAILABLE)
		{
			AddBoolean("024D03", SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG | WORKGROUP, NULL, NULL, &LowApsectRatioLinearFinding, true, true);
			AddBoolean("024D04", SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG | WORKGROUP, NULL, NULL, &UseCyclingFinder, true, true);
			AddBoolean("024E00", SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG | WORKGROUP, NULL, NULL, &PresenationDecodeCyclingFinder, true, true);
			AddNumericList("024D05", SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG | WORKGROUP, NULL, NULL, &LowSymbolContrast, LSC_LINEARS_PDF_NOTI25, LSC_LINEARS_PDF_NOTI25, LSC_NONE, LSC_LINEARS_PDF_NOTI25,LSC_I25,LSC_ALL);
			AddNumericList("024E01", SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG | WORKGROUP, NULL, NULL, &PresenationDecodeLowConstrastImprovements, LSC_NONE, LSC_NONE, LSC_NONE, LSC_LINEARS_PDF_NOTI25,LSC_I25,LSC_ALL);
			AddBoolean("024D06", SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG | WORKGROUP, NULL, NULL, &ForcePresentationStyleDecode, false, false);
		}
		else
		{
			AddBoolean("024D03", Two_DCommandModifier | WORKGROUP, NULL, NULL, &LowApsectRatioLinearFinding, false, false);
			AddBoolean("024D04", Two_DCommandModifier | WORKGROUP, NULL, NULL, &UseCyclingFinder, true, true);
			AddBoolean("024E00", Two_DCommandModifier | WORKGROUP, NULL, NULL, &PresenationDecodeCyclingFinder, true, true);
			AddNumericList("024D05", Two_DCommandModifier | WORKGROUP, NULL, NULL, &LowSymbolContrast, LSC_LINEARS_PDF_NOTI25, LSC_LINEARS_PDF_NOTI25, LSC_NONE, LSC_LINEARS_PDF_NOTI25,LSC_I25,LSC_ALL);
			AddNumericList("024E01", Two_DCommandModifier | WORKGROUP, NULL, NULL, &PresenationDecodeLowConstrastImprovements, LSC_NONE, LSC_NONE, LSC_NONE, LSC_LINEARS_PDF_NOTI25,LSC_I25,LSC_ALL);
			AddBoolean("024D06", Two_DCommandModifier | WORKGROUP, NULL, NULL, &ForcePresentationStyleDecode, false, false);
		}
		if(ControlMask & MAKE_LINEAR_SYMBOLOGIES_AVAILABLE)
			AddNumber("024D07",SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG | WORKGROUP, NULL, NULL, &LinearShortQuietZones, 1, 1, 0, 2); /* 2 for now to make adaptus like.*/
		else
			AddNumber("024D07",LinearSymbologyCommandModifier | WORKGROUP, NULL, NULL, &LinearShortQuietZones, 1, 1, 0, 2); /* 2 for now to make adaptus like.*/
#else
	if(ControlMask & MAKE_2D_COMMANDS_AVAILABLE)
		AddNumericList("024D08", Two_DCommandModifier | WORKGROUP, NULL, NULL, &DecodeMode, FULLSCREEN_OMNI, FULLSCREEN_OMNI,
		               FULLSCREEN_OMNI,QUICK_OMNI,NON_OMNI_ALD,OMNI_LINEAR);
	else
		AddNumericList("024D08", Two_DCommandModifier | WORKGROUP, NULL, NULL, &DecodeMode, NON_OMNI_ALD, NON_OMNI_ALD,
		               FULLSCREEN_OMNI,QUICK_OMNI,NON_OMNI_ALD,OMNI_LINEAR);
#endif

#ifdef MULTIPLE_DECODE_CORES
	//Decoder Cores
	AddNumericList("024D09", Two_DCommandModifier | WORKGROUP, NULL, NULL, &DecoderCore1, DEC_TYPE_ID, DEC_TYPE_ID,
	               DEC_TYPE_NONE,DEC_TYPE_ID,DEC_TYPE_ADAPTUS);
	AddNumericList("024D0A", Two_DCommandModifier | WORKGROUP, NULL, NULL, &DecoderCore2, DEC_TYPE_NONE, DEC_TYPE_NONE,
	               DEC_TYPE_NONE,DEC_TYPE_ID,DEC_TYPE_ADAPTUS);
	//Core optimization Mode
	AddNumericList("024D0B", Two_DCommandModifier | WORKGROUP, NULL, NULL, &DecoderCoreOptimize1, DEC_OPTIMIZE_NONE, DEC_OPTIMIZE_NONE,
	               DEC_OPTIMIZE_NONE,DEC_OPTIMIZE_DM);
	AddNumericList("024D0C", Two_DCommandModifier | WORKGROUP, NULL, NULL, &DecoderCoreOptimize2, DEC_OPTIMIZE_DM, DEC_OPTIMIZE_DM,
	               DEC_OPTIMIZE_NONE,DEC_OPTIMIZE_DM);
#endif

	//For Manual triggers
#ifndef XENON_LITE_1500
	AddNumber("024D0D", SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG  | WORKGROUP, NULL, NULL, &AuxiliaryDecoder, 1, 1, 0, 2);
#else
	AddNumber("024D0D", SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG  | WORKGROUP, NULL, NULL, &AuxiliaryDecoder, 0, 0, 0, 2);
#endif
	//For Streaming Presentation Trigger
	AddNumber("024E02", SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG  | WORKGROUP, NULL, NULL, &PresentationAuxiliaryDecoder, 0, 0, 0, 2);

	AddNumericList("024F00", Two_DCommandModifier | WORKGROUP, NULL, NULL, &iEngineRotation, IMAGER_UPRIGHT, IMAGER_UPRIGHT,
	               IMAGER_UPRIGHT,IMAGER_RIGHT_SIDE_DOWN,IMAGER_UPSIDE_DOWN,IMAGER_LEFT_SIDE_DOWN);


	/*	params for image centering */
	AddNumber("024D0E", SUB_TAG_TABLE::FULLY_VISIBLE | WORKGROUP, NULL, NULL, &DecodeInWindow, 0, 0, 0, 3);
	AddNumber("024D0F", Two_DCommandModifier | WORKGROUP, NULL, NULL, &WindowTopY, 40, 40, 0, 100, &WindowBotY);
	AddNumber("024D10", Two_DCommandModifier | WORKGROUP, NULL, NULL, &WindowBotY, 60, 60, 0, &WindowTopY, 100);
	AddNumber("024D11", SUB_TAG_TABLE::FULLY_VISIBLE | WORKGROUP, NULL, NULL, &WindowTopX, 40, 40, 0, 100, &WindowBotX);
	AddNumber("024D12", SUB_TAG_TABLE::FULLY_VISIBLE | WORKGROUP, NULL, NULL, &WindowBotX, 60, 60, 0, &WindowTopX, 100);
	AddNumber("025000", SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG | WORKGROUP, NULL, NULL, &ShowDecodeWindow, 0, 0, 0, 3);

	//ewr04791 Codegate
	/*	params for image centering - presentation type trigger */
	AddNumber("024E03", SUB_TAG_TABLE::FULLY_VISIBLE | WORKGROUP, NULL, NULL, &PresentationDecodeInWindow, 0, 0, 0, 3);
	AddNumber("024E04", Two_DCommandModifier | WORKGROUP, NULL, NULL, &PresentationWindowTopY, 40, 40, 0, 100, &PresentationWindowBotY);
	AddNumber("024E05", Two_DCommandModifier | WORKGROUP, NULL, NULL, &PresentationWindowBotY, 60, 60, 0, &PresentationWindowTopY, 100);
	AddNumber("024E06", SUB_TAG_TABLE::FULLY_VISIBLE | WORKGROUP, NULL, NULL, &PresentationWindowTopX, 40, 40, 0, 100, &PresentationWindowBotX);
	AddNumber("024E07", SUB_TAG_TABLE::FULLY_VISIBLE | WORKGROUP, NULL, NULL, &PresentationWindowBotX, 60, 60, 0, &PresentationWindowTopX, 100);

#if (DECODER_ID) || defined(RFBASE)
	AddBoolean("024D13", SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG | WORKGROUP, NULL, NULL, &DecodePassThrough, false, false);
	AddBoolean("024E08", SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG | WORKGROUP, NULL, NULL, &PresenationDecodePassThrough, true, true);
	AddBoolean("024D14", SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG | WORKGROUP, NULL, NULL, &DecodeSubpixelFinder, true, true);
	AddBoolean("024E09", SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG | WORKGROUP, NULL, NULL, &PresenationDecodeSubpixelFinder, false, false);
	AddBoolean("024D15", SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG | WORKGROUP, NULL, NULL, &DecodeEdgeDetector, true, true);
	AddBoolean("024E0A", SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG | WORKGROUP, NULL, NULL, &PresenationDecodeEdgeDetector, false, false);
#endif

	AddNumber("024D16", SUB_TAG_TABLE::FULLY_VISIBLE | WORKGROUP, NULL, NULL, &DecodeBox, 0, 0, 0, 2);
	AddNumber("024D17", SUB_TAG_TABLE::FULLY_VISIBLE | WORKGROUP, NULL, NULL, &DecodeBoxWidth, 68, 68, 1, 100);
	AddNumber("024D18", SUB_TAG_TABLE::FULLY_VISIBLE | WORKGROUP, NULL, NULL, &DecodeBoxHeight, 60, 60, 1, 100);

#if (DECODER_ID) || defined(RFBASE)
	AddBoolean("024D1A", SUB_TAG_TABLE::FULLY_VISIBLE | WORKGROUP, NULL, NULL, &UndoMirroredImage, FALSE, FALSE);
#else
	AddBoolean("024D19", Two_DCommandModifier | WORKGROUP, NULL, NULL, &DecodeZoom, FALSE, FALSE);
	AddNumericList("024D1A", SUB_TAG_TABLE::FULLY_VISIBLE | WORKGROUP, NULL, NULL, &UndoMirroredImage, NO_MIRROR, NO_MIRROR,
	               NO_MIRROR, LEFT_TO_RIGHT, TOP_TO_BOTTOM);
	AddNumber("024D1B", Two_DCommandModifier | WORKGROUP, NULL, NULL, &DecodeWidth, 3, 3, 1, 6);
	AddNumber("024D1C", SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG  | WORKGROUP, NULL, NULL, &DecodeSlitHeight, 8, 8, 1, 32);
#endif

#if defined(MANUFACTURING_IMAGER_TEST)

	AddNumber("024D1D", SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG  | WORKGROUP, NULL, NULL, &DecodeControl, 0, 0, 0, 32);
#else

	AddNumber("024D1D", SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG  | WORKGROUP, NULL, NULL, &DecodeControl, 0, 0, 0, 32);
#endif

	AddNumber("024D1E", SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG  | WORKGROUP, NULL, NULL, &DecodeVigor, 2, 2, 0, 2);
#if defined(MANUFACTURING_IMAGER_TEST)

	AddBoolean("024D1F", SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG | WORKGROUP, NULL, NULL, &DecodeMLD, FALSE, FALSE);
#else

	AddBoolean("024D1F", SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG | WORKGROUP, NULL, NULL, &DecodeMLD, TRUE, TRUE);
#endif

#if (DECODER_AD) || defined(MANUFACTURING_IMAGER_TEST)
	AddBoolean("024D20", SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG | WORKGROUP, NULL, NULL, &DecodeReflections, FALSE, FALSE);
#endif

	AddBoolean("025001", SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG | WORKGROUP, NULL, NULL, &ShowSnappyTime, false, false);
	AddBoolean("025002", SUB_TAG_TABLE::FULLY_VISIBLE | WORKGROUP, NULL, NULL, &ShowNoRead, false, false );

	//	Decoder timeouts

	// Core 1 timeout
	AddNumber("024D21", Two_DCommandModifier | WORKGROUP, NULL, NULL, &DecodeMinTimeout, 400, 400, 0, 2500);
	AddNumber("024D22", Two_DCommandModifier | WORKGROUP, NULL, NULL, &DecodeMaxTimeout, 800, 800, 0, 2500);

#ifdef MULTIPLE_DECODE_CORES
	//Core 2 timeout
	AddNumber("024D23", Two_DCommandModifier | WORKGROUP, NULL, NULL, &DecodeMinTimeout2, 100, 100, 0, 2500);
	AddNumber("024D24", Two_DCommandModifier | WORKGROUP, NULL, NULL, &DecodeMaxTimeout2, 400, 400, 0, 2500);

	AddBoolean("024D25", SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG | WORKGROUP, NULL, NULL, &AlternateImages, FALSE, FALSE);
#endif

#if (DECODER_AD)
	AddNumber("024D26", Two_DCommandModifier | WORKGROUP, NULL, NULL, &DecodeCouponRefreshMinTimeout, 50, 50, 0, 200);
	AddNumber("024D27", Two_DCommandModifier | WORKGROUP, NULL, NULL, &DecodeCouponRefreshMaxTimeout, 400,400 , 0, 500);
#endif
	//FIXME
	//	if ((CHal::WhatBoardAmI() >= 0x20) && (CHal::WhatBoardAmI() < 0x30)) // if 4010/80
	//		AddNumber("DECTMA", SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG | WORKGROUP, NULL, NULL, &DecodeAdditionalTimeout, 0, 0, 0, 1000);
	//	else
	AddNumber("024D28", SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG | WORKGROUP, NULL, NULL, &DecodeAdditionalTimeout, 50, 50, 0, 1000);
	AddNumber("025100", SUB_TAG_TABLE::FULLY_VISIBLE | WORKGROUP, NULL, NULL, &DecodeAddendaTimeout, 120, 120, 0, 500);
	AddNumber("025101", SUB_TAG_TABLE::FULLY_VISIBLE | WORKGROUP, NULL, NULL, &DecodeConCatTimeout, 800, 800, 10, 4000);
	AddNumber("025102", SUB_TAG_TABLE::FULLY_VISIBLE | WORKGROUP, NULL, NULL, &DecodeUPC_CompTimeout, 300, 300, 10, 1000);

	AddNumber("024E0B",SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG | WORKGROUP, NULL, NULL, &PresentationDecodeVigor, 0, 0, 0, 2);
	AddNumber("024E0C",SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG | WORKGROUP, NULL, NULL, &PresentationDecodeControl, 0, 0, 0, 32);
	AddNumber("024E0D",SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG | WORKGROUP, NULL, NULL, &PresentationDecodeMinTimeout, 150, 150, 0, 2500);
	AddNumber("024E0E",SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG | WORKGROUP, NULL, NULL, &PresentationDecodeMaxTimeout, 200, 200, 0, 2500);

#if (DECODER_AD)
	AddNumber("024E0F",SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG | WORKGROUP, NULL, NULL, &PresentationCouponRefreshMinTimeout, 50, 50, 0, 200);
	AddNumber("024E10",SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG | WORKGROUP, NULL, NULL, &PresentationCouponRefreshMaxTimeout, 175, 175, 0, 200);
#endif

	//	Other
	AddNumber("024D29", SUB_TAG_TABLE::FULLY_VISIBLE | WORKGROUP, NULL, NULL, &GeneralDecodeImprovements, 0, 0, 0, 3);
	AddNumber("025200", SUB_TAG_TABLE::FULLY_VISIBLE | WORKGROUP, NULL, NULL, &Shotgun, 0, 0, 0, 10);

	//		if(ControlMask & MAKE_2D_COMMANDS_AVAILABLE)
	//		{
#ifdef VUQUEST3310
	AddNumericList("025300", SUB_TAG_TABLE::FULLY_VISIBLE | WORKGROUP, NULL, NULL, &SequenceEnabled, SEQUENCE_DISABLED, SEQUENCE_DISABLED,
	               SEQUENCE_DISABLED,SEQUENCE_ENABLED,SEQUENCE_REQUIRED);
#else
	AddNumericList("025300", SUB_TAG_TABLE::FULLY_VISIBLE | WORKGROUP | SUB_TAG_TABLE::SYMBOLOGY_ENABLE_COMMAND, NULL, NULL, &SequenceEnabled, SEQUENCE_DISABLED, SEQUENCE_DISABLED,
	               SEQUENCE_DISABLED,SEQUENCE_ENABLED,SEQUENCE_REQUIRED);
#endif
	//		}
	//		else
	//		{
	//			AddNumericList("SEQ_EN", NULL, NULL, NULL, &SequenceEnabled, SEQUENCE_DISABLED, SEQUENCE_DISABLED,
	//				SEQUENCE_DISABLED,SEQUENCE_ENABLED,SEQUENCE_REQUIRED);
	//		}


	AddHexString("025301",	SUB_TAG_TABLE::FULLY_VISIBLE | WORKGROUP, NULL, &TerminateSeqBlock, &SequenceBlock,SIZE_OF_SEQUENCE_BLOCK, "ffff", "ffff");
	AddCommand("025302", SUB_TAG_TABLE::FULLY_VISIBLE | WORKGROUP, NULL, NULL, SUB_TAG_TABLE::SET_WORKING_AND_ACTIVE_VALUES_TO_DEFAULT_FOR_THIS_TAG);

#ifdef VUQUEST3310
#else
	AddBoolean("025400", SUB_TAG_TABLE::FULLY_VISIBLE | SUB_TAG_TABLE::SYMBOLOGY_ENABLE_COMMAND | WORKGROUP, NULL, NULL, &PrioritySequence, false, false);
#endif
	AddNumber("025401", SUB_TAG_TABLE::FULLY_VISIBLE | WORKGROUP, NULL, NULL, &PrioritySequenceTimeValue, 500, 500, 100, 3000);
	AddHexString("025402",	SUB_TAG_TABLE::FULLY_VISIBLE | WORKGROUP, NULL, &TerminateNonPreferredBlock, &NonPreferredBlock,SIZE_OF_NON_PREFERRED_BLOCK, "ffff", "ffff");
	AddHexString("025403",	SUB_TAG_TABLE::FULLY_VISIBLE | WORKGROUP, NULL, &TerminateCodePreferredBlock, &CodePreferredBlock,SIZE_OF_CODE_PREFERRED_BLOCK, "ffff", "ffff");
	AddCommand("025404", SUB_TAG_TABLE::FULLY_VISIBLE | WORKGROUP, NULL, NULL, SUB_TAG_TABLE::SET_WORKING_AND_ACTIVE_VALUES_TO_DEFAULT_FOR_THIS_TAG);

	AddBoolean("024D2A", SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG | WORKGROUP, NULL, NULL, &DecoderExtraInfo, false, false);

#if (DECODER_AD)
	AddCommand("024D2B", SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG | LOCAL_SETTING | SUB_TAG_TABLE::RETAIN_EXTRANEOUS_TEXT_AFTER_COMMAND,	&DisplayDecoderSetup, NULL);
	AddNumber("024D2C", SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG | WORKGROUP, NULL, NULL, &SudokuEnabled, 0, 0, 0, 2);
	AddCommand("024D2D", SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG | LOCAL_SETTING,	&DisplayDecoderData, NULL);
#endif
	AddCommand("024D2E", SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG | LOCAL_SETTING, &ResetDecoderWorkspace, NULL);

#if defined(DEVELOPMENT_BUILD)

	AddBoolean("024D2F", SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG | WORKGROUP, NULL, NULL, &DisableDecoder, false, false);
	AddBoolean("024D30", SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG | WORKGROUP, NULL, NULL, &DecodeInjectedImage, false, false);
#endif

	AddNumber("024702", LinearSymbologyCommandModifier | WORKGROUP, NULL, NULL, &PrintResults, 0, 0, 0, 3);
	AddNumber("024D31", LinearSymbologyCommandModifier | WORKGROUP, NULL, NULL, &DecodeSecurityLevel, 2, 2, 0, 3);
	AddNumber("025500", SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG | WORKGROUP, NULL, NULL, &SwiftDebug, 0, 0, 0, 999999999);

#if (DECODER_ID) || defined(RFBASE)
	AddCommand("024D32",SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG | LOCAL_SETTING | SUB_TAG_TABLE::RETAIN_EXTRANEOUS_TEXT_AFTER_COMMAND,
				&ChangeDecoderSetting, NULL);
#endif
   AddNumber("024D33", SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG | WORKGROUP, NULL, NULL, &DecodersToUse, 2, 2, 0, 15);
   //1=adaptus,2=ID,4=RapidScan


	AddBoolean("025600", LinearSymbologyCommandModifier | WORKGROUP, NULL, NULL, &skipLinear, false, false);


 // may be temporary command, ideally this would be handled by engine ID control intellegently under hood
 // need something for testing

 // Change DECMCI to DECMFI --- 0-force not to interpolation, 1-force to interpolation, 2-use engine ID/Configuration (For Xenon)
	AddNumber("024D34", SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG | WORKGROUP, NULL, NULL, &DecodeMonocolorForceInterpolation,  2, 2, 0, 2);


	return;
}



