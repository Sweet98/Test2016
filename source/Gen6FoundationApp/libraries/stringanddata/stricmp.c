//======================================================================================
// stricmp.c
//======================================================================================
// $RCSfile: stricmp.c $
// $Revision: 1.4 $
// $Date: 2009/09/29 11:50:19EDT $
//======================================================================================

//======================================================================================





#include <ctype.h>


#ifdef __cplusplus
extern "C" {
#endif


//
//	stricmp
//		Do a case insensitive character compare.
//
int stricmp(const char *string1, const char *string2)
{
	int		StringComparisonResult=0;
	for(;StringComparisonResult==0;string1++,string2++)
   {
   	StringComparisonResult=toupper(*string1)-toupper(*string2);
   	if (*string1==0 ||*string2==0)
   		break;
   }
	return(StringComparisonResult);
}


#ifdef __cplusplus
}
#endif


