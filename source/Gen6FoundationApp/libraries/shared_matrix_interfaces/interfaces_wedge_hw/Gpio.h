/*=================================================================================
  typename: CGpio

//=================================================================================
   $Source: interfaces_wedge_hw/Gpio.h $
   $Date: 2009/06/29 05:11:02EDT $
   $Revision: 1.2 $     


//=================================================================================*/

#ifndef GPIO_H_F3E1EF42_35D6_11D2_93BE_00805F2987D9
#define GPIO_H_F3E1EF42_35D6_11D2_93BE_00805F2987D9

#include "interrupts.h"
#include "product.h"
#define USE_GPIO_INLINE_FUNCTIONS 1

typedef union
{
	struct tag_porta
  	{
  		volatile unsigned long			Bit0	  	: 1;
  		volatile unsigned long			Bit1	  	: 1;
  		volatile unsigned long			Bit2	  	: 1;
  		volatile unsigned long			Bit3	  	: 1;
  		volatile unsigned long			Bit4	  	: 1;
  		volatile unsigned long			Bit5	  	: 1;
  		volatile unsigned long			Bit6	  	: 1;
  		volatile unsigned long			Bit7	  	: 1;
  		volatile unsigned long			Bit8	  	: 1;
  		volatile unsigned long			Bit9	  	: 1;
  		volatile unsigned long			Bit10	  	: 1;
  		volatile unsigned long			Bit11	  	: 1;
  		volatile unsigned long			Bit12	  	: 1;
  		volatile unsigned long			Bit13	  	: 1;
  		volatile unsigned long			Bit14	  	: 1;
  		volatile unsigned long			Bit15	  	: 1;
  		volatile unsigned long			Bit16	  	: 1;
  		volatile unsigned long			Bit17	  	: 1;
  		volatile unsigned long			Bit18	  	: 1;
  		volatile unsigned long			Bit19	  	: 1;
  		volatile unsigned long			Bit20	  	: 1;
  		volatile unsigned long			Bit21	  	: 1;
  		volatile unsigned long			Bit22	  	: 1;
  		volatile unsigned long			Bit23	  	: 1;
  		volatile unsigned long			Bit24	  	: 1;
  		volatile unsigned long			Bit25	  	: 1;
  		volatile unsigned long			Bit26	  	: 1;
  		volatile unsigned long			Bit27	  	: 1;
  		volatile unsigned long			Bit28	  	: 1;
  		volatile unsigned long			Bit29	  	: 1;
  		volatile unsigned long			Bit30	  	: 1;
  		volatile unsigned long			Bit31	  	: 1;
	} b;
	volatile unsigned long f;
} GpioPort_t;

typedef struct tagDoubleBit
{
  	volatile unsigned long			Bit0	  	: 2;
  	volatile unsigned long			Bit1	  	: 2;
  	volatile unsigned long			Bit2	  	: 2;
  	volatile unsigned long			Bit3	  	: 2;
  	volatile unsigned long			Bit4	  	: 2;
  	volatile unsigned long			Bit5	  	: 2;
  	volatile unsigned long			Bit6	  	: 2;
  	volatile unsigned long			Bit7	  	: 2;
  	volatile unsigned long			Bit8	  	: 2;
  	volatile unsigned long			Bit9	  	: 2;
  	volatile unsigned long			Bit10	  	: 2;
  	volatile unsigned long			Bit11	  	: 2;
  	volatile unsigned long			Bit12	  	: 2;
  	volatile unsigned long			Bit13	  	: 2;
  	volatile unsigned long			Bit14	  	: 2;
  	volatile unsigned long			Bit15	  	: 2;
  	volatile unsigned long			Bit16	  	: 2;
  	volatile unsigned long			Bit17	  	: 2;
  	volatile unsigned long			Bit18	  	: 2;
  	volatile unsigned long			Bit19	  	: 2;
  	volatile unsigned long			Bit20	  	: 2;
  	volatile unsigned long			Bit21	  	: 2;
  	volatile unsigned long			Bit22	  	: 2;
  	volatile unsigned long			Bit23	  	: 2;
  	volatile unsigned long			Bit24	  	: 2;
  	volatile unsigned long			Bit25	  	: 2;
  	volatile unsigned long			Bit26	  	: 2;
  	volatile unsigned long			Bit27	  	: 2;
  	volatile unsigned long			Bit28	  	: 2;
  	volatile unsigned long			Bit29	  	: 2;
  	volatile unsigned long			Bit30	  	: 2;
  	volatile unsigned long			Bit31	  	: 2;
}DoubleBit_t;


