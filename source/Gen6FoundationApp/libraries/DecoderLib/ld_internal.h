#ifndef LD_INTERNAL_H_INCLUDED
#define LD_INTERNAL_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

/* Definitions */
/* Definitions->Constants->Components and Symbologies */
#define LD_CONST_PDF      (1L << 16)
#define LD_CONST_MLD      (1L << 19)
#define LD_CONST_DEBUG    (1L << 31)
#define LD_CONST_TIMING   (1L << 30)


/* Definitions->Constants->Symbologies */


/* Definitions->Constants->Linkage */
#define SD_CONST_LINKED_C39                 13


/* Definitions->Constants->General */


/* Definitions->Constants->Progress */
#define SD_CONST_PROGRESS_PAUSE             2


/* Definitions->Constants->Status */


/* Definitions->Properties */
/*#define LD_LPROP_MISC_ISSUE_IDENTICAL_SYMBOLS */
/*#define LD_LPROP_MISC_ISSUE_IDENTICAL_SPACING */    
/*#define LD_LPROP_MISC_SUBREGION_PROCESSING      */  
/*#define LD_PPROP_RESULT_BOUNDS                    */
/*#define LD_LPROP_RESULT_CENTER                    */
/*#define LD_LPROP_CB_SHORT_MARGIN                  */
/*#define LD_LPROP_C128_SHORT_MARGIN                */
/*#define LD_LPROP_C39_SHORT_MARGIN                 */
/*#define LD_LPROP_TRIOPTIC_SHORT_MARGIN            */
/*#define LD_LPROP_I25_SHORT_MARGIN                 */
/*#define LD_LPROP_UPC_SHORT_MARGIN                 */
/*#define LD_LPROP_C93_SHORT_MARGIN                 */

/* Function Prototypes */


/* Error Codes */



#ifdef __cplusplus
}
#endif
#endif /* LD_INTERNAL_H_INCLUDED */
