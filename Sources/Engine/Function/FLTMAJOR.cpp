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
/* FilterMajority
   Copyright Ilwis System Development ITC
   June 1995, by Jelle Wind
	Last change:  JEL  14 Aug 96    1:09 pm
*/

#include "Engine\Function\FLTMAJOR.H"
#include "Engine\Base\DataObjects\ERR.H"

const char* FilterMajority::sSyntax()
{
  return "Majority(rows,cols)\nUndefMajority(rows,cols),ZeroMajority(rows,cols)";
}

FilterMajority* FilterMajority::create(const FileName& fn, const String& sExpr)
{
  Array<String> as(2);
  String sFunc = IlwisObjectPtr::sParseFunc(sExpr);
  if (!IlwisObjectPtr::fParseParm(sExpr, as))
    ExpressionError(sExpr, sSyntax());
  short iFltRows = as[0].shVal();
  short iFltCols = as[1].shVal();
  if ((iFltRows <= 0) || (iFltCols <= 0))
    ExpressionError(sExpr, sSyntax());
  if (fCIStrEqual(sFunc, "Majority"))
    return new FilterMajority(fn, iFltRows, iFltCols, mtNORMAL);
  else if (fCIStrEqual(sFunc, "UndefMajority"))
    return new FilterMajority(fn, iFltRows, iFltCols, mtUNDEF);
  else if (fCIStrEqual(sFunc, "ZeroMajority"))
    return new FilterMajority(fn, iFltRows, iFltCols, mtZERO);
  else
    NotFoundError(String("Filter: %S", sExpr));

  return NULL;
}

FilterMajority::FilterMajority(const FileName& fn)
: FilterPtr(fn)
{
  String sType;
  ReadElement("FilterMajority", "Type", sType);
  if (fCIStrEqual(sType, "normal")) 
    mt = mtNORMAL;
  else if (fCIStrEqual(sType, "zero"))
    mt = mtZERO;
  else if (fCIStrEqual(sType, "undef")) 
    mt = mtUNDEF;
  Init();
}

FilterMajority::FilterMajority(const FileName& fn, short iRows, short iCols, FilterMajorityType mtp)
: FilterPtr(fn, iRows, iCols), mt(mtp)
{
  Init();
}

FilterMajority::~FilterMajority()
{
}
void FilterMajority::Init()
{
  iFreq.Resize(iFltRows*iFltCols+1);
  iValues.Resize(iFltRows*iFltCols+1); // last index is used as stop criterion
  rValues.Resize(iFltRows*iFltCols+1);
}

void FilterMajority::Store()
{
  FilterPtr::Store();
  WriteElement("Filter", "Type", "FilterMajority");
  switch (mt) {
    case mtNORMAL: 
      WriteElement("FilterMajority", "Type", "normal");
      break;
    case mtUNDEF: 
      WriteElement("FilterMajority", "Type", "undef");
      break;
    case mtZERO: 
      WriteElement("FilterMajority", "Type", "zero");
      break;
    
    default :
      break;
  }
}

String FilterMajority::sName(bool fExt, const String& sDirRelative) const
{
  String s = IlwisObjectPtr::sName(fExt, sDirRelative);
  if (s.length() != 0)
    return s;
  String sType;
  switch (mt) {
    case mtNORMAL: 
      break;
    case mtUNDEF: 
      sType = "Undef";
      break;
    case mtZERO: 
      sType = "Zero";
      break;
  } 
  return String("%SMajority(%i,%i)", sType, iFltRows, iFltCols);
}

