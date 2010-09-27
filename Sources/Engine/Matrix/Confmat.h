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
/* MatrixConfusion
   Copyright Ilwis System Development ITC
   oct. 1996, by Jelle Wind
	Last change:  WK   12 Aug 98   12:24 pm
*/

#ifndef ILWCONFMT_H
#define ILWCONFMT_H
#include "Engine\Matrix\Matrxobj.h"
#include "Engine\Table\Col.h"
#include "Engine\Base\objdepen.h"

class DATEXPORT DomainSort;

class DATEXPORT MatrixConfusion: public MatrixObjectPtr
{
  friend class DATEXPORT MatrixObjectPtr;
protected:
  MatrixConfusion(const Column& col1, const Column& col2, const Column& colFreq);
public:
  static _export MatrixConfusion* create(const Column& col1, const Column& col2, const Column& colFreq);
  static _export MatrixConfusion* create(const String& sExpr);
  virtual ~MatrixConfusion();
  virtual String sName(bool fExt = false, const String& sDirRelative = "") const;
  virtual String sType() const;
  virtual void GetRowText(Array<String>& as) const;
  virtual void GetColumnText(Array<String>& as) const;
  String sSummary() const;
//virtual String sAdditionalInfo() const;
//virtual bool fAdditionalInfo() const;
  virtual void MakeUsable();
  virtual bool fUsable() const;
  virtual int iWidth() const;
  virtual String sValue(int iRow, int iCol, int iWidth) const;
private:  
  Column col1, col2, colFreq;
  DomainSort *pdsrt, *pdsrt2;
  bool fDiffDoms;
  ObjectTime tmCalc;
  ObjectDependency objdep;
  RealArray arAcc, arRel;
  LongArray aiTotals;
  double rAcc, rRel, rOverallAcc;
};

#endif




