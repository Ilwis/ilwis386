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
#pragma warning( disable : 4786 )

#include "Engine\Table\tbl.h"
#include "Engine\Base\DataObjects\ObjectCollection.h"
#include "Engine\Applications\ObjectCollectionVirtual.h"
#include "Engine\DataExchange\DatabaseCollection.h"
#include "Engine\DataExchange\ForeignCollection.h"
#include "Engine\Base\objdepen.h"
#include "Engine\Base\DataObjects\ObjectStructure.h"
#include "Engine\Base\DataObjects\valrange.h"
#include "Engine\Table\Col.h"
#include "Engine\Applications\objvirt.h"
#include "Engine\Applications\ModuleMap.h"
#include "Engine\Base\System\Engine.h"
#include "Engine\DataExchange\ForeignFormat.h"
#include "engine\map\basemap.h"
#include "Engine\Base\DataObjects\URL.h"
#include "Engine\DataExchange\WMSCollection.h"
#include "Engine\Base\System\mutex.h"

typedef vector<FileName>::iterator FileNameIter;
const String sTypeObjectCollection("ObjectCollection");

mapCreates	ObjectCollection::mapCreateFuncs;

IlwisObjectPtrList ObjectCollection::listObjectCollection;


ObjectCollection::ObjectCollection()
: IlwisObject(listObjectCollection)
{
	Init();
}

ObjectCollection::ObjectCollection(const ObjectCollection& mv)
: IlwisObject(listObjectCollection, mv.pointer()) 
{
		Init();
}

ObjectCollection::ObjectCollection(const FileName& fn,  const String& sExpr) 
{
  FileName fnCol(fn, ".ioc", false);
  MutexFileName mut(fnCol);
  SetPointer(ObjectCollectionPtr::create(fnCol,sExpr));
  if (fValid())
    ptr()->Store();
}

ObjectCollection::ObjectCollection(const FileName& fn)
 : IlwisObject(listObjectCollection, FileName(fn,".ioc"))
{
		Init();	
	
	MutexFileName mut(fn);
	String sCollectionType;
	ObjectInfo::ReadElement("ObjectCollection", "Type", fn, sCollectionType);
	if ( sCollectionType == "" ) //  for old collections
		ObjectInfo::ReadElement("Ilwis", "Type", fn, sCollectionType);
	SetPointer(Create(fn, sCollectionType, ParmList()));
}

ObjectCollection::ObjectCollection(const FileName& fn, const String& sType, ParmList& pm)
 : IlwisObject(listObjectCollection)
{
	Init();
	SetPointer(Create(fn, sType, pm));
}

ObjectCollectionPtr *ObjectCollection::Create(const FileName& fn, const String& sType, ParmList& pm)
{
	if ( sType == "ObjectCollectionVirtual") {
		return new ObjectCollectionPtr(fn);
	} else {
		mapCreates::iterator where = ObjectCollection::mapCreateFuncs.find(sType);	
		if ( where != mapCreateFuncs.end() )
		{
			CollectionCreate func = (*where).second;
			return (func)(fn, pm);
		}
	}
	return 0;		
}

ObjectCollectionPtr* ObjectCollectionPtr::create(const FileName& fn, const String& sExpression)
{
  if (fn.sFile.length() == 0) { // no file name
    char *p = sExpression.strrchrQuoted('.');
    char *q = sExpression.strrchrQuoted('\\');
    if (p<q)
    	p = 0;
    // Now we're (only) sure that if p!=0 it points to the last dot in the filename
    if ((p == 0) || (0 == _strcmpi(p, ".ioc"))) { // no attrib map
      FileName fnCol(sExpression, ".ioc", true);
      if (File::fExist(fnCol)) {
        // see if map is already open (if it is in the list of open base maps)
        MutexFileName mut(fnCol);
		ObjectCollectionPtr* p = ObjectCollection::pGet(fnCol);
        if (p) // if already open return it
          return p;
        // open map and return it
        return ObjectCollection::Create(fnCol,"ObjectCollection");
      }
    }  
  }
  bool fPrevExist = File::fExist(fn);
  ObjectCollectionPtr* p = new ObjectCollectionPtr(fn, true);
  bool fPrevErase = p->fErase;
  p->fErase = true;
  p->ocv = ObjectCollectionVirtual::create(fn, *p, sExpression);
  p->fErase = fPrevErase;
  if ((0 != p->ocv) && fPrevExist) {
    //ObjectCollectionStore::UnStore(fn);
//  unlink(fn.sFullName(true)); // delete previous object def file
    ObjectInfo::WriteElement("ObjectDependency", (char*)0, fn, (char*)0);
    ObjectInfo::WriteElement("ObjectCollectionVirtual", (char*)0, fn, (char*)0);
  }  
  if (0 != p->ocv) {
     p->SetExpressionChangeable(p->ocv->fExpressionChangeable());
  }
  return p;
}


