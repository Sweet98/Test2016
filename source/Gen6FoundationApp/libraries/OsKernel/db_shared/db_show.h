/************************************************************************/
//
// HHP 4480 embedded firmware
//
//  Show Display Functions for boot code
//  Date:       2/24/02
//  Tools:      ARM SDT 2.5 for starters... will be ARM ADS 1.1 or Metaware
//  Target:     D-ball 4480
//
//======================================================================================
//	Revision information
//======================================================================================
// $RCSfile: db_shared/db_show.h $
// $Revision: 1.1.2.2 $
// $Date: 2008/11/11 16:04:11EST $
//======================================================================================
/************************************************************************/
/*|=======================================================================
  |           MODIFICATION HISTORY
  |=======================================================================
*************************************************************************/

#ifndef __DB_SHOW_H         /*  prevent multiple definitions    */
#define __DB_SHOW_H     1


void show_system_firmware_version (void);  // display system firmware version only
void show_system (void) ;	/* display system configuration screen  */
void show_decode (void) ;	/* display decoder configuration screen */
void show_imager (void) ;	/* you get the idea... */
void show_processor_speed (void);	// report processor speed

void internal_reg_dump (void);

#endif  /*  __DB_SHOW_H */



