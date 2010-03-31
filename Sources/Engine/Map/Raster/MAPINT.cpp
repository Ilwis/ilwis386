/***************************************************************
 ILWIS integrates image, vector and thematic data in one unique 
 and powerful package on the desktop. ILWIS delivers a wide 
 range of feautures including import/export, digitizing, editing, 
 analysis and display of data as well as production of 
 quality mapsinformation about the sensor mounting platform
 
 Exclusive rights of use by 52°North Initiative for Geospatial 
 Open Source Software GmbH 2007, Germany

 Copyright (C) 2007 by 52°North Initiative for Geospatial
 Open Source Software GmbH

 Author: Jan Hendrikse, Willem Nieuwenhuis,Wim Koolhoven 
 Bas Restsios, Martin Schouwenburg, Lichun Wang, Jelle Wind 

 Contact: Martin Schouwenburg; schouwenburg@itc.nl; 
 tel +31-534874371

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 version 2 as published by the Free Software Foundation.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program (see gnu-gpl v2.txt); if not, write to
 the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 Boston, MA 02111-1307, USA or visit the web page of the Free
 Software Foundation, http://www.fsf.org.

 Created on: 2007-02-8
 ***************************************************************/
/* MapInt
Copyright Ilwis System Development ITC
march 1995, by Wim Koolhoven
Last change:  JEL  23 Jun 97    1:14 pm
*/

#include "Engine\Map\Raster\MAPINT.H"
#include "Engine\Domain\Dmvalue.h"

String MapInt::sType() const
{
	return "Int";
}

StoreType MapInt::st() const
{
	return stINT;
}

// Get/Put one pixel value
long MapInt::iRaw(RowCol rc) const
{
	if (0 == file)
		return iUNDEF;
	short i;
	if (!fInside(rc))
		return iUNDEF;
	if (fPixelInterLeaved)
		file->Seek(iStartOffset + 2 * (rc.Row * iRowLength + rc.Col * iNrBands));
	else
		file->Seek(iStartOffset + 2 * (rc.Row * iRowLength + rc.Col));
	file->Read(2, &i);
	if (fSwapBytes)
		swapbytes(&i);
	if (dm()->pdsrt())
		//  if (i == shUNDEF)
		if (i <= 0)
			return iUNDEF;
		else
			return (long)(unsigned int)i;  
		else
			return longConv(i);
}

void MapInt::PutRaw(RowCol rc, long v)
{
	if (0 == file)
		return;  
	if (!fInside(rc)) return;
	short int i = shortConv(v);
	if (fPixelInterLeaved)
		file->Seek(iStartOffset + 2 * (rc.Row * iRowLength + rc.Col * iNrBands));
	else
		file->Seek(iStartOffset + 2 * (rc.Row * iRowLength + rc.Col));
	if (fSwapBytes)
		swapbytes(&i);
	file->Write(2, &i);
	fChanged = true;
}

// Get/Put a (part of a) line

void MapInt::GetLineRaw(long l, ByteBuf& b, long iColFrom, long iColNum, int iPyrLayer) const
{
	long iNum=(iColNum==0 ? iCols() : iColNum);
	assert(iNum <= b.iSize());
	IntBuf ib(iNum);
	GetLineRaw(l, ib, iColFrom, iNum, iPyrLayer);
	for (long i = 0; i < iNum; i++)
		b[i] = byteConv(ib[i]);
}

