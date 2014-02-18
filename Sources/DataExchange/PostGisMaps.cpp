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
#include "Engine\SpatialReference\Grcornrs.h"
#include "Engine\Map\basemap.h"
#include "Engine\Base\DataObjects\URL.h"
#include "Engine\Table\Colbinar.h"
#include "Engine\DataExchange\GdalProxy.h"
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
	return new PostGisMaps(fnObj, fnObj, pm);
}

PostGisMaps::PostGisMaps() :
	PostgreSQLTables()
	, rasterTiles(0)
{

}

PostGisMaps::PostGisMaps(const FileName& fn, const FileName & fnDomAttrTable, ParmList& pm) :
	PostgreSQLTables(fn, fnDomAttrTable, pm)
	, rasterTiles(0)
	, x_pixels(1)
	, y_pixels(1)
{
	IlwisObject::iotIlwisObjectType type = IlwisObject::iotObjectType(fn);
	String tablecol;
	String id;
	split(fn.sFile, tablecol, id, "@");
	if (id != "") {
		split(tablecol, tableName, geometryColumn, "_");
		sQuery = String("Select * From %S.%S Where rid='%S'", schema, tableName, id);
	} else {
		split(fn.sFile, tableName, geometryColumn, "_");
		sQuery = String("Select * From %S.%S", schema, tableName);
	}
	ObjectInfo::WriteElement("ForeignFormat","Table",fn,tableName);
	ObjectInfo::WriteElement("ForeignFormat","Query",fn,sQuery);
	if ( type == IlwisObject::iotPOINTMAP ||  type == IlwisObject::iotSEGMENTMAP ||  type == IlwisObject::iotPOLYGONMAP) {
		PostGreSQL db(sConnectionString.c_str());
   	    String query("Select srid from geometry_columns where f_table_schema='%S' and f_table_name='%S' and f_geometry_column='%S'", schema, tableName, geometryColumn);
		db.getNTResult(query.c_str());
		ForeignCollection fc(pm.fExist("parentcollection") ? pm.sGet("parentcollection") : pm.sGet("collection"));
		if(db.getNumberOf(PostGreSQL::ROW) > 0) {
			FileName fnCsy(FileName(geometryColumn), ".csy");
			if ( _access(fnCsy.sRelative().c_str(),0) == 0)
				csy = CoordSystem(fnCsy);
			else {
				String srid(db.getValue(0,"srid"));
				try {
					csy = getEngine()->gdal->getCoordSystem(FileName(geometryColumn), srid.iVal());
				} catch (ErrorObject&) {
					csy = CoordSystem("unknown");
				}
			}
			if ( fc.fValid()) {
				fc->Add(csy);
			}
		}
	} else if ( type == IlwisObject::iotRASMAP) {
		LayerInfo inf;

		if (fn.fExist()) {
			String sType;
			ObjectInfo::ReadElement("BaseMap", "Type", fn, sType);
			if (fCIStrEqual(sType, "Map")) {
				mtLoadType = mtRasterMap;
				String srid;
				int x_pixels_tile;
				int y_pixels_tile;
				double nodata_value;
				String pixel_type;
				StoreType stPostgres;
				ObjectInfo::ReadElement("Map", "GeoRef", fn, inf.grf);
				ObjectInfo::ReadElement("BaseMap", "CoordSystem", fn, csy);
				ObjectInfo::ReadElement("ForeignFormat", "srid", fn, srid);
				ObjectInfo::ReadElement("ForeignFormat", "x_pixels_tile", fn, x_pixels_tile);
				ObjectInfo::ReadElement("ForeignFormat", "y_pixels_tile", fn, y_pixels_tile);
				ObjectInfo::ReadElement("ForeignFormat", "x_pixels", fn, x_pixels);
				ObjectInfo::ReadElement("ForeignFormat", "y_pixels", fn, y_pixels);
				ObjectInfo::ReadElement("ForeignFormat", "nodata_value", fn, nodata_value);
				ObjectInfo::ReadElement("ForeignFormat", "pixel_type", fn, pixel_type);
				SetStoreType(pixel_type, inf, stPostgres);
				rasterTiles = new PostGisRasterTileset(sConnectionString, schema, tableName, geometryColumn, id, inf.grf, srid, x_pixels_tile, y_pixels_tile, nodata_value, stPostgres);
				dvrsMap = inf.dvrsMap;

				return;
			}
		}

		// GetRasterInfo
		PostGreSQL db(sConnectionString.c_str());
		String query;
		if (id != "")
			query = String("Select ST_SRID(%S), ST_ScaleX(%S), ST_ScaleY(%S), ST_Width(%S), ST_Height(%S), st_astext(ST_Envelope(%S)), ST_NumBands(%S), ST_BandPixelType(%S), ST_BandNoDataValue(%S) From %S.%S Where rid=%S", geometryColumn, geometryColumn, geometryColumn, geometryColumn, geometryColumn, geometryColumn, geometryColumn, geometryColumn, geometryColumn, schema, tableName, id);
		else
   			query = String("Select srid, scale_x, scale_y, blocksize_x, blocksize_y, st_astext(extent), num_bands, pixel_types, nodata_values from raster_columns where r_table_schema='%S' and r_table_name='%S' and r_raster_column='%S'", schema, tableName, geometryColumn);
		db.getNTResult(query.c_str());
		if(db.getNumberOf(PostGreSQL::ROW) > 0) {
			// CoordinateSystem
			FileName fnCsy(FileName(geometryColumn), ".csy");
			String srid(db.getValue(0, 0));
			if ( _access(fnCsy.sRelative().c_str(),0) == 0)
				csy = CoordSystem(fnCsy);
			else {
				try {
					csy = getEngine()->gdal->getCoordSystem(FileName(geometryColumn), srid.iVal());
				} catch (ErrorObject&) {
					csy = CoordSystem("unknown");
				}
			}
			inf.csy = csy;
			
			// GeoReference
			double x_pixel_size = String(db.getValue(0, 1)).rVal();
			double y_pixel_size = String(db.getValue(0, 2)).rVal();
			int x_pixels_tile = String(db.getValue(0, 3)).iVal();
			int y_pixels_tile = String(db.getValue(0, 4)).iVal();
			String extent(db.getValue(0, 5));
			if (extent.sLeft(7) == "POLYGON") {
				extent = extent.sSub(9, extent.size() - 11); // remove POLYGON(( and ))
				Array<String> coords;
				Split(extent, coords, ",");
				CoordBounds cb; // for now this is suitable only for "GeoRefCorners"
				for(int	i = 0; i < coords.size(); ++i) {
					double x = coords[i].sHead(" ").rVal();
					double y = coords[i].sTail(" ").rVal();					
					cb += Coord(x,y);
				}
				if (cb.fValid()) {
					GeoRef gr;
					FileName fnGrf(FileName::fnUnique(FileName(fn, ".grf")));
					x_pixels = round(abs(cb.width() / x_pixel_size));
					y_pixels = round(abs(cb.height() / y_pixel_size));
					gr.SetPointer(new GeoRefCorners(fnGrf, csy, RowCol(y_pixels, x_pixels), true, cb.cMin, cb.cMax));		
					inf.cbActual = inf.cbMap = cb;
					inf.grf = gr;
				}
			}

			// Domain
			int nrBands = String(db.getValue(0, 6)).iVal();
			String pixel_types(db.getValue(0, 7));
			String nodata_values(db.getValue(0, 8));
			String pixel_type;
			double nodata_value;
			int iBand = 0; // first band
			if (pixel_types.iPos('{') == 0) {
				pixel_types = pixel_types.sSub(1, pixel_types.size() - 2); // remove { and }
				nodata_values = nodata_values.sSub(1, nodata_values.size() - 2); // remove { and }
				Array<String> bands;
				Split(pixel_types, bands, ",");
				pixel_type = bands[iBand];
				bands.clear();
				Split(nodata_values, bands, ",");
				nodata_value = bands[iBand].rVal();
			} else {
				pixel_type = pixel_types;
				nodata_value = nodata_values.rVal();
			}

			StoreType stPostgres;
			SetStoreType(pixel_type, inf, stPostgres);

			rasterTiles = new PostGisRasterTileset(sConnectionString, schema, tableName, geometryColumn, id, inf.grf, srid, x_pixels_tile, y_pixels_tile, nodata_value, stPostgres);
			dvrsMap = inf.dvrsMap;

			ObjectInfo::WriteElement("ForeignFormat", "srid", fn, srid);
			ObjectInfo::WriteElement("ForeignFormat", "x_pixels_tile", fn, x_pixels_tile);
			ObjectInfo::WriteElement("ForeignFormat", "y_pixels_tile", fn, y_pixels_tile);
			ObjectInfo::WriteElement("ForeignFormat", "x_pixels", fn, x_pixels);
			ObjectInfo::WriteElement("ForeignFormat", "y_pixels", fn, y_pixels);
			ObjectInfo::WriteElement("ForeignFormat", "nodata_value", fn, nodata_value);
			ObjectInfo::WriteElement("ForeignFormat", "pixel_type", fn, pixel_type);
		}

		inf.fnObj = fn;
		
		// end GetRasterInfo

		Map map = Map(fn, inf);	
		if (!pm.fExist("import"))
			map->SetUseAs(true);
		else
			map->SetUseAs(false);

		if (inf.dvrsMap.fValues() && id == "" /* temporarily disable fetching stats from postgis as they are wrong */) {
			if (id != "")
				query = String("SELECT ST_SummaryStats(%S) From %S.%S Where rid=%S", geometryColumn, schema, tableName, id);
			else
				query = String("SELECT ST_SummaryStats('%S', '%S')", tableName, geometryColumn);
			db.getNTResult(query.c_str());
			if(db.getNumberOf(PostGreSQL::ROW) > 0) {
				String res (db.getValue(0, 0));
				res = res.sSub(1, res.size() - 2); // remove ( and )
				Array<String> parts;
				Split(res, parts, ",");
				if (inf.dvrsMap.fRealValues()) {
					double rMin = parts[4].rVal();
					double rMax = parts[5].rVal();
					map->SetMinMax(RangeReal(rMin, rMax));
				}
				else {
					long iMin = parts[4].iVal();
					long iMax = parts[5].iVal();
					map->SetMinMax(RangeInt(iMin, iMax));
				}
			}
		}

		map->Store();
		map->gr()->Store();
		Store(map);
		ForeignCollection fc(pm.fExist("parentcollection") ? pm.sGet("parentcollection") : pm.sGet("collection"));
		if (fc.fValid()) {
			fc->Add(map->gr());
			fc->Add(map->cs());
		}
	}
	switch(type) {
		case IlwisObject::iotPOINTMAP:
			mtLoadType = mtPointMap; break;
		case IlwisObject::iotSEGMENTMAP:
			mtLoadType = mtSegmentMap; break;
		case IlwisObject::iotPOLYGONMAP:
			mtLoadType = mtPolygonMap; break;
		case IlwisObject::iotRASMAP:
			mtLoadType = mtRasterMap; break;
		default:
			mtLoadType = mtUnknown;
	}
}

