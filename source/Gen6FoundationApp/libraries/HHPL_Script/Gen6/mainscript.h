
/*----------------------------------------------------------

  Written by MAKEHEX from ..\..\DB_APP\DEFAULTMAINSCRIPT.BIN.
  ----------------------------------------------------------*/

#ifndef _HHP_MAINSCRIPT_H_
#define _HHP_MAINSCRIPT_H_ 1

#define MAINSCRIPT_DESTINATION 0x0
#define MAINSCRIPT_SIZE 0xDF
#ifndef CONST
#define CONST const
#endif

extern CONST unsigned char mainscript[MAINSCRIPT_SIZE];
extern CONST unsigned long mainscriptSize;
extern CONST unsigned long mainscriptStart;

#endif /* _HHP_MAINSCRIPT_H_ */

