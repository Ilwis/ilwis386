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
/* Function, FunctionPtr
   Copyright Ilwis System Development ITC
   june 1995, by Jelle Wind
	Last change:  JEL  29 May 97   12:10 pm
*/

#ifndef ILWFUNCTION_H
#define ILWFUNCTION_H
#include "Engine\Base\DataObjects\ilwisobj.h"
#include "Engine\Domain\dm.h"
#include "Engine\Base\DataObjects\valrange.h"

class DATEXPORT FunctionPtr: public IlwisObjectPtr
{
  friend class DATEXPORT Function;
  static FunctionPtr* create(const FileName&);
public:
  virtual ~FunctionPtr();
  virtual void Store();
  virtual String sType() const;
  virtual Domain dmDefault() const;
  virtual ValueRange vrDefault() const;
  void SetDefaultDomain(const Domain& dm);
  void SetDefaultValueRange(const ValueRange& vr);
	virtual void _export GetObjectStructure(ObjectStructure& os);
	virtual void _export DoNotUpdate();	
	
protected:
  FunctionPtr(){}
  FunctionPtr(const FileName&);
  FunctionPtr(const FileName&, const Domain& dmDefault);
  Domain dmDeflt;
  ValueRange vrDeflt;
};

class Function: public IlwisObject
{
  static IlwisObjectPtrList listFnc;
protected:
  Function(IlwisObjectPtr* ptr);
public:
  Function();
  _export Function(const FileName& filename);
  Function(const Function& fnc);
  void operator = (const Function& fnc) { SetPointer(fnc.pointer()); }
  FunctionPtr* ptr() const { return static_cast<FunctionPtr*>(pointer()); }
  FunctionPtr* operator -> () const { return ptr(); }
  static FunctionPtr*  pGet(const FileName& fn);
};

#endif




