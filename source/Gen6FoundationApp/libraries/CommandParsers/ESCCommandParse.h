/************************************************************************/
//
// HHP Matrix embedded firmware
//
//  SYN Command Parser definitions
//  Date:       6/22/04
//  Tools:      ADS 1.2
//  Target:     D-ball 4480
//
//======================================================================================
//	Revision information
//======================================================================================
// $RCSfile: ESCCommandParse.h $
// $Revision: 1.2 $
// $Date: 2011/11/08 18:02:47EST $
//======================================================================================
/************************************************************************/
/*|======================================================================
  |		   MODIFICATION HISTORY
  |======================================================================
*/
#ifndef __DB_ESCCOMMANDPARSE_H         /*  prevent redundant definitions   */
#define __DB_ESCCOMMANDPARSE_H  1

class HSTRING;



class ESC_COMMAND_PARSE
{
private:
	HSTRING			*hstrBuffer;
	unsigned int	uiState;
	int				Workgroup;
	HSTRING			*Command;
	ESC_COMMAND_PARSE (const ESC_COMMAND_PARSE&);            // declared private, but not defined, to prevent copying         
	ESC_COMMAND_PARSE& operator=(const ESC_COMMAND_PARSE&);  // declared private, but not defined, to prevent copying 

public:
// Constructor
	ESC_COMMAND_PARSE( int BufferSize );
	
// Destructor
	~ESC_COMMAND_PARSE( void );
	
// Method used by the instantiator to feed received data to this routine.
	bool ServiceESCCommand(unsigned char  ucData);
};

extern ESC_COMMAND_PARSE *ESCCommandParser;
#endif  /*  __DB_ESCCOMMANDPARSE_H  */



