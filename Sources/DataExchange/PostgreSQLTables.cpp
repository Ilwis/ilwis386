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
//#include <ole2.h>

#pragma warning( disable : 4786 )

#include "DataExchange\gdalincludes\gdal.h"
#include "DataExchange\gdalincludes\gdal_frmts.h"
#include "DataExchange\gdalincludes\cpl_vsi.h"
#include "DataExchange\gdalincludes\ogr_srs_api.h"

#include "Engine\Table\tbl.h"
#include "Engine\DataExchange\DatabaseCollection.h"
#include "Engine\Domain\dmsort.h"
#include "Engine\DataExchange\ForeignFormat.h"
#include "Headers\Hs\IMPEXP.hs"
#include "Headers\constant.h"
#include "Engine\Applications\objvirt.h"
#include "Engine\Applications\ModuleMap.h"
#include "Engine\Base\System\Engine.h"
#include "DataExchange\PostgreSql.h"
#include "DataExchange\PostgreSQLTables.h"
#include "Engine\DataExchange\TableForeign.h"
#include "Engine\Domain\dmcoord.h"
#include "Engine\SpatialReference\Coordsys.h"
#include "Engine\SpatialReference\Csproj.h"
#include "Engine\SpatialReference\Cslatlon.h"
#include "Engine\Map\basemap.h"
#include "Engine\Domain\DomainUniqueID.h"
#include "Engine\Base\DataObjects\URL.h"
#include "Engine\Base\Algorithm\Rijndael.h"
#include "Headers\Hs\IMPEXP.hs"
//#include "temp\msado15.tlh"


String PostgreSQLTables::sFormatPostGreSQL(const FileName& fnForeign) {
	if ( fnForeign.sFile == "POSTGRESQL")
		return "POSTGRESQL";
	return "";
}
ForeignFormat *CreateQueryObjectPostGres() //create query object
{
	return new PostgreSQLTables();
}

ForeignFormat *CreateImportObjectPostGres(const FileName& fnObj, ParmList& pm) //create import object
{
	if ( pm.iSize() < 2 ) // existing object
	{
		String sMethod;
		if ( ObjectInfo::ReadElement("ForeignFormat", "method", fnObj, sMethod) != 0)	// oldstyle expressions
		{
			ForeignFormat *ff = ForeignFormat::Create(sMethod);
			if ( ff )
				ff->ReadParameters(fnObj, pm);
			delete ff;
		}	
	}

	FileName fnDom(pm.sGet("output") != "" ? pm.sGet("output") : fnObj.sFullPathQuoted());
	return new PostgreSQLTables(fnObj, Domain(fnDom,0, dmtUNIQUEID,"id"), pm);
}

PostgreSQLTables::PostgreSQLTables() :
	ForeignFormat()
{

}
//PostgreSQLTables::PostgreSQLTables(const FileName& fn, ParmList& pm) {
//	String sCol = pm.sGet("collection");
//}

PostgreSQLTables::PostgreSQLTables(const FileName& fn, const Domain & dm, ParmList& pm)
{
	int currentSize = pm.iSize();
	String collection = pm.sGet("collection");
	ForeignCollection db(FileName(collection.c_str()));
	sConnectionString = sCreateConnectionString(db, pm);
	tableName = pm.sGet("table");
	username = pm.sGet("username");
	password = pm.sGet("password");
	host = pm.sGet("host");
	port = pm.sGet("port");
	sQuery = pm.sGet("query");
	if ( sQuery == "" && tableName != "") // query
			sQuery = String("Select * FROM %S", tableName);		

	fnTable =FileName(pm.sGet(0).sTail("\\"));
	if ( fnTable.sFile == "")
		fnTable = fn;

	mtLoadType = mtTable; 
	if ( db.fValid()) {
		if ( tableName == "") {
			tableName = fn.sFile;
			pm.Add(new Parm("table", tableName));
		}
		if ( sQuery == "" && tableName != "") // query
			sQuery = String("Select * FROM %S", tableName);	
						
		Table tbl;
		FileName fnTbl(pm.sGet("output") != "" ? pm.sGet("output") : fn.sFullPathQuoted());
		tbl.SetPointer(new TablePtr(fnTbl, dm, pm));

		if ( !pm.fExist("import")) {
			tbl->SetUseAs(true);
			tbl->setVersionBinary(ILWIS::Version::bvFORMATFOREIGN);
		}
		else
		{
			tbl->LoadData();
			tbl->SetUseAs(false);
		}			
		tbl->Store();
		Store(tbl);
		db->Add(fnTbl);
	}
	/*else {
		Table tbl(fnTable);
		if ( tbl.fValid())
			tbl->setFormat(IlwisObjectPtr::fvFORMATFOREIGN);
	}*/
}

