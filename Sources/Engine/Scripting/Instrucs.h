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
/*
// $Log: /ILWIS 3.0/Calculator/Instrucs.h $
 * 
 * 11    28-09-00 2:57p Martin
 * instructions has now the tranquilizer as member to be able to prevent
 * endless lopps from functions. Access function included
 * 
 * 10    14-02-00 16:13 Wind
 * added functions to retrieve dependent objects from instructions (bug
 * 404)
 * 
 * 9     18-01-00 9:50 Wind
 * instruction list with 'gotofalse' instructions are now executed pixel
 * by pixel, or record by record
 * 
 * 8     5-01-00 18:12 Wind
 * some cosmetic changes (renaming of some functions, removing of others)
 * 
 * 7     5-01-00 11:09 Wind
 * rename class ExternFuncDesc to UserDefFuncDesc
 * 
 * 6     4-01-00 10:28 Wind
 * partial redesign and simplification to solve some bugs related with
 * user defined functions
 * 
 * 5     22-12-99 10:29 Wind
 * change in enum nbDir
 * deleted function undef(Label)
 * 
 * 4     8-11-99 9:03 Wind
 * protected moved
 * 
 * 3     8-11-99 9:02 Wind
 * comment
 * 
 * 2     5-11-99 13:06 Wind
 * doxygen comment
*/
// Revision 1.3  1998/09/16 17:30:36  Wim
// 22beta2
//
// Revision 1.2  1997/07/25 10:27:31  Wim
// Latest changes of Jelle
//
/* instrucs.h
   Jelle Wind, april 1995
	Last change:  WK   14 Apr 98    4:30 pm
*/
#ifndef ILW_INSTRUCSH
#define ILW_INSTRUCSH
#include "Engine\Domain\dm.h"
#include "Engine\SpatialReference\Gr.h"
#include "Engine\Map\Raster\Map.h"
#include "Engine\Table\Col.h"
#include "Engine\Table\tbl.h"
#include "Engine\Base\DataObjects\Stack.h"
#include "Engine\Scripting\CALCSTCK.H"
#include "Engine\Scripting\FUNCS.H"
#include "Engine\Scripting\INSTRUC.H"
#include "Engine\Scripting\DMMERGER.H"
#include <vector>
#include <stack>

class CALCEXPORT ExternFuncsDesc;
class DATEXPORT DomainGroup;

#include "Engine\Scripting\Calcvar.h"

#define DEFBUFSIZE 2048L
enum dirNb { dirNbDOWN=1, dirNbUP, dirNbLEFT, dirNbRIGHT };

//!
class CALCEXPORT Neighbours : public Array<int>
{
public:
  Neighbours() { }
  void Set(int iNbrs, dirNb dnb);
  void Reset() { Resize(0); }
};

//!
class CALCEXPORT CalcEnv {
public:
  CalcEnv();
  long iLines, iCols, iRecs;
  long iCurLine, iCurCol, iCurRec;
  long iBufSize;
  int iCurrNb;
  Neighbours nb;
  bool fNeighbAgg;
  dirNb dnb;
};


//!
class UserDefFuncDesc {
public:
  UserDefFuncDesc(const String& sNam, VarType vt = vtVALUE);
  int iParms() const  { return iPrms; } 
  String sName;
  VarType vtRes;
  Domain dmDefault;
  int iPrms;
};


//!
class Instructions// : public ErrorHandling
{
  friend class CALCEXPORT Calculator;
  friend class CALCEXPORT CodeGenerator;
  friend class CALCEXPORT InstStop; // access to Stop()
  friend class CALCEXPORT InstLoad; // access to fNbPosInstUsed
  // for access to iNextInst and lstLabels, lstInst :
  friend class CALCEXPORT InstGoto;
  friend class CALCEXPORT InstGotoFalse;
  friend class CALCEXPORT InstFuncEnd;
public:
  _export Instructions();
  _export ~Instructions();
  const Domain& dm() const { return _dm; }
  const ValueRange& vr() const { return _vr; }
  const GeoRef& gr() const { return _gr; }
  RowCol _export rcSize() const;
  // for pocket line calculate
  double _export rCalcVal();
  String sCalcVal();
  Coord cCalcVal();
  double _export rCalcVal(double); // for GraphCalcTupel
  double _export rCalcVal(double,double); // for CoordSystemFormula
  void _export CalcCoords(const CoordBuf& cBufIn, CoordBuf& cBufOut);
  void _export CalcCoords(const Coord& cIn, Coord& cOut);
  // for map calculate
  double _export rMapCalcVal(RowCol rc);
  long _export iMapCalcVal(RowCol rc);
  long _export iMapCalcRaw(RowCol rc);
  void _export MapCalcVal(long iLine, RealBuf& buf, long iFrom, long iNum);
  void _export MapCalcVal(long iLine, LongBuf& buf, long iFrom, long iNum);
  void _export MapCalcRaw(long iLine, LongBuf& buf, long iFrom, long iNum);
  void _export SetGeoRef(const GeoRef& grf);
  void _export GetMapsUsed(Array<Map>& amp);
  void _export ReplaceMapUsed(const Map& map1, const Map& map2);
  void SetNbPosInstUsed();
  int iNewLabel(); // returns new label number and adds a label to lstLabels
  // for column calculate
  void _export ColumnCalcVal(StringBuf& buf, long iKey, long iNum, short iWid=0, short iDec =-1);
  void _export ColumnCalcVal(RealBuf& buf, long iKey, long iNum);
  void _export ColumnCalcVal(LongBuf& buf, long iKey, long iNum);
  void _export ColumnCalcVal(CoordBuf& buf, long iKey, long iNum);
  void ColumnCalcRaw(LongBuf& buf, long iKey, long iNum);
  double _export rColumnCalcVal(long iKey);
  long iColumnCalcVal(long iKey);
  String _export sColumnCalcVal(long iKey, short iWid=0, short iDec =-1);
  Coord _export cColumnCalcVal(long iKey);
  long iColumnCalcRaw(long iKey);
  // for map and column calculate
  void _export ChangeConstStringsToRaw(const Domain& dm);
  // for lienar filter calc
  static Instructions _export *instSimple(const String& sCalc);
  void _export GetVars(Array<CalcVariable>& acv);
  void Get(ObjectDependency& objdep);
	void _export SetTranquilizer(Tranquilizer *_trq);
	Tranquilizer *trqTranquilizer();

  // public members
  Stack<StackObject*> stkCalc;
  stack<vector<StackObject*> > stkParms;
  CalcEnv env;
  ArrayDomainMerger admmrg;
  DomainValueRangeStruct dvrsValue; // place holder for a domain value (efficiency)
protected:
private:
  void Add(InstBase* inst);
  void Stop() { fStop = true; }
  Array<InstBase*> lstInst;
  Array<int>     lstLabels;
//  Array<DomainGroup*> aGroupDom; // for adding to dependencies
  bool fStop;
  int iNextInst;
  Domain _dm;
  ValueRange _vr;
  GeoRef _gr;
  void CalcExec(); // execute instructions
  bool fNbPosInstUsed;
	Tranquilizer *trq;	
  bool fGotoFalseInstUsed() const;
};

#endif
