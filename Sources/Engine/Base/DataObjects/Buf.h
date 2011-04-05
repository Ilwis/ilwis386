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
/* $Log: /ILWIS 3.0/Tools/Buf.h $
 * 
 * 6     20-01-00 4:37p Martin
 * copy constructor sets initial size to 0, destructor initializes (sets
 * 0) basemembers.
 * 
 * 5     19-01-00 16:19 Wind
 * removed check on 64KB
 * 
 * 4     19-11-99 11:18a Martin
 * inline function caused compiler error under Win95 (not under NT)
 * 
 * 3     28-04-99 12:08 Koolhoven
 * Header comment
 * 
 * 2     28-04-99 12:07 Koolhoven
 * IntBuf and IntBufExt now use shorts, because 2 byte size is assumed at
 * lots of places
// Revision 1.4  1997/09/11 08:11:26  Wim
// now all new's give a MemAllocError() when they return a null pointer
//
// Revision 1.3  1997-09-01 13:03:16+02  Wim
// Switched asserts on in operator []
//
// Revision 1.2  1997-08-08 15:53:13+02  Willem
// Added FloatBuf and FloatBufExt, they were missing.
//
/* dat/buf.h
 Buffer Interface for ILWIS 2.0
 17 may 1993, by Wim Koolhoven
 template by Jelle Wind
	Last change:  WK   28 Aug 98    5:18 pm
*/

#ifndef ILWBUF_H
#define ILWBUF_H

#include "Geos\Geos.h"
#include "Engine\Base\DataObjects\Dat2.h"

struct Coord;
struct RowCol;

void _export MemAllocError();

template <class T>
class Buf// : public ErrorHandling
{
protected:
  T * ptr;
  int iSz;
  bool fExt;
public:
  int iSize() const;
  T * buf() const   { return ptr; }
  Buf() { ptr = 0; iSz = 0; fExt = false;};
  Buf( int iBufSize)
    { ptr = 0; iSz = 0; Size(iBufSize);fExt=false; }
  Buf(const Buf<T>& buf);
  Buf<T>& operator = (const Buf<T>& buf);
  virtual ~Buf();
  void Size( int iBufSize);
  void Reverse(long iStart=iUNDEF, long iEnd=iUNDEF);
  friend void Swap(Buf<T>& buf1, Buf<T>& buf2)
    { T * hptr = buf1.ptr; buf1.ptr = buf2.ptr; buf2.ptr = hptr; }
  T  operator[](int i) const
  { if ( !fExt) {assert( i >= 0 && i < iSz && ptr);} // extended buffers could be there!
        return ptr[i]; }
  T & operator[](int i)
  { if ( !fExt) {assert(i >= 0 && i < iSize() && ptr);}
        return ptr[i]; }
};

class _export CoordBuf : public CoordinateArraySequence {
public:
	int iSize() const { return (int)size(); };
	CoordBuf();
	CoordBuf(int iSize);
	CoordBuf(const CoordinateSequence *seq);
	Coord operator[](int i) const;
	Coord& operator[](int i);
	void Size(int iBufSize);
	CoordBuf& operator=(const CoordBuf& b);
	CoordBuf& operator=(const CoordinateSequence *b);
};

template <class T> inline int Buf<T>::iSize() const
{ return iSz; }

template <class T>
class BufExt: public Buf<T>
{
private:
  int iExtendLo, iExtendHi;
public:
  int iTotSize() const { return (int)(iSz + iExtLo() + iExtHi()); }
  int iExtLo() const { return iExtendLo; }
  int iExtHi() const { return iExtendHi; }
  int iUpper() const { return (int)(iSize() + iExtHi() - 1); }
  int iLower() const { return -(int)iExtLo(); }
  BufExt() : Buf<T>() { iExtendLo = iExtendHi = 0;fExt=true; };
  BufExt( int iBufSize, int iExtLo = 0, int iExtHi = 0)
    : Buf<T>()
    { Size(iBufSize, iExtLo, iExtHi); fExt=true;}
  BufExt(const BufExt<T>& buf);
  BufExt<T>& operator = (const BufExt<T>& buf);
  virtual ~BufExt();
  void SetPtr()
    { ptr += iExtLo(); }
  void ResetPtr()
    { ptr -= iExtLo(); }
  void Size( int iBufSize, int iExtLo = 0, int iExtHi = 0);
  friend void Swap(BufExt<T>& buf1, BufExt<T>& buf2)
    { T * hptr = buf1.ptr; buf1.ptr = buf2.ptr; buf2.ptr = hptr; }
};

