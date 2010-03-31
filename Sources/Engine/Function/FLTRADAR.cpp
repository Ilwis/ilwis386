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
/* FilterRadar
   Copyright Ilwis System Development ITC
   June 1996, by Jelle Wind
	Last change:  JEL  29 May 97   12:24 pm
*/

#include "Engine\Function\FLTRADAR.H"
#include "Engine\Base\DataObjects\ERR.H"

FilterRadar* FilterRadar::create(const FileName& fn)
{
  String sType;
  ObjectInfo::ReadElement("FilterRadar", "Type", fn, sType);
  if (fCIStrEqual(sType,"FilterLee"))
    return new FilterLee(fn);
  if (fCIStrEqual(sType,"FilterLeeEnhanced"))
    return new FilterLeeEnhanced(fn);
  if (fCIStrEqual(sType,"FilterKuan"))
    return new FilterKuan(fn);
  if (fCIStrEqual(sType, "FilterFrost"))
    return new FilterFrost(fn);
  if (fCIStrEqual(sType, "FilterFrostEnhanced"))
    return new FilterFrostEnhanced(fn);
  if (fCIStrEqual(sType, "FilterGammaMap"))
    return new FilterGammaMap(fn);
  InvalidTypeError(fn, "FilterRadar", sType);
  return 0;
}

FilterRadar::FilterRadar(const FileName& fn)
: FilterPtr(fn)
{

  ReadElement("FilterRadar", "NrLooks", iNrLooks);
  ReadElement("FilterRadar", "Intensity", fIntensity);
  ReadElement("FilterRadar", "DampFactor", rDampFactor);
  if (fIntensity)
    cu = 1.0 / sqrt((double)iNrLooks);
  else 
    cu = 0.523 / sqrt((double)iNrLooks);
}

FilterRadar::FilterRadar(const FileName& fn, int iRows, int iCols, long iNrLks, bool fIntens, double rDampFac)
: FilterPtr(fn, iRows, iCols)
{
  SetDomainChangeable(true);
  SetValueRangeChangeable(true);
  iNrLooks = iNrLks;
  fIntensity = fIntens;
  if (fIntensity)
    cu = 1.0 / sqrt((double)iNrLooks);
  else 
    cu = 0.523 / sqrt((double)iNrLooks);
  rDampFactor = rDampFac;
}


FilterRadar::~FilterRadar()
{
}

void FilterRadar::Store()
{
  FilterPtr::Store();
  WriteElement("Filter", "Type", "FilterRadar");
  WriteElement("FilterRadar", "NrLooks", iNrLooks);
}

bool FilterRadar::fRawAllowed() const
{
  return false;
}

void FilterRadar::ExecuteRaw(const LongBufExt* /*bufListInp*/, LongBuf& bufRes)
{
  for (int i=0; i < bufRes.iSize() ; i++) {
    bufRes[i] = iUNDEF;
  }
}

void FilterRadar::CalcMeanStd(const RealBufExt* bufLstInp, int c)
{
  double r;
  int cc;
  rMean = rStd = 0;
  for (int i = 0; i < iFltRows ; i++)
    for (cc = c-(iFltCols>>1); cc <= c+(iFltCols>>1) ; cc++) {
      r = bufLstInp[i][cc];
      if (r == rUNDEF) {
        rStdMeanRatio = rUNDEF; return;
      }  
      rMean += r;
    }
  rMean /= iFltRows*iFltCols;
  for (int i = 0; i < iFltRows ; i++)
    for (cc = c-(iFltCols>>1); cc <= c+(iFltCols>>1) ; cc++) {
      r = bufLstInp[i][cc];
      rStd += (rMean - r) * (rMean - r);
    }
  rStd = sqrt(rStd/(iFltCols*iFltRows-1));
  if (rMean == 0)
    rStdMeanRatio = rUNDEF;
  else if (rStd == 0)
    rStdMeanRatio = rUNDEF;
  else
    rStdMeanRatio = rStd / rMean;
}

int FilterRadar::iParse(const String& sExpr, int& iRows, int& iCols, int& iNrLooks, bool& fIntens, double& rDampFact, double& k1)
{
  Array<String> as;
  int iParms = IlwisObjectPtr::iParseParm(sExpr, as);
  if (iParms < 3)
    return iParms;
  iRows = as[0].shVal();
  iCols = as[1].shVal();
  iNrLooks = 1;
  if (iParms >= 3)
    iNrLooks = as[2].shVal();
  fIntens = true;
  if (iParms >= 4)
    fIntens = as[3]=="Yes";
  rDampFact = 1;
  if (iParms == 5)
    rDampFact = as[4].rVal();
  k1 = 1;
  if (iParms == 6)
    k1 = as[5].rVal();
  return iParms;
}

