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
/* calcvar.h  'Variables' for calculator
   Copyright Ilwis System Development ITC
   nov 1995, by Jelle Wind
	Last change:  WK   10 Aug 98   12:12 pm
*/

#ifndef ILW_CALCVARH
#define ILW_CALCVARH
#include "Engine\Domain\dm.h"
#include "Engine\SpatialReference\Gr.h"
#include "Engine\Map\Raster\Map.h"
#include "Engine\Table\Col.h"
#include "Engine\Table\tbl.h"
#include "Engine\Base\DataObjects\valrange.h"
#include "Engine\Base\DataObjects\Stack.h"
#include "Engine\Scripting\CALCSTCK.H"
#include "Engine\Table\tbl2dim.h"

class CALCEXPORT CalcVarMap;
class CALCEXPORT CalcVarColumn;
class CALCEXPORT CalcVarTable2;
class CALCEXPORT CalcVarSimple;
class CALCEXPORT CalcVarParm;
class CALCEXPORT CalcVarConstString;
class CALCEXPORT InstBase;
class CALCEXPORT InstLoad;
class CALCEXPORT InstConstString;

//!
enum VarType { vtNONE = 0, vtVALUE, vtSTRING, vtTABLE, 
               vtCOLUMN, vtMAP, vtCOORD, vtTABLE2, vtCOLOR };
//inline VarType undef(VarType) { return vtNONE; }

class CALCEXPORT CalcVariablePtr;

//!
class CalcVariable {
public:
  _export CalcVariable();
  CalcVariable(const DomainValueRangeStruct& dvs, VarType vart);
  CalcVariable(const DomainValueRangeStruct& dvs, VarType vart, const GeoRef& gr, RowCol rc);
  CalcVariable(const CalcVariable& cv)
    { ptr =0; SetPointer(cv.ptr); }
  _export ~CalcVariable();
  void operator = (const CalcVariable& cv)
    { SetPointer(cv.ptr); }
  CalcVariablePtr* operator -> () const
    { return ptr; }
  bool fValid() const
    { return 0 != ptr; }
  bool operator == (const CalcVariable& cv) const
    { return cv.ptr == ptr; }
  void _export SetPointer(CalcVariablePtr* p);
  CalcVariablePtr* ptr;
};

//!
class CALCEXPORT CalcVariablePtr {
  friend CalcVariable;
public:
  CalcVariablePtr(const DomainValueRangeStruct& dvs, VarType vart);
  virtual ~CalcVariablePtr();
  virtual long iValue() const;
  virtual double rValue() const; 
  virtual String sValue() const; 
  virtual Coord cValue() const; 
  virtual Color clrValue() const;
  virtual String sName() const;  
  virtual Map map() const;
  virtual Column col() const;
  virtual bool fEqual(const CalcVariablePtr& cv) const;
  bool operator==(const CalcVariablePtr& cv) const
    { return fEqual(cv); }
  bool operator!=(const CalcVariablePtr& cv) const
    { return !fEqual(cv); }
  CalcVarMap* pcvMap();
  CalcVarColumn* pcvCol();
  CalcVarTable2* pcvTb2();
  CalcVarSimple* pcvSimple();
  CalcVarParm* pcvParm();
  CalcVarConstString* pcvConstString();
  void SetInstruc(InstLoad* _instload)
  { ainstload &= _instload; }
  void SetDomainValueRangeStruct(const DomainValueRangeStruct& dvrs)
     { dvs = dvrs; }
  const Array<InstLoad*>& aild() const
  { return ainstload; }
  StackObjectType sot;
  DomainValueRangeStruct dvs;
  VarType vt;
  GeoRef gr;
  RowCol rcSize;
protected:
private:
  Array<InstLoad*> ainstload;
  int iRef;
  static CalcVariablePtr* create(const DomainValueRangeStruct& dvs, VarType vart, const GeoRef& gr , RowCol rc);
  void Init(const DomainValueRangeStruct& dvs, VarType vart, const GeoRef& gr);
};


//!
class ArrayCalcVariable : public Array<CalcVariable>
{
public:
  ArrayCalcVariable();
  ~ArrayCalcVariable();
};



class CALCEXPORT CalcVarConstInt : public CalcVariablePtr {
public:
  CalcVarConstInt(long iVal);
  virtual long iValue() const;
  virtual double rValue() const;
  virtual String sValue() const;
protected:
private:
  long _iVal;
};


class CALCEXPORT CalcVarConstReal : public CalcVariablePtr {
public:
  CalcVarConstReal(double rVal, const String& sVal);
  virtual long iValue() const;
  virtual double rValue() const;
  virtual String sValue() const;
protected:
private:
  double _rVal;
};

class CALCEXPORT CalcVarConstTime : public CalcVariablePtr {
public:
  CalcVarConstTime(double rVal, const String& sVal);
  virtual long iValue() const;
  virtual double rValue() const;
  virtual String sValue() const;
protected:
private:
  double _rVal;
};


class CALCEXPORT CalcVarConstString  : public CalcVariablePtr {
public:
  CalcVarConstString(const String& sVal, InstConstString* icstr);
  virtual long iValue() const;
  virtual double rValue() const;
  virtual String sValue() const;
  InstConstString* ics; // also some 'non class member' functions need it
protected:
private:
  String _sVal;
};


