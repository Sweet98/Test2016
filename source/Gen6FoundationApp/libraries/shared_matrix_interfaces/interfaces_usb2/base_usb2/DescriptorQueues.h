/*===================================================================
  See .cpp file for description 

=====================================================================
   $Source: interfaces_usb2/base_usb2/DescriptorQueues.h $
   $Date: 2011/03/08 15:15:35EST $
   $Revision: 1.1 $


===================================================================*/
#ifndef _DescriptorQueues_h_
#define _DescriptorQueues_h_

#include "DataArray.h"



class CDescriptorQueue
{
public:
	struct Descriptor_t
	{
		int id; // could be interface number or string index
		const unsigned char * pDescData;
		int DescLength;
		
		Descriptor_t(void){};
		Descriptor_t(Descriptor_t * pNewDesc) {*this = *pNewDesc;}  // DATA_ARRAY::Add() needs this constructor.
	};
	
	void Add(int id, const unsigned char * pDescriptorData, int length);
	void Remove(int id);
	bool Find(int id, Descriptor_t * pFoundDescriptor);
	
protected:
	 
	DATA_ARRAY <Descriptor_t> m_Descriptors;

	bool Find(int id, int * pFoundIndex);
};



#endif // endof #ifndef _DescriptorQueues_h_


