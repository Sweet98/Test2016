/*=================================================================================
  class: TCallBackTranslator
	This class helps with passing a class member function to a low level
 	function (C style) that needs a compatible function pointer.

 	Note: There are also macros to do this in older code. This here is more portable.
//=================================================================================
   $Source: app_kernel/CallBackHelper.h $
   $Date: 2009/03/10 21:08:07EDT $
   $Revision: 1.1 $


//=================================================================================*/

#ifndef CALLBACK_TRANSLATOR_H_F3E1EF42_35D6_11D2_93BE_00805F2987D9
#define CALLBACK_TRANSLATOR_H_F3E1EF42_35D6_11D2_93BE_00805F2987D9

//! \file

typedef void (*FunctionPointer_void_void_t)(void);
typedef void (*FunctionPointer_void_pvoid_t)(void *);

///////////////////////////////////////////////////////////////////////////////
//! A base class that helps to pass callbacks to class members.
/*!
	Be carefull with this class, it might not work for every possible compiler!
*/
class CCallBackTranslator
{
protected:
	void *pThis;										//!< Stores the this pointer of the class (callback)
	FunctionPointer_void_pvoid_t pFunction;	//!< Stores the function pointer of the class
public:
   // Two possible functions to get the address of a member function and the this pointer.
	// Virtual derived classes will use a pointer to an object and a pointer to a member function
   // to return the values.
	///////////////////////////////////////////////////////////////////////////////
	//! Return the this pointer.
	/*!
	 \return The stored this pointer of the class we want to callback
	*/
	void *GetTranslatedThisPointer(void)
	{
		return pThis;
	}
	///////////////////////////////////////////////////////////////////////////////
	//! Return the function pointer.
	/*!
	 \return The stored function pointer of the class we want to callback
	*/
	FunctionPointer_void_pvoid_t GetTranslatedFunctionPointer(void)
	{
		return pFunction;
	}
};

///////////////////////////////////////////////////////////////////////////////
//! A derived template class that helps to pass callbacks to class members.
template <class TClass> class TCallBackTranslator : public CCallBackTranslator
{
public:
	///////////////////////////////////////////////////////////////////////////////
	//!  Constructor
	/*!
	 Takes pointer to an object and pointer to a member and stores
	 them in two private variables

	 \param pObject  The this pointer of the class we want to callback
	 \param TClass::*fpt The function pointer of the class we want to callback
	*/
	TCallBackTranslator(TClass* pObject, void(TClass::*fpt)(void))
	{
   	pThis = (void*) pObject;

		union dual_t
		{
			void(TClass::*mfp)(void);
			FunctionPointer_void_pvoid_t fp;
		};
		dual_t DualPointer;
		DualPointer.mfp  = fpt;
		pFunction = (FunctionPointer_void_pvoid_t) DualPointer.fp;
	}
};

///////////////////////////////////////////////////////////////////////////////////////////
/*! \class TCallBackTranslator

	This class helps with passing a class member function to a low level
 	function (C style) that needs a compatible function pointer.

 	This is usually the case for callbacks. We defined our callbacks to allow for this trick.
	The function pointer looks like this:
 	\code
 	void (*std_action)(void *pUniversal)
 	\endcode

	The low level function also gets a void *pUniversal.
 	Here is an example:
	\code
	bool Run (void (*std_action)(void *pUniversal), void *pUniversal);
	\endcode

	\b Details:\n
	With the help of the TCallBackTranslator we can pass pointers to member functions
 	to such interfaces. A pointer to a class member is actually two pointers:
 	The function pointer and the this pointer.
	The this pointer gets translated into the "void *pUniversal" in the example above.
	The function pointer gets translated into the "void (*std_action)(void *pUniversal)".
	The callback gets then later called with the right parameters, so it really
 	calls the class member in the right way.

 	\remark There are also macros to do this in older code. This here is more portable.

	Usage example:
\code
class MyClass
 {
	....
	void CallBack(void);
	void RunThread(void);

	CThreadSimple m_Thread;
 };

void MyClass::RunThread(void)
{
	<MyClass>TCallBackTranslator Connector(this, &MyClass::CallBack);
	m_Thread.Run (Connector);	// connect our callback to the thread code
}
\endcode
*/

//=================================================================================*/

#endif /* CALLBACK_TRANSLATOR_H_F3E1EF42_35D6_11D2_93BE_00805F2987D9 */





