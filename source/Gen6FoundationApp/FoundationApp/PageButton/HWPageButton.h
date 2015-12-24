//======================================================================================
//	Revision information
//======================================================================================
// $RCSfile: Gen6FoundationApp/FoundationApp/PageButton/HWPageButton.h $
// $Revision: 1.2 $
// $Date: 2010/01/25 03:53:43EST $
//======================================================================================
//======================================================================================

//======================================================================================
#ifndef HWPAGEBUTTON_H_
#define HWPAGEBUTTON_H_

#define DEBOUNCE_TIMER 2

class CHWPageButton
{
public:
	static int CreateHWPageButton(void);		// call once in main to create this class
	static void DestroyHWPageButton(void);		// call when destroying this class (never usually)

	virtual bool IsPageButtonPulled(void); // trigger pressed debounced true = pressed
	void ResetPageButton(void);
	virtual void PrepareForStop(void);  // set up irq to wake up from stop mode
	virtual void UnPrepareForStop(void); // shutdown irq after waking up from stop
protected:
	CHWPageButton()
	{
		prevPagePulled=false;
		m_PagePulled=false;
		m_PageButtonTimer=DEBOUNCE_TIMER;
	}
	;
	virtual ~CHWPageButton()
	{}
	;
	static void DoDebounce(void); // wrapper for irq function
	virtual bool IsHwPaging(void); // raw not debounced trigger
	virtual int SetUpHardware(void); // called by CreateHWTrigger to set up hw pins

	virtual void DebouncePageButton(void);
	bool prevPagePulled;
	bool m_PagePulled;
	volatile unsigned int m_PageButtonTimer;
	bool isIntEnabled;
};
extern CHWPageButton*pCHWPaging;


#endif /*HWPAGEBUTTON_H_*/


//
