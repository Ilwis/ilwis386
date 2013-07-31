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
#include "DataExchange\PostGisMaps.h"
#include "Engine\DataExchange\TableForeign.h"
#include "Engine\Domain\dmcoord.h"
#include "Engine\SpatialReference\Coordsys.h"
#include "Engine\SpatialReference\Csproj.h"
#include "Engine\SpatialReference\Cslatlon.h"
#include "Engine\Map\basemap.h"
#include "Engine\Base\DataObjects\URL.h"
#include "Headers\Hs\IMPEXP.hs"
//#include "temp\msado15.tlh"


String PostGisMaps::sFormatPostGis(const FileName& fnForeign) {
	if ( fnForeign.sFile == "POSTGIS")
		return "POSTGIS";
	return "";
}
ForeignFormat *CreateQueryObjectPostGis() //create query object
{
	return new PostGisMaps();
}

ForeignFormat *CreateImportObjectPostGis(const FileName& fnObj, ParmList& pm) //create import object
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
	Domain dmAttrTable (fnObj, 0, dmtUNIQUEID, "id");
	return new PostGisMaps(fnObj, dmAttrTable, pm);
}

PostGisMaps::PostGisMaps() :
	PostgreSQLTables()
{

}
PostGisMaps::PostGisMaps(const FileName& fn, const Domain & dmAttrTable, ParmList& pm) :
	PostgreSQLTables(fn, dmAttrTable, pm)
{
	IlwisObject::iotIlwisObjectType type = IlwisObject::iotObjectType(fn);
	if ( type == IlwisObject::iotPOINTMAP ||  type == IlwisObject::iotSEGMENTMAP ||  type == IlwisObject::iotPOLYGONMAP) {
		geometryColumn = fn.sFile.sTail("_");
		tableName = fn.sFile.sHead("_");
		sQuery = "Select * From " + tableName;
		PostGreSQL db(sConnectionString.c_str());
   	    String query("Select srid from geometry_columns where f_geometry_column='%S'", geometryColumn);
		db.getNTResult(query.c_str());
		ForeignCollection fc(pm.sGet("collection"));
		if(db.getNumberOf(PostGreSQL::ROW) > 0) {
			String srid(db.getValue(0,"srid"));
			csy = getCoordSystem( FileName(geometryColumn), String("EPSG:%S",srid));
			if ( fc.fValid()) {
				fc->Add(csy);
			}
		}
		ObjectInfo::WriteElement("ForeignFormat","Table",fn,tableName);
		ObjectInfo::WriteElement("ForeignFormat","Query",fn,sQuery);
	}
	switch(type) {
		case IlwisObject::iotPOINTMAP:
			mtLoadType = mtPointMap; break;
		case IlwisObject::iotSEGMENTMAP:
			mtLoadType = mtSegmentMap; break;
		case IlwisObject::iotPOLYGONMAP:
			mtLoadType = mtPolygonMap; break;
		default:
			mtLoadType = mtUnknown;
	}
}

void PostGisMaps::ReadParameters(const FileName& fnObj, ParmList& pm) 
{
	PostgreSQLTables::ReadParameters(fnObj, pm);
	ObjectInfo::ReadElement("ForeignFormat","GeometryColumn",fnObj,geometryColumn);
	pm.Add(new Parm("geometry", geometryColumn));
}

void PostGisMaps::Store(IlwisObject obj) {
	PostgreSQLTables::Store(obj);
	obj->WriteElement("ForeignFormat","Method","POSTGIS");
	obj->WriteElement("ForeignFormat","GeometryColumn",geometryColumn);
}

PostGisMaps::~PostGisMaps()
{
	delete dmKey;
	dmKey = NULL;
}

// fills a database collection with appropriate tables
void PostGisMaps::PutDataInCollection(ForeignCollectionPtr* collection, ParmList& pm)
{
	PostGreSQL db(sConnectionString.c_str());
	db.getNTResult("SELECT table_name,column_name FROM INFORMATION_SCHEMA.Columns WHERE table_schema = 'public' and udt_name='geometry'");
	int rows = db.getNumberOf(PostGreSQL::ROW);
	if ( rows <= 0)
		throw ErrorObject("Meta data of the database is invalid or incomplete");

	for(int i = 0; i < rows; ++i)
	{
		String tname(db.getValue(i, "table_name"));
		String cname(db.getValue(i, "column_name"));
		PostGreSQL db2(sConnectionString.c_str());
		String query = String("Select distinct(ST_GeometryType(%S)) from %S",cname, tname);
		db2.getNTResult(query.c_str());
		if (db2.getNumberOf(PostGreSQL::ROW) > 0) {
			for(int j = 0; j < db2.getNumberOf(PostGreSQL::ROW); ++j) {
				String type = String(db2.getValue(j,0)).toLower();
				String name = tname + "_" + cname;
				name.sTrimSpaces();
				String sMap;
				if (type == "st_point" || type == "st_multipoint") 
					sMap = String("%S.mpp", name);
				else if (type == "st_linestring" || type == "st_linestring")
					sMap = String("%S.mps", name);
				//else if (type == "st_polygon" || type == "st_multipolygon")
				//	sMap = String("%S.mpa", name);
				if ( sMap != "")
					collection->Add(sMap);
			}
		}
	}
	Store(IlwisObject::obj(collection->fnObj));
}

