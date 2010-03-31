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
// $Log: MAPCALC.H $
// Revision 1.3  1998/09/16 17:24:28  Wim
// 22beta2
//
// Revision 1.2  1997/08/11 13:18:12  Wim
// InitFreeze() added, SetDomainValueRangeStruct() removed because it is
// never called.
//
/* MapCalculate
   Copyright Ilwis System Development ITC
   april 1995, by Jelle Wind
	Last change:  WK   11 Aug 97    3:00 pm
*/
#ifndef ILWMAPCALCULATE_H
#define ILWMAPCALCULATE_H
#include "Engine\Applications\MAPVIRT.H"
#include "Engine\Base\objdepen.h"

IlwisObjectPtr * createMapCalculate(const FileName& fn, IlwisObjectPtr& ptr, const String& sExpr, vector<void *> parms=vector<void*>() );

class CALCEXPORT Instructions;

class DATEXPORT MapCalculate: public MapVirtual
{
  friend class MapVirtual;
public:
  MapCalculate(const FileName& fn, MapPtr& p);
  static MapCalculate* create(const FileName& fn, MapPtr& p, const String& sExpression);
  ~MapCalculate();
  static const char* sSyntax();
  virtual String sType() const;
  virtual void ComputeLineRaw(long Line, LongBuf&, long iFrom, long iNum) const;
  virtual void ComputeLineVal(long Line, LongBuf&, long iFrom, long iNum) const;
  virtual void ComputeLineVal(long Line, RealBuf&, long iFrom, long iNum) const;
protected:
  void Init();
  virtual void InitFreeze();
  virtual void Store();
  virtual long iComputePixelRaw(RowCol) const;
  virtual long iComputePixelVal(RowCol) const;
  virtual double rComputePixelVal(RowCol) const;
  virtual String sExpression() const;
  virtual bool fDomainChangeable() const;
  virtual bool fValueRangeChangeable() const;
  virtual bool fGeoRefChangeable() const;
  virtual bool fExpressionChangeable() const;
//  virtual void SetDomainValueRangeStruct(const DomainValueRangeStruct& dvs);
  void Replace(const String& sExpression);
  virtual DomainValueRangeStruct dvrsDefault(const String& sExpression) const;
  virtual bool fMergeDomainForCalc(Domain& dm, const String& sExpr);
private:
  MapCalculate(const FileName&, MapPtr& p, Instructions* inst, const String& sExpr, const ObjectDependency& objdep, bool fNoGeoRef);
//  virtual void Replace(const String& sExpression);
//  void ComputeLine(long Line, StringBuf&, long iFrom, long iNum) const;
  static Instructions* instCreate(const String& sExpr, const FileName& fn, ObjectDependency& objdep, bool& fNoGeoRef);
  Instructions* instruc;
  String _sExpression;
  void SetUse();
  bool fUseRaw, fUseReal, fNoGeoRef;
};


#endif // ILWMAPCALCULATE_H





