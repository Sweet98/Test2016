//======================================================================================
//	Revision information
//======================================================================================
// $RCSfile: Gen6FoundationApp/FoundationApp/PSOC/PSoC_27xxx_Vectors.h $
// $Revision: 1.2 $
// $Date: 2010/07/12 23:47:54EDT $

/// GENERAL DESCRIPTION
//**************************************************************************************
//


#ifndef PSoC_27xxx_Vector_Table
#define PSoC_27xxx_Vector_Table
//struct
//{
const unsigned int Initialize1_27x_24x[] =
{
	0x650000,
	0x000000,
	0x000000,
	0x000000,
	0x000000,
	0x000000,
	0x6f700e,
	0x6f600e,
	0x4f83ae,
	0x4f903e,
	0x6f500e,
	0x6f403e,
	0x4fb80e,
	0x6f930e,
	0x6fa40e,
	0x6f009e,
	0x6f800e,
	0x6ff12e,
	1
};

const unsigned int Initialize2_27x_24x[] =
{
	0x6f700e,
	0x6f600e,
	0x4f83ae,
	0x4f903e,
	0x6f500e,
	0x6f403e,
	0x4fb80e,
	0x6f930e,
	0x6fa40e,
	0x4fa01e,
	0x6f006e,
	0x6f800e,
	0x6ff12e,
	1
};

const unsigned int Initialize3_27x_24x[] =
{
	0x6f700e,
	0x6f403e,
	0x6f500e,
	0x6f608e,
	0x6f851e,
	0x6f9f8e,		// a1
	0x6fa30e,
	0x6ff12e,
	0x000000,
	0x6f700e,
	0x6f403e,
	0x6f500e,
	0x6f608e,
	0x6f860e,
	0x6f9eae,		// b1
	0x6fa30e,
	0x6f710e,
	0x6ff12e,
	0x000000,
	0x6f700e,
	0x6f403e,
	0x6f500e,
	0x6f608e,
	0x6f851e,
	0x6f9f9e,		// a2
	0x6fa30e,
	0x6ff12e,
	0x000000,
	0x6f700e,
	0x6f403e,
	0x6f500e,
	0x6f608e,
	0x6f860e,
	0x6f9e8e,		// b2
	0x6fa30e,
	0x6f710e,
	0x6ff12e,
	0x000000,
	1
};

const unsigned int IdSetup_27x_24x[] =
{
	0x6f710e,
	0x6e002e,
	0x6f700e,
	0x6f600e,
	0x4f83ae,
	0x4f903e,
	0x6f500e,
	0x6f403e,
	0x4fb80e,
	0x6f930e,
	0x6fa40e,
	0x4fa00e,
	0x6f006e,
	0x6f800e,
	0x6ff12e,
	1
};

const unsigned int EraseAll_27x_24x[] =
{
   0x4fc15e,
   0x4fe56e,
   0x6f700e,
   0x6f600e,
   0x4f83ae,
   0x4f903e,
   0x6f500e,
   0x6f403e,
   0x4fb80e,
   0x6f930e,
   0x6fa40e,
   0x6f005e,
   0x6f800e,
   0x6ff12e,
	1
};

const unsigned int ProgramBlock_27x[] =
{
	0x4fc15e,
	0x4fe56e,
	0x6f700e,
	0x6f600e,
	0x4f83ae,
	0x4f903e,
	0x6f500e,
	0x6f403e,
	0x4fb80e,
	0x6f930e,
	0x6fa40e,
	0x6f002e,
	0x6f800e,
	0x6ff12e,
	1
};

const unsigned int ProgramBlock_24x[] =
{
	0x4fc54e,
	0x4fe56e,
	0x6f700e,
	0x6f600e,
	0x4f83ae,
	0x4f903e,
	0x6f500e,
	0x6f403e,
	0x4fb80e,
	0x6f930e,
	0x6fa40e,
	0x6f002e,
	0x6f800e,
	0x6ff12e,
	1
};

const unsigned int VerifySetup_27x_24x[]=
{
	0x6f700e,
	0x6f600e,
	0x4f83ae,
	0x4f903e,
	0x6f500e,
	0x6f403e,
	0x4fb80e,
	0x6f930e,
	0x6fa40e,
	0x6f001e,
	0x6f800e,
	0x6ff12e,
	1
};

const unsigned int SecureBlock_27x[] =
{
	0x4fc15e,
	0x4fe56e,
	0x6f700e,
	0x6f600e,
	0x4f83ae,
	0x4f903e,
	0x6f500e,
	0x6f403e,
	0x4fb80e,
	0x6f930e,
	0x6fa40e,
	0x6f004e,
	0x6f800e,
	0x6ff12e,
	1
};

const unsigned int SecureBlock_24x[] =
{
	0x4fc54e,
	0x4fe56e,
	0x6f700e,
	0x6f600e,
	0x4f83ae,
	0x4f903e,
	0x6f500e,
	0x6f403e,
	0x4fb80e,
	0x6f930e,
	0x6fa40e,
	0x6f004e,
	0x6f800e,
	0x6ff12e,
	1
};

const unsigned int ChecksumSetup_27x[] =
{
	0x6f700e,
	0x6f600e,
	0x4f83ae,
	0x4f903e,
	0x6f500e,
	0x6f403e,
	0x4fb80e,
	0x6f930e,
	0x6fa40e,
	0x4fa00e,
	0x6f007e,
	0x6f800e,
	0x6ff12e,
	1
};

const unsigned int ChecksumSetup_24x[] =
{
	0x6f700e,
	0x6f600e,
	0x4f83ae,
	0x4f903e,
	0x6f500e,
	0x6f403e,
	0x4fb80e,
	0x6f930e,
	0x6fa40e,
	0x4fa40e,
	0x6f007e,
	0x6f800e,
	0x6ff12e,
	1
};

const unsigned int ReadID_27x_24x[] =
{
	0x5F8,
	0x5F9,
	1
};

const unsigned int ReadChecksum_27x_24x[] =
{
	0x5F9,
	0x5F8,
	1
};

const unsigned int EraseBlock[] =
{
	0x37b800,
	0x37b040,
	0x27c1d0,
	0x27c858,
	0x37a800,
	0x37a018,
	0x27d900,
	0x37c980,
	0x37d200,
	0x378018,
	0x37c000,
	0x37f890,
	1
};

const unsigned int EraseBlock_A[]=
{
	0x37b807,
	0x37b047,
	0x27c1d7,
	0x27c85f,
	0x37a807,
	0x37a01f,
	0x27d907,
	0x37c987,
	0x37d207,
	0x37801f,
	0x37c007,
	0x37f897,
	1
};

//}


#endif
