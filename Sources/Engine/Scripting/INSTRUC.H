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
/* instruc.h
// Interface for calculator instruction Ilwis 2.0
// sept. 1994, Jelle Wind.
	Last change:  JEL  14 Jul 97   11:20 pm
*/
#ifndef ILWINSTRUC_H
#define ILWINSTRUC_H

#include "Engine\Domain\dm.h"
#include "Engine\Scripting\Parscnst.h"
#include "Engine\Scripting\FUNCS.H"
#include "Engine\Representation\Rpr.h"
#include "Engine\Base\DataObjects\Color.h"
#include "Engine\SpatialReference\Gr.h"
#include "Engine\Map\Point\PNT.H"
#include "Engine\Base\mask.h"

class CALCEXPORT Instructions;
struct InstList;
class CALCEXPORT CodeGenerator;
class CALCEXPORT CalcVariable;
class CALCEXPORT UserDefFuncDesc;

//!
class CALCEXPORT InstBase
{
  friend class Instructions;
public:
  InstBase(Instructions* instrucs);
  virtual ~InstBase();
  virtual void Exec()=0;
  virtual void ReplaceMapUsed(const Map& mp);
  virtual void GetMapUsed(Map&);
  virtual void AddDependentObjects(ObjectDependency& objdep);
protected:
  Instructions* inst;
};



//////////////////////////////////

class InstConstInt : public InstBase
{
public:
  InstConstInt(Instructions* instrucs, long iValue);
  ~InstConstInt();
  CalcVariable cv;
  void Exec();
};

class InstConstReal : public InstBase
{
public:
  InstConstReal(Instructions* instrucs, double rValue, const String& sVal);
  ~InstConstReal();
  CalcVariable cv;
  void Exec();
};

class InstConstString : public InstBase
{
public:
  InstConstString(Instructions* instrucs, const String& sValue);
  ~InstConstString();
  CalcVariable cv;
  void Exec();
};

class InstConstCoord : public InstBase
{
public:
  InstConstCoord(Instructions* instrucs, const Coord& cValue, const Domain& dm);
  ~InstConstCoord();
  CalcVariable cv;
  void Exec();
};

class InstCoord : public InstBase
{
public:
  InstCoord(Instructions* instrucs, const Domain& _dm);
  ~InstCoord();
  void Exec();
  DomainValueRangeStruct dvs;
  RealBuf bufX, bufY;
};

class InstColor : public InstBase
{
public:
  InstColor(Instructions* instrucs);
  ~InstColor();
  void Exec();
  LongBuf bufRed, bufGreen, bufBlue;
  DomainValueRangeStruct dvsColor;
};

class InstColorHSI : public InstBase
{
public:
  InstColorHSI(Instructions* instrucs);
  ~InstColorHSI();
  void Exec();
  LongBuf bufHue, bufSat, bufInt;
  DomainValueRangeStruct dvsColor;
};

class InstTransformCoord : public InstBase
{
public:
  InstTransformCoord(Instructions* instrucs, const CoordSystem& _csyOld, const CoordSystem& _csyNew);
  ~InstTransformCoord();
  void Exec();
  virtual void AddDependentObjects(ObjectDependency& objdep);
  CoordSystem csyOld, csyNew;
  Domain dm;
};

////// load and store
class InstLoad : public InstBase
{
public:
  InstLoad(Instructions* instrucs, const CalcVariable& cvar, bool fNeighb=false);
  ~InstLoad();
  void Exec();
  void InitDomainConvert();
  virtual void ReplaceMapUsed(const Map& mp);
  virtual void GetMapUsed(Map&);
  virtual void AddDependentObjects(ObjectDependency& objdep);
  CalcVariable cv;
  Representation rpr;
  bool fDomainSortConvert;
  LongArrayLarge aiConvertRaw;
  LongBuf bufL;
  RealBuf bufR;
  StringBuf bufS;
  CoordBuf bufC;
  bool fNb;
  Map mp;
  Column col;
};

