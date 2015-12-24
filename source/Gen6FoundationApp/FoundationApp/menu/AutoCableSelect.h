//======================================================================================
// Revision information
//======================================================================================
// $RCSfile: Gen6FoundationApp/FoundationApp/menu/AutoCableSelect.h $
// $Revision: 1.7 $
// $Date: 2011/02/10 13:47:44EST $
//======================================================================================

//======================================================================================
#ifndef AUTOCABLESELECT_H_
#define AUTOCABLESELECT_H_

typedef enum
{
	Interface_USB_Group,
	Interface_232_Group,
	Interface_KBDWDG_Group,
	Interface_485_Group,
	Interface_Fac232_Group,
	Interface_Unknow
}CABLE_STATES;

///////////////////read BANK_SEL, CABLE_SEL0,CABLE_SEL1//////////////////////////////////////////
#define BANK_SEL_PINSTATUS()            (READ_FIELD(GPIO1_PSR0,GPIO_BIT3_LSH,GPIO_BIT3_WID))
#define CABLE_SEL0_PINSTATUS()          (READ_FIELD(GPIO1_PSR0,GPIO_BIT12_LSH,GPIO_BIT12_WID))
#define CABLE_SEL1_PINSTATUS()		    (READ_FIELD(GPIO1_PSR0,GPIO_BIT13_LSH,GPIO_BIT13_WID))
/////////////////////////
void AutoCable_Ctl_Sys_Control(void);
void AutoCable_Ctl_Sys_Initialization(void);
#if defined(DISABLE_AUTO_CBL)
void ManualCable_Ctl_Sys_Initialization(void);
#endif
void ConfigCable_Ctl_Sys_Control();
void Cable_Sel_Sys_stateChangeToUnknow();
void Cable_Sel_Sys_StateChangeTo232Group();
void Cable_Sel_Sys_StateChangeToUSBGroup();
void Cable_Sel_Sys_StateChangeTo485Group();
void Cable_Sel_Sys_StateChangeToKBWGroup();
CABLE_STATES AutoCable_Initialize_Interface();

bool UpdateNumericListWorkingValue(const char *SettingName1,const char*SettingName2);           

extern CABLE_STATES PresentInterface;
extern int AutoCable_StrIndex(unsigned char *str,unsigned char *substr);

#endif

// 
