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
/* tls/smplhs.c
   Sampling histogram and statistics Implementation for ILWIS 2.0
   march 1995, by Jan-Willem Rensman
   (c) Computer Department ITC
	Last change:  JEL  21 Jun 97    1:16 pm
*/
#include "Engine\SampleSet\SMPLHS.H"
#include "Engine\Map\Raster\Map.h"
#include "Engine\Domain\Dmvalue.h"
#include "Engine\Base\DataObjects\Buflist.h"

static long iDummy = 0;
static double rDummy = 0;

long& SmplHist::operator() (byte bKey, short iBand, byte bVal)
{ 
  if (!riRanges[iBand].fContains(bVal))
    return iDummy;
  return iSHptr[bKey][iBand][bVal]; 
}

double& SmplStats::operator()(byte bKey, short iBand, byte b) const
{ 
  if (bKey > dc->iSize())
    return rDummy;
  return rSSptr[bKey][iBand][b]; 
}

void SmplHist::Allocate(DomainClass* domc, const MapList& ml)
{
  dc = domc;
  iNrBands = (short)ml->iSize();
  int b;
  byte bKey;
  riRanges = new RangeInt[iNrBands];
  Map mp;
  
  for (b = ml->iLower(); b <= ml->iUpper(); b++) 
  {
    riRanges[b-(ml->iLower())] = ml[b]->riMinMax(BaseMapPtr::mmmCALCULATE);
  }
  iSHptr = new long**[MAXCLASS+1]; 
  for (int i=0; i<=MAXCLASS ; i++) 
    iSHptr[i] = 0;
  // index 0 is used for selection-class:
  AppClass(0);
  for (bKey = 1; bKey <= dc->iSize(); bKey++)
//{ 
//  bKey = dc->iKey(bOrd);
    AppClass(bKey);
//}
} 
  
SmplHist::~SmplHist()
{
  if (0 == iSHptr)
    return;
  byte bKey;
  DelClass(0);
  for (bKey = 1; bKey <= dc->iSize(); bKey++)
//{ 
//  bKey = dc->iKey(bOrd);
    DelClass(bKey);
//}
  delete [] iSHptr;
  delete [] riRanges;
}
    
void SmplHist::AppClass(byte bKey)
{
  if (0 != iSHptr[bKey]) 
    return;
  int b, i;
  iSHptr[bKey] = new long*[iNrBands];
  for (b = 0; b < iNrBands; b++)
  { 
    iSHptr[bKey][b] = new long[riRanges[b].iHi() - 
                             riRanges[b].iLo() + 1];
    iSHptr[bKey][b] -= riRanges[b].iLo();
      // init all with 0.
    for (i = riRanges[b].iLo(); i <= riRanges[b].iHi(); i++) 
      operator() (bKey, b, i) = 0;
  }
}

void SmplHist::DelClass(byte bKey)
{
  int b;
  for (b = 0; b < iNrBands; b++)
  { 
    iSHptr[bKey][b] += riRanges[b].iLo();
    delete [] iSHptr[bKey][b];
  }  
  delete [] iSHptr[bKey];  
  iSHptr[bKey] = 0;
}   

void SmplHist::MrgClass(byte bKey1, byte bKey2)
{
  if (bKey1 == bKey2) return;
  int b, i;
  for (b = 0; b < iNrBands; b++) 
    for (i = riRanges[b].iLo(); i <= riRanges[b].iHi(); i++)
      operator() (bKey1, b, i) += operator() (bKey2, b, i);
  for (b = 0; b < iNrBands; b++) 
    for (i = riRanges[b].iLo(); i <= riRanges[b].iHi(); i++)
      operator() (bKey2, b, i) = 0;
}    

void SmplHist::ResetClass(byte bKey)
{
  int b, i;
  for (b = 0; b < iNrBands; b++) 
    for (i = riRanges[b].iLo(); i <= riRanges[b].iHi(); i++)
      operator() (bKey, b, i) = 0;
}    
 
/**************/

