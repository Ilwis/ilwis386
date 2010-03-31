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
/* FilterRankOrder
   Copyright Ilwis System Development ITC
   June 1995, by Jelle Wind
	Last change:  WK   28 Aug 97    1:03 pm
*/

#include "Engine\Function\FLTRANK.H"
#include "Engine\Base\Algorithm\Qsort.h"
#include "Engine\Base\DataObjects\ERR.H"
#include "Headers\Err\ILWISDAT.ERR"
#include "Engine\Map\Raster\Map.h"
#include "Headers\Hs\DAT.hs"

const char* FilterRankOrder::sSyntax()
{
  return "RankOrder(rows,cols,rank)\nRankOrder(rows,cols,rank,threshold)";
}

FilterRankOrder* FilterRankOrder::create(const FileName& fn, const String& sExpr)
{
  Array<String> as;
  int iParms = IlwisObjectPtr::iParseParm(sExpr, as);
  if (iParms < 3 || iParms > 4)
    ExpressionError(sExpr, sSyntax());
  short iFltRows = as[0].shVal();
  short iFltCols = as[1].shVal();
//  if ((iFltRows <= 0) || (iFltCols <= 0))
//    ExpressionError(sExpr, sSyntax());
  short iRank = as[2].shVal()-1;
  double rThreshold = rUNDEF;
  if (iParms == 4)
    rThreshold = as[3].rVal();
  return new FilterRankOrder(fn, iFltRows, iFltCols, iRank, rThreshold);
}

FilterRankOrder* FilterRankOrder::create(const FileName& fn)
{

  String sType;
  ObjectInfo::ReadElement("FilterRankOrder", "Type", fn, sType);
  if (fCIStrEqual(sType, "FilterMedian"))
    return new FilterMedian(fn);
  return new FilterRankOrder(fn);
//  throw ErrorInvalidType(fn, "FilterRankOrder", sType);
}

FilterRankOrder::FilterRankOrder(const FileName& fn)
: FilterPtr(fn)
{
  iRank = (short)iReadElement("FilterRankOrder", "Rank");
  iRank--; // for computation rank order starts at zero
  rThreshold = rReadElement("FilterRankOrder", "Threshold");
  rSorted.Resize(iFltRows*iFltCols);
}

FilterRankOrder::FilterRankOrder(const FileName& fn, short iRows, short iCols, 
                                 short iRankOrder, double rThresh)
: FilterPtr(fn, iRows, iCols), iRank(iRankOrder), rThreshold(rThresh)
{
 if ((rThreshold <= 0) && (rThreshold != rUNDEF))
   Filter::ErrorThreshold(rThreshold, sTypeName());
 if ((iRank < 0) || (iRank >= iCols*iRows))
   throw ErrorObject(WhatError(String("%S 1..%i: %i", SDATErrRankOutOfRange, iCols*iRows, iRank), errFilter+3), sTypeName());
 rSorted.Resize(iFltRows*iFltCols);
}

FilterRankOrder::~FilterRankOrder()
{
}

void FilterRankOrder::Store()
{
  FunctionPtr::Store();
  WriteElement("Filter", "Type", "FilterRankOrder");
  WriteElement("FilterRankOrder", "Rank", (long)iRank+1); 
  // for computation rankorder starts at zero
  if (rThreshold != rUNDEF)
    WriteElement("FilterRankOrder", "Threshold", rThreshold);
  else
    WriteElement("FilterRankOrder", "Threshold", (const char*)0);
}

bool FilterRankOrder::fRawAllowed() const
{
  return false;
}

String FilterRankOrder::sName(bool fExt, const String& sDirRelative) const
{
  String s = IlwisObjectPtr::sName(fExt, sDirRelative);
  if (s.length() != 0)
    return s;
  if (rThreshold != rUNDEF)
    return String("RankOrder(%i,%i,%i,%f)", iFltRows, iFltCols, iRank+1, rThreshold);
    // for computation rankorder starts at zero
  else  
    return String("RankOrder(%i,%i,%i)", iFltRows, iFltCols, iRank+1);
    // for computation rankorder starts at zero
}

