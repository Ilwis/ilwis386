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
/* MapBit
   Copyright Ilwis System Development ITC
   march 1995, by Wim Koolhoven
	Last change:  WK   21 Jul 97   10:13 am
*/

#include "Engine\Map\Raster\MAPBIT.H"
#include "Engine\Domain\Dmvalue.h"

class BitBuf: public Buf<byte>
{
  BitBuf& operator = (const BitBuf&);  // not allowed
protected:
  int iBitNum;  // Number of bits to store (GenBuf stores #bytes needed to store the bits)
public:
  BitBuf(int iBufSize) : Buf<byte>(((iBufSize+7)>>3)+1)  { iBitNum = iBufSize; }
  int iSize() const  { return iBitNum; }
  byte	get(int) const;	  // bitwise
  void	put(int, long);	  // bitwise
  void  ShiftLeft(int);   // shift bits in buffer
  void  ShiftRight(int);  // shift bits in buffer
};

inline byte BitBuf::get(int i) const
{
  byte b = operator[](i>>3);
  return (b & (0x80 >> i % 8)) != 0;
}

inline void BitBuf::put(int i, long v)
{
  if (v == 0)
    operator[](i>>3) &= ~(0x80 >> (i & 7));
  else
    operator[](i>>3) |= 0x80 >> (i & 7);
}

// Shift the contents of the bit buffer
// iSh is number of bits to shift ( 1 <= iSh <= 7)
// Shift Left: Left most bit will stay in the first byte, so no extra space is needed
void BitBuf::ShiftLeft(int iSh)
{
  byte HUGEBUFPTR *p = &operator[](0);
  int i;
  for (i=0; i<ByteBuf::iSize()-1; i++) {     // the loop takes care of completely
    *p=(*p << iSh) + (*(p+1) >> (8-iSh));   // bit filled bytes
    p++;
  }
  *p=(*p<<iSh);     // last bits also
}

// Right shift: first bit will stay in first byte; one byte extra storage at
// end may be needed (this already provided for during allocation of the bitbuf)
void BitBuf::ShiftRight(int iSh)
{
  byte HUGEBUFPTR *p = &operator[](ByteBuf::iSize()-2);
  int i;
  for (i=ByteBuf::iSize()-1; i>0; i--) {    // the loop takes care of completely
    *(p+1)=(*(p+1) >> iSh) + (*p << (8-iSh));   // bit filled bytes
    p--;
  }
  *p=(*p>>iSh);     // first bits also
}

// class MapBit functions
String MapBit::sType() const
{ 
	return "Bit";
}

StoreType MapBit::st() const
{ 
	return stBIT;
}

long MapBit::iRaw(RowCol rc) const
{
  if (0 == file)
    return iUNDEF;
  if (!fInside(rc))
    return iUNDEF;
  byte b;
  ULONGLONG loc = iStartOffset + (ULONGLONG)rc.Row * ((iRowLength+7) >> 3) + (rc.Col >> 3);
  file->Seek(loc);
  file->Read(1, &b);
  return (b & (0x80 >> (rc.Col & 7))) ? 1 : 0;
}

void MapBit::PutRaw(RowCol rc, long v)
{
  if (0 == file)
    return;
  if (!fInside(rc)) return;
  byte b;
  ULONGLONG loc = iStartOffset + (ULONGLONG)rc.Row * ((iRowLength+7) >> 3) + (rc.Col >> 3);
  file->Seek(loc);
  file->Read(1, &b);
  if (v == 0 || v == iUNDEF)
    b &= ~(0x80 >> (rc.Col & 7));
  else
    b |= 0x80 >> (rc.Col & 7);
  file->Seek(loc);
  file->Write(1, &b);
  fChanged = true;
}

// Get/Put a (part of a) line