void PostGisMaps::SetStoreType(String pixel_type, LayerInfo & inf, StoreType & stPostgres)
{
	if (pixel_type == "1BB") {
		inf.dvrsMap = DomainValueRangeStruct(0,255);
		stPostgres = stBIT;
	} else if (pixel_type == "2BUI") {
		inf.dvrsMap = DomainValueRangeStruct(0,255);
		stPostgres = stBYTE;
	} else if (pixel_type == "4BUI") {
		inf.dvrsMap = DomainValueRangeStruct(0,255);
		stPostgres = stBYTE;
	} else if (pixel_type == "8BUI") {
		inf.dvrsMap = DomainValueRangeStruct(Domain("image"));
		stPostgres = stBYTE;
	} else if (pixel_type == "8BSI") {
		inf.dvrsMap = DomainValueRangeStruct(-128,127);
		stPostgres = stBYTE; // signed: values between 128 and 255 should be interpreted as values between -128 and -1 
	} else if (pixel_type == "16BSI") {
		inf.dvrsMap = DomainValueRangeStruct(-SHRT_MAX + 2, SHRT_MAX -2 );
		stPostgres = stINT;
	} else if (pixel_type == "16BUI") {
		inf.dvrsMap = DomainValueRangeStruct(0, 65535 - 2);
		stPostgres = stINT;
	} else if (pixel_type == "32BSI") {
		inf.dvrsMap = DomainValueRangeStruct(-LONG_MAX + 2, LONG_MAX -2 );
		stPostgres = stLONG;
	} else if (pixel_type == "32BUI") {
		inf.dvrsMap = DomainValueRangeStruct(0, LONG_MAX -2 );
		stPostgres = stLONG;
	} else if (pixel_type == "32BF") {
		inf.dvrsMap = DomainValueRangeStruct(-1e100, 1e100, 0.0); // preferrably float instead of double
		stPostgres = stFLOAT;
	} else if (pixel_type == "64BF") {
		inf.dvrsMap = DomainValueRangeStruct(-1e100, 1e100, 0.0);
		stPostgres = stREAL;
	} else {
		inf.dvrsMap = DomainValueRangeStruct(-1e100, 1e100, 0.0);
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
	if (rasterTiles) {
		delete rasterTiles;
		rasterTiles = 0;
	}
}

// fills a database collection with appropriate tables
void PostGisMaps::PutDataInCollection(ForeignCollectionPtr* collection, ParmList& pm)
{
	bool fChanged = false;
	vector<FileName> lfnCurrent;
	PostGreSQL db(sConnectionString.c_str());
	if (schema == "") {
		db.getNTResult(String("SELECT distinct table_schema FROM INFORMATION_SCHEMA.Columns").c_str());
		int rows = db.getNumberOf(PostGreSQL::ROW);
		for (int i = 0; i < rows; ++i) {
			String schemaName (db.getValue(i, 0));
			String fileName (merge(collection->fnObj.sFile, schemaName, "."));
			FileName fnOC ("'" + fileName + "'", ".ioc");
			lfnCurrent.push_back(fnOC);
			if (!collection->fObjectAlreadyInCollection(fnOC)) {
				collection->Add(fnOC);
				fChanged = true;
			}
		}
	} else if (tableName != "" && geometryColumn != "") {
		db.getNTResult(String("SELECT rid FROM %S.%S", schema, tableName).c_str());
		int rows = db.getNumberOf(PostGreSQL::ROW);
		String name = merge(tableName, geometryColumn, "_");
		for(int i = 0; i < rows; ++i) {
			String id (db.getValue(i, 0));
			String fileName = merge(name, id, "@");
			FileName fnMap ("'" + fileName + "'", ".mpr");
			lfnCurrent.push_back(fnMap);
			if (!collection->fObjectAlreadyInCollection(fnMap)) {
				collection->Add(fnMap);
				fChanged = true;
			}
		}
	} else {
		db.getNTResult(String("SELECT table_name,column_name FROM INFORMATION_SCHEMA.Columns WHERE table_schema='%S' and udt_name='geometry'", schema).c_str());
		int rows = db.getNumberOf(PostGreSQL::ROW);
		//if ( rows <= 0)
		//	throw ErrorObject("Meta data of the database is invalid or incomplete"); // not incomplete; just no 'geometry' columns found

		for(int i = 0; i < rows; ++i)
		{
			String tname(db.getValue(i, "table_name"));
			String cname(db.getValue(i, "column_name"));
			PostGreSQL db2(sConnectionString.c_str());
			String query = String("Select distinct(ST_GeometryType(%S)) from %S.%S",cname, schema, tname);
			db2.getNTResult(query.c_str());
			if (db2.getNumberOf(PostGreSQL::ROW) > 0) {
				for(int j = 0; j < db2.getNumberOf(PostGreSQL::ROW); ++j) {
					String type = String(db2.getValue(j,0)).toLower();
					String name = merge(tname, cname, "_");
					name.sTrimSpaces();
					if (name != "") {
						FileName fnMap;
						if (type == "st_point" || type == "st_multipoint") 
							fnMap = FileName("'" + name + "'", ".mpp");
						else if (type == "st_linestring" || type == "st_multilinestring")
							fnMap = FileName("'" + name + "'", ".mps");
						else if (type == "st_polygon" || type == "st_multipolygon")
							fnMap = FileName("'" + name + "'", ".mpa");
						lfnCurrent.push_back(fnMap);
						if (!collection->fObjectAlreadyInCollection(fnMap)) {
							collection->Add(fnMap);
							fChanged = true;
						}
					}
				}
			}
		}
		std::vector<std::pair<String, String>> rasters;
		db.getNTResult(String("SELECT table_name,column_name FROM INFORMATION_SCHEMA.Columns WHERE table_schema='%S' and udt_name='raster'", schema).c_str());
		rows = db.getNumberOf(PostGreSQL::ROW);
		for (int i = 0; i < rows; ++i)
		{
			String tname(db.getValue(i, "table_name"));
			String cname(db.getValue(i, "column_name"));
			rasters.push_back(pair<String, String>(tname,cname));
		}

		for (std::vector<std::pair<String, String>>::iterator raster = rasters.begin(); raster != rasters.end(); ++raster)
		{
			String & tname = raster->first;
			String & cname = raster->second;
			String name = merge(tname, cname, "_");
			name.sTrimSpaces();
			if (name != "") {
				//String query("Select same_alignment,regular_blocking from raster_columns where r_table_schema='%S' and r_table_name='%S' and r_raster_column='%S'", schema, tname, cname);
	   			String query = String("Select scale_x, scale_y, blocksize_x, blocksize_y, extent from raster_columns where r_table_schema='%S' and r_table_name='%S' and r_raster_column='%S'", schema, tname, cname);
				db.getNTResult(query.c_str());
				if(db.getNumberOf(PostGreSQL::ROW) > 0) {
					bool fOk = String(db.getValue(0, 0)) != "";
					fOk = fOk && (String(db.getValue(0, 1)) != "");
					fOk = fOk && (String(db.getValue(0, 2)) != "");
					fOk = fOk && (String(db.getValue(0, 3)) != "");
					fOk = fOk && (String(db.getValue(0, 4)) != "");
					if (fOk /* same_alignment && regular_blocking */) { // currently (postgis 2.x / AddRasterConstraints) same_alignment has a random value and regular_blocking is always false, we rely on same_alignment
						FileName fnMap ("'" + name + "'", ".mpr");
						lfnCurrent.push_back(fnMap);
						if (!collection->fObjectAlreadyInCollection(fnMap)) {
							collection->Add(fnMap);
							fChanged = true;
						}
					} else {
						FileName fnCollection ("'" + name + "'", ".ioc");
						lfnCurrent.push_back(fnCollection);
						if (!collection->fObjectAlreadyInCollection(fnCollection)) {
							collection->Add(fnCollection);
							fChanged = true;
						}
					}
				}
			}
		}
	}

	// remove filenames from collection that aren't in the database anymore
	vector<FileName> lfnRemove;
	for (int i = 0; i < collection->iNrObjects(); ++i) {
		FileName fn (collection->fnObject(i));
		IlwisObject::iotIlwisObjectType type = IlwisObject::iotObjectType(fn);
		if (type != IlwisObject::iotRASMAP && type != IlwisObject::iotPOLYGONMAP && type != IlwisObject::iotSEGMENTMAP && type != IlwisObject::iotPOINTMAP && type != IlwisObject::iotOBJECTCOLLECTION)
			continue; // it is not an item that was added to the collection by this function, so we can't remove it
		if (find(lfnCurrent.begin(), lfnCurrent.end(), fn) == lfnCurrent.end())
			lfnRemove.push_back(fn);
	}
	if (lfnRemove.size() > 0) {
		for (vector<FileName>::iterator fnit = lfnRemove.begin(); fnit != lfnRemove.end(); ++fnit)
			collection->Remove(*fnit);
		fChanged = true;
	}

	if (fChanged)
		Store(IlwisObject::obj(collection->fnObj));
}

bool PostGisMaps::fIsSupported(const FileName& fn, ForeignFormat::Capability dtType ) const
{
	return dtType & ForeignFormat::cbEDIT;
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
	} else if ( AssociatedMapType() == ForeignFormat::mtSegmentMap) {
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
	} else if ( AssociatedMapType() == ForeignFormat::mtPolygonMap) {
		Domain dmbin("Binary");
		Column col = tbl->colNew("PolygonValue", *dmKey);
		col->SetOwnedByTable();
		col->SetLoadingForeignData(true);
		col = tbl->colNew("Coords", dmbin);
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
	int iRec = 0;
	for(int i = 0; i < iNumRecords; ++i) {
		CoordBounds cb;
		String val (db.getValue(i, 0));
		bool fValid ((val.length() > 0) ? val.fVal() : false);
		if (fValid) {
			String v(db.getValue(i, 1));
			if ( mtLoadType == mtPointMap) {
				Column col = tbl->col("Coordinate");
				PutData(col, iRec + 1, v, &cb);
				col = tbl->col("Name");
				PutData(col, iRec + 1, (*dmKey)->pdsrt()->sValue(i + 1));
			} else if ( mtLoadType == mtSegmentMap) {
				Column col = tbl->col("Coords");
				PutData(col,iRec + 1, v, &cb);
				col = tbl->col("SegmentValue");
				PutData(col,iRec + 1, (*dmKey)->pdsrt()->sValue(i + 1));
				tbl->col("MinCoords")->PutVal(iRec + 1, cb.cMin);
				tbl->col("MaxCoords")->PutVal(iRec + 1, cb.cMax);
				tbl->col("Deleted")->PutVal(iRec + 1, (long)false);
			} else if ( mtLoadType == mtPolygonMap) {
				Column col = tbl->col("Coords");
				PutData(col,iRec + 1, v, &cb);
				col = tbl->col("PolygonValue");
				PutData(col,iRec + 1, (*dmKey)->pdsrt()->sValue(i + 1));
				tbl->col("MinCoords")->PutVal(iRec + 1, cb.cMin);
				tbl->col("MaxCoords")->PutVal(iRec + 1, cb.cMax);
				tbl->col("Deleted")->PutVal(iRec + 1, (long)false);
			}
			++iRec;
		}
	}
}

// this loads the table with the data. It creates the appropriate columns and then fills them
void PostGisMaps::LoadTable(TablePtr *tbl)
{
	PostGreSQL db(sConnectionString.c_str());

	String type;
	switch(mtLoadType) {
		case mtPointMap:
			type = "point";
			break;
		case mtSegmentMap:
			type = "linestring";
			break;
		case mtPolygonMap:
			type = "polygon";
			break;
		default:
			type = "point";
	}

	String geometryQuery("Select lower(ST_GeometryType(%S)) like '%%%S', st_astext(%S) from %S.%S",geometryColumn,type,geometryColumn,schema,tableName);
	db.getNTResult(geometryQuery.c_str());	
	
	int iNumColumns = db.getNumberOf(PostGreSQL::COLUMN);
	vector<String> vDataTypes;
	vDataTypes.resize(iNumColumns);
	int iKeyColumn=iUNDEF;
	if ( dmKey == NULL)
		dmKey = new Domain(fnTable);
	CreateColumns(db, tbl, iNumColumns, iKeyColumn, vDataTypes);
	int iNumRecords = db.getNumberOf(PostGreSQL::ROW);
	if ( tbl->iRecs() == 0) // true for new tables
	{
		PostGreSQL db2(sConnectionString.c_str());
		geometryQuery = String("Select count(*) from %S.%S where lower(ST_GeometryType(%S)) like'%%%S'",schema,tableName,geometryColumn,type);
		db2.getNTResult(geometryQuery.c_str());
		if (db2.getNumberOf(PostGreSQL::ROW) == 1 && db2.getNumberOf(PostGreSQL::COLUMN) == 1) {
			int iNettoRecs = String(db2.getValue(0, 0)).iVal();
			if (iNettoRecs > 0)
				tbl->iRecNew(iNettoRecs);
			else
				tbl->iRecNew(iNumRecords);
		} else
			tbl->iRecNew(iNumRecords);
	}				
	
	FillRecords(db, tbl, iNumRecords, vDataTypes);
}

void PostGisMaps::PutData(Column& col, long iRec, const String& data, CoordBounds *cb)
{
	DomainType dmt = col->dm()->dmt();
	switch(dmt)
	{
	case dmtCOORD:
		{
			String res = data;
			bool fMulti = false;
			if (res.sLeft(5) == "MULTI") {
				res = res.sSub(5, res.size() - 5); // remove MULTI
				fMulti = true;
			}
			if (res.sLeft(5) == "POINT") {
				if (fMulti)
					res = res.sSub(7, res.size() - 9); // remove POINT(( and ))
				else
					res = res.sSub(6, res.size() - 7); // remove POINT( and )
			}
			if (fMulti)
				res = res.sSub(0, res.iPos(String("),("))); // no true multipoint support yet
			double x = res.sHead(" ").rVal();
			double y = res.sTail(" ").rVal();
			Coord crd(x,y);
			if ( iRec >= 1)
				col->PutVal(iRec, crd);
			(*cb) += crd;
			break;
		}
	case dmtCOORDBUF:
		{
			String res = data;
			bool fMulti = false;
			if (res.sLeft(5) == "MULTI") {
				res = res.sSub(5, res.size() - 5); // remove MULTI
				fMulti = true;
			}
			if (res.sLeft(10) == "LINESTRING") {
				if (fMulti)
					res = res.sSub(12, res.size() - 14); // remove LINESTRING(( and ))
				else
					res = res.sSub(11, res.size() - 12); // remove LINESTRING( and )
			}
			if (fMulti)
				res = res.sSub(0, res.iPos(String("),("))); // no true multilinestring support yet
			Array<String> parts;
			Split(res, parts, ",");
			vector<Coordinate> *coords = new vector<Coordinate>();
			for(int	i = 0; i < parts.size(); ++i) {
				double x = parts[i].sHead(" ").rVal();
				double y = parts[i].sTail(" ").rVal();
				Coord crd(x,y);
				coords->push_back(crd);
				(*cb) += crd;
			}
			if ( iRec >= 1)
				col->PutVal(iRec, new CoordinateArraySequence(coords), coords->size());
			else
				delete coords;
		}
		break;
	case dmtBINARY:
		{
			String res = data;
			bool fMulti = false;
			if (res.sLeft(5) == "MULTI") {
				res = res.sSub(5, res.size() - 5); // remove MULTI
				fMulti = true;
			}
			if (res.sLeft(7) == "POLYGON") {
				if (fMulti)
					res = res.sSub(10, res.size() - 13); // remove POLYGON((( and )))
				else
					res = res.sSub(9, res.size() - 11); // remove POLYGON(( and ))
			}
			long iBufSize = 0;
			char * b = 0;
			char * p = b;
			if (fMulti) {
				Array<String> polygons;
				FFBlobUtils::SplitOnString(res, polygons, ")),((");
				FFBlobUtils::WriteBuf(&b, &p, iBufSize, polygons.size());
				for (long i = 0; i < polygons.size(); ++i) {
					Array<String> rings;
					FFBlobUtils::SplitOnString(polygons[i], rings, "),(");
					FFBlobUtils::WriteBuf(&b, &p, iBufSize, rings.size());
					for(long j = 0; j < rings.size(); ++j) {
						Array<String> coords;
						Split(rings[j], coords, ",");
						FFBlobUtils::WriteBuf(&b, &p, iBufSize, coords.size());
						FFBlobUtils::WriteBuf(&b, &p, iBufSize, coords, cb);
					}
				}
			} else {
				FFBlobUtils::WriteBuf(&b, &p, iBufSize, 1);
				Array<String> rings;
				FFBlobUtils::SplitOnString(res, rings, "),(");
				FFBlobUtils::WriteBuf(&b, &p, iBufSize, rings.size());
				for(long j = 0; j < rings.size(); ++j) {
					Array<String> coords;
					Split(rings[j], coords, ",");
					FFBlobUtils::WriteBuf(&b, &p, iBufSize, coords.size());
					FFBlobUtils::WriteBuf(&b, &p, iBufSize, coords, cb);
				}
			}
			if (iRec >= 1) {
				BinMemBlock bmb(iBufSize, (void*)b);
				col->PutVal(iRec, bmb);
			}
			delete [] b; // unfortunately BinMemBlock doesn't take over the buffer, it performs a memcpy
		}
		break;
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
		return sType == "ILWIS Postgres DataBase Collections";
	else
		return sType == "ILWIS Tables";
}

LayerInfo PostGisMaps::GetLayerInfo(ParmList& parms) {
	LayerInfo info;
	FileName fn(fnTable);
	PostGreSQL db(sConnectionString.c_str());

	String type;
	switch(mtLoadType) {
		case mtPointMap:
			type = "point";
			break;
		case mtSegmentMap:
			type = "linestring";
			break;
		case mtPolygonMap:
			type = "polygon";
			break;
		default:
			type = "point";
	}
	String query = parms.sGet("query").sUnQuote().c_str();
	String geometryQuery("Select *,st_astext(%S) as _coords_ from %S.%S where lower(ST_GeometryType(%S)) like'%%%S'",geometryColumn,schema,tableName,geometryColumn,type);
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
			p2 = new Parm("query", String("Select * FROM %S.%S", schema, tableName));
		else
			p2 = new Parm(p->sOpt(), p->sVal());

		pmAttribTable.Add(p2);
	}
	pmAttribTable.Add(new Parm("key",fnTable.sFullPath()));
	PostgreSQLTables pgt(fnAttr, dmKey->ptr()->fnObj, pmAttribTable);

	info.tbl = TableForeign::CreateDataBaseTable(fn, parms);
	info.tbl->iRecNew(info.iShapes);
	info.tblattr = Table(fnAttr);

	createFeatureColumns(info.tbl.ptr());
	CoordBounds cb;
	for(int i = 0; i < info.iShapes; ++i) { // the only purpose of this loop is to compute cb
		String res(db.getValue(i, "_coords_"));
		if ( mtLoadType == mtPointMap) {
			Column col = info.tbl->col("Coordinate");
			PutData(col, -1, res, &cb);
		} else if ( mtLoadType == mtSegmentMap) {
			Column col = info.tbl->col("Coords");
			PutData(col, -1, res, &cb);	
		} else if ( mtLoadType == mtPolygonMap) {
			Column col = info.tbl->col("Coords");
			PutData(col, -1, res, &cb);	
		}
	}

	info.cbActual = info.cbMap = cb;
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
	if ( err != OGRERR_NONE ) {
		// throw ErrorObject(String("The SRS %S could not be used", srsName));
		return CoordSystem("unknown");
	} else {

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
}

void PostGisMaps::getImportFormats(vector<ImportFormat>& formats) {
	ImportFormat frm;
	frm.type = ImportFormat::ifPoint | ImportFormat::ifSegment | ImportFormat::ifPolygon | ImportFormat::ifRaster;
	frm.name = frm.shortName = "Database";
	frm.method = "POSTGIS";
	frm.provider = frm.method;
	frm.useasSuported = true;
	frm.ui = NULL;
	formats.push_back(frm);
}

void PostGisMaps::GetLineRaw(long iLine, ByteBuf& buf, long iFrom, long iNum) const
{
}

void PostGisMaps::GetLineRaw(long iLine, IntBuf& buf, long iFrom, long iNum) const
{
}

void PostGisMaps::GetLineRaw(long iLine, LongBuf& buf, long iFrom, long iNum) const
{
}

void PostGisMaps::GetLineVal(long iLine, LongBuf& buf, long iFrom, long iNum) const
{
	rasterTiles->GetLineVal(iLine, buf, iFrom, iNum);
}

void PostGisMaps::GetLineVal(long iLine, RealBuf& buf, long iFrom, long iNum) const
{
	rasterTiles->GetLineVal(iLine, buf, iFrom, iNum);
}

long PostGisMaps::iRaw(RowCol rc) const
{
	if (dvrsMap.fRawAvailable()) {
		return dvrsMap.iRaw(iValue(rc));
	} else
		return iValue(rc);
}

long PostGisMaps::iValue(RowCol rc) const
{
	if (rc.Col >= 0 && rc.Col < x_pixels && rc.Row >= 0 && rc.Row < y_pixels) {
		LongBuf buf (x_pixels);
		rasterTiles->GetLineVal(rc.Row, buf, 0, x_pixels);
		return buf[rc.Col];
	} else
		return iUNDEF;
}

double PostGisMaps::rValue(RowCol rc) const
{
	if (rc.Col >= 0 && rc.Col < x_pixels && rc.Row >= 0 && rc.Row < y_pixels) {
		RealBuf buf (x_pixels);
		rasterTiles->GetLineVal(rc.Row, buf, 0, x_pixels);
		return buf[rc.Col];
	} else
		return rUNDEF;
}

PostGisRasterTileset::PostGisRasterTileset(String sConnectionString, String _schema, String _tableName, String _geometryColumn, String _id, const GeoRef & _gr, String _srid, int _x_pixels_tile, int _y_pixels_tile, double _nodata_value, StoreType _stPostgres)
: db(new PostGreSQL (sConnectionString.c_str()))
, schema(_schema)
, tableName(_tableName)
, geometryColumn(_geometryColumn)
, id(_id)
, gr(_gr)
, srid(_srid)
, x_pixels_tile(_x_pixels_tile)
, y_pixels_tile(_y_pixels_tile)
, iNumTiles(0)
, iTop(-1)
, iBottom(-1)
, iLeft(-1)
, iRight(-1)
, headerSize(61)
, bandHeaderSize(1)
, nodata_value(_nodata_value)
, stPostgres(_stPostgres)
{
}

PostGisRasterTileset::~PostGisRasterTileset()
{
	if (db)
		delete db;
}

void PostGisRasterTileset::GetLineVal(long iLine, LongBuf& buf, long iFrom, long iNum)
{
	if (iNumTiles == 0 || iLine < iTop || iLine > iBottom || iFrom < iLeft || iFrom + iNum - 1 > iRight)
		RenewTiles(iLine, iFrom, iNum);

	long nodata_val = round(nodata_value);

	long * ptrBuf = buf.buf();
	iLine = iLine % y_pixels_tile;
	int iStartTile = (iFrom > iLeft) ? (iFrom - iLeft) / x_pixels_tile : 0;
	int iEndTile = (iFrom + iNum - 1 < iRight) ? (iNumTiles - (iRight - iNum - iFrom + 1) / x_pixels_tile) : iNumTiles;
	unsigned int pos = 0;
	for (int i = iStartTile; i < iEndTile; ++i) {
		int jMin = ((i == iStartTile) && (iFrom > iLeft)) ? ((iFrom - iLeft) % x_pixels_tile) : 0;
		int jMax = ((i == (iEndTile - 1)) && (iFrom + iNum - 1 < iRight)) ? (x_pixels_tile - (iRight - iNum - iFrom + 1) % x_pixels_tile) : x_pixels_tile;
		char * tileHex = db->getValue(i, 0);
		tileHex += 2 * (headerSize + bandHeaderSize);
		switch(stPostgres) {
			case stBIT:
				break;
			case stBYTE:
				nodata_val = hex2dec(tileHex);
				tileHex += 2;
				for (int j = jMin; j < jMax; ++j) {
					char b = hex2dec(&tileHex[2 * iLine * x_pixels_tile + 2 * j]);
					ptrBuf[pos++] = b;
				}
				break;
			case stINT:
				tileHex += 2 * sizeof(short);
				{
					short hl;
					char * c = (char*)(&hl);
					for (int j = jMin; j < jMax; ++j) {
						c[0] = hex2dec(&tileHex[4 * iLine * x_pixels_tile + 4 * j]);
						c[1] = hex2dec(&tileHex[4 * iLine * x_pixels_tile + 4 * j + 2]);
						ptrBuf[pos++] = hl;
					}
				}
				break;
			case stLONG:
				tileHex += 2 * sizeof(long);
				{
					long hl;
					char * c = (char*)(&hl);
					for (int j = jMin; j < jMax; ++j) {
						c[0] = hex2dec(&tileHex[8 * iLine * x_pixels_tile + 8 * j]);
						c[1] = hex2dec(&tileHex[8 * iLine * x_pixels_tile + 8 * j + 2]);
						c[2] = hex2dec(&tileHex[8 * iLine * x_pixels_tile + 8 * j + 4]);
						c[3] = hex2dec(&tileHex[8 * iLine * x_pixels_tile + 8 * j + 6]);
						ptrBuf[pos++] = hl;
					}
				}
				break;
			case stFLOAT:
				tileHex += 2 * sizeof(float);
				{
					float f;
					char * c = (char*)(&f);
					for (int j = jMin; j < jMax; ++j) {
						c[0] = hex2dec(&tileHex[8 * iLine * x_pixels_tile + 8 * j]);
						c[1] = hex2dec(&tileHex[8 * iLine * x_pixels_tile + 8 * j + 2]);
						c[2] = hex2dec(&tileHex[8 * iLine * x_pixels_tile + 8 * j + 4]);
						c[3] = hex2dec(&tileHex[8 * iLine * x_pixels_tile + 8 * j + 6]);
						ptrBuf[pos++] = round(f);
					}
				}
				break;
			case stREAL:
				tileHex += 2 * sizeof(double);
				{
					double d;
					char * c = (char*)(&d);
					for (int j = jMin; j < jMax; ++j) {
						c[0] = hex2dec(&tileHex[16 * iLine * x_pixels_tile + 16 * j]);
						c[1] = hex2dec(&tileHex[16 * iLine * x_pixels_tile + 16 * j + 2]);
						c[2] = hex2dec(&tileHex[16 * iLine * x_pixels_tile + 16 * j + 4]);
						c[3] = hex2dec(&tileHex[16 * iLine * x_pixels_tile + 16 * j + 6]);
						c[4] = hex2dec(&tileHex[16 * iLine * x_pixels_tile + 16 * j + 8]);
						c[5] = hex2dec(&tileHex[16 * iLine * x_pixels_tile + 16 * j + 10]);
						c[6] = hex2dec(&tileHex[16 * iLine * x_pixels_tile + 16 * j + 12]);
						c[7] = hex2dec(&tileHex[16 * iLine * x_pixels_tile + 16 * j + 14]);
						ptrBuf[pos++] = round(d);
					}
				}

				break;
		}
	}
	for (int i = 0; i < iNum; ++i) {
		if(ptrBuf[i] == nodata_val)
			ptrBuf[i] = iUNDEF;
	}
}

void PostGisRasterTileset::GetLineVal(long iLine, RealBuf& buf, long iFrom, long iNum)
{
	if (iNumTiles == 0 || iLine < iTop || iLine > iBottom || iFrom < iLeft || iFrom + iNum - 1 > iRight)
		RenewTiles(iLine, iFrom, iNum);

	double nodata_val = nodata_value;

	double * ptrBuf = buf.buf();
	iLine = iLine % y_pixels_tile;
	int iStartTile = (iFrom > iLeft) ? (iFrom - iLeft) / x_pixels_tile : 0;
	int iEndTile = (iFrom + iNum - 1 < iRight) ? (iNumTiles - (iRight - iNum - iFrom + 1) / x_pixels_tile) : iNumTiles;
	unsigned int pos = 0;
	for (int i = iStartTile; i < iEndTile; ++i) {
		int jMin = ((i == iStartTile) && (iFrom > iLeft)) ? ((iFrom - iLeft) % x_pixels_tile) : 0;
		int jMax = ((i == (iEndTile - 1)) && (iFrom + iNum - 1 < iRight)) ? (x_pixels_tile - (iRight - iNum - iFrom + 1) % x_pixels_tile) : x_pixels_tile;
		char * tileHex = db->getValue(i, 0);
		tileHex += 2 * (headerSize + bandHeaderSize);
		switch(stPostgres) {
			case stBIT:
				break;
			case stBYTE:
				tileHex += 2;
				for (int j = jMin; j < jMax; ++j) {
					char b = hex2dec(&tileHex[2 * iLine * x_pixels_tile + 2 * j]);
					ptrBuf[pos++] = b;
				}
				break;
			case stINT:
				tileHex += 2 * sizeof(short);
				{
					short hl;
					char * c = (char*)(&hl);
					for (int j = jMin; j < jMax; ++j) {
						c[0] = hex2dec(&tileHex[4 * iLine * x_pixels_tile + 4 * j]);
						c[1] = hex2dec(&tileHex[4 * iLine * x_pixels_tile + 4 * j + 2]);
						ptrBuf[pos++] = hl;
					}
				}
				break;
			case stLONG:
				tileHex += 2 * sizeof(long);
				{
					long hl;
					char * c = (char*)(&hl);
					for (int j = jMin; j < jMax; ++j) {
						c[0] = hex2dec(&tileHex[8 * iLine * x_pixels_tile + 8 * j]);
						c[1] = hex2dec(&tileHex[8 * iLine * x_pixels_tile + 8 * j + 2]);
						c[2] = hex2dec(&tileHex[8 * iLine * x_pixels_tile + 8 * j + 4]);
						c[3] = hex2dec(&tileHex[8 * iLine * x_pixels_tile + 8 * j + 6]);
						ptrBuf[pos++] = hl;
					}
				}
				break;
			case stFLOAT:
				tileHex += 2 * sizeof(float);
				{
					float f;
					char * c = (char*)(&f);
					for (int j = jMin; j < jMax; ++j) {
						c[0] = hex2dec(&tileHex[8 * iLine * x_pixels_tile + 8 * j]);
						c[1] = hex2dec(&tileHex[8 * iLine * x_pixels_tile + 8 * j + 2]);
						c[2] = hex2dec(&tileHex[8 * iLine * x_pixels_tile + 8 * j + 4]);
						c[3] = hex2dec(&tileHex[8 * iLine * x_pixels_tile + 8 * j + 6]);
						ptrBuf[pos++] = f;
					}
				}
				break;
			case stREAL:
				tileHex += 2 * sizeof(double);
				{
					double d;
					char * c = (char*)(&d);
					for (int j = jMin; j < jMax; ++j) {
						c[0] = hex2dec(&tileHex[16 * iLine * x_pixels_tile + 16 * j]);
						c[1] = hex2dec(&tileHex[16 * iLine * x_pixels_tile + 16 * j + 2]);
						c[2] = hex2dec(&tileHex[16 * iLine * x_pixels_tile + 16 * j + 4]);
						c[3] = hex2dec(&tileHex[16 * iLine * x_pixels_tile + 16 * j + 6]);
						c[4] = hex2dec(&tileHex[16 * iLine * x_pixels_tile + 16 * j + 8]);
						c[5] = hex2dec(&tileHex[16 * iLine * x_pixels_tile + 16 * j + 10]);
						c[6] = hex2dec(&tileHex[16 * iLine * x_pixels_tile + 16 * j + 12]);
						c[7] = hex2dec(&tileHex[16 * iLine * x_pixels_tile + 16 * j + 14]);
						ptrBuf[pos++] = d;
					}
				}
				break;
		}
	}
	for (int i = 0; i < iNum; ++i) {
		if(ptrBuf[i] == nodata_val)
			ptrBuf[i] = rUNDEF;
	}
}

char PostGisRasterTileset::hex2dec(char * str)
{
	char h = str[0];
	char l = str[1];
	char h1 = (h <= '9') ? (h - '0') : (h - 'A' + 10);
	char l1 = (l <= '9') ? (l - '0') : (l - 'A' + 10);
	char hl = (h1 << 4) | l1;
	return hl;
}

void PostGisRasterTileset::RenewTiles(long iLine, long iFrom, long iNum)
{
	if (id != "") {
		String str("SELECT %S FROM %S.%S WHERE rid=%S", geometryColumn, schema, tableName, id);
		db->getNTResult(str.c_str());
		iNumTiles = db->getNumberOf(PostGreSQL::ROW);
		iLeft = iFrom - iFrom % x_pixels_tile;
		iRight = iLeft + iNumTiles * x_pixels_tile - 1;
		iTop = iLine - iLine % y_pixels_tile;
		iBottom = iTop + y_pixels_tile - 1;
	} else {
		Coord crd1;
		Coord crd2;
		gr->RowCol2Coord(iLine + 0.5, iFrom + 0.5, crd1);
		gr->RowCol2Coord(iLine + 0.5, iFrom + iNum - 1.5, crd2);
		//String str("SELECT %S FROM %S.%S WHERE ST_Intersects(%S, ST_GeomFromEWKT('SRID=%S;POLYGON((%.18f %.18f,%.18f %.18f,%.18f %.18f,%.18f %.18f,%.18f %.18f))')) order by ST_UpperLeftX(%S)", geometryColumn, schema, tableName, geometryColumn, srid, crd1.x, crd1.y, crd2.x, crd1.y, crd2.x, crd2.y, crd1.x, crd2.y, crd1.x, crd1.y, geometryColumn);
		String str("SELECT %S FROM %S.%S WHERE ST_Intersects(%S, ST_GeomFromEWKT('SRID=%S;LINESTRING(%.18f %.18f,%.18f %.18f)')) order by ST_UpperLeftX(%S)", geometryColumn, schema, tableName, geometryColumn, srid, crd1.x, crd1.y, crd2.x, crd2.y, geometryColumn);
		db->getNTResult(str.c_str());
		iNumTiles = db->getNumberOf(PostGreSQL::ROW);
		iLeft = iFrom - iFrom % x_pixels_tile;
		iRight = iLeft + iNumTiles * x_pixels_tile - 1;
		iTop = iLine - iLine % y_pixels_tile;
		iBottom = iTop + y_pixels_tile - 1;
	}
}