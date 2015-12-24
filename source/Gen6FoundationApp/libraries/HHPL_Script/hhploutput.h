//======================================================================================
// HHPLOutput.h
//======================================================================================
// $RCSfile: hhploutput.h $
// $Revision: 1.2 $
// $Date: 2011/11/14 17:05:03EST $
//======================================================================================

//======================================================================================






//---------------------------------------------------------------------------
//
//  Class definitions for HHPL_OUTPUT
//
//---------------------------------------------------------------------------
#ifndef HHPL_OUTPUTH
#define HHPL_OUTPUTH
//---------------------------------------------------------------------------

#include "HWSTRING.h"



//---------------------------------------------------------------------------
class HHPL_OUTPUT
{

private:


protected:	// User declarations


public:		// User declarations

	HWSTRING	*Data;			// Output data stream


	#define		MINIMUM_NUMBER_OF_EMPTY_DATA_MARKERS		20	// When reallocating data markers
																// because we ran out of space,
																// this is the minimum number to
																// reallocate.

	typedef enum {						// Actions that can be marked to take place when the data
    	WAIT_FOR_TIME_TO_ELAPSE=0,		// stream gets reported.
		SWITCH_PORTS
    } DATA_MARKER_ACTIONS;

    typedef struct {
    	unsigned long			Position;		// Position in data stream where special action is
												//	to take place.
        unsigned long			Timer;			// For delay action, this is the time to delay
												//	(expressed in milliseconds)
        DATA_MARKER_ACTIONS		Action;			// Action to take before we start reporting from
												//	Position onward in the data stream.
    } DATA_MARKER;

    DATA_MARKER		*DataMarker;

    unsigned long	NumberOfDataMarkers;
    unsigned long	MaximumNumberOfDataMarkers;


	#define		MINIMUM_NUMBER_OF_EMPTY_DATA_FIELD_POSITIONS	20

    typedef struct {
    	unsigned long			Start;
    	unsigned long			Length;
    } DATA_FIELD_POSITION;

    DATA_FIELD_POSITION	*DataFieldPosition;

    unsigned long	NumberOfDataFieldPositions;
    unsigned long	MaximumNumberOfDataFieldPositions;


	bool		MissingDataErrorFlagged;	// Set by string searches or match string operations to indicate
    										//	that we require data to be present and it wasn't found.
	bool		MissingDataWarningFlagged;	// Set by string searches or match string operations to indicate
    										//	that data was not found.
	bool		MissingDataTerminateFlagged;// Set by string searches or match string operations to indicate
    										//	that data was not found.

// Constructor #1 (starting out empty)
	HHPL_OUTPUT(void);

// Constructor #2 (starting out with a copy of existing HHPL output)
	HHPL_OUTPUT(HHPL_OUTPUT *ExistingOutput);

// Free space allocated to the local string.
	~HHPL_OUTPUT(void);

// Initialize to indicate no data.
    void Initialize(void);

// Create a new timer marker at the end of the data string.
	void MarkDataWithTimer(unsigned int Timer);

// Create a new port switch marker at the end of the data string.
	void MarkDataWithPortSwitch(void);

// Add new data field and store position information.
	void AppendDataField(HWSTRING *DataField);

// Add new fixed text.
	void AppendFixedText(HWSTRING *FixedText);

// Create a copy of existing output.
	void Copy(HHPL_OUTPUT *ExistingOutput);
	
private:
	HHPL_OUTPUT (const HHPL_OUTPUT&);            // declared private, but not defined, to prevent copying         
	HHPL_OUTPUT& operator=(const HHPL_OUTPUT&);  // declared private, but not defined, to prevent copying 

};
//---------------------------------------------------------------------------
#endif		//	HHPL_OUTPUTH