typedef union
{
	DoubleBit_t b;
	volatile unsigned __int64 f;
} GPIO_2BIT_t;

enum bit_number_t
{
	BitNum0  = 0,
	BitNum1  = 1,
	BitNum2  = 2,
	BitNum3  = 3,
	BitNum4  = 4,
	BitNum5  = 5,
	BitNum6  = 6,
	BitNum7  = 7,
	BitNum8  = 8,
	BitNum9  = 9,
	BitNum10 = 10,
	BitNum11 = 11,
	BitNum12 = 12,
	BitNum13 = 13,
	BitNum14 = 14,
	BitNum15 = 15,
	BitNum16 = 16,
	BitNum17 = 17,
	BitNum18 = 18,
	BitNum19 = 19,
	BitNum20 = 20,
	BitNum21 = 21,
	BitNum22 = 22,
	BitNum23 = 23,
	BitNum24 = 24,
	BitNum25 = 25,
	BitNum26 = 26,
	BitNum27 = 27,
	BitNum28 = 28,
	BitNum29 = 29,
	BitNum30 = 30,
	BitNum31 = 31,

	BitNumBorder = BitNum16
};


class CGpio
{
public:
	CGpio(GpioPort_t *pPort);
friend class CPortDetails;
public:
	GpioPort_t *m_pPort;
	////////////////////////////////////////////////////////////////////////////
	GpioPort_t *AddrDR (void)		{ return m_pPort +0x00; }
	GpioPort_t *AddrDDIR (void)		{ return m_pPort +0x04; }
	GpioPort_t *AddrSSR (void)		{ return m_pPort +0x08; }
	GpioPort_t *AddrICR1 (void)		{ return m_pPort +0x0C; }
	GpioPort_t *AddrICR2 (void)		{ return m_pPort +0x10; }
	GpioPort_t *AddrIMR (void)		{ return m_pPort +0x14; }
	GpioPort_t *AddrISR (void)		{ return m_pPort +0x18; }
	GpioPort_t *AddrEDGE (void)	{ return m_pPort +0x1C; }


public:
	GPIO_2BIT_t *AddrICR (void)		{ return (GPIO_2BIT_t*)AddrICR1();		}
protected:

public:
	////////////////////////////////////////////////////////////////////////////
	static void SetPortFast(ULONG bitmask, GpioPort_t *pPort)
	{
		GpioPort_t *p = pPort;
		p->f |= bitmask;
	}

	static void SetPortFast(GpioPort_t bitmask, GpioPort_t *pPort)
	{
		GpioPort_t *p = pPort;
		p->f |= bitmask.f;
	}

	static void ClrPortFast(ULONG bitmask, GpioPort_t *pPort)
	{
		pPort->f &= ~bitmask;
	}

	static void ClrPortFast(GpioPort_t bitmask, GpioPort_t *pPort)
	{
		pPort->f &= ~bitmask.f;
	}

	static void TogglePortFast(ULONG bitmask, GpioPort_t *pPort)
	{
		GpioPort_t *p = pPort;
		p->f ^= bitmask;
	}

	static void TogglePortFast(GpioPort_t bitmask, GpioPort_t *pPort)
	{
		GpioPort_t *p = pPort;
		p->f ^= bitmask.f;
	}

	static void SetPort(ULONG bitmask, GpioPort_t *pPort)
	{
		ULONG mask=bitmask;			// This line helps the compiler to place the move #bitmask before the disable IRQ
		DISABLE_INTERRUPTS();
		SetPortFast(mask, pPort);
		RESTORE_INTERRUPTS();
	}

	static void SetPort(GpioPort_t bitmask, GpioPort_t *pPort)
	{
		GpioPort_t mask=bitmask;	// This line helps the compiler to place the move #bitmask before the disable IRQ
		DISABLE_INTERRUPTS();
		SetPortFast(mask, pPort);
		RESTORE_INTERRUPTS();
	}