String PostgreSQLTables::sCreateConnectionString(const ForeignCollection& coll, ParmList& pm) {
	database = pm.sGet("database");
	if ( database == "" && coll.fValid()) {
		String sConn; 
		coll->ReadElement("ForeignFormat","ConnectionString",sConn);
		sConn = decrypt(coll->fnObj, sConn);
		Array<String> parts;
		Split(sConn,parts);
		host = parts[0].substr(6, parts[0].find_last_of("'") - 6);
		port = parts[1].substr(6, parts[1].find_last_of("'") - 6);
		database = parts[2].substr(8, parts[2].find_last_of("'") - 8);
		username = parts[3].substr(6, parts[3].find_last_of("'") - 6);
		password = parts[4].substr(10, parts[4].find_last_of("'") - 10);
		pm.Add(new Parm("host", host));
		pm.Add(new Parm("port", port));
		pm.Add(new Parm("database", database));
		pm.Add(new Parm("username", username));
		pm.Add(new Parm("password", password));

	}
	if ( database == "") {
		String collName = pm.sGet("collection");
		database = FileName(collName).sFile;
	}
	String host = pm.sGet("host");
	String uid = pm.sGet("username");
	String pwd = pm.sGet("password");
	String port = pm.sGet("port");

	return String("host='%S' port='%S' dbname='%S' user='%S' password='%S' connect_timeout='10'", host, port, database,uid, pwd);
}

String PostgreSQLTables::encrypt(const FileName& fn, const String& sentence) {
	String key = makeKey(fn);
	CRijndael rd;
	rd.MakeKey(key.c_str(), CRijndael::sm_chain0,32,32);
	String s = expandSentence(sentence);
    char out[1024];
	memset(out,0,1024);
	rd.Encrypt(s.c_str(), out, s.size(), CRijndael::CBC);
	String sOut;
	for(int i = 0; i < s.size(); ++i) {
		//sOut += String("%03d",(int)out[i]);
		unsigned char v = (unsigned char)out[i];
		int r = v % 26;
		int z = v / 26;
		char c1 = (char)65 + z;
		char c2 = (char)65 + r;
		sOut += String("%c%c",c1,c2);
	}
	return sOut;
}

String PostgreSQLTables::decrypt(const FileName& fn, const String& sentence) {
	int sz = sentence.size()/2;
	char in[1024];
	memset(in,0,1024);
	for(int i = 0; i < sz; ++i) {
		String sub = sentence.substr(i*2,2);
		char c1 = sub[0];
		char c2 = sub[1];
		in[i] = (unsigned char)(c2 - 65 + (c1 - 65)*26);
	}
	String key = makeKey(fn);
	CRijndael rd;
	rd.MakeKey(key.c_str(), CRijndael::sm_chain0,32,32);
	char out[1024];
	memset(out,0,1024);
	rd.Decrypt(in,out,sz,CRijndael::CBC);
	int size = String("%c%c%c",out[sz - 3],out[sz - 2],out[sz - 1]).iVal();
    return String(out).substr(0,size);

}

String PostgreSQLTables::makeKey(const FileName& fnObj) {
	String s = fnObj.sFile + fnObj.sExt;
	int i = 0;
	while ( s.size() < 32) {
		s += s[i++];
	}
	if ( s.size() > 32) {
		s = s.substr(0,31);
	}
	return s;
}

String PostgreSQLTables::expandSentence(const String& p) {
	String pw = p;
	int isz = pw.size();
	int k = isz / 32 + 1;
	int  i =0;
	while( pw.size() < k * 32 - 3) {
		pw += pw[i++];
	}
	pw = String("%S%03d",pw,isz);
	return pw;
}

