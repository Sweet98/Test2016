//======================================================================================
// strnicmp.c
//======================================================================================
// $RCSfile: strnicmp.c $
// $Revision: 1.2 $
// $Date: 2003/12/04 12:32:25EST $
//======================================================================================

//======================================================================================




#ifdef __cplusplus
extern "C" {
#endif


//
//	strnicmp
//		Do a case insensitive character compare.
//
int strnicmp(const char *string1, const char *string2, unsigned int count)
{
	int		StringComparisonResult;
    unsigned int	i;
    char	Char1;
    char	Char2;


	StringComparisonResult = 0;

	for(i = 0;    (i < count) && (StringComparisonResult == 0);    ++i)
    {
		Char1 = ((unsigned char) (((string1[i] >= 97) && (string1[i] <= 122)) ? string1[i] & 0xdf : string1[i]));
		Char2 = ((unsigned char) (((string2[i] >= 97) && (string2[i] <= 122)) ? string2[i] & 0xdf : string2[i]));
        if(Char1 < Char2)
	    {
			StringComparisonResult = -1;
    	}
        else if(Char1 > Char2)
        {
			StringComparisonResult = 1;
        }
    }

	return(StringComparisonResult);
}


#ifdef __cplusplus
}
#endif