class CALCEXPORT CalcVarConstCoord  : public CalcVariablePtr {
public:
  CalcVarConstCoord(const Coord& cVal, const Domain& dm);
  virtual Coord cValue() const;
protected:
private:
  Coord _cVal;
};


class CALCEXPORT CalcVarConstColor  : public CalcVariablePtr {
public:
  CalcVarConstColor(const Color& clrVal, const Domain& dm);
  virtual Color clrValue() const;
protected:
private:
  Color _clrVal;
};


class CALCEXPORT CalcVarSimple  : public CalcVariablePtr {
public:
  CalcVarSimple(const DomainValueRangeStruct& dvs, VarType vart, const String& sName);
  virtual String sName() const;
  virtual void PutVal(long);
  virtual void PutVal(double);
  virtual void PutVal(const String&);
  virtual void PutVal(const Coord& cVal);
  virtual void PutVal(const Color& clrVal);
protected:
private:
  String _sName;
};


class CALCEXPORT CalcVarInt  : public CalcVarSimple {
public:
  CalcVarInt(const String& sName);
  virtual long iValue() const;
  virtual double rValue() const;
  virtual String sValue() const;
  virtual void PutVal(long iVal);
  virtual void PutVal(int iVal);
  virtual void PutVal(double rVal);
  virtual void PutVal(const String& sVal);
protected:
private:
  long _iVal;
};


class CALCEXPORT CalcVarUndef : public CalcVarSimple {
public:
  CalcVarUndef();
  virtual long iValue() const;
  virtual double rValue() const;
  virtual String sValue() const;
  virtual void PutVal(long);
  virtual void PutVal(double);
  virtual void PutVal(const String&);
};


class CALCEXPORT CalcVarReal  : public CalcVarSimple {
public:
  CalcVarReal(const String& sName);
  virtual long iValue() const;
  virtual double rValue() const;
  virtual String sValue() const;
  virtual void PutVal(long iVal);
  virtual void PutVal(double rVal);
  virtual void PutVal(const String& sVal);
protected:
private:
  double _rVal;
};

class CALCEXPORT CalcVarTime  : public CalcVarReal {
public:
  CalcVarTime(const String& sName);
  virtual long iValue() const;
  virtual double rValue() const;
  virtual String sValue() const;
  virtual void PutVal(long iVal);
  virtual void PutVal(double rVal);
  virtual void PutVal(const String& sVal);
protected:
private:
  double _rVal;
};


class CALCEXPORT CalcVarString  : public CalcVarSimple {
public:
  CalcVarString(const String& sName);
  virtual long iValue() const;
  virtual double rValue() const;
  virtual String sValue() const;
  virtual void PutVal(long iVal);
  virtual void PutVal(double rVal);
  virtual void PutVal(const String& sVal);
protected:
private:
  String _sVal;
};


class CALCEXPORT CalcVarCoord  : public CalcVarSimple {
public:
  CalcVarCoord(const String& sName, const Domain& dm);
  virtual Coord cValue() const;
  virtual void PutVal(const Coord& cVal);
protected:
private:
  Coord _cVal;
};


class CALCEXPORT CalcVarColor  : public CalcVarSimple {
public:
  CalcVarColor(const String& sName, const Domain& dm);
  virtual Color clrValue() const;
  virtual void PutVal(const Color& clrVal);
protected:
private:
  Color _clrVal;
};


class CALCEXPORT CalcVarIlwisObject : public CalcVariablePtr {
public:
  CalcVarIlwisObject(const String& sName, VarType vart);
  CalcVarIlwisObject(const DomainValueRangeStruct& dvs,
                     VarType vart, const IlwisObject& obj);
  virtual String sName() const;
protected:
  IlwisObjectPtr *ptr;
  String _sName;
};


class CALCEXPORT CalcVarMap  : public CalcVarIlwisObject {
  friend class CALCEXPORT Instructions;
public:
  CalcVarMap(const String& sName);
  CalcVarMap(const Map& mp) ;
  virtual Map map() const;
protected:
private:
  void SetMinMax();
  void SetMap(const Map& mp);
  Map _map;
};


class CALCEXPORT CalcVarTable2  : public CalcVarIlwisObject {
public:
  CalcVarTable2(const String& sName);
  CalcVarTable2(const Table2Dim& tb2);
	~CalcVarTable2();
  const Table2Dim& tb2() const;
protected:
private:
  void SetTable2(const Table2Dim& tb2);
  void SetMinMax();
  Table2Dim _tb2;
};


class CALCEXPORT CalcVarColumn  : public CalcVarIlwisObject {
public:
  CalcVarColumn(const String& sName);
  CalcVarColumn(const Column& col);
  virtual Column col() const;
protected:
private:
  void SetMinMax();
  void SetColumn(const Column& cl);
  Column _col;
};


class CALCEXPORT CalcVarParm  : public CalcVariablePtr {
public:
  CalcVarParm(const DomainValueRangeStruct& dvs, VarType vart, const String& sName);
  virtual String sName() const;
  virtual void PutVal(long); 
  virtual void PutVal(double);
  virtual void PutVal(const String&);
protected:
private:
  String _sName;
};

#endif