void MapInt::GetLineRaw(long l, IntBuf& b, long iColFrom, long iColNum, int iPyrLayer) const
{
	if (0 == file)
		return;  
	if (l < 0 || l >= iLines()) {
		for (long i=0; i < b.iSize(); ++i )
			b[i] = shUNDEF;
		return;
	}
	if ( iPyrLayer > iPyramidLayerOffset.size())
		iPyrLayer = 0;	
	File *fileData = iPyrLayer == 0 ? file : filePyramid;
	// byte swapping is not needed for pyramid files, they are always stored INTEL wise (low-endian)
	bool fSwap = fSwapBytes && fileData != filePyramid;
	int iDiv = (int)pow(2, (double)iPyrLayer);
	
	long iNum = iColNum == 0 ? iCols() / iDiv : iColNum;	
	long iRightCols = max(0, iNum+iColFrom-iCols() / iDiv); //columns to right of map
	long iLeftCols = -min(iColFrom, 0); // columns to left of map
	
	int iFOffSet;
	iFOffSet = iPyrLayer == 0 ? iStartOffset : iPyramidLayerOffset[iPyrLayer - 1] ;	// 0 based index
	
	for (long i=0; i < iLeftCols ; ++i)
		b[i] = shUNDEF;
	short HUGEBUFPTR* p = b.buf()+iLeftCols;
	if (fPixelInterLeaved) {
		long iOff = iFOffSet + 2 * (l * iRowLength + (iColFrom+iLeftCols) * iNrBands);
		for (long c = 0; c < iColNum; c++, p++){
			fileData->Seek(iOff);  
			fileData->Read(2, p);
			if (fSwap)
				swapbytes(p);
			iOff += 2 * iNrBands;
		}
	}
	else {
		fileData->Seek(iFOffSet + 2 * (l * int(iRowLength/iDiv) + iColFrom+iLeftCols));
		fileData->Read(2 * (iNum-iLeftCols-iRightCols), p);
		if (fSwap)
			for (long c = 0; c < iNum-iLeftCols-iRightCols; c++, p++)
				swapbytes(p);
	}
	for (long i=iNum-iRightCols; i < iNum ; ++i)
		b[i] = shUNDEF;
}

void MapInt::GetLineRaw(long l, LongBuf& b, long iColFrom, long iColNum, int iPyrLayer) const
{
	long iNum=(iColNum==0 ? iCols() : iColNum);
	assert(iNum <= b.iSize());
	IntBuf ib(iNum);
	GetLineRaw(l, ib, iColFrom, iNum, iPyrLayer);
	if (dm()->pdsrt())
		for (long i = 0; i < iNum; i++) {
			short iTmp = ib[i];
			//    if (iTmp == shUNDEF)
			if (iTmp <= 0)
				b[i] = iUNDEF;
			else
				b[i] = (long)(unsigned int)iTmp;  
		}  
		else
			for (long i = 0; i < iNum; i++)
				b[i] = longConv(ib[i]);
}

void MapInt::GetLineVal(long l, LongBuf& b, long iColFrom, long iColNum, int iPyrLayer) const
{
	long iNum=(iColNum==0 ? iCols() : iColNum);
	assert(iNum <= b.iSize());
	if (!dvs.fValues()) {
		for (long i = 0; i < iNum; i++)
			b[i] = iUNDEF;
		return;
	}
	IntBuf ib(iNum);
	GetLineRaw(l, ib, iColFrom, iNum, iPyrLayer);
	for (long i = 0; i < iNum; i++)
		b[i] = vr()->iValue(ib[i]);
}

void MapInt::GetLineVal(long l, RealBuf& b, long iColFrom, long iColNum, int iPyrLayer) const
{
	long iNum=(iColNum==0 ? iCols() : iColNum);
	assert(iNum <= b.iSize());
	if (!dvs.fValues()) {
		for (long i = 0; i < iNum; i++)
			b[i] = rUNDEF;
		return;
	}
	IntBuf ib(iNum);
	GetLineRaw(l, ib, iColFrom, iNum, iPyrLayer);
	for (long i = 0; i < iNum; i++)
		b[i] = vr()->rValue(ib[i]);
}

void MapInt::PutLineRaw(long l, const ByteBuf& b, long iColFrom, long iColNum)
{
	long iNum=(iColNum==0 ? iCols() : iColNum);
	assert(iNum <= b.iSize());
	IntBuf ib(iNum);
	for (long i = 0; i < iNum; i++)
		ib[i] = b[i];
	PutLineRaw(l, ib, iColFrom, iNum);
}