class InstNbNum : public InstBase
{
public:
  InstNbNum(Instructions* instrucs, const Map& mp);
  ~InstNbNum();
  void Exec();
  virtual void ReplaceMapUsed(const Map& mp);
  virtual void GetMapUsed(Map&);
  virtual void AddDependentObjects(ObjectDependency& objdep);
  LongBuf bufNb;
  LongBuf bufL;
  RealBuf bufR;
  Map mp;
  StackObjectType sot;
};

class InstStore : public InstBase
{
public:
  InstStore(Instructions* instrucs, const CalcVariable& cvar);
  ~InstStore();
  void Exec();
  CalcVariable cv;
  LongBuf bufL;
  RealBuf bufR;
  StringBuf bufS;
  CoordBuf bufC;
  Column col;
};
/*
class InstLoadX : public InstBase
{
public:
  InstLoadX(Instructions* instrucs, const CalcVariable& cvar);
  ~InstLoadX();
  void Exec();
  CalcVariable cv;
  RealBuf buf;
};

class InstLoadY : public InstBase
{
public:
  InstLoadY(Instructions* instrucs, const CalcVariable& cvar);
  ~InstLoadY();
  void Exec();
  CalcVariable cv;
  RealBuf buf;
};
*/
class InstCurrLine : public InstBase
{
public:
  InstCurrLine(Instructions* instrucs);
  void Exec();
  DomainValueRangeStruct dvs;
};

class InstCurrColumn : public InstBase
{
public:
  InstCurrColumn(Instructions* instrucs);
  void Exec();
  DomainValueRangeStruct dvs;
};

class InstCurrRecord : public InstBase
{
public:
  InstCurrRecord(Instructions* instrucs, const Domain & dm);
  void Exec();
  DomainSort* pdsrt;
  DomainValueRangeStruct dvs;
};

class InstCurrKey : public InstBase
{
public:
  InstCurrKey(Instructions* instrucs, const Domain & dm);
  void Exec();
  DomainValueRangeStruct dvs;
};

class InstCurrCoord : public InstBase
{
public:
  InstCurrCoord(Instructions* instrucs);
  void Exec();
  DomainValueRangeStruct dvs;
};

class InstCurrCoordX : public InstBase
{
public:
  InstCurrCoordX(Instructions* instrucs);
  void Exec();
};

class InstCurrCoordY : public InstBase
{
public:
  InstCurrCoordY(Instructions* instrucs);
  void Exec();
};


///////// functions and procedures

class InstFuncEnd : public InstBase
{
public:
  InstFuncEnd(Instructions* instrucs, UserDefFuncDesc* funcDesc);
  void Exec();
  UserDefFuncDesc* fds;
};


////////////////// parameters

class InstLoadParm : public InstBase
{
public:
  InstLoadParm(Instructions* instruc, int iPrm);
  void Exec();
  int iParm;
};

/*
class InstStoreParm : public InstBase
{
public:
  InstStoreParm(Instructions* instruc, int iPrm, UserDefFuncDesc* funcDesc): InstBase(instruc)
    { iParm = iPrm;
      fds = funcDesc; }
//      codegen->stkVariable.pop(); }
  void Exec();
//    { codegen->stkParms[iParm] = codegen->stkCalcItem.pop();
//      codegen->stkCalcType.pop(); }
  int iParm;
  UserDefFuncDesc* fds;
};
	Last change:  JEL  26 May 97   11:29 am
*/
/////////////////// labels

class InstGoto: public InstBase
{
public:
  InstGoto(Instructions* instrucs, int iLab);
  void Exec();
  int iLabel;
};

class InstGotoFalse: public InstBase
{
public:
  InstGotoFalse(Instructions* instrucs, int iLab);
  void Exec();
  int iLabel;
  LongBuf buf;
};


/////////// function calls

class InstEnterExternFunc : public InstBase
{
public:
  InstEnterExternFunc(Instructions* instrucs, int iParms);
  ~InstEnterExternFunc();
  void Exec();
  int iParms;
};

class InstLeaveExternFunc : public InstBase
{
public:
  InstLeaveExternFunc(Instructions* instrucs, int iParms);
  ~InstLeaveExternFunc();
  void Exec();
  int iParms;
};