//// Lee Filter

const char* FilterLee::sSyntax()
{
  return "RankLee(rows,cols,looks,intens,dampfac)";
}

FilterLee::FilterLee(const FileName& fn, int iRows, int iCols, long iNrLks, bool fIntens, double rDampFac)
: FilterRadar(fn, iRows, iCols, iNrLks, fIntens, rDampFac)
{
}


FilterLee::FilterLee(const FileName& fn)
: FilterRadar(fn)
{
}

FilterLee::~FilterLee()
{
}

FilterLee* FilterLee::create(const FileName& fn, const String& sExpr)
{
  int iFltRows, iFltCols;
  int iNrLooks;
  bool fIntens;
  double rDampFact, k1;
  int iParms = iParse(sExpr, iFltRows, iFltCols, iNrLooks, fIntens, rDampFact, k1);
  if (iParms < 2 || iParms > 5)
    ExpressionError(sExpr, sSyntax());
  if ((iFltRows <= 0) || (iFltCols <= 0))
    ExpressionError(sExpr, sSyntax());
  return new FilterLee(fn, iFltRows, iFltCols, iNrLooks, fIntens, rDampFact);
}

void FilterLee::Store()
{
  FilterRadar::Store();
  WriteElement("FilterRadar", "Type", "FilterLee");
}

void FilterLee::ExecuteVal(const RealBufExt* bufLstInp, RealBuf& bufRes)
{
  double r, w;
  for (int c = 0; c < bufRes.iSize() ; c++) {
    r = bufLstInp[iFltRows>>1][c];
    if (r == rUNDEF) {
      bufRes[c] = rUNDEF;
      continue;
    }
    CalcMeanStd(bufLstInp, c);
    if (rStdMeanRatio == rUNDEF) {
      bufRes[c] = rUNDEF;
      continue;
    }
    if (rStdMeanRatio <= cu)
      bufRes[c] = rMean;
    else {
		w = cu * cu /( rStdMeanRatio * rStdMeanRatio);
      bufRes[c] =  r * (1 - w) + rMean * w;
    }
  }
}

String FilterLee::sName(bool fExt, const String& sDirRelative) const
{
  String s = IlwisObjectPtr::sName(fExt, sDirRelative);
  if (s.length() != 0)
    return s;
  return String("FilterLee(%i,%i,%li,%s,%g)", iFltRows, iFltCols, iNrLooks, pcYesNo(fIntensity), rDampFactor);
}

//// LeeEnhanced Filter

const char* FilterLeeEnhanced::sSyntax()
{
  return "FilterLeeEnhanced(rows,cols,looks,intens,dampfac)";
}

FilterLeeEnhanced::FilterLeeEnhanced(const FileName& fn, int iRows, int iCols, long iNrLks, bool fIntens, double rDampFac)
: FilterRadar(fn, iRows, iCols, iNrLks, fIntens, rDampFac)
{
  cmax = sqrt(1+2.0/iNrLooks);
}


FilterLeeEnhanced::FilterLeeEnhanced(const FileName& fn)
: FilterRadar(fn)
{
  cmax = sqrt(1+2.0/iNrLooks);
}

FilterLeeEnhanced::~FilterLeeEnhanced()
{
}

FilterLeeEnhanced* FilterLeeEnhanced::create(const FileName& fn, const String& sExpr)
{
  int iFltRows, iFltCols;
  int iNrLooks;
  bool fIntens;
  double rDampFact, k1;
  int iParms = iParse(sExpr, iFltRows, iFltCols, iNrLooks, fIntens, rDampFact, k1);
  if (iParms < 2 || iParms > 5)
    ExpressionError(sExpr, sSyntax());
  if ((iFltRows <= 0) || (iFltCols <= 0))
    ExpressionError(sExpr, sSyntax());
  return new FilterLeeEnhanced(fn, iFltRows, iFltCols, iNrLooks, fIntens, rDampFact);
}

void FilterLeeEnhanced::Store()
{
  FilterRadar::Store();
  WriteElement("FilterRadar", "Type", "FilterLeeEnhanced");
}

