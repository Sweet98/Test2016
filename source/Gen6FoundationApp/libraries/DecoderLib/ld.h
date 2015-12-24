#ifndef LD_H_INCLUDED
#define LD_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>

/* Check if this is a dll build */
#ifdef LD_IS_DLL
    #define LD_STDCALL __stdcall
    #ifndef LD_API
        #define LD_API __declspec(dllimport)  /* Customer applications import from dll */
    #endif
#else
    #define LD_STDCALL 
    #define LD_API
#endif /* LD_API_IS_DLL */

/* Definitions */
/* Definitions->Constants->Components and Symbologies */
#define LD_CONST_UPC      (1L << 0)
#define LD_CONST_C128     (1L << 1)
#define LD_CONST_C39      (1L << 2)
#define LD_CONST_CB       (1L << 3)
#define LD_CONST_I25      (1L << 4)
#define LD_CONST_DB       (1L << 5)
#define LD_CONST_C93      (1L << 6)
#define LD_CONST_S25      (1L << 7)
#define LD_CONST_MSIP     (1L << 8)
#define LD_CONST_C11      (1L << 9)
#define LD_CONST_M25      (1L << 10)
#define LD_CONST_TP       (1L << 11)
#define LD_CONST_NEC25    (1L << 12)
#define LD_CONST_TRIOPTIC (1L << 13)
#define LD_CONST_HK25     (1L << 14)
#define LD_CONST_UKP      (1L << 15)


/* Definitions->Constants->Symbologies */
#define LD_CONST_DB_14             (LD_CONST_DB | 1)
#define LD_CONST_DB_EXPANDED       (LD_CONST_DB | 2)
#define LD_CONST_DB_LIMITED        (LD_CONST_DB | 4)
#define LD_CONST_S25_2SS            LD_CONST_S25
#define LD_CONST_S25_3SS           (LD_CONST_S25 | 1)
#define LD_CONST_UPC_SUPP_2        (LD_CONST_UPC | 1)
#define LD_CONST_UPC_SUPP_5        (LD_CONST_UPC | 2)


/* Definitions->Constants->Linkage */
#define LD_CONST_NOT_LINKED                 0
#define LD_CONST_LINKED_C128                11
#define LD_CONST_LINKED_RSS                 12


/* Definitions->Constants->General */
#define LD_CONST_FALSE                      0
#define LD_CONST_TRUE                       1
#define LD_CONST_DISABLED                   0
#define LD_CONST_ENABLED                    1
#define LD_CONST_STRIPPED                   2


/* Definitions->Constants->Progress */
#define LD_CONST_PROGRESS_CONTINUE          0
#define LD_CONST_PROGRESS_CANCEL            1


/* Definitions->Constants->Status */
#define LD_CONST_STATUS_PROCESSING          0


/* Definitions->Constants->Property Types */
#define LD_CONST_PROP_TYPE_LONG                   0
#define LD_CONST_PROP_TYPE_POINTER                1
#define LD_CONST_PROP_TYPE_FUNCTION               2


/* Definitions->Constants->Input Types */
#define LD_CONST_INPUT_TYPE_RUNLENGTH       0
#define LD_CONST_INPUT_TYPE_DMLDD           1


