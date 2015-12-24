/* ==============================================================================
   ECI Module Header
   ==============================================================================

   $RCSfile: Gen6FoundationApp/FoundationApp/eci/eci_base.h $
   $Revision: 1.1 $
   $Date: 2009/03/05 14:45:52EST $

   ==============================================================================
 
   ============================================================================== */
//! \file

#ifndef ECI_BASE_H_20DB49DD_8F9C_4ADD_9830_CF227654ADFA__INCLUDED_
#define ECI_BASE_H_20DB49DD_8F9C_4ADD_9830_CF227654ADFA__INCLUDED_

///////////////////////////////////////////////////////////////////////////////
/// Base class for the ECI decoder. (ECI == Extended Channel Interpretation)
/*!
	
*/
class CEciBase
{
public:
	CEciBase(void);
//	~CEciBase(void);

protected:
	// Helpers that Searches for a terminating character
	size_t ParseEndOfECI(const bchar_t *pIn, size_t size, bchar_t Endmark);
	bchar_t *SearchForNextBackslash(const bchar_t *pIn, size_t size)
	{
		return (bchar_t*)memchr(pIn, '\\', size);
	}


	enum EciContents_t
	{
		empty=0,
		ECIC_CODEPAGE=0x0001,
		ECIC_MACRO=0x0002,
		ECIC_LAST_MACRO=0x0004,
		ECIC_MACRO_FILENAME=0x0010,
		ECIC_MACRO_SEGMENT_COUNT=0x0020,
		ECIC_MACRO_TIMESTAMP=0x0040,
		ECIC_MACRO_SENDER=0x0080,
		ECIC_MACRO_ADDRESSEE=0x0100,
		ECIC_MACRO_FILESIZE=0x0200,
		ECIC_MACRO_CHECKSUM=0x0400,

		ECIC_BAD_MACRO=0x01000,
		ECIC_BAD_PARAMETER=0x02000,
		ECIC_BAD_CODEWORD=0x04000,
		ECIC_BAD_CHARSET=0x08000,
		ECIC_ERROR=ECIC_BAD_MACRO|ECIC_BAD_PARAMETER|ECIC_BAD_CODEWORD|ECIC_BAD_CHARSET
	};

	enum EciType_t
	{
		ECIT_empty=0,
		ECIT_GLI=0x0001,
		ECIT_MACRO_HEADER=0x0002,
		ECIT_DISABLED = 0x8000
	};

	enum modeflags_t
	{
		ECIMO_None=0,
		ECIMO_SHOW_GLI=0x01,
		ECIMO_SHOW_MACRO=0x02,
		ECIMO_ENABLE_MACRO=0x04,
		ECIMO_SHOW_ECI=0x10,
		ECIMO_SHOW_ONLY_UNKNOWN_ECI=0x20,
//		ECIMO_TRANSLATE_PDF_TO_ECI=0x40,
		ECIMO_ENABLE_ECI=0x80,
		ECIMO_DOUBLE_BACKSLASH=ECIMO_SHOW_GLI|ECIMO_SHOW_MACRO|ECIMO_SHOW_ECI|ECIMO_SHOW_ONLY_UNKNOWN_ECI
	};

//  commented out a few functions so I can control the usage of them
//	bool IsShowGLI(void)					{ return m_ModeFlags & ECIMO_SHOW_GLI				? true : false;	}
//	bool IsShowMacro(void)				{ return m_ModeFlags & ECIMO_SHOW_MACRO			? true : false;	}
	bool IsShowECI(void)					{ return m_ModeFlags & ECIMO_SHOW_ECI				? true : false;	}
//	bool IsDoublingBackSlash(void)	{ return m_ModeFlags & ECIMO_DOUBLE_BACKSLASH	? true : false;	}

	bool IsMacroEnabled(void)			{ return m_ModeFlags & ECIMO_ENABLE_MACRO			? true : false;	}
	bool IsECIEnabled(void)				{ return (m_ModeFlags & ECIMO_ENABLE_ECI)			? true : false;	}

	// data members
	UINT m_ModeFlags;			//! Similar to symbology option flags
	UINT m_Contents;
};


#endif // ECI_BASE_H_20DB49DD_8F9C_4ADD_9830_CF227654ADFA__INCLUDED_



