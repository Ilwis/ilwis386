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
/* calc.h
Jelle Wind, ILWIS System Development
june 1997
	Last change:  WK   14 Apr 98    4:21 pm
*/
#ifndef CALC_H
#define CALC_H
#include "Engine\Base\objdepen.h"
#include "Engine\Scripting\Parser.h"
#include "Engine\Scripting\CODEGEN.H"
#include "Engine\Scripting\Instrucs.h"

class CALCEXPORT Instructions;
class CALCEXPORT UserDefFuncDesc;
class ParserObject;

//! Class for access to the calculator functionality.
class Calculator
{
public:
  _export Calculator(const Table& tbl, const String& sExpr);
  _export Calculator(const String& sExpr, const FileName& fn);
  _export Calculator(const String& sExpr);
  _export ~Calculator();
  Instructions _export *instMapCalc(ObjectDependency& objdep, bool& fNoGeoRef);
  Instructions _export *instColCalc(ObjectDependency& objdep);
  void _export ExecColumnIndexStore();
  UserDefFuncDesc _export *fdsParse(Instructions*);
  static String _export sSimpleCalc(const String& sExpr);
  static String _export sSimpleCalc(const String& sExpr, const Table& tbl);
  static Instructions _export *instExprX(const String& sExpr);
  static Instructions _export *instExprXY(const String& sExpr);
  static Instructions _export *instCoordFunc(const String& sFuncName);
protected:
private:
  ParserObject* po;
  long iRecs;
};

#endif




