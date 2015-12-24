//======================================================================================
//	Revision information
//======================================================================================
// $RCSfile: app_kernel/interrupts.h $
// $Revision: 1.5.2.14 $
// $Date: 2010/02/16 12:00:17EST $
//======================================================================================

#ifndef INTERRUPTS_H_F3E1EF42_35D6_11D2_93BE_00805F2987D9
#define INTERRUPTS_H_F3E1EF42_35D6_11D2_93BE_00805F2987D9
#include <stdint.h>
#include <stdlib.h>

//DOG DEBUG added to MAKEFILE. UNCOMMENT IT THERE.
//#define WDOGDEBUG 1 //define here and in threadsinterrupts.s and DiagnosticAndDebugMenuSettings.cpp. to enable reporting of wdog trip address

#if PLATFORM_CURRENT == imx25
	#include "HardwareMacros.h"		//Provides Macros for setting registers
	#include "iMX25_INTVEC.h"
	#include "iMX25_ASIC.h"
#elif PLATFORM_CURRENT == imx27
	#include "HardwareMacros.h"		//Provides Macros for setting registers
	#include "iMX27_intervec.h"
	#include "iMX27_aitc.h"
#elif	PLATFORM_CURRENT == mxl
	#include "db_mx1.h"
#else
	#error VALID PLATFORM NOT DEFINED
#endif //endof PLATFORM Check




#include "interruptMacros.h" //Enable and Disable Interrupt Macros

/////////////////////////////////////////////////////////////////////////////////////////////
// All ISR must be modeled after this:
typedef unsigned long threadhandle_t;
typedef threadhandle_t (*isrfuncptr_t)(void *) ;
#ifdef WDOGDEBUG
threadhandle_t WDogIsr(void *);
#endif

#define PROCESSOR_MODE_USER 0x10
#define PROCESSOR_MODE_FIQ	 0x11
#define PROCESSOR_MODE_IRQ	 0x12
#define PROCESSOR_MODE_SVC	 0x13        // supervisor mode
#define PROCESSOR_MODE_ABT	 0x17        // abort mode (data and code )
#define PROCESSOR_MODE_UND	 0x1b        // undefined instruction
#define PROCESSOR_MODE_SYS	 0x1f        // system mode our normal mode


/////////////////////////////////////////////////////////////////////////////////////////////
// A helper class to store the function address (ISR) and it's parameter
class CIrqVector
{
public:
	CIrqVector(void);
	void Set(isrfuncptr_t pIsrFunction, void *pUniversal)
	{
		m_pIsrFunction = pIsrFunction;
		m_pUniversal = pUniversal;
	}
	void Get(isrfuncptr_t &pIsrFunction, void *&pUniversal);
protected:
	void *m_pUniversal;                  // these must be the only data members and they must be
	isrfuncptr_t m_pIsrFunction;         // in this order se interrupt code in db_start
};

extern CIrqVector UserIsrVectors[64];

/////////////////////////////////////////////////////////////////////////////////////////////
/*
 Various functions to set an interrupt vector.
	Usage:
		InitPhase:
		GetVector(m_SavedRxVetor, VEX_xxxx);
		GetVector(m_SavedTxVetor, VEX_xxxx);
		// now install our ISRs:
		SetVector(&RxIsr, pLocalData, VEX_xxxx);
		SetVector(&TxIsr, pLocalData, VEX_xxxx);

	Close phase:
		SetVector(m_SavedRxVetor, VEX_xxxx);
		SetVector(m_SavedTxVetor, VEX_xxxx);

 * SetVector : places address of interrupt routine
 * in table to be executed by NISR
 *  funcaddr : address of function
 * vector_number: number of AITC interrupt to associate
 * 	routine to.
 */

// These functions are inline because its faster and even smaller code
inline void SetVector(CIrqVector &Vector, vector_number_t VectorNumber)
{
	UserIsrVectors[VectorNumber] = Vector;
}

inline void GetVector(CIrqVector &Vector, vector_number_t VectorNumber)
{
	Vector = UserIsrVectors[VectorNumber];
}

inline void SetVector(isrfuncptr_t pIsr, void *pUniversal, vector_number_t VectorNumber)
{
	UserIsrVectors[VectorNumber].Set(pIsr, pUniversal);
}

