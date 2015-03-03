#pragma once

#include "DataExchange\gdalincludes\gdal.h"
#include "DataExchange\gdalincludes\gdal_frmts.h"
#include "DataExchange\gdalincludes\cpl_vsi.h"
#include "DataExchange\gdalincludes\ogr_api.h"
#include "DataExchange\gdalincludes\ogr_srs_api.h"

typedef void(__stdcall *GDALCloseFunc)(GDALDatasetH);
typedef void(__stdcall *GDALAllRegisterFunc)();
typedef GDALDriverH (__stdcall *GDALIdentifyDriverFunc)(const char *, char **);
typedef GDALDatasetH (__stdcall *GDALOpenFunc) (const char *, GDALAccess );
typedef int (__stdcall *GDALGetNumbersDataSet)(GDALDatasetH);
typedef GDALRasterBandH (__stdcall *GDALGetRasterBandFunc) (GDALDatasetH, int);
typedef GDALDatasetH (__stdcall *GDALCreateFunc)(GDALDriverH hDriver, const char *, int, int, int, GDALDataType, char **);
typedef GDALDataType (__stdcall *GDALGetRasterDataTypeFunc) (GDALRasterBandH);
typedef const char* (__stdcall *GDALGetProjectionRefFunc)(GDALDatasetH);
typedef OGRSpatialReferenceH (__stdcall *OSRNewSpatialReferenceFunc)( const char *);
typedef OGRErr (*OSRImportFromWktFunc)( OGRSpatialReferenceH, char ** );
typedef OGRErr (*OSRSetWellKnownGeogCSFunc)( OGRSpatialReferenceH, char * );
//typedef OGRErr (*OSRImportFromEPSGFunc)(OGRSpatialReferenceH,int);
typedef int (*OSRIsProjectedFunc)( OGRSpatialReferenceH );
typedef CPLErr 	(__stdcall *GDALGetGeoTransformFunc)(GDALDatasetH, double *);
typedef CPLErr 	(__stdcall *GDALRasterIOFunc)(GDALRasterBandH hRBand, GDALRWFlag eRWFlag, int nDSXOff, int nDSYOff, int nDSXSize, int nDSYSize, void *pBuffer, int nBXSize, int nBYSize, GDALDataType eBDataType, int nLineSpace, int nBandSpace);
typedef int	(__stdcall *GDALGetDataTypeSizeFunc) (GDALDataType);
typedef int (__stdcall *GDALGetAccessFunc)(GDALDatasetH hDS);
typedef const char* (__stdcall  *OSRGetAttrValueFunc)( OGRSpatialReferenceH hSRS, const char * pszName, int iChild /* = 0 */ );
typedef GDALDriverH (__stdcall *GDALGetDriverFunc)(int driver);
typedef int (__stdcall *GDALGetDriverCountFunc)(void);
typedef const char* (__stdcall *GDALGetDriverLongNameFunc)(GDALDriverH dr);
typedef const char* (__stdcall *GDALGetDriverShortNameFunc)(GDALDriverH dr);
typedef const char* (__stdcall *GDALGetMetadataItemFunc)(GDALMajorObjectH hObject, const char *pszName, const char *pszDomain); 
typedef OGRErr (__stdcall *OSRImportFromEPSGFunc)( OGRSpatialReferenceH, int );
typedef OGRErr (__stdcall *OSRExportToPrettyWktFunc)(OGRSpatialReferenceH ,char **ppszReturn,int);
typedef double (*OSRGetProjParmFunc)(OGRSpatialReferenceH,const char *pszName,double dfDefaultValue,OGRErr * 	pnErr);
typedef int (*VSIFCloseLFunc)( FILE * );
typedef FILE* (*VSIFileFromMemBufferFunc)( const char *, GByte *, vsi_l_offset ,int  );
typedef int (__stdcall *GDALGetGCPCountFunc)(GDALDatasetH);
typedef const char* (__stdcall *GDALGetGCPProjectionFunc)(GDALDatasetH);
typedef const GDAL_GCP* (__stdcall *GDALGetGCPsFunc)(GDALDatasetH);