class InstInternFunc: public InstBase
{
public:
  InstInternFunc(Instructions* instrucs, InternFuncDesc* funcDesc,
                 const DomainValueRangeStruct& dvs, const StackObjectType& sotp);
  ~InstInternFunc();
  void Exec();
  InternFuncDesc* fds;
  DomainValueRangeStruct dvs;
  StackObjectType sot;
};


////////////////// miscelaneous

class InstIndexLoad: public InstBase
{
public:
  InstIndexLoad(Instructions* instrucs, const CalcVariable& cvar, const Table& table);
  ~InstIndexLoad();
  void Exec();
  virtual void AddDependentObjects(ObjectDependency& objdep);
  CalcVariable cv;
  Table tbl;
  DomainSort* pdsrt;
  LongBuf bufInd;
  StringBuf sBufInd;
  LongBuf bufL;
  RealBuf bufR;
  StringBuf bufS;
  CoordBuf bufC;
  Column col;
};

class InstIndexStore: public InstBase
{
public:
  InstIndexStore(Instructions* instrucs, const CalcVariable& cvar, const Table& table);
  ~InstIndexStore();
  void Exec();
  CalcVariable cv;
  Table tbl;
  DomainSort* pdsrt;
  LongBuf bufInd;
  StringBuf sBufInd;
  LongBuf bufL;
  RealBuf bufR;
  StringBuf bufS;
  CoordBuf bufC;
  Column col;
};

class InstIndex2Load: public InstBase
{
public:
  InstIndex2Load(Instructions* instrucs, CalcVarTable2* cvar, const Table& table);
  ~InstIndex2Load();
  void Exec();
  virtual void AddDependentObjects(ObjectDependency& objdep);
  CalcVarTable2 *cvt2;
  CalcVariable cv;
  Table tbl;
  LongBuf bufInd1, bufInd2;
  LongBuf bufL;
  RealBuf bufR;
  StringBuf bufS;
  CoordBuf bufC;
};

/*
class InstIndexLoadParm: public InstBase
{
public:
  InstIndexLoadParm(Instructions* instruc, CodeGenerator * cg, int iParm);
  void Exec();
  Variable *var;
};

class InstIndexStoreParm: public InstBase
{
public:
  InstIndexStoreParm(Instructions* instruc, CodeGenerator * cg, int iParm);
  void Exec();
  Variable *var;
};
*/

class InstStop : public InstBase
{
public:
  InstStop(Instructions* instrucs);
  void Exec();
};

class InstSimpleStart : public InstBase
{
public:
  InstSimpleStart(Instructions* instrucs);
  void Exec() { } // codegen->stkBufSize.push(1); }
};

class InstSimpleEnd : public InstBase
{
public:
  InstSimpleEnd(Instructions* instrucs);
  void Exec() { } // codegen->stkBufSize.pop(); }
};

class DATEXPORT GroupFunction;

class InstClfy : public InstBase
{
public:
  InstClfy(Instructions* instrucs, const Domain& dmGroup, const DomainValueRangeStruct& dvsInp);
  void Exec();
  virtual void AddDependentObjects(ObjectDependency& objdep);
  Domain dom;
  DomainGroup* dmgrp;
  GroupFunction* grpfnc;
  LongBuf bufRes;
};

class InstMapValue : public InstBase
{
public:
  InstMapValue(Instructions* instrucs, const BaseMap&, const CoordSystem& csIn);
  ~InstMapValue();
  void Exec();
  virtual void AddDependentObjects(ObjectDependency& objdep);
  BaseMap mp;
  StackObjectType sot;
  CoordSystem csIn, csOut;
  bool fTransform;
  Representation rpr;
  CoordBuf bufCrd;
  LongBuf bufResL;
  RealBuf bufResR;
  StringBuf bufResS;
};

