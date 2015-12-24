/*=================================================================================
  IBM Family bitbanger

//=================================================================================
   $Source: interfaces_wedge_hw/bbibm.h $
   $Date: 2009/06/29 05:04:47EDT $
   $Revision: 1.3 $


//=================================================================================*/

#ifndef BITBANG_IBM_H_F3E1EF42_35D6_11D2_93BE_00805F2987D9
#define BITBANG_IBM_H_F3E1EF42_35D6_11D2_93BE_00805F2987D9

#include "BitBanger.h"

class CBitBangIBM : public CBitBanger
{
public:
	CBitBangIBM(void);

	virtual MatrixError_t BitBangOutTerm(unsigned int Data, unsigned int exinfo);
	virtual MatrixError_t BitBangInTerm(unsigned int &uiReturnData);
	virtual MatrixError_t BitBangOutKbd(unsigned int Data, unsigned int exinfo);
	virtual MatrixError_t BitBangInKbd(unsigned int &uiReturnData);

protected:
	void PulseClockTerm(void);
};


#endif /* BITBANG_IBM_H_F3E1EF42_35D6_11D2_93BE_00805F2987D9 */



