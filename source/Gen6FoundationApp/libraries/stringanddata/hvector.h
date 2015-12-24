/*===================================================================
  This template class is disigned to mimic the STL vector as much as
  practical.  When we can't/don't meet ISO vector spec are noted in 
  each method's documentation.  These notes will be labelled 
  "ISO Note:".
  
  ISO Note:  no methods will throw an exception and there is no true 
  iterator support (we return a pointer instead of an iterator).
  
  Limitations:  There are many methods not implemented.
=====================================================================
   $Source: hvector.h $
   $Date: 2011/08/24 13:53:05EDT $
   $Revision: 1.2 $


===================================================================*/
#ifndef _hvector_h_
#define _hvector_h_
#include <stdlib.h>

template <class T> class hvector
{
public:
	hvector(size_t size = 0);
	hvector(const hvector<T> & TheVector);

	~hvector(void)
	{
		delete [] m_pArray;
	}

	T * begin() {return m_pArray;} // returns a T pointer, not an iterator
	const T * begin() const {return m_pArray;} // returns a T pointer, not an iterator
	size_t size(void) const {return m_Size;}
	size_t capacity(void) const {return m_Capacity;}

	T & operator[](size_t index);
	const T & operator[](size_t index) const;
	hvector<T> & operator=(const hvector<T> & rhs);

	bool reserve(size_t NewCapacity);
	bool push_back(const T & item);
	void erase(size_t index);


protected:
	T * m_pArray;
	size_t m_Size;
	size_t m_Capacity;
};



/*----------------------------------------------------------------------------------------
hvector

  Constructor.  Will construct a vector with the optional size.

  ISO Note:  since we can't throw an exception, if we can't allocate the memory, we create 
  an empty vector.
  
  Parameters:  
				size[in]:  size of vector

  Returns:  nothing
----------------------------------------------------------------------------------------*/
template <class T> hvector<T>::hvector(size_t size /*= 0*/)
{
	m_pArray = new T[size];  // ISO C++ says we'll get a pointer if size is 0.  This is handy because we don't have to check for NULL everywhere.
	if ( m_pArray )
	{
		m_Size = size;
		m_Capacity = size;
	}
	else
	{	// we're out of ram.  Gracefully handle this by creating an empty array
		m_Size = 0;
		m_Capacity = 0;
		m_pArray = new T[m_Size];  // if we're so low on memory that THIS fails, we have bigger issues, so don't bother checking
	}
}


/*----------------------------------------------------------------------------------------
hvector

  Copy constructor.  Will construct a vector with the same size and values of the one
  passed.

  Parameters:  
				TheVector[in]:  the vector we will be set to

  Returns:  nothing
----------------------------------------------------------------------------------------*/
template <class T> hvector<T>::hvector(const hvector<T> & TheVector)
{
	m_pArray = new T[TheVector.size()];
	if ( m_pArray )
	{
		m_Size = TheVector.size();
		m_Capacity = m_Size;
		const T * pRhs = TheVector.begin();
		T * pArray = m_pArray;
		for (size_t i = 0; i < m_Size; i++)
		{
			*pArray = *pRhs;
			pArray++;
			pRhs++;
		}
	}
	else
	{	// we're out of ram.  Gracefully handle this by creating an empty array
		m_Size = 0;
		m_Capacity = 0;
		m_pArray = new T[m_Size];  // if we're so low on memory that THIS fails, we have bigger issues, so don't bother checking
	}
}


/*----------------------------------------------------------------------------------------
reserve

  Ensures vector can hold the number of bytes passed in.

  ISO Note:  since we can't throw an exception, if we can't allocate the memory, we leave
  things unchanged and return false.

  Parameters:  
				NewCapacity[in]:  minimum size of new vector

  Returns:  true if OK, false otherwise.
----------------------------------------------------------------------------------------*/
template <class T> bool hvector<T>::reserve(size_t NewCapacity)
{
	if ( NewCapacity > m_Capacity )
	{	// We need to re-allocate.  Keep member variables intact untill we safely allocated space for new array
		T * pNewArray;
		pNewArray = new T[NewCapacity];
		if ( pNewArray == NULL )
		{	// Failed allocation. 
			return false;
		}

		// It's now safe to modify member variables
		T * pOldArrayBytes = m_pArray;
		T * pNewArrayBytes = pNewArray;
		// memcpy(pNewArray, m_pArray, m_Size * sizeof(T)); //this won't work with objects with destructors as they will get called twice (1st time when we delete m_pArray below)  
		for (size_t i = 0; i < m_Size; i++ )
		{
			*pNewArrayBytes = *pOldArrayBytes;
			pNewArrayBytes++;
			pOldArrayBytes++;
		}

		delete [] m_pArray;
		m_pArray = pNewArray;
		m_Capacity = NewCapacity;
	}
	return true;
}



