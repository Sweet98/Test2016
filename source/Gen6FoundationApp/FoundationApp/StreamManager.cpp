//======================================================================================
// StreamManager.cpp
//======================================================================================
// $RCSfile: Gen6FoundationApp/FoundationApp/StreamManager.cpp $
// $Revision: 1.5 $
// $Date: 2010/01/25 13:42:09EST $
//======================================================================================

//======================================================================================


//---------------------------------------------------------------------------
//
//  Module Description:
//      This file contains the management of Stream capture organization and image buffer
//      memory.
//
//---------------------------------------------------------------------------

#include "stdInclude.h"
#include "StreamManager.h"

#ifdef STREAM_CAPTURE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "beep.h"
#include "scandrivermatrixinterface.h"
#include "scandriverdebug.h"
#include "menu.h"
#include "readerconfiguration.h"
#include "IndicatorLEDs.h"

StreamManager *pStreamManager;  //the one and only, shouldn't need more than 1
UINT *pGlobalImageBuffers;      //again, scandriver registered pointers

//a good guess, 185 buffers with just extended mem, 3 images per stream min,
//that's 61 streams min, another 25 if we hit that, but not likely
#define STREAM_START        50
#define STREAM_INCREMENT    25

//available buffer flag, to keep the number of tracking tables down
#define MALLOC_FLAG         0x0001
#define SCANDRIVER_FLAG     0x0002


/********************************  Stream Class  ******************************/
/******************************************************************************
 *
 * Function:    Stream
 * Description: Starts a new stream at addr 0 with 0 images
 *
 * Arguments:   none
 *
 * Returns:     new stream
 *
 *****************************************************************************/
Stream::Stream(void)
{
    this->ResetStream();
}

/******************************************************************************
 *
 * Function:    Stream
 * Description: Starts a new stream at 'start' with 0 images
 *
 * Arguments:   starting address of stream
 *
 * Returns:     new stream
 *
 *****************************************************************************/
Stream::Stream( UINT *start )
{
    mBase = (ImageBuffer_t *)start;
    mCount = 0;
}

/******************************************************************************
 *
 * Function:    ~Stream
 * Arguments:   none
 *
 * Returns:     nothing
 *
 *****************************************************************************/
Stream::~Stream(void)
{
}

/******************************************************************************
 *
 * Function:    ResetStream
 * Arguments:   none
 *
 * Returns:     nothing
 *
 *****************************************************************************/
void Stream::ResetStream(void)
{
    mBase = NULL;
    mCount = 0;
    m_Decoded = 0;
}

/******************************************************************************
 *
 * Function:    GetImage
 * Arguments:   unsigned int    index   -   index of image requested in this stream
 *
 * Returns:     ImageBuffer_t   *       -   pointer to the image buffer
 *              NULL if error
 *
 *****************************************************************************/
ImageBuffer_t * Stream::GetImage( int index )
{
    ImageBuffer_t *returnBuf = NULL;
    ImageBuffer_t *currentBuf = mBase;

    if( index < mCount ) // bounds check
    {
        for( int j=0; j<=index; j++ )
      {
          returnBuf = currentBuf;
          currentBuf = currentBuf->next;
      }
    }

    return returnBuf;
}

/******************************************************************************
 *
 * Function:    AddImage
 * Arguments:   ImageBuffer_t * imgAddr     -   address of image to add to this stream
 *
 * Returns:     nothing
 *
 *****************************************************************************/
void Stream::AddImage( ImageBuffer_t *imgAddr )
{
    //if this is the first image
    if( mBase == NULL )
    {
        mBase = imgAddr;
    }
    else
    {
        mLastImage->next = imgAddr;
    }

    mLastImage = imgAddr;
    //mLastImage->next = NULL;
    mCount++;
}

/******************************************************************************
 *
 * Function:    SetBase
 * Arguments:   ImageBuffer_t   base    -   address of first image in this stream
 *
 * Returns:     nothing
 *
 *****************************************************************************/
