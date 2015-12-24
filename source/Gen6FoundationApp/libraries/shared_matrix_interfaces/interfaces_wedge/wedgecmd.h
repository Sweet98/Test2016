/*=================================================================================
  class: CWedgeCmd

//=================================================================================
   $Source: interfaces_wedge/wedgecmd.h $
   $Date: 2009/06/29 05:03:12EDT $
   $Revision:


//=================================================================================*/

#ifndef WEDGE_CMD_H__20DB49DD_8F9C_4ADD_9830_CF227654ADFA__INCLUDED_
#define WEDGE_CMD_H__20DB49DD_8F9C_4ADD_9830_CF227654ADFA__INCLUDED_

#include "WdgType.h"

//struct IFILE;

typedef unsigned char  drvcmds_t;
typedef unsigned short drvparamw_t;
typedef unsigned long  drvparaml_t;

typedef signed long drvdly_t;

class CWedgeCmd
{
public:
	CWedgeCmd();
//	virtual ~CWedgeCmd();

	void AddCommand(drvcmds_t cmd);
	void AddCommand(drvcmds_t cmd, drvparamw_t parm);
	void AddCommand(drvcmds_t cmd, drvparamw_t parm1, drvparamw_t parm2);
	void AddCommand(drvcmds_t cmd, drvparaml_t parm);
	void AddEndCommand(void);

	void PrepareWrite(void);
	void PrepareRead(void)	{ PrepareWrite();	}

	void AddDelayCommand(long delay);
//	void AddMediumDelayCommand(void);
	void AddScancode(drvparamw_t scancode, drvparamw_t exinfo);

	drvcmds_t GetCommand(void);
	drvparamw_t GetParameterW(void);
	drvparaml_t GetParameterL(void);

	enum
	{
		/* commands for the interface driver */
		CMD_NOP					=	0x00,
		CMD_WAITMIRCO			=	0x40,	/* wait x micro seconds (short) */
		CMD_WAITMIRCO_LONG	=	0x50,	/* wait x micro seconds (long)  */
		CMD_SCANCODE			=	0x80,	// send scancode
		CMD_END					=	0xF0	/* end comand */
	};
protected:
	enum
	{
		DRVCMD_BUFFER_SIZE =	128*sizeof(drvcmds_t)*(1+4)
	};

	drvcmds_t m_CmdBuffer[DRVCMD_BUFFER_SIZE];
	drvcmds_t *m_pCmd;
};


#endif // WEDGE_CMD_H__20DB49DD_8F9C_4ADD_9830_CF227654ADFA__INCLUDED_




