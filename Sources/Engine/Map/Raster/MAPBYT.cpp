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
/* MapByte
   Copyright Ilwis System Development ITC
   march 1995, by Wim Koolhoven
	Last change:  JEL   8 Jul 97    1:40 pm
*/

#include "Engine\Map\Raster\MAPBYT.H"
#include "Engine\Domain\Dmvalue.h"
#include "Headers\Hs\map.hs"


String MapByte::sType() const
{
	return "Byte";
}

StoreType MapByte::st() const
{
	return stBYTE;
}

// Get/Put one pixel value
long MapByte::iRaw(RowCol rc) const
{
  byte b;
  if (!fInside(rc))
    return iUNDEF;
  if (0 == file)
    return iUNDEF;
  if (fPixelInterLeaved)
    file->Seek(iStartOffset + rc.Row * iRowLength + rc.Col * iNrBands);
  else
    file->Seek(iStartOffset + rc.Row * iRowLength + rc.Col);
  file->Read(1, &b);
  if (b == 0)
    if (dvs.iRawUndef() == 0)
      return iUNDEF;
  return b;
}

void MapByte::PutRaw(RowCol rc, long v)
{
  if (0 == file)
    return;
  if (!fInside(rc)) return;
  byte b = byteConv(v);
  if (fPixelInterLeaved)
    file->Seek(iStartOffset + rc.Row * iRowLength + rc.Col * iNrBands);
  else
    file->Seek(iStartOffset + rc.Row * iRowLength + rc.Col);
  file->Write(1, &b);
  fChanged = true;
}

// Get/Put a (part of a) line

void MapByte::GetLineRaw(long l, ByteBuf& b, long iColFrom, long iColNum, int iPyrLayer) const
{
  if (0 == file)
    return;
  if (l < 0 || l >= iLines()) {
    for (long i=0; i < b.iSize(); ++i )
      b[i] = 0;
    return;
  }
	// guard against odd behaviour, should not happen but better safe the sorry.
	if ( iPyrLayer > iPyramidLayerOffset.size())
		iPyrLayer = 0;
	File *fileData = iPyrLayer == 0 ? file : filePyramid;
	int iDiv = (int)pow(2, (double)iPyrLayer);
	
  long iNum = iColNum == 0 ? iCols() / iDiv : iColNum;
  long iRightCols = max(0, iNum+iColFrom-iCols() / iDiv); //columns to right of map
  long iLeftCols = -min(iColFrom, 0); // columns to left of map
		
	int iFOffSet;
	iFOffSet = iPyrLayer == 0 ? iStartOffset : iPyramidLayerOffset[iPyrLayer - 1] ;	// 0 based index
  for (long i=0; i < iLeftCols ; ++i)
    b[i] = 0;
  byte HUGEBUFPTR* p = b.buf()+iLeftCols;
  if (fPixelInterLeaved) 
	{
    long iOff = iFOffSet + l * int(iRowLength/iDiv) + (iColFrom+iLeftCols) * iNrBands;
    for (long c = 0; c < iColNum; c++, p++){
      fileData->Seek(iOff);  
      fileData->Read(1, p);
      iOff += iNrBands;
    }
  }
  else 
	{
    long iOff = iFOffSet + l * int(iRowLength/iDiv) + (iColFrom+iLeftCols);		
    fileData->Seek(iOff);
    fileData->Read(iNum-iLeftCols-iRightCols, p);
  }
  for (long i=iNum-iRightCols; i < iNum ; ++i)
    b[i] = 0;
}

void MapByte::GetLineRaw(long l, IntBuf&  b, long iColFrom, long iColNum, int iPyrLayer) const
{
	int iDiv = (int)pow(2, (double)iPyrLayer);	
  long iNum = iColNum==0 ? iCols() / iDiv : iColNum;
  assert(iNum <= b.iSize());
  ByteBuf bb(iNum);
  GetLineRaw(l, bb, iColFrom, iNum, iPyrLayer);
  if (dvs.iRawUndef() != 0)
    for (long i = 0; i < iNum; i++)
      b[i] = bb[i];
  else {
    short raw;
    for (long i = 0; i < iNum; i++) 
		{
      raw = bb[i];
      if (raw == 0)
        b[i] = shUNDEF;
      else
        b[i] = raw;
    }
  }
}

void MapByte::GetLineRaw(long l, LongBuf& b, long iColFrom, long iColNum, int iPyrLayer) const
{
	int iDiv = (int)pow(2, (double)iPyrLayer);	
  long iNum = iColNum==0 ? iCols() / iDiv: iColNum;
  assert(iNum <= b.iSize());
  ByteBuf bb(iNum);
  GetLineRaw(l, bb, iColFrom, iNum, iPyrLayer);
  if (dvs.iRawUndef())
    for (long i = 0; i < iNum; i++)
      b[i] = bb[i];
  else {
    byte raw;
    for (long i = 0; i < iNum; i++) {
      raw = bb[i];
      if (raw == 0)
        b[i] = iUNDEF;
      else
        b[i] = raw;
    }
  }
}

