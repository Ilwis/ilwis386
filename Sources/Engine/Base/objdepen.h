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
/* ObjectDependency
   Copyright Ilwis System Development ITC
   feb. 1996, by Jelle Wind
	Last change:  WK   18 Jul 97   10:14 am
*/
#ifndef ILW_OBJDEP_H
#define ILW_OBJDEP_H

#include "Engine\Base\DataObjects\ilwisobj.h"

class ObjectDependency
{
public:
  _export ObjectDependency();
  _export ObjectDependency(const ObjectDependency&);
  _export ObjectDependency(const FileName&);
  _export ObjectDependency(const FileName&, const String& sColName);
  _export ~ObjectDependency();
  void _export operator = (const ObjectDependency&);
  void _export Add(const IlwisObject& obj);
  void _export Remove(const IlwisObject& obj);
  void AddCol(const String& sColName);
  void _export Add(IlwisObjectPtr* obj);
  void Add(const ObjectDependency& objdep);
  bool _export fMakeAllUsable(bool fFreezeAlways=true) const;
    // returns false if a dependency object is not frozen
  bool fCalcAll();
    // returns false if a dependency object is not frozen
  bool _export fUpdateAll();
    // returns false if a dependency object is not frozen
//bool fAllFrozenValid() const;
    // returns true if all dependencies are valid in frozen form
  bool _export fAllOlder(const ObjectTime& tm) const; 
    // returns true if all dependencies are older than tm
  bool fUpToDate() const;
    // returns true if all dependencies are up to date
  ObjectTime _export tmNewest() const;
//  void Copy(const FileName& fn) const;
    // copy all dependent objects to the directory of fn
  void _export Store(IlwisObjectPtr* ptr, const String& sSection = "");
  bool _export fUses(const FileName& fn) const;
  bool _export fUses(const IlwisObjectPtr* ptr) const;
  bool _export fUses(const Column& col) const;
  void Clear();
  void _export Names(Array<FileName>& afn) const;
  // returns file names of dependencies (for columns: the table name)
  static void _export Read(const FileName& fn, Array<FileName>& afnObj); 
  // returns file names of dependencies (for columns: the table name)
  void _export Names(const String& sRelPath, Array<String>& asNames) const; // returns names of dependencies
  // in printable form (columns are preceded by the word Column)
  // sPath is the relative path
  static void _export ReadNames(const FileName& fn, Array<String>& asNames);  // returns names of dependencies
  // in printable form (columns are preceded by the word Column)
  static void _export ReadNames(const String& sSection, const FileName& fn, Array<String>& asNames);  
  // returns names of dependencies in section sSection
  // in printable form (columns are preceded by the word Column)
  static void _export Read(const FileName& fn, Array<FileName>& afnObj, Array<String>& asColName);
  // returns file names of dependencies and names of columns
  static void _export Read(const String& sSection, const FileName& fn, Array<FileName>& afnObj, Array<String>& asColName);
  // returns file names of dependencies in section sSection
  // and names of columns
  static void _export GetNewestDependentObject(const FileName& fn, const String& sColN, ObjectTime tmObj,
                                      String& sObjName, ObjectTime& tmNewer, Array<FileName>& afnChecked);
  // return (printable, columns preceded by the word Column) name and time of a dependent 
  // object that is newer than the current object
private:
  Array<IlwisObject*> objs;  
};

#endif