void ObjectCollection::Init()
{
	mapCreateFuncs[sTypeObjectCollection] = ObjectCollectionPtr::Create;
	mapCreateFuncs[sTypeDataBaseCollection] = DataBaseCollectionPtr::Create;				
	mapCreateFuncs[sTypeForeignCollection] = ForeignCollectionPtr::Create;					
	mapCreateFuncs[sTypeWMSCollection] = WMSCollectionPtr::Create;					
}

ObjectCollectionPtr *ObjectCollection::pGet(const FileName& fn)
{
  if (!fn.fValid())
    return 0;

  ILWISSingleLock(&listObjectCollection.csAccess, TRUE,SOURCE_LOCATION);
  for (DLIterCP<IlwisObjectPtr> iter(&listObjectCollection); iter.fValid(); ++iter) {
    ObjectCollectionPtr* db = dynamic_cast<ObjectCollectionPtr*>(iter());
    if (db && (fn == db->fnObj))
      return db;



  }
  return 0;
}

//-----------------------------------------------------------------------------------------------------------
ObjectCollectionPtr::~ObjectCollectionPtr()
{
	if ( fErase )
	{
		for(unsigned int i=0; i < arObjects.size(); ++i)
		{
			FileName fn = arObjects[0];
			Remove(fn);
		}
	}
}

ObjectCollectionPtr::ObjectCollectionPtr(const FileName& fn) 
: IlwisObjectPtr(fn), ocv(0)
{
	long iNr;
	ReadElement("ObjectCollection", "NrObjects", iNr);
	ReadElement("ObjectCollection", "AttributeTable", attTable);
	long ft;
	ReadElement("ObjectCollection", "FilterTypes", ft);
	for(int i=0; i < ft; ++i) {
		long ft;
		ReadElement("ObjectCollection", String("FilterType%d",i).scVal(), ft);
		filterTypes.insert((IObjectType)ft);
	}
	FileName fnObject;
	for(int i = 0; i < iNr; ++i)
	{
		ReadElement("ObjectCollection", String("Object%d", i).scVal(), fnObject);
		arObjects.push_back(fnObject);
	}
}

ObjectCollectionPtr::ObjectCollectionPtr(const FileName& fn, bool fCreate)
: IlwisObjectPtr(fn, fCreate), ocv(0)
{
}

ObjectCollectionPtr *ObjectCollectionPtr::Create(const FileName& fn, ParmList& )
{
	MutexFileName mut(fn);
	ObjectCollectionPtr *ocp = ObjectCollection::pGet(fn);
	if (ocp)
	{ // if already open return it
		return ocp;
	}	
	if ( fn.fExist())
		return new ObjectCollectionPtr(fn);
	else
		return new ObjectCollectionPtr(fn, true);
}

bool ObjectCollectionPtr::fCalculated() const
// returns true if a calculated result exists
{
  if (!fDependent())
    return IlwisObjectPtr::fCalculated();
  ILWISSingleLock sl(const_cast<CCriticalSection*>(&csCalc), TRUE);
  return 0 != ocv;
} 

void ObjectCollectionPtr::BreakDependency() {
   if (!fCalculated())
    Calc(true);
  if (!fCalculated())
    return; 
  ILWISSingleLock sl(&csAccess, TRUE, SOURCE_LOCATION);
  delete ocv;
  ocv = 0;
  fChanged = true;
//  _fDataReadOnly = false;
  Store();
}

void ObjectCollectionPtr::OpenCollectionVirtual()
{
  if (0 != ocv) // already there
    return;
  if (!fDependent())
    return;
  try {
    ocv = ObjectCollectionVirtual::create(fnObj, *this);
    objdep = ObjectDependency(fnObj);
  }
  catch (const ErrorObject& err) {
    err.Show();
    ocv = 0;
    objdep = ObjectDependency();
  }
}

void ObjectCollectionPtr::Calc(bool fMakeUpToDate)
// calculates the result     
{
  ILWISSingleLock sl(&csCalc, TRUE, SOURCE_LOCATION);
  OpenCollectionVirtual();
  if (fMakeUpToDate)
    if (!objdep.fUpdateAll())
      return;
  if (0 != ocv) 
    ocv->Freeze();
}