typedef  OGRDataSourceH (*OGROpenFunc)(const char *, int, OGRSFDriverH *);
typedef void (__stdcall *OGRRegisterAllFunc)();
typedef int (*OGRGetDriverCountFunc)();
typedef OGRSFDriverH (*OGRGetDriverFunc) (int);
typedef const char * (*OGRGetDriverNameFunc)(OGRSFDriverH);
typedef OGRSFDriverH  (*OGRGetDriverByNameFunc) (const char *);
typedef OGRLayerH (*GetLayerByNameFunc)	(OGRDataSourceH 	hDS, const char * pszLayerName);
typedef int (*GetLayerCountFunc)(OGRDataSourceH);
typedef OGRLayerH (*GetLayerFunc) (OGRDataSourceH, int);
typedef const char * (*GetLayerNameFunc)(OGRLayerH);
typedef OGRwkbGeometryType (*GetLayerGeometryTypeFunc)(OGRLayerH ) ;
typedef void (*ResetReadingFunc)(OGRLayerH 	hLayer	 ) 	;
typedef OGRFeatureH (*GetNextFeatureFunc)(OGRLayerH hLayer ) 	;
typedef OGRFeatureDefnH (*GetLayerDefnFunc)	(OGRLayerH hLayer );
typedef int (*GetFieldCountFunc)( OGRFeatureDefnH hDefn ) ;
typedef OGRFieldDefnH (*GetFieldDefnFunc)(OGRFeatureDefnH,int );
typedef OGRFieldType (*GetFieldTypeFunc)(OGRFieldDefnH 	hDefn );
typedef int (*GetFieldAsIntegerFunc)(OGRFeatureH,int);
typedef double (*GetFieldAsDoubleFunc)(OGRFeatureH,int);
typedef int (*GetFieldAsDateTimeFunc)(OGRFeatureH,int , int *pnYear,int *pnMonth,int *pnDay,int * pnHour,int *pnMinute,int *pnSecond,int *pnTZFlag);
typedef const char* (*GetFieldAsStringFunc)(OGRFeatureH,int);
typedef OGRGeometryH (*GetGeometryRefFunc)(OGRFeatureH );
typedef OGRwkbGeometryType (*GetGeometryTypeFunc)(OGRGeometryH);
typedef void (*DestroyFeatureFunc)(OGRFeatureH );
typedef int (*GetPointCountFunc)(OGRGeometryH);
typedef void (*GetPointsFunc)(OGRGeometryH,int,double *,double *,double *);
typedef int (*GetSubGeometryCountFunc)(OGRGeometryH); 
typedef int (*GetGeometryCountFunc)(OGRGeometryH);
typedef OGRGeometryH (*GetSubGeometryRefFunc)(OGRGeometryH,int);
typedef OGRSpatialReferenceH (*GetSpatialRefFunc)(OGRLayerH hLayer ) 	;
typedef OGRErr (__stdcall *ExportToWktFunc)(OGRSpatialReferenceH,char **);
typedef int (*GetFeatureCountFunc)(OGRLayerH,int);
typedef OGRErr 	(*GetLayerExtentFunc)(OGRLayerH, OGREnvelope *, int);
typedef const char * (*GetFieldNameFunc)(OGRFieldDefnH);
typedef const char * (__stdcall *OSRGetAttrValueFunc)( OGRSpatialReferenceH hSRS, const char * pszName, int iChild /* = 0 */ );
typedef void (*CPLPushFinderLocationFunc)( const char * );



class _export GdalProxy {
public:
	void loadMethods(const String& ilwDir);
	CoordSystem getCoordSystem(const FileName& fnBase, int epsg);
	CoordSystem getCoordSystem(const FileName& fnBase, const String& wkt);
	CoordSystem getCoordSystemFromHandlePtr(const FileName& fnBase, void * phSRS);

