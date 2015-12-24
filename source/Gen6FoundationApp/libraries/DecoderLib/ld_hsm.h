#ifndef LD_HSM_H_INCLUDED
#define LD_HSM_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif


/* Definitions */
/* Definitions->Constants->Components and Symbologies */
#define LD_CONST_DMLDD    (1L << 17)
#define LD_CONST_MLDD     (1L << 18)

/* Definitions->Constants->Symbologies */


/* Definitions->Constants->Linkage */


/* Definitions->Constants->General */


/* Definitions->Constants->Progress */


/* Definitions->Constants->Status */
#define LD_CONST_STATUS_PARSE_COMPLETE              1

/* Definitions->Constants->Input Types */
#define LD_CONST_INPUT_TYPE_DMLDD           1
#define LD_CONST_INPUT_TYPE_MLDD            2


/* Definitions->Properties */
#define LD_PPROP_INPUT_MLDD_POSITION                              0x01000054
#define LD_PPROP_INPUT_MLDD_EDGE_STRENGTH                         0x01000055
#define LD_PPROP_INPUT_DMLDD_POINTER                              0x01000056
#define LD_LPROP_INPUT_DMLDD_FRAME_NUMBER                         0x00000069
#define LD_PPROP_PARSE_BUFFER                                     0x01000057
#define LD_LPROP_INPUT_PARSE_THRESHOLD                            0x00000058
#define LD_LPROP_C128_COLLAPSE_VOIDS                              0x00000059
#define LD_LPROP_C128_STITCHING_ENABLED                           0x0000005A
#define LD_LPROP_C128_STITCHING_TIMEOUT                           0x0000005B
#define LD_LPROP_C39_COLLAPSE_VOIDS                               0x0000005C
#define LD_LPROP_I25_ALLOW_ODD_DIGIT                              0x0000005D
#define LD_LPROP_UPC_STITCH_OVERLAP                               0x0000005E
#define LD_LPROP_UPC_STITCH_EXTENDED_OVERLAP                      0x0000005F
#define LD_LPROP_UPC_UNCERTAINTY_ROUNDING                         0x00000060
#define LD_LPROP_UPC_COLLAPSE_VOIDS                               0x00000061
#define LD_LPROP_DATABAR_CODEWORD_VOTING                          0x00000062
#define LD_LPROP_DATABAR_MISC                                     0x00000063
#define LD_LPROP_UKP_MISC                                         0x00000064
#define LD_LPROP_TP_MISC                                          0x00000065
#define LD_LPROP_PARSE_NUM_RUNLENGTHS                             0x0000006A

/* Type Definitions */


/* Function Prototypes */


/* Error Codes */


#ifdef __cplusplus
}
#endif
#endif /* LD_HSM_H_INCLUDED */


