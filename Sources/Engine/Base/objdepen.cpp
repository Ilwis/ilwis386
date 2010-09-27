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
/* $Log: /ILWIS 3.0/BasicDataStructures/objdepen.cpp $
 * 
 * 11    22-04-04 10:23 Willem
 * [Bug=6471]
 * - Changed: the call to GetNewestDependentObject now uses an array. This
 * array is used to keep track of all files that already have been
 * checked. Because of the recursive nature of the function indefinite
 * loops are a danger. This is now prevented by first checking the array
 * of checked objects
 * 
 * 10    19-11-02 17:21 Willem
 * Changed: Because the Array::iSize() is now unsigned int a cast is
 * needed to resolve the WriteElement call (only int and long available)
 * 
 * 9     16-08-01 19:00 Koolhoven
 * simplified fMakeAllUsable() so that it works for all Ilwis Objects
 * 
 * 8     25-01-01 2:00p Martin
 * the remove function did not delete the member so it left a reference
 * and memory
 * 
 * 7     21-12-00 1:04p Martin
 * object virtuals added for dependenciy information
 * 
 * 6     1-03-00 11:45 Wind
 * added member function to remove objects from the dependency list
 * 
 * 5     3-11-99 12:31 Wind
 * case insensitive comparison
 * 
 * 4     29-10-99 12:58 Wind
 * case sensitive stuff
 * 
 * 3     9/08/99 10:08a Wind
 * adpated to use of quoted file and column names
 * 
 * 2     3/11/99 12:15p Martin
 * Added support for Case insesitive 
// Revision 1.3  1998/09/16 17:22:46  Wim
// 22beta2
//
// Revision 1.2  1997/08/27 09:09:51  Wim
// In fMakeAllUsable() do not recalculate columns and tables,
// just like maps.
//
/* ObjectDependency
   Copyright Ilwis System Development ITC
   feb. 1996, by Jelle Wind
	Last change:  WK   27 Aug 97   11:06 am
*/

#include "Engine\Base\objdepen.h"
#include "Engine\Applications\MAPVIRT.H"
#include "Engine\Applications\SEGVIRT.H"
#include "Engine\Applications\POLVIRT.H"
#include "Engine\Applications\PNTVIRT.H"
#include "Engine\Applications\TBLVIRT.H"
#include "Engine\Applications\COLVIRT.H"
#include "Engine\Map\Raster\MapList\maplist.h"
#include "Engine\Function\FILTER.H"

ObjectDependency::ObjectDependency()
{
}

ObjectDependency::ObjectDependency(const ObjectDependency& objdep)
{
  Add(objdep);
}

ObjectDependency::ObjectDependency(const FileName& fnObj)
{
  Array<String> asColName;
  Array<FileName> afnObj;
  Read(fnObj, afnObj, asColName);
  unsigned int i;
  for (i=0; i < afnObj.iSize(); ++i) {
    IlwisObject io = IlwisObject::obj(afnObj[i]);
    Add(io);
  }
  for (i=0; i < asColName.iSize(); ++i) {
    Column col(asColName[i]);
    Add(col);
  }
}

ObjectDependency::ObjectDependency(const FileName& fnObj, const String& sColName)
{
  String sSection("Col:%S", sColName);
  int iNrDep = 0;
  String s;
  ObjectInfo::ReadElement(sSection.scVal(), "Type", fnObj, s);
  if (s == "ColumnStore")
    return;
  ObjectInfo::ReadElement(sSection.scVal(), "NrDepObjects", fnObj, iNrDep);
  FileName fn;
  Table tbl(fnObj);
  for (int i=0; i < iNrDep; i++) {
    String sEntry("Object%i", i);
    ObjectInfo::ReadElement(sSection.scVal(), sEntry.scVal(), fnObj, s);
    if (0 == strncmp("Column", s.scVal(), 6)) {
      String sCol = &s[7]; // skip "Column "
      if (0 != sCol.strchrQuoted('.')) // table.colname
        sCol = String("%S\\%S", fnObj.sPath(), sCol);
      Column col(tbl, sCol);
      Add(col);
    }
    else {
      ObjectInfo::ReadElement(sSection.scVal(), sEntry.scVal(), fnObj, fn);
      IlwisObject io = IlwisObject::obj(fn);
      Add(io);
    }  
  }
}

