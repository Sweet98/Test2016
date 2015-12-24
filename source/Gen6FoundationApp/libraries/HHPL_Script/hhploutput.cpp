//======================================================================================
// HHPLOutput.cpp
//======================================================================================
// $RCSfile: hhploutput.cpp $
// $Revision: 1.1 $
// $Date: 2009/06/12 16:55:19EDT $
//======================================================================================

//======================================================================================





//---------------------------------------------------------------------------
//
//  Class name:     HHPL_OUTPUT
//
//  Class description:
//      This class handles storage for HHPL output, namely the output string
//		and data markers.  When an application desire to run an HHPL program,
//		an HHPL_OUTPUT object is passed to the HHPL processor, along with
//		the HHPL program, in order to receive the text output and any special
//		actions that must take place, such as indicating that there is to be a
//		pause at a certain point of reporting the output data stream.
//
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
#include	"HHPLOutput.h"

#include	<ctype.h>		// data type conversion functions
#include	<string.h>		// 'C' string functions
#include	<stdio.h>		// Standard I/O



//
// Constructor #1 (starting out empty)
//
HHPL_OUTPUT::HHPL_OUTPUT(void)
{
	Data = new HWSTRING(20L, true);

// Allocate storage to the data markers
    MaximumNumberOfDataMarkers = MINIMUM_NUMBER_OF_EMPTY_DATA_MARKERS;
    DataMarker = new DATA_MARKER[MaximumNumberOfDataMarkers];

// Allocate storage to the data field positions
    MaximumNumberOfDataFieldPositions = MINIMUM_NUMBER_OF_EMPTY_DATA_FIELD_POSITIONS;
    DataFieldPosition = new DATA_FIELD_POSITION[MaximumNumberOfDataFieldPositions];

// Initialize all fields.
	Initialize();
}



//
// Constructor #2 (starting out with a copy of existing HHPL output)
//
HHPL_OUTPUT::HHPL_OUTPUT(HHPL_OUTPUT *ExistingOutput)
{
	Data = new HWSTRING(20L, true);

// Allocate storage to the data markers
    MaximumNumberOfDataMarkers = MINIMUM_NUMBER_OF_EMPTY_DATA_MARKERS;
    DataMarker = new DATA_MARKER[MaximumNumberOfDataMarkers];
    NumberOfDataMarkers = 0;

// Allocate storage to the data field positions
    MaximumNumberOfDataFieldPositions = MINIMUM_NUMBER_OF_EMPTY_DATA_FIELD_POSITIONS;
    DataFieldPosition = new DATA_FIELD_POSITION[MaximumNumberOfDataFieldPositions];
    NumberOfDataFieldPositions = 0;

    Copy(ExistingOutput);

    return;
}





// Initialize to indicate no data.
void HHPL_OUTPUT::Initialize(void)
{
	Data->Size = 0;
    NumberOfDataMarkers = 0;
    NumberOfDataFieldPositions = 0;
	MissingDataErrorFlagged = false;
	MissingDataWarningFlagged = false;
	MissingDataTerminateFlagged = false;

    return;
}





//
// Free space allocated to the local string.
//
HHPL_OUTPUT::~HHPL_OUTPUT(void)
{
	delete Data;
    delete DataMarker;
    delete DataFieldPosition;
}




//
// Create a new timer marker at the end of the data string.
//
void HHPL_OUTPUT::MarkDataWithTimer(unsigned int Timer)
{
// If we're out of space for a new data marker, reallocate it.
    if(NumberOfDataMarkers >= MaximumNumberOfDataMarkers)
    {
    	int		i;
		MaximumNumberOfDataMarkers += MINIMUM_NUMBER_OF_EMPTY_DATA_MARKERS;
        DATA_MARKER *TemporaryDataMarker = new DATA_MARKER[MaximumNumberOfDataMarkers];
        for(i = 0;    i < (int) NumberOfDataMarkers;    ++i)
    	{
        	TemporaryDataMarker[i] = DataMarker[i];
    	}
        delete DataMarker;
        DataMarker = TemporaryDataMarker;
    }

// Add the new timer data marker.
	DataMarker[NumberOfDataMarkers].Position = Data->Size;
    DataMarker[NumberOfDataMarkers].Timer = Timer;
	DataMarker[NumberOfDataMarkers].Action = WAIT_FOR_TIME_TO_ELAPSE;
	++NumberOfDataMarkers;

    return;
}