void FilterLeeEnhanced::ExecuteVal(const RealBufExt* bufLstInp, RealBuf& bufRes)
{
  double r, w;
  for (int c = 0; c < bufRes.iSize() ; c++) {
    r = bufLstInp[iFltRows>>1][c];
    if (r == rUNDEF) {
      bufRes[c] = rUNDEF;
      continue;
    }
    CalcMeanStd(bufLstInp, c);
    if (rStdMeanRatio == rUNDEF) {
      bufRes[c] = rUNDEF;
      continue;
    }
    if (rStdMeanRatio <= cu)
      bufRes[c] = rMean;
    else if (rStdMeanRatio >= cmax)
      bufRes[c] = r;
    else   {
      w = exp(-rDampFactor*(rStdMeanRatio-cu)/(cmax-rStdMeanRatio));
      bufRes[c] =  r * w + rMean * (1 - w);
    }
  }
}

String FilterLeeEnhanced::sName(bool fExt, const String& sDirRelative) const
{
  String s = IlwisObjectPtr::sName(fExt, sDirRelative);
  if (s.length() != 0)
    return s;
  return String("FilterLeeEnhanced(%i,%i,%li,%s,%g)", iFltRows, iFltCols, iNrLooks, pcYesNo(fIntensity), rDampFactor);
}

//// Kuan Filter

const char* FilterKuan::sSyntax()
{
  return "FilterKuan(rows,cols,looks,intens,dampfac)";
}

FilterKuan::FilterKuan(const FileName& fn, int iRows, int iCols, long iNrLks, bool fIntens, double rDampFac)
: FilterRadar(fn, iRows, iCols, iNrLks, fIntens, rDampFac)
{
  cu2 = cu * cu;
}


FilterKuan::FilterKuan(const FileName& fn)
: FilterRadar(fn)
{
  cu2 =cu * cu;
}

FilterKuan::~FilterKuan()
{
}

FilterKuan* FilterKuan::create(const FileName& fn, const String& sExpr)
{
  int iFltRows, iFltCols;
  int iNrLooks;
  bool fIntens;
  double rDampFact, k1;
  int iParms = iParse(sExpr, iFltRows, iFltCols, iNrLooks, fIntens, rDampFact, k1);
  if (iParms < 2 || iParms > 5)
    ExpressionError(sExpr, sSyntax());
  if ((iFltRows <= 0) || (iFltCols <= 0))
    ExpressionError(sExpr, sSyntax());
  return new FilterKuan(fn, iFltRows, iFltCols, iNrLooks, fIntens, rDampFact);
}

void FilterKuan::Store()
{
  FilterRadar::Store();
  WriteElement("FilterRadar", "Type", "FilterKuan");
}

void FilterKuan::ExecuteVal(const RealBufExt* bufLstInp, RealBuf& bufRes)
{
  double r, w;
  for (int c = 0; c < bufRes.iSize() ; c++) {
    r = bufLstInp[iFltRows>>1][c];
    if (r == rUNDEF) {
      bufRes[c] = rUNDEF;
      continue;
    }
    CalcMeanStd(bufLstInp, c);
    if (rStdMeanRatio == rUNDEF) {
      bufRes[c] = rUNDEF;
      continue;
    }
    if (rStdMeanRatio <= cu)
      bufRes[c] = rMean;
    else {
      w = (1 - (cu * cu /(rStdMeanRatio *rStdMeanRatio))) / (1 + cu2);
      bufRes[c] =  r * w + rMean * (1 - w);
    }
  }
}
String FilterKuan::sName(bool fExt, const String& sDirRelative) const
{
  String s = IlwisObjectPtr::sName(fExt, sDirRelative);
  if (s.length() != 0)
    return s;
  return String("FilterKuan(%i,%i,%li,%s,%g)", iFltRows, iFltCols, iNrLooks, pcYesNo(fIntensity), rDampFactor);
}

//// Frost Filter

const char* FilterFrost::sSyntax()
{
  return "FilterFrost(rows,cols,looks,intens,dampfac,k1)";
}

FilterFrost::FilterFrost(const FileName& fn, int iRows, int iCols, long iNrLks, bool fIntens, double rDampFac, double kOne)
: FilterRadar(fn, iRows, iCols, iNrLks, fIntens, rDampFac)
{
  k1 = kOne;
}


FilterFrost::FilterFrost(const FileName& fn)
: FilterRadar(fn)
{
  ReadElement("FilterFrost", "k1", k1);
}

FilterFrost::~FilterFrost()
{
}

