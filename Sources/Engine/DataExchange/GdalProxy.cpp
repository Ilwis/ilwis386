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
				registerJPEG = (GDALAllRegisterFunc)GetProcAddress(hm,"GDALRegister_JPEG");
				registerPNG = (GDALAllRegisterFunc)GetProcAddress(hm,"GDALRegister_PNG");
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
				vsiClose = (VSIFCloseLFunc)GetProcAddress(hm,"VSIFCloseL");
				vsiFileFromMem = (VSIFileFromMemBufferFunc)GetProcAddress(hm,"VSIFileFromMemBuffer");
				getGCPCount = (GDALGetGCPCountFunc)GetProcAddress(hm,"_GDALGetGCPCount@4");
				getGCPProjection = (GDALGetGCPProjectionFunc)GetProcAddress(hm,"_GDALGetGCPProjection@4");
				getGCPs = (GDALGetGCPsFunc)GetProcAddress(hm,"_GDALGetGCPs@4");
				getRasterColorInterpretation = (GDALGetRasterColorInterpretationFunc)GetProcAddress(hm,"_GDALGetRasterColorInterpretation@4");
				getRasterColorTable = (GDALGetRasterColorTableFunc)GetProcAddress(hm,"_GDALGetRasterColorTable@4");
				getPaletteInterpretation = (GDALGetPaletteInterpretationFunc)GetProcAddress(hm,"_GDALGetPaletteInterpretation@4");
				getColorEntryCount = (GDALGetColorEntryCountFunc)GetProcAddress(hm,"_GDALGetColorEntryCount@4");
				getColorEntry = (GDALGetColorEntryFunc)GetProcAddress(hm,"_GDALGetColorEntry@8");
				setUndefinedValue = (GDALSetRasterNoDataValueFunc)GetProcAddress(hm,"_GDALSetRasterNoDataValue@12");
				getUndefinedValue = (GDALGetRasterNoDataValueFunc)GetProcAddress(hm,"_GDALGetRasterNoDataValue@8");

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
				ogrtVal = (GetFieldAsDateTimeFunc)GetProcAddress(hm,"OGR_F_GetFieldAsDateTime");
				ogrGetGeometryRef = (GetGeometryRefFunc)GetProcAddress(hm,"OGR_F_GetGeometryRef");
				ogrGetGeometryType = (GetGeometryTypeFunc)GetProcAddress(hm,"OGR_G_GetGeometryType");
				ogrDestroyFeature = (DestroyFeatureFunc)GetProcAddress(hm,"OGR_F_Destroy");
				ogrGetNumberOfPoints = (GetPointCountFunc)GetProcAddress(hm,"OGR_G_GetPointCount");
				ogrGetPoints = (GetPointsFunc)GetProcAddress(hm,"OGR_G_GetPoint");
				ogrGetSubGeometryCount = (GetSubGeometryCountFunc)GetProcAddress(hm,"OGR_G_GetGeometryCount");
				ogrGetGeometryCount = (GetGeometryCountFunc)GetProcAddress(hm,"OGR_G_GetGeometryCount");
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

