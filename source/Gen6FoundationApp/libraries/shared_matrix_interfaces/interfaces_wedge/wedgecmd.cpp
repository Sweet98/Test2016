/*=================================================================================
  class: CWedgeCmd

//=================================================================================
   $Source: interfaces_wedge/wedgecmd.cpp $
   $Date: 2009/06/29 05:03:11EDT $
   $Revision: 1.2 $



//=================================================================================*/

#include "stdInclude.h"
#include "CommonDriver.h"
#include "WedgeCmd.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CWedgeCmd::CWedgeCmd()
{
	memset(m_CmdBuffer, CMD_END, DRVCMD_BUFFER_SIZE);
	m_pCmd=NULL;
}

void CWedgeCmd::PrepareWrite(void)
{
	m_pCmd = m_CmdBuffer;
}

/* Writes a command with no parameters into the command buffer */
void CWedgeCmd::AddCommand(drvcmds_t cmd)
{
	ASSERT(m_pCmd!=NULL);
	*m_pCmd++ = cmd;
}

/* Writes a command with a WORD size parameter into the command buffer */
void CWedgeCmd::AddCommand(drvcmds_t cmd, drvparamw_t parm)
{
	ASSERT(m_pCmd!=NULL);
	*m_pCmd++ = cmd;
	*m_pCmd++ = parm>>8;
	*m_pCmd++ = parm&0xFF;
}

/* Writes a command with two WORD size parameters into the command buffer */
void CWedgeCmd::AddCommand(drvcmds_t cmd, drvparamw_t parm1, drvparamw_t parm2)
{
	ASSERT(m_pCmd!=NULL);
	*m_pCmd++ = cmd;
	*m_pCmd++ = (drvcmds_t)(parm1>>8);
	*m_pCmd++ = (drvcmds_t)(parm1&0xFF);
	*m_pCmd++ = (drvcmds_t)(parm2>>8);
	*m_pCmd++ = (drvcmds_t)(parm2&0xFF);
}

/* Writes a command with a LONG size parameter into the command buffer */
void CWedgeCmd::AddCommand(drvcmds_t cmd, drvparaml_t parm)
{
	AddCommand(cmd,  (drvparamw_t) ((parm>>16)&0xffff), (drvparamw_t) (parm&0xffff));
}

/* Writes the end command into the command buffer */
void CWedgeCmd::AddEndCommand(void)
{
	AddCommand(CMD_END);
}

/* Writes the delay command into the command buffer */
/* Most delays are faster than 32K micro seconds. Therefore
	we have two ways to send the delay. In most cases we can
	use the more compact way. This saves space in the command buffer. */
void CWedgeCmd::AddDelayCommand(long delay)
{
	if (delay < 0x7fff)
		AddCommand(CMD_WAITMIRCO, (drvparamw_t)delay);
	else
		AddCommand(CMD_WAITMIRCO_LONG, (drvparaml_t)delay);
}

///* Writes a delay command into the command buffer */
//void CWedgeCmd::AddMediumDelayCommand(IFILE *fp)
//{
//	AddDelayCommand(GetMediumDelay(fp));
//}

/* Writes the scancode command into the command buffer */
void CWedgeCmd::AddScancode(drvparamw_t scancode, drvparamw_t exinfo)
{
	AddCommand(CMD_SCANCODE, scancode, exinfo);
}

/*****************************************************************************
Here are the helpers to read a command out of the command buffer
 *****************************************************************************/

drvcmds_t CWedgeCmd::GetCommand(void)
{
	ASSERT(m_pCmd!=NULL);
	return *m_pCmd++;
}

drvparamw_t CWedgeCmd::GetParameterW(void)
{
	ASSERT(m_pCmd!=NULL);
	drvparamw_t temp;
	temp  = ((drvparamw_t)(*m_pCmd++))<<8;
	temp +=  (drvparamw_t)*m_pCmd++;
	return temp;
}

drvparaml_t CWedgeCmd::GetParameterL(void)
{
	ASSERT(m_pCmd!=NULL);
	drvparaml_t temp;
	temp  = ((drvparaml_t)GetParameterW())<<16;
	temp += GetParameterW();
	return temp;
}