bool PostGisMaps::fIsSupported(const FileName& fn, ForeignFormat::Capability dtType ) const
{
	return true;
}

void PostGisMaps::createFeatureColumns(TablePtr* tbl) {
		Domain dmcrd;
		dmcrd.SetPointer(new DomainCoord(csy->fnObj));
		if ( AssociatedMapType() == ForeignFormat::mtPointMap) {
			Column col = tbl->colNew("Name", *dmKey);
			col->SetOwnedByTable();
			col->SetLoadingForeignData(true);
			col = tbl->colNew("Coordinate", dmcrd, ValueRange());
			col->SetOwnedByTable();
			col->SetLoadingForeignData(true);
		}

		if ( AssociatedMapType() == ForeignFormat::mtSegmentMap) {
			Domain dmcrdbuf("CoordBuf");
			Column col = tbl->colNew("SegmentValue", *dmKey);
			col->SetOwnedByTable();
			col->SetLoadingForeignData(true);
			col = tbl->colNew("Coords", dmcrdbuf);
			col->SetOwnedByTable();
			col->SetLoadingForeignData(true);
			col = tbl->colNew("MinCoords", dmcrd, ValueRange());
			col->SetOwnedByTable();
			col->SetLoadingForeignData(true);
			col = tbl->colNew("MaxCoords", dmcrd, ValueRange());
			col->SetOwnedByTable();
			col->SetLoadingForeignData(true);
			col = tbl->colNew("Deleted", Domain("bool"));
			col->SetOwnedByTable();
			col->SetLoadingForeignData(true);
		}
}

void PostGisMaps::CreateColumns(PostGreSQL& db, TablePtr* tbl, int iNumColumns, int &iKeyColumn, vector<String>& vDataTypes) {
	if (  AssociatedMapType() != ForeignFormat::mtTable) {
		createFeatureColumns(tbl);
	}
}

void PostGisMaps::FillRecords(PostGreSQL& db, TablePtr* tbl, int iNumRecords, const vector<String>& vDataTypes) {
	for(int iRec = 0; iRec < iNumRecords; ++iRec) {
		CoordBounds crb;
		String v(db.getValue(iRec, 0));
		if ( mtLoadType == mtPointMap) {
			Column col = tbl->col("Coordinate");
			PutData(col, iRec + 1, v, &crb);
			col = tbl->col("Name");
			PutData(col, iRec + 1, (*dmKey)->pdsrt()->sValue(iRec + 1));
		}
		if ( mtLoadType == mtSegmentMap) {
			Column col = tbl->col("Coords");
			PutData(col,iRec + 1, v, &crb);
			col = tbl->col("SegmentValue");
			PutData(col,iRec + 1, (*dmKey)->pdsrt()->sValue(iRec + 1));
			tbl->col("MinCoords")->PutVal(iRec + 1, crb.cMin);
			tbl->col("MaxCoords")->PutVal(iRec + 1, crb.cMax);
			tbl->col("Deleted")->PutVal(iRec + 1, (long)false);
			
		}
	}
}

// this loads the table with the data. It creates the appropriate columns and then fills them
void PostGisMaps::LoadTable(TablePtr *tbl)
{
	PostGreSQL db(sConnectionString.c_str());

	String type = mtLoadType == mtPointMap ? "point" : "linestring";
	String geometryQuery("Select st_astext(%S) from %S where lower(ST_GeometryType(%S)) like '%%%S'",geometryColumn,tableName,geometryColumn,type);
	db.getNTResult(geometryQuery.c_str());

	
	
	int iNumColumns = db.getNumberOf(PostGreSQL::COLUMN);
	vector<String> vDataTypes;
	//columns[mtLoadType].resize(iNumColumns);
	vDataTypes.resize(iNumColumns);
	int iKeyColumn=iUNDEF;
	if ( dmKey == NULL)
		dmKey = new Domain(fnTable);
	CreateColumns(db, tbl, iNumColumns, iKeyColumn, vDataTypes);

//	db.getNTResult(sQuery.c_str());
	int iNumRecords = db.getNumberOf(PostGreSQL::ROW);
	if ( tbl->iRecs() == 0) // true for new tables
	{
		tbl->iRecNew(iNumRecords);
	}				
	
	FillRecords(db, tbl, iNumRecords, vDataTypes);
}