/*----------------------------------------------------------------------------------------
operator[]

  Allows you to set an item at a particular offset.

  ISO Note:  since we can't throw an exception, if the index is out of range, we use the
  the last index.

  Parameters:  
				index[in]:  offset of item in the vector

  Returns:  the item at that index.
----------------------------------------------------------------------------------------*/
template <class T> T & hvector<T>::operator[](size_t index)
{
	if ( index >= m_Size ) index = m_Size - 1;  // size_t is an unsigned variable
	return *(m_pArray + index);
}

/*----------------------------------------------------------------------------------------
operator[]

  Allows you to read an item at a particular offset.

  ISO Note:  since we can't throw an exception, if the index is out of range, we use the
  the last index.

  Parameters:  
				index[in]:  offset of item in the vector

  Returns:  the item at that index.
----------------------------------------------------------------------------------------*/
template <class T> const T & hvector<T>::operator[](size_t index) const
{
	if ( index >= m_Size ) index = m_Size - 1;  // size_t is an unsigned variable
	return *(m_pArray + index);
}


/*----------------------------------------------------------------------------------------
operator=

  Allows you to set one vector to another.

  ISO Note:  since we can't throw an exception, if we can't expand, we don't and we don't
  bother copying the data over.  It's up to the caller to check size() to ensure the
  assignment went OK.

  Parameters:  
				rhs[in]:  the vector we will be set to

  Returns:  this vector.
----------------------------------------------------------------------------------------*/
template <class T> hvector<T> & hvector<T>::operator=(const hvector<T> & rhs)
{
	if ( this == &rhs ) return *this;  // someone is assigning me to me
	if ( rhs.size() > m_Capacity )
	{	// we need more space to hold the larger vector
		T * pNewArray;
		pNewArray = new T[rhs.size()];
		if ( pNewArray == NULL )
		{	// Failed allocation. 
			return *this;
		}
		delete [] m_pArray;
		m_pArray = pNewArray;
		m_Capacity = rhs.size();
	}
	m_Size = rhs.size();
	// now do the copying
	const T * pRhs = rhs.begin();
	T * pArray = m_pArray;
	for (size_t i = 0; i < m_Size; i++)
	{
		*pArray = *pRhs;
		pArray++;
		pRhs++;
	}

	return *this;
}




/*----------------------------------------------------------------------------------------
push_back

  Appends an item at the end of the vector.

  ISO Note:  since we can't throw an exception, if we need to grow the vector and can't,
  we return false.

  Parameters:  
				item[in]:  item to add to the vector

  Returns:  true if OK, false otherwise.
----------------------------------------------------------------------------------------*/
template <class T> bool hvector<T>::push_back(const T & item)
{
	if ( m_Size == m_Capacity )
	{
		size_t extra = m_Capacity/4;
		if ( ! extra ) extra = 4;
		size_t NewCapacity = m_Capacity + extra;  // increase by 25% to keep re-allocations down
		if ( ! reserve(NewCapacity) ) return false;
	}
	*(m_pArray + m_Size) = item;  // m_Size right now is the index of the new last item in the array
	m_Size++;
	return true;
}

/*----------------------------------------------------------------------------------------
erase

  Erases an item.

  ISO Note:  This returns void instead of an iterator.  There is no overload to allow
  removing a range of items.

  Parameters:  
				index[in]:  index of item to remove

  Returns:  nothing.
----------------------------------------------------------------------------------------*/
template <class T> void hvector<T>::erase(size_t index)
{
	if ( index >= m_Size ) return;

	T * pFirstVal = (m_pArray + index);
	T * pNextVal = (m_pArray + index + 1);
	size_t NumToMove = m_Size - index - 1;
	for (size_t i = 0; i < NumToMove; i++)
	{
		*pFirstVal = *pNextVal;
		pFirstVal++;
		pNextVal++;
	}
	m_Size--;
}