//
// Create a new port switch marker at the end of the data string.
//
void HHPL_OUTPUT::MarkDataWithPortSwitch(void)
{
// If we're out of space for a new data marker, reallocate it.
    if(NumberOfDataMarkers >= MaximumNumberOfDataMarkers)
    {
    	int		i;
		MaximumNumberOfDataMarkers += MINIMUM_NUMBER_OF_EMPTY_DATA_MARKERS;
        DATA_MARKER *TemporaryDataMarker = new DATA_MARKER[MaximumNumberOfDataMarkers];
        for(i = 0;    i < (int) NumberOfDataMarkers;    ++i)
    	{
        	TemporaryDataMarker[i] = DataMarker[i];
    	}
        delete DataMarker;
        DataMarker = TemporaryDataMarker;
    }

// Add the new port switch data marker.
	DataMarker[NumberOfDataMarkers].Position = Data->Size;
	DataMarker[NumberOfDataMarkers].Action = SWITCH_PORTS;
	++NumberOfDataMarkers;

    return;
}





// Add new data field and store position information.
void HHPL_OUTPUT::AppendDataField(HWSTRING *DataField)
{
// If we're out of space for a new data field position, reallocate.
    if(NumberOfDataFieldPositions >= MaximumNumberOfDataFieldPositions)
    {
    	int		i;
		MaximumNumberOfDataFieldPositions += MINIMUM_NUMBER_OF_EMPTY_DATA_FIELD_POSITIONS;
        DATA_FIELD_POSITION *TemporaryDataFieldPosition =
        										new DATA_FIELD_POSITION[MaximumNumberOfDataFieldPositions];
        for(i = 0;    i < (int) NumberOfDataFieldPositions;    ++i)
    	{
        	TemporaryDataFieldPosition[i] = DataFieldPosition[i];
    	}
        delete DataFieldPosition;
        DataFieldPosition = TemporaryDataFieldPosition;
    }

// Add the new data field position.
	DataFieldPosition[NumberOfDataFieldPositions].Start = Data->Size;
    DataFieldPosition[NumberOfDataFieldPositions].Length = DataField->Size;
	++NumberOfDataFieldPositions;

// Add data to output string
	Data->Append(DataField);

    return;
}




// Add new fixed text.
void HHPL_OUTPUT::AppendFixedText(HWSTRING *FixedText)
{
	Data->Append(FixedText);

    return;
}



//
// Create a copy of existing output.
//
void HHPL_OUTPUT::Copy(HHPL_OUTPUT *ExistingOutput)
{
	int		i;


// Copy the data.
	Data->Copy(ExistingOutput->Data);

// If we're going to run out of space for data markers, reallocate.
    if(ExistingOutput->NumberOfDataMarkers >= MaximumNumberOfDataMarkers)
    {
    	delete DataMarker;
		MaximumNumberOfDataMarkers = MINIMUM_NUMBER_OF_EMPTY_DATA_MARKERS + ExistingOutput->NumberOfDataMarkers;
        DataMarker = new DATA_MARKER[MaximumNumberOfDataMarkers];
    }

// Copy the data markers.
	for(i = 0;    i < (int) ExistingOutput->NumberOfDataMarkers;    ++i)
    {
    	DataMarker[i] = ExistingOutput->DataMarker[i];
    }
	NumberOfDataMarkers = ExistingOutput->NumberOfDataMarkers;



// If we're going to run out of space for data field positions, reallocate.
    if(ExistingOutput->NumberOfDataFieldPositions >= MaximumNumberOfDataFieldPositions)
    {
    	delete DataFieldPosition;
		MaximumNumberOfDataFieldPositions =
        			MINIMUM_NUMBER_OF_EMPTY_DATA_FIELD_POSITIONS + ExistingOutput->NumberOfDataFieldPositions;
        DataFieldPosition = new DATA_FIELD_POSITION[MaximumNumberOfDataFieldPositions];
    }

// Copy the data field positions.
	for(i = 0;    i < (int) ExistingOutput->NumberOfDataFieldPositions;    ++i)
    {
    	DataFieldPosition[i] = ExistingOutput->DataFieldPosition[i];
    }
	NumberOfDataFieldPositions = ExistingOutput->NumberOfDataFieldPositions;

    return;
}





