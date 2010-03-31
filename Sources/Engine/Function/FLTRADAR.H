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
   june 1996, by Jelle Wind
	Last change:  JEL  29 May 97    1:12 pm
*/

#ifndef ILWFLTRADAR_H
#define ILWFLTRADAR_H
#include "Engine\Function\FILTER.H"

class DATEXPORT FilterRadar: public FilterPtr
{
  friend class DATEXPORT FilterPtr;
  static FilterRadar* create(const FileName&);
public:
  FilterRadar(const FileName&, int iRows, int iCols, long iNrLooks,
                 bool fIntensity, double rDampFac);
  virtual ~FilterRadar();
  virtual void Store();
  virtual bool fRawAllowed() const;
  void CalcMeanStd(const RealBufExt* bufLstInp, int c);
  virtual void ExecuteRaw(const LongBufExt* bufListInp, LongBuf& bufRes);
  virtual void ExecuteVal(const RealBufExt* bufLstInp, RealBuf& bufRes) =0;
  static int iParse(const String& sExpr, int& iRows, int& iCols, int& iNrLks, bool& fIntens, double& rDampFac, double& k1);
protected:
  FilterRadar(const FileName&);
  long iNrLooks;
  double rMean, rStd;
  double rStdMeanRatio; // local std / local mean;
  double cu; // 1 /sqrt(nr. looks) or 0.523 / sqrt(nr.looks)
  double rDampFactor; 
  bool fIntensity;
};

class DATEXPORT FilterLee: public FilterRadar
{
  friend class DATEXPORT FilterRadar;
public:
  FilterLee(const FileName&, int iRows, int iCols, long iLooks, bool fIntens, double rDampFac);
  static FilterLee* create(const FileName&, const String& sExpression);
  virtual ~FilterLee();
  static const char* sSyntax();
  virtual void Store();
  virtual void ExecuteVal(const RealBufExt* bufLstInp, RealBuf& bufRes);
  virtual String sName(bool fExt, const String& sDirRelative) const;
protected:
  FilterLee(const FileName&);
};

class DATEXPORT FilterLeeEnhanced: public FilterRadar
{
  friend class DATEXPORT FilterRadar;
public:
  static FilterLeeEnhanced* create(const FileName&, const String& sExpression);
  FilterLeeEnhanced(const FileName&, int iRows, int iCols, long iLooks, bool fIntens, double rDampFac);
  virtual ~FilterLeeEnhanced();
  static const char* sSyntax();
  virtual void Store();
  virtual void ExecuteVal(const RealBufExt* bufLstInp, RealBuf& bufRes);
  virtual String sName(bool fExt, const String& sDirRelative) const;
protected:
  FilterLeeEnhanced(const FileName&);
  double cmax;
};

class DATEXPORT FilterKuan: public FilterRadar
{
  friend class DATEXPORT FilterRadar;
public:
  static FilterKuan* create(const FileName&, const String& sExpression);
  FilterKuan(const FileName&, int iRows, int iCols, long iLooks, bool fIntens, double rDampFac);
  virtual ~FilterKuan();
  static const char* sSyntax();
  virtual void Store();
  virtual void ExecuteVal(const RealBufExt* bufLstInp, RealBuf& bufRes);
  virtual String sName(bool fExt, const String& sDirRelative) const;
protected:
  FilterKuan(const FileName&);
  double cu2;
};

class DATEXPORT FilterFrost: public FilterRadar
{
  friend class DATEXPORT FilterRadar;
public:
  static FilterFrost* create(const FileName&, const String& sExpression);
  FilterFrost(const FileName&, int iRows, int iCols, long iLooks, bool fIntens, double rDampFac, double kOne);
  virtual ~FilterFrost();
  static const char* sSyntax();
  virtual void Store();
  virtual void ExecuteVal(const RealBufExt* bufLstInp, RealBuf& bufRes);
  virtual String sName(bool fExt, const String& sDirRelative) const;
protected:
  FilterFrost(const FileName&);
  double k1;
};

class DATEXPORT FilterFrostEnhanced: public FilterRadar
{
  friend class DATEXPORT FilterRadar;
public:
  static FilterFrostEnhanced* create(const FileName&, const String& sExpression);
  FilterFrostEnhanced(const FileName&, int iRows, int iCols, long iLooks, bool fIntens, double rDampFac, double kOne);
  virtual ~FilterFrostEnhanced();
  static const char* sSyntax();
  virtual void Store();
  virtual void ExecuteVal(const RealBufExt* bufLstInp, RealBuf& bufRes);
  virtual String sName(bool fExt, const String& sDirRelative) const;
protected:
  FilterFrostEnhanced(const FileName&);
  double cmax, k1;
};

class DATEXPORT FilterGammaMap: public FilterRadar
{
  friend class DATEXPORT FilterRadar;
public:
  static FilterGammaMap* create(const FileName&, const String& sExpression);
  FilterGammaMap(const FileName&, int iRows, int iCols, long iLooks, bool fIntens, double rDampFac);
  virtual ~FilterGammaMap();
  static const char* sSyntax();
  virtual void Store();
  virtual void ExecuteVal(const RealBufExt* bufLstInp, RealBuf& bufRes);
  virtual String sName(bool fExt, const String& sDirRelative) const;
protected:
  FilterGammaMap(const FileName&);
  double cu2, cmax;
};

#endif




