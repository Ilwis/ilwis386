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
/* MapLong
Copyright Ilwis System Development ITC
march 1995, by Wim Koolhoven
Last change:  JEL  23 Jun 97    1:18 pm
*/

#include "Engine\Map\Raster\MAPLONG.H"
#include "Engine\Domain\Dmvalue.h"

String MapLong::sType() const
{
	return "Long";
}

StoreType MapLong::st() const
{
	return stLONG;
}

// Get/Put one pixel value
long MapLong::iRaw(RowCol rc) const
{
	if (0 == file)
		return iUNDEF;
	long i;
	if (!fInside(rc))
		return iUNDEF;
	if (fPixelInterLeaved)
		file->Seek(iStartOffset + 4 * ((ULONGLONG)rc.Row * iRowLength + rc.Col * iNrBands));
	else
		file->Seek(iStartOffset + 4 * ((ULONGLONG)rc.Row * iRowLength + rc.Col));
	file->Read(4, &i);
	if (fSwapBytes)
		swapbytes(&i);
	return i;
}

void MapLong::PutRaw(RowCol rc, long v)
{
	if (0 == file)
		return;
	if (!fInside(rc)) return;
	if (fPixelInterLeaved)
		file->Seek(iStartOffset + 4* ((ULONGLONG)rc.Row * iRowLength + rc.Col * iNrBands));
	else
		file->Seek(iStartOffset + 4* ((ULONGLONG)rc.Row * iRowLength + rc.Col));
	if (fSwapBytes)
		swapbytes(&v);
	file->Write(4, &v);
	fChanged = true;
}

// Get/Put a (part of a) line

void MapLong::GetLineRaw(long l, ByteBuf& b, long iColFrom, long iColNum, int iPyrLayer) const
{
	long iNum=(iColNum==0 ? iCols() : iColNum);
	assert(iNum <= b.iSize());
	LongBuf ib(iNum);
	GetLineRaw(l, ib, iColFrom, iNum, iPyrLayer);
	for (long i = 0; i < iNum; i++)
		b[i] = byteConv(ib[i]);
}

void MapLong::GetLineRaw(long l, IntBuf& b, long iColFrom, long iColNum, int iPyrLayer) const
{
	long iNum=(iColNum==0 ? iCols() : iColNum);
	assert(iNum <= b.iSize());
	LongBuf ib(iNum);
	GetLineRaw(l, ib, iColFrom, iNum, iPyrLayer);
	for (long i = 0; i < iNum; i++)
		b[i] = shortConv(ib[i]);
}

void MapLong::GetLineRaw(long l, LongBuf& b, long iColFrom, long iColNum, int iPyrLayer) const
{
	if (0 == file)
		return;
	if (l < 0 || l >= iLines()) {
		for (long i=0; i < b.iSize(); ++i )
			b[i] = iUNDEF;
		return;
	}
	if ( iPyrLayer > iPyramidLayerOffset.size())
		iPyrLayer = 0;	
	
	File *fileData = iPyrLayer == 0 ? file : filePyramid;
	// byte swapping is not needed for pyramid files, they are always stored INTEL wise (low-endian)
	bool fSwap = fSwapBytes && fileData != filePyramid;
	int iDiv = (int)pow(2, (double)iPyrLayer);
	
	long iNum = iColNum==0 ? iCols() / iDiv : iColNum;
	long iRightCols = max(0, iNum+iColFrom-iCols() / iDiv); //columns to right of map
	long iLeftCols = -min(iColFrom, 0); // columns to left of map
	
	ULONGLONG iFOffSet = iPyrLayer == 0 ? iStartOffset : iPyramidLayerOffset[iPyrLayer - 1] ;	// 0 based index
	
	for (long i=0; i < iLeftCols ; ++i)
		b[i] = iUNDEF;
	long HUGEBUFPTR* p = b.buf()+iLeftCols;
	if (fPixelInterLeaved) {
		ULONGLONG iOff = iFOffSet + 4 * ((ULONGLONG)l * (ULONGLONG)(iRowLength/ iDiv) + (iColFrom+iLeftCols) * iNrBands);
		for (long c = 0; c < iColNum; c++, p++){
			fileData->Seek(iOff);  
			fileData->Read(4, p);
			if (fSwap)
				swapbytes(p);
			iOff += 4 * iNrBands;
		}
	}
	else {
		iFOffSet += 4 * ((ULONGLONG)l * (ULONGLONG)(iRowLength/ iDiv) + iColFrom+iLeftCols);		
		fileData->Seek(iFOffSet);
		fileData->Read(4 * (iNum-iLeftCols-iRightCols), p);
		if (fSwap)
			for (long c = 0; c < iNum-iLeftCols-iRightCols; c++, p++)
				swapbytes(p);
	}
	for (long i=iNum-iRightCols; i < iNum ; ++i)
		b[i] = iUNDEF;
	
}