void MapBit::GetLine(long l, BitBuf& b, long iColFrom, long iColNum, int iPyrLayer) const
{
  if (0 == file)
    return;

  long iNum = iColNum==0 ? iCols() : iColNum;
  assert(l >= 0 && l < iLines());
  if (iNum+iColFrom > iCols()) iNum=iCols()-iColFrom; // clip at end of line
  assert(iNum <= b.iSize());
  int iSh=iColFrom & 7;        // bit number in byte
  long iPos = iColFrom>>3;    // position of byte containing first bit
  long iLen = (iNum+iSh+7)>>3;    // number of bytes to fit the bits
  long iLineSize = (iRowLength+7)>>3;  // number of bytes per line of bits
  
  file->Seek(iStartOffset + (ULONGLONG)l * iLineSize + iPos);
  file->Read(iLen, b.buf());

  if (iSh != 0) {          // not at byte boundary so a bit-shift is needed
     b.ShiftLeft(iSh);
  }
}

void MapBit::GetLineRaw(long l, ByteBuf& b, long iColFrom, long iColNum, int iPyrLayer ) const
{
  long iNum = iColNum==0 ? iCols() : iColNum;
  assert(iNum <= b.iSize());
  BitBuf bb(iNum);
  GetLine(l, bb, iColFrom, iNum, iPyrLayer);
  for (int i = 0; i < iNum; i++)
    b[i] = bb.get(i);
}

void MapBit::GetLineRaw(long l,  IntBuf& b, long iColFrom, long iColNum, int iPyrLayer) const
{
  long iNum = iColNum==0 ? iCols() : iColNum;
  assert(iNum <= b.iSize());
  BitBuf bb(iNum);
  GetLine(l, bb, iColFrom, iNum, iPyrLayer);
  for (int i = 0; i < iNum; i++)
    b[i] = bb.get(i);
}

void MapBit::GetLineRaw(long l, LongBuf& b, long iColFrom, long iColNum, int iPyrLayer) const
{
  long iNum = iColNum==0 ? iCols() : iColNum;
  assert(iNum <= b.iSize());
  BitBuf bb(iNum);
  GetLine(l, bb, iColFrom, iNum, iPyrLayer);
  for (int i = 0; i < iNum; i++)
    b[i] = bb.get(i);
}

void MapBit::GetLineVal(long l, LongBuf& b, long iColFrom, long iColNum, int iPyrLayer) const
{
  long iNum = iColNum==0 ? iCols() : iColNum;
  assert(iNum <= b.iSize());
  if (!dvs.fValues()) {
    for (int i = 0; i < iNum; i++)
      b[i] = iUNDEF;
    return;
  }
  BitBuf bb(iNum);
  GetLine(l, bb, iColFrom, iNum, iPyrLayer);
  if (dvs.fRawIsValue())
    for (int i = 0; i < iNum; i++)
      b[i] = bb.get(i);
  else  
    for (int i = 0; i < iNum; i++)
      b[i] = vr()->iValue(bb.get(i));
}

void MapBit::GetLineVal(long l, RealBuf& b, long iColFrom, long iColNum, int iPyrLayer) const
{
  long iNum = iColNum==0 ? iCols() : iColNum;
  assert(iNum <= b.iSize());
  if (!dvs.fValues()) {
    for (int i = 0; i < iNum; i++)
      b[i] = iUNDEF;
    return;
  }
  BitBuf bb(iNum);
  GetLine(l, bb, iColFrom, iNum, iPyrLayer);
  if (dvs.fRawIsValue())
    for (int i = 0; i < iNum; i++)
      b[i] = bb.get(i);
  else
    for (int i = 0; i < iNum; i++)
      b[i] = vr()->rValue(bb.get(i));
}

void MapBit::PutLine(long l, BitBuf& b, long iColFrom, long iColNum)
{
  if (0 == file)
    return;
  long iNum = iColNum==0 ? iCols() : iColNum;
  assert(l >= 0 && l < iLines());
  if (iNum+iColFrom > iCols()) iNum=iCols()-iColFrom; // clip at end of line
  assert(iNum <= b.iSize());
  BitBuf bb(iNum);
  GetLine(l, bb, iColFrom, iColNum);
  int iSh=iColFrom & 7;        // bit number in byte
  long iPos = iColFrom>>3;    // position of byte containing first bit
  long iLen = (iNum+iSh+7)>>3;    // number of bytes to fit the bits
  long iLineSize = (iRowLength+7)>>3;  // number of bytes per line of bits
  if (iSh != 0) {          // not at byte boundary so a bit-shift is needed
     b.ShiftRight(iSh);
  }
  file->Seek(iStartOffset + (ULONGLONG)l * iLineSize + iPos);
  file->Write(iLen, b.buf());
  fChanged = true;
}