ObjectDependency::~ObjectDependency()
{
  for (unsigned int i = 0; i < objs.iSize(); ++i)
    delete objs[i];
}

void ObjectDependency::operator = (const ObjectDependency& objdep)
{
  for (unsigned int i = 0; i < objs.iSize(); ++i)
    delete objs[i];
  objs.Reset();  
  Add(objdep);
}

void ObjectDependency::Add(const ObjectDependency& objdep)
{
  for (unsigned int i = 0; i < objdep.objs.iSize(); i++)
    Add(*objdep.objs[i]);
}

void ObjectDependency::Add(const IlwisObject& obj)
{
  if (!obj.fValid())
    return;
  if (!obj->fnObj.fValid())
	{
		IlwisObjectVirtual *obv;
		if ( (obv = obj->pGetVirtualObject()) != NULL )
		{
			Add(obv->odGetObjectDependencies());
		}			
		return;
	}		
  for (unsigned int i=0; i < objs.iSize(); i++)
    if (obj.pointer() == objs[i]->pointer()) // already in list
      return;
  objs &= new IlwisObject(obj);
}

void ObjectDependency::Remove(const IlwisObject& obj)
{
  if (!obj.fValid())
    return;
  if (!obj->fnObj.fValid())
    return;
  for (unsigned int i=0; i < objs.iSize(); i++)
    if (obj.pointer() == objs[i]->pointer()) { // already in list
			delete objs[i];
      objs.Remove(i, 1);
      break;
    }
}

void ObjectDependency::AddCol(const String& sColName)
{
  Column col(sColName);
//  Add(col);
  if (!col.fValid())
    return;
  if (!col->fnObj.fValid())
    return;
  for (unsigned int i=0; i < objs.iSize(); i++)
    if (col.pointer() == objs[i]->pointer()) // already in list
      return;
  objs &= new IlwisObject(col);
}

void ObjectDependency::Add(IlwisObjectPtr* obj)
{
  if (0 == obj)
    return;
//  if (!obj->fnObj.fValid())
//    return;
  for (unsigned int i=0; i < objs.iSize(); i++)
    if (obj == objs[i]->pointer()) // already in list
      return;
  ColumnPtr* colp = dynamic_cast<ColumnPtr*>(obj);
  if (colp) {
    Column col;
    col.SetPointer(colp);
    Add(col);
    return;
  }    
  if (obj->fnObj.fValid()) {
    try {
      IlwisObject io = IlwisObject::obj(obj->fnObj);
      Add(io);
      return;
    }
    catch (ErrorObject&) { return; }   // forget it (seems to happen with histograms of newly created maps ??
  }
  BaseMapPtr* bmap = dynamic_cast<BaseMapPtr*>(obj);
  if (bmap && bmap->fDependent()) {
    Add(bmap->objdep);
    return;
  }
  ColumnPtr* col = dynamic_cast<ColumnPtr*>(obj);
  if (col && col->fDependent()) {
    Add(col->objdep);
    return;
  }
  TablePtr* tbl = dynamic_cast<TablePtr*>(obj);
  if (tbl && col->fDependent()) {
    Add(tbl->objdep);
    return;
  }
  MapListPtr *mpl = dynamic_cast<MapListPtr*>(obj);
  if (mpl) {
    for (int i= mpl->iLower(); i <= mpl->iUpper(); i++)
      Add(mpl->map(i));
    return;
  }
}

bool ObjectDependency::fMakeAllUsable(bool fFreezeAlways) const
{
  for (unsigned int i=0; i < objs.iSize(); ++i) 
  {
    IlwisObjectPtr* obj = objs[i]->pointer();
    if (0 != obj) {
      if (obj->fCalculated())
        continue;
      if (fFreezeAlways || !obj->fDefOnlyPossible()) {
        obj->Calc();
        if (!obj->fCalculated())
          return false;
      }  
    }
  }
  return true;
}

