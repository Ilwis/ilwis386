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
/* FuncUser, FuncUserPtr
   Copyright Ilwis System Development ITC
   june 1995, by Jelle Wind
	Last change:  WK   14 Jul 97   12:55 pm
*/

#ifndef ILWFNCUSR_H
#define ILWFNCUSR_H
#include "Engine\Function\FUNCTION.H"

class CALCEXPORT UserDefFuncDesc;
class CALCEXPORT Instructions;

class FuncUserPtr: public FunctionPtr
{
  friend class DATEXPORT FuncUser;
  friend class DATEXPORT FunctionPtr;
public:
  virtual ~FuncUserPtr();
  virtual void Store();
  virtual String sType() const;
  const String _export &sFuncDef() { return sDef; }
  void  _export SetFuncDef(const String& sFuncDef);
  bool _export fFuncDefOk(const String& sFuncDef);
  int iParms() const { return _iParms; }
  UserDefFuncDesc _export *fdsParse(Instructions*);
  virtual void GetDataFiles(Array<FileName>& afnDat, Array<String>* asSection=0, Array<String>* asEntry=0) const;
  virtual void Rename(const FileName& fnNew);
	virtual void _export GetObjectStructure(ObjectStructure& os);
	virtual void _export DoNotUpdate();		
protected:
  FuncUserPtr(const FileName&);
  FuncUserPtr(const FileName&, const String& sFuncDef, bool fOnlyRetExpr=true);
private:
  String sGenerateDef(const String& sFuncDef);
  String sDef;
  UserDefFuncDesc* _fds;
  int _iParms;
  FileName fnFuncDef;
};

class FuncUser: public Function
{
protected:
  FuncUser(IlwisObjectPtr* ptr);
public:
  _export FuncUser();
  _export FuncUser(const FileName& filename);
  FuncUser(const FuncUser& fnc);
  _export FuncUser(const FileName& filename, const String& sFuncDef, bool fOnlyRetExpr=true);
  FuncUser(const String& sFuncName);
  void operator = (const FuncUser& fnc) { SetPointer(fnc.pointer()); }
  FuncUserPtr* ptr() const { return static_cast<FuncUserPtr*>(pointer()); }
  FuncUserPtr* operator -> () const { return ptr(); }
  static void import(const FileName& fnFnc14);
};

#endif