class InstRasValue : public InstBase
{
public:
  InstRasValue(Instructions* instrucs, const Map&);
  ~InstRasValue();
  void Exec();
  virtual void ReplaceMapUsed(const Map& mp);
  virtual void GetMapUsed(Map&);
  virtual void AddDependentObjects(ObjectDependency& objdep);
  Map mp;
  StackObjectType sot;
  Representation rpr;
  LongBuf bufRow, bufCol;
  LongBuf bufResL;
  RealBuf bufResR;
  StringBuf bufResS;
};

class InstMapCrd : public InstBase
{
public:
  InstMapCrd(Instructions* instrucs, const Map&);
  ~InstMapCrd();
  void Exec();
  virtual void ReplaceMapUsed(const Map& mp);
  virtual void GetMapUsed(Map&);
  virtual void AddDependentObjects(ObjectDependency& objdep);
  DomainValueRangeStruct dvsCoord;
  GeoRef gr;
  LongBuf bufRow, bufCol;
  CoordBuf bufRes;
  Map mp;
};

class InstMapLineCol : public InstBase
{
public:
  InstMapLineCol(Instructions* instrucs, const Map&, const CoordSystem& csIn, bool fCol);
  ~InstMapLineCol();
  void Exec();
  virtual void ReplaceMapUsed(const Map& mp);
  virtual void GetMapUsed(Map&);
  virtual void AddDependentObjects(ObjectDependency& objdep);
  GeoRef gr;
  CoordSystem csIn, csOut;
  bool fTransform;
  bool fCol;
  DomainValueRangeStruct dvrs;
  CoordBuf bufCrd;
  LongBuf bufRes;
  Map mp;
};


class InstMapColor : public InstBase
{
public:
  InstMapColor(Instructions* instrucs, const Map&);
  ~InstMapColor();
  void Exec();
  virtual void ReplaceMapUsed(const Map& mp);
  virtual void GetMapUsed(Map&);
  virtual void AddDependentObjects(ObjectDependency& objdep);
  DomainValueRangeStruct dvsColor;
  Representation rpr;
  Map mp;
  bool fColor;
  LongBuf bufRow, bufCol;
  LongBuf bufRes;
  RealBuf bufVal;
};

class InstRprColor : public InstBase
{
public:
  InstRprColor(Instructions* instrucs, const Representation&);
  ~InstRprColor();
  void Exec();
  virtual void AddDependentObjects(ObjectDependency& objdep);
  DomainValueRangeStruct dvsColor;
  Representation rpr;
  bool fRprClass;
  LongBuf bufRes;
  LongBuf bufValL;
  RealBuf bufValR;
  StringBuf bufValS;
};

class InstPntCrd : public InstBase
{
public:
  InstPntCrd(Instructions* instrucs, const PointMap&);
  ~InstPntCrd();
  void Exec();
  virtual void AddDependentObjects(ObjectDependency& objdep);
  DomainValueRangeStruct dvsCoord;
  PointMap pmp;
  LongBuf bufIndex;
  CoordBuf bufRes;
};

class InstPntVal : public InstBase
{
public:
  InstPntVal(Instructions* instrucs, const PointMap&);
  ~InstPntVal();
  void Exec();
  virtual void AddDependentObjects(ObjectDependency& objdep);
  PointMap pmp;
  StackObjectType sot;
  LongBuf bufIndex;
  LongBuf bufResL;
  RealBuf bufResR;
  StringBuf bufResS;
  CoordBuf bufResC;
};

class InstPntNr : public InstBase
{
public:
  InstPntNr(Instructions* instrucs, const PointMap&);
  ~InstPntNr();
  void Exec();
  virtual void AddDependentObjects(ObjectDependency& objdep);
  PointMap pmp;
  DomainValueRangeStruct dvrs;
  LongBuf bufRes;
  StringBuf bufStrIndex;
};

class InstInMask : public InstBase
{
public:
  InstInMask(Instructions* instrucs);
  ~InstInMask();
  void Exec();
  Mask msk;
  StringBuf bufMask, bufStr;
  LongBuf bufRes;
  DomainValueRangeStruct dvsBool;
};

class InstNbStart : public InstBase
{
public:
  InstNbStart(Instructions* instrucs, int iNbrs);
  ~InstNbStart();
  void Exec();
  int iNbrs;
};