	static void ClrPort(ULONG bitmask, GpioPort_t *pPort)
	{
		ULONG mask=bitmask;			// This line helps the compiler to place the move #bitmask before the disable IRQ
		DISABLE_INTERRUPTS();
		ClrPortFast(mask, pPort);
		RESTORE_INTERRUPTS();
	}

	static void ClrPort(GpioPort_t bitmask, GpioPort_t *pPort)
	{
		GpioPort_t mask=bitmask;	// This line helps the compiler to place the move #bitmask before the disable IRQ
		DISABLE_INTERRUPTS();
		ClrPortFast(mask, pPort);
		RESTORE_INTERRUPTS();
	}

	static void TogglePort(ULONG bitmask, GpioPort_t *pPort)
	{
		ULONG mask=bitmask;			// This line helps the compiler to place the move #bitmask before the disable IRQ
		DISABLE_INTERRUPTS();
		TogglePortFast(mask, pPort);
		RESTORE_INTERRUPTS();
	}

	static void TogglePort(GpioPort_t bitmask, GpioPort_t *pPort)
	{
		GpioPort_t mask=bitmask;	// This line helps the compiler to place the move #bitmask before the disable IRQ
		DISABLE_INTERRUPTS();
		TogglePortFast(mask, pPort);
		RESTORE_INTERRUPTS();
	}

	template <typename M> static void WritePortFast_(M bitmask, GpioPort_t *pPort, bool val)
	{
		if(val)
			SetPortFast(bitmask, pPort);
		else
			ClrPortFast(bitmask, pPort);
	}

	template <typename M> static void WritePort_(M bitmask, GpioPort_t *pPort, bool val)
	{
		if(val)
			SetPort(bitmask, pPort);
		else
			ClrPort(bitmask, pPort);
	}

	// use the next two function to clear interrupt bits
	static void Write1ToClear(ULONG bitmask, GpioPort_t *pPort)
	{
		pPort->f = bitmask;
	}

	static void Write1ToClear(GpioPort_t bitmask, GpioPort_t *pPort)
	{
		pPort->f = bitmask.f;
	}

	static ULONG GetPort(ULONG bitmask, GpioPort_t *pPort)
	{
		return pPort->f & bitmask;
	}


	static ULONG GetPort(GpioPort_t *pPort)
	{
		return pPort->f;
	}

	static bool IsAllSet(ULONG bitmask, GpioPort_t *pPort)
	{
		return ((pPort->f & bitmask) == bitmask);
	}

	static bool IsAllSet(GpioPort_t bitmask, GpioPort_t *pPort)
	{
		return ((pPort->f & bitmask.f) == bitmask.f);
	}

	template <typename M> void PreparePortAsOutput_(M bitmask, bool initial_val)    //Haven't solved by Jess
	{
		DISABLE_INTERRUPTS();
		MakeOutputFast(bitmask);
		WritePortFast(bitmask, initial_val);
		RESTORE_INTERRUPTS();
	}
	template <typename M> void PreparePortAsOutput_(M bitmask) //Haven't solved by Jess
	{
		DISABLE_INTERRUPTS();
		MakeOutputFast(bitmask);
		RESTORE_INTERRUPTS();
	}

	template <typename M> void PreparePortAsInput_(M bitmask)    //Haven't solved by Jess
	{
		DISABLE_INTERRUPTS();
		MakeInputFast(bitmask);
		RESTORE_INTERRUPTS();
	}


	static ULONG GetMaskFromNumber(bit_number_t Bitnumber)
	{
		return 1UL <<Bitnumber;
	}

	static ULONG Get2BitMaskFromNumber(bit_number_t Bitnumber, int val);
	GpioPort_t *GetIrqConfigAddr(bit_number_t Bitnumber);

public:
#ifdef USE_GPIO_INLINE_FUNCTIONS
	////////////////////////////////////////////////////////////////////////////
	// these functions are interrupt save. They disable interrupts while criticals ections.
	template <typename M> void MakeOutput(M bitmask)				{ SetPort(bitmask, AddrDDIR());	}
	template <typename M> void MakeInput(M bitmask)					{ ClrPort(bitmask, AddrDDIR());	}

