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
/* FunctionPtr
   Copyright Ilwis System Development ITC
   June 1995, by Jelle Wind
	Last change:  JEL  29 May 97   12:09 pm
*/

#include "Engine\Function\FUNCTION.H"
#include "Engine\Function\FILTER.H"
#include "Engine\Function\FUNCUSER.H"
#include "Engine\Function\CLASSIFR.H"
#include "Engine\Base\DataObjects\valrange.h"
#include "Engine\Base\DataObjects\ObjectStructure.h"
#include "Engine\Base\DataObjects\ERR.H"
#include "Engine\Base\System\mutex.h"
#include "Engine\Applications\objvirt.h"
#include "Engine\Applications\ModuleMap.h"
#include "Engine\Base\System\Engine.h"

IlwisObjectPtrList Function::listFnc;

Function::Function()
: IlwisObject(listFnc)
{
}

Function::Function(const Function& fnc)
: IlwisObject(listFnc, fnc.pointer())
{
}

Function::Function(const FileName& filename)
: IlwisObject(listFnc, filename)
{
  if (!pointer())
    SetPointer(FunctionPtr::create(filename));
}

Function::Function(IlwisObjectPtr* ptr)
: IlwisObject(listFnc, ptr)
{
}

FunctionPtr* Function::pGet(const FileName& fn)
{
  return static_cast<FunctionPtr*>(listFnc.pGet(fn));
}

FunctionPtr::~FunctionPtr()
{
}

FunctionPtr* FunctionPtr::create(const FileName& fn)
{
  FileName filnam = fn;
  if (!File::fExist(filnam)) { // check std dir
    filnam.Dir(getEngine()->getContext()->sStdDir());
    if (!File::fExist(filnam))
      NotFoundError(fn);
  }
  String sType;
  ObjectInfo::ReadElement("Function", "Type", filnam, sType);
  if (fCIStrEqual("Filter" , sType))
    return FilterPtr::create(filnam);
  if (fCIStrEqual("Classifier" , sType))
    return ClassifierPtr::create(filnam);
  if (fCIStrEqual("FuncUser" , sType))
    return new FuncUserPtr(filnam);
  InvalidTypeError(fn, "Function", sType);

  return NULL;
}

FunctionPtr::FunctionPtr(const FileName& fn)
: IlwisObjectPtr(fn)
{
  if (!File::fExist(fn))
    NotFoundError(fn);
  String s;
  if (0 != ReadElement("Function", "DefaultDomain", s))
    ReadElement("Function", "DefaultDomain", dmDeflt);
  ReadElement("Function", "DefaultValueRange", vrDeflt);
  if (dmDeflt.fValid() && (!vrDeflt.fValid()))
    vrDeflt = ValueRange(dmDeflt);
}

FunctionPtr::FunctionPtr(const FileName& fn, const Domain& dmDefault)
: IlwisObjectPtr(fn, true, fn.sExt.scVal())
{
  dmDeflt = dmDefault;
  if (dmDeflt.fValid())
    vrDeflt = ValueRange(dmDeflt);
}

String FunctionPtr::sType() const
{
  return "Function";
}

void FunctionPtr::Store()
{
  IlwisObjectPtr::Store();
  WriteElement("Ilwis", "Type", "Function");
  WriteElement("Function", "DefaultDomain", dmDefault());
  WriteElement("Function", "DefaultValueRange", vrDeflt);
}

Domain FunctionPtr::dmDefault() const
{
  return dmDeflt;
}

ValueRange FunctionPtr::vrDefault() const
{
  return vrDeflt;
}

void FunctionPtr::SetDefaultDomain(const Domain& dm)
{
  dmDeflt = dm;
}

void FunctionPtr::SetDefaultValueRange(const ValueRange& vr)
{
  vrDeflt = vr;
}

void FunctionPtr::GetObjectStructure(ObjectStructure& os)
{
	IlwisObjectPtr::GetObjectStructure(os);
}

void FunctionPtr::DoNotUpdate()
{
	IlwisObjectPtr::DoNotUpdate();
	
}




