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

#include "Engine\DataExchange\DatabaseCollection.h"
#include "Engine\Table\tbl.h"
#include "Engine\DataExchange\ForeignFormat.h"
#include "Engine\Base\System\mutex.h"
#include "Headers\Hs\DAT.hs"

DataBaseCollection::DataBaseCollection()
{
	Init();		
}

DataBaseCollection::DataBaseCollection(const FileName& fn, ParmList& pm) :
	ForeignCollection(fn, sTypeDataBaseCollection, pm)
{
}	

DataBaseCollection::DataBaseCollection(const FileName& fn) :
	ForeignCollection(fn)
{


}		

DataBaseCollection::~DataBaseCollection()
{
}

FileName DataBaseCollection::createCollectionName(const String& name) {
	return FileName();
}

//---------------------------------------------------------------------
DataBaseCollectionPtr::DataBaseCollectionPtr(const FileName& fn, ParmList& pm) :
	ForeignCollectionPtr(fn, pm)
{
	sDataBaseName = pm.sGet("database");
	if ( sDataBaseName == "" )
		ReadElement("DataBaseCollection", "DataBaseName", sDataBaseName);
	if ( sMethod == "")
		sMethod = pm.sGet("method");
	if ( sMethod == "") // used for older db collections, they did not contain this info
		sMethod = "ADO";
}

DataBaseCollectionPtr::~DataBaseCollectionPtr()
{
}

void DataBaseCollectionPtr::Fill()
{
	//ForeignFormat *ff = ForeignFormat::Create("", sExpr);
	//delete ff;
}

void DataBaseCollectionPtr::GetObjectDependencies(Array<FileName>& afnObjDep)
{
//  ObjectDependency(fnObj).Names(afnObjDep);
}

bool DataBaseCollectionPtr::fCanAdd(const FileName& fn)
{
	return true;
}

ObjectCollectionPtr *DataBaseCollectionPtr::Create(const FileName& fn, ParmList& pm)
{
	MutexFileName mut(fn);
	DataBaseCollectionPtr *dcp = dynamic_cast<DataBaseCollectionPtr *>(ObjectCollection::pGet(fn));	
	if (dcp)
	{ // if already open return it
		return dcp;
	}		
	return new DataBaseCollectionPtr(fn, pm);
}

void DataBaseCollectionPtr::Add(const FileName& fnTable)
{
	if ( IlwisObject::iotObjectType(fnTable) == IlwisObject::iotTABLE )
	{
		ObjectCollectionPtr::Add(fnTable);
		// non-existing tables (from databases may just bee created). They dont have a collection section yet
		// add it now
		if ( fnTable.fExist() && ObjectInfo::fInCollection(fnTable) == false)
		{
			ObjectInfo::WriteAdditionOfFileToCollection(fnTable, fnObj)	;
		}			
	}		
}

void DataBaseCollectionPtr::Create(ParmList& pm)
{
	ForeignCollectionPtr::Create(pm);
}

String DataBaseCollectionPtr::sContainer()
{
	return sDataBaseName;
}

void DataBaseCollectionPtr::Store()
{
	ForeignCollectionPtr::Store();
	WriteElement("Ilwis", "Type", sType());	
	WriteElement("DataBaseCollection", "DataBaseName", sDataBaseName);
}

String DataBaseCollectionPtr::sType() const
{
	return sTypeDataBaseCollection;
}


void DataBaseCollectionPtr::CreateImpliciteObject(const FileName& fnNew, ParmList& parms) {
	parms.Add(new Parm(0, String("%S\\%S", sContainer(), parms.sGet("table"))));	
	parms.Add(new Parm("collection", fnObj.sRelative()));
	ForeignCollection::CreateImplicitObject(fnNew, parms);	
}