void SmplStats::Allocate(DomainClass* domc, short iNrBnd)
{
  dc = domc;
  iNrBands = iNrBnd;
  int i;
  byte bKey;
  rSSptr = new double**[MAXCLASS+1];
  bSSptr = new byte*[MAXCLASS+1];
  for (i=0; i<=MAXCLASS ; i++) {
    rSSptr[i] = 0;
    bSSptr[i] = 0;
  }
  // index 0 is used for selection-class
  AppClass(0);
  for (bKey = 1; bKey <= dc->iSize(); bKey++)
    AppClass(bKey);
}
  
SmplStats::~SmplStats()
{
  if (0 == rSSptr)
    return;
  byte bKey;
  DelClass(0);
  for (bKey = 1; bKey <= dc->iSize(); bKey++)
    DelClass(bKey);
  delete [] rSSptr;
  delete [] bSSptr;
}
    
void SmplStats::AppClass(byte bKey)
{
  if (0 != rSSptr[bKey]) 
    return;
  int b;
  rSSptr[bKey] = new double*[iNrBands];
  bSSptr[bKey] = new byte[iNrBands];
  for (b = 0; b < iNrBands; b++)
  { 
    rSSptr[bKey][b] = new double[2];
      // init all with 0.
    operator() (bKey, b, CLASSMEAN) = 0;
    operator() (bKey, b, CLASSSTD) = 0;
    bSSptr[bKey][b] = 0;
  }
}

void SmplStats::DelClass(byte bKey)
{
  int b;
  for (b = 0; b < iNrBands; b++)
    delete [] rSSptr[bKey][b];
  delete [] rSSptr[bKey];
  delete [] bSSptr[bKey];
  rSSptr[bKey] = 0;
  bSSptr[bKey] = 0;
}   

void SmplStats::MrgClass(byte bKey1, byte bKey2, 
                         const SmplSum&   sSum, 
                         const SmplSumXY& sSumXY)
{
  if (bKey1 == bKey2) return;
  int b;
  double dSum, dMean, dNum, dStd;
  for (b = 0; b < iNrBands; b++)
  {   
    dNum = sSum(bKey1, iNrBands);
    if (dNum == 0) 
    {
      dMean = 0; 
      dStd = 0;
    }
    else
    {  
      dSum = sSum(bKey1, b);
      dMean = dSum / dNum;
      // because sum over i = 1 to n of (sqr(Xi-M)) ==
      //         sum over i = 1 to n of (srq(Xi)) - n*sqr(M):
      dStd = sqrt((  sSumXY(bKey1, b, b) - dNum* dMean * dMean  ) / 
                                               (dNum - 1));
    }
    operator()(bKey1, b, CLASSMEAN) = dMean;
    bMean(bKey1, b) = byteConv(dMean);
    operator()(bKey1, b, CLASSSTD) = dStd; 
    operator()(bKey2, b, CLASSMEAN) = 0; 
    bMean(bKey2, b) = 0;
    operator()(bKey2, b, CLASSSTD) = 0; 
  }
}    

void SmplStats::ResetClass(byte bKey)
{
  for (int b = 0; b < iNrBands; b++) {
    bMean(bKey, b) = 0;
    operator()(bKey, b, CLASSMEAN) = 0; 
    operator()(bKey, b, CLASSSTD) = 0; 
  }  
} 

/**************/

void SmplSum::Allocate(DomainClass* domc, short iNrBnd)
{
  dc = domc;
  iNrBands = iNrBnd;
  byte bKey;
//  rSumptr = new double*[MAXCLASS+1];   
  iSumptr = new long*[MAXCLASS+1];   
  for (int i=0; i<=MAXCLASS ; i++) 
    iSumptr[i] = 0;
  // index 0 is used for selection-class
  AppClass(0);
  for (bKey = 1; bKey <= dc->iSize(); bKey++)
//{ 
//  bKey = dc->iKey(bOrd);
    AppClass(bKey);
//}  
}
  
SmplSum::~SmplSum()
{
  if (0 == iSumptr)
    return;
  byte bKey;
  DelClass(0);
  for (bKey = 1; bKey <= dc->iSize(); bKey++)
//{ 
//  bKey = dc->iKey(bOrd);
    DelClass(bKey);
//}
//  delete [] rSumptr;
  delete [] iSumptr;
}
       
