/************************************************************************/
//
// HHP Matrix embedded firmware
//
//  SYN Command Parser definitions
//  Date:       3/27/02
//  Tools:      ARM SDT 2.5 for starters... will be ARM ADS 1.1 or Metaware
//  Target:     D-ball 4480
//
//======================================================================================
//	Revision information
//======================================================================================
// $RCSfile: SynCommandParser.h $
// $Revision: 1.12 $
// $Date: 2011/11/08 18:02:48EST $
//======================================================================================
/************************************************************************/
/*|======================================================================
  |		   MODIFICATION HISTORY
  |======================================================================
*/
#ifndef __DB_SYNCOMMANDPARSE_H         /*  prevent redundant definitions   */
#define __DB_SYNCOMMANDPARSE_H  1
#ifndef SIZEOF_HIGHLEVEL_PARSE_BUFFER
#define SIZEOF_HIGHLEVEL_PARSE_BUFFER 3000
#endif

#define FUNCTION_WAITING_FOR_RESPONSE 1
#define NOT_WAITING_FOR_RESPONSE 0
#define SYN_COMMAND_CCB_TRIGGER_LOCK	"\x02M\rRF_INB1!"
#define SYN_COMMAND_COB_TRIGGER_LOCK	"\x02M\rRF_INB3!"

#if defined(RFBASE) || defined(RFSCAN)
void SendOWCommandDataIndicateResponse(HSTRING *, unsigned int, unsigned int iMenuZero_CmdOne = 0);  //default for menu response
void SendOWData(HSTRING *, unsigned int);
bool IsOWReadyToSend (void);
bool IsOWDataAvailable(void);
void UpdateWakingInProgress(bool);
void ClearReceiveFlags(void);
int GetOWData (void);
void GetOWCharData( char * data, int * size );
#endif

#if defined(RFBASE)
bool IsScannerInCradle(void);
#endif

class HSTRING;
extern HSTRING * DebugMenuCommandAndResponse;
void ProcessInterfaceData(void);
void ProcessEngineData(void);

class SYN_COMMAND_PARSE
{
private:
	char			*pucBuffer;
	unsigned int	ui;
	unsigned int	uiState;
	SYN_COMMAND_PARSE (const SYN_COMMAND_PARSE&);            // declared private, but not defined, to prevent copying         
	SYN_COMMAND_PARSE& operator=(const SYN_COMMAND_PARSE&);  // declared private, but not defined, to prevent copying 

public:
	bool			Busy;

public:
	// Constructor
	SYN_COMMAND_PARSE(void);
	SYN_COMMAND_PARSE( int BufferSize );

	// Destructor
	~SYN_COMMAND_PARSE( void );

	// Method used by the instantiator to feed received data to this routine.
	bool ServiceSYNCommand(UCHAR ucData);

protected:
	void SendResponse(const HSTRING * pMenuCommand);

};

extern SYN_COMMAND_PARSE ** SYNCommandParser;
extern SYN_COMMAND_PARSE * OWSYNCommandParser;
extern SYN_COMMAND_PARSE * g_pRemSynCommandParser;

class STX_COMMAND_PARSE
{
private:
	char			*pucBuffer;
	unsigned int	ui;
	unsigned int	uiState;
	STX_COMMAND_PARSE (const STX_COMMAND_PARSE&);            // declared private, but not defined, to prevent copying         
	STX_COMMAND_PARSE& operator=(const STX_COMMAND_PARSE&);  // declared private, but not defined, to prevent copying 

public:
	bool			Busy;

public:
	// Constructor
	STX_COMMAND_PARSE(void);
	STX_COMMAND_PARSE( int BufferSize );

	// Destructor
	~STX_COMMAND_PARSE( void );

	// Method used by the instantiator to feed received data to this routine.
	bool ServiceSTXCommand(UCHAR ucData);

protected:
	void SendResponse(const HSTRING * pMenuCommand);

};

extern STX_COMMAND_PARSE ** STXCommandParser;
extern STX_COMMAND_PARSE * OWSTXCommandParser;
extern STX_COMMAND_PARSE * g_pRemStxCommandParser;

#endif  /*  __DB_SYNCOMMANDPARSE_H  */