static bool fSortComp(long i, long j, void *p)
{
  double* rSorted = static_cast<double*>(p);
  return rSorted[i] < rSorted[j];
}

static void SortSwap(long i, long j, void *p)
{
  double* rSorted = static_cast<double*>(p);
  double r = rSorted[i];
  rSorted[i] = rSorted[j];
  rSorted[j] = r;
}

void FilterRankOrder::ExecuteRaw(const LongBufExt* /*bufListInp*/, LongBuf& bufRes)
{
  for (int i=0; i < bufRes.iSize(); i++) {
    bufRes[i] = iUNDEF;
  }
}

void FilterRankOrder::ExecuteVal(const RealBufExt* bufLstInp, RealBuf& bufRes)
{
  double rCenterVal;
  int i, k, c, cc;
  for (c = 0; c < bufRes.iSize() ; c++) {
    k = 0;
    for (i = 0; i < iFltRows ; i++)
      for (cc = c-(iFltCols>>1); cc <= c+(iFltCols>>1) ; cc++, k++)
        rSorted[k] = bufLstInp[i][cc];
    if (rThreshold != rUNDEF) {
      rCenterVal = rSorted[iRank]; // center
      QuickSort(0, k-1, fSortComp, SortSwap, rSorted.buf());
//                (IlwisObjectPtrLessProc)&FilterRankOrder::fSortComp, 
//                (IlwisObjectPtrSwapProc)&FilterRankOrder::SortSwap);  
      if (fabs(rCenterVal - rSorted[iRank]) <= rThreshold)
        bufRes[c] = rSorted[iRank];
      else
        bufRes[c] = rCenterVal;
    }
    else {  
      QuickSort(0, k-1, fSortComp, SortSwap, rSorted.buf());
//                (IlwisObjectPtrLessProc)&FilterRankOrder::fSortComp, 
//                (IlwisObjectPtrSwapProc)&FilterRankOrder::SortSwap);  
      bufRes[c] = rSorted[iRank];
    }  
  }
}

Domain FilterRankOrder::dmDefault(const Map& mp) const
{
  if (!mp->fValues())
    return Domain();  // linear filter not defined for non value maps
  else
    return mp->dm();
}

const char* FilterMedian::sSyntax()
{
  return "Median(rows,cols)\nMedian(rows,cols,threshold)";
}

FilterMedian* FilterMedian::create(const FileName& fn, const String& sExpr)
{
  Array<String> as;
  int iParms = IlwisObjectPtr::iParseParm(sExpr, as);
  if (iParms < 2 || iParms > 3)
    ExpressionError(sExpr, sSyntax());
  short iFltRows = as[0].shVal();
  short iFltCols = as[1].shVal();
  if ((iFltRows <= 0) || (iFltCols <= 0))
    ExpressionError(sExpr, sSyntax());
  double rThreshold = rUNDEF;
  if (iParms == 3)
    rThreshold = as[2].rVal();
  return new FilterMedian(fn, iFltRows, iFltCols, rThreshold);
}

FilterMedian::FilterMedian(const FileName& fn)
: FilterRankOrder(fn)
{
  iRank = iFltRows * iFltCols / 2;
}

FilterMedian::FilterMedian(const FileName& fn, short iRows, short iCols, double rThreshold)
: FilterRankOrder(fn, iRows, iCols, iRows * iCols / 2, rThreshold)
{
}

FilterMedian::~FilterMedian()
{
}

void FilterMedian::Store()
{
  FilterRankOrder::Store();
  WriteElement("FilterRankOrder", "Type", "FilterMedian");
}  

String FilterMedian::sName(bool fExt, const String& sDirRelative) const
{
  String s = IlwisObjectPtr::sName(fExt, sDirRelative);
  if (s.length() != 0)
    return s;
  if (rThreshold != rUNDEF)
    return String("Median(%i,%i,%f)", iFltRows, iFltCols, rThreshold);
  else  
    return String("Median(%i,%i)", iFltRows, iFltCols);
}