template <class T>
Buf<T>::Buf(const Buf<T>& buf)
{ ptr = 0;
  iSz =0;
  fExt = false;
  Size(buf.iSize());
  for (int i=0; i < iSize(); ++i)
    operator[](i) = buf[i];
//      memcpy(ptr, buf.buf(), iSize() * sizeof(T));
}

template <class T> void Buf<T>::Reverse(long iStart, long iEnd)
{
    T temp;
    if (iStart==iUNDEF) iStart=0;
    if (iEnd==iUNDEF) iEnd=iSize();
    for(long i=iStart; i<iEnd/2; ++i)
    {
        temp=ptr[i];
        ptr[i]=ptr[iEnd-i-1];
        ptr[iEnd-i-1]=temp;
    }
}

template <class T>
Buf<T>& Buf<T>::operator = (const Buf<T>& buf) {
  Size(buf.iSize());
  for (int i=0; i < iSize(); ++i)
    operator[](i) = buf[i];
//  memcpy(ptr, buf.buf(), iSize() * sizeof(T));
  return *this;
}

template <class T>
Buf<T>::~Buf()
{
  delete [] ptr;
	ptr = NULL;
	iSz = 0;
}

template <class T>
void Buf<T>::Size( int iBufSize)
{
  if (iSz == iBufSize)
    return;
  iSz = iBufSize;
  if (ptr) {
    delete [] ptr;
    ptr = 0;
  }
  long iTotSize = sizeof(T) * long(iSize());
  ptr = new T [(long)iSize()];
  if (0 == ptr)
    MemAllocError();
}

template <class T>
BufExt<T>::BufExt(const BufExt<T>& buf) : Buf<T>()
{ iExtendLo = buf.iExtLo();
  iExtendHi = buf.iExtHi();
  iSz = buf.iSize();
  fExt = true;
  ptr = new T [(long)buf.iTotSize()];
  if (0 == ptr)
    MemAllocError();
  SetPtr();
//  memcpy(&ptr[iLower()], &(buf.buf()[iLower()]), iTotSize() * sizeof(T));
  for (int i = iLower(); i <= iUpper(); i++)
    operator[](i) = buf[i];
}

template <class T>
BufExt<T>::~BufExt()
{
  ResetPtr();
}

template <class T>
void BufExt<T>::Size( int iBufSize, int iExtLo, int iExtHi)
{
  if (ptr) {
    ResetPtr();
    delete [] ptr;
  }
  iSz = iBufSize;
  iExtendLo = iExtLo; iExtendHi = iExtHi;
  if (iSz != 0) {
	  long iSz = iTotSize();
    ptr = new T [iSz];
    if (0 == ptr)
      MemAllocError();
    SetPtr();
  }
  else
    ptr = 0;
}

template <class T>
BufExt<T>& BufExt<T>::operator = (const BufExt<T>& buf)
{
  assert(ptr);
  int iMax = min(iUpper(), buf.iUpper());
  int iMin = max(iLower(), buf.iLower());
  for (int i = iMin; i <= iMax; i++)
    operator[](i) = buf[i];
  return *this;
}

typedef Buf<byte>    ByteBuf;
typedef Buf<short>   IntBuf;
typedef Buf<long>    LongBuf;
typedef Buf<float>   FloatBuf;
typedef Buf<double>  RealBuf;
typedef Buf<RowCol>  RowColBuf;
//typedef Buf<Coord>   CoordBuf;

typedef BufExt<byte>    ByteBufExt;
typedef BufExt<short>   IntBufExt;
typedef BufExt<long>    LongBufExt;
typedef BufExt<double>  RealBufExt;
typedef BufExt<RowCol>  RowColBufExt;
typedef BufExt<Coord>   CoordBufExt;

#endif







