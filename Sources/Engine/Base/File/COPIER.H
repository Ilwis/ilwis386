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
/* IlwisObject, IlwisObjectPtr, IlwisObjectPtrList
   Copyright Ilwis System Development ITC
   march 1995, by Wim Koolhoven
	Last change:  JEL  20 Jun 97   12:13 pm
*/

#ifndef ILWCOPIER_H
#define ILWCOPIER_H
#include "Engine\Base\DataObjects\ilwisobj.h"
#include "Engine\Base\DataObjects\Tranq.h"

class Copier
{
public:
  Copier();
  // copy one object to other dir. :
  _export Copier(const FileName& fnObj, const String& sDestPath,          
         bool fBreakDep=false);
  // copy one object to same dir. with other name :
  _export Copier(const FileName& fnObj, const FileName& fnObjCopy, 
         const FileName& fnAttTable, const FileName& fnAttTableCopy, bool fBreakDep=false); 
  // copy multiple objects to other dir. :
  Copier(const Array<FileName>& afnObj, const String& sDestPath);
  // returns object names that already exist :
  void _export CheckExist(Array<FileName>& fnObjExist);
  // excludes objects from copying
  void _export ExcludeObjects(Array<FileName>& fnObjExcl);
  void _export Exec();
  bool _export fSufficientDiskSpace(unsigned __int64 & iSpaceRequired, unsigned __int64 & iSpaceAvailable);
  void ConstructObjectList();
  // only in same dir:  
  static void _export Copy(const FileName& fn, const FileName& fnCopy, bool fBreakDep=false); 
  // only to other dir:
  static void _export Copy(const FileName& fn, const String& sDestDir, bool fBreakDep=false);
  // copy files to other dir
  static void _export Copy(const Array<FileName>& afn, const String& sDestDir, bool fBreakDep=false);
private:
  void AddObject(const FileName& fnObj);
  void AddObjectAndProp(const FileName& fnObj);
  void AddObject(const Domain& dm); // also add representation
  void AddObjects(const Array<FileName>& afnObj);
  void init(const String& sDestPath, bool fCopyAttTable, bool fBreakDep);
  void SetDestinationPath(const String& sDestPath);
  void SetCopyAttTable(const FileName& fnAttTableCopy);
  void SetBreakDependency(bool f);
  void DetermineDataFiles();
  bool fSingleCopy() const;
  bool fSameDirCopy() const;
  Array<FileName> afnObj;    // list of objects to be copied
  Array<FileName> afnData;   // all files to be copied
  Array<FileName> afnObjExclSav; // files to be excluded from copy
  Array<String> asSection;   // section in object file for data file
  Array<String> asEntry;     // entry in object file for data file
  String sDestPath, sSystemDir;
  bool fCopyAttTable, fBreakDep, fIncludeSystemFiles, fCopyDep, fObjectListConstructed;
  FileName fnObjCopy;        // file name for copy in current dir.
  FileName fnAttTable;       // file name for att.table when copy in current dir.
  FileName fnAttTableCopy;   // file name for new att.table when copy in current dir.
  Tranquilizer trq;
};

#endif




