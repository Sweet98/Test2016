/*=============================================================================
 * Header file for 4683tx

 *=============================================================================
 * $RCSfile: interfaces/4683tx.h $
 * $Revision: 2.0 $
 * $Date: 2002/12/06 15:37:59EST $
 *============================================================================*/

#ifndef FILE_4683TX_H__INCLUDED_
#define FILE_4683TX_H__INCLUDED_

void SendRetailFrame(char* Message);
void ReSendFrame(void);
void ClearTxBuffer(void);

#endif