void Stream::SetBase( ImageBuffer_t *base ) {
    mBase = base;
    //mBase->next = NULL;
    mLastImage = mBase;
    mCount++;
}

/******************************************************************************
 *
 * Function:    GetBase
 * Arguments:   none
 *
 * Returns:     ImageBuffer_t   base    -   address of first image in this stream
 *
 *****************************************************************************/
ImageBuffer_t * Stream::GetBase(void)
{
    return mBase;
}

/******************************************************************************
 *
 * Function:    GetImageCount
 * Arguments:   none
 *
 * Returns:     int     -   the number of images in this stream
 *
 *****************************************************************************/
int Stream::GetImageCount(void)
{
    return mCount;
}

/******************************************************************************
 *
 * Function:    Decoded
 * Arguments:   int     -   the decode value of this stream
 *
 * Returns:     nothing
 *
 *****************************************************************************/
void Stream::Decoded( int decVal )
{
    m_Decoded = decVal;
}

/******************************************************************************
 *
 * Function:    DidIDecode
 * Arguments:   none
 *
 * Returns:     int     -   the decode value of this stream
 *
 *****************************************************************************/
int Stream::DidIDecode(void)
{
    return m_Decoded;
}


/*****************************  StreamManager Class  **************************/
/******************************************************************************
 *
 * Function:    StreamManager
 * Arguments:   none
 *
 * Returns:     nothing
 *
 *****************************************************************************/
StreamManager::StreamManager(void)
{
	mStreams = NULL;
	mRawBufferPointers = NULL;
	mAvailableBuffers = NULL;
	pGlobalImageBuffers = NULL;
}

/******************************************************************************
 *
 * Function:    ~StreamManager
 * Arguments:   none
 *
 * Returns:     nothing
 *
 *****************************************************************************/
StreamManager::~StreamManager(void)
{
}

/******************************************************************************
 *
 * Function:    Initialize
 * Arguments:   none
 *
 * Returns:     nothing
 *
 *****************************************************************************/
void StreamManager::Initialize(void)
{
    mStreamIndex = mLastBuff = -1;
    mIndicated = mManageImages = 0;
    if( mStreams == NULL ) {
        mStreamSize = 0;
        mStreamTotal = STREAM_START;
        mStreams = new Stream[STREAM_START];
    }
    if( mRawBufferPointers == NULL ) {
        mBufferTotal = 0;
        mRawBufferPointers = new UINT[1];
        mRawBufferPointers[0] = NO_BUFFERS;
    }
    if( mAvailableBuffers == NULL ) {
        mAvailableBuffers = new int[1];
        mAvailableBuffers[0] = 0;
    }
    if( pGlobalImageBuffers == NULL ) {
        pGlobalImageBuffers = new UINT[1];
    }
    else {
        this->RegisterStreamBuffers(TRUE);
    }
}

/******************************************************************************
 *
 * Function:    DeInitialize
 * Arguments:   none
 *
 * Returns:     nothing
 *
 *****************************************************************************/
void StreamManager::DeInitialize(void)
{
    if( mManageImages && pGlobalImageBuffers != NULL ) {
        this->RegisterStreamBuffers(FALSE);
    }

}

/******************************************************************************
 *
 * Function:    RegisterStreamBuffers
 * Description: Register or unregisters buffer pointers with the scan driver.
 *
 * Arguments:   int reg     -   1 = register, 0 = unregister
 *
 * Returns:     nothing
 *
 *****************************************************************************/
void StreamManager::RegisterStreamBuffers( int reg )
{
    for( int j=0;j<mBufferTotal;j++ )
    {
        if( reg )
           SCN_IOControl(nOpenCntxt,HHPSD_IOCTL_REGISTER_SCAN_BUFFER,(UCHAR *)&pGlobalImageBuffers[j],0,NULL,0,NULL);
        else
           SCN_IOControl(nOpenCntxt,HHPSD_IOCTL_UNREGISTER_SCAN_BUFFER,(UCHAR *)&pGlobalImageBuffers[j],0,NULL,0,NULL);
    }
}

