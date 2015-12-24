/************************************************************************/
//
// HHP 4480 embedded firmware
//
//  Command Handler for boot code
//  Date:       2/24/02
//  Tools:      ARM SDT 2.5 for starters... will be ARM ADS 1.1 or Metaware
//  Target:     D-ball 4480
//
//======================================================================================
//	Revision information
//======================================================================================
// $RCSfile: db_shared/db_queue.h $
// $Revision: 1.1.2.3 $
// $Date: 2009/05/20 17:41:52EDT $
//======================================================================================
/************************************************************************/
/*|=======================================================================
  |           MODIFICATION HISTORY
  |=======================================================================
*************************************************************************/

#ifndef __DB_QUEUE_H
#define __DB_QUEUE_H    1
#include "compiler.h"

typedef struct Que {
    BYTE    *inptr ;        /* input pointer    */
    BYTE    *outptr ;       /* output pointer   */
    BYTE    *head ;         /* start of queue   */
    BYTE    *tail ;         /* end of queue     */
} QUEUE ;

/***** Queue methods *****/
void    clear_que (QUEUE *) ;         /* clear the queue                      */
BYTE    get_from_que (QUEUE *) ;      /* return next item from the queue      */
BYTE    put_in_que (QUEUE *, BYTE) ;  /* put data in the queue if room        */
int     que_has_data (QUEUE *) ;      /* return number of items in the queue  */
WORD    getw_from_que (QUEUE *) ;     /* return next item from the queue      */
WORD    putw_in_que (QUEUE *, WORD) ; /* put data in the queue if room        */

#endif      /*  __DB_QUEUE_H    */