bool ObjectDependency::fUpdateAll()
{
  for (unsigned int i=0; i < objs.iSize(); i++) {
    IlwisObjectPtr* obj = objs[i]->pointer();
    obj->MakeUpToDate();
    if (!obj->fCalculated())
      return false;
  }
  return true;
}

bool ObjectDependency::fAllOlder(const ObjectTime& tm) const
{
  for (unsigned int i=0; i < objs.iSize(); i++) 
    if ((*objs[i])->objtime > tm)
      return false;
  for (unsigned int i=0; i < objs.iSize(); i++) {
    ColumnPtr* pCol = dynamic_cast<ColumnPtr*>(objs[i]->pointer());
    if (0 == pCol) {
      ObjectDependency objdep((*objs[i])->fnObj);
      if (!objdep.fAllOlder(tm))
        return false;
    }
    else {
      if (!pCol->objdep.fAllOlder(tm))
        return false;
//      ColumnVirtual* pcv = pCol->pcv();
//      if (0 != pcv)
//        if (!pcv->objdep.fAllOlder(tm))
//         return false;
//      ObjectDependency objdep((*objs[i])->fnObj, pCol->sName());
  //    if (!objdep.fAllOlder(tm))
//        return false;
    }  
  }  
  return true;
}

ObjectTime ObjectDependency::tmNewest() const
{
  ObjectTime tm;
  for (unsigned int i=0; i < objs.iSize(); i++)
    if ((*objs[i])->objtime > tm)
      tm = (*objs[i])->objtime;
  return tm;
}

bool ObjectDependency::fUpToDate() const
{
  for (unsigned int i=0; i < objs.iSize(); i++)
    if (!(*objs[i])->fUpToDate())
      return false;
  return true;
}

void ObjectDependency::Store(IlwisObjectPtr* ptr, const String& sSection) 
{
  String sSec = sSection;
  if (0 == sSection.length())
    sSec = "ObjectDependency";
  ptr->WriteElement(sSec.scVal(), "NrDepObjects", (long)objs.iSize());
  String s;
  for (unsigned int i=0; i < objs.iSize(); i++) {
    String s("Object%i", i);
    ColumnPtr* pCol = dynamic_cast<ColumnPtr*>(objs[i]->pointer());
    if (0 == pCol)
      ptr->WriteElement(sSec.scVal(), s.scVal(), (*objs[i])->sNameQuoted(true, ptr->fnObj.sPath()));
    else {
      if (ptr->fnObj == pCol->fnTbl)
        ptr->WriteElement(sSec.scVal(), s.scVal(), String("Column %S", pCol->sNameQuoted()));
      else
        ptr->WriteElement(sSec.scVal(), s.scVal(), String("Column %S", pCol->sTableAndColumnName(ptr->fnObj.sPath())));
    }  
  }
}

bool ObjectDependency::fUses(const FileName& fn) const
{
  for (unsigned int i=0; i < objs.iSize(); i++)
    if (fn == objs[i]->pointer()->fnObj)
      return true;
  return false;
}

bool ObjectDependency::fUses(const IlwisObjectPtr* ptr) const
{
  for (unsigned int i=0; i < objs.iSize(); i++)
    if (ptr == objs[i]->pointer())
      return true;
  return false;
}

bool ObjectDependency::fUses(const Column& col) const
{
  for (unsigned int i=0; i < objs.iSize(); i++)
    if (col.ptr() == objs[i]->pointer())
      return true;
  return false;
}

void ObjectDependency::Names(const String& sPath, Array<String>& asNames) const
{
  asNames.Resize(objs.iSize());
  for (unsigned int i=0; i < objs.iSize(); i++) {
    ColumnPtr* pCol = dynamic_cast<ColumnPtr*>(objs[i]->pointer());
    if (0 == pCol)
      asNames[i] = (*objs[i])->sNameQuoted(true, sPath);
    else
      asNames[i] = String("Column %S.%S", pCol->fnTbl.sRelativeQuoted(false, sPath), pCol->sNameQuoted());
  }  
}