/******************************************************************************
 *
 * Function:    AddMemoryForStreams
 * Description: This function added memory to be managed for streams. This function is
 *              meant to be called as many times as needed, but before the manager is used
 *              for anything. This function is also meant to be used with non-heap memory.
 *
 * Arguments:   BYTE *  base    -   starting address for adding memory for stream buffers
 *                              ** a value of 0 indicated heap space and malloc() will be called
 *              UINT    memSize -   size of memory space
 *              UINT    imgSize -   size of image array portion
 *
 * Returns:     nothing
 *
 *****************************************************************************/
void StreamManager::AddMemoryForStreams( BYTE *base, UINT memSize, UINT imgSize )
{
    int oldBufCount = 0;
	
	mManageImages = imgSize; //if the image size is 0, just manage the image buffer info

    //check to see if this address space was previously added. This works because the
    //extended memory chunks will always start at the same locations.
    while( oldBufCount < mBufferTotal ) {
        if( mRawBufferPointers[oldBufCount++] == (UINT)base ) return;
    }

    mStreamSize = sizeof(ImageBuffer_t) + imgSize;

    //if memory was registered prevously, could be multiple extended chips
    //unregister what was there because the table needs to grow and addresses could change
    if( mRawBufferPointers[0] != NO_BUFFERS )
    {
        oldBufCount = mBufferTotal;
		if( mManageImages ) {
			this->RegisterStreamBuffers(FALSE);
		}
    }

    mBufferTotal += (memSize/mStreamSize);

    UINT *tempArray = new UINT[mBufferTotal]; // to store current buffer pointers
    UINT *tempAvailable = new UINT[mBufferTotal];

    //save what's here already, if anything
    for( int j=0;(j<oldBufCount);j++ )
    {
        tempArray[j] = mRawBufferPointers[j];
        tempAvailable[j] = mAvailableBuffers[j];
    }

    // need to make larger tables
    delete [] pGlobalImageBuffers;
    delete [] mRawBufferPointers;
    delete [] mAvailableBuffers;

    pGlobalImageBuffers = new UINT[mBufferTotal];
    mRawBufferPointers = new UINT[mBufferTotal];
    mAvailableBuffers = new int[mBufferTotal];

    //copy everything back and reset availability to fully available
    for(int j=0;j<oldBufCount;j++ )
    {
        mRawBufferPointers[j] = tempArray[j];
        mAvailableBuffers[j] = tempAvailable[j];
    }

    //create the new pointers, base = 0 means use the heap space
    for(int j=oldBufCount;j<mBufferTotal;j++ )
    {
        if( base == NULL )
            mRawBufferPointers[j] = (UINT)malloc(mStreamSize);
        else
            mRawBufferPointers[j] = (UINT)(base+(mStreamSize*j));

        mAvailableBuffers[j] = (MALLOC_FLAG|SCANDRIVER_FLAG);
    }
    delete [] tempArray;
    delete [] tempAvailable;

	if( mManageImages ) {
		this->RegisterStreamBuffers(TRUE); //register with scan driver
	}
}

/******************************************************************************
 *
 * Function:    GetNextAvailableBuffer
 * Arguments:   unsigned int    flag    -   type of buffer needed
 *
 * Returns:     int     -   index of buffer found
 *              NO_BUFFERS if error
 *
 *****************************************************************************/
