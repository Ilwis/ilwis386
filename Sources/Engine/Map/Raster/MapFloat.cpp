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
/* MapFloat
Copyright Ilwis System Development ITC
march 1995, by Wim Koolhoven
Last change:  JEL  25 Jun 97    3:48 pm
*/

#include "Engine\Map\Raster\MapFloat.h"
#include "Engine\Domain\Dmvalue.h"

String MapFloat::sType() const
{
	return "Float";
}

StoreType MapFloat::st() const
{
	return stFLOAT;
}

// Get/Put one pixel value
double MapFloat::rValue(RowCol rc) const
{
	if (0 == file)
		return rUNDEF;
	float fl;
	if (!fInside(rc))
		return rUNDEF;
	if (fPixelInterLeaved)
		file->Seek(iStartOffset + 4 * ((ULONGLONG)rc.Row * iRowLength + rc.Col * iNrBands));
	else
		file->Seek(iStartOffset + 4 * ((ULONGLONG)rc.Row * iRowLength + rc.Col));
	file->Read(4, &fl);
	if (fSwapBytes)
		swapbytes((long*)(void*)&fl);
	return doubleConv(fl);
}

void MapFloat::PutVal(RowCol rc, double val)
{
	if (0 == file)
		return;
	if (!fInside(rc))
		return;
	if (fPixelInterLeaved)
		file->Seek(iStartOffset + 4 * ((ULONGLONG)rc.Row * iRowLength + rc.Col * iNrBands));
	else
		file->Seek(iStartOffset + 4 * ((ULONGLONG)rc.Row * iRowLength + rc.Col));
	float fl = floatConv(val);
	if (fSwapBytes)
		swapbytes((long*)(void*)&fl);
	file->Write(4, &fl);
	fChanged = true;
}

void MapFloat::PutRaw(RowCol rc, long v)
{
	double val = doubleConv(v);
	PutVal(rc, val);
}

void MapFloat::PutVal(RowCol rc, const String& s)
{
	if (dvs.fUseReals()) {
		double val = dvs.rValue(s);
		PutVal(rc, val);
	}
}

// Get/Put a (part of a) line

void MapFloat::GetLineRaw(long, ByteBuf& b, long, long iColNum, int iPyrLayer) const
{
	long iNum=(iColNum==0 ? iCols() : iColNum);
	assert(iNum <= b.iSize());
	for (long i = 0; i < iNum; i++)
		b[i] = 0;
}

void MapFloat::GetLineRaw(long, IntBuf& b, long, long iColNum, int iPyrLayer) const
{
	long iNum=(iColNum==0 ? iCols() : iColNum);
	assert(iNum <= b.iSize());
	for (long i = 0; i < iNum; i++)
		b[i] = shUNDEF;
}

void MapFloat::GetLineRaw(long, LongBuf& b, long, long iColNum, int iPyrLayer) const
{
	long iNum=(iColNum==0 ? iCols() : iColNum);
	assert(iNum <= b.iSize());
	for (long i = 0; i < iNum; i++)
		b[i] = iUNDEF;
}

void MapFloat::GetLineVal(long l, LongBuf& b, long iColFrom, long iColNum, int iPyrLayer) const
{
	long iNum=(iColNum==0 ? iCols() : iColNum);
	assert(iNum <= b.iSize());
	RealBuf rb(iNum);
	GetLineVal(l, rb, iColFrom, iColNum, iPyrLayer);
	for (long i = 0; i < iNum; i++)
		b[i] = longConv(rb[i]);
}

void MapFloat::GetLineVal(long l, RealBuf& b, long iColFrom, long iColNum, int iPyrLayer) const
{
	if (0 == file)
		return;
	if (l < 0 || l >= iLines()) {
		for (long i=0; i < b.iSize(); ++i )
			b[i] = rUNDEF;
		return;
	}
	File *fileData = iPyrLayer == 0 ? file : filePyramid;
	// byte swapping is not needed for pyramid files, they are always stored INTEL wise (low-endian)
	bool fSwap = fSwapBytes && fileData != filePyramid;
	int iDiv = (int)pow(2, (double)iPyrLayer);
	
	long iNum = iColNum==0 ? iCols() / iDiv : iColNum;
	long iRightCols = max(0, iNum+iColFrom-iCols() / iDiv); //columns to right of map
	long iLeftCols = -min(iColFrom, 0); // columns to left of map
	
	ULONGLONG iFOffSet = iPyrLayer == 0 ? iStartOffset : iPyramidLayerOffset[iPyrLayer - 1] ;	// 0 based index
	
	for (long i=0; i < iLeftCols ; ++i)
		b[i] = rUNDEF;
	double HUGEBUFPTR* p = b.buf()+iLeftCols;
	float fl;
	if (fPixelInterLeaved) {
		ULONGLONG iOff = iFOffSet + 4 * ((ULONGLONG)l * iRowLength + (iColFrom+iLeftCols) * iNrBands);
		for (long c = 0; c < iColNum; c++, p++){
			fileData->Seek(iOff);  
			fileData->Read(4, &fl);
			if (fSwap)
				swapbytes((long*)(void*)&fl);
			*p = doubleConv(fl); 
			iOff += 4 * iNrBands;
		}
	}
	else {
		FloatBuf bufFl(iNum-iLeftCols-iRightCols);
		float HUGEBUFPTR* pfl = bufFl.buf();
		fileData->Seek(iFOffSet + 4 * ((ULONGLONG)l * (ULONGLONG)(iRowLength / iDiv) + iColFrom+iLeftCols));
		fileData->Read(4 * (iNum-iLeftCols-iRightCols), pfl);
		for (long i=0; i < bufFl.iSize(); i++, pfl++, p++) {
			if (fSwap)
				swapbytes((long*)(void*)pfl);
			*p = doubleConv(*pfl);
		}
	}
	for (long i=iNum-iRightCols; i < iNum ; ++i)
		b[i] = rUNDEF;
}