inline void SetVector(isrfuncptr_t pIsr, vector_number_t VectorNumber)
{
	UserIsrVectors[VectorNumber].Set(pIsr, NULL);
}

/////////////////////////////////////////////////////////////////////////////////////////////
// The following is a  helper to easily create a SetVector for members of a class.
// This is used to connect an interrupt service routine to it's vector.
// CAUTION: This code might not be 100% comatiple with all compilers!!
// If the compiler uses a different representation for
// member pointers and function pointers, this will fail!!
// It works for the MSVC 6.0 and the CW compiler (and all other CPP compilers I know so far).
//
// Usage:
// Into your class declaration, add this:
//		class testclass
//		{ ......
//			CREATE_SETVECTOR_MEMBER(testclass);
//		};
// Usage of the resulting member function:
//		SetVector(vector_number, testclass::memberISR);
//
// The inline function gets resolved by the compiler (at least the CW)
// to nothing more as you would need anyway.
//ads 1.2
#define CREATE_SETVECTOR_MEMBER(classname)													\
typedef threadhandle_t (classname::*IsrMemberFunctionPointer_t)(void);						\
void SetVector(IsrMemberFunctionPointer_t callback, vector_number_t VectorNumber)			\
{																							\
	union dual_t																			\
	{																						\
		IsrMemberFunctionPointer_t mfp;														\
		isrfuncptr_t fp[2]; /* ads has fn ptr first*/										\
	};																						\
	dual_t DualPointer;																		\
	DualPointer.mfp  = callback;															\
	::SetVector(DualPointer.fp[0], this, VectorNumber);										\
}

/////////////////////////////////////////////////////////////////////////////////////////////
/*-----------------4/12/2002 11:40AM----------------
 * DisableInterruptNumber:
 * enables aitc interrupt 0-63
 * call with number of interrupt to disable
 * --------------------------------------------------*/

inline void DisableInterruptNumber(vector_number_t intnumber)
{
#if PLATFORM_CURRENT == imx25
	HAZARD_WRITE_REG(ASIC_INTDISNUM,intnumber);

#elif PLATFORM_CURRENT == imx27
	HAZARD_WRITE_REG(AITC_INTDISNUM,intnumber);

#elif	PLATFORM_CURRENT == mxl
	pAitc->Intdisnum.f=intnumber; //disable this interrupt
#endif //endof PLATFORM Check
}//DisableInterruptNumber

/*-----------------4/12/2002 11:40AM----------------
 * EnableInterruptNumber:
 * enables aitc interrupt 0-63
 * call with number of interrupt to enable
 * --------------------------------------------------*/

inline void EnableInterruptNumber(vector_number_t intnumber)
{
#if PLATFORM_CURRENT == imx25
	HAZARD_WRITE_REG(ASIC_INTENNUM,intnumber);

#elif PLATFORM_CURRENT == imx27
	HAZARD_WRITE_REG(AITC_INTENNUM,intnumber);

#elif	PLATFORM_CURRENT == mxl
	pAitc->Intennum.f=intnumber; //disable this interrupt
#endif //endof PLATFORM Check
}//EnableInterruptNumber


// Changes the interrupt from normal to fast
void UseFastInterrupt(vector_number_t intnumber);
void UseNormalInterrupt(vector_number_t intnumber);

// Set and clear the fore interrupt bit
void ForceInterrupt(vector_number_t intnumber);
void ClrForceInterrupt(vector_number_t intnumber);

// For extreme speed resons, get the addr and mask with the functions below and store it.
// Later inside an ISR you can access the mask register with the CGpio::SetPortFast (ClrPortFast).
#if	PLATFORM_CURRENT == mxl
void GetInterruptTypeAddrAndMask(vector_number_t intnumber, uint32_t &bitmask, GpioPort_t *&pPort );
void GetInterruptForceAddrAndMask(vector_number_t intnumber, uint32_t &bitmask, GpioPort_t *&pPort );
#endif //endof PLATFORM Check (mxl)

