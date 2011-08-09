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
#include "Engine\SpatialReference\Coordsys.h"
#include "Engine\Base\DataObjects\valrange.h"
#include "Engine\Table\Col.h"
#include "Engine\DataExchange\ForeignFormat.h"
#include "Engine\DataExchange\TableForeign.h"

TableForeign::TableForeign(const FileName& fnObj, TablePtr& p, ParmList& pm) :
	TableStore(fnObj, p),
	fLoading(false),
	sMethod(pm.sGet("method")),
	sDataBase(pm.sGet("database")),
	sTableName(pm.sGet("table"))

{
}

void TableForeign::LoadBinary()
{
	ParmList pm;
	ForeignFormat *ff = ForeignFormat::Create(sMethod);
	ff->ReadParameters(ptr.fnObj, pm);
	delete ff;
	ff = ForeignFormat::Create(ptr.fnObj, pm);
	if ( ff )
	{
		if ( ff->GetTableDomain() != 0 )
			ptr.SetDomain(*(ff->GetTableDomain()));
		ff->LoadTable(&ptr);
	}		
	delete ff;
}

TableForeign::~TableForeign()
{
}

void TableForeign::Store()
{

	if ( ptr.fUseAs() )
	{
		ObjectInfo::WriteElement(ptr.sSection("TableStore").c_str(),"Type", ptr.fnObj, "TableForeign");
	/*	if ( sDataBase != "")
			ObjectInfo::WriteElement(ptr.sSection("ForeignFormat").c_str(),"DataBase", ptr.fnObj, sDataBase);		
		if ( sMethod != "")
			ObjectInfo::WriteElement(ptr.sSection("ForeignFormat").c_str(),"Method", ptr.fnObj, sMethod);*/
	}		
	else
	{
		ObjectInfo::RemoveSection(ptr.fnObj, "ForeignFormat");
		SetDataFile(FileName(ptr.fnObj, ".tb#"));
		StoreAsBinary(fnData());
	}
	
	TableStore::Store();	
//	if ( sTableName != "")
//		ObjectInfo::WriteElement(ptr.sSection("DataBaseCollection").c_str(),"TableName", ptr.fnObj, sTableName);	
}

void TableForeign::SetLoading(bool fYesNo)
{
	fLoading = fYesNo;
}

void TableForeign::PutStringInForeign(const String& sCol, long iRec, const String& str)
{
	if ( !fLoading) // do not write during loading
	{
		ParmList pm;
		ForeignFormat *ff = ForeignFormat::Create(ptr.fnObj,pm);
		if ( ff )
			ff->PutStringField(sCol, iRec, str);
		delete ff;
	}		
}

void TableForeign::PutByteInForeign(const String& sCol, long iRec, char str)
{
}
void TableForeign::PutUByteInForeign(const String& sCol, long iRec, unsigned char bV)
{
}
void TableForeign::PutULongInForeign(const String& sCol, long iRec, unsigned long iV)
{
}
void TableForeign::PutLongInForeign(const String& sCol, long iRec, long iV)	
{
}
void TableForeign::PutRealInForeign(const String& sCol, long iRec, double rV)		
{
}
void TableForeign::PutBoolInForeign(const String& sCol, long iRec, bool fV)		
{
}

void TableForeign::PutCoordInForeign(const String& sCol, long iRec, Coord cV)		
{
	if ( !fLoading) // do not write during loading
	{
		ParmList pm;
		ForeignFormat *ff = ForeignFormat::Create(ptr.fnObj, pm);
		if ( ff )
			ff->PutCoordField(sCol, iRec, cV);
		delete ff;
	}
}

Table TableForeign::CreateDataBaseTable(const FileName& fnObject, ParmList& pm)
{
	Table tbl;
	//ForeignFormat *ff = ForeignFormat::Create(pm.sGet("method"));
	ForeignFormat *ff = ForeignFormat::Create(fnObject, pm);
	if ( ff )
	{
		tbl.SetPointer(new TablePtr(fnObject, Domain("none"), pm));
		if ( !pm.fExist("import"))
			tbl->SetUseAs(true);
		else
		{
			tbl->LoadData();
			tbl->SetUseAs(false);			
		}
		tbl->Store();
		ff->Store(tbl);
		delete ff;
	}
	return tbl;
}