FilterFrost* FilterFrost::create(const FileName& fn, const String& sExpr)
{
  int iFltRows, iFltCols;
  int iNrLooks;
  bool fIntens;
  double rDampFact, k1;
  int iParms = iParse(sExpr, iFltRows, iFltCols, iNrLooks, fIntens, rDampFact, k1);
  if (iParms < 2 || iParms > 6)
    ExpressionError(sExpr, sSyntax());
  if ((iFltRows <= 0) || (iFltCols <= 0))
    ExpressionError(sExpr, sSyntax());
  return new FilterFrost(fn, iFltRows, iFltCols, iNrLooks, fIntens, rDampFact, k1);
}

void FilterFrost::Store()
{
  FilterRadar::Store();
  WriteElement("FilterRadar", "Type", "FilterFrost");
  WriteElement("FilterFrost", "k1", k1);
}

void FilterFrost::ExecuteVal(const RealBufExt* bufLstInp, RealBuf& bufRes)
{
  double r, rW, rWeightSum, rTotal;
  for (int c = 0; c < bufRes.iSize() ; c++) {
    r = bufLstInp[iFltRows>>1][c];
    if (r == rUNDEF) {
      bufRes[c] = rUNDEF;
      continue;
    }
    CalcMeanStd(bufLstInp, c);
    if (rStdMeanRatio == rUNDEF) {
      bufRes[c] = rUNDEF;
      continue;
    }
    rWeightSum = 0;
    rTotal = 0;
    for (int i = 0; i < iFltRows ; i++)
      for (int cc = c-(iFltCols>>1); cc <= c+(iFltCols>>1) ; cc++) {
        r = bufLstInp[i][cc];
        rW = sqrt((double)(sqr((long)i-(iFltCols>>1))+sqr((long)cc-c)));
        rW = k1 * exp(-rDampFactor*rStdMeanRatio*rW);
        rWeightSum += r * rW;
        rTotal += rW;
      }  
    if (rTotal == 0)
      bufRes[c] = rUNDEF;
    else
      bufRes[c] = rWeightSum / rTotal;
  }
}
String FilterFrost::sName(bool fExt, const String& sDirRelative) const
{
  String s = IlwisObjectPtr::sName(fExt, sDirRelative);
  if (s.length() != 0)
    return s;
  return String("FilterFrost(%i,%i,%li,%s,%g,%g)", iFltRows, iFltCols, iNrLooks, pcYesNo(fIntensity), rDampFactor, k1);
}

//// FrostEnhanced Filter
const char* FilterFrostEnhanced::sSyntax()
{
  return "FilterFrostEnhanced(rows,cols,looks,intens,dampfac,k1)";
}

FilterFrostEnhanced::FilterFrostEnhanced(const FileName& fn, int iRows, int iCols, long iNrLks, bool fIntens, double rDampFac, double kOne)
: FilterRadar(fn, iRows, iCols, iNrLks, fIntens, rDampFac)
{
  cmax = sqrt(1.0+2.0/iNrLooks);
  k1 = kOne;
}


FilterFrostEnhanced::FilterFrostEnhanced(const FileName& fn)
: FilterRadar(fn)
{
  cmax = sqrt(1.0+2.0/iNrLooks);
  ReadElement("FilterFrostEnhanced", "k1", k1);
}

FilterFrostEnhanced::~FilterFrostEnhanced()
{
}

FilterFrostEnhanced* FilterFrostEnhanced::create(const FileName& fn, const String& sExpr)
{
  int iFltRows, iFltCols;
  int iNrLooks;
  bool fIntens;
  double rDampFact, k1;
  int iParms = iParse(sExpr, iFltRows, iFltCols, iNrLooks, fIntens, rDampFact, k1);
  if (iParms < 2 || iParms > 6)
    ExpressionError(sExpr, sSyntax());
  if ((iFltRows <= 0) || (iFltCols <= 0))
    ExpressionError(sExpr, sSyntax());
  return new FilterFrostEnhanced(fn, iFltRows, iFltCols, iNrLooks, fIntens, rDampFact, k1);
}

void FilterFrostEnhanced::Store()
{
  FilterRadar::Store();
  WriteElement("FilterRadar", "Type", "FilterFrostEnhanced");
  WriteElement("FilterFrostEnhanced", "k1", k1);
}