void PostgreSQLTables::Store(IlwisObject obj) {
	ForeignFormat::Store(obj);
	obj->WriteElement("ForeignFormat","Method","POSTGRESQL");
	if( username!= "")
		obj->WriteElement("ForeignFormat","UserName",username);
	if( password != "")	{
		String out = encrypt(obj->fnObj,password);
		obj->WriteElement("ForeignFormat","Password",out);
	}
	if( sQuery != "")
		obj->WriteElement("ForeignFormat","Query",sQuery);
	if( database != "")
		obj->WriteElement("ForeignFormat","Database",database);
	if( sConnectionString != "") {
		String out = encrypt(obj->fnObj,sConnectionString);
		obj->WriteElement("ForeignFormat","ConnectionString",out);
	}
	if( tableName != "")
		obj->WriteElement("ForeignFormat","Table",tableName);
	if( host != "")
		obj->WriteElement("ForeignFormat","Host",host);
	if( port != "")
		obj->WriteElement("ForeignFormat","Port",port);
}
void PostgreSQLTables::ReadParameters(const FileName& fnObj, ParmList& pm) {
	ForeignFormat::ReadParameters(fnObj, pm);

	ObjectInfo::ReadElement("ForeignFormat","UserName",fnObj,username);
	pm.Add(new Parm("username",username));
	String pw;
	ObjectInfo::ReadElement("ForeignFormat","Password",fnObj,pw);
	password = decrypt(fnObj,pw);
	pm.Add(new Parm("password",password));
	ObjectInfo::ReadElement("ForeignFormat","Query",fnObj,sQuery);
	pm.Add(new Parm("query",sQuery));
	ObjectInfo::ReadElement("ForeignFormat","Database",fnObj,database);
	pm.Add(new Parm("database",database));
	String cs;
	ObjectInfo::ReadElement("ForeignFormat","ConnectionString",fnObj,cs);
	sConnectionString = decrypt(fnObj,cs);
	pm.Add(new Parm("connectionstring",sConnectionString));
	ObjectInfo::ReadElement("ForeignFormat","Table",fnObj, tableName);
	pm.Add(new Parm("table",tableName));
	ObjectInfo::ReadElement("ForeignFormat","Host",fnObj,host);
	pm.Add(new Parm("host",host));
	ObjectInfo::ReadElement("ForeignFormat","Port",fnObj,port);
	pm.Add(new Parm("port",port));

}

PostgreSQLTables::~PostgreSQLTables()
{
}

// fills a database collection with appropriate tables
void PostgreSQLTables::PutDataInCollection(ForeignCollectionPtr* collection, ParmList& pm)
{
	PostGreSQL db(sConnectionString.c_str());
	db.getNTResult("SELECT table_name, table_schema FROM INFORMATION_SCHEMA.Columns	WHERE table_schema = 'public'");
	int rows = db.getNumberOf(PostGreSQL::ROW);
	if ( rows <= 0)
		throw ErrorObject("Meta data of the database is invalid or incomplete");

	for(int i = 0; i < rows; ++i)
	{
		String name(db.getValue(i, "table_name"));
		name.sTrimSpaces();
		String sTable("%S.tbt", name);
		collection->Add(sTable);
	}
	Store(IlwisObject::obj(collection->fnObj));
}

bool PostgreSQLTables::fIsSupported(const FileName& fn, ForeignFormat::Capability dtType ) const
{
	return true;
}