int StreamManager::GetNextAvailableBuffer( UINT flag )
{
    //if no memory was set up
    if( mRawBufferPointers[0] == NO_BUFFERS )
        return NO_BUFFERS;

    int bufStart = mLastBuff++;

    while( (mAvailableBuffers[mLastBuff]&flag) == 0 )
    {
        mLastBuff = (mLastBuff%mBufferTotal);

        //if we've gone through all available buffers
        if( mLastBuff == bufStart )
            return NO_BUFFERS;
        mLastBuff++;
    }

    return mLastBuff;
}

 /******************************************************************************
 *
 * Function:    ReturnBuffer
 * Arguments:   unsigned int    flags   -   buffer type(s) to set to available
 *              void *          bufPtr  -   pointer to return
 *
 * Returns:     nothing
 *
 *****************************************************************************/
int StreamManager::ReturnBuffer( void *bufPtr, UINT flags )
{
    for( int j=0;j<mBufferTotal;j++ )
    {
        if( mRawBufferPointers[j] == (UINT)bufPtr )
        {
            mAvailableBuffers[j] |= flags;
            return TRUE;
        }
    }

    //if we got through everything, no buffer was found to match
    return FALSE;
}

/******************************************************************************
 *
 * Function:    GetLockableBuffer
 * Description: Gets a buffer index to be used for getting a scan buffer from the
 *              scandriver.
 *
 * Arguments:   none
 *
 * Returns:     unsigned int    -   buffer index
 *
 *****************************************************************************/
UINT StreamManager::GetLockableBuffer(void)
{
    UINT retBuf = (UINT)(this->GetNextAvailableBuffer(SCANDRIVER_FLAG));

    //if there was a buffer available, mark it unavailable
    if( retBuf != NO_BUFFERS )
        mAvailableBuffers[retBuf] = mAvailableBuffers[retBuf] &~ SCANDRIVER_FLAG;

	return retBuf;
}

/******************************************************************************
 *
 * Function:    ReturnLockableBuffer
 * Description: Sets a buffer that was taken to be used by the scandriver to be available
 *
 * Arguments:   unsigned int    bufIndex    -   index of the buffer to set available
 *
 * Returns:     nothing
 *
 *****************************************************************************/
void StreamManager::ReturnLockableBuffer( UINT bufIndex )
{
    this->ReturnBuffer( (void *)mRawBufferPointers[bufIndex], SCANDRIVER_FLAG );
}

/******************************************************************************
 *
 * Function:    GetMemoryBuffer
 * Description: Gets a buffer to be used for writing to, mainly by image_malloc
 *
 * Arguments:   none
 *
 * Returns:     void *      -   pointer to buffer location
 *
 *****************************************************************************/
void * StreamManager::GetMemoryBuffer(int bufSize)
{
    if( bufSize != (mStreamSize-sizeof(ImageBuffer_t) ) )
        return NULL;

    int retIndex = this->GetNextAvailableBuffer(MALLOC_FLAG);

    //if there was no buffer available
    if( retIndex == NO_BUFFERS )
        return NULL;

    mAvailableBuffers[retIndex] = mAvailableBuffers[retIndex] &~ MALLOC_FLAG;

    return (void *)mRawBufferPointers[retIndex];
}

/******************************************************************************
 *
 * Function:    ReturnMemoryBuffer
 * Description: Sets a pointer that was taken for use, most likley from image_malloc,
 *              to be available
 *
 * Arguments:   void *      ptrToFree   -   pointer to make available
 *
 * Returns:     int     -   TRUE = buffer was returned, FALSE = it wasn't
 *
 *****************************************************************************/
int StreamManager::ReturnMemoryBuffer( void *ptrToFree )
{
    return (this->ReturnBuffer(ptrToFree,MALLOC_FLAG));
}

/******************************************************************************
 *
 * Function:    GetNumberOfBuffers
 * Arguments:   none
 *
 * Returns:     int     -   number of physical buffers total
 *
 *****************************************************************************/
int StreamManager::GetNumberOfBuffers(void)
{
    return mBufferTotal;
}

/******************************************************************************
 *
 * Function:    GetNumberOfStreams
 * Arguments:   none
 *
 * Returns:     int     -   number of streams saved
 *
 *****************************************************************************/
int StreamManager::GetNumberOfStreams(void)
{
    return mStreamIndex+1;
}