inline void SetNormalInterruptPriority(vector_number_t intnumber,unsigned int priority)
{
	//Offset is an index number to the Priority Register
	unsigned int offset =(63-intnumber)/8;

#if PLATFORM_CURRENT == imx25
	WRITE_FIELD_TO_REG((ASIC_NIPRIORITY7 + (offset * 4)),((intnumber %8)*4),0xF,priority);

#elif PLATFORM_CURRENT == imx27
	WRITE_FIELD_TO_REG((AITC_NIPRIORITY7 + (offset * 4)),((intnumber %8)*4),0xF,priority);

#elif	PLATFORM_CURRENT == mxl
	DISABLE_INTERRUPTS();
	(&pAitc->Nipriority7)[offset].f&=~((unsigned long)0xf<<((intnumber %8)*4));
	(&pAitc->Nipriority7)[offset].f|=((priority&(unsigned long)0x0f)<<((intnumber %8)*4));
	RESTORE_INTERRUPTS();
#endif //endof PLATFORM Check

}

inline unsigned int GetNormalInterruptPriority(vector_number_t intnumber)
{
	unsigned int offset =(63-intnumber)/8;
#if PLATFORM_CURRENT == imx25
	return READ_FIELD((ASIC_NIPRIORITY7 + (offset * 4)),((intnumber %8)*4),0xF);

#elif PLATFORM_CURRENT == imx27
	return READ_FIELD((AITC_NIPRIORITY7 + (offset * 4)),((intnumber %8)*4),0xF);

#elif	PLATFORM_CURRENT == mxl
	unsigned int ret;
	DISABLE_INTERRUPTS();
	ret=(&pAitc->Nipriority7)[offset].f;
	RESTORE_INTERRUPTS();
	ret >>=((intnumber %8)*4);
	ret&=0x0f;
	return ret;
#endif //endof PLATFORM Check
}

inline unsigned int GetLowInterruptEnableRegister(void)
{
#if PLATFORM_CURRENT == imx25
	return READ_REG(ASIC_INTENABLEL);

#elif PLATFORM_CURRENT == imx27
	return READ_REG(AITC_INTENABLEL);

#elif	PLATFORM_CURRENT == mxl
	return pAitc->Intenablel.f
#endif //endof PLATFORM Check
}//GetLowInterruptEnableRegister

inline unsigned int GetHighInterruptEnableRegister(void)
{
#if PLATFORM_CURRENT == imx25
	return READ_REG(ASIC_INTENABLEH);

#elif PLATFORM_CURRENT == imx27
	return READ_REG(AITC_INTENABLEH);

#elif	PLATFORM_CURRENT == mxl
	return pAitc->Intenableh.f
#endif //endof PLATFORM Check
}//GetLowInterruptEnableRegister

inline void SetLowInterruptEnableRegister(unsigned int newregvalue)
{
#if PLATFORM_CURRENT == imx25
	WRITE_REG(ASIC_INTENABLEL,newregvalue);

#elif PLATFORM_CURRENT == imx27
	WRITE_REG(AITC_INTENABLEL,newregvalue);

#elif	PLATFORM_CURRENT == mxl
	pAitc->Intenablel.f = newregvalue
#endif //endof PLATFORM Check
}//SetHighInterruptEnableRegister

inline void SetHighInterruptEnableRegister(unsigned int newregvalue)
{
#if PLATFORM_CURRENT == imx25
	WRITE_REG(ASIC_INTENABLEH,newregvalue);

#elif PLATFORM_CURRENT == imx27
	WRITE_REG(AITC_INTENABLEH,newregvalue);

#elif	PLATFORM_CURRENT == mxl
	pAitc->Intenableh.f = newregvalue
#endif //endof PLATFORM Check
}//SetHighInterruptEnableRegister


void InterruptInit(void);
void LockSheduler(void);
void UnLockSheduler(void);
void ZeroInterruptInit(void);
bool AreAnyNormalInterruptPending(void);
bool AreAnyFastInterruptPending(void);

// some debugging helpers
bool IsFastInterruptPending(vector_number_t intnumber);
bool IsNormalInterruptPending(vector_number_t intnumber);
bool IsInterruptSourceActive(vector_number_t intnumber);
void StartWDogDebugIrq(void); //sets up wtachdog as irq for debugging


#endif // INTERRUPTS_H_F3E1EF42_35D6_11D2_93BE_00805F2987D9