/* Definitions->Properties */
#define LD_LPROP_INPUT_SATURATION                                 0x00000001
#define LD_LPROP_INPUT_TYPE                                       0x00000002
#define LD_LPROP_INPUT_LENGTH                                     0x00000003
#define LD_PPROP_INPUT_RUNLENGTH_POINTER                          0x01000004
#define LD_LPROP_INPUT_TIME_MS                                    0x00000067
#define LD_LPROP_INPUT_CHANNEL                                    0x00000068
#define LD_FPROP_CALLBACK_RESULT                                  0x02000005
#define LD_FPROP_CALLBACK_STATUS                                  0x02000006
#define LD_PPROP_MISC_TAG                                         0x01000007
#define LD_LPROP_STATUS_CANCEL                                    0x00000008
#define LD_LPROP_STATUS_REASON                                    0x00000009
#define LD_LPROP_RESULT_SYMBOLOGY                                 0x0000000A
#define LD_PPROP_RESULT_STRING                                    0x0100000B
#define LD_LPROP_RESULT_LENGTH                                    0x0000000C
#define LD_LPROP_RESULT_LINKAGE                                   0x0000000D
#define LD_LPROP_RESULT_MODIFIER                                  0x0000000E
#define LD_LPROP_RESULT_MODIFIER_EX				                  0x0000000F
#define LD_PPROP_RESULT_CODEWORDS                                 0x01000010
#define LD_LPROP_RESULT_CODEWORDS_LENGTH                          0x00000011
#define LD_LPROP_RESULT_QUALITY                                   0x00000012
#define LD_LPROP_RESULT_READER_PROGRAMMING                        0x00000013
#define LD_LPROP_RESULT_STITCHED                                  0x00000014
#define LD_LPROP_CB_ENABLED                                       0x00000015
#define LD_LPROP_CB_CHECKSUM                                      0x00000016
#define LD_LPROP_CB_MISC                                          0x00000017
#define LD_LPROP_C128_ENABLED                                     0x00000018
#define LD_LPROP_C128_MISC                                        0x00000019
#define LD_LPROP_C39_ENABLED                                      0x0000001A
#define LD_LPROP_C39_CHECKSUM                                     0x0000001B
#define LD_LPROP_C39_FULL_ASCII                                   0x0000001C
#define LD_LPROP_C39_MISC                                         0x0000001D
#define LD_LPROP_TRIOPTIC_ENABLED                                 0x0000001E
#define LD_LPROP_I25_ENABLED                                      0x0000001F
#define LD_LPROP_I25_CHECKSUM                                     0x00000020
#define LD_LPROP_I25_LENGTHS                                      0x00000021
#define LD_LPROP_I25_COLLAPSE_VOIDS                               0x00000066
#define LD_LPROP_UPC_A_ENABLED                                    0x00000022
#define LD_LPROP_UPC_E_ENABLED                                    0x00000023
#define LD_LPROP_EAN_13_ENABLED                                   0x00000024
#define LD_LPROP_EAN_8_ENABLED                                    0x00000025
#define LD_LPROP_UPC_A_STITCHING_ENABLED                          0x00000026
#define LD_LPROP_UPC_E_STITCHING_ENABLED                          0x00000027
#define LD_LPROP_EAN_13_STITCHING_ENABLED                         0x00000028
#define LD_LPROP_EAN_8_STITCHING_ENABLED                          0x00000029
#define LD_LPROP_UPC_SUPPLEMENTAL_2_ENABLED                       0x0000002A
#define LD_LPROP_UPC_SUPPLEMENTAL_5_ENABLED                       0x0000002B
#define LD_LPROP_UPC_STITCHING_TIMEOUT                            0x0000002C
#define LD_LPROP_UPC_EXPANSION                                    0x0000002D
#define LD_LPROP_C93_ENABLED                                      0x0000002E
#define LD_LPROP_DATABAR_14_ENABLED                               0x0000002F
#define LD_LPROP_DATABAR_LIMITED_ENABLED                          0x00000030
#define LD_LPROP_DATABAR_EXPANDED_ENABLED                         0x00000031
#define LD_LPROP_DATABAR_14_STITCHING_TIMEOUT                     0x00000032
#define LD_LPROP_DATABAR_EXPANDED_STITCHING_TIMEOUT               0x00000033
#define LD_LPROP_S25_2SS_ENABLED                                  0x00000034
#define LD_LPROP_S25_2SS_LENGTHS                                  0x00000035
#define LD_LPROP_S25_3SS_ENABLED                                  0x00000036
#define LD_LPROP_S25_3SS_LENGTHS                                  0x00000037
#define LD_LPROP_S25_MISC                                         0x00000038
#define LD_LPROP_MSIP_ENABLED                                     0x00000039
#define LD_LPROP_MSIP_CHECKSUM                                    0x0000003A
#define LD_LPROP_UKP_ENABLED                                      0x0000003B
#define LD_LPROP_UKP_CHECKSUM                                     0x0000003C
#define LD_LPROP_C11_ENABLED                                      0x0000003D
#define LD_LPROP_C11_CHECKSUM                                     0x0000003E
#define LD_LPROP_M25_ENABLED                                      0x0000003F
#define LD_LPROP_M25_CHECKSUM                                     0x00000040
#define LD_LPROP_M25_LENGTHS                                      0x00000041
#define LD_LPROP_TP_ENABLED                                       0x00000042
#define LD_LPROP_NEC25_ENABLED                                    0x00000043
#define LD_LPROP_NEC25_CHECKSUM                                   0x00000044
#define LD_LPROP_NEC25_LENGTHS                                    0x00000045
#define LD_LPROP_HK25_ENABLED                                     0x00000046
#define LD_LPROP_HK25_LENGTHS                                     0x00000047
#define LD_LPROP_VERSION_MAJOR                                    0x00000048
#define LD_LPROP_VERSION_MINOR                                    0x00000049
#define LD_LPROP_VERSION_BUILD                                    0x0000004A
#define LD_LPROP_VERSION_COMPONENTS_AUTHORIZED                    0x0000004B
#define LD_LPROP_VERSION_COMPONENTS_PRESENT                       0x0000004C
#define LD_PPROP_VERSION_CONFIGURATION                            0x0100004D
#define LD_PPROP_VERSION_COPYRIGHTS                               0x0100004E
#define LD_PPROP_VERSION_TRADEMARKS                               0x0100004F
#define LD_PPROP_VERSION_COMMENTS                                 0x01000050
#define LD_PPROP_VERSION_COMPANY_NAME                             0x01000051
#define LD_PPROP_VERSION_PRODUCT_NAME                             0x01000052
#define LD_PPROP_VERSION_TIMESTAMP                                0x01000053