bool ObjectCollectionPtr::fCanAdd(const FileName& fn)
{
	return fn.fExist();
}

String ObjectCollectionPtr::sType() const
{
	return sTypeObjectCollection;
}

void ObjectCollectionPtr::Store()
{
	IlwisObjectPtr::Store();
	WriteElement("Ilwis", "Type", "ObjectCollection");
	WritePrivateProfileString("ObjectCollection", NULL, NULL, fnObj.sFullPath().scVal()); // remove entire section
	if ( sType() != sTypeObjectCollection) // else it gets circular in on open document
		WriteElement("ObjectCollection", "Type", sType());	
	WriteElement("ObjectCollection", "NrObjects", (long)arObjects.size());
	WriteElement("ObjectCollection", "AttributeTable", attTable);
	long sz = filterTypes.size();
	WriteElement("ObjectCollection", "FilterTypes", sz);
	int count = 0;
	for(set<IObjectType>::iterator cur=filterTypes.begin(); cur != filterTypes.end(); ++cur) {
		long ft = (*cur);
		WriteElement("ObjectCollection", String("FilterType%d",count++).scVal(), ft);
	}


	for(unsigned int i=0; i < arObjects.size(); ++i)
	{
		// storing may have started from a different directory than the oc is. sRelative does not work properly then
		// all files may be in the same dir but they still get a full path because current dir is different
		if ( fCIStrEqual(arObjects[i].sPath(), fnObj.sPath()) )
			WriteElement("ObjectCollection", String("Object%i", i).scVal(), arObjects[i].sFileExtQuoted());
		else
			WriteElement("ObjectCollection", String("Object%i", i).scVal(), arObjects[i].sFullPathQuoted());
	}
	if ( fDependent())
		WriteElement("ObjectCollection", "Type", "ObjectCollectionVirtual");
	if (ocv) 
		ocv->Store();
}

bool ObjectCollectionPtr::fDependent() const
{
  ILWISSingleLock sl(const_cast<CCriticalSection*>(&csAccess), TRUE, SOURCE_LOCATION);
  if (0 != ocv)
    return true;
  String s;
  ReadElement("ObjectCollection", "Type", s);
  return fCIStrEqual(s , "ObjectCollectionVirtual");
}

void ObjectCollectionPtr::SetAttributeTable(const Table& tbl){
	if (tbl->iRecs() != arObjects.size())
		throw ErrorObject(TR("Table's number of records doesn't match number of objects in the collection"));
	attTable = tbl;
	fChanged = true;
}

void ObjectCollectionPtr::GetObjectDependencies(Array<FileName>& afnObjDep)
{
}

void ObjectCollectionPtr::GetDataFiles(Array<FileName>& afnDat, Array<String>* asSection, Array<String>* asEntry) const
{
	IlwisObjectPtr::GetDataFiles(afnDat, asSection, asEntry);
}

void ObjectCollectionPtr::Add(const IlwisObject& obj)
{
	Add(obj->fnObj);
}

void ObjectCollectionPtr::Add(const FileName& fnObject)
{
	set<IObjectType>::iterator whre = filterTypes.find(IOTYPE(fnObject));
	if ( whre == filterTypes.end() && filterTypes.size() != 0)
		return;

	FileNameIter cur = find(arObjects.begin(), arObjects.end(), fnObject);
	ObjectInfo::WriteAdditionOfFileToCollection(fnObject, fnObj);
	if (cur != arObjects.end())	{
		return;
	}
	arObjects.push_back(fnObject);
	Store();
}

// remove single object from object collection
// this will also remove the link from the contained object to the ioc
void ObjectCollectionPtr::Remove(const IlwisObject& obj)
{
	Remove(obj->fnObj);
}

// remove single object from object collection
// this will also remove the link from the contained object to the ioc
void ObjectCollectionPtr::Remove(const FileName& fnObject)
{
	int iNr=0;
	FileNameIter cur = find(arObjects.begin(), arObjects.end(), fnObject);
	if (cur != arObjects.end())
		arObjects.erase(cur);	

	ObjectInfo::WriteRemovalOfFileFromCollection(fnObject, fnObj);
	Updated();
}

// remove all objects from object collection
// this will also remove the links from the contained objects to the ioc
void ObjectCollectionPtr::RemoveAllObjects()
{
	for (FileNameIter cur = arObjects.begin(); cur != arObjects.end(); ++cur)
		ObjectInfo::WriteRemovalOfFileFromCollection(*cur, fnObj);

	arObjects.clear();
	Updated();
}

