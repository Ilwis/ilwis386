#include "Headers\toolspch.h"
#include "Engine\SpatialReference\Coordsys.h"
#include "Engine\SpatialReference\Csproj.h"
#include "Engine\SpatialReference\Cslatlon.h"
#include "Engine\SpatialReference\Ellips.h"
#include "Engine\base\System\engine.h"
#include "gdalproxy.h"

void GdalProxy::loadMethods(const String& ilwDir) {
	CFileFind finder;
	String path = ilwDir + "\\gdal*.dll";
	BOOL fFound = finder.FindFile(path.c_str());
	while(fFound) {
		fFound = finder.FindNextFile();
		if (!finder.IsDirectory())
		{
			FileName fnModule (finder.GetFilePath());
			HMODULE hm = LoadLibrary(fnModule.sFullPath().c_str());
			if ( hm != NULL ) {
				finderLoc = (CPLPushFinderLocationFunc)GetProcAddress(hm,"CPLPushFinderLocation");

				close = (GDALCloseFunc)GetProcAddress(hm, "_GDALClose@4");
				registerAll = (GDALAllRegisterFunc)GetProcAddress(hm,"_GDALAllRegister@0");
				identifyDriver = (GDALIdentifyDriverFunc)GetProcAddress(hm,"_GDALIdentifyDriver@8");
				open = (GDALOpenFunc)GetProcAddress(hm,"_GDALOpen@8");
				xSize = (GDALGetNumbersDataSet)GetProcAddress(hm,"_GDALGetRasterXSize@4");
				ySize = (GDALGetNumbersDataSet)GetProcAddress(hm,"_GDALGetRasterYSize@4");
				count = (GDALGetNumbersDataSet)GetProcAddress(hm,"_GDALGetRasterCount@4");
				getBand = (GDALGetRasterBandFunc)GetProcAddress(hm,"_GDALGetRasterBand@8");
				create = (GDALCreateFunc)GetProcAddress(hm, "_GDALCreate@28" );
				getProjectionRef = (GDALGetProjectionRefFunc)GetProcAddress(hm, "_GDALGetProjectionRef@4" );
				getDataType = (GDALGetRasterDataTypeFunc)GetProcAddress(hm, "_GDALGetRasterDataType@4");
				newSRS = (OSRNewSpatialReferenceFunc)GetProcAddress(hm, "_OSRNewSpatialReference@4");
				fromCode = (OSRSetWellKnownGeogCSFunc)GetProcAddress(hm, "OSRSetWellKnownGeogCS");
				fromEPSG = (OSRImportFromEPSGFunc)GetProcAddress(hm,"_OSRImportFromEPSG@8");
				srsImportFromWkt = (OSRImportFromWktFunc)GetProcAddress(hm,"OSRImportFromWkt");
				isProjected = (OSRIsProjectedFunc)GetProcAddress(hm,"OSRIsProjected");
				geotransform = (GDALGetGeoTransformFunc)GetProcAddress(hm,"_GDALGetGeoTransform@8");
				rasterIO = (GDALRasterIOFunc)GetProcAddress(hm,"_GDALRasterIO@48");
				dataSize = (GDALGetDataTypeSizeFunc)GetProcAddress(hm,"_GDALGetDataTypeSize@4");
				access = (GDALGetAccessFunc)GetProcAddress(hm,"_GDALGetAccess@4");
				getAttr = (OSRGetAttrValueFunc)GetProcAddress(hm,"_OSRGetAttrValue@12");
				getDriver = (GDALGetDriverFunc)GetProcAddress(hm,"_GDALGetDriver@4");
				getDriverCount = (GDALGetDriverCountFunc)GetProcAddress(hm,"_GDALGetDriverCount@0");
				getDriverLongName = (GDALGetDriverLongNameFunc)GetProcAddress(hm,"_GDALGetDriverLongName@4");
				getDriverShortName = (GDALGetDriverShortNameFunc)GetProcAddress(hm,"_GDALGetDriverShortName@4");
				getMetaDataItem = (GDALGetMetadataItemFunc)GetProcAddress(hm,"_GDALGetMetadataItem@12");
				getAttribute = (OSRGetAttrValueFunc)GetProcAddress(hm,"_OSRGetAttrValue@12");
				wktPretty = (OSRExportToPrettyWktFunc)GetProcAddress(hm,"_OSRExportToPrettyWkt@12");
				getProjParam = (OSRGetProjParmFunc)GetProcAddress(hm,"OSRGetProjParm");

				ogrOpen = (OGROpenFunc)GetProcAddress(hm,"OGROpen");
				ogrRegAll = (OGRRegisterAllFunc)GetProcAddress(hm,"OGRRegisterAll");
				ogrGetDriverCount = (OGRGetDriverCountFunc)GetProcAddress(hm,"OGRGetDriverCount");
				ogrGetDriver = (OGRGetDriverFunc)GetProcAddress(hm,"OGRGetDriver");
				ogrGetDriverName = (OGRGetDriverNameFunc)GetProcAddress(hm,"OGR_Dr_GetName");
				ogrGetDriverByName = (OGRGetDriverByNameFunc)GetProcAddress(hm,"OGRGetDriverByName");
				ogrGetLayerByName = (GetLayerByNameFunc)GetProcAddress(hm,"OGR_DS_GetLayerByName");
				ogrGetLayerCount = (GetLayerCountFunc)GetProcAddress(hm,"OGR_DS_GetLayerCount");
				ogrGetLayer = (GetLayerFunc)GetProcAddress(hm,"OGR_DS_GetLayer");
				ogrGetLayerName = (GetLayerNameFunc)GetProcAddress(hm,"OGR_L_GetName");
				ogrGetLayerGeometryType = (GetLayerGeometryTypeFunc)GetProcAddress(hm,"OGR_L_GetGeomType");
				ogrResetReading = (ResetReadingFunc)GetProcAddress(hm,"OGR_L_ResetReading");
				ogrGetNextFeature = (GetNextFeatureFunc)GetProcAddress(hm,"OGR_L_GetNextFeature");
				ogrGetLayerDefintion = (GetLayerDefnFunc)GetProcAddress(hm,"OGR_L_GetLayerDefn");
				ogrGetFieldCount = (GetFieldCountFunc)GetProcAddress(hm,"OGR_FD_GetFieldCount");
				ogrGetFieldDefinition = (GetFieldDefnFunc)GetProcAddress(hm,"OGR_FD_GetFieldDefn");
				ogrGetFieldType = (GetFieldTypeFunc)GetProcAddress(hm,"OGR_Fld_GetType");
				ogrsVal = (GetFieldAsStringFunc)GetProcAddress(hm,"OGR_F_GetFieldAsString");
				ogrrVal = (GetFieldAsDoubleFunc)GetProcAddress(hm,"OGR_F_GetFieldAsDouble");
				ogriVal = (GetFieldAsIntegerFunc)GetProcAddress(hm,"OGR_F_GetFieldAsInteger");
				ogrGetGeometryRef = (GetGeometryRefFunc)GetProcAddress(hm,"OGR_F_GetGeometryRef");
				ogrGetGeometryType = (GetGeometryTypeFunc)GetProcAddress(hm,"OGR_G_GetGeometryType");
				ogrDestroyFeature = (DestroyFeatureFunc)GetProcAddress(hm,"OGR_F_Destroy");
				ogrGetNumberOfPoints = (GetPointCountFunc)GetProcAddress(hm,"OGR_G_GetPointCount");
				ogrGetPoints = (GetPointsFunc)GetProcAddress(hm,"OGR_G_GetPoint");
				ogrGetSubGeometryCount = (GetSubGeometryCountFunc)GetProcAddress(hm,"OGR_G_GetGeometryCount");
				ogrGetSubGeometry = (GetSubGeometryRefFunc)GetProcAddress(hm,"OGR_G_GetGeometryRef");
				ogrGetSpatialRef = (GetSpatialRefFunc)GetProcAddress(hm,"OGR_L_GetSpatialRef");
				exportToWkt = (ExportToWktFunc)GetProcAddress(hm,"_OSRExportToWkt@8");
				ogrGetFeatureCount = (GetFeatureCountFunc)GetProcAddress(hm,"OGR_L_GetFeatureCount");
				ogrGetLayerExtent = (GetLayerExtentFunc)GetProcAddress(hm,"OGR_L_GetExtent");
				ogrGetFieldName = (GetFieldNameFunc)GetProcAddress(hm,"OGR_Fld_GetNameRef");

			} 
		}
	}   

}