	GDALCloseFunc close;
	GDALAllRegisterFunc registerAll;
	GDALAllRegisterFunc registerJPEG;
	GDALAllRegisterFunc registerPNG;
	GDALIdentifyDriverFunc identifyDriver;
	GDALOpenFunc open;
	GDALGetNumbersDataSet xSize;
	GDALGetNumbersDataSet ySize;
	GDALGetNumbersDataSet count;
	GDALGetRasterBandFunc getBand;
	GDALCreateFunc create;
	GDALGetRasterDataTypeFunc getDataType;
	GDALGetProjectionRefFunc getProjectionRef;
	OSRNewSpatialReferenceFunc newSRS;
	OSRImportFromWktFunc srsImportFromWkt;
	OSRIsProjectedFunc isProjected;
	GDALGetGeoTransformFunc geotransform;
	GDALRasterIOFunc rasterIO;
	GDALGetDataTypeSizeFunc dataSize;
	GDALGetAccessFunc access;
	OSRGetAttrValueFunc getAttr;
	GDALGetDriverFunc getDriver;
	GDALGetDriverCountFunc getDriverCount;
	GDALGetDriverLongNameFunc getDriverLongName;
	GDALGetDriverShortNameFunc getDriverShortName;
	GDALGetMetadataItemFunc getMetaDataItem;
	OSRSetWellKnownGeogCSFunc fromCode;
	OSRImportFromEPSGFunc fromEPSG;
	OSRGetAttrValueFunc getAttribute;
	CPLPushFinderLocationFunc finderLoc;
	OSRExportToPrettyWktFunc wktPretty;
	OSRGetProjParmFunc getProjParam;
	VSIFCloseLFunc vsiClose;
	VSIFileFromMemBufferFunc vsiFileFromMem;
	GDALGetGCPCountFunc getGCPCount;
	GDALGetGCPProjectionFunc getGCPProjection;
	GDALGetGCPsFunc getGCPs;

	//ogr
	OGROpenFunc ogrOpen;
	OGRRegisterAllFunc ogrRegAll;
	OGRGetDriverCountFunc ogrGetDriverCount;
	OGRGetDriverFunc ogrGetDriver;
	OGRGetDriverNameFunc ogrGetDriverName;
	OGRGetDriverByNameFunc ogrGetDriverByName;
	GetLayerByNameFunc ogrGetLayerByName;
	GetLayerCountFunc ogrGetLayerCount;
	GetLayerFunc ogrGetLayer;
	GetLayerNameFunc ogrGetLayerName;
	GetLayerGeometryTypeFunc ogrGetLayerGeometryType;
	ResetReadingFunc ogrResetReading;
	GetNextFeatureFunc ogrGetNextFeature;
	GetLayerDefnFunc ogrGetLayerDefintion;
	GetFieldCountFunc ogrGetFieldCount;
	GetFieldDefnFunc ogrGetFieldDefinition;
	GetFieldTypeFunc ogrGetFieldType;
	GetFieldAsStringFunc ogrsVal;
	GetFieldAsDoubleFunc ogrrVal;
	GetFieldAsIntegerFunc ogriVal;
	GetFieldAsDateTimeFunc ogrtVal;
	GetGeometryRefFunc ogrGetGeometryRef;
	GetGeometryTypeFunc ogrGetGeometryType;
	DestroyFeatureFunc ogrDestroyFeature;
	GetPointCountFunc ogrGetNumberOfPoints;
	GetPointsFunc ogrGetPoints;
	GetSubGeometryCountFunc ogrGetSubGeometryCount;
	GetGeometryCountFunc ogrGetGeometryCount;
	GetSubGeometryRefFunc ogrGetSubGeometry;
	GetSpatialRefFunc ogrGetSpatialRef;
	ExportToWktFunc exportToWkt;
	GetFeatureCountFunc ogrGetFeatureCount;
	GetLayerExtentFunc ogrGetLayerExtent;
	GetFieldNameFunc ogrGetFieldName;

private:
	CoordSystem getCoordSystem(const FileName& fnBase, OGRSpatialReferenceH handle);
};