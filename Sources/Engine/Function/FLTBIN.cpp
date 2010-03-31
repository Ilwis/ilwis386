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
/* FilterBinary
   Copyright Ilwis System Development ITC
   august 1995, by Jelle Wind
	Last change:  JEL  28 Oct 96   10:32 am
*/

#include "Engine\Function\FLTBIN.H"
#include "Engine\Base\DataObjects\ERR.H"

FilterBinary::FilterBinary(const FileName& fn)
: FilterPtr(fn)
{
  int i;
  String s;
  ReadElement("FilterBinary", "0-63", s);
  for (i=0; i <= 63; i++)
    fBinLut[i] = s[i] == '1';
  ReadElement("FilterBinary", "64-127", s);
  for (i=64; i <= 127; i++)
    fBinLut[i] = s[i-64] == '1';
  ReadElement("FilterBinary", "128-191", s);
  for (i=128; i <= 191; i++)
    fBinLut[i] = s[i-128] == '1';
  ReadElement("FilterBinary", "192-255", s);
  for (i=192; i <= 255; i++)
    fBinLut[i] = s[i-192] == '1';
  ReadElement("FilterBinary", "256-319", s);
  for (i=256; i <= 319; i++)
    fBinLut[i] = s[i-256] == '1';
  ReadElement("FilterBinary", "320-383", s);
  for (i=320; i <= 383; i++)
    fBinLut[i] = s[i-320] == '1';
  ReadElement("FilterBinary", "384-447", s);
  for (i=384; i <= 447; i++)
    fBinLut[i] = s[i-384] == '1';
  ReadElement("FilterBinary", "448-511", s);
  for (i=448; i <= 511; i++)
    fBinLut[i] = s[i-448] == '1';
}

FilterBinary::FilterBinary(const FileName& fn, const String& sBinVals)
: FilterPtr(fn, 3, 3, Domain("bool"))
{
  for (int i=0; i <= 511; i++)
    fBinLut[i] = sBinVals[i] == '1';
}

FilterBinary::~FilterBinary()
{
}

void FilterBinary::Store()
{
  FunctionPtr::Store();
  String s;
  for (int i=0; i <= 511; i++)
    s &= fBinLut[i] ? '1' : 0;
  WriteElement("Filter", "Type", "FilterBinary");
  WriteElement("FilterBinary", "0-63", s.sLeft(64));
  WriteElement("FilterBinary", "64-127", s.sSub(64, 64));
  WriteElement("FilterBinary", "128-191", s.sSub(128, 64));
  WriteElement("FilterBinary", "192-255", s.sSub(192,64));
  WriteElement("FilterBinary", "256-319", s.sSub(256, 64));
  WriteElement("FilterBinary", "320-383", s.sSub(320, 64));
  WriteElement("FilterBinary", "384-447", s.sSub(384, 64));
  WriteElement("FilterBinary", "448-511", s.sRight(64));
}

bool FilterBinary::fRawAllowed() const
{
  return true;
}

String FilterBinary::sName(bool fExt, const String& sDirRelative) const
{
  String s = IlwisObjectPtr::sName(fExt, sDirRelative);
  if (s.length() != 0)
    return s;
  return String("Binary(3,3)");
}

void FilterBinary::ExecuteRaw(const LongBufExt* bufListInp, LongBuf& bufRes)
{
  int iLutInd, c;
  for (c = 0; c < bufRes.iSize() ; c++) {
    iLutInd = 0;
    if (bufListInp[1][c+1] != iUNDEF && bufListInp[1][c+1] != 0)
      iLutInd |= 1;
    if (bufListInp[0][c+1] != iUNDEF && bufListInp[0][c+1] != 0)
      iLutInd |= 2;
    if (bufListInp[0][c]   != iUNDEF && bufListInp[0][c] != 0)
      iLutInd |= 4;
    if (bufListInp[0][c-1] != iUNDEF && bufListInp[0][c-1] != 0)
      iLutInd |= 8;
    if (bufListInp[1][c-1] != iUNDEF && bufListInp[1][c-1] != 0)
      iLutInd |= 16;
    if (bufListInp[2][c-1] != iUNDEF && bufListInp[2][c-1] != 0)
      iLutInd |= 32;
    if (bufListInp[2][c]   != iUNDEF && bufListInp[2][c] != 0)
      iLutInd |= 64;
    if (bufListInp[2][c+1] != iUNDEF && bufListInp[2][c+1] != 0)
      iLutInd |= 128;
    if (bufListInp[1][c]   != iUNDEF && bufListInp[1][c] != 0)
      iLutInd |= 256;
    bufRes[c] = fBinLut[iLutInd];
  }
}

void FilterBinary::ExecuteVal(const RealBufExt* bufListInp, RealBuf& bufRes)
{
  int iLutInd, c;
  for (c = 0; c < bufRes.iSize() ; c++) {
    if (bufListInp[1][c] == rUNDEF) {
      bufRes[c] = rUNDEF;
      continue;
    }
    iLutInd = 0;
    if (bufListInp[1][c+1] != 0 && bufListInp[1][c+1] != rUNDEF)
      iLutInd |= 1;
    if (bufListInp[0][c+1] != 0 && bufListInp[0][c+1] != rUNDEF)
      iLutInd |= 2;
    if (bufListInp[0][c]   != 0 && bufListInp[0][c]   != rUNDEF)
      iLutInd |= 4;
    if (bufListInp[0][c-1] != 0 && bufListInp[0][c-1] != rUNDEF)
      iLutInd |= 8;
    if (bufListInp[1][c-1] != 0 && bufListInp[1][c-1] != rUNDEF)
      iLutInd |= 16;
    if (bufListInp[2][c-1] != 0 && bufListInp[2][c-1] != rUNDEF)
      iLutInd |= 32;
    if (bufListInp[2][c]   != 0 && bufListInp[2][c]   != rUNDEF)
      iLutInd |= 64;
    if (bufListInp[2][c+1] != 0 && bufListInp[2][c+1] != rUNDEF)
      iLutInd |= 128;
    if (bufListInp[1][c]   != 0 && bufListInp[1][c]   != rUNDEF)
      iLutInd |= 256;
    bufRes[c] = fBinLut[iLutInd];
  }
}




