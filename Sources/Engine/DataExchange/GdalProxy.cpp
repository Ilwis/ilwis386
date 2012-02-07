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
		if ( dn == "")
			throw ErrorObject("Datum can't be transformed to an ILWIS known datum");
		csp->datum = new MolodenskyDatum(dn,"");
		String projName(getAttribute(handle, "Projection",0));
		replace(projName.begin(), projName.end(),'_',' ');
		csp->prj = Projection(projName);

		//SetProjectionParm func = (*where).second.parmfunc;
		//String sIlwProj = (*where).second.sProjectionName;
		//(func)(csp->prj, (void *)(&oSRS));

		csv = csp;
	} else {
		csv = new CoordSystemLatLon(fnCsy, 1);
		csv->datum = new MolodenskyDatum("WGS 1984","");
	}

	CoordSystem csy;
	csy.SetPointer(csv);

	return csy;
}