void MapLong::GetLineVal(long l, LongBuf& b, long iColFrom, long iColNum, int iPyrLayer) const
{
	long iNum=(iColNum==0 ? iCols() : iColNum);
	assert(iNum <= b.iSize());
	if (!dvs.fValues()) {
		for (long i = 0; i < iNum; i++)
			b[i] = iUNDEF;
		return;
	}
	LongBuf ib(iNum);
	GetLineRaw(l, ib, iColFrom, iNum, iPyrLayer);
	for (long i = 0; i < iNum; i++)
		b[i] = vr()->iValue(ib[i]);
}

void MapLong::GetLineVal(long l, RealBuf& b, long iColFrom, long iColNum, int iPyrLayer) const
{
	long iNum=(iColNum==0 ? iCols() : iColNum);
	assert(iNum <= b.iSize());
	if (!dvs.fValues()) {
		for (long i = 0; i < iNum; i++)
			b[i] = rUNDEF;
		return;
	}
	LongBuf ib(iNum);
	GetLineRaw(l, ib, iColFrom, iNum, iPyrLayer);
	for (long i = 0; i < iNum; i++)
		b[i] = vr()->rValue(ib[i]);
}

void MapLong::PutLineRaw(long l, const ByteBuf& b, long iColFrom, long iColNum)
{
	long iNum=(iColNum==0 ? iCols() : iColNum);
	assert(iNum <= b.iSize());
	LongBuf ib(iNum);
	for (long i = 0; i < iNum; i++)
		ib[i] = b[i];
	PutLineRaw(l, ib, iColFrom, iNum);
}

void MapLong::PutLineRaw(long l, const IntBuf& b, long iColFrom, long iColNum)
{
	long iNum=(iColNum==0 ? iCols() : iColNum);
	assert(iNum <= b.iSize());
	LongBuf ib(iNum);
	for (long i = 0; i < iNum; i++)
		ib[i] = longConv(b[i]);
	PutLineRaw(l, ib, iColFrom, iNum);
}

void MapLong::PutLineRaw(long l, const LongBuf& b, long iColFrom, long iColNum)
{
	if (0 == file)
		return;
	long iNum=(iColNum==0 ? iCols() : iColNum);
	if (iNum+iColFrom > iCols()) iNum=iCols()-iColFrom; // clip at end of line
	assert(l >= 0 && l < iLines());
	assert(iNum <= b.iSize());
	long HUGEBUFPTR* p = b.buf();
	if (fPixelInterLeaved) {
		ULONGLONG iOff = iStartOffset + 4 * ((ULONGLONG)l * iRowLength + iColFrom * iNrBands);
		for (long c = 0; c < iColNum; c++, p++){
			file->Seek(iOff);  
			if (fSwapBytes)
				swapbytes(p);
			file->Write(4, p);
			iOff += 4 * iNrBands;
		}
	}
	else {
		if (fSwapBytes)
			for (long c = 0; c < iRowLength + iColFrom; c++, p++)
				swapbytes(p);
			file->Seek(iStartOffset + 4 * ((ULONGLONG)l * iRowLength + iColFrom));
			file->Write(4 * iNum, b.buf());
	}
	fChanged = true;
}

void MapLong::PutLineVal(long l, const LongBuf& b, long iColFrom, long iColNum)
{
	if (!dvs.fValues())
		return;
	long iNum=(iColNum==0 ? iCols() : iColNum);
	assert(iNum <= b.iSize());
	LongBuf ib(iNum);
	for (long i = 0; i < iNum; i++)
		ib[i] = vr()->iRaw(b[i]);
	PutLineRaw(l, ib, iColFrom, iNum);
}

void MapLong::PutLineVal(long l, const RealBuf& b, long iColFrom, long iColNum)
{
	if (!dvs.fValues())
		return;
	long iNum=(iColNum==0 ? iCols() : iColNum);
	assert(iNum <= b.iSize());
	LongBuf ib(iNum);
	for (long i = 0; i < iNum; i++)
		ib[i] = vr()->iRaw(b[i]);
	PutLineRaw(l, ib, iColFrom, iNum);
}

void MapLong::IterateCreatePyramidLayer(int iPyrLayer, ULONGLONG &iLastFilePos, Tranquilizer *trq)
{
	int iDiv = (int)pow(2, (double)iPyrLayer);	
	int iColsPyrLayer = iCols() / iDiv;
	int iLinePrevPyrLayer = iLines() / iDiv;
	LongBuf buf(iColsPyrLayer);
	LongBuf bufPyr(iColsPyrLayer / 2);
	
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
		for(int iCol = 0; iCol < iColsPyrLayer - 2; iCol += 2)
		{
			long iPred = buf[iCol]; //rPredominant(bufPrev[iCol], bufPrev[iCol+1], buf[iCol], buf[iCol + 1]);
			bufPyr[iCol / 2] = iPred;
		}	
		filePyramid->Seek(iLastFilePos);
		iLastFilePos += filePyramid->Write(bufPyr.iSize() * 4, (void *)bufPyr.buf());
	}			
}