void FilterMajority::ExecuteRaw(const LongBufExt* bufListInp, LongBuf& bufRes)
{
  int i, c, cc;
  long iVal;
  int iCenterRow = iFltRows >> 1;
  int iCenterCol = iFltCols >> 1;
  if (mt == mtUNDEF) 
    for (c=0; c < bufRes.iSize(); c++) {
      for (i=0; i<iFreq.iSize(); i++) 
        iFreq[i] = 0;
      iVal = bufListInp[iCenterRow][c]; // center line
      if (iVal != iUNDEF)
        bufRes[c] = iVal;
      else { // determine majority of non undefs
        for (i = 0; i < iFltRows; i++)
          for (cc = c-iCenterCol; cc <= c+iCenterCol; cc++) {
            iVal = bufListInp[i][cc];
            if (iVal != iUNDEF)
              InsertVal(iVal);
          }
        bufRes[c] = iGetMajor();
      }
    }
  else if (mt == mtZERO)
    for (c=0; c < bufRes.iSize(); c++) {
      for (i=0; i<iFreq.iSize(); i++) 
        iFreq[i] = 0;
      iVal = bufListInp[iCenterRow][c]; // center line
      if (iVal != 0)
        bufRes[c] = iVal;
      else { // determine majority of non undefs
        for (i = 0; i < iFltRows; i++)
          for (cc = c-iCenterCol; cc <= c+iCenterCol; cc++) {
            iVal = bufListInp[i][cc];
            if (iVal != 0)
              InsertVal(iVal);
          }
        bufRes[c] = iGetMajor();
      }
    }
  else {
    for (c=0; c < bufRes.iSize(); c++) {
      for (i=0; i<iFreq.iSize(); i++) 
        iFreq[i] = 0;
      InsertVal(bufListInp[iCenterRow][c]); // insert center pixel first, so it gets priority
      for (i = 0; i < iFltRows; i++)
        for (cc = c-(iFltCols>>1); cc <= c+(iFltCols>>1); cc++)
          if ((i != iCenterRow) || (cc != c)) // don't insert center pixel again
            InsertVal(bufListInp[i][cc]);
      bufRes[c] = iGetMajor();
    }
  }
}

void FilterMajority::ExecuteVal(const RealBufExt* bufListInp, RealBuf& bufRes)
{
  unsigned int i;
  int c, cc;
  for (i=0; i<iFreq.iSize(); i++) 
    iFreq[i] = 0;
  double rVal;
  int iCenterRow = iFltRows >> 1;
  int iCenterCol = iFltCols >> 1;
  if (mt == mtUNDEF) 
    for (c=0; c < bufRes.iSize(); c++) {
      for (i=0; i<iFreq.iSize(); i++) 
        iFreq[i] = 0;
      rVal = bufListInp[iCenterRow][c];
      if (rVal != rUNDEF)
        bufRes[c] = rVal;
      else { // determine majority of non undefs
        for (i = 0; i < iFltRows; i++)
          for (cc = c-iCenterCol; cc <= c+iCenterCol; cc++) {
            rVal = bufListInp[i][cc];
            if (rVal != rUNDEF)
              InsertVal(rVal);
          }
        bufRes[c] = rGetMajor();
      }
    }
  else if (mt == mtZERO) 
    for (c=0; c < bufRes.iSize(); c++) {
      for (i=0; i<iFreq.iSize(); i++) 
        iFreq[i] = 0;
      rVal = bufListInp[iCenterRow][c];
      if (rVal != 0)
        bufRes[c] = rVal;
      else { // determine majority of non undefs
        for (i = 0; i < iFltRows; i++)
          for (cc = c-iCenterCol; cc <= c+iCenterCol; cc++) {
            rVal = bufListInp[i][cc];
            if (rVal != 0)
              InsertVal(rVal);
          }
        bufRes[c] = rGetMajor();
      }
    }
  else 
    for (c=0; c < bufRes.iSize(); c++) {
      for (i=0; i<iFreq.iSize(); i++) 
        iFreq[i] = 0;
      InsertVal(bufListInp[iCenterRow][c]);  // insert center pixel firs, so it gets priorityt
      for (i = 0; i < iFltRows; i++)
        for (cc = c-iCenterCol; cc <= c+iCenterCol; cc++)
          if ((i != iCenterRow) || (cc != c)) // don't insert center pixel again
            InsertVal(bufListInp[i][cc]);
      bufRes[c] = rGetMajor();
    }
}

void FilterMajority::InsertVal(long iVal)
{
  int i = 0;
  while ((iFreq[i] != 0) && (iValues[i] != iVal))
    i++;
  if (iFreq[i] == 0)
    iValues[i] = iVal;
  iFreq[i]++;
}

void FilterMajority::InsertVal(double rVal)
{
  int i = 0;
  while ((iFreq[i] != 0) && (rValues[i] != rVal))
    i++;
  if (iFreq[i] == 0)
    rValues[i] = rVal;
  iFreq[i]++;
}

long FilterMajority::iGetMajor()
{
  int iMaj = 0, i=1;
  if (iFreq[0] == 0)
    return iUNDEF;
  while (iFreq[i] != 0) {
    if (iFreq[i] > iFreq[iMaj])
      iMaj = i;
    i++;
  }
  return iValues[iMaj];
}

double FilterMajority::rGetMajor()
{
  int iMaj = 0, i=1;
  if (iFreq[0] == 0)
    return rUNDEF;
  while (iFreq[i] != 0) {
    if (iFreq[i] > iFreq[iMaj])
      iMaj = i;
    i++;
  }
  return rValues[iMaj];
}