void PostGisMaps::PutData(Column& col, long iRec, const String& data, CoordBounds *crdBuf)
{
	DomainType dmt = col->dm()->dmt();
	switch(dmt)
	{
	case dmtCOORD:
		{
			String res = data.sSub(6, data.size() - 7);
			double x = res.sHead(" ").rVal();
			double y = res.sTail(" ").rVal();
			Coord crd(x,y);
			if ( iRec >= 1)
				col->PutVal(iRec, crd);
			(*crdBuf) += crd;
			break;
		}
	case dmtCOORDBUF:
		{
			String res = data.sSub(11, data.size() - 12);
			Array<String> parts;
			Split(res, parts,",");
			vector<Coordinate> *coords = new vector<Coordinate>();
			for(int	i = 0; i < parts.size(); ++i) {
				double x = parts[i].sHead(" ").rVal();
				double y = parts[i].sTail(" ").rVal();
				Coord crd(x,y);
				coords->push_back(crd);
				(*crdBuf) += crd;
			}
			if ( iRec >= 1)
				col->PutVal(iRec, new CoordinateArraySequence(coords), coords->size());
			else
				delete coords;
		}
	default:
		{
			if ( iRec >= 1)
				PostgreSQLTables::PutData(col, iRec, data);
		}
		break;
	}				
}

void PostGisMaps::PutCoordField(const String& sColumn, long iRecord, Coord cValue){
	double x = cValue.x;
	double y = cValue.y;

}

// function will test if a type passed from a doctemplate matches the object type of the filename
bool PostGisMaps::fMatchType(const String& sFileName, const String& sType)
{
	// Templates object type name
	if ( fIsCollection(sFileName))
		return sType == "ILWIS ForeignCollections";
	else
		return sType == "ILWIS Tables";
}



LayerInfo PostGisMaps::GetLayerInfo(ParmList& parms) {
	LayerInfo info;
	FileName fn(fnTable);
	PostGreSQL db(sConnectionString.c_str());

	String type = mtLoadType == mtPointMap ? "point" : "linestring";
	String query = parms.sGet("query").sUnQuote().c_str();
	String geometryQuery("Select *,st_astext(%S) as _coords_ from %S where lower(ST_GeometryType(%S)) like'%%%S'",geometryColumn,tableName,geometryColumn,type);
	if (query != "")
		query = query + "in (" + geometryQuery + ")";
	else
		query = geometryQuery;
	parms.Add(new Parm("query",query));
		
	db.getNTResult(query.c_str());
	info.iShapes = db.getNumberOf(PostGreSQL::ROW);

	String sKey = parms.sGet("key");
	if ( sKey == "")
		dmKey = new Domain(fn,info.iShapes,dmtUNIQUEID,type);
	info.dvrsMap = DomainValueRangeStruct(*dmKey);
	FileName fnAttr(fnTable);
	fnAttr.sExt = ".tbt";
	ParmList pmAttribTable;
	Parm *p2;
	for(int i = 0; i < parms.iSize(); ++i) {
		Parm *p = parms.parms[i];
		if ( p->sOpt() == "method")
		    p2 = new Parm("method","POSTGRES");		
		else if ( p->sOpt() == "table")
			p2 = new Parm("table",tableName);
		else if ( p->sOpt() == "query")
			p2 = new Parm("query", String("Select * FROM %S", tableName));
		else
			p2 = new Parm(p->sOpt(), p->sVal());

		pmAttribTable.Add(p2);
	}
	pmAttribTable.Add(new Parm("key",fnTable.sFullPath()));
	PostgreSQLTables pgt(fnAttr, *dmKey, pmAttribTable);

	info.tbl = TableForeign::CreateDataBaseTable(fn, parms);
	info.tbl->iRecNew(info.iShapes);
	info.tblattr = Table(fnAttr);

	createFeatureColumns(info.tbl.ptr());
	CoordBounds crdbuf;
	for(int i = 0; i < info.iShapes; ++i) {
		int iRaw = i ; //(*dmKey)->iRaw(id);
		String res(db.getValue(i, "_coords_"));
		if ( mtLoadType == mtPointMap) {
			Column col = info.tbl->col("Coordinate");
			PutData(col, -1, res, &crdbuf);
		}
		if ( mtLoadType == mtSegmentMap) {
			Column col = info.tbl->col("Coords");
			PutData(col, -1, res, &crdbuf);	
		}
			
		
	}

	info.cbActual = info.cbMap = crdbuf;
	info.fnObj = fn;
	info.csy = csy;
	return info;
}

