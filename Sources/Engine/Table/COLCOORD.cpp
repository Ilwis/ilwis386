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
/* ColumnCoord
   Copyright Ilwis System Development ITC
   oct. 1996, by Jelle Wind
	Last change:  JELLA  27 May 97    1:50 pm
*/
#define ILWCOLCOORD_C
#include "Engine\Table\COLCOORD.H"
#include "Engine\Domain\dmcoord.h"

ColumnCoord::ColumnCoord(long iRecs, long iOffset, ColumnStore& cs, bool fCreate)
: ColumnStoreBase(iRecs, iOffset, cs, fCreate),  buf(iRecs) // iOffset)
{
  domCrd = dm()->pdcrd();
  if (fCreate)
    Fill();  // NAN protection for not used domain values
}

ColumnCoord::~ColumnCoord()
{
//  if (0 != colX)
//    delete colX;
//  if (0 != colY)
//    delete colY;
}

String ColumnCoord::sType() const
{
	return domCrd->f3D() ? "Coord3D" :  "Coord";
}

Coord ColumnCoord::cValue(long iKey) const
{
  if ((iKey < iOffset()) || (iKey >= iOffset()+iRecs()))
    return crdUNDEF;
  return buf[iKey-iOffset()];
}

void ColumnCoord::GetBufVal(CoordBuf& b, long iKey, long iNr) const
{
  if (iNr == 0 || iNr > b.iSize())
    iNr = b.iSize();
  int i = 0;
  for (long j = iKey; i < iNr; ++j, ++i)
    try {
      b[i] = buf[j-iOffset()];
    }
    catch (...) {
      b[i] = crdUNDEF;
    }
}

String ColumnCoord::sValue(long iKey, short iWidth, short iDec) const
{
  if ((iKey < iOffset()) || (iKey >= iOffset()+iRecs()) || 0 == domCrd)
    return sUNDEF;
  return domCrd->sValue(buf[iKey-iOffset()], iWidth, iDec);
}

void ColumnCoord::GetBufVal(StringBuf& b, long iKey, long iNr, short iWid, short iDec) const
{
  if (iNr == 0 || iNr > b.iSize())
    iNr = b.iSize();
  int i = 0;
  for (long j = iKey; i < iNr; ++j, ++i)
    try {
			if (0 == domCrd)
				b[i] = sUNDEF;
			else
				b[i] = domCrd->sValue(buf[j-iOffset()], iWid, iDec);
    }
    catch (...) {
      b[i] = sUNDEF;
    }
}

void ColumnCoord::PutVal(long iKey, const Coord& cVal)
{
  if ((iKey < iOffset()) || (iKey >= iOffset()+iRecs()))
		return;
  buf[iKey-iOffset()] = cVal;
//  Updated();
//  TablePtr::Changed(fnTbl);
}

void ColumnCoord::PutBufVal(const CoordBuf& b, long iKey, long iNr)
{
  if (iNr == 0 || iNr > b.iSize())
    iNr = b.iSize();
  int i = 0;
  for (long j = iKey; i < iNr; ++j, ++i)
    buf[j-iOffset()] = b[i];
//  Updated();
//  TablePtr::Changed(fnTbl);
}

void ColumnCoord::PutVal(long iKey, const String& sVal)
{
	if (0 == domCrd)
		return;
  if ((iKey < iOffset()) || (iKey >= iOffset()+iRecs()))
		return;
  buf[iKey-iOffset()] = domCrd->cValue(sVal);
//  Updated();
//  TablePtr::Changed(fnTbl);
}

void ColumnCoord::PutBufVal(const StringBuf& b, long iKey, long iNr)
{
	if (0 == domCrd)
		return;
  if (iNr == 0 || iNr > b.iSize())
    iNr = b.iSize();
  int i = 0;
  for (long j = iKey; i < iNr; ++j, ++i)
    buf[j-iOffset()] = domCrd->cValue(b[i]);;
//  Updated();
//  TablePtr::Changed(fnTbl);
}

void ColumnCoord::DeleteRec(long iStartRec, long iRecs)
{
  buf.Remove(iStartRec-iOffset(), iRecs);
  _iRecs = buf.iSize();
}

void ColumnCoord::AppendRec(long iRcs)
{
  buf.Append(iRcs);
  for (unsigned long i = iRecs(); i < buf.iSize(); i++)
    buf[i] = crdUNDEF;
  _iRecs = buf.iSize();
}

void ColumnCoord::Fill()
{
  for (unsigned long i=0; i < buf.iSize(); i++)
    buf[i] = crdUNDEF;
}