/* Type Definitions */
typedef void *LD_Handle;

typedef int LD_STDCALL LD_Callback(LD_Handle Handle);

typedef struct tag_LD_STRUCT_BOUNDS
{
    int Point[2];
} LD_STRUCT_BOUNDS;


/* Function Prototypes */
LD_API size_t LD_STDCALL LD_GetMemSize(void);
LD_API int LD_STDCALL LD_Create(LD_Handle Handle, size_t Length);
LD_API int LD_STDCALL LD_Decode(LD_Handle Handle);
LD_API int LD_STDCALL LD_Destroy(LD_Handle Handle);
LD_API int LD_STDCALL LD_GetLastError(LD_Handle Handle);
LD_API int LD_STDCALL LD_GetLong(LD_Handle Handle, long Property, long *Value);
LD_API int LD_STDCALL LD_GetPtr(LD_Handle Handle, long Property, void *Value);
LD_API int LD_STDCALL LD_GetFnPtr(LD_Handle Handle, long Property, LD_Callback **Value);
LD_API int LD_STDCALL LD_SetLong(LD_Handle Handle, long Property, long Value);
LD_API int LD_STDCALL LD_SetPtr(LD_Handle Handle, long Property, void *Value);
LD_API int LD_STDCALL LD_SetFnPtr(LD_Handle Handle, long Property, LD_Callback *Value);
LD_API int LD_STDCALL LD_GetPropType(long Property, long *PropType);
LD_API int LD_STDCALL LD_ClearStitchInfo(LD_Handle Handle);


/* Error Codes */
#define LD_ERR_INVALID_HANDLE                1001
#define LD_ERR_READ_ONLY                     1002
#define LD_ERR_UNKNOWN_PROPERTY              1003
#define LD_ERR_INSUFFICIENT_MEMORY           1004
#define LD_ERR_PROPERTY_LIFETIME             1005
#define LD_ERR_COMPONENT_ABSENT              1006
#define LD_ERR_COMPONENT_UNAUTHORIZED        1007
#define LD_ERR_PROPERTY_VALUE                1008
#define LD_ERR_INVALID_PROPERTY_TYPE         1009
#define LD_ERR_INVALID_ARGUMENT              1010
#define LD_ERR_INTERNAL                      1011


#ifdef __cplusplus
}
#endif
#endif /* LD_H_INCLUDED */


