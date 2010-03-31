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
	Last change:  WK   11 Sep 97    3:16 pm
*/
#ifndef FUNCS_H
#define FUNCS_H

#include "Engine\Domain\dm.h"
#include "Engine\Base\DataObjects\ARRAY.H"

#include "Engine\Scripting\CALCSTCK.H"
#include "Engine\Scripting\Calcvar.h"

//!
typedef void CALCEXPORT (*InternalFunc)(...);

//!
typedef double (*FuncMath)(double r);
//!
typedef void CALCEXPORT (*FuncResultDomain)(DomainValueRangeStruct& dvs, StackObjectType& sot, FuncMath,
                                            const Array<CalcVariable>&);
//!
typedef bool CALCEXPORT (*FuncCheckDomains)(const Array<CalcVariable>& acv, int iStartIndex, int& iWrongParm);

//!
class CALCEXPORT InternFuncDesc {
public:
  InternFuncDesc(char* sFuncName, InternalFunc fi, FuncMath fm, FuncResultDomain fdom,
                 FuncCheckDomains fchdm, short iPrms, VarType vtRes ...);
  InternFuncDesc* fdsCopy() const;
  short iParms()
    { return vtl.iSize(); }
  bool fCheckParms(const Array<CalcVariable>& acv, int& iWrongParm) const;
  static void CreateInternFuncs(Array<InternFuncDesc*>&);
  static InternFuncDesc* fdFindFunc(const Array<InternFuncDesc*>& fdl, const String& sFunc,
                     Array<CalcVariable>& acv);
  char* sFunc;
  InternalFunc fp;
  FuncResultDomain fpdm;
  FuncCheckDomains fpchdm;
  FuncMath fmath;
  VarType vtRes;
  Array<VarType> vtl;
};


#endif