void FilterFrostEnhanced::ExecuteVal(const RealBufExt* bufLstInp, RealBuf& bufRes)
{
  double r, rW, rWeightSum, rTotal, rFunc;
  for (int c = 0; c < bufRes.iSize() ; c++) {
    r = bufLstInp[iFltRows>>1][c];
    if (r == rUNDEF) {
      bufRes[c] = rUNDEF;
      continue;
    }
    CalcMeanStd(bufLstInp, c);
    if (rStdMeanRatio == rUNDEF) {
      bufRes[c] = rUNDEF;
      continue;
    }
    if (rStdMeanRatio <= cu)
      bufRes[c] =  rMean;
    else if (rStdMeanRatio >= cmax)
      bufRes[c] =  r;
    else {
      rWeightSum = 0;
      rTotal = 0;
      rFunc = (rStdMeanRatio - cu) / (cmax - rStdMeanRatio);
      for (int i = 0; i < iFltRows ; i++)
        for (int cc = c-(iFltCols>>1); cc <= c+(iFltCols>>1) ; cc++) {
          r = bufLstInp[i][cc];
          rW = sqrt((double)sqr((long)i-(iFltCols>>1))+(double)sqr((long)cc-c));
          rW = k1 * exp(-rDampFactor*rFunc*rW);
          rWeightSum += r * rW;
          rTotal += rW;
        }  
      if (rTotal == 0)
        bufRes[c] = rUNDEF;
      else
        bufRes[c] = rWeightSum / rTotal;
    }  
  }
}

String FilterFrostEnhanced::sName(bool fExt, const String& sDirRelative) const
{
  String s = IlwisObjectPtr::sName(fExt, sDirRelative);
  if (s.length() != 0)
    return s;
  return String("FilterFrostEnhanced(%i,%i,%li,%s,%g,%g)", iFltRows, iFltCols, iNrLooks, pcYesNo(fIntensity), rDampFactor, k1);
}

//// GammaMap Filter
const char* FilterGammaMap::sSyntax()
{
  return "FilterGammaMap(rows,cols,looks,intens,dampfac)";
}

FilterGammaMap::FilterGammaMap(const FileName& fn, int iRows, int iCols, long iNrLks, bool fIntens, double rDampFac)
: FilterRadar(fn, iRows, iCols, iNrLks, fIntens, rDampFac)
{
  cmax = sqrt(2.0) * cu;
  cu2= sqr(cu);
}


FilterGammaMap::FilterGammaMap(const FileName& fn)
: FilterRadar(fn)
{
  cmax = sqrt(2.0) * cu;
}

FilterGammaMap* FilterGammaMap::create(const FileName& fn, const String& sExpr)
{
  int iFltRows, iFltCols;
  int iNrLooks;
  bool fIntens;
  double rDampFact, k1;
  int iParms = iParse(sExpr, iFltRows, iFltCols, iNrLooks, fIntens, rDampFact, k1);
  if (iParms < 2 || iParms > 5)
    ExpressionError(sExpr, sSyntax());
  if ((iFltRows <= 0) || (iFltCols <= 0))
    ExpressionError(sExpr, sSyntax());
  return new FilterGammaMap(fn, iFltRows, iFltCols, iNrLooks, fIntens, rDampFact);
}

FilterGammaMap::~FilterGammaMap()
{
}

void FilterGammaMap::Store()
{
  FilterRadar::Store();
  WriteElement("FilterRadar", "Type", "FilterGammaMap");
}

void FilterGammaMap::ExecuteVal(const RealBufExt* bufLstInp, RealBuf& bufRes)
{
  double r, alpha, b, x;
  for (int c = 0; c < bufRes.iSize() ; c++) {
    r = bufLstInp[iFltRows>>1][c];
    if (r == rUNDEF) {
      bufRes[c] = rUNDEF;
      continue;
    }
    CalcMeanStd(bufLstInp, c);
    if (rStdMeanRatio == rUNDEF) {
      bufRes[c] = rUNDEF;
      continue;
    }
    if (rStdMeanRatio <= cu)
      bufRes[c] = rMean;
    else if (rStdMeanRatio >= cmax)
      bufRes[c] = r;
    else {
      alpha = (1 + cu2) / (sqr(rStdMeanRatio) - cu2);
      b = alpha - iNrLooks -1;
      x = sqr(rMean * b) + 4 * alpha * iNrLooks * rMean * r;
      if (x > 0)
        bufRes[c] = (b * rMean + sqrt(x)) / ( 2 * alpha);
      else
        bufRes[c] = rUNDEF;
    }
  }
}

String FilterGammaMap::sName(bool fExt, const String& sDirRelative) const
{
  String s = IlwisObjectPtr::sName(fExt, sDirRelative);
  if (s.length() != 0)
    return s;
  return String("FilterGammaMap(%i,%i,%li,%s,%g)", iFltRows, iFltCols, iNrLooks, pcYesNo(fIntensity), rDampFactor);
}




