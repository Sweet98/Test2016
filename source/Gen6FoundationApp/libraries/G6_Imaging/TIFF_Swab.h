//======================================================================================
// TIFF_Swab.h
//======================================================================================
// $RCSfile: TIFF_Swab.h $
// $Revision: 1.1 $
// $Date: 2009/04/03 15:08:47EDT $
//======================================================================================

//======================================================================================
//
//	This file is part of a TIFF compression / decompression package that Tim Meier picked
//	up from .  This package was ported from the CxImage library to be a standalone
//	compression / decompression library for TIFF images.
//
//======================================================================================



#ifndef SWAB_H
#define SWAB_H

#include "TIFF_CSB.h"


void TIFFSwabShort(uint16 *wp);
void TIFFSwabLong(uint32 *lp);
void TIFFSwabArrayOfShort(uint16 *wp, register uint32 n);
void TIFFSwabArrayOfLong(register uint32 *lp, register uint32 n);
void TIFFSwabDouble(double *dp);
void TIFFSwabArrayOfDouble(double* dp, register uint32 n);
const unsigned char *TIFFGetBitRevTable(int reversed);
void TIFFReverseBits(register uchar* cp, register uint32 n);

#endif