void ObjectDependency::ReadNames(const FileName& fn, Array<String>& asNames)
{
  ReadNames("ObjectDependency", fn, asNames);
/*  Array<FileName> afnObj;
  Array<String> asColName;
  asNames.Resize(0);
  Read(fn, afnObj, asColName);
  for (int i=0; i < afnObj.iSize(); ++i)
    asNames &= afnObj[i].sRelative(true, fn.sPath());
  for (i=0; i < asColName.iSize(); ++i)
    asNames &= String("Column %S", asColName[i]);*/
}

void ObjectDependency::ReadNames(const String& sSection, const FileName& fn, Array<String>& asNames)
{
  Array<FileName> afnObj;
  Array<String> asColName;
  asNames.Resize(0);
  Read(sSection, fn, afnObj, asColName);
  for (unsigned int i=0; i < afnObj.iSize(); ++i)
    asNames &= afnObj[i].sRelative(true, fn.sPath());
  for (unsigned int i=0; i < asColName.iSize(); ++i)
    asNames &= String("Column %S", asColName[i]);
}

void ObjectDependency::Names(Array<FileName>& afn) const
{
  for (unsigned int i=0; i < objs.iSize(); ++i) {
    unsigned int j =0;
    for (; j < afn.iSize(); ++j)
      if (afn[j] == (*objs[i])->fnObj)
        break;
    if (j == afn.iSize()) // not found
      afn &= (*objs[i])->fnObj;
  }  
}

void ObjectDependency::Clear()
{
  objs.Resize(0);
}

void ObjectDependency::Read(const FileName& fnObj, Array<FileName>& afnObj, Array<String>& asColName)
{
  afnObj.Resize(0);
  asColName.Resize(0);
  String sType;
  ObjectInfo::ReadElement("Ilwis", "Type", fnObj, sType);
  if ((fCIStrEqual(sType , "BaseMap"))  || (fCIStrEqual(sType , "Table")))
    if (!ObjectInfo::fDependent(fnObj))
      return;
  Read("ObjectDependency", fnObj, afnObj, asColName);
/*ObjectInfo::ReadElement("ObjectDependency", "NrDepObjects", fnObj, iNrDep);
  FileName fn;
  String s;
  for (int i=0; i < iNrDep; i++) {
    String sEntry("Object%i", i);
    ObjectInfo::ReadElement("ObjectDependency", sEntry, fnObj, s);
    if (0 == strncmp("Column", s, 6)) {
      String sCol;
      if (sType == "Table") {
        sCol = fnObj.sFile;
        sCol &= '.';
      }  
      sCol &= &s[7]; // skip "Column "
      char* p = strrchr((char*)sCol, '.');
      if (p != 0) {
        int iTblNameSize = p-(char*)sCol;
        FileName fnTbl(sCol.sLeft(iTblNameSize), ".tbt", false);
        if (fnTbl.sPath() != fnObj.sPath())
          sCol = String("%S.%S", fnTbl.sFullName(fnTbl.sExt != ".tbt"), sCol.sRight(sCol.length()-iTblNameSize-1));
      }
      asColName &= sCol;
    }
    else {
      ObjectInfo::ReadElement("ObjectDependency", sEntry, fnObj, fn);
      afnObj &= fn;
      AddFileName(fn, afnObj);
    }  
  }*/
}

static void Split(String& sCol, FileName& fnTbl, String& sColName)
{
  char* ps = sCol.sVal(); 
  if (0 == strncmp(ps, "Column ", 7)) // skip Column prefix
    ps = ps+7;
  String s = ps;
  char* p = s.strrchrQuoted('.');
  if (p != 0) {
    int iTblNameSize = p-(char*)&s[0];
    fnTbl = FileName(s.sLeft(iTblNameSize), ".tbt", false);
    sColName = p+1;
  }
  else
    sColName = s;
}

