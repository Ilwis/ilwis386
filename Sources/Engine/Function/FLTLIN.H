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
/* $Log: /ILWIS 3.0/Function/FLTLIN.H $
 * 
 * 4     14-12-99 17:54 Koolhoven
 * LinearFilterEditor editing works now
 * 
 * 3     14-12-99 15:30 Koolhoven
 * Header comment
 * 
 * 2     14-12-99 15:21 Koolhoven
 * Added FilterWindow (still only read-only and too simple)
// Revision 1.3  1998/09/16 17:30:33  Wim
// 22beta2
//
// Revision 1.2  1997/08/20 06:53:35  Wim
// FilterLinear::sName() added
//
/* FilterLinear
   Copyright Ilwis System Development ITC
   june 1995, by Jelle Wind
	Last change:  WK   20 Aug 97    8:34 am
*/

#ifndef ILWFLTLIN_H
#define ILWFLTLIN_H
#include "Engine\Function\FILTER.H"
#include "Engine\Base\Algorithm\Basemat.h"

class DATEXPORT FilterLinear: public FilterPtr
{
  friend class DATEXPORT FilterPtr;
  friend class CALCEXPORT InstNbFlt;
  static FilterLinear* create(const FileName&);
public:
  virtual ~FilterLinear();
  virtual void Store();
  virtual bool fRawAllowed() const;
  virtual void ExecuteRaw(const LongBufExt* bufListInp, LongBuf& bufRes);
  virtual void ExecuteVal(const RealBufExt* bufLstInp, RealBuf& bufRes);
  FilterLinear(const FileName&, const IntMatrix& mat, double rGain,
               const Domain& dmDefault = Domain());
  FilterLinear(const FileName&, const RealMatrix& mat, double rGain,
               const Domain& dmDefault = Domain());
  FilterLinear(const FileName&, int iRows, int iCols, const String& sExpression,
               const Domain& dmDefault = Domain());
  FilterLinear(const FileName&, int iRows, int iCols, bool fUseReals);
  static FilterLinear* create(const FileName&, const String& sExpression);
  virtual String sName(bool fExt, const String& sDirRelative) const;
  virtual Domain dmDefault(const Map&) const;
  virtual ValueRange vrDefault(const Map&, const Domain&) const;
	long iValue(int iRow, int iCol) const 
		{
			return iMat(iRow, iCol);
		}
	double rValue(int iRow, int iCol) const 
		{ if (fUseRealMat)
				return rMat(iRow, iCol);
			else
				return iMat(iRow, iCol);
		}
	void SetValue(int iRow, int iCol, long iVal)
		{
			iMat(iRow, iCol) = iVal;
			Updated();
		}
	void SetValue(int iRow, int iCol, double rVal)
		{ if (fUseRealMat)
				rMat(iRow, iCol) = rVal;
			else
				iMat(iRow, iCol) = rounding(rVal);
		}
	double rGetGain() const { return rGain; }
  void SetGain(double rGn) 
		{ 
			rGain = rGn; 
			Updated();
		}
protected:
  FilterLinear(const FileName&);
  void FillMatrix(const String& sExpr);
  IntMatrix iMat;
  RealMatrix rMat;
  bool fUseRealMat;
  double rGain;
  String sExpression;
};

class DATEXPORT FilterAverage: public FilterLinear
{
  friend class DATEXPORT FilterPtr;
  friend class DATEXPORT FilterLinear;
  static FilterAverage* create(const FileName&);
  static FilterAverage* create(const FileName&, const String& sExpr);
public:
  virtual ~FilterAverage();
  static const char* sSyntax();
  virtual void Store();
  virtual String sName(bool fExt, const String& sDirRelative) const;
  virtual Domain dmDefault(const Map&) const;
  virtual ValueRange vrDefault(const Map&, const Domain&) const;
protected:
  FilterAverage(const FileName&);
  FilterAverage(const FileName&, short iRows, short iCols);
};

#endif




