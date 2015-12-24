//======================================================================================
// TIFF_Fax.h
//======================================================================================
// $RCSfile: TIFF_Fax.h $
// $Revision: 1.1 $
// $Date: 2009/04/03 15:08:46EDT $
//======================================================================================

//======================================================================================
//
//	This file is part of a TIFF compression / decompression package that Tim Meier picked
//	up from .  This package was ported from the CxImage library to be a standalone
//	compression / decompression library for TIFF images.
//
//======================================================================================



#ifndef FAX_H
#define FAX_H

void Fax3PreDecode(CSB *csb, uint16 td_fillorder);
int Fax4Encode(CSB *csb, uchar *bp, tsize_t cc); //, tsample_t s);
int Fax4Decode(CSB *csb, uchar *buf, tsize_t occ); //, tsample_t s);

#endif