/******************************************************************************
 *
 * Function:    AddImageToStream
 * Arguments:   ImageBuffer_t *     img     -   buffer to add to stream
 *              int            bufIndex     -   index of buffer used to get this buffer from
 *                                              the scandriver
 *
 * Returns:     nothing
 *
 *****************************************************************************/
void StreamManager::AddImageToStream( ImageBuffer_t *img, int bufIndex )
{
    //if the stream table wasn't set up yet
    if( mStreams == NULL )
    {
        mStreamTotal = STREAM_START;
        mStreams = new Stream[mStreamTotal];
    }
    //if the table wasn't big enough
    else if( mStreamIndex == mStreamTotal )
    {
        mStreamTotal += STREAM_INCREMENT;
        Stream *tempStr = new Stream[mStreamTotal];

        //copy what was there
        for( int j=0;j<=mStreamIndex;j++ )
        {
            tempStr[j] = mStreams[j];
        }

        //make the table bigger
        delete [] mStreams;
        mStreams = new Stream[mStreamTotal];

        //copy it back
        for( int j=0;j<=mStreamIndex;j++ )
        {
            mStreams[j] = tempStr[j];
        }

        delete [] tempStr;
    }

    //add the image to the current stream
    mStreams[mStreamIndex].AddImage(img);

    //mark the buffer used to get this image as unavailable, locking it in the scandriver
    mAvailableBuffers[bufIndex] = mAvailableBuffers[bufIndex] &~ SCANDRIVER_FLAG;

}

/******************************************************************************
 *
 * Function:    EndStream
 * Description: Ends the current stream by going to the next one, unless the indicator
 *              was triggered, which means out of memory for streams. This avoids
 *              confusing download messages when downloading streams.
 *
 * Arguments:   none
 *
 * Returns:     nothing
 *
 *****************************************************************************/
void StreamManager::EndStream(void)
{
    if( !mIndicated )
        mStreamIndex++;
}

/******************************************************************************
 *
 * Function:    GetCurrentStream
 * Arguments:   none
 *
 * Returns:     Stream *    -   the current stream
 *
 *****************************************************************************/
Stream * StreamManager::GetCurrentStream(void)
{
    return &mStreams[mStreamIndex];
}

/******************************************************************************
 *
 * Function:    GetStream
 * Arguments:   int     index   -   index of the stream to get, usually used when
 *                                  downloading streams
 *
 * Returns:     Stream *    -   pointer to the stream requested
 *
 *****************************************************************************/
Stream * StreamManager::GetStream(int index)
{
    return &mStreams[index];
}

/******************************************************************************
 *
 * Function:    ClearAllStreams
 * Arguments:   none
 *
 * Returns:     nothing
 *
 *****************************************************************************/
void StreamManager::ClearAllStreams(void)
{
    for( int j=0; j<mBufferTotal; j++ )
    {
        //make all buffers available
        mAvailableBuffers[j] |= (SCANDRIVER_FLAG);
        //blank out data
        //memset( (void *)mRawBufferPointers[j],0x00,mStreamSize );
        //return any locked buffers to the scan driver
        SCN_IOControl(nOpenCntxt,HHPSD_IOCTL_RETURN_SCAN,(UCHAR *)&pGlobalImageBuffers[j],0,NULL,0,NULL);
    }

    for( int j=0; j<=mStreamIndex; j++ )
    {
        //reset all streams
        mStreams[j].ResetStream();
    }

    //reset all flags and counters
    mStreamIndex = mLastBuff = -1;
    mIndicated = 0;
}

/******************************************************************************
 *
 * Function:    GetStreamBufferSize
 * Arguments:   none
 *
 * Returns:     int     -   size of each buffer in the stream, assumed to be all equal
 *
 *****************************************************************************/
int StreamManager::GetStreamBufferSize(void)
{
    return mStreamSize;
}


