//======================================================================================
// StreamManager.h
//======================================================================================
// $RCSfile: Gen6FoundationApp/FoundationApp/StreamManager.h $
// $Revision: 1.2 $
// $Date: 2010/01/25 13:42:10EST $
//======================================================================================

//======================================================================================




//---------------------------------------------------------------------------
//
//  Module Interface Description:
//      This file contains definitions of the Stream manager and Extended
//      memory utilities.
//
//---------------------------------------------------------------------------

#ifndef StreamManagerH
#define StreamManagerH

#include "language.h"

//possibly temp, need to see how this plays with Point in the decoder
typedef struct {
	S32 x;	// x-coordinate
	S32 y;	// y-coordinate
} Point_t;

#define NO_BUFFERS        0xFFFFFFFF //max mem location, means no extended mem setup

//Tags for stream processing can be any length but MUST be terminated with an '='
#define EXPOSURE_TAG            "Exposure:EXP="
#define TARGET_WHITE_TAG        "Target White:TGW="
#define IQ_SCORE_TAG            "IQ Score:IQS="
#define ENERGY_SCORE_TAG        "Energy Score:EGS="
#define ILLUMINATION_TAG        "Illumination Value:ILL="
#define ILLUM_MAX_TAG           "Illumination Max:IMX="
#define GAIN_TAG                "Gain:GAN="
#define HEIGHT_TAG              "Height:HGT="
#define WIDTH_TAG               "Width:WDT="
#define IMAGE_NUMBER_TAG        "Image Number:IMG="
#define FAST_DECODER_TAG        "Fast Decoder:FST="
#define FULL_DECODER_TAG        "Full Decoder:FUL="
#define MOTION_SCORE_TAG        "Motion Score:MOT="
#define ENGINE_ID_TAG           "Engine ID:EID="
#define TIME_STAMP_TAG           "Time stamp:TIM="

//WARNING: Please be careful when inserting things into this structure
//The image array in the ImageBuffer_t structure needs to be word aligned
//While the compiler may fill either structure in order to acheive word alignment,
//being conscientious of it here will prevent problems later!
typedef struct {
    int height;
    int width;
    int imageNumber;
    int exposure;
    int gain;
    int illumValue;
    int illumMax;
    int iqScore;
    int energyScore;
    int motionScore;
    int fastDecoder; //1 got into, 2 decoded
    int fullDecoder; //1 got into, 2 decoded

    //info for fast to full passing
    Point_t pdfStart,pdfStop;
    Point_t newCenter; //adaptive centering?
    int timeStamp;
} ImageBufferInfo;

//Main image buffer structure
typedef struct ImageBuffer {
    ImageBufferInfo info;
    ImageBuffer *next;  // image stream link
    UINT array;         //tried to set this to be an empty [], but compliler doesn't like that
} ImageBuffer_t;

#define IMAGEARRAY(p) &((ImageBuffer_t *)p)->array
#define IMAGEINFO(p) ((ImageBuffer_t *)p)->info

/******************************************************************************
 *
 * Class:       Stream
 * Description: This class contains management functions for individual streams
 *              of images. The linking of buffers together into a stream is done
 *              through this class, but the physcial link is in the ImageBuffer_t
 *              structure.
 *
 *              See the function definitions for complete function descriptions.
 *
 *****************************************************************************/
#ifdef STREAM_CAPTURE
class Stream {
    public:
        Stream();
        Stream(UINT *start);
        ~Stream();
        void ResetStream(void);

        ImageBuffer_t *GetImage(int index);
        void AddImage( ImageBuffer_t *img );

        void SetBase( ImageBuffer_t *base );
        ImageBuffer_t *GetBase(void);

        int GetImageCount(void);

        void Decoded(int decVal);
        int DidIDecode(void);

    private:
        ImageBuffer_t *mBase;       //start of stream - first image
        ImageBuffer_t *mLastImage;  //last image
        int mCount;                 //how many
        int m_Decoded;              //whether an image in this stream decoded or not
};


/******************************************************************************
 *
 * Class:       StreamManager
 * Description: This class contains management functions for all buffers that are
 *              to be used for streaming as well at the streams themselves. The
 *              buffers that are used for streams can be either gotten directly
 *              from this manager or through raw_malloc, which the scandriver will
 *              use.
 *
 *              In order to keep streams of images for download and not
 *              impact decoding, the buffers for streams must be registered with
 *              the scandriver and locked after being added to a stream. This
 *              manager handles the index used in obtaining new images from the
 *              scandriver as well as the registering and unregistering of the
 *              buffers to be used for streaming.
 *
 *              See the function definitions for complete function descriptions.
 *
 *****************************************************************************/

class StreamManager {
    public:
        StreamManager();
        ~StreamManager();
        void Initialize(void);
        void DeInitialize(void);

        //stream management
        void StartStream( ImageBuffer_t *start );
        void EndStream(void);
        void AddImageToStream( ImageBuffer_t *img, int bufIndex );
        Stream *GetCurrentStream(void);
        Stream *GetStream(int index);
        void ClearAllStreams(void);

        //physical buffer management
        void RegisterStreamBuffers( int reg );
        void AddMemoryForStreams( BYTE *base, UINT size, UINT imgSize );
        int GetNumberOfBuffers(void);

        //general buffer utils used by functions below
        int GetNextAvailableBuffer(UINT flag);
        int ReturnBuffer(void *bufPtr, UINT flags);

        //external scandriver utilites
        UINT GetLockableBuffer(void);
        void ReturnLockableBuffer(UINT bufIndex);

        //image_malloc/free utilities
        void *GetMemoryBuffer(int bufSize);
        int ReturnMemoryBuffer(void *ptrToFree);

        //external processing management
        int GetStreamBufferSize(void);
        int GetNumberOfStreams(void);

        //used when no more images can be added to a stream
        void DoIndicate( int mode );
        void ShowReport(void);

   private:
        int mStreamIndex;           // current stream
        int mStreamTotal;           // total streams
        int mBufferTotal;           // total physical buffers
        int mLastBuff;              // last buff accessed, for speed
        int mStreamSize;            // consolidating code (ImageBuffer size + image size)
        Stream *mStreams;           // Stream table
        UINT *mRawBufferPointers;     // physical buffer table (pointers)
        int *mAvailableBuffers;       // avaible buffer table, used with flags to consolidate management
        int mIndicated;             // says memory is full and we told the user
		int mManageImages;			// flag for if we need to manage images in the scandriver as well or just the image buffers.
};


extern StreamManager *pStreamManager;   //should only use 1 of these, need to instantiate if using the system
extern UINT *pGlobalImageBuffers;       //table of scandriver pointers for registering buffers

#endif //STREAM_CAPTRE


void *image_malloc( UINT size );      //pulls buffers from streammanager if available first, then regular malloc
void image_free( void *ptrToFree );   //frees the same order as malloc above

#endif //StreamManager.h





