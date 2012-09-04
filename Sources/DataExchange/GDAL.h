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

#ifndef ILWGDAL_H
#define ILWGDAL_H

struct GDBLayer_t;

void ogrgdal(const String& cmd);
void rastergdal(const String& cmd);

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

typedef const char* (__stdcall *CPLGetLastErrorFunc)(void);

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




struct GDALCFunctions {
	GDALCloseFunc close;
	GDALAllRegisterFunc registerAll;
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

	CPLGetLastErrorFunc errorMsg;

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
	GetFieldAsDateTimeFunc ogrtVal;
	GetFieldAsIntegerFunc ogriVal;
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





};

struct GDALImportFormat : ImportFormat {
};

ForeignFormat *CreateImportObjectGDAL(const FileName& fnFO, ParmList& pm);
ForeignFormat *CreateQueryObjectGDAL();

struct FormatInfo
{
	enum Type{ tRASTER, tVECTOR, tTABLE};
	enum ExpProp{ ep8SBIT, ep8UBIT, ep16SBIT, ep16UBIT, epREAL, epBANDS};
	
	FormatInfo() : sDisplayName(""), fUpdate(false), fCreate(false), fImport(true) {}
	FormatInfo(String sDN, String sF, String sEx, Type t, bool fUpd, bool fCr, const String& sExp) : 
		sDisplayName(sDN), sFormatName(sF), sExt(sEx), fUpdate(fUpd), fCreate(fCr), fImport(true), sExportProperties(sExp) {}

	bool fSupports(ExpProp prop);
	
	String sDisplayName;
	String sFormatName;
	String sExt;
	Type tType;
	bool fUpdate;
	bool fCreate;
	bool fImport;
	String sExportProperties;
};

class GeometryFiller {
public:
	virtual void fillFeature(OGRGeometryH hGeom, int& rec);
protected:
	GeometryFiller(GDALCFunctions& _funcs, BaseMap& _bmp) : bmp(_bmp), funcs(_funcs) {}
	virtual void fillGeometry(OGRGeometryH hGeom, int& rec) {};
	BaseMap& bmp;
	GDALCFunctions& funcs;

};

class PointFiller : public GeometryFiller {
public:
	PointFiller(GDALCFunctions& _funcs, BaseMap& _bmp) : GeometryFiller(_funcs, _bmp) {}
private:
	void fillGeometry(OGRGeometryH hGeom, int& rec);
};

class SegmentFiller : public GeometryFiller {
public:
	SegmentFiller(GDALCFunctions& _funcs, BaseMap& _bmp) : GeometryFiller(_funcs, _bmp) {}

private:
	void fillGeometry(OGRGeometryH hGeom, int& rec);
};

class PolygonFiller : public GeometryFiller {
public:
	PolygonFiller(GDALCFunctions& _funcs, BaseMap& _bmp) : GeometryFiller(_funcs, _bmp) {}
	virtual void fillFeature(OGRGeometryH hGeom, int& rec);
private:
	void fillPolygon(int count, int rec, OGRGeometryH hGeometry);
	LinearRing *getRing(OGRGeometryH hGeom);
};

class GDALFormat : public ForeignFormat
{
	friend UINT PutDataInThread(LPVOID lp);

public:
	GDALFormat();
	GDALFormat(const FileName& fn, ParmList& pm, ForeignFormat::mtMapType _mtType = ForeignFormat::mtUnknown);

	~GDALFormat();
	void                         IterateLayer(vector<LayerInfo>& objects, bool fCreate=true);
	virtual bool _export         fIsCollection(const FileName& fnForeignObject) const;
	virtual bool _export         fMatchType(const String& fnFileName, const String& sType);
	virtual void _export         PutDataInCollection(ForeignCollectionPtr* col, ParmList& pm); 
	void                         GetRasterInfo(LayerInfo& inf, int iChannel, bool fBitMap);
	void                         GetGeoRef(GeoRef& grf);
	virtual void _export         GetLineRaw(long iLine, ByteBuf&, long iFrom, long iNum) const ;
	virtual void _export         GetLineRaw(long iLine, IntBuf&,  long iFrom, long iNum) const ;
	virtual void _export         GetLineRaw(long iLine, LongBuf&, long iFrom, long iNum) const ;
	virtual void _export         GetLineVal(long iLine, LongBuf&, long iFrom, long iNum) const ;
	virtual void _export         GetLineVal(long iLine, RealBuf&, long iFrom, long iNum) const ;
	virtual void _export         PutLineRaw(const FileName& fnMap,long iLine, const ByteBuf&, long iFrom, long iNum) ;
	virtual void _export         PutLineRaw(const FileName& fnMap,long iLine, const IntBuf&,  long iFrom, long iNum) ;
	virtual void _export         PutLineRaw(const FileName& fnMap,long iLine, const LongBuf&, long iFrom, long iNum) ;
	virtual void _export         PutLineVal(const FileName& fnMap,long iLine, const LongBuf&, long iFrom, long iNum) ;
	virtual void _export         PutLineVal(const FileName& fnMap,long iLine, const RealBuf&, long iFrom, long iNum) ;   		
	virtual long _export         iRaw(RowCol) const;
	virtual long _export         iValue(RowCol) const;
	virtual double _export       rValue(RowCol) const;
	virtual bool _export		 fIsSupported(const FileName& fn, ForeignFormat::Capability dtType = cbANY) const;

