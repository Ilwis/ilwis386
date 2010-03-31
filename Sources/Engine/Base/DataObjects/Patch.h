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
/* patch.h
   Patch Interface for ILWIS 2.0
   22 Mar 95, by Willem Nieuwenhuis
   Copyright ILWIS Department, ITC
	Last change:  WK   28 Aug 98    5:16 pm
*/

#ifndef ILW_PATCH
#define ILW_PATCH

#include "Engine\Base\DataObjects\Buf.h"

#define PATCH_SIDE 64
#define PATCH_UNITS 4096

template <class T>
class Patch : public Buf<T> {        // Let buffer handle storage
public:
  Patch();
  Patch(const Patch<T>&);
  virtual ~Patch();   //  { if (_pt) delete _pt; } ; Buf takes care of this

  T  operator ()(short iRow, short iCol) const
    { return buf()[(unsigned)iRow * PATCH_SIDE + iCol]; }
  T& operator ()(short iRow, short iCol)
    { return buf()[(unsigned)iRow * PATCH_SIDE + iCol]; }
  T  operator [](short iRC) const
    { return buf()[(unsigned)iRC]; }
  T& operator [](short iRC)
    { return buf()[(unsigned)iRC]; }
};

template <class T>
Patch<T>::Patch() :
    Buf<T>((unsigned short)( PATCH_UNITS ))
{
  if (buf()) memset(buf(), 0, PATCH_UNITS * sizeof(T));
}

template <class T>
Patch<T>::Patch(const Patch<T>& mmp) : Buf<T>(mmp) {}

template <class T>
Patch<T>::~Patch() 
{}

#ifdef TLSTMPL_C
#define PATCH_SPEC __export
#else
#define PATCH_SPEC __import
#endif
template class PATCH_SPEC Patch<byte>;
template class PATCH_SPEC Patch<short>;
template class PATCH_SPEC Patch<long>;
template class PATCH_SPEC Patch<double>;

typedef Patch<byte>   BytePatch;
typedef Patch<short>  IntPatch;
typedef Patch<long>   LongPatch;
typedef Patch<double> RealPatch;


#endif