void MapBit::PutLineRaw(long l, const ByteBuf& b, long iColFrom, long iColNum)
{
  long iNum = iColNum==0 ? iCols() : iColNum;
  assert(iNum <= b.iSize());
  BitBuf bb(iNum);
  for (int i = 0; i < iNum; i++)
    bb.put(i, b[i]==0 ? 0 : 1);
  PutLine(l, bb, iColFrom, iNum);
}

void MapBit::PutLineRaw(long l, const IntBuf& b,  long iColFrom, long iColNum)
{
  long iNum = iColNum==0 ? iCols() : iColNum;
  assert(iNum <= b.iSize());
  BitBuf bb(iNum);
  for (int i = 0; i < iNum; i++)
    bb.put(i, b[i]==0 || b[i]==shUNDEF ? 0 : 1);
  PutLine(l, bb, iColFrom, iNum);
}

void MapBit::PutLineRaw(long l, const LongBuf& b, long iColFrom, long iColNum)
{
  long iNum = iColNum==0 ? iCols() : iColNum;
  assert(iNum <= b.iSize());
  BitBuf bb(iNum);
  for (int i = 0; i < iNum; i++)
    bb.put(i, b[i]==0 || b[i]==iUNDEF ? 0 : 1);
  PutLine(l, bb, iColFrom, iNum);
}

void MapBit::PutLineVal(long l, const LongBuf& b, long iColFrom, long iColNum)
{
  if (!dvs.fValues())
    return;
  long iNum = iColNum==0 ? iCols() : iColNum;
  assert(iNum <= b.iSize());
  BitBuf bb(iNum);
  if (dvs.fRawIsValue())
    for (int i = 0; i < iNum; i++)
      bb.put(i, b[i]);
  else  
    for (int i = 0; i < iNum; i++)
      bb.put(i, vr()->iRaw(b[i]));
  PutLine(l, bb, iColFrom, iNum);
}

void MapBit::PutLineVal(long l, const RealBuf& b, long iColFrom, long iColNum)
{
  if (!dvs.fValues())
    return;
  long iNum = iColNum==0 ? iCols() : iColNum;
  assert(iNum <= b.iSize());
  BitBuf bb(iNum);
  if (dvs.fRawIsValue())
    for (int i = 0; i < iNum; i++)
      bb.put(i, longConv(b[i]));
  else
    for (int i = 0; i < iNum; i++)
      bb.put(i, vr()->iRaw(b[i]));
  PutLine(l, bb, iColFrom, iNum);
}


void MapBit::IterateCreatePyramidLayer(int iPyrLayer, ULONGLONG &iLastFilePos)
{
	/*int iDiv = pow(2, iPyrLayer);	
	int iColsPyrLayer = iCols() / iDiv;
	int iLinePrevPyrLayer = iLines() / iDiv;
	ByteBuf buf(iColsPyrLayer), bufPrev(iColsPyrLayer);
	ByteBuf bufPyr(iColsPyrLayer / 2);
	
	for(int iLine = 0, iPyrLine = 0; iPyrLine < iLinePrevPyrLayer / 2; iLine += 2, ++iPyrLine)
	{
		int iPyrCol = 0;		
		GetLineRaw(iLine, buf, 0, iColsPyrLayer, iPyrLayer);			
		GetLineRaw(iLine + 1, bufPrev, 0, iColsPyrLayer, iPyrLayer);
		for(int iCol = 0; iCol < iColsPyrLayer - 2; iCol += 2)
		{
			double rPred = bufPrev[iCol]; //rPredominant(bufPrev[iCol], bufPrev[iCol+1], buf[iCol], buf[iCol + 1]);
			bufPyr[iCol / 2] = rPred;
		}	
		filePyramid->Seek(iLastFilePos);
		iLastFilePos += filePyramid->Write(bufPyr.iSize() * 1, (void *)bufPyr.buf());
	}	*/		
}
