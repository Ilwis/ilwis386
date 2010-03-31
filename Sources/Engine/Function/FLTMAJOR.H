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
   june 1995, by Jelle Wind
	Last change:  MS   24 Sep 97    8:39 am
*/

#ifndef ILWFLTMAJ_H
#define ILWFLTMAJ_H
#include "Engine\Function\FILTER.H"

enum FilterMajorityType { mtNORMAL=0, mtUNDEF, mtZERO };

class DATEXPORT FilterMajority: public FilterPtr
{
  friend class DATEXPORT FilterPtr;
  static FilterMajority* create(const FileName&, const String& sExpr);
public:
  virtual ~FilterMajority();
  static const char* sSyntax();
  virtual void Store();
  virtual String sName(bool fExt, const String& sDirRelative) const;
  virtual void ExecuteRaw(const LongBufExt* bufListInp, LongBuf& bufRes);
  virtual void ExecuteVal(const RealBufExt* bufLstInp, RealBuf& bufRes);
  bool fRawAllowed() const { return mtZERO != mt; }
protected:
  FilterMajority(const FileName&);
  FilterMajority(const FileName&, short iRows, short iCols, FilterMajorityType mt);
  void Init();
  void InsertVal(double rVal);
  void InsertVal(long iVal);
  long iGetMajor();
  double rGetMajor();
  FilterMajorityType mt;
  LongArray iFreq;
  LongArray iValues;
  RealArray rValues;
};

#endif




