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
/* grouplim.c GroupingLimits
   Copyright Ilwis System Development ITC
   april 1995, by Jelle Wind
	Last change:  WK   28 Aug 98    4:10 pm
*/

#define GROUPLIM_C
#include "Headers\toolspch.h"
#include "Engine\Base\Algorithm\Grouplim.h"


GroupingLimits::GroupingLimits(double rMaxDiff, int iMaxNr, int iNrSubDec)
// make interval table for grouping of observations based on distances.
// table is based on row Er (see below) and decimal multiples.
{
  int Ert[] = {10, 11, 12, 13, 15, 16, 18, 20, 22, 24, 27, 30,
               33, 36, 39, 43, 47, 51, 56, 62, 68, 75, 82, 91, 100};
  int istep,imax;
  int ic,ir;// counters
  int Er[26];
  iMaxNmbr=iMaxNr;  //keep max value for lookup
  switch (iNrSubDec)
  { case  6:
      istep=4; imax= 5; break;
    case 24:
      istep=1; imax=23; break;
    case 12:
    default:
      istep=2; imax=11;
  }
  for (ic = 0, ir = 0; ic <= imax + 1; ic += istep)
    Er[ir++] = Ert[ic];
  arGroupLim.Resize(iMaxNr+1); // zero based now
  // compute first power of 10 >= rMaxDiff
  int impow10 = (int) log10(rMaxDiff) - 1;
  int iErc = 0;
  while (Er[iErc] * pow(10.0, (double) impow10) < rMaxDiff)
    iErc++;
  for (ic = iMaxNr; ic > 0; ic--)  {
    arGroupLim[ic] = Er[iErc--] * pow(10.0, (double) impow10);
    if (iErc < 0) {
      impow10--; iErc = imax;
    }
  }
}


int GroupingLimits::iGroup(double rD)
// look with given distance rD for place in interval table
{
  int il,im,ih;
  il = 1; ih = iMaxNmbr;
  if (rD > arGroupLim[ih])
    return (iMaxNmbr+1);
  if (rD < arGroupLim[il])
    return (il);
  while (ih-il > 1) {
    im = (il+ih) / 2;
    if (rD > arGroupLim[im])
      il = im;
    else
      ih = im;
  }
  return ih;
}

float GroupingLimits::rValue(int index)
{
  return (float)(arGroupLim[index]);
}

GroupingLimits::~GroupingLimits()
{
}