void MapByte::GetLineVal(long l, LongBuf& b, long iColFrom, long iColNum, int iPyrLayer) const
{
	int iDiv = (int)pow(2, (double)iPyrLayer);		
  long iNum = iColNum==0 ? iCols() / iDiv : iColNum;
  assert(iNum <= b.iSize());
  if (!dvs.fValues()) {
    for (long i = 0; i < iNum; i++)
      b[i] = iUNDEF;
    return;
  }
  ByteBuf bb(iNum);
  GetLineRaw(l, bb, iColFrom, iNum, iPyrLayer);
  if (dvs.fRawIsValue())
    for (long i = 0; i < iNum; i++)
      b[i] = bb[i];
  else  
    for (long i = 0; i < iNum; i++)
      b[i] = dvs.iValue(bb[i]);
}

void MapByte::GetLineVal(long l, RealBuf& b, long iColFrom, long iColNum, int iPyrLayer) const
{
	int iDiv = (int)pow(2, (double)iPyrLayer);		
  long iNum = iColNum==0 ? iCols() / iDiv : iColNum;
  assert(iNum <= b.iSize());
  if (!dvs.fValues()) {
    for (long i = 0; i < iNum; i++)
      b[i] = rUNDEF;
    return;
  }
  ByteBuf bb(iNum);
  GetLineRaw(l, bb, iColFrom, iNum, iPyrLayer);
  if (dvs.fRawIsValue())
    for (long i = 0; i < iNum; i++)
      b[i] = bb[i];
  else  
    for (long i = 0; i < iNum; i++)
      b[i] = dvs.rValue(bb[i]);
}

void MapByte::PutLineRaw(long l, const ByteBuf& b, long iColFrom, long iColNum)
{
  if (0 == file)
    return;
  long iNum = iColNum==0 ? iCols() : iColNum;
  if (iNum+iColFrom > iCols()) iNum=iCols()-iColFrom; // clip at end of line
  assert(iNum <= b.iSize());
  if (l < 0 || l >= iLines())
    return;
  if (fPixelInterLeaved) {
    byte HUGEBUFPTR* p = b.buf();
    long iOff = iStartOffset + l * iRowLength + iColFrom * iNrBands;
    for (long c = 0; c < iColNum; c++, p++){
      file->Seek(iOff);  
      file->Write(1, p);
      iOff += iNrBands;
    }
  }
  else {
    file->Seek(iStartOffset + l * iRowLength + iColFrom);
    file->Write(iNum, b.buf());
  }
  fChanged = true;
}

void MapByte::PutLineRaw(long l, const IntBuf& b, long iColFrom, long iColNum)
{
  long iNum = iColNum==0 ? iCols() : iColNum;
  assert(iNum <= b.iSize());
  ByteBuf bb(iNum);
  for (long i = 0; i < iNum; i++)
    bb[i] = byteConv(b[i]);
  PutLineRaw(l, bb, iColFrom, iNum);
}

void MapByte::PutLineRaw(long l, const LongBuf& b, long iColFrom, long iColNum)
{
  long iNum = iColNum==0 ? iCols() : iColNum;
  assert(iNum <= b.iSize());
  ByteBuf bb(iNum);
  for (long i = 0; i < iNum; i++)
    bb[i] = byteConv(b[i]);
  PutLineRaw(l, bb, iColFrom, iNum);
}

void MapByte::PutLineVal(long l, const LongBuf& b, long iColFrom, long iColNum)
{
  if (!dvs.fValues())
    return;
  long iNum = iColNum==0 ? iCols() : iColNum;
  assert(iNum <= b.iSize());
  ByteBuf bb(iNum);
  if (dvs.fRawIsValue())
    for (long i = 0; i < iNum; i++)
      bb[i] = byteConv(b[i]);
  else  
    for (long i = 0; i < iNum; i++)
      bb[i] = byteConv(dvs.iRaw(b[i]));
  PutLineRaw(l, bb, iColFrom, iNum);
}

void MapByte::PutLineVal(long l, const RealBuf& b, long iColFrom, long iColNum)
{
  if (!dvs.fValues())
    return;
  long iNum = iColNum==0 ? iCols() : iColNum;
  assert(iNum <= b.iSize());
  ByteBuf bb(iNum);
  if (dvs.fRawIsValue())
    for (long i = 0; i < iNum; i++)
      bb[i] = byteConv(b[i]);
  else
    for (long i = 0; i < iNum; i++)
      bb[i] = byteConv(dvs.iRaw(b[i]));
  PutLineRaw(l, bb, iColFrom, iNum);
}

void MapByte::IterateCreatePyramidLayer(int iPyrLayer, long &iLastFilePos, Tranquilizer *trq)
{
	int iDiv = (int)pow(2, (double)iPyrLayer);	
	int iColsPyrLayer = iCols() / iDiv;
	int iLinePrevPyrLayer = iLines() / iDiv;
	ByteBuf buf(iColsPyrLayer), bufPrev(iColsPyrLayer);
	ByteBuf bufPyr(iColsPyrLayer / 2);
	
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
			byte iPred = bufPrev[iCol]; //rPredominant(bufPrev[iCol], bufPrev[iCol+1], buf[iCol], buf[iCol + 1]);
			bufPyr[iCol / 2] = iPred;
		}	
		filePyramid->Seek(iLastFilePos);
		iLastFilePos += filePyramid->Write(bufPyr.iSize() * sizeof(byte), (void *)bufPyr.buf());
	}			
}