/******************************************************************************
 *
 * Function:    DoIndicate
 * Arguments:   int     mode    -   passthrough for menu setting STRIND, also marks
 *                                  no buffers left for streams
 *
 * Returns:     nothing
 *
 *****************************************************************************/
void StreamManager::DoIndicate( int mode )
{

    switch( mode )
    {
        case 3: //both beep and LED
        case 1: //Beeper only
            //if we hadn't indicated to the user before, do it once
            //keeps the LED on after beeping, whether this or any other beep
            if( !mIndicated )
                SoundOOMBeep();

            if( mode == 1 ) break;
        case 2: //LED only
            GoodReadLedOn(); break;
        default:
            break;
    }

    mIndicated = 1;
}

/******************************************************************************
 *
 * Function:    ShowReport
 * Arguments:   none
 *
 * Returns:     nothing
 *
 *****************************************************************************/
void StreamManager::ShowReport(void)
{
    dbg_output( "\r\n---Stream Menu Command Status---\r\n" );
    dbg_output( "Description                Command     Value\r\n");
    dbg_output( " Enabled                    IMGSTR      %d \r\n",GET_SETTING(SaveStream));
    dbg_output( " Auto Dump                  STRADP      %d \r\n",GET_SETTING(StreamAutoDump));
    dbg_output( " Auto Clear                 STRACL      %d \r\n",GET_SETTING(StreamAutoClear));
    dbg_output( " Decodes to wait            STRDEC      %d \r\n",GET_SETTING(StreamDecodeStart));
    dbg_output( " Extra images after decode  STREXT      %d \r\n",GET_SETTING(StreamExtraImages));

    dbg_output( "\r\n---Stream Manager Report---\r\n" );
    dbg_output( " Total buffers:            %d \r\n", mBufferTotal );
    dbg_output( " Current stream index:     %d \r\n", mStreamIndex );
    dbg_output( "\r\n---Individual Buffer Status---\r\n");
        dbg_output( "Index    Address      Status\r\n");
    for( int j=0; j<mBufferTotal; j++ ) {
        dbg_output( " %03d     %08X       %04X   \r\n",j,mRawBufferPointers[j],mAvailableBuffers[j] );
    }
    dbg_output( "\r\n---Individual Stream Status---\r\n");
        dbg_output( "Index    Base       Images\r\n");
    for( int j=0; j<=mStreamIndex; j++ ) {
        dbg_output( " %03d     %08X       %04d   \r\n",j,mStreams[j].GetBase(),mStreams[j].GetImageCount() );
    }

    dbg_output( "\r\n--- End of Report ---\r\n" );

}
#endif //STREAM_CAPTURE

/******************************************************************************
 *
 * Function:    image_malloc
 * Arguments:   unsigned int    size    -   size of buffer to malloc, this is
 *                              meant to be used for images and is assumed to be
 *                              the same size as programmed in the stream manager
 *
 * Returns:     void *  -   pointer to memory space
 *
 *****************************************************************************/
void *image_malloc( UINT size )
{
    void *p = NULL;
#ifdef STREAM_CAPTURE
    //if the manager was set up
    if( pStreamManager )
        p = pStreamManager->GetMemoryBuffer(size);
#endif
    //if a buffer wasn't available, use the heap
    if( p == NULL )
    {
        p = (ImageBuffer_t *)malloc( sizeof(ImageBuffer_t)+size );
    }

    return p;
}

/******************************************************************************
 *
 * Function:    image_free
 * Arguments:   void *  ptrToFree   -   pointer to free back to whatever
 *
 * Returns:     nothing
 *
 *****************************************************************************/
void image_free( void *ptrToFree )
{
#ifdef STREAM_CAPTURE
    //if there is no stream manager or the manager couldn't return the buffer, free it to the heap
    if( (pStreamManager == NULL) || ((pStreamManager->ReturnMemoryBuffer(ptrToFree)) == FALSE) )
#endif
        free(ptrToFree);
}


