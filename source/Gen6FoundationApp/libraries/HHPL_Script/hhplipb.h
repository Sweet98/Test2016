//======================================================================================
// HHPLIPB.h
//======================================================================================
// $RCSfile: hhplipb.h $
// $Revision: 1.2 $
// $Date: 2011/11/11 17:01:41EST $
//======================================================================================

//======================================================================================



#ifndef __HHPL_IPB_H
#define __HHPL_IPB_H



#include	"HSTRING.h"				// HSTRING interface definition
#include	"HWSTRING.h"			// HWSTRING interface definition

#include	"HHPLInstructionSet.h"		// HHPL instruction set definitions



class HHPL_INSTRUCTION_PARAMETER_BLOCK
{

public:

//	Constructor for HHPL_INSTRUCTION_PARAMETER_BLOCK
	HHPL_INSTRUCTION_PARAMETER_BLOCK(void);


//	Destructor for HHPL_INSTRUCTION_PARAMETER_BLOCK
	~HHPL_INSTRUCTION_PARAMETER_BLOCK(void);





	bool				BooleanOperand1;	// Boolean operand "registers" used during execution
	bool				BooleanOperand2;	//	of a script
	bool				BooleanOperand3;

	int					IntegerOperand1;	// Integer operand "registers" used during execution
	int					IntegerOperand2;	//	of a script
	int					IntegerOperand3;
	int					IntegerOperand4;
	int					IntegerOperand5;
	int					IntegerOperand6;
	int					IntegerOperand7;
	int					IntegerOperand8;

	int					DataAddress1;		// Addresses of operands
	int					DataAddress2;

	int					CodeAddress1;		// Addresses of branch locations in code
	int					CodeAddress2;

	HSTRING				*StringOperand1;	// String operand "registers" used during execution
	bool				StringOperand1Defined;
	HSTRING				*StringOperand2;	//	of a script
	bool				StringOperand2Defined;

	HWSTRING			*WideStringOperand1;	// Wide string operand "registers" used during execution
	bool				WideStringOperand1Defined;
	HWSTRING			*WideStringOperand2;	//	of a script
	bool				WideStringOperand2Defined;

	unsigned int		SubinstructionOperand1;

	HHPL_INSTRUCTION_SET::RANGE
						RangeOperand1;		// Range operand "registers" used during execution
	HHPL_INSTRUCTION_SET::RANGE
						RangeOperand2;		//	of a script
private:
	HHPL_INSTRUCTION_PARAMETER_BLOCK (const HHPL_INSTRUCTION_PARAMETER_BLOCK&);            // declared private, but not defined, to prevent copying         
	HHPL_INSTRUCTION_PARAMETER_BLOCK& operator=(const HHPL_INSTRUCTION_PARAMETER_BLOCK&);  // declared private, but not defined, to prevent copying 

};
#endif      /*  __HHPL_IPB_H   */


