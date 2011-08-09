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

#define DomainInfo ggDomainInfo

#undef DomainInfo

#pragma warning( disable : 4786 )

#include "Engine\Base\DataObjects\valrange.h"
#include "Engine\SpatialReference\Coordsys.h"
#include "Engine\Table\Col.h"
#include "Engine\DataExchange\ForeignCollection.h"
#include "Engine\DataExchange\ForeignFormat.h"
#include "Engine\Base\objdepen.h"
#include "Engine\Map\Point\PNT.H"
#include "Engine\Map\Segment\Seg.h"
#include "Engine\Map\Polygon\POL.H"
#include "Engine\Map\Raster\Map.h"
#include "Engine\Map\Raster\MapList\maplist.h"
#include "Engine\Base\System\mutex.h"
#include "Headers\constant.h"
#include "Headers\Hs\IMPEXP.hs"

static const int iGDB_NOT_FOUND = -1;

typedef vector<FileName>::iterator FileNameIter;

ForeignCollection::ForeignCollection()
: ObjectCollection()
{}

ForeignCollection::~ForeignCollection()
{
}

ForeignCollection::ForeignCollection(const ForeignCollection& mv)
: ObjectCollection(mv) 
{}

ForeignCollection::ForeignCollection(const FileName& fn)
 : ObjectCollection(fn)
{
  //SetPointer(new ForeignCollectionPtr(FileName(fn.sShortNameQuoted(),".ioc"), sExpr));
}

ForeignCollection::ForeignCollection(const FileName& fn, ParmList& pm)
 : ObjectCollection(FileName(fn.sShortNameQuoted(),".ioc"), sTypeForeignCollection, pm)
{
  //SetPointer(new ForeignCollectionPtr(FileName(fn.sShortNameQuoted(),".ioc"), sExpr));
}

ForeignCollection::ForeignCollection(const FileName& fn, const String& sType, ParmList& pm)
 : ObjectCollection(fn, sType, pm)
{
}

bool ForeignCollection::fForeignFileTimeChanged(const FileName& fnForeign, const FileName& fnObj)
{
	String sTime;
	long iLastModified;
	if ( ObjectInfo::ReadElement("ForeignFormat", "LastStoreTime", fnObj, iLastModified) == 0)
		return true;


	CTime LastWrite;
	if ( GetTime( fnForeign, LastWrite ))
	{
		return !(LastWrite.GetTime() == iLastModified );
	}
	return true;
}

bool ForeignCollection::GetTime(const FileName& fnForeign, CTime& LastWrite)
{
	if (!fnForeign.fExist()) 
    return false;
	CFileStatus status;
	CFile::GetStatus(fnForeign.sFullPath().c_str(), status);
	LastWrite = status.m_mtime;
	return true;
}

void ForeignCollection::CreateImplicitObject(const FileName& fnObject, ParmList& pm) {
	BaseMap map;
	String sC = pm.sGet("collection");
	FileName fnCollection(sC);
	ForeignCollection fc(fnCollection);
	if ( !fc.fValid() )
		throw ErrorObject(String(TR("Could not open collection %S").c_str(), fnCollection.sFile));
	
	if ( !pm.fExist("method"))
		pm.Add(new Parm("method", fc->sGetMethod()));
	String output = pm.sGet("output");
	FileName fn(output != "" ? output : fnObject.sFullPathQuoted());
	ForeignFormat *ff = ForeignFormat::Create(fn, pm);
	if ( ff )
	{
		if ( IlwisObject::iotObjectType(fn) == IlwisObject::iotTABLE) {
			CreateTable(fn, pm);
		}
		if ( IlwisObject::iotObjectType(fn) > IlwisObject::iotRASMAP && IlwisObject::iotObjectType(fn) < IlwisObject::iotTABLE) {
			CreateVectorMap(fn, ff->GetLayerInfo(pm));
		}
		delete ff;

		fc->Add(fnObject);
	}

}

void ForeignCollection::CreateTable(const FileName& fn, ParmList& pm) {
	Table tbl;
	if ( fn.fExist())
		tbl = Table(fn);
	else {
		tbl.SetPointer(new TablePtr(fn, Domain("none"),pm ));
	}
	if ( !pm.fExist("import"))
		tbl->SetUseAs(true);
	else
	{
		tbl->LoadData();
		tbl->SetUseAs(false);			
	}
	tbl->Store();
}

void  ForeignCollection::CreateVectorMap(const FileName& fn, LayerInfo li) {
	if ( IlwisObject::iotObjectType(fn) == IlwisObject::iotPOINTMAP) {
		PointMap pmp(fn, li);
		pmp->SetUseAs(true);
		pmp->Store();
	}
	if ( IlwisObject::iotObjectType(fn) == IlwisObject::iotSEGMENTMAP) {
		SegmentMap sm(fn, li);
		sm->Store();
	}
}

//-----------------------------------------------------------------------------------------------------------
ForeignCollectionPtr::~ForeignCollectionPtr()
{
	delete ff;
}

ForeignCollectionPtr::ForeignCollectionPtr(const FileName& fn, ParmList& pm) :
  ObjectCollectionPtr(fn),
  ff(NULL)
{
	if ( !pm.fExist("method"))
		ReadElement("ObjectCollection", "Method", sMethod);
	else
		sMethod = pm.sGet("method");

	ReadElement("ForeignCollection", "Expression", sExpression);
}

void ForeignCollectionPtr::Create(ParmList& pm)
{
  try
  {
	  	pm.Add(new Parm("collection",fnObj.sShortName()));
		ff = ForeignFormat::Create(fnObj, pm);
		if ( ff )
		{
			ff->PutDataInCollection(this, pm);
			Store();
		}
	}
	catch(ErrorObject& err)
	{
		err.Show();
		DeleteEntireCollection();
	}
}

String ForeignCollectionPtr::sType() const
{
  return "Foreign Collection";
}

void ForeignCollectionPtr::Store()
{
  ObjectCollectionPtr::Store();
	CTime ft;
	if ( ForeignCollection::GetTime(fnForeign, ft) || fnForeign.fExist() == false)
	{
		WriteElement("ForeignFormat", "LastStoreTime", (long)ft.GetTime());
		if ( sMethod != "")
			WriteElement("ObjectCollection", "Method", sMethod);
		if ( sExpression != "")
			WriteElement("ForeignCollection","Expression", sExpression);
	}

}

void ForeignCollectionPtr::GetObjectDependencies(Array<FileName>& afnObjDep)
{
//  ObjectDependency(fnObj).Names(afnObjDep);
}

void ForeignCollectionPtr::GetDataFiles(Array<FileName>& afnDat, Array<String>* asSection, Array<String>* asEntry) const
{
  ObjectCollectionPtr::GetDataFiles(afnDat, asSection, asEntry);
	afnDat &= fnForeign;
	if ( asSection)
		*asSection &= String();
	if ( asEntry)
		*asEntry &= String();
}

ObjectCollectionPtr *ForeignCollectionPtr::Create(const FileName& fn, ParmList& pm)
{
	MutexFileName mut(fn);
	ForeignCollectionPtr *dcp = dynamic_cast<ForeignCollectionPtr *>(ObjectCollection::pGet(fn));	
	if (dcp)
	{ // if already open return it
		return dcp;
	}	
	return new ForeignCollectionPtr(fn, pm);
}

String ForeignCollectionPtr::sGetMethod()
{
	return sMethod;
}

String ForeignCollectionPtr::sContainer() {
	return fnObj.sFile + fnObj.sExt;
}

String ForeignCollectionPtr::sGetExpression() {
	return sExpression;
}





