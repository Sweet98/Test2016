//======================================================================================
// PluginFileProcessing.h
//======================================================================================
// $RCSfile: Gen6FoundationApp/FoundationApp/Plugin/PluginFileProcessing.h $
// $Revision: 1.3 $
// $Date: 2010/02/12 16:29:37EST $
//======================================================================================

//======================================================================================

#ifndef HTRANSFERPROCESSING_H
#define HTRANSFERPROCESSING_H

#include "HTransferProcessing.h"

class PluginFileProcessor : public HTAG_FILE_POST_TRANSFER_PROCESSOR
{
public:
	PluginFileProcessor();
	~PluginFileProcessor();
	virtual void StoreFile(int * /* Status */, char * /* Result */, HTAG_FILE * /* NewFile */);
	virtual void RetrieveFile(int * /* Status */, char * /* Result */, HTAG_FILE * /* File */);

};

extern PluginFileProcessor *FileProcessor;

#endif

//=================================================================================
//                       File Modification History
//===================================================================================

