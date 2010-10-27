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
/* FilterStandardDev
   Copyright Ilwis System Development ITC
   June 1996, by Jelle Wind
	Last change:  JEL  29 May 97   12:24 pm
*/

#include "Engine\Function\FLTSTDEV.H"
#include "Engine\Map\Raster\Map.h"
#include "Engine\Base\DataObjects\ERR.H"

FilterStandardDev::FilterStandardDev(const FileName& fn)
: FilterPtr(fn)
{
}

FilterStandardDev::FilterStandardDev(const FileName& fn, int iRows, int iCols)
: FilterPtr(fn, iRows, iCols)
{
  SetDomainChangeable(true);
  SetValueRangeChangeable(true);
}

FilterStandardDev* FilterStandardDev::create(const FileName& fn, const String& sExpr)
{
  Array<String> as;
  int iParms = IlwisObjectPtr::iParseParm(sExpr, as);
  if (iParms != 2)
    ExpressionError(sExpr, sSyntax());
  int iFltRows = as[0].shVal();
  int iFltCols = as[1].shVal();
  if ((iFltRows <= 0) || (iFltCols <= 0))
    ExpressionError(sExpr, sSyntax());
  return new FilterStandardDev(fn, iFltRows, iFltCols);
}

FilterStandardDev::~FilterStandardDev()
{
}

void FilterStandardDev::Store()
{
  FilterPtr::Store();
  WriteElement("Filter", "Type", "FilterStandardDev");
}

bool FilterStandardDev::fRawAllowed() const
{
  return false;
}

void FilterStandardDev::ExecuteRaw(const LongBufExt* /*bufListInp*/, LongBuf& bufRes)
{
  for (int i=0; i < bufRes.iSize() ; i++) {
    bufRes[i] = iUNDEF;
  }
}

double FilterStandardDev::rCalcStd(const RealBufExt* bufLstInp, int c)
{
  double r;
  int cc;
  double rMean = 0, rStd = 0;
  for (int i = 0; i < iFltRows ; i++)
    for (cc = c-(iFltCols>>1); cc <= c+(iFltCols>>1) ; cc++) {
      r = bufLstInp[i][cc];
      if (r == rUNDEF)
        return rUNDEF;
      rMean += r;
    }
  rMean /= iFltRows*iFltCols;
  for (int i = 0; i < iFltRows ; i++)
    for (cc = c-(iFltCols>>1); cc <= c+(iFltCols>>1) ; cc++) {
      r = bufLstInp[i][cc];
      rStd += sqr(rMean - r);
    }
  rStd = sqrt(rStd/(iFltCols*iFltRows-1));
  return rStd;
}

void FilterStandardDev::ExecuteVal(const RealBufExt* bufLstInp, RealBuf& bufRes)
{
  double r;
  for (int c = 0; c < bufRes.iSize() ; c++) {
    r = bufLstInp[iFltRows>>1][c];
    if (r == rUNDEF) {
      bufRes[c] = rUNDEF;
      continue;
    }
    bufRes[c] = rCalcStd(bufLstInp, c);
  }
}

String FilterStandardDev::sName(bool fExt, const String& sDirRelative) const
{
  String s = IlwisObjectPtr::sName(fExt, sDirRelative);
  if (s.length() != 0)
    return s;
  return String("FilterStandardDev(%i,%i)", iFltRows, iFltCols);
}

const char* FilterStandardDev::sSyntax()
{
  return "FilterStandardDev(rows,cols)";
}

Domain FilterStandardDev::dmDefault(const Map& ) const
{
  return Domain("value");
}

ValueRange FilterStandardDev::vrDefault(const Map& mp, const Domain&) const
{
  RangeReal rr = mp->rrMinMax();
  if (!rr.fValid())
    rr = mp->dvrs().rrMinMax();
  // calc extremes
  RealBuf bufRes(1);
  RealBufExt* bufListVal = new RealBufExt[iFltRows];
  for (int i=0; i < iFltRows; i++) 
    bufListVal[i].Size(1, iFltCols>>1, iFltCols>>1);
  // try kernel with max in center, surrounded by min
  for (int i=0; i < iFltRows; i++) 
    for (int j = -(iFltCols>>1); j <= (iFltCols>>1) ; j++) 
      bufListVal[i][j] = rr.rLo();
  bufListVal[iFltRows>>1][0] = rr.rHi();
  const_cast<FilterStandardDev *>(this)->ExecuteVal(bufListVal, bufRes);
  return ValueRange(0, bufRes[0], 0.1);
}