// main routine for reading a vector layer. It construct all data objects (tables. csy)
void PostGisMaps::IterateLayer(vector<LayerInfo>& objects, bool fCreate)
{
	PostGreSQL db(sConnectionString.c_str());
	db.getNTResult(sQuery.c_str());

	objects[mtLoadType].tbl.SetPointer(new TablePtr(objects[mtLoadType].fnObj, "",false));
	objects[mtLoadType].tbl->Load();
	objects[mtLoadType].tbl->Loaded(true);

}

typedef OGRErr (__stdcall *OSRImportFromEPSGFunc)( OGRSpatialReferenceH, int );
typedef void (*CPLPushFinderLocationFunc)( const char * );
typedef OGRSpatialReferenceH (__stdcall *OSRNewSpatialReferenceFunc)( const char *);
typedef const char* (__stdcall  *OSRGetAttrValueFunc)( OGRSpatialReferenceH hSRS, const char * pszName, int iChild /* = 0 */ );
typedef int (*OSRIsProjectedFunc)( OGRSpatialReferenceH );

CoordSystem PostGisMaps::getCoordSystem(const FileName& fnBase, const String& srsName) {
	CFileFind finder;
	String path = getEngine()->getContext()->sIlwDir() + "\\gdal*.dll";
	BOOL found = finder.FindFile(path.c_str());
	if ( !found) 
		return CoordSystem();
	finder.FindNextFile();
	FileName fnModule (finder.GetFilePath());
	HMODULE hm = LoadLibrary(fnModule.sFullPath().c_str());
	CPLPushFinderLocationFunc findGdal = (CPLPushFinderLocationFunc)GetProcAddress(hm, "CPLPushFinderLocation");
	OSRNewSpatialReferenceFunc newsrs = (OSRNewSpatialReferenceFunc)GetProcAddress(hm, "_OSRNewSpatialReference@4");
	OSRImportFromEPSGFunc importepsg = (OSRImportFromEPSGFunc)GetProcAddress(hm,"_OSRImportFromEPSG@8");
	OSRGetAttrValueFunc getAttr = (OSRGetAttrValueFunc)GetProcAddress(hm,"_OSRGetAttrValue@12");
	OSRIsProjectedFunc isProjected = (OSRIsProjectedFunc)GetProcAddress(hm,"OSRIsProjected");

	path = getEngine()->getContext()->sIlwDir();
	path += "Resources\\gdal_data";
	findGdal(path.c_str());
	OGRSpatialReferenceH handle = newsrs(NULL);
	//char epsg[5000];
	//strcpy(epsg,srsName.sTail(":").sVal()); 
	//char *pepsg = (char *)epsg;
	int epsg = srsName.sTail(":").iVal();
	OGRErr err = importepsg( handle, epsg);
	if ( err != OGRERR_NONE )
		throw ErrorObject(String("The SRS %S could not be used", srsName));

	String datumName(getAttr(handle, "Datum",0));
	//map<String, ProjectionConversionFunctions>::iterator where = mpCsyConvers.find(projectionName);


	FileName fnCsy(fnBase, ".csy");
	if ( _access(fnCsy.sRelative().c_str(),0) == 0)
		return CoordSystem(fnCsy);

	CoordSystemViaLatLon *csv=NULL;
	if ( isProjected(handle)) {
		CoordSystemProjection *csp =  new CoordSystemProjection(fnCsy, 1);
		String dn = Datum::WKTToILWISName(datumName);
		if ( dn == "")
			throw ErrorObject("Datum can't be transformed to an ILWIS known datum");
		csp->datum = new MolodenskyDatum(dn,"");
		csv = csp;
	} else {
		csv = new CoordSystemLatLon(fnCsy, 1);
		csv->datum = new MolodenskyDatum("WGS 1984","");
	}

	CoordSystem csy;
	csy.SetPointer(csv);

	return csy;
}

void PostGisMaps::getImportFormats(vector<ImportFormat>& formats) {
	ImportFormat frm;
	frm.type = ImportFormat::ifPoint | ImportFormat::ifSegment;
	frm.name = frm.shortName = "Database";
	frm.method = "POSTGIS";
	frm.provider = frm.method;
	frm.useasSuported = true;
	frm.ui = NULL;
	formats.push_back(frm);

}