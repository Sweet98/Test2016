/*===================================================================
  This is a generic container class for holding various descriptors.
  The intent is to eventually replace all pointers to descriptors and
  their associated lengths with this class.  Doing this will clean the
  code up and help CUsb2Enum::get_descriptor() know when to issue a 
  STALL packet (it it can't find the descriptor, it should STALL).
  
  Since it's meant to be used just for the descriptors and they're
  usually static member variables because they have to persist, we
  don't actually copy the data to the container--we just keep a 
  pointer to it.
=====================================================================
   $Source: interfaces_usb2/base_usb2/DescriptorQueues.cpp $
   $Date: 2011/03/08 15:15:35EST $
   $Revision: 1.1 $


===================================================================*/
#include "DescriptorQueues.h"



/*----------------------------------------------------------------------------------------
Add

  Adds a descriptor.  If it already exists, it won't do anything and leave the first
  copy.

  Parameters:  
  			id[in]:  ID of the descriptor.  It'll be the wIndex in the USB GET_DESCRIPTOR
  			(interface number or string index)

  			pData[in]:  descriptor data

  			length[in]:  length of descriptor data

  Returns:  nothing.
----------------------------------------------------------------------------------------*/
void CDescriptorQueue::Add(int id, const unsigned char * pData, int length)
{
	Descriptor_t descriptor;
	
	
	if ( ! Find(id, &descriptor) )
	{
		descriptor.id = id;
		descriptor.pDescData = pData;
		descriptor.DescLength = length;
		m_Descriptors.Add(&descriptor);
	}
}


/*----------------------------------------------------------------------------------------
Remove

  Removes a descriptor if it exists.

  Parameters:  
  			id[in]:  ID of the descriptor.

  Returns:  nothing.
----------------------------------------------------------------------------------------*/
void CDescriptorQueue::Remove(int id)
{
	int index;
	
	
	if ( Find(id, &index) ) m_Descriptors.RemoveAt(index);
}


/*----------------------------------------------------------------------------------------
Find

  Searches the queue for a descriptor.  If it does not exist, it won't set pFoundDescriptor.

  Parameters:  
  			id[in]:  ID of the descriptor

  			pFoundDescriptor[in]:  pointer to Descriptor_t struct

  Returns:  true if the descriptor was found, false otherwise.
----------------------------------------------------------------------------------------*/
bool CDescriptorQueue::Find(int id, Descriptor_t * pFoundDescriptor)
{
	int index;
	bool bFound;
	
	bFound = Find(id, &index);
	if ( bFound ) *pFoundDescriptor = m_Descriptors.ElementAt(index);
	return bFound;
}


/*----------------------------------------------------------------------------------------
Find

  Searches the queue for a descriptor.  If it does not exist, it won't set pFoundIndex.

  Parameters:  
  			id[in]:  ID of the descriptor

  			pFoundIndex[in]:  pointer to the index in the array, if found

  Returns:  true if the descriptor was found, false otherwise.
----------------------------------------------------------------------------------------*/
bool CDescriptorQueue::Find(int id, int * pFoundIndex)
{
	bool bFound = false;
	
	*pFoundIndex = -1;  // compiler was generating a variable used before set warning in Remove() and the other Find().  Rather than fix it in two places, I just always set it here.
	for (int i = 0; (i < m_Descriptors.GetSize()) && ! bFound; i++)
	{
		Descriptor_t * pThisReportDescriptor = m_Descriptors.ElementAt(i);
		if ( pThisReportDescriptor->id == id )
		{
			*pFoundIndex = i;
			bFound = true;
		}
	}
	return bFound;
}