CoordSystem GdalProxy::getCoordSystem(const FileName& fnBase, OGRSpatialReferenceH handle) {
	String datumName(getAttribute(handle, "Datum",0));
	//map<String, ProjectionConversionFunctions>::iterator where = mpCsyConvers.find(projectionName);

	FileName fnCsy(FileName::fnUnique(FileName(fnBase, ".csy")));

	CoordSystemViaLatLon *csv=NULL;
	if ( isProjected(handle)) {
		try {
			CoordSystemProjection *csp =  new CoordSystemProjection(fnCsy, 1);
			String dn = Datum::WKTToILWISName(datumName);
			//char *wkt = new char[5000];
			//getEngine()->gdal->wktPretty(handle,&wkt,FALSE);
			if ( dn != "" && dn != sUNDEF)
				csp->datum = new MolodenskyDatum(dn,"");

			String spheroid = getEngine()->gdal->getAttribute(handle,"SPHEROID",0);
			try {
				try {
					Ellipsoid ell(spheroid);
					csp->ell = ell;
				} catch (ErrorNotFound&) {
					String spheroidIlw = spheroid;
					replace(spheroidIlw.begin(), spheroidIlw.end(),'_',' ');
					Ellipsoid ell(spheroidIlw);
					csp->ell = ell;
				}
			} catch (ErrorObject& ) {
				String majoraxis = getEngine()->gdal->getAttribute(handle,"SPHEROID",1);
				String invFlattening = getEngine()->gdal->getAttribute(handle,"SPHEROID",2);
				double ma = majoraxis.rVal();
				double ifl = invFlattening.rVal();
				if ( ma == rUNDEF || ifl == rUNDEF)
					throw ErrorObject(String(TR("Ellipsoid %S could not be found").c_str(),spheroid));
				csp->ell = Ellipsoid(ma, ifl);
			} 
			OGRErr err = CP_NONE;
			double easting  = getEngine()->gdal->getProjParam(handle, "false_easting",rUNDEF,&err);
			double northing = getEngine()->gdal->getProjParam(handle, "false_northing",rUNDEF,&err);
			double scale = getEngine()->gdal->getProjParam(handle, "scale_factor",rUNDEF,&err);
			double centralMeridian = getEngine()->gdal->getProjParam(handle, "central_meridian",rUNDEF,&err);
			double latOfOrigin = getEngine()->gdal->getProjParam(handle, "latitude_of_origin",rUNDEF,&err);
			double stParal1 = getEngine()->gdal->getProjParam(handle, "standard_parallel_1",rUNDEF,&err);
			double stParal2 = getEngine()->gdal->getProjParam(handle, "standard_parallel_2",rUNDEF,&err);
			String projName(getAttribute(handle, "Projection",0));
			if ( projName == "Oblique_Stereographic")
				projName = "Stereographic";
			else if ( projName == "Lambert_Conformal_Conic_2SP")
				projName = "Lambert Conformal Conic";
			else if ( projName == "Lambert_Conformal_Conic_1SP") {
				projName = "Lambert Conformal Conic";
				if (stParal1 == rUNDEF && stParal2 == rUNDEF) {
					stParal1 = latOfOrigin;
					stParal2 = latOfOrigin;
				} else if (stParal1 == rUNDEF)
					stParal1 = stParal2;
				else if (stParal2 == rUNDEF)
					stParal2 = stParal1;
			} else if ( projName == "Mercator_1SP")
				projName = "Mercator";
			else if ( projName == "Polar_Stereographic")
				projName = "StereoPolar";
			else if ( projName == "Albers_Conic_Equal_Area")
				projName = "Albers EqualArea Conic";
			replace(projName.begin(), projName.end(),'_',' ');
			if ( projName == "StereoPolar" && easting == 2000000.) // one of those exceptions
				projName = "UPS";
			csp->prj = Projection(projName,csp->ell);
			if ( easting != rUNDEF)
				csp->prj->Param(pvX0,easting);
			if ( northing != rUNDEF)
				csp->prj->Param(pvY0,northing);
			if ( scale != rUNDEF)
				csp->prj->Param(pvK0, scale);
			if ( centralMeridian != rUNDEF)
				csp->prj->Param(pvLON0, centralMeridian);
			if ( latOfOrigin != rUNDEF) {
				csp->prj->Param(pvLAT0, latOfOrigin);
				if (projName == "Mercator") // for now this is only an exception for mercator; more projections could need this
					csp->prj->Param(pvLATTS, latOfOrigin);
			}
			if ( stParal1 != rUNDEF)
				csp->prj->Param(pvLAT1, min(stParal2, stParal1));
			if ( stParal2 != rUNDEF)
				csp->prj->Param(pvLAT2, max(stParal2, stParal1));
			csp->prj->Prepare();
			csv = csp;
			//delete [] wkt;
		} catch ( ErrorObject& err) {
			return CoordSystem("unknown");
		}
	} else {
		csv = new CoordSystemLatLon(fnCsy, 1);
		csv->datum = new MolodenskyDatum("WGS 1984","");
		csv->ell = csv->datum->ell;
	}

	CoordSystem csy;
	csy.SetPointer(csv);
	csy->Store();

	return csy;
}

CoordSystem GdalProxy::getCoordSystem(const FileName& fnBase, const String& wkt) {
	String path = getEngine()->getContext()->sIlwDir();
	path += "Resources\\gdal_data";
//	OGRSpatialReference oSRS;
	finderLoc(path.c_str());

	OGRSpatialReferenceH handle = newSRS(NULL);
	char wktRaw[5000];
	
	strcpy(wktRaw, wkt.c_str());
	char *wkt2 = (char *)wktRaw;
	OGRErr err = srsImportFromWkt(handle, &wkt2);
	if ( err == OGRERR_UNSUPPORTED_SRS || err == OGRERR_CORRUPT_DATA)
		 return CoordSystem("unknown");

	return getCoordSystem(fnBase, handle);

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

	return getCoordSystem(fnBase, handle);
}

CoordSystem GdalProxy::getCoordSystemFromHandlePtr(const FileName& fnBase, void * phSRS) {
	return getCoordSystem(fnBase,*(OGRSpatialReferenceH*)phSRS);
}