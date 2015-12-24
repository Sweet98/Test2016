/************************************************************************/
//
// HHP 4480 embedded firmware
//
//  Simple Que handler
//  Date:       2/24/02
//  Tools:      ARM SDT 2.5 for starters... will be ARM ADS 1.1 or Metaware
//  Target:     D-ball 4480
//
//======================================================================================
//	Revision information
//======================================================================================
// $RCSfile: db_shared/db_queue.c $
// $Revision: 1.2.1.3 $
// $Date: 2009/05/20 17:41:51EDT $
//======================================================================================
/************************************************************************/
/************************************************************************/
/*  This module is hardware independent.                                */
/************************************************************************/
#include    "db_queue.h"    /* queue definitions & functions            */

/********************************************************************/
/*  Clear the queue. Set the input and output pointers to the head  */
/* of the queue.                                                    */
/*  Entry:  pointer to the queue                                    */
/********************************************************************/
void clear_que (QUEUE *q)
{
    q->inptr = q->outptr = q->head ;
}   /*  clear_que   */

/********************************************************************/
/*  Remove the next data item from the queue. Return the data item  */
/* or FALSE if the queue is empty. Pointer based queues are empty   */
/* when the input pointer is equal to the output pointer.           */
/*  Entry:  pointer to the queue                                    */
/*  Exit:   next data item from the queue or FALSE if the queue was */
/*          empty.                                                  */
/********************************************************************/
BYTE get_from_que (QUEUE *q)
{
    BYTE    data = 0 ;

    if (q->outptr != q->inptr)      /* check for data in the queue  */
    {
        data = *q->outptr++ ;           /* data in the queue, get next  */
        if (q->outptr >= q->tail)       /* bump pointer, stay in bounds */
            q->outptr = q->head ;
    }

    return data ;                   /* return data or FALSE if empty    */
}   /*  get_from_que    */

/********************************************************************/
/*  Add a data item to the queue. Return false if the queue is full.*/
/* Pointer based queues are considered full when the input pointer  */
/* one less than the output pointer.                                */
/*  Entry:  pointer to the queue                                    */
/*          data item to save -- exception done for if null         */
/*  Exit:   FALSE if item was not saved (queue is full)             */
/********************************************************************/
BYTE put_in_que (QUEUE *q, BYTE data)
{
    BYTE    *temp ;

    temp = q->inptr + 1 ;       /* check for room in the queue      */
     if (temp >= q->tail)        /* adjust pointer to stay in bounds */
        temp = q->head ;

    if (temp == q->outptr)
    {
		return 0;
	}
    *q->inptr = data ;      /* room in the queue, enter the data    */
     q->inptr = temp ;       /* bump pointer                         */
    return 1;              /* indicate queue is full, data not saved   */
}   /* put_in_que   */


/************************************************************/
/*  Compute the number of data items stored in the queue.   */
/*  Entry:  pointer to the queue                            */
/*  Exit:   number of data items in the queue               */
/************************************************************/
int que_has_data (QUEUE *q)
{
    int   result ;


     result = q->inptr - q->outptr ;     /* compute # elements in the queue  */
      if (result >= 0)
      {
          return (result) ;                     /*  indicate amount of data in queue    */
      }
    return ((q->tail - q->head) + result) ;   /*  indicate amount of data in queue    */

}   /*  que_has_data    */