void PostgreSQLTables::CreateColumns(PostGreSQL& db, TablePtr* tbl, int iNumColumns, vector<String>& vDataTypes) {
	if (  AssociatedMapType() != ForeignFormat::mtTable) {
		vDataTypes[1] = String(db.getValue(1,"data_type"));
		//createFeatureColumns(tbl);
		//setColumnMapping(db);
		return;
	}
	for(int iCol=0; iCol < iNumColumns; ++iCol)
	{

		// get the type of the column
		String sName(db.getValue(iCol,"column_name"));
		vDataTypes[iCol] = String(db.getValue(iCol,"data_type"));	
		bool fReadOnly = false;	
		Domain dm;

		if ( AssociatedMapType() == ForeignFormat::mtTable) {
			DomainValueRangeStruct dvsColumn = dmTranslateDataTypeToIlwis(vDataTypes[iCol], fReadOnly, sName, tbl);					
			if ((!(dvsColumn.dm().fValid())) || (dvsColumn.dm()->pdnone() == 0) )
			{
				if ( psgrToIlwMapping[sName].fValid())
					continue;
				psgrToIlwMapping[sName] = tbl->colNew(sName, dvsColumn);
				if (psgrToIlwMapping[sName].fValid())
				{
					sarForeignCols.insert(sName);
					psgrToIlwMapping[sName]->SetReadOnly(fReadOnly);
					psgrToIlwMapping[sName]->SetOwnedByTable(false);
				}
			}
		}

	}
}

void PostgreSQLTables::FillRecords(PostGreSQL& db, TablePtr* tbl, int iNumRecords, int iNumColumns, const vector<String>& vDataTypes) {
	tbl->dm()->pdUniqueID()->Resize(iNumRecords);
	for(int iCol = 0; iCol < iNumColumns; ++iCol) //  put data in the column
	{
		String colName = db.getFieldName(iCol);
		Column col = psgrToIlwMapping[colName];
		if ( col.fValid() )
		{
			col->SetLoadingForeignData(true);				
			for( long iRec = 0; iRec < iNumRecords; ++iRec) // put data in the field
			{
				String type = vDataTypes[iCol];
				char *v = db.getValue(iRec, iCol);
				String val(v);
				val = val.sTrimSpaces();
				
				PutData(col, iRec + 1, val);
			}
			col->SetLoadingForeignData(false);
		}
	}
}

// this loads the table with the data. It creates the appropriate columns and then fills them
void PostgreSQLTables::LoadTable(TablePtr *tbl)
{
	PostGreSQL db(sConnectionString.c_str());

	String schemaQuery("SELECT column_name, data_type FROM INFORMATION_SCHEMA.Columns WHERE table_schema = 'public' and table_name='%S' ORDER BY ordinal_position", tableName);
	db.getNTResult(schemaQuery.c_str());

	
	
	int iNumColumns = db.getNumberOf(PostGreSQL::ROW); // this was a schema query, every row of the result describes a column of table 'tableName'
	vector<String> vDataTypes;
	//columns[mtLoadType].resize(iNumColumns);
	vDataTypes.resize(iNumColumns);
	CreateColumns(db, tbl, iNumColumns, vDataTypes);

	db.getNTResult(sQuery.c_str());
	int iNumRecords = db.getNumberOf(PostGreSQL::ROW);
	if ( tbl->iRecs()!= iNumRecords) 
	{
		if ( iNumRecords > tbl->iRecs())
			tbl->iRecNew(iNumRecords - tbl->iRecs());
		else
			tbl->DeleteRec(iNumRecords, tbl->iRecs() - iNumRecords);
	}				
	
	FillRecords(db, tbl, iNumRecords, iNumColumns, vDataTypes);
}

void PostgreSQLTables::PutData(Column& col, long iRec, const String& data)
{
DomainType dmt = col->dm()->dmt();
switch(dmt)
	{
	case dmtCLASS:
	case dmtID:			
	case dmtSTRING:
	case dmtUNIQUEID:
		{
		col->PutVal(iRec, data);
		}				
		break;
	case dmtIMAGE:			
	case dmtVALUE:
	case dmtBOOL:
		{
		double rV = data.rVal();
		col->PutVal(iRec, rV);				
		}
		break;
	default:
		break;
	}				
}

