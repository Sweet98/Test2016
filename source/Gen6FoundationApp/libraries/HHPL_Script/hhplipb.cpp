//======================================================================================
// HHPLIPB.cpp
//======================================================================================
// $RCSfile: hhplipb.cpp $
// $Revision: 1.1 $
// $Date: 2009/06/12 16:55:18EDT $
//======================================================================================

//======================================================================================


//---------------------------------------------------------------------------
//
//  Class Name:     HHPL_INSTRUCTION_PARAMETER_BLOCK
//
//  Class Description:
//		HHPL_INSTRUCTION_PARAMETER_BLOCK initializes data that keeps track of the parameters
//		used by HHPL instructions at run-time.
//
//---------------------------------------------------------------------------


#include	"HHPLIPB.h"	// HHPL instruction parameter block definitions







//
//	Constructor for HHPL_INSTRUCTION_PARAMETER_BLOCK
//		Set defaults.
//
HHPL_INSTRUCTION_PARAMETER_BLOCK::HHPL_INSTRUCTION_PARAMETER_BLOCK(void)
{
	StringOperand1Defined = false;
	StringOperand2Defined = false;

	WideStringOperand1Defined = false;
	WideStringOperand2Defined = false;

	return;
}  // Constructor





//
//	Destructor for HHPL_INSTRUCTION_PARAMETER_BLOCK
//		Free string memory.
//
HHPL_INSTRUCTION_PARAMETER_BLOCK::~HHPL_INSTRUCTION_PARAMETER_BLOCK()
{
	if(StringOperand1Defined)
	{
		delete StringOperand1;
	}

	if(StringOperand2Defined)
	{
		delete StringOperand2;
	}

	if(WideStringOperand1Defined)
	{
		delete WideStringOperand1;
	}

	if(WideStringOperand2Defined)
	{
		delete WideStringOperand2;
	}

    return;
}  // Destructor