void MapInt::PutLineRaw(long l, const IntBuf& b, long iColFrom, long iColNum)
{
	if (0 == file)
		return;  
	long iNum=(iColNum==0 ? iCols() : iColNum);
	if (iNum+iColFrom > iCols()) iNum=iCols()-iColFrom; // clip at end of line
	assert(l >= 0 && l < iLines());
	assert(iNum <= b.iSize());
	short HUGEBUFPTR* p = b.buf();
	if (fPixelInterLeaved) {
		long iOff = iStartOffset + 2 * (l * iRowLength + iColFrom * iNrBands);
		for (long c = 0; c < iColNum; c++, p++){
			file->Seek(iOff);  
			if (fSwapBytes)
				swapbytes(p);
			file->Write(2, p);
			iOff += 2 * iNrBands;
		}
	}
	else {
		if (fSwapBytes)
			for (long c = 0; c < iNum; c++, p++)
				swapbytes(p);
			file->Seek(iStartOffset + 2 * (l * iRowLength + iColFrom));
			file->Write(2 * iNum, b.buf());
	}
	fChanged = true;
}

void MapInt::PutLineRaw(long l, const LongBuf& b, long iColFrom, long iColNum)
{
	long iNum=(iColNum==0 ? iCols() : iColNum);
	assert(iNum <= b.iSize());
	IntBuf ib(iNum);
	if (dm()->pdsrt())
		for (long i = 0; i < iNum; i++) {
			long iTmp = b[i];
			//    if (iTmp == iUNDEF)
			if (iTmp <= 0)
				ib[i] = shUNDEF;
			else
				ib[i] = (short)(unsigned int)iTmp;  
		}  
		else
			for (long i = 0; i < iNum; i++)
				ib[i] = shortConv(b[i]);
			PutLineRaw(l, ib, iColFrom, iNum);
}

void MapInt::PutLineVal(long l, const LongBuf& b, long iColFrom, long iColNum)
{
	if (!dvs.fValues())
		return;
	long iNum=(iColNum==0 ? iCols() : iColNum);
	assert(iNum <= b.iSize());
	IntBuf ib(iNum);
	for (long i = 0; i < iNum; i++)
		ib[i] = shortConv(vr()->iRaw(b[i]));
	PutLineRaw(l, ib, iColFrom, iNum);
}

void MapInt::PutLineVal(long l, const RealBuf& b, long iColFrom, long iColNum)
{
	if (!dvs.fValues())
		return;
	long iNum=(iColNum==0 ? iCols() : iColNum);
	assert(iNum <= b.iSize());
	IntBuf ib(iNum);
	for (long i = 0; i < iNum; i++)
		ib[i] = shortConv(vr()->iRaw(b[i]));
	PutLineRaw(l, ib, iColFrom, iNum);
}

void MapInt::IterateCreatePyramidLayer(int iPyrLayer, long &iLastFilePos, Tranquilizer *trq)
{
	int iDiv = (int)pow(2, (double)iPyrLayer);	
	int iColsPyrLayer = iCols() / iDiv;
	int iLinePrevPyrLayer = iLines() / iDiv;
	IntBuf buf(iColsPyrLayer), bufPrev(iColsPyrLayer);
	IntBuf bufPyr(iColsPyrLayer / 2);
	
	for(int iLine = 0, iPyrLine = 0; iPyrLine < iLinePrevPyrLayer / 2; iLine += 2, ++iPyrLine)
	{
		if ( trq )
		{
			if ( trq->fUpdate(iPyrLine, iLinePrevPyrLayer) )
			{
				filePyramid->SetErase()	;
				delete filePyramid;
				filePyramid = 0;
				return;
			}
		}		
		int iPyrCol = 0;		
		GetLineRaw(iLine, buf, 0, iColsPyrLayer, iPyrLayer);			
		GetLineRaw(iLine + 1, bufPrev, 0, iColsPyrLayer, iPyrLayer);
		for(int iCol = 0; iCol < iColsPyrLayer - 2; iCol += 2)
		{
			int iPred = bufPrev[iCol]; //rPredominant(bufPrev[iCol], bufPrev[iCol+1], buf[iCol], buf[iCol + 1]);
			bufPyr[iCol / 2] = iPred;
		}	
		filePyramid->Seek(iLastFilePos);
		iLastFilePos += filePyramid->Write(bufPyr.iSize() * 2, (void *)bufPyr.buf());
	}			
}