void MapFloat::PutLineRaw(long, const ByteBuf&, long, long)
{
}

void MapFloat::PutLineRaw(long, const IntBuf&, long, long)
{
}

void MapFloat::PutLineRaw(long, const LongBuf&, long, long)
{
}

void MapFloat::PutLineVal(long l, const LongBuf& b, long iColFrom, long iColNum)
{
	long iNum=(iColNum==0 ? iCols() : iColNum);
	assert(iNum <= b.iSize());
	RealBuf rb(iNum);
	for (long i = 0; i < iNum; i++)
		rb[i] = doubleConv(b[i]);
	PutLineVal(l, rb, iColFrom, iColNum);
}

void MapFloat::PutLineVal(long l, const RealBuf& b, long iColFrom, long iColNum)
{
	if (0 == file)
		return;
	long iNum=(iColNum==0 ? iCols() : iColNum);
	if (iNum+iColFrom > iCols()) 
		iNum=iCols()-iColFrom; // clip at end of line
	assert(l >= 0 && l < iLines());
	assert(iNum <= b.iSize());
	RealBuf rb(iNum);
	RangeReal rr = vr()->rrMinMax();
	for (long i = 0; i < iNum; i++)
		rb[i] = rr.fContains(b[i]) ? b[i] : rUNDEF;
	double HUGEBUFPTR* p = b.buf();
	if (fPixelInterLeaved) {
		double HUGEBUFPTR* p = b.buf();
		ULONGLONG iOff = iStartOffset + 4 * ((ULONGLONG)l * iRowLength + iColFrom * iNrBands);
		for (long c = 0; c < iColNum; c++, p++){
			file->Seek(iOff);  
			float fl = floatConv(*p);
			if (fSwapBytes)
				swapbytes((long*)(void*)&fl);
			file->Write(4, &fl);
			iOff += 4 * iNrBands;
		}
	}
	else {
		FloatBuf bufFl(iNum);
		float HUGEBUFPTR* pfl = bufFl.buf();
		for (long i=0; i < bufFl.iSize(); i++, pfl++, p++) {
			*pfl = floatConv(*p);
			if (fSwapBytes)
				swapbytes((long*)(void*)pfl);
		} 
		file->Seek(iStartOffset + 4 * ((ULONGLONG)l * iRowLength + iColFrom));
		file->Write(4 * iNum, bufFl.buf());
	}
	fChanged = true;
}

void MapFloat::IterateCreatePyramidLayer(int iPyrLayer, ULONGLONG &iLastFilePos, Tranquilizer *trq)
{
	int iDiv = (int)pow(2, (double)iPyrLayer);	
	int iColsPyrLayer = iCols() / iDiv;
	int iLinePrevPyrLayer = iLines() / iDiv;
	RealBuf buf(iColsPyrLayer), bufPrev(iColsPyrLayer);
	FloatBuf bufPyr(iColsPyrLayer / 2);
	
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
		GetLineVal(iLine, buf, 0, iColsPyrLayer, iPyrLayer);			
		GetLineVal(iLine + 1, bufPrev, 0, iColsPyrLayer, iPyrLayer);
		for(int iCol = 0; iCol < iColsPyrLayer - 2; iCol += 2)
		{
			float rPred = bufPrev[iCol]; //rPredominant(bufPrev[iCol], bufPrev[iCol+1], buf[iCol], buf[iCol + 1]);
			bufPyr[iCol / 2] = rPred;
		}	
		filePyramid->Seek(iLastFilePos);
		iLastFilePos += filePyramid->Write(bufPyr.iSize() * 4, (void *)bufPyr.buf());
	}			
}
