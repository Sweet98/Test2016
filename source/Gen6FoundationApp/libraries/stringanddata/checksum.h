/************************************************************************/
//
// HHP 4480 embedded firmware
// Checksum calculation definitions and interfaces
//
//  Date:       3/20/02
//  Tools:      ARM SDT 2.5 for starters... will be ARM ADS 1.1 or Metaware
//  Target:     D-ball 4480
//
//======================================================================================
//	Revision information
//======================================================================================
// $RCSfile: checksum.h $
// $Revision: 1.7 $
// $Date: 2003/11/06 08:40:59EST $
//======================================================================================


#ifndef __CHECKSUM_H          /*  prevent multiple definitions    */
#define __CHECKSUM_H  1





unsigned int CalculateChecksum(unsigned char *Data, unsigned int Size);


#endif      /*  __CHECKSUM_H  */



