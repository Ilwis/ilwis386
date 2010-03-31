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
// $Log: FLTPATT.C $
// Revision 1.3  1998/09/16 17:30:33  Wim
// 22beta2
//
// Revision 1.2  1997/09/03 11:02:57  Wim
// dmDefault() now checks if input map has value domain
//
/* FilterPattern
   Copyright Ilwis System Development ITC
   august 1995, by Jelle Wind
	Last change:  WK    3 Sep 97    1:02 pm
*/

#include "Engine\Function\FLTPATT.H"
#include "Engine\Base\DataObjects\valrange.h"
#include "Engine\Base\DataObjects\ERR.H"
#include "Engine\Map\Raster\Map.h"

const char* FilterPattern::sSyntax()
{
  return "Pattern(threshold)";
}

FilterPattern* FilterPattern::create(const FileName& fn, const String& sExpr)
{
  Array<String> as(1);
  if (!IlwisObjectPtr::fParseParm(sExpr, as))
    ExpressionError(sExpr, sSyntax());
  double rThresh = as[0].rVal();
  return new FilterPattern(fn, rThresh);
}

FilterPattern::FilterPattern(const FileName& fn)
: FilterPtr(fn)
{
  rThreshold = rReadElement("FilterPattern", "Threshold");
}

FilterPattern::FilterPattern(const FileName& fn, double rThres)
: FilterPtr(fn, 3, 3, Domain("image")), rThreshold(rThres)
{
}

FilterPattern::~FilterPattern()
{
}

void FilterPattern::Store()
{
  FunctionPtr::Store();
  WriteElement("Filter", "Type", "FilterPattern");
  WriteElement("FilterPattern", "Threshold", rThreshold);
}

Domain FilterPattern::dmDefault(const Map& mp) const
{
  if (!mp->fValues())
    return Domain();  // pattern filter not defined for non value maps
  else
    return Domain("image");
}

ValueRange FilterPattern::vrDefault(const Map&, const Domain&) const
{
  return ValueRange(0,255,1);
}

bool FilterPattern::fRawAllowed() const
{
  return false;
}

String FilterPattern::sName(bool fExt, const String& sDirRelative) const
{
  String s = IlwisObjectPtr::sName(fExt, sDirRelative);
  if (s.length() != 0)
    return s;
  return String("Pattern(%f)", rThreshold);
}

void FilterPattern::ExecuteRaw(const LongBufExt* /*bufListInp*/, LongBuf& bufRes)
{
  for (int i=0; i < bufRes.iSize(); i++) {
    bufRes[i] = iUNDEF;
  }
}

void FilterPattern::ExecuteVal(const RealBufExt* bufListInp, RealBuf& bufRes)
{
  int c;
  double rCenterVal;
  byte bVal;
  for (c = 0; c < bufRes.iSize() ; c++) {
    rCenterVal = bufListInp[1][c];
    if (rCenterVal == rUNDEF) {
      bufRes[c] = rUNDEF;
      continue;
    } 
    bVal = 0;
    if (fabs(bufListInp[1][c+1] - rCenterVal) <= rThreshold)
      bVal |= 1;
    if (fabs(bufListInp[0][c+1] - rCenterVal) <= rThreshold)
      bVal |= 2;
    if (fabs(bufListInp[0][c] - rCenterVal) <= rThreshold)
      bVal |= 4;
    if (fabs(bufListInp[0][c-1] - rCenterVal) <= rThreshold)
      bVal |= 8;
    if (fabs(bufListInp[1][c-1] - rCenterVal) <= rThreshold)
      bVal |= 16;
    if (fabs(bufListInp[2][c-1] - rCenterVal) <= rThreshold)
      bVal |= 32;
    if (fabs(bufListInp[2][c] - rCenterVal) <= rThreshold)
      bVal |= 64;
    if (fabs(bufListInp[2][c+1] - rCenterVal) <= rThreshold)
      bVal |= 128;
    bufRes[c] = (double)bVal;
  }
}