DomainValueRangeStruct PostgreSQLTables::dmTranslateDataTypeToIlwis(String dte, bool &fReadOnly, const String& sColName, TablePtr *tbl)
{
	FileName fnDom;
	String sColSection("Col:%S", sColName);
	ObjectInfo::ReadElement(sColSection.c_str(), "Domain", tbl->fnObj, fnDom);
	if ( fnDom.fExist() && !ObjectInfo::fSystemObject(fnDom))
	{
		Domain dom(fnDom);
		if ( dom->pdsrt() )
			return DomainValueRangeStruct(dom);
	}
	if ( dte == "character varying" || dte == "character" || dte == "text" || dte == "varchar" || dte == "time" || dte == "date" || dte == "oid" || dte == "char" || dte == "timestamp")
		return DomainValueRangeStruct(Domain("String"));
	else if ( dte == "byte")
		 return DomainValueRangeStruct(0, 255);
	else if ( dte == "int2" || dte == "smallint")
		 return DomainValueRangeStruct(-16383, 16383);
	else if ( dte == "int4" || dte == "integer" )
		 return DomainValueRangeStruct( -LONG_MAX + 2 , LONG_MAX - 2);
	else if ( dte == "int8" || dte == "numeric" || dte == "float" || dte == "double precision" || dte == "real")
		 return DomainValueRangeStruct(-1e307, 1e307, 0.0);
	else if ( dte == "boolean")
		return DomainValueRangeStruct(Domain("bool"));
	return Domain("none")	;
}

void PostgreSQLTables::PutStringField(const String& sColumn, long iRecord, const String& sValue)
{
	Column col;
	String sWhereClause;
	Table tbl(fnTable);
	for(int i = 0; i < tbl->iCols(); ++i) {
		Column col1 = tbl->col(i);

		if ( !col1.fValid()) continue;

		if ( col1->sName() == sColumn) {
			col = col1;
		} else {
			String v = col1->sValue(iRecord, 0);
			DomainType dmt = col1->dm()->dmt();
			if( dmt == dmtSTRING || dmt == dmtID || dmt == dmtCLASS)
				sWhereClause += sWhereClause == "" ? String("%S='%S'",col1->sName(), v) : String(" AND %S='%S'",col1->sName(), v);
			else
				sWhereClause += sWhereClause == "" ? String("%S=%S",col1->sName(), v) : String(" AND %S=%S",col1->sName(), v);
		}
  	}
	if ( col.fValid() == false)
		return;
	DomainType dmt = tbl->dm()->dmt();
	String sV = sValue;
	if( dmt != dmtNONE && dmKey != NULL)
		sWhereClause += sWhereClause == ""  ? String("%S='%S'",sKeyColumn, (*dmKey)->sValueByRaw(iRecord,0)) : String(" AND %S='%S'",sKeyColumn, (*dmKey)->sValueByRaw(iRecord,0)) ;
	dmt = col->dm()->dmt();
	String setValue = sValue;
	if (dmt == dmtSTRING || dmt == dmtID || dmt == dmtCLASS)
		setValue = String("'%S'", setValue);	
	String query("Update %S Set %S = %S Where %S", tableName, sColumn, setValue, sWhereClause);
	PostGreSQL db(sConnectionString.c_str());
	db.getNTResult(query.c_str());
}

bool PostgreSQLTables::fIsCollection(const String& sForeignObject) const 
{ 
	URL url(sForeignObject);
	String sPath = url.getPath();
	Array<String> parts;
	Split(sPath,parts,"/");
	return parts.size() == 2;
}

// function will test if a type passed from a doctemplate matches the object type of the filename
bool PostgreSQLTables::fMatchType(const String& sFileName, const String& sType)
{
	// Templates object type name
	if ( fIsCollection(sFileName))
		return sType == "ILWIS DataBase Collections";
	else
		return sType == "ILWIS Tables";
}

FileName PostgreSQLTables::createCollectionName(const String& name, ParmList& pm) {
	String dbName;
	if ( pm.fExist("output")) {
		dbName = pm.sGet("output");
	} else {
		URL url(name);
		String path = url.getPath();
		Array<String> parts	;
		Split(path,parts,"/");
		dbName = parts[parts.size() - 1];
	}
	FileName fnIOC(FileName::fnUnique(FileName(dbName,".ioc")));
	return fnIOC;;
}

void PostgreSQLTables::getImportFormats(vector<ImportFormat>& formats) {
	ImportFormat frm;
	frm.type = ImportFormat::ifTable;
	frm.name = frm.shortName = "Database";
	frm.method = "POSTGRESQL";
	frm.provider = frm.method;
	frm.useasSuported = true;
	frm.ui = NULL;
	formats.push_back(frm);

}