/* 
// Test Code:  put in a debug Menu Command and run.
#include "hvector.h"
#define VEC_SIZE 3
#ifdef WIN32
#define PRINT printf
#else
#define PRINT theStdInterface.printf
#endif

class MyClass
{
// There are commented out PRINTs that aid in test & debug.
public:
	MyClass(void)
	{
		printf("MyClass()\n");
		a=1;
		pInt = new int[5];
		RefCount++;
		m_RefCount = RefCount;
		//PRINT(" pInt=%p, m_RefCount = %d\n", pInt, m_RefCount);
	}
	MyClass(const MyClass & rhs)
	{
		PRINT("MyClass copy constructor\n");
		a = rhs.a;
		pInt = new int[5];
		memcpy(pInt,rhs.pInt,5);
		RefCount++;
		m_RefCount = RefCount;
		//PRINT(" pInt=%p, m_RefCount = %d\n", pInt, m_RefCount);
	}
	~MyClass(void)
	{
		PRINT("~MyClass()\n");
		a=0;
		//PRINT(" pInt=%p, m_RefCount = %d\n", pInt, m_RefCount);
		delete [] pInt;
		RefCount--;
	}
	MyClass operator =(MyClass rhs)
	{
		//PRINT("MyClass =\n");
		a = rhs.a;
		m_RefCount = rhs.m_RefCount;
		memcpy(pInt,rhs.pInt,5*sizeof(int));
		return *this;
	}

	void print(void) const
	{
		PRINT("a = %d, m_RefCount = %d, pInt = %p\n", a, m_RefCount, pInt);
	}


	int a;
	int * pInt;
	int m_RefCount;
	static int RefCount;
};
int MyClass::RefCount = 0;
void PrintClasses(hvector<MyClass> * pClasses)
{
	unsigned i;
	PRINT("Printing classes:\n");
	for (i = 0; i < pClasses->size(); i++)
	{
		PRINT("%d) ", i);
		pClasses->operator[](i).print();
	}
}



void TestObjects(void)
{
	PRINT("Creating vector\n");
	unsigned int i;
	hvector<MyClass> ObjectList(VEC_SIZE);
	PRINT("RefCount = %d\n", MyClass::RefCount);
	MyClass val;
	PRINT("RefCount = %d\n", MyClass::RefCount);
	PrintClasses(&ObjectList);
	PRINT("Setting vals\n");
	for (i = 0; i < VEC_SIZE; i++)
	{
		val.a = i*2;
		ObjectList[i] = val;
		PRINT("%d) ", i);
		ObjectList[i].print();
	}
	val.a=100;
	PrintClasses(&ObjectList);
	PRINT("Calling push_back\n");
	ObjectList.push_back(val);
	PRINT("RefCount = %d\n", MyClass::RefCount);
	PrintClasses(&ObjectList);
	PRINT("Calling push_back again\n");
	val.a=101;
	ObjectList.push_back(val);
	PRINT("RefCount = %d\n", MyClass::RefCount);
	PRINT("Calling erase\n");
	ObjectList.erase(2);
	PRINT("Done calling erase\n");
	PRINT("RefCount = %d\n", MyClass::RefCount);
}

void PrintVector(hvector<int> * pVec)
{
	for (size_t i = 0; i < pVec->size(); i++)
	{
		PRINT("%d\n", pVec->operator[](i));
	}
}

void TestHvector(void)
{
	hvector<int> vec(VEC_SIZE);
	int IntArray[VEC_SIZE];
	size_t OriginalCapacity = vec.capacity();
	bool bIsOk;
	// test []
	for (int i = 0; i < VEC_SIZE; i++)
	{
		vec[i] = i;
	}
	int x = vec[0];
	for (int i = 0; i < VEC_SIZE; i++)
	{
		int x = vec[i];
		if ( x != i ) PRINT("Error test []\n");
	}
	
	// test size()
	if ( vec.size() != VEC_SIZE ) PRINT("Error test size()\n");

	// test copy constructor
	hvector<int> InitVec = vec;
	if ( InitVec.size() != vec.size() ) printf("Error (size) test copy constructor\n");
	for (int i = 0; i < VEC_SIZE; i++)
	{
		if ( InitVec[i] != vec[i] ) printf("Error (value) test copy constructor\n");
	}

	// test assignment operator
	InitVec.erase(1);
	InitVec[0] = vec[0] + 1; // make sure they're different
	InitVec = vec;
	if ( InitVec.size() != vec.size() ) printf("Error (size) test test assignment operator A\n");
	for (int i = 0; i < VEC_SIZE; i++)
	{
		if ( InitVec[i] != vec[i] ) printf("Error (value) test test assignment operator A\n");
	}
	// test assignment operator--larger vector
	hvector<int> NewInt(VEC_SIZE - 1);
	NewInt = vec;
	if ( NewInt.size() != vec.size() ) printf("Error (size) test assignment operator B\n");
	for (int i = 0; i < VEC_SIZE; i++)
	{
		if ( NewInt[i] != vec[i] ) printf("Error (value) test assignment operator B\n");
	}

	// test out of range index
	vec[VEC_SIZE] = -2;
	if ( vec[VEC_SIZE - 1] != -2 ) PRINT("Error test out of range index\n");

	// test begin()
	for (int i = 0; i < VEC_SIZE; i++)
	{
		vec[i] = i;
	}
	memcpy(IntArray, vec.begin(), VEC_SIZE*sizeof(int));
	for (int i = 0; i < VEC_SIZE; i++)
	{
		if ( IntArray[i] != i ) PRINT("Error test begin() const\n");
	}
	*vec.begin() = 5;
	if ( vec[0] != 5 ) printf("Error test begin() non const\n");
	vec[0] = 0; // restore it to the orignal value

	// test reserve() A
	bIsOk = vec.reserve(VEC_SIZE);
	if ( ! bIsOk  || (vec.capacity() != OriginalCapacity) ) PRINT("Error test reserve() A\n");

	// test reserve() B
	bIsOk = vec.reserve(VEC_SIZE + 1);
	if ( ! bIsOk  || (vec.capacity() != OriginalCapacity + 1) ) PRINT("Error test reserve() B\n");
	for (int i = 0; i < VEC_SIZE; i++)
	{
		if ( vec[i] != i ) PRINT("Error test reserve() B, part 2\n");
	}

	// test push_back() A
	hvector<int> NewVec(VEC_SIZE);
	for (int i = 0; i < VEC_SIZE; i++)
	{
		NewVec[i] = i;
	}
	OriginalCapacity = NewVec.capacity();
	bIsOk = NewVec.push_back(3);
	if ( ! bIsOk  || (NewVec.capacity() == OriginalCapacity) ) PRINT("Error test push_back() A\n");

	// test push_back() B
	OriginalCapacity = NewVec.capacity();
	bIsOk = NewVec.push_back(4);
	if ( ! bIsOk  || (NewVec.capacity() != OriginalCapacity) ) PRINT("Error test push_back() B\n");
	for (int i = 0; i < VEC_SIZE + 2; i++)
	{
		if ( NewVec[i] != i ) PRINT("Error test push_back() B, part 2 (%d)\n", NewVec[i]);
	}

	// test erase() A
	size_t OrigSize = NewVec.size();
	PRINT("Erasing last index\n");
	PRINT("Orig vector:\n");
	PrintVector(&NewVec);
	NewVec.erase(OrigSize - 1);
	PRINT("\nAfter erase:\n");
	PrintVector(&NewVec);
	if ( NewVec.size()!= OrigSize - 1 ) PRINT("Error test erase() A\n");

	// test erase() B
	PRINT("Erasing first index\n");
	NewVec.erase(0);
	PRINT("\nAfter erase:\n");
	PrintVector(&NewVec);
	for (size_t i = 0; i < NewVec.size(); i++)
	{
		if ( NewVec[i] != i + 1 ) PRINT("Error test erase() B (%d)\n", NewVec[i]);
	}

	// test erase() C
	PRINT("Erasing index 2 (middle)\n");
	NewVec.erase(1);
	PRINT("\nAfter erase:\n");
	PrintVector(&NewVec);

	// Test objects
	PRINT("\n\nTesting objects\n");
	TestObjects();
	if ( MyClass::RefCount != 0 ) PRINT("\n\n\nError RefCount = %d\n", MyClass::RefCount);
}

*/

#endif // endof #ifndef _hvector_h_

