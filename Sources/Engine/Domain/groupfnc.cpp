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
/* GroupFunction
   Copyright Ilwis System Development ITC
   december 1995, by Wim Koolhoven
	Last change:  JEL  24 Jun 97    1:15 pm
*/

#include "Engine\Domain\groupfnc.h"
#include "Engine\Domain\Dmvalue.h"

GroupFunction::GroupFunction(const ValueRange& vr, int iSteps)
{
  iNr = iSteps;
  buf = new clsbnd[iNr+1];
  long i = 0;
  for (; i <= iNr; ++i)
    buf[i].cls = i;
  RangeReal rr = vr->rrMinMax();  
  long iMin = vr->iRaw(rr.rLo());
  long iMax = vr->iRaw(rr.rHi());
  long iDiff = iMax - iMin;  
  iSteps -= 1;
  for (i = 0; i < iNr; ++i)
    buf[i].bnd = iMin + (i * iDiff) / iSteps;
  buf[i].bnd = iMax;
}

GroupFunction::GroupFunction(const DomainValueRangeStruct& dvs, const DomainGroup* dg)
{
  iNr = dg->iSize() - 1;
  buf = new clsbnd[iNr+1];
  double rMax = dvs.rrMinMax().rHi();
  long iMaxRaw = dvs.iRaw(rMax);
  for (long i = 0; i <= iNr; ++i) {
    if (dg->sValueByRaw(i+1,0) == sUNDEF)
      buf[i].cls = iUNDEF;
    else  
      buf[i].cls = i + 1;
    double rUppBnd = dg->rUpper(i+1);
    if (rUppBnd > rMax)
      buf[i].bnd = iMaxRaw;
    else
      buf[i].bnd = dvs.iRaw(rUppBnd);
  }  
  sort();
}

GroupFunction::~GroupFunction()
{
  delete [] buf;
}

long GroupFunction::iClassifyRaw(long iRaw) const
{
  if (iRaw == iUNDEF)
    return iUNDEF;
  if (iRaw > buf[iNr].bnd)
    return iUNDEF;
  long i0 = 0;
  long i1 = iNr;
  while (i0 < i1) {
    long iRes = i0/2 + i1/2;
    if (iRaw <= buf[iRes].bnd)
      i1 = iRes;
    else 
      i0 = iRes + 1;
  }
  return buf[i0].cls;
}

//int _USERENTRY sortClsBnd(const void* a, const void* b)
static int sortClsBnd(const void* a, const void* b)
{
  const GroupFunction::clsbnd* cbA = (const GroupFunction::clsbnd*)a;
  const GroupFunction::clsbnd* cbB = (const GroupFunction::clsbnd*)b;
  if (cbA->bnd < cbB->bnd)
    return -1;
  else if (cbA->bnd > cbB->bnd)
    return 1;
  else
    return 0;    
}

void GroupFunction::sort()
{
  qsort(buf, iNr+1, sizeof(clsbnd), sortClsBnd);
}





