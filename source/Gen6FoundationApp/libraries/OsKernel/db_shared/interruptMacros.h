/**************************************************************************************/
//
// HIM 48X0G GENERATION 6 SOFTWARE
//
//  INTERRUPT MACROS 
//  Date:       8/04/2008
//  Tools:      RVCT Build 674
//  Target:     Freescale iMX27
//
//======================================================================================
// Revision information
//======================================================================================
// $RCSfile: db_shared/interruptMacros.h $
// $Revision: 1.3 $
// $Date: 2011/04/28 09:46:49EDT $
//======================================================================================

//======================================================================================
//------------------------------------------------------------------------------
//Header:  interruptMacros.h
//
// Provides Macros Enabling and Disabling Interrupts
//
// This file was created to prevent Circle dependencies with interrupts.h and HardwareMacros.h
//
//------------------------------------------------------------------------------

#ifndef INTERRUPTMACROS_H_
#define INTERRUPTMACROS_H_

#ifndef WIN32
#if defined(__GNUC__)
__inline unsigned int __GETSR(void)
{
	register unsigned int temp;
//	__asm ("mrs temp,CPSR" );
	return temp;
}
__inline void __SETSR(register unsigned int temp)
{
//	__asm ("msr CPSR_c,temp" );
}
#else //not defined(__GNUC__)
__inline unsigned int __GETSR(void)
{
	register unsigned int cpsr __asm("cpsr");
	return cpsr;
//	register unsigned int temp;
//	__asm ("mrs temp,CPSR" );
//	return temp;
}
__inline void __SETSR(register unsigned int temp)
{
//	__asm ("msr CPSR_C,temp" );
	register unsigned int cpsr __asm("cpsr");
	cpsr=temp;
}

#endif //endof defined(__GNUC__)
__inline void EnableFastAndNormalInterrupts(void)
{
	unsigned int cpsr =__GETSR();
	cpsr&=~(0xc0);
	__SETSR(cpsr);
}

__inline void RestoreInterrupts(int reg_r0)
{
	unsigned int cpsr =__GETSR();
	cpsr&=~(0xc0);
	__SETSR(cpsr |(reg_r0&0xc0));
}

// Using a return value prevents the CW compiler from doing it inline,
// so I used a reference.
inline void DisableInterrupts(int &oldvalue)
{
	oldvalue =__GETSR();
	__SETSR(oldvalue |0xc0);
}
inline void DisableNormalInterrupts(int &oldvalue)
{
	oldvalue =__GETSR();
	__SETSR(oldvalue |0x80);
}

#else //not WIN32
inline void EnableNormalInterrupts(void)	{};
inline void EnableFastAndNormalInterrupts(void)	{};
inline void RestoreInterrupts(int mask)	{};
inline void DisableInterrupts(int &mask)	{};
inline void DisableNormalInterrupts(int &mask)	{};
#endif //endof WIN32

#define ENABLE_INTERRUPTS()		EnableFastAndNormalInterrupts()
#define DISABLE_INTERRUPTS()		int _SavedIrqMask_; DisableInterrupts(_SavedIrqMask_)
#define RESTORE_INTERRUPTS()		RestoreInterrupts(_SavedIrqMask_)
#define DISABLE_NORMAL_INTERRUPTS()	int _SavedIrqMask_; DisableNormalInterrupts(_SavedIrqMask_)


#endif /*INTERRUPTMACROS_H_*/


