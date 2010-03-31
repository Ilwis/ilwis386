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
/* Record
   by Wim Koolhoven
  (c) Ilwis System Development ITC
	Last change:  MS   29 Sep 97    9:30 am
*/  

#pragma warning( disable : 4786 )

#include "Engine\Table\Rec.h"
#include "Engine\Table\COLSTORE.H"

namespace Ilwis
{



short Record::iCol(const String& s) const
{
  for (short i = 0; i < tbl->iCols(); ++i)
    if (fCIStrEqual(sCol(i), s))
      return i;
  return -1;  
}

String Record::sCol(int iCol) const  
{
  Column col = tbl->col(iCol);
  if (col.fValid())
    return tbl->col(iCol)->sName();
  else
    return String();
}

bool Record::fEditable(int iCol) const      
{ 
  if (tbl->fDataReadOnly())
    return false;
  Column col = tbl->col(iCol);
  if (col.fValid())
    return !tbl->col(iCol)->fDataReadOnly();
  else
    return false;
}

void Record::PutVal(int iCol, const String& s)
{ 
  Column col = tbl->col(iCol);
  if (col.fValid()) {
    col->PutVal(iCurr, s);
    col->Updated();
    tbl->fChanged = true;
  }
}

bool Record::Goto(const String& sKey)
{
  long iTmp = tbl->dm()->iRaw(sKey);
  if (iUNDEF == iTmp) {
		if (0 == tbl->dm()->pdnone())
      return false;
		iTmp = sKey.iVal();
		if (iTmp < tbl->iOffset() || iTmp > tbl->iRecs() - 1 + tbl->iOffset())
			return false;
	}
  iCurr = iTmp;
  return true;
}

void Record::Next()
{
    long iHere=iCurr++;
    if (iCurr >= tbl->iOffset() + tbl->iRecs()+1)
            First();
    while(fDeleted() && iCurr!=iHere)
    {
        if (iCurr >= tbl->iOffset() + tbl->iRecs()+1)
            First();
        else
            ++iCurr;
    }
}

void Record::Back()
{
    long iHere=iCurr--;
    if (iCurr < tbl->iOffset())
            Last();
    while(fDeleted() && iCurr!=iHere)
    {
        if (iCurr < tbl->iOffset())
            Last();
        else
            --iCurr;
    }
}

bool Record::fDeleted() const
{
  if (tbl->dm()->pdsrt())
    return (bool)(tbl->dm()->pdsrt()->iOrd(iCurr)==iUNDEF);
  return false;
}

}