	template <typename M> void SetPort(M bitmask)					{ SetPort(bitmask, AddrDR());		}
	template <typename M> void ClrPort(M bitmask)					{ ClrPort(bitmask, AddrDR());		}
	template <typename M> void WritePort(M bitmask, bool val)	{ WritePort_(bitmask, AddrDR(), val);}

	template <typename M> void EnableIrq(M bitmask)					{ SetPort(bitmask, AddrIMR());	}
	template <typename M> void DisableIrq(M bitmask)				{ ClrPort(bitmask, AddrIMR());	}

#else
	////////////////////////////////////////////////////////////////////////////
	// these functions are interrupt save. They disable interrupts while criticals ections.
	void MakeOutput(ULONG bitmask);
	void MakeInput(ULONG bitmask);

	void SetPort(ULONG bitmask);
	void ClrPort(ULONG bitmask);

	void EnableIrq(ULONG bitmask);
	void DisableIrq(ULONG bitmask);

#endif
	////////////////////////////////////////////////////////////////////////////
	// some higher level helpers (not inline)
	template <typename M> void PreparePortAsOutput(M bitmask, bool initial_val)
	{
		PreparePortAsOutput_(bitmask, initial_val);
	}

		template <typename M> void PreparePortAsOutput(M bitmask)
	{
		PreparePortAsOutput_(bitmask);
	}

	template <typename M> void PreparePortAsInput(M bitmask)
	{
		PreparePortAsInput_(bitmask);
	}

	////////////////////////////////////////////////////////////////////////////
	// Use only if you disabled the interrupts!
	template <typename M> void MakeOutputFast(M bitmask)				{ SetPortFast(bitmask, AddrDDIR());	}
	template <typename M> void MakeInputFast(M bitmask)				{ ClrPortFast(bitmask, AddrDDIR());	}

	template <typename M> void SetPortFast(M bitmask)					{ SetPortFast(bitmask, AddrDR());	}
	template <typename M> void ClrPortFast(M bitmask)					{ ClrPortFast(bitmask, AddrDR());	}
	template <typename M> void WritePortFast(M bitmask, bool val)	{ WritePortFast_(bitmask, AddrDR(), val);}

	template <typename M> void EnableIrqFast(M bitmask)				{ SetPortFast(bitmask, AddrIMR());	}
	template <typename M> void DisableIrqFast(M bitmask)				{ ClrPortFast(bitmask, AddrIMR());	}


	////////////////////////////////////////////////////////////////////////////
	// Do not use other functions to reset an interrupt flag! ('or', 'and' functions are not save!!)
	template <typename M> void AcknowlegeIrq(M bitmask)			{ Write1ToClear(bitmask, AddrISR());}

	////////////////////////////////////////////////////////////////////////////
	// Returns true if any of the bitmask interrupts are set
	template <typename M> bool IsIrq(M bitmask)						{ return GetPort(bitmask, AddrISR()) ? true : false;	}
	// Returns the interrupt status register of the port
	ULONG GetIsr(void)														{ return GetPort(AddrISR());	}


	////////////////////////////////////////////////////////////////////////////
	// Returns true if any of the bitmask bits are set
	template <typename M> bool IsAnySet(M bitmask)					{ return GetPort(bitmask, AddrSSR()) ? true : false;	}
	// Returns true if all of the bitmask bits are set
	template <typename M> bool IsAllSet(M bitmask)					{ return IsAllSet(bitmask, AddrSSR());		}

	// Returns true if any of the bitmask bits are clr
	template <typename M> bool IsAnyClr(M bitmask)					{ return !IsAllSet(bitmask, AddrSSR());	}
	// Returns true if all of the bitmask bits are clr
	template <typename M> bool IsAllClr(M bitmask)					{ return GetPort(bitmask, AddrSSR()) ? false : true;	}

	// Returns the interrupt status register of the port
	ULONG GetInput(void)														{ return GetPort(AddrSSR());	}

	////////////////////////////////////////////////////////////////////////////

};


#endif /*GPIO_H_*/