void ObjectCollectionPtr::DeleteEntireCollection()
{
	String sFiles;
	for(vector<FileName>::iterator cur=arObjects.begin(); cur != arObjects.end(); ++cur)
	{
		try
		{
			IlwisObject object = IlwisObject::obj(*cur);
			if ( object.fValid())
			{
				sFiles += object->fnObj.sFullPathQuoted() + " ";
			}				
		}
		catch(ErrorObject& )
		{}
	}

	if ( sFiles != "" )
	{
		String sCmd("*del %S -quiet -force", sFiles);
		getEngine()->Execute(sCmd);
	}		
  fErase = true;
}

FileName ObjectCollectionPtr::fnObject(int i)
{
	if ( i < arObjects.size())
		return arObjects[i];

	return FileName();
}

IlwisObject ObjectCollectionPtr::ioObj(int i)
{
	return IlwisObject::obj(arObjects[i]);
}

int ObjectCollectionPtr::iNrObjects()
{
	return arObjects.size();
}

void ObjectCollectionPtr::GetObjectStructure(ObjectStructure& os)
{
	IlwisObjectPtr::GetObjectStructure(os);
	if (os.fGetAssociatedFiles() ||
		( os.caGetCommandAction() != ObjectStructure::caCOPY &&
		  os.caGetCommandAction() != ObjectStructure::caDELETE)
		)
	{
		int iNr;
		ObjectInfo::ReadElement("ObjectCollection", "NrObjects", fnObj, iNr);
		if ( iNr != iUNDEF)
		{
			for(int i=0; i<iNr; ++i)
			{
				String sEntry("Object%d", i);
				os.AddFile(fnObj, "ObjectCollection", sEntry, "", os.fRetrieveContentsContainer() ? false : true);
			}	
		}
	}		
}

// checks if it depends on objects outside the collection, it may depend on objects in the collection
bool ObjectCollectionPtr::fUsesDependentObjects() const
{
	for(vector<FileName>::const_iterator cur=arObjects.begin(); cur != arObjects.end(); ++cur)
	{
		IlwisObject obj = IlwisObject::obj(*cur);
		if ( obj.fValid() )
		{
			if ( obj->fDependent() )
			{
				Array<FileName> arNames;
				// get the names of the the objects this object depends on
				// all objects in this list must be in the collection to disqualify this object
				// that is. all dependent objects are part of the collection
				obj->GetObjectDependencies(arNames);
				int iCount=0;
				for(unsigned int i = 0; i < arNames.size(); ++i )
				{
					for(unsigned int j=0; j < arObjects.size(); ++j )
					{
						if ( arNames[i] == arObjects[j])
						{
							++iCount;
							if ( iCount == arNames.size() )
								return false;
						}								
					}					
				}					
				return true;				
			}				
		}			
	}		
	return false;
}

bool ObjectCollectionPtr::fObjectAlreadyInCollection(const FileName& fn) const
{
	for(vector<FileName>::const_iterator cur=arObjects.begin(); cur != arObjects.end(); ++cur)
	{
		if ( *cur == fn )	
			return true;
	}	
	return false;
}

bool ObjectCollectionPtr::fTblAtt() const {
	return attTable.fValid();
}
Table ObjectCollectionPtr::tblAtt() const {
	return attTable;
}

set<IObjectType> ObjectCollectionPtr::getFilterTypes() const{
	return filterTypes;
}

void ObjectCollectionPtr::addFilterType(IObjectType type){
	filterTypes.insert(type);
}

void ObjectCollectionPtr::removeFilterType(IObjectType type){
	filterTypes.erase(type);
}

bool ObjectCollectionPtr::isBaseMapOnly() const {
	for(vector<FileName>::const_iterator cur=arObjects.begin(); cur != arObjects.end(); ++cur)	 {
		if ( !IOTYPEBASEMAP(*cur)) {
			return false;
		}
	}
	return true;
}

CoordBounds ObjectCollectionPtr::cb() const {
	CoordSystem cs;
	CoordBounds cb;
	for(vector<FileName>::const_iterator cur=arObjects.begin(); cur != arObjects.end(); ++cur)	 {
		if ( IOTYPEBASEMAP(*cur)) {
			BaseMap bm((*cur));
			if ( !cs.fValid() || cs->fUnknown()) {
				cs = bm->cs();
				cb += bm->cb();
			} else {
				if ( cs == bm->cs()) {
					cb += bm->cb();
				} else {
					CoordBounds cb2 = cs->cbConv(bm->cs(), cb);
					cb += cb2;
				}
			}
		}
	}
	return cb;
}