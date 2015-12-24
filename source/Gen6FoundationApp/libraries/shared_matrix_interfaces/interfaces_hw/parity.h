/*=================================================================================
  class: CParity

//=================================================================================
   $Source: interfaces_hw/parity.h $
   $Date: 2002/10/01 16:13:19EDT $
   $Revision: 1.2 $


//=================================================================================*/

#ifndef PARITY_H__INCLUDED_
#define PARITY_H__INCLUDED_

class CParity
{
public:

char ParityTable[256];

#define EVEN	0
#define ODD		1
void BuildParityTable(int Parity)
{
	int c,j,k;
	char p;
	for(c=0; c<256; c++)
	{
		p = 0x01;
		k = c;
		for(j=0; j<8; j++)
		{
			p = p^(k & 0x01);
			k=k>>1;
		}
		if (ODD == Parity)
			ParityTable[c] = p;
		else
			ParityTable[c] = p^1;
	}
}

};

#endif // PARITY_H__INCLUDED_