	virtual CoordSystem _export  GetCoordSystem();
	virtual void _export         SetForeignGeoTransformations(const CoordSystem &cs, const GeoRef& grf);
	void _export				 getImportFormats(vector<ImportFormat>& formats);
	void _export				 ReadParameters(const FileName& fnObj, ParmList& pm);
	void _export				 Store(IlwisObject ob, int iLayerIndex);
	void						 ogr(const String& name, const String& source, const String& target);

private:
	void           Init();
	bool           fReUseExisting(const FileName& fn);
	void           CreateLayer(vector<LayerInfo> &li);

	GDALRasterBandH  OpenLayer(int iChannel);
	void           CreateLineBuffer(long iSize);
	void		   CreateRasterLayer(int iMaxChannels, bool fBitMap);
	void           ImportRasterMap(const FileName& fnRasMap, Map& mp ,LayerInfo& li, int iChannel);
	void		   GetRasterLayer(int iLayerIndex, Map& mp, Array<FileName>& arMaps, GeoRef& grf, 	Domain& dm,  bool fBitMap);
	void           GetBitValues(unsigned char * buffer, LongBuf& buf, long iNum) const;

	void           ReadLayers();
	void           ReadAllLayers();
	bool           fCombinePossible();
	FileName       fnCreateFileName(const FileName& fnBase);

	static UINT    PutDataInThread(LPVOID lp);
	void           ReadForeignFormat(ForeignCollectionPtr* col);
	void		   AddNewFiles(const FileName& fnNew);
	int   iFormatID(const String& sFormat) const;
	void		   InitGDAL();
	FormatInfo	   fiGetFormatInfo(const String sExt) const;
	void		   CleanUp();
	void           InitExportMaps(const FileName& fn, ParmList& pm);
	void           DetermineOutputType(const FileName& fnMp, int& iBands, GDALDataType& iDataType);

	void		   LoadMethods();
	Feature::FeatureType getFeatureType(OGRLayerH hLayer) const;
	CoordSystem	getCoordSystemFrom(OGRSpatialReferenceH handle, char *wkt);
	FileName	createFileName( const String& name, Feature::FeatureType ftype, int layerCount, int layer);
	BaseMap     createBaseMap(const FileName& fn, Feature::FeatureType ftype, const Domain& dm, const CoordSystem& csy, const CoordBounds& cb);
	void		createTable(const FileName& fn, const Domain& dm,OGRFeatureDefnH hFeatureDef, OGRLayerH hLayer, Table& tbl);
	CoordBounds getLayerCoordBounds(OGRLayerH hLayer);
	Domain		createSortDomain(const String& name, const vector<String>& values);

	int iLayer;
	GDALDatasetH dataSet; ; //multiple are only used when exporting (bands)
	GDALRasterBandH currentLayer;
	GDALDataType dataType;
	unsigned char *buffer;
	bool fCombine;
	bool fImport;
	bool fExport;
	bool fShowCollection;
	bool f4BytesInt; // used for raster; 
	FileName fnBaseOutputName;
	list<CoordSystem> addedCsy;
	static map<int, FormatInfo> mapFormatInfo;
	GDALDriverH gdalDriver; // export datat type, id as in ccltask.h
	list<FileName> fnUsedFileNames; // files do not yet exist on disk, but uniqueness must be guaranteed
	static CCriticalSection m_CriticalSection;
	GDALCFunctions funcs;

		//creation info
};

;

#endif
