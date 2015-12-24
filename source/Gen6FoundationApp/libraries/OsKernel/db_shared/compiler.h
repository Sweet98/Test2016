/*----------------------------------------------------------------------*
 *	"Compiler.h" -- Compiler specific type definition MACROS
  *----------------------------------------------------------------------*
 * $RCSfile: db_shared/compiler.h $
 * $Revision: 1.4 $
 * $Date: 2004/06/28 05:21:27EDT $
 *
 *
 *----------------------------------------------------------------------*/
#ifndef _INC_COMPILER
  #define _INC_COMPILER

// all areas below imagine being conditional compiled, but only MX1/ARM (SDT 2.5 or CW ADS) are represented for now

// Compiler specific typedefs, some useful things for math guarentees
typedef 	unsigned char 	U8;
typedef 	signed char 	S8;
typedef 	unsigned short U16;
typedef 	signed short	S16;
typedef	unsigned int	U32;
typedef 	signed int		S32;

// Other useful Type Definitions ...
// these are really system defines, but as they are compiler specific I am including them here for now
typedef  unsigned char  			FLAG;
typedef  volatile unsigned long	REGTYPE;
#ifndef BOOL
typedef  int							BOOL;
#endif
typedef 	unsigned char   			BYTE ;
typedef 	unsigned short  			WORD ;

// alternative definitions that would be compiler specific
typedef 	int				INT32;
typedef 	unsigned int 	UINT32;

#ifndef INT16
typedef	short				INT16;
#endif

#ifndef UINT16
typedef	unsigned short	UINT16;
#endif

#ifndef INT8
typedef	signed char		INT8;
#endif

#ifndef UINT8
typedef	unsigned char 	UINT8;
#endif



#endif // __INC_COMPILER