void SmplSum::AppClass(byte bKey)
{
  if (0 != iSumptr[bKey]) 
    return;
  int b;
  // 1 extra for the total number of pixels.
//  rSumptr[bKey] = new double[iNrBands+1];
  iSumptr[bKey] = new long[iNrBands+1];
  for (b = 0; b <= iNrBands; b++)
    operator() (bKey, b) = 0;
}

void SmplSum::DelClass(byte bKey)
{
//  delete [] rSumptr[bKey];
  delete [] iSumptr[bKey];
  iSumptr[bKey] = 0;
}   

void SmplSum::MrgClass(byte bKey1, byte bKey2) 
{
  if (bKey1 == bKey2) return;
  // all values are gathered in the class with the lowest key
  int b;
  for (b = 0; b <= iNrBands; b++)
    operator() (bKey1, b) += operator() (bKey2, b);
  for (b = 0; b <= iNrBands; b++)
    operator() (bKey2, b) = 0;
}    

void SmplSum::ResetClass(byte bKey) 
{
  int b;
  for (b = 0; b <= iNrBands; b++)
    operator() (bKey, b) = 0;
}    

double SmplSum::rPixInClass(byte bKey) const
{
  return operator()(bKey, iNrBands);
}  
   
/**************/

void SmplSumXY::Allocate(DomainClass* domc, short iNrBnd)
{
  dc = domc;
  iNrBands = iNrBnd;   
  int i;
  byte bKey;
//  rSumptr = new double**[MAXCLASS+1];  
  iSumptr = new long**[MAXCLASS+1];  
  for (i=0; i<=MAXCLASS ; i++) 
    iSumptr[i] = 0;
  // index 0 is used for selection-class
  AppClass(0);
  for (bKey = 1; bKey <= dc->iSize(); bKey++)
//{ 
//  bKey = dc->iKey(bOrd);
    AppClass(bKey);
//}
}
  
SmplSumXY::~SmplSumXY()
{  
  if (0 == iSumptr)
    return;
  byte bKey;
  DelClass(0);
  for (bKey = 1; bKey <= dc->iSize(); bKey++) 
//{ 
//  bKey = dc->iKey(bOrd);
    DelClass(bKey);
//}
//  delete [] rSumptr;
  delete [] iSumptr;
}

void SmplSumXY::AppClass(byte bKey)
{
  if (0 != iSumptr[bKey]) 
    return;
  int b, i;
//  rSumptr[bKey] = new double*[iNrBands];
  iSumptr[bKey] = new long*[iNrBands];
  for (b = 0; b < iNrBands; b++)
  { 
//    rSumptr[bKey][b] = new double[b+1];
    iSumptr[bKey][b] = new long[b+1];
    // init all with 0.
    for (i = 0; i <= b; i++)
      operator() (bKey, b, i) = 0;
  }
}

void SmplSumXY::DelClass(byte bKey)
{
  int b;
  for (b = 0; b < iNrBands; b++)
//    delete [] rSumptr[bKey][b];
    delete [] iSumptr[bKey][b];
//  delete [] rSumptr[bKey];
  delete [] iSumptr[bKey];
  iSumptr[bKey] = 0;
}

void SmplSumXY::MrgClass(byte bKey1, byte bKey2)
{
  if (bKey1 == bKey2) return;
  // all values are gathered in the class with the lowest key
  int b, i;
  for (b = 0; b < iNrBands; b++)
    for (i = 0; i <= b; i++)
      operator() (bKey1, b, i) += operator() (bKey2, b, i);
  for (b = 0; b < iNrBands; b++)
    for (i = 0; i <= b; i++)
      operator() (bKey2, b, i) = 0;
}    

void SmplSumXY::ResetClass(byte bKey)
{
  int b, i;
  for (b = 0; b < iNrBands; b++)
    for (i = 0; i <= b; i++)
      operator() (bKey, b, i) = 0;
}




