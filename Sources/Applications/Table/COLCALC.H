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
/* ColumnCalculate
   Copyright Ilwis System Development ITC
   may 1995, by Jelle Wind
	Last change:  JEL   9 Jun 97   12:39 pm
*/
#ifndef ILWCOLCALCULATE_H
#define ILWCOLCALCULATE_H
#include "Engine\Applications\COLVIRT.H"

_export IlwisObjectPtr * createColumnCalculate(const FileName& fn, IlwisObjectPtr& ptr, const String& sExpr, vector<void *> parms );

class CALCEXPORT Instructions;

class ColumnCalculate: public ColumnVirtual
{
  friend class ColumnVirtual;
public:
  ~ColumnCalculate();
    _export ColumnCalculate(const Table& tbl, const String& sColName, ColumnPtr& p);
  static ColumnCalculate _export *create(const Table& tbl, const String& sColName, ColumnPtr& p,
                                 const String& sExpression, const DomainValueRangeStruct& dvs);
//  virtual void ComputeBuf(ByteBuf&, long iKey, long iNum) const;
//  virtual void ComputeBuf(IntBuf& , long iKey, long iNum) const;
  virtual void ComputeBufRaw(LongBuf&, long iKey, long iNum) const;
  virtual void ComputeBufVal(RealBuf&, long iKey, long iNum) const;
  virtual void ComputeBufVal(StringBuf&, long iKey, long iNum, short iWid=-1, short iDec=-1) const;
  virtual void ComputeBufVal(CoordBuf&, long iKey, long iNum) const;
  virtual bool fInit();
  virtual bool fDomainChangeable() const;
  virtual bool fValueRangeChangeable() const;
  virtual bool fExpressionChangeable() const;
  virtual void SetDomainValueRangeStruct(const DomainValueRangeStruct&);
//  virtual void SetDomain(const Domain& dm);
  virtual String sType() const;
  virtual String sExpression() const;
  virtual void InitFreeze();
  virtual DomainValueRangeStruct dvrsDefault(const String& sExpression) const;
  virtual bool fMergeDomainForCalc(Domain& dm, const String& sExpr);
protected:
  virtual void Store();
  virtual long iComputeRaw(long iKey) const;
  virtual double rComputeVal(long iKey) const;
  virtual Coord cComputeVal(long iKey) const;
  virtual String sComputeVal(long iKey, short iWidth=-1, short iDec=-1) const;
private:
  ColumnCalculate(const Table& tbl, const String& sColName, ColumnPtr& p,
                  Instructions* inst, const String& sExpr, 
                  const ObjectDependency& objdep, const Table& tblSearch);
  virtual void Replace(const String& sExpression);
  static Instructions* instCreate(const Table& tbl, const String& sExpr,
                                  const DomainValueRangeStruct& dvs, ObjectDependency& objdep, 
                                  const Table& tblSearch);
  Instructions* instruc;
  String _sExpression;
};


#endif // ILWCOLCALCULATE_H