CoordSystem GdalProxy::getCoordSystem(const FileName& fnBase, int epsg) {
	String path = getEngine()->getContext()->sIlwDir();
	path += "Resources\\gdal_data";
//	OGRSpatialReference oSRS;
	finderLoc(path.c_str());

	OGRSpatialReferenceH handle = newSRS(NULL);
	OGRErr err = fromEPSG( handle, epsg);

	if ( err == OGRERR_UNSUPPORTED_SRS )
		throw ErrorObject(String("The epsg %d is not supported", epsg));

	String datumName(getAttribute(handle, "Datum",0));
	//map<String, ProjectionConversionFunctions>::iterator where = mpCsyConvers.find(projectionName);

	FileName fnCsy(FileName::fnUnique(FileName(fnBase, ".csy")));

	CoordSystemViaLatLon *csv=NULL;
	if ( isProjected(handle)) {
		CoordSystemProjection *csp =  new CoordSystemProjection(fnCsy, 1);
		String dn = Datum::WKTToILWISName(datumName);
		//char *wkt = new char[5000];
		//getEngine()->gdal->wktPretty(handle,&wkt,FALSE);
		if ( dn != "" && dn != sUNDEF)
			csp->datum = new MolodenskyDatum(dn,"");
		String projName(getAttribute(handle, "Projection",0));
		if ( projName == "Oblique_Stereographic")
			projName = "Stereographic";
		if ( projName == "Lambert_Conformal_Conic_2SP")
			projName = "Lambert Conformal Conic";
		replace(projName.begin(), projName.end(),'_',' ');

		String spheroid = getEngine()->gdal->getAttribute(handle,"SPHEROID",0);
		try{
		Ellipsoid ell(spheroid);
		csp->ell = ell;
		} catch (ErrorObject& ) {
			String majoraxis = getEngine()->gdal->getAttribute(handle,"SPHEROID",1);
			String invFlattening = getEngine()->gdal->getAttribute(handle,"SPHEROID",2);
			double ma = majoraxis.rVal();
			double ifl = invFlattening.rVal();
			if ( ma == rUNDEF || ifl == rUNDEF)
				throw ErrorObject(String(TR("Ellipsoid %S could not be found").c_str(),spheroid));
			csp->ell = Ellipsoid(ma, ifl);
			csp->ell.sName = spheroid;


		} 


		double easting  = getEngine()->gdal->getProjParam(handle, "false_easting",rUNDEF,&err);
		double northing = getEngine()->gdal->getProjParam(handle, "false_northing",rUNDEF,&err);
		double scale = getEngine()->gdal->getProjParam(handle, "scale_factor",rUNDEF,&err);
		double centralMeridian = getEngine()->gdal->getProjParam(handle, "central_meridian",rUNDEF,&err);
		double lattOfOrigin = getEngine()->gdal->getProjParam(handle, "latitude_of_origin",rUNDEF,&err);
		double stParal1 = getEngine()->gdal->getProjParam(handle, "standard_parallel_1",rUNDEF,&err);
		double stParal2 = getEngine()->gdal->getProjParam(handle, "standard_parallel_2",rUNDEF,&err);
		csp->prj = Projection(projName,csp->ell);
		if ( easting != rUNDEF)
			csp->prj->Param(pvX0,easting);
		if ( northing != rUNDEF)
			csp->prj->Param(pvY0,northing);
		if ( scale != rUNDEF)
			csp->prj->Param(pvK0, scale);
		if ( centralMeridian != rUNDEF)
			csp->prj->Param(pvLON0, centralMeridian);
		if ( lattOfOrigin != rUNDEF)
			csp->prj->Param(pvLAT0, lattOfOrigin);
		if ( stParal1 != rUNDEF)
			csp->prj->Param(pvLAT1, min(stParal2, stParal1));
		if ( stParal2 != rUNDEF)
			csp->prj->Param(pvLAT2, max(stParal2, stParal1));
		csp->prj->Prepare();
		csv = csp;
		//delete [] wkt;

	} else {
		csv = new CoordSystemLatLon(fnCsy, 1);
		csv->datum = new MolodenskyDatum("WGS 1984","");
	}

	CoordSystem csy;
	csy.SetPointer(csv);
	csy->Store();

	return csy;
}
