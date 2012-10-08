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
/* Table2Dim
   Copyright Ilwis System Development ITC
   dec. 1996, by Jelle Wind
   	Last change:  WK   27 Apr 98    3:53 pm
*/

#ifndef ILWTABLE2_H
#define ILWTABLE2_H
#include "Engine\Table\tbl.h"
#include "Engine\Domain\dmsort.h"

class DATEXPORT DomainSort;
class DATEXPORT Table2Dim;

class DATEXPORT Table2DimPtr: public TablePtr
{
  friend class DATEXPORT Table2Dim;
  friend class DATEXPORT TablePtr;
  static Table2DimPtr* create(const FileName&);
  static Table2DimPtr* create(const FileName&, const String&, bool& fLoadColumns);
protected:
  Table2DimPtr(const FileName&);
  Table2DimPtr(const FileName& fn, const DomainValueRangeStruct& dvrs1,
               const DomainValueRangeStruct& dvrs2, 
               const DomainValueRangeStruct& dvs);
  Table2DimPtr(const FileName&, const String& sSecPrf, bool fCreate=false);

public:
  virtual ~Table2DimPtr();
  virtual void Store();
//virtual void Export(const FileName& fn) const;
  virtual String sType() const;
//  virtual bool fChangeReadOnlyAllowed(const Column&);
  const Domain& dm1() const { return dom1; }
  const Domain& dm2() const { return dom2; }
  DomainSort* ds1() const { return pdsrt1; }
  DomainSort* ds2() const { return pdsrt2; }
	int iMatrixRows() const { return ds1()->iNettoSize(); }
	int iMatrixCols() const { return ds2()->iNettoSize(); }
  long iRaw(const String& sKey1, const String& sKey2) const;
  long iValue(const String& sKey1, const String& sKey2) const;
  double rValue(const String& sKey1, const String& sKey2) const;
  String sValue(const String& sKey1, const String& sKey2, short iWidth=-1, short iDec=-1) const;
  Coord cValue(const String& sKey1, const String& sKey2) const;
  void PutRaw(const String& sKey1, const String& sKey2, long iRaw);
  void PutVal(const String& sKey1, const String& sKey2, long iValue);
  void PutVal(const String& sKey1, const String& sKey2, double rValue);
  void PutVal(const String& sKey1, const String& sKey2, const String& sValue);
  void PutVal(const String& sKey1, const String& sKey2, const Coord& cValue);
  long iRec(long iKey1, long iKey2) const ;
  long iKey1(long iRec) const;
  long iKey2(long iRec) const;
  String _export sKey1(long iRec) const;
  String _export sKey2(long iRec) const;
  long iRec(const String& sKey1, const String& sKey2) const;
  long _export iRaw(long iKey1, long iKey2) const;
  long _export iValue(long iKey1, long iKey2) const;
  double _export rValue(long iKey1, long iKey2) const;
  String _export sValue(long iKey1, long iKey2, short iWidth=-1, short iDec=-1) const;
  Coord cValue(long iKey1, long iKey2) const;
  void PutRaw(long iRaw1, long iRaw2, long iRaw);
  void PutVal(long iRaw1, long iRaw2, long iValue);
  void PutVal(long iRaw1, long iRaw2, double rValue);
  _export void PutVal(long iRaw1, long iRaw2, const String& sValue);
  void PutVal(long iRaw1, long iRaw2, const Coord& cValue);
  DomainValueRangeStruct _export dvrsVal() const;
  Column colValue() { return colVal; }
  void _export SetAxisDomains(const DomainValueRangeStruct& dvrs1, const DomainValueRangeStruct& dvrs2);
  void _export SetDataDomain(const DomainValueRangeStruct& dvs);
  virtual bool fDependent() const; // returns true if it's dependent table
  virtual void BreakDependency();
  void AdjustForDomainChange();
	void GetObjectStructure(ObjectStructure& os);
private:
  Domain dom1, dom2;
  DomainSort *pdsrt1, *pdsrt2;
  long iSortSize1, iSortSize2;
  Column colVal;
};

class Table2Dim: public Table
{
public:
  _export Table2Dim();
  _export Table2Dim(const FileName& fn);
  _export Table2Dim(const Table2Dim& tbl2);
  _export Table2Dim(const FileName& fn, const DomainValueRangeStruct& dvrs1,
            const DomainValueRangeStruct& dvrs2,
            const DomainValueRangeStruct& dvs);
  _export Table2Dim(const FileName& fn, const String& sExpression);
  void operator = (const Table2Dim& tbl2) { SetPointer(tbl2.pointer()); }
  Table2DimPtr* ptr() const { return static_cast<Table2DimPtr*>(pointer()); }
  Table2DimPtr* operator -> () const { return ptr(); }
//void Export(const FileName& fn) const;
};

#endif