class InstNbEnd : public InstBase
{
public:
  InstNbEnd(Instructions* instrucs);
  ~InstNbEnd();
  void Exec();
};

class InstNbMinMax : public InstBase
{
public:
  InstNbMinMax(Instructions* instrucs, bool fMin, bool fPos, bool fCond);
  ~InstNbMinMax();
  void Exec();
  LongBuf bufL1, bufL2, bufPos, bufCond;
  RealBuf bufR1, bufR2;
  bool fMin, fPosit, fCond;
  DomainValueRangeStruct dvsNbPos;
};

class InstNbPrd : public InstBase
{
public:
  InstNbPrd(Instructions* instrucs, bool fPos, bool fCond, bool fReal);
  ~InstNbPrd();
  void Exec();
  LongBuf bufL, bufPos;
  RealBuf bufR;
  bool fPosit, fCond, fReal;
  DomainValueRangeStruct dvsNbPos;
  LongBuf abufL[9];
  RealBuf abufR[9];
  LongBuf abufCond[9];
  double arVal[9];
  long aiVal[9];
  long aiCnt[9];
  long aiPos[9];
};

class InstNbSum : public InstBase
{
public:
  InstNbSum(Instructions* instrucs, const DomainValueRangeStruct&);
  ~InstNbSum();
  void Exec();
  LongBuf bufL1, bufL2;
  RealBuf bufR1, bufR2;
  StackObjectType sot;
  DomainValueRangeStruct dvs;
};

class InstNbAvg : public InstBase
{
public:
  InstNbAvg(Instructions* instrucs, const DomainValueRangeStruct&);
  ~InstNbAvg();
  void Exec();
  LongBuf bufL1, bufL2;
  RealBuf bufR1, bufR2;
  ByteBuf bufCnt;
  StackObjectType sot;
  DomainValueRangeStruct dvs;
};

class InstNbStd : public InstBase
{
public:
  InstNbStd(Instructions* instrucs, const DomainValueRangeStruct&);
  ~InstNbStd();
  void Exec();
  RealBuf bufR1, bufR2, bufRR;
  ByteBuf bufCnt;
  StackObjectType sot;
  DomainValueRangeStruct dvs;
};

class InstNbCnt : public InstBase
{
public:
  InstNbCnt(Instructions* instrucs);
  ~InstNbCnt();
  void Exec();
  LongBuf bufCond, bufCnt;
  DomainValueRangeStruct dvsCnt;
};

class InstNbPos : public InstBase
{
public:
  InstNbPos(Instructions* instrucs);
  ~InstNbPos();
  void Exec();
  LongBuf bufPos;
  DomainValueRangeStruct dvsNbPos;
};

class InstNbCondPos : public InstBase
{
public:
  InstNbCondPos(Instructions* instrucs);
  ~InstNbCondPos();
  void Exec();
  LongBuf bufPos, bufCond;
  DomainValueRangeStruct dvsNbPos;
};


class InstNbDis : public InstBase
{
public:
  InstNbDis(Instructions* instrucs);
  ~InstNbDis();
  void Exec();
  RealBuf bufDis;
  DomainValueRangeStruct dvsNbDis;
};

class DATEXPORT FilterLinear;
class InstNbFlt : public InstBase
{
public:
  InstNbFlt(Instructions* instrucs, FilterLinear*);
  ~InstNbFlt();
  void Exec();
  DomainValueRangeStruct dvsNbFlt;
  RealBuf bufFlt;
  RealArray arFlt;
};

class InstTblValue : public InstBase
{
public:
	InstTblValue(Instructions* instrucs, const Table& _tbl);
	void Exec();
	virtual void AddDependentObjects(ObjectDependency& objdep);

	Table tbl;	

  StackObjectType sot;
  LongBuf bufRow;
  LongBuf bufResL;
  RealBuf bufResR;
  StringBuf bufResS, bufCol, sbufRow;	
};


static  Array<InstBase*> aibDummy;
//static  Array<InstList*> ailDummy;
//static  Stack<InstList*> stilDummy;


#endif




