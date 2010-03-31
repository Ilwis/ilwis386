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
/* $Log: /ILWIS 3.0/Tools/ARRAY.CPP $
 * 
 * 3     14-06-99 18:10 Koolhoven
 * Header comment
 * 
 * 2     14-06-99 8:33a Martin
 * shorts to int where applicable
// Revision 1.3  1997/09/08 14:24:49  Wim
// Array::Reset(0) was causing memory leaks
//
// Revision 1.2  1997-09-02 15:59:11+02  Wim
// Removed the word HUGE
*/
#define ARRAY_C
#include "Headers\toolspch.h"

BaseArray::BaseArray(int iItemSiz)
 : iItemSize(iItemSiz)
{ p = 0;
  iSiz = iOffset = 0; }

BaseArray::BaseArray(const BaseArray& a)
 : iItemSize(a.iItemSize)
{ Reset();
  Resize(a.iSize(), a.iLower());
  if (a.iSize() == 0) return;
  for (int i = iLower(); i <= iUpper(); i++)
    PutVal(i, a.index(i));
}

const void* BaseArray::index(int i) const
{ if (fIndexValid(i))
    return &p[i*iItemSize];
  else
    return undef();
}

void* BaseArray::index(int i)
{ if (fIndexValid(i))
    return &p[i*iItemSize];
  else
    return undef();
}

void BaseArray::Append(int iSize)
{ Resize(iSiz + iSize, iOffset); }

void BaseArray::Reset()
{ if (p != 0) {
    ResetPtr();
    Free(p);
    p = 0;
  }
  iSiz = 0;
}

void BaseArray::Resize(int iSize, int iOff)
{
  if (iSize == iSiz) { 
    ResetPtr();
    iOffset = iOff; 
    SetPtr();
    return; 
  }
  int iNewSize = iAdjustedSize(iSize);
  if (iSiz == 0) {
    Allocate(iNewSize);
    if (p == 0) return;
    iSiz = iSize;
    iOffset = iOff;
    SetPtr();
  }
  else if (iNewSize > iAdjustedSize(iSiz)) {
    ResetPtr();
    byte* pp = p;
    Allocate(iNewSize);
    if (p == 0) return;
    int iSizeOld = iSiz;
    iSiz = iSize;
    iOffset = iOff;
    SetPtr();
    for (int i = 0; i < iSizeOld; i++)
      PutVal(i+iLower(), &pp[i*iItemSize]);
    Free(pp);
    return;
  }
  else if (iSize == 0) {
    Reset();
    iOffset = iOff;
  }
  else {
    iSiz = iSize;
    iOffset = iOff;
  }
}

void BaseArray::Insert(int iStart, int iSz)
{
  if (iStart > iUpper())
    { Append(iSz); return; }
  Resize(iSize() + iSz, iOffset);
  for (int i = iUpper(); i >= iStart + iSz; i--)
    PutVal(i, index(i-iSz));
}

void BaseArray::Remove(int iStart, int iSz)
{
  if (iStart > iUpper()) return;
  if (iStart + iSz > iUpper())
    iSz = iUpper() - iStart + 1;
  for (int i = iStart; i <= iUpper() - iSz; i++)
    PutVal(i, index(i+iSz));
  Resize(iSize() - iSz, iOffset);
}


void BaseArray::operator=(const BaseArray& a)
{ Reset();
  Resize(a.iSize(), a.iLower());
  if (a.iSize() == 0) return;
  for (int i = iLower(); i <= iUpper(); i++)
    PutVal(i, a.index(i));
}

void BaseArray::operator&=(const BaseArray& a)
{
  int iOldUpper = iUpper();
  Resize(iSize() + a.iSize(), iOffset);
  for (int i = 0; i < a.iSize(); i++)
    PutVal(iOldUpper+i+1, a.index(i));
}

void BaseArray::operator&=(const void *p)
{
  int iOldUpper = iUpper();
  Resize(iSize() + 1, iOffset);
  PutVal(iOldUpper+1, p);
}

void BaseArray::PutVal(int, const void*)
{
}

void BaseArray::Allocate(int)
{
}

void* BaseArray::undef() const
{
  return 0;
}

void BaseArray::Free(void*)
{
}








