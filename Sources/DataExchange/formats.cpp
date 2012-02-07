#include "DataExchange\gdalincludes\gdal.h"
#include "DataExchange\gdalincludes\gdal_frmts.h"
#include "DataExchange\gdalincludes\cpl_vsi.h"
#include "DataExchange\gdalincludes\ogr_api.h"
#include "DataExchange\gdalincludes\ogr_srs_api.h"

#include "Engine\Table\tbl.h"
#include "Engine\Base\System\Engine.h"
#include "Engine\Applications\ModuleMap.h"
#include "Engine\Base\System\commandhandler.h"
#include "Engine\SpatialReference\prj.h"
#include "Engine\Base\DataObjects\URL.h"
#include "Engine\Map\Feature.h"
#include "Engine\DataExchange\hdfincludes\hdf.h"
#include "Engine\DataExchange\hdfincludes\vg.h"
#include "Engine\DataExchange\hdfincludes\mfhdf.h"
#include "Engine\DataExchange\HDFReader.h"
#include "Engine\DataExchange\ForeignFormat.h"
#include "Engine\DataExchange\OLEDBProvider.h"
#include "DataExchange\ADOTable.h"
#include "DataExchange\WMSFormat.h"
#include "DataExchange\ASTERFormat.h"
#include "DataExchange\GDAL.h"
#include "DataExchange\PostgreSql.h"
#include "DataExchange\PostgreSQLTables.h"
#include "DataExchange\PostGisMaps.h"
#include "Engine\DataExchange\ForeignFormatMap.h"


extern "C" _export void getForeignFormatInfo(map<String, ForeignFormatFuncs> *funcs) {
	(*funcs)["ADO"] = ForeignFormatFuncs(CreateImportObjectADO, CreateQueryObjectADO);
	(*funcs)["ASTER"] = ForeignFormatFuncs(CreateImportObjectAster, CreateQueryObjectAster);
	(*funcs)["WMS"] = ForeignFormatFuncs(CreateImportObjectWMS, CreateQueryObjectWMS);
	(*funcs)["GDAL"] = ForeignFormatFuncs(CreateImportObjectGDAL, CreateQueryObjectGDAL);
	//(*funcs)["POSTGRESQL"] = ForeignFormatFuncs(CreateImportObjectPostGres, CreateQueryObjectPostGres);

	getEngine()->getContext()->ComHandler()->AddCommand("gdalogrimport",ogrgdal);
	getEngine()->getContext()->ComHandler()->AddCommand("gdalrasterimport",rastergdal);
}

extern "C" _export void getImportDriverList(vector<ImportDriver>& drivers) {
	ImportDriver gdalDriver;
	gdalDriver.driverName="Geospatial Data Abstraction Library(GDAL)";
	GDALFormat gdal;
	gdal.getImportFormats(gdalDriver.formats);
	drivers.push_back(gdalDriver);
	//ImportDriver postgresDriver;
	//postgresDriver.driverName= "Postgres";
	//PostgreSQLTables postgres;
	//postgres.getImportFormats(postgresDriver.formats);
	///*PostGisMaps postgis;
	//postgis.getImportFormats(postgresDriver.formats);*/
	//drivers.push_back(postgresDriver);
	ImportDriver asterDriver;
	asterDriver.driverName= "Aster";
	ASTERFormat aster;
	aster.getImportFormats(asterDriver.formats);
	drivers.push_back(asterDriver);
	ImportDriver adoDriver;
	adoDriver.driverName= "ActiveX Data Objects(ADO)";
	ADOTable ado;
	ado.getImportFormats(adoDriver.formats);
	drivers.push_back(adoDriver);

}