void ObjectDependency::Read(const String& sSection, const FileName& fnObj, Array<FileName>& afnObj, Array<String>& asColName)
{
  int iNrDep = 0;
  ObjectInfo::ReadElement(sSection.scVal(), "NrDepObjects", fnObj, iNrDep);
  FileName fn;
  String s, sType;
  for (int i=0; i < iNrDep; i++) {
    String sEntry("Object%i", i);
    ObjectInfo::ReadElement(sSection.scVal(), sEntry.scVal(), fnObj, s);
    if (0 == strncmp("Column", s.scVal(), 6)) {
      String sCol;
      if (fCIStrEqual(sType , "Table")) {
        sCol = fnObj.sFile;
        sCol &= '.';
      }  
      sCol &= &s[7]; // skip "Column "
      FileName fnTbl;
      Split(sCol, fnTbl, s);
      if (fnTbl.fValid()) {
        if (fnTbl.sPath() != fnObj.sPath())
          sCol = String("%S.%S", fnTbl.sFullNameQuoted(!fCIStrEqual(fnTbl.sExt,".tbt")), s);
      }
      asColName &= sCol;
    }
    else {
      ObjectInfo::ReadElement(sSection.scVal(), sEntry.scVal(), fnObj, fn);
      afnObj &= fn;
      AddFileName(fn, afnObj);
    }  
  }
}

void ObjectDependency::Read(const FileName& fnObj, Array<FileName>& afnObj)
{
  Array<String> asColNames;
  ObjectDependency::Read(fnObj, afnObj, asColNames);
  for (unsigned int k=0; k < asColNames.iSize(); ++k) {
    char* p = asColNames[k].strrchrQuoted('.');
    if (p != 0) {
      int iTblNameSize = p-asColNames[k].sVal();
      FileName fnTbl(asColNames[k].sLeft(iTblNameSize), ".tbt", false);
      AddFileName(fnTbl, afnObj);
    }  
  }    
}

void ObjectDependency::GetNewestDependentObject(const FileName& fn, const String& sColN, ObjectTime tmObj, String& sObjName, ObjectTime& tmNewer, Array<FileName>& afnChecked)
{
	if (!File::fExist(fn)) {
		sObjName = fn.sFullNameQuoted();
		tmNewer = -1;
		return;
	}
	if (!ObjectInfo::fDependent(fn) && sColN.length() == 0)
		return;
	Array<FileName> afnObj;
	Array<String> asColName;
	FileName fnTbl;
	String sCol;
	ObjectTime tm;
	String sSection = "ObjectDependency";
	if (sColN.length() != 0)
		sSection = String("Col:%S", sColN);
	ObjectDependency::Read(sSection, fn, afnObj, asColName);
	for (unsigned int i=0; i < afnObj.iSize(); ++i) {
		tm = 0;
		ObjectInfo::ReadElement("Ilwis","Time", afnObj[i], tm);
		if ((tm > tmObj) && (tm > tmNewer)) {
			sObjName = afnObj[i].sRelative(true, fn.sPath());
			tmNewer = tm;
		}
	}
	for (unsigned int i=0; i < asColName.iSize(); ++i) {
		Split(asColName[i], fnTbl, sCol);
		if (!fnTbl.fValid())
			fnTbl = fn;
		tm = 0;
		ObjectInfo::ReadElement(String("Col:%S", sCol).scVal(), "Time", fnTbl, tm);
		if ((tm > tmObj) && (tm > tmNewer)) {
			sObjName = String("Column %S", asColName[i]);
			tmNewer = tm;
		}
	}

	for (unsigned int i=0; i < afnObj.iSize(); ++i)
	{
		if (!(afnObj[i] == fn))
		{
			bool fIsChecked = false;
			for (unsigned int index = 0; index < afnChecked.iSize(); ++index)
				fIsChecked = fIsChecked || afnObj[i] == afnChecked[index];
				
			if (!fIsChecked)  // prevent cyclic looping
			{
				afnChecked &= afnObj[i];
				GetNewestDependentObject(afnObj[i], String(), tmObj, sObjName, tmNewer, afnChecked);
				if (sObjName.length() > 0)
					return;
			}
		}
	}
	for (unsigned int i=0; i < asColName.iSize(); ++i)
	{
		Split(asColName[i], fnTbl, sCol);
		if (!fnTbl.fValid())
			fnTbl = fn;

		Array<FileName> afnChecked;
		GetNewestDependentObject(fnTbl, sCol, tmObj, sObjName, tmNewer, afnChecked);
		if (sObjName.length() > 0)
			return;
	}
}





