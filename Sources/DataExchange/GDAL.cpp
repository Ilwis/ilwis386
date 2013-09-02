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

#define DomainInfo ggDomainInfo
#include "DataExchange\gdalincludes\gdal.h"
#include "DataExchange\gdalincludes\gdal_frmts.h"
#include "DataExchange\gdalincludes\cpl_vsi.h"
#include "DataExchange\gdalincludes\ogr_api.h"
#include "DataExchange\gdalincludes\ogr_srs_api.h"
#include <geos/algorithm/CGAlgorithms.h>


#undef DomainInfo

#pragma warning( disable : 4786 )

#include "Headers\toolspch.h"

#pragma warning( disable : 4715 )

#include <set>
#include "Engine\Base\DataObjects\ilwisobj.h"
#include "Engine\SpatialReference\Coordsys.h"
#include "Engine\SpatialReference\Csproj.h"
#include "Engine\SpatialReference\Cslatlon.h"
#include "Engine\SpatialReference\Ellips.h"
#include "Engine\Domain\dmcoord.h"
#include "Engine\base\System\engine.h"
#include "Engine\Domain\dminfo.h"
#include "Engine\Domain\dmsort.h"
#include "Engine\Domain\DomainUniqueID.h"
#include "Engine\DataExchange\TableForeign.h"
#include "Engine\Base\File\objinfo.h"
#include "Engine\SpatialReference\prj.h"
#include "Engine\Map\Segment\Seg.h"
#include "Engine\Map\Polygon\POLAREA.H"
#include "Engine\Map\Point\PNT.H"
#include "Engine\DataExchange\ForeignFormat.h"
#include "Engine\SpatialReference\Grcornrs.h"
#include "Engine\Base\File\Directory.h"
#include "Engine\Map\Raster\Map.h"
#include "Engine\Map\Raster\MapList\maplist.h"
//#include "Client\ilwis.h"
#include "Engine\Base\DataObjects\Tranq.h"
//#include "Engine\Applications\ModuleMap.h"
#include "Engine\Base\System\Engine.h"
#include "Headers\constant.h"
#include "Headers\messages.h"
#include "Engine\Base\DataObjects\URL.h"
#include "DataExchange\WMSFormat.h"
#include "DataExchange\GDAL.h"
#include "Engine\DataExchange\GdalProxy.h"
#include "Headers\Hs\IMPEXP.hs"
#include "Headers\Htp\Ilwismen.htp"
#include "Engine\Base\Round.h"


map<int, FormatInfo> mapDummy;
map<int, FormatInfo> GDALFormat::mapFormatInfo = mapDummy;

static const int iGDB_NOT_FOUND = -1;
const String sZCOLUMNNAME("Z_Value");

CCriticalSection GDALFormat::m_CriticalSection;

struct gdaldata{
	String cmd;
	String dir;
};

UINT OgrInThread(void * data) {
	try{
	gdaldata *d = (gdaldata *)data;
	getEngine()->InitThreadLocalVars();
	GDALFormat frmt;
	ParmList parms(d->cmd);
	getEngine()->SetCurDir(d->dir);
	frmt.ogr(parms.sGet(0), parms.sGet(1), parms.sGet(2));
	delete d;
	getEngine()->RemoveThreadLocalVars();

	}	catch (std::exception& err) {
		getEngine()->RemoveThreadLocalVars();
		const char *txt = err.what();
		String mes("%s, probably invalid or corrupt data", txt);
		ErrorObject errObj(mes);
		throw errObj;
	}

	return 1;
}


void ogrgdal(const String& cmd) {
	ParmList parms(cmd);
	if ( !parms.fExist("quiet")) {
		gdaldata *d = new gdaldata();
		d->cmd = cmd;
		d->dir = getEngine()->sGetCurDir();
		AfxBeginThread(OgrInThread, (LPVOID)d);
	} else {
		GDALFormat frmt;
		frmt.ogr(parms.sGet(0), parms.sGet(1), parms.sGet(2));
	}

}

void rastergdal(const String& cmd) {
	ParmList pmInput(cmd);
	ParmList pm;
	pm.Add(new Parm("import", true));
	pm.Add(new Parm("method",String("GDAL")));
	pm.Add(new Parm("input",pmInput.sGet(0)));
	pm.Add(new Parm("output", pmInput.sGet(1)));
	ForeignCollection col(pmInput.sGet(0), pm);
	pm.Add(new Parm("threading",false));
	col->Create(pm);

}

class StopConversion : public ErrorObject
{};   

class ReadingLayerError : public ErrorObject
{
public:
		ReadingLayerError(int iLayer) :
			ErrorObject(String(TR("Error reading layer %d, layer skipped").c_str(), iLayer)) {}
};

//----------------------------------------------------------------------------------------
bool FormatInfo::fSupports(FormatInfo::ExpProp prop)
{
	int iPlace = (int)prop;
	return sExportProperties[iPlace] == '1';
}
//----------------------------------------------------------------------------------------

GDALFormat::GDALFormat()
{
	iLayer = iUNDEF;
	dataSet = NULL;	
	buffer = NULL;
	fCombine = f4BytesInt = false;
	gdalDriver = NULL;
	fShowCollection = false;
	currentLayer= NULL;
	LoadMethods();
	funcs.registerAll();
	funcs.ogrRegAll();
	//OGRDataSourceH p = funcs.ogrOpen("d:\\Data\\ILWIS\\data\\shape\\natural.shp",FALSE,0);
}

ForeignFormat *CreateQueryObjectGDAL() //create query object
{
	return new GDALFormat();
}

ForeignFormat *CreateImportObjectGDAL(const FileName& fnObj, ParmList& pm) //create import object
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

	return new GDALFormat(fnObj, pm);
}


GDALFormat::GDALFormat(const FileName& fn, ParmList& pm, ForeignFormat::mtMapType _mtType) :
	ForeignFormat(pm.sGet("input"), _mtType),
	buffer(0),
	fImport(false),
	fExport(false),
	fShowCollection(false),
	currentLayer(NULL)
{
	if ( sForeignInput == "")
		sForeignInput = FileName(pm.sGet(0)).sRelative();
	if ( pm.iSize() == 0)
		ReadParameters(fn,pm);

	dataSet = NULL;

	fExport = pm.fExist("export");
	fImport = pm.fExist("import");
	fShowCollection = !pm.fExist("noshow");	
	
	if (!fExport)
	{

	  FileName fnOutput = pm.sGet("output");
	  bool fAbsolute = false;
	  if ( fnOutput != FileName() )
		{
			if ( fnOutput.sFile == "" ) // empty name, use default name
			{
				fnBaseOutputName = fnOutput;
				if ( !fnBaseOutputName.fValid())
					fnBaseOutputName.sFile = fn.sFile;
			}
			else
				fnBaseOutputName = fn;

			fAbsolute = true;
		}
		if ( fnBaseOutputName == FileName() )
		{
				fnBaseOutputName = fn;
				fnBaseOutputName.sFile = fnBaseOutputName.sFile.sTrimSpaces(); // ilwis can not handle name that start wit a space;
		}					

		fCombine = pm.fExist("combine");
		iLayer = 1;
		if ( pm.fExist("layer"))
			iLayer = pm.sGet("layer").iVal();

	}
	else
		InitExportMaps(fn, pm);

}

GDALFormat::~GDALFormat()
{
	CleanUp();
}

void GDALFormat::CleanUp()
{
	if ( trq )
	{
		trq->Stop();
		delete trq;
		trq = NULL;
	}
	for(int i = 0; i < 3; ++i)
	{
		if ( dataSet ) 
		{
			ILWISSingleLock lock(&m_CriticalSection, TRUE, SOURCE_LOCATION);	 			
			funcs.close(dataSet );
			currentLayer = NULL;
			dataSet = NULL;	
		}			
	}		
	if ( buffer) delete [] buffer;
	buffer = NULL;	
}

void GDALFormat::InitExportMaps(const FileName& fn, ParmList& pm)
{
	String sFormat = pm.sGet("format");
	FileName fnSource = pm.sGet("input");

	ILWISSingleLock lock(&m_CriticalSection, TRUE, SOURCE_LOCATION);	 	
	int id;
	if ( (id = iFormatID(sFormat)) != iUNDEF)
	{
			FileName fnNew(fn);
			if ( fnNew.sExt == "")
				fnNew.sExt = mapFormatInfo[id].sExt;		
			Map mp(fnSource);	
			int iRow = mp->rcSize().Row;
			int iCol = mp->rcSize().Col;			
			DomainValueRangeStruct dvrs = mp->dvrs();
			int iBands;
		 	GDALDataType gdalDataType;
			DetermineOutputType(fnSource, iBands, gdalDataType);
			bool fRGB = false;
			if ( gdalDataType == iUNDEF) // will go the RGB
			{
				gdalDataType = GDT_Byte;
				fRGB = true;
			}				
			if ( mapFormatInfo[id].fSupports(FormatInfo::epBANDS))
				dataSet = funcs.create(gdalDriver, fnNew.sFullPath().c_str(), iCol, iRow, iBands, gdalDataType, NULL);
			else
			{
				for(int i=0; i < iBands; ++i)
				{
					FileName fnN;
					if ( fRGB)
					{
						String sCol = i == 0 ? "red" : i == 1 ? "green" : "blue";
						fnN = FileName::fnUnique(FileName(String("%S_%S", fnNew.sFullPath(false), sCol), fnNew.sExt));
  				}
					else
						fnN = FileName::fnUnique(fnNew);
					dataSet = funcs.create(gdalDriver, fnN.sFullPath().c_str(), iCol, iRow, 1, gdalDataType, NULL);
				}					
			}				
			if ( !dataSet )
				throw ErrorObject(String(TR("No acceptable conversion method found").c_str(), fnGetForeignFile().sFullPath()));	
			iLayer = 1;
	}
	else
		throw ErrorObject(TR("File format is not recognized by GDAL"));
}

void GDALFormat::DetermineOutputType(const FileName& fnMap, int& iBands, GDALDataType& gdalDataType) 
{
	iBands = 1;
	BaseMap mp(fnMap);

	RangeInt riRange = mp->riMinMax();
	bool fRecalc = !riRange.fValid()  ;

	
	gdalDataType = GDT_Unknown; //  if iUNDEF is the result, the map will be split into three RGB bands based on the rpr
	int id = 0;
	if ( gdalDriver == 0 )
	{
		FormatInfo fi = fiGetFormatInfo(fnGetForeignFile().sExt);
		id = iFormatID(fi.sFormatName);
	}		
	if ( mp->dm()->pdc() )
	{
		if ( mapFormatInfo[id].fSupports(FormatInfo::ep8UBIT) )
			gdalDataType = GDT_Byte;		
		else if ( mapFormatInfo[id].fSupports(FormatInfo::ep16SBIT) )
			gdalDataType = GDT_Int16;				
		else if ( mapFormatInfo[id].fSupports(FormatInfo::ep16UBIT) )
			gdalDataType = GDT_UInt16;			
	}
	else if ( mp->dm()->pdid() )
	{
		int iSizeNeeded = mp->dm()->pdsrt()->iNettoSize();
		if ( iSizeNeeded < 255 && mapFormatInfo[id].fSupports(FormatInfo::ep8UBIT) )
			gdalDataType = GDT_Byte;
		else if ( mapFormatInfo[id].fSupports(FormatInfo::ep16UBIT) )
			gdalDataType = GDT_UInt16;
		else if ( mapFormatInfo[id].fSupports(FormatInfo::ep16SBIT) )
			gdalDataType = GDT_Int16;			
	}
	else if ( mp->fRealValues() )
	{
		if ( mapFormatInfo[id].fSupports(FormatInfo::epREAL) )
			gdalDataType = GDT_Float32;
	}			
	else if (  RangeInt(0, 255).fContains( mp->riMinMax(fRecalc ? BaseMapPtr::mmmCALCULATE : BaseMapPtr::mmmNOCALCULATE)))
	{
		if ( mapFormatInfo[id].fSupports(FormatInfo::ep8UBIT) )
			gdalDataType = GDT_Byte;
		else if ( mapFormatInfo[id].fSupports(FormatInfo::ep16UBIT) )
			gdalDataType = GDT_UInt16;
		else if ( mapFormatInfo[id].fSupports(FormatInfo::ep16SBIT) )
			gdalDataType = GDT_Int16;			
	}		
	else if (  RangeInt(0, 65535).fContains( mp->riMinMax(fRecalc ? BaseMapPtr::mmmCALCULATE : BaseMapPtr::mmmNOCALCULATE)))
	{
		if ( mapFormatInfo[id].fSupports(FormatInfo::ep16UBIT) )
			gdalDataType = GDT_UInt16;
	}		
	else if (  RangeInt(-32767, 32768).fContains( mp->riMinMax(fRecalc ? BaseMapPtr::mmmCALCULATE : BaseMapPtr::mmmNOCALCULATE)))
	{
		if ( mapFormatInfo[id].fSupports(FormatInfo::ep16SBIT) )
			gdalDataType = GDT_Int16;		
	}	
	if ( gdalDataType == -1)
		iBands = 3; // export 3 RGB colors;
}

void GDALFormat::InitGDAL()
{
	if ( dataSet ) return; // already done

		
	if ( mapFormatInfo.size() == 0 )
	{
		String sPath = getEngine()->getContext()->sIlwDir() + "System";
		File FormatDef(sPath + "\\GDBFormatInfo.def");

		while (!FormatDef.fEof() )	
		{
			String sLine;
			FormatDef.ReadLnAscii(sLine);	
			if ( sLine[0] !=';')
			{
				Array<String> arParts;
				Split(sLine, arParts, ",");
				if ( arParts.size() > 6  && arParts[3].iVal() != -1 )
				{
					for(int j=0; j<arParts.size(); ++j )
						arParts[j] = arParts[j].sTrimSpaces();
					FormatInfo fi(arParts[0], arParts[1], arParts[2], (FormatInfo::Type)(arParts[4].iVal()), arParts[5].fVal(), arParts[6].fVal(), arParts[7]);  
					mapFormatInfo[arParts[3].iVal()] = fi;
				}				
			}			
		}
	}
}

void GDALFormat::LoadMethods() {
	CFileFind finder;
	String path = getEngine()->getContext()->sIlwDir() + "\\gdal*.dll";
	BOOL fFound = finder.FindFile(path.c_str());
	while(fFound) {
		fFound = finder.FindNextFile();
		if (!finder.IsDirectory())
		{
			FileName fnModule (finder.GetFilePath());
			HMODULE hm = LoadLibrary(fnModule.sFullPath().c_str());
			if ( hm != NULL ) {
				funcs.close = (GDALCloseFunc)GetProcAddress(hm, "_GDALClose@4");
				funcs.registerAll = (GDALAllRegisterFunc)GetProcAddress(hm,"_GDALAllRegister@0");
				funcs.identifyDriver = (GDALIdentifyDriverFunc)GetProcAddress(hm,"_GDALIdentifyDriver@8");
				funcs.open = (GDALOpenFunc)GetProcAddress(hm,"_GDALOpen@8");
				funcs.xSize = (GDALGetNumbersDataSet)GetProcAddress(hm,"_GDALGetRasterXSize@4");
				funcs.ySize = (GDALGetNumbersDataSet)GetProcAddress(hm,"_GDALGetRasterYSize@4");
				funcs.count = (GDALGetNumbersDataSet)GetProcAddress(hm,"_GDALGetRasterCount@4");
				funcs.getBand = (GDALGetRasterBandFunc)GetProcAddress(hm,"_GDALGetRasterBand@8");
				funcs.create = (GDALCreateFunc)GetProcAddress(hm, "_GDALCreate@28" );
				funcs.getProjectionRef = (GDALGetProjectionRefFunc)GetProcAddress(hm, "_GDALGetProjectionRef@4" );
				funcs.getDataType = (GDALGetRasterDataTypeFunc)GetProcAddress(hm, "_GDALGetRasterDataType@4");
				funcs.newSRS = (OSRNewSpatialReferenceFunc)GetProcAddress(hm, "_OSRNewSpatialReference@4");
				funcs.fromCode = (OSRSetWellKnownGeogCSFunc)GetProcAddress(hm, "OSRSetWellKnownGeogCS");
				funcs.fromEPSG = (OSRImportFromEPSGFunc)GetProcAddress(hm,"_OSRImportFromEPSG@8");
				funcs.srsImportFromWkt = (OSRImportFromWktFunc)GetProcAddress(hm,"OSRImportFromWkt");
				funcs.isProjected = (OSRIsProjectedFunc)GetProcAddress(hm,"OSRIsProjected");
				funcs.geotransform = (GDALGetGeoTransformFunc)GetProcAddress(hm,"_GDALGetGeoTransform@8");
				funcs.rasterIO = (GDALRasterIOFunc)GetProcAddress(hm,"_GDALRasterIO@48");
				funcs.dataSize = (GDALGetDataTypeSizeFunc)GetProcAddress(hm,"_GDALGetDataTypeSize@4");
				funcs.access = (GDALGetAccessFunc)GetProcAddress(hm,"_GDALGetAccess@4");
				funcs.getAttr = (OSRGetAttrValueFunc)GetProcAddress(hm,"_OSRGetAttrValue@12");
				funcs.getDriver = (GDALGetDriverFunc)GetProcAddress(hm,"_GDALGetDriver@4");
				funcs.getDriverCount = (GDALGetDriverCountFunc)GetProcAddress(hm,"_GDALGetDriverCount@0");
				funcs.getDriverLongName = (GDALGetDriverLongNameFunc)GetProcAddress(hm,"_GDALGetDriverLongName@4");
				funcs.getDriverShortName = (GDALGetDriverShortNameFunc)GetProcAddress(hm,"_GDALGetDriverShortName@4");
				funcs.getMetaDataItem = (GDALGetMetadataItemFunc)GetProcAddress(hm,"_GDALGetMetadataItem@12");

				funcs.errorMsg = (CPLGetLastErrorFunc)GetProcAddress(hm, "_CPLGetLastErrorMsg@0");

				funcs.ogrOpen = (OGROpenFunc)GetProcAddress(hm,"OGROpen");
				funcs.ogrRegAll = (OGRRegisterAllFunc)GetProcAddress(hm,"OGRRegisterAll");
				funcs.ogrGetDriverCount = (OGRGetDriverCountFunc)GetProcAddress(hm,"OGRGetDriverCount");
				funcs.ogrGetDriver = (OGRGetDriverFunc)GetProcAddress(hm,"OGRGetDriver");
				funcs.ogrGetDriverName = (OGRGetDriverNameFunc)GetProcAddress(hm,"OGR_Dr_GetName");
				funcs.ogrGetDriverByName = (OGRGetDriverByNameFunc)GetProcAddress(hm,"OGRGetDriverByName");
				funcs.ogrGetLayerByName = (GetLayerByNameFunc)GetProcAddress(hm,"OGR_DS_GetLayerByName");
				funcs.ogrGetLayerCount = (GetLayerCountFunc)GetProcAddress(hm,"OGR_DS_GetLayerCount");
				funcs.ogrGetLayer = (GetLayerFunc)GetProcAddress(hm,"OGR_DS_GetLayer");
				funcs.ogrGetLayerName = (GetLayerNameFunc)GetProcAddress(hm,"OGR_L_GetName");
				funcs.ogrGetLayerGeometryType = (GetLayerGeometryTypeFunc)GetProcAddress(hm,"OGR_L_GetGeomType");
				funcs.ogrResetReading = (ResetReadingFunc)GetProcAddress(hm,"OGR_L_ResetReading");
				funcs.ogrGetNextFeature = (GetNextFeatureFunc)GetProcAddress(hm,"OGR_L_GetNextFeature");
				funcs.ogrGetLayerDefintion = (GetLayerDefnFunc)GetProcAddress(hm,"OGR_L_GetLayerDefn");
				funcs.ogrGetFieldCount = (GetFieldCountFunc)GetProcAddress(hm,"OGR_FD_GetFieldCount");
				funcs.ogrGetFieldDefinition = (GetFieldDefnFunc)GetProcAddress(hm,"OGR_FD_GetFieldDefn");
				funcs.ogrGetFieldType = (GetFieldTypeFunc)GetProcAddress(hm,"OGR_Fld_GetType");
				funcs.ogrsVal = (GetFieldAsStringFunc)GetProcAddress(hm,"OGR_F_GetFieldAsString");
				funcs.ogrrVal = (GetFieldAsDoubleFunc)GetProcAddress(hm,"OGR_F_GetFieldAsDouble");
				funcs.ogriVal = (GetFieldAsIntegerFunc)GetProcAddress(hm,"OGR_F_GetFieldAsInteger");
				funcs.ogrtVal = (GetFieldAsDateTimeFunc)GetProcAddress(hm,"OGR_F_GetFieldAsDateTime");
				funcs.ogrGetGeometryRef = (GetGeometryRefFunc)GetProcAddress(hm,"OGR_F_GetGeometryRef");
				funcs.ogrGetGeometryType = (GetGeometryTypeFunc)GetProcAddress(hm,"OGR_G_GetGeometryType");
				funcs.ogrGetGeometryCount = (GetGeometryCountFunc)GetProcAddress(hm,"OGR_G_GetGeometryCount");
				funcs.ogrDestroyFeature = (DestroyFeatureFunc)GetProcAddress(hm,"OGR_F_Destroy");
				funcs.ogrGetNumberOfPoints = (GetPointCountFunc)GetProcAddress(hm,"OGR_G_GetPointCount");
				funcs.ogrGetPoints = (GetPointsFunc)GetProcAddress(hm,"OGR_G_GetPoint");
				funcs.ogrGetSubGeometryCount = (GetSubGeometryCountFunc)GetProcAddress(hm,"OGR_G_GetGeometryCount");
				funcs.ogrGetSubGeometry = (GetSubGeometryRefFunc)GetProcAddress(hm,"OGR_G_GetGeometryRef");
				funcs.ogrGetSpatialRef = (GetSpatialRefFunc)GetProcAddress(hm,"OGR_L_GetSpatialRef");
				funcs.exportToWkt = (ExportToWktFunc)GetProcAddress(hm,"_OSRExportToWkt@8");
				funcs.ogrGetFeatureCount = (GetFeatureCountFunc)GetProcAddress(hm,"OGR_L_GetFeatureCount");
				funcs.ogrGetLayerExtent = (GetLayerExtentFunc)GetProcAddress(hm,"OGR_L_GetExtent");
				funcs.ogrGetFieldName = (GetFieldNameFunc)GetProcAddress(hm,"OGR_Fld_GetNameRef");

			} 
		}
	}   

}

void GDALFormat::Init()
{
	if ( dataSet != NULL ) return; // already done
	ForeignFormat::Init();

	if ( !File::fExist(fnGetForeignFile()) )
		throw ErrorObject(String(TR("File %S \ncan not be opened. The file may be corrupt, incomplete or the format is not supported").c_str(), fnGetForeignFile().sFullPath()));

	LoadMethods();
	ILWISSingleLock lock(&m_CriticalSection, TRUE, SOURCE_LOCATION);
	funcs.registerAll();


	//GDALAllRegister();
	
	gdalDriver = funcs.identifyDriver(fnGetForeignFile().sFullPath().c_str(), NULL);
	if ( gdalDriver != 0 )
	{
		if (0 == trq)
			trq = new Tranquilizer();
		trq->SetDelayShow(false);
		trq->SetNoStopButton(true);
		trq->SetTitle(TR("GDAL"));
		trq->fText(String(TR("Loading file. Please wait").c_str(), fnGetForeignFile().sRelative()));
		dataSet = funcs.open(fnGetForeignFile().sFullPath().c_str(), GA_ReadOnly);
		if ( dataSet == NULL) //  if not, try  again but this time only read access
			dataSet = funcs.open(fnGetForeignFile().sFullPath().c_str(), GA_ReadOnly);
		if ( !dataSet )
			throw ErrorObject(String(TR("File %S \ncan not be opened. The file may be corrupt, incomplete or the format is not supported").c_str(), fnGetForeignFile().sFullPath()));
		delete trq;
		trq = 0;
	}
	else {
		const char *txt = funcs.errorMsg();
		if ( txt) {
			throw ErrorObject(String(txt));
		}else
			throw ErrorObject(TR("File format is not recognized by GDAL"));
	}
}

struct GGThreadData
{
	GGThreadData() : gg(NULL), fptr(NULL), fThreading(true) {}
	GDALFormat *gg;
	ForeignCollection* fptr;
	Directory dir;
	bool fThreading;
};

void GDALFormat::PutDataInCollection(ForeignCollectionPtr* col, ParmList& pm)
{
	bool *fDoNotLoadGDB = (bool *)(getEngine()->pGetThreadLocalVar(IlwisAppContext::tlvDONOTLOADGDB));	
	if ( *fDoNotLoadGDB == true )	
		return;

	GGThreadData *data = new GGThreadData;
	data->gg = this;
	data->fptr = new ForeignCollection(col->fnObj, ParmList());   // pass the complete object as pointer
	                                                  // needed to keep pointer in memory when passing it to thread
	data->dir = Directory(getEngine()->sGetCurDir());

	bool fUseThreading = true;
	if ( pm.fExist("threading")) {
		fUseThreading = pm.fGet("threading");
	}
	if ( fUseThreading)
		::AfxBeginThread(PutDataInThread, (VOID *)data);
	else {
		data->fThreading = false;
		PutDataInThread((VOID *)data);
	}
}

UINT GDALFormat::PutDataInThread(LPVOID lp)
{
	GGThreadData *data = NULL;
	FileName fnCol;
	try
	{
		data = (GGThreadData *)lp;
		fnCol = (*(data->fptr))->fnObj;
		if ( !data->fThreading)
			data->gg->fShowCollection = false; // scripting situation

		if ( data->fThreading)
			getEngine()->InitThreadLocalVars();
		getEngine()->SetCurDir(data->dir.sFullPath());
		
		data->gg->ReadForeignFormat(data->fptr->ptr());

		if ( data->fThreading)
			getEngine()->RemoveThreadLocalVars();
		if ( data->fThreading)
			delete data->fptr;  // remove the foreign collection (because passed as pointer)
		delete data;
		data = 0;
	}
	catch(ErrorObject&)
	{
		FileName *fn = new FileName(fnCol);								
		getEngine()->RemoveThreadLocalVars();					
		AfxGetApp()->GetMainWnd()->SendMessage(ILWM_CLOSECOLLECTION, (long)fn, 0);
		GGThreadData *data = (GGThreadData *)lp;
		if ( fnCol != FileName() )
		{
			String sCmd("del %S -quiet -force", fnCol.sFullPath());
			getEngine()->Execute(sCmd);
			AfxGetApp()->GetMainWnd()->PostMessage(ILW_READCATALOG, WP_RESUMEREADING, 0);		
		}
		if (data)
			delete data->fptr;
		delete data;		
	}
	return 1;
}

void GDALFormat::ReadForeignFormat(ForeignCollectionPtr* col)
{
	try
	{
		AfxGetApp()->GetMainWnd()->PostMessage(ILW_READCATALOG, WP_STOPREADING, 0);

		collection = col;
		Init();

		ILWISSingleLock lock(&m_CriticalSection, TRUE, SOURCE_LOCATION);	 		
		int iMaxChannels = funcs.count( dataSet );
		GDALRasterBandH iBitMapLayer = funcs.getBand(dataSet, 1);
		lock.Unlock();
		
		if ( iMaxChannels >= 1 || iBitMapLayer != 0)
		{
			CreateRasterLayer(iMaxChannels, iBitMapLayer != 0);
		}

		if ( collection->iNrObjects() == 0 )
			throw ErrorObject(TR("No layer recognized (not supported ?)"));
	
		AfxGetApp()->GetMainWnd()->PostMessage(ILW_READCATALOG, WP_RESUMEREADING, 0);
		collection->Store();
		AfxGetApp()->GetMainWnd()->PostMessage(ILW_READCATALOG, 0, 0);
		String sCommand("*open %S", col->fnObj.sRelativeQuoted());
		if ( fShowCollection)
			AfxGetApp()->GetMainWnd()->SendMessage(ILWM_EXECUTE, 0, (LPARAM)sCommand.c_str());				
		delete trq;
		trq = NULL;
	}
	catch(ErrorObject& err)
	{
		err.Show();
		if ( collection != 0 )
		{
			collection->DeleteEntireCollection();			
			collection->fErase = true;
		}
		CleanUp();
		throw err;
	}
}

void GDALFormat::GetRasterLayer(int iLayerIndex, Map& mp, Array<FileName>& arMaps, GeoRef& grf, 	Domain& dm,  bool fBitMap)
{
		LayerInfo li;
		String s("GDAL(%d)", iLayerIndex);
		if ( grf.fValid() ) // only one georef has to be created;
			li.grf = grf;
		// get csy, grf and domain info
		GetRasterInfo(li, iLayerIndex, fBitMap);
		String sPath =  fnGetForeignFile().sPath() != fnBaseOutputName.sPath() ? 
				            fnGetForeignFile().sFullPathQuoted() : fnGetForeignFile().sRelativeQuoted();
		li.sExpr = String("GDAL(%S, %d)", sPath, iLayerIndex); // bitmaps will have a negative index as they use the segment routines
		FileName fnM(fnBaseOutputName, ".mpr");
		ILWISSingleLock lock(&m_CriticalSection, TRUE, SOURCE_LOCATION);	 					
		int iMaxChannels = funcs.count( dataSet );
		lock.Unlock();
		
		FileName fnRasMap;
		if ( iMaxChannels > 1)
			fnRasMap = FileName(FileName::fnUniqueWithIndex(fnM));
		else
			fnRasMap = FileName(FileName::fnUnique(fnM));			

		li.fnObj = fnRasMap;

		bool fUseAs = !fImport;
		String sMethod = fUseAs ? ": use as" : ": import ";
		if (fUseAs) {
			mp = Map(fnRasMap, li);
			Store(mp, iLayerIndex);
		}
		else
			ImportRasterMap(li.fnObj, mp, li, iLayerIndex);

		// sets MinMax to undef. MinMax will be calculated when first showing
		mp->SetMinMax(RangeInt());
		mp->SetMinMax(RangeReal());
		mp->SetDescription(String(TR("Raster Map from %S, using GDAL, method %S").c_str(), fnGetForeignFile().sRelative(), sMethod));
		mp->Store();

		// check if the domains are equal, needed if a maplist is created
		if ( !dm.fValid() || mp->dm() == dm )
		{
			arMaps &= mp->fnObj;
			if ( !dm.fValid())
				dm = mp->dm();
		}

		collection->Add(mp);
		for( set<String>::iterator cur=AddedFiles.begin(); cur != AddedFiles.end(); ++cur)
			collection->Add(FileName(*cur));

		grf = li.grf;	
	
}
void GDALFormat::CreateRasterLayer(int iMaxChannels, bool fBitMap)
{
	GeoRef grf;
	Domain dm;
	Array<FileName> arMaps;
	Map mp;

	for(int iChannel = 1 ; iChannel <= iMaxChannels; ++iChannel)
	{
		try
		{
			currentLayer = 0; // reset this else it will continue with the first layer
			GetRasterLayer(iChannel, mp, arMaps, grf, dm, fBitMap);
		}
		catch(StopConversion&)
		{
			if (mp.fValid())  
			{
				mp->fErase = true;
				if (mp->gr().fValid())
				{
					mp->gr()->fErase = true;				
					if ( mp->gr()->cs().fValid() )
						mp->gr()->cs()->fErase = true;
				}				
			}				
			if (grf.fValid())
			{
				grf->fErase = true;				
				if ( grf->cs().fValid() )
					grf->cs()->fErase = true;
			}
			if (dm.fValid())  dm->fErase = true;
			if ( collection )
			{
				collection->DeleteEntireCollection();
				collection->fErase = true;
			}	
			return;
		}
	}
	// a maplist will be created if there are more than two raster channels and the domains are
	// compatible
	if ( dm.fValid() && arMaps.size() > 2 )
	{
		FileName fnM = FileName::fnUnique(FileName(fnBaseOutputName, ".mpl"));
		MapList mplst(fnM, arMaps);
		mplst->Store();
		collection->Add(mplst);
	}
	collection->Updated();
}

void GDALFormat::ImportRasterMap(const FileName& fnRasMap, Map& mp ,LayerInfo& li, int iChannel)
{
	if (0 == trq)
		trq = new Tranquilizer();
	trq->SetTitle(String(TR("Importing raster map %S").c_str(), fnGetForeignFile().sShortName()));

	iLayer = iChannel;
	mp = Map(li.fnObj, li.grf, li.grf->rcSize(), li.dvrsMap);
	for ( int iLine = 0; iLine < mp->iLines(); ++iLine)
	{
		if (trq->fUpdate(iLine, mp->iLines()))
			throw StopConversion();
		if ( mp->st() == stREAL )
		{
			RealBuf buf(mp->iCols());
			GetLineVal(iLine, buf, 0, mp->iCols());
			mp->PutLineVal(iLine, buf);
		}				
		else			
		{
			LongBuf buf(mp->iCols());
			GetLineRaw(iLine, buf, 0, mp->iCols());
			mp->PutLineRaw(iLine, buf);			
		}			
	}
}

GDALRasterBandH GDALFormat::OpenLayer(int iChannel)
{
	Init();

	ILWISSingleLock lock(&m_CriticalSection, TRUE, SOURCE_LOCATION);	 				
	if ( iChannel == iUNDEF )
		throw ErrorObject(TR("Opening an undefined GDAL layer"));

	GDALRasterBandH  gdalRasterBand = funcs.getBand( dataSet, iChannel);
	dataType = funcs.getDataType(gdalRasterBand);

	return gdalRasterBand;
}

CoordSystem GDALFormat::GetCoordSystem()
{
	String wkt(funcs.getProjectionRef(dataSet));
	return getEngine()->gdal->getCoordSystem(fnBaseOutputName,wkt);

	//char wkt[5000];
	//
	//strcpy(wkt, cwkt);
	//char *wkt2 = (char *)wkt;
	//OGRSpatialReferenceH handle = funcs.newSRS(NULL);

	////OGRErr err = funcs.fromEPSG(handle, 4326);

	//OGRErr err = funcs.srsImportFromWkt(handle, &wkt2);
	//if ( err == OGRERR_UNSUPPORTED_SRS )
	//	 	return CoordSystem("unknown");
	//return getCoordSystemFrom(handle, wkt);
}

CoordSystem GDALFormat::getCoordSystemFrom(OGRSpatialReferenceH handle, char *wkt) {


	String datumName(funcs.getAttr(handle,"Datum",0));
	//map<String, ProjectionConversionFunctions>::iterator where = mpCsyConvers.find(projectionName);

	FileName fnCsy(FileName::fnUnique(FileName(fnBaseOutputName, ".csy")));

	CoordSystemViaLatLon *csv=NULL;
	if ( funcs.isProjected(handle)) {
		try{
			CoordSystemProjection *csp =  new CoordSystemProjection(fnCsy, 1);
			String dn = Datum::WKTToILWISName(datumName);
			if ( dn == "" || dn == "?") {
				Projection proj = ProjectionPtr::WKTToILWISName(wkt);
				if ( proj.fValid() == false)
					throw ErrorObject("Projection can't be transformed to an ILWIS known datum");
				csp->prj = proj;
			} else {
				csp->datum = new MolodenskyDatum(dn,"");
			}
			String projName(funcs.getAttr(handle, "Projection",0));
			if ( projName == "Oblique_Stereographic")
				projName = "Stereographic";
			if ( projName == "Lambert_Conformal_Conic_2SP")
				projName = "Lambert Conformal Conic";
			replace(projName.begin(), projName.end(),'_',' ');
			OGRErr err;

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


			double easting  = getEngine()->gdal->getProjParam(handle, "false_easting",0,&err);
			double northing = getEngine()->gdal->getProjParam(handle, "false_northing",0,&err);
			double scale = getEngine()->gdal->getProjParam(handle, "scale_factor",0,&err);
			double centralMeridian = getEngine()->gdal->getProjParam(handle, "central_meridian",0,&err);
			double lattOfOrigin = getEngine()->gdal->getProjParam(handle, "latitude_of_origin",0,&err);
			csp->prj = Projection(projName);
			csp->prj->Param(pvX0,easting);
			csp->prj->Param(pvY0,northing);
			csp->prj->Param(pvK0, scale);
			csp->prj->Param(pvLON0, centralMeridian);
			csp->prj->Param(pvLAT0, lattOfOrigin);
			csv = csp;
		} catch ( ErrorObject& err) {
			return CoordSystem("unknown");
		}
	} else {
		csv = new CoordSystemLatLon(fnCsy, 1);
		csv->datum = new MolodenskyDatum("WGS 1984","");
	}

	CoordSystem csy;
	csy.SetPointer(csv);

	return csy;
}

bool GDALFormat::fReUseExisting(const FileName& fn)
{
	// if it does not exist it can be used (the name)
	if ( !fn.fExist() ) return false;
	int iNr;
	ObjectInfo::ReadElement("Collection", "NrOfItems", fn, iNr);
	if ( iNr == iUNDEF ) return false;
	FileName fnForeignCollection(fnGetForeignFile(), ".ioc");
	//the domain must be used already within this collection, if it is it may be reused
	for(int i=0; i<iNr; ++i)
	{
		String sItem("Item%d", i);
		FileName fnCollection;
		ObjectInfo::ReadElement("Collection", sItem.c_str(), fn, fnCollection);
		if ( fnForeignCollection == fnCollection )
			return true;
	}
	return true;	
}


// main routine for reading a vector layer. It construct all data objects (tables. csy)
void GDALFormat::IterateLayer(vector<LayerInfo>& objects, bool fCreate)
{
	bool *fDoNotLoadGDB = (bool *)(getEngine()->pGetThreadLocalVar(IlwisAppContext::tlvDONOTLOADGDB));	
	if ( *fDoNotLoadGDB == true )	
		return;
	if ( mtLoadType != mtUnknown) // checking if datafile has changed
	{
		//FileName fn(fnBaseOutputName.sShortName(false), sExt(mtLoadType));
		if (  mtLoadType >= objects.size())
			return;

		if (ForeignCollection::fForeignFileTimeChanged(fnGetForeignFile(), objects[mtLoadType].fnObj) )
		{
			fCreate = true;
			mtLoadType = mtUnknown; // everything has to be processed
		}
		else
			fCreate = false;

	}

	Init();

}

//void GDALFormat::CreateBasicLayer(vector<LayerInfo>& objects)
//{

	//for(int i=0; i<ForeignFormat::mtUnknown; i++)
	//{
	//	mtMapType mtType = (mtMapType)i;
	//	if ( objects[i].iShapes > 0 )
	//	{
	//		objects[i].fnObj = fnCreateFileName(FileName(fnBaseOutputName.sShortName(false), sExt(mtType)));
	//		objects[i].fnForeign = fnForeign; 
	//		objects[i].dvrsMap = Domain(objects[i].fnObj, objects[i].iShapes, dmtUNIQUEID, sType(mtType));
	//		if ( objects[i].cbMap.fContains( objects[i].cbActual) )
	//			objects[i].cbMap = objects[i].cbActual;
	//		objects[i].csy = GetCoordSystem();
	//		objects[i].csy->cb = objects[i].cbMap;

	//		String sPath = fnForeign.sDrive != fnBaseOutputName.sDrive ? fnForeign.sFullPathQuoted() : fnForeign.sRelativeQuoted(); 
	//		if ( fImport)
	//			objects[i].sExpr = "import";
	//		else if ( fCombine )
	//			objects[i].sExpr = String("GeoGateway(%S, combine)", sPath);
	//		else
	//			objects[i].sExpr = String("GeoGateway(%S,%d)", sPath, iLayer);

	//	}
	//}
//}

FileName GDALFormat::fnCreateFileName(const FileName& fnBase)
{
	int iCount = 1;
	FileName fnNewFile = fnBase;
	bool fFound = true;
	while ( fFound )		
	{
		fFound = find(fnUsedFileNames.begin(), fnUsedFileNames.end(), fnNewFile ) != fnUsedFileNames.end();
		fFound |= fnNewFile.fExist();
		fFound |= ObjectInfo::fSystemObject(fnNewFile);
		if ( fFound )
		{
			fnNewFile.sFile = String("%S_%d", fnBase.sFile, iCount++);
		}
	}
	fnUsedFileNames.push_back(fnNewFile);
	
	return fnNewFile;
}

//-- rasters
void GDALFormat::GetRasterInfo(LayerInfo& inf, int iChannel, bool fBitMap)
{
	if (!currentLayer )
		currentLayer = OpenLayer(iChannel);


	GetGeoRef(inf.grf);
	ILWISSingleLock lock(&m_CriticalSection, TRUE, SOURCE_LOCATION);			
	int type = funcs.getDataType(currentLayer);
    switch ( type )
		{
			case GDT_Byte:
				inf.dvrsMap = DomainValueRangeStruct(Domain("image"));
				break;
			case GDT_Int16:
				inf.dvrsMap = DomainValueRangeStruct(-SHRT_MAX + 2, SHRT_MAX -2 );
				break;
			case GDT_UInt16:
				inf.dvrsMap = DomainValueRangeStruct(0, SHRT_MAX-1);
				break;
			case GDT_UInt32:
				inf.dvrsMap = DomainValueRangeStruct(0, LONG_MAX -2 );
				break;
			case GDT_Int32:
				inf.dvrsMap = DomainValueRangeStruct(-LONG_MAX+2, LONG_MAX -2 );
				break;
			case GDT_Float64:
			case GDT_Float32:

				inf.dvrsMap = DomainValueRangeStruct(-1e100, 1e100, 0.0);;
				break;
			default:
				throw ErrorObject(String("Unknown data type : %d",type));
		}
	lock.Unlock();
}
void GDALFormat::GetGeoRef(GeoRef& grf)
{
	if ( grf.fValid() ) return; // all channels have the same georef, this one was set by previous run
	ILWISSingleLock lock(&m_CriticalSection, TRUE, SOURCE_LOCATION);	
	RowCol rcSize(funcs.ySize( dataSet ), funcs.xSize( dataSet ));
	double geosys[6];
	CPLErr ret = funcs.geotransform(dataSet, geosys) ;
	if ( ret == 0 )
	{	
		lock.Unlock();
		double a1, a2, b1, b2;
		a1 = geosys[0];
		b1 = geosys[3];
		a2 = geosys[1];
		b2 = geosys[5];
		FileName fnG(fnBaseOutputName, ".grf");
		FileName fnGeo(FileName::fnUnique(fnG));
		Coord crdLeftup( a1 , b1);
		Coord crdRightDown(a1 + rcSize.Col * a2, b1 + rcSize.Row * b2 ) ;
		Coord cMin( min(crdLeftup.x, crdRightDown.x), min(crdLeftup.y, crdRightDown.y));
		Coord cMax( max(crdLeftup.x, crdRightDown.x), max(crdLeftup.y, crdRightDown.y));
		CoordSystem cs = GetCoordSystem();
		if ( cs->pcsLatLon() && abs(cMin.y) > 360 && cs != CoordSystem("Unknown"))
		{
			cs->fErase = true;
			collection->Remove(cs->fnObj);
			cs = CoordSystem("Unknown");
		} else {
			AddNewFiles(cs->fnObj);
		}
		grf.SetPointer(new GeoRefCorners(fnGeo, cs, rcSize, true, cMin, cMax)); 
		grf->SetDescription(String(TR("GeoReference from %S, using GDAL").c_str(), fnGetForeignFile().sRelative()));
		grf->cs()->cb = CoordBounds(cMin, cMax);
		cs->cb = grf->cs()->cb;
		cs->Updated();
		grf->cs()->Updated();
	}
	else
	{
		grf = GeoRef(rcSize);
		CoordSystem cs = GetCoordSystem();
		grf->SetCoordSystem(cs);
	}
	grf->Store();
	//AddedFiles.insert(grf->fnObj.sFullPathQuoted());
	AddNewFiles(grf->fnObj);

}

#define iRAW0 0L
#define iRAW1 1L

void GDALFormat::GetBitValues(unsigned char * buffer, LongBuf& buf, long iNum) const
{
	for( int iX = 0; iX < iNum; iX++ )
	{
     if( buffer[ iX >> 3] & (0x80 >> (iX & 0x7)) )
        buf[iX] = iRAW1;
    else
        buf[iX] = iRAW0;
	}				
}
	
void GDALFormat::GetLineRaw(long iLine, ByteBuf& buf, long iFrom, long iNum) const
{
	ILWISSingleLock lock(&m_CriticalSection, TRUE, SOURCE_LOCATION);			
	if ( buffer == NULL)
		(const_cast<GDALFormat *>(this))->CreateLineBuffer(iNum);

	funcs.rasterIO(currentLayer, GF_Read, iFrom, iLine,iNum,1,(unsigned char *)buffer, iNum, 1, GDT_Byte,0,0);								
	for( int i = 0; i< iNum; ++i)
		buf[i] = buffer[i];		
}

void GDALFormat::GetLineRaw(long iLine, IntBuf& buf, long iFrom, long iNum) const
{
	ILWISSingleLock lock(&m_CriticalSection, TRUE, SOURCE_LOCATION);				
	if ( buffer == NULL)
		(const_cast<GDALFormat *>(this))->CreateLineBuffer(iNum);

	funcs.rasterIO(currentLayer, GF_Read, iFrom, iLine,iNum,1,(unsigned char *)buffer, iNum, 1, GDT_Int16, 0,0);								

	for( int i = 0; i< iNum; ++i)
		buf[i] = ((short *)buffer)[i * 2];
}

void GDALFormat::GetLineRaw(long iLine, LongBuf& buf, long iFrom, long iNum) const
{
	ILWISSingleLock lock(&m_CriticalSection, TRUE, SOURCE_LOCATION);				
	if ( buffer == NULL)
		(const_cast<GDALFormat *>(this))->CreateLineBuffer(iNum);

	CPLErr ret = funcs.rasterIO (currentLayer, GF_Read, iFrom, iLine,iNum, 1, buffer, iNum, 1, dataType,0, 0);								

	int i = 0;
	switch(dataType)
	{
		case GDT_Int16:
			for( i = 0; i< iNum; ++i)
				buf[i] = ((short *)buffer)[i];
			break;
		case GDT_UInt16:
		{
			//DomainValueRangeStruct dvrs(0, USHRT_MAX - 2);			
			unsigned short n;
			for( i = 0; i< iNum; ++i)
			{
				n = (((unsigned short *)buffer)[i]);
				buf[i] = n;
			}
			break;
		}
		case GDT_UInt32:
			for( i = 0; i< iNum; ++i)
				buf[i] = ((unsigned int *)buffer)[i];
			break;
		break;
		case GDT_Int32:
			for( i = 0; i< iNum; ++i)
				buf[i] = ((int *)buffer)[i];
			break;
		break;
		case GDT_Byte:
			for( i = 0; i< iNum; ++i)
				buf[i] = buffer[i];
			break;
		case GDT_Float64:
			for( i = 0; i< iNum; ++i)				
 				buf[i] = ((double *)buffer)[i];
			break;
	};
}

void GDALFormat::GetLineVal(long iLine, RealBuf& buf, long iFrom, long iNum) const
{
	ILWISSingleLock lock(&m_CriticalSection, TRUE, SOURCE_LOCATION);				
	if ( buffer == NULL)
		(const_cast<GDALFormat *>(this))->CreateLineBuffer(iNum);

	int gdalDataType = funcs.getDataType(dataSet);
	funcs.rasterIO(currentLayer, GF_Read, iFrom, iLine,iNum,1, (unsigned char *)buffer, iNum, 1, dataType, 0,0);								

	int i = 0;
	switch(dataType)
	{
		case GDT_Int16:
			for( i = 0; i< iNum; ++i)
				buf[i] = ((short *)buffer)[i];
			break;
		case GDT_UInt16:
		{
			for( i = 0; i< iNum; ++i)
			{
				buf[i] = (((unsigned short *)buffer)[i]); 
			}
		}			
		break;
		case GDT_UInt32:
			for( i = 0; i< iNum; ++i)
				buf[i] = ((unsigned int *)buffer)[i];
			break;
		break;
		case GDT_Int32:
			for( i = 0; i< iNum; ++i)
				buf[i] = ((int *)buffer)[i];
			break;
		break;
		case GDT_Byte:
			for( i = 0; i< iNum; ++i)
				buf[i] = buffer[i];
			break;
		case GDT_Float32:
			for( i = 0; i< iNum; ++i)				
 				buf[i] = ((float *)buffer)[i];
			break;
		case GDT_Float64:
			for( i = 0; i< iNum; ++i)				
 				buf[i] = ((double *)buffer)[i];
			break;
	};
}

void GDALFormat::GetLineVal(long iLine, LongBuf& buf, long iFrom, long iNum) const
{
	ILWISSingleLock lock(&m_CriticalSection, TRUE, SOURCE_LOCATION);				
	GetLineRaw(iLine, buf, iFrom, iNum);
}

void GDALFormat::CreateLineBuffer(long iSize)
{
	if(!currentLayer)
		currentLayer = OpenLayer(iLayer);
	int iDataSize = funcs.dataSize(dataType);
	buffer = new unsigned char [ iSize * iDataSize/8 ];
	memset(buffer, 0, iSize * iDataSize/8);
	f4BytesInt = dataType == GDT_Int16 || dataType == GDT_UInt16 || dataType == GDT_Float32 || dataType == GDT_UInt32 || dataType == GDT_Int32;		
}

long GDALFormat::iRaw(RowCol rc) const
{
	ILWISSingleLock lock(&m_CriticalSection, TRUE, SOURCE_LOCATION);			
	if ( buffer == NULL )
	{
		(const_cast<GDALFormat *>(this))->CreateLineBuffer(1);
	}
	funcs.rasterIO(currentLayer, GF_Read, rc.Col, rc.Row, 1,1, (unsigned char *)buffer, 1, 1, GDT_Int32, 0,0);								
	switch (funcs.getDataType(currentLayer))
	{
		case GDT_Byte:
				return buffer[0];
				break;
		case GDT_Int16:
				return (((int *)buffer)[0]);
				break;
		case GDT_UInt16:
			return (long)(((unsigned int *)buffer)[0]);
			break;
		case GDT_Int32:
			return ((long *)buffer)[0];
			break;
		case GDT_UInt32:
			return ((unsigned int *)buffer)[0];
			break;

	};
	return iUNDEF;
}

long GDALFormat::iValue(RowCol rc) const
{
	return iRaw(rc);
}

double GDALFormat::rValue(RowCol rc) const
{
	ILWISSingleLock lock(&m_CriticalSection, TRUE, SOURCE_LOCATION);					
	if ( buffer == NULL )
	{
		int iLineSize = funcs.ySize( dataSet );
		(const_cast<GDALFormat *>(this))->CreateLineBuffer(iLineSize);
	}
	funcs.rasterIO(currentLayer, GF_Read, rc.Col, rc.Row,1,1,(unsigned char *)buffer, 1, 1, GDT_Float64, 0,0);								
	double rVal =  (double)((float *)buffer)[0];
	return rVal;
}

void GDALFormat::AddNewFiles(const FileName& fnNew)
{
	if (0 == trq)
		trq = new Tranquilizer();
	trq->SetDelayShow(false);	
	trq->fText(String(TR("Adding File %S").c_str(), fnNew.sRelative()));	
	AddedFiles.insert(fnNew.sFullPath().c_str());	
}

void GDALFormat::PutLineVal(const FileName& fnMap, long iLine, const RealBuf& buf, long iFrom, long iNum) 
{
	ILWISSingleLock lock(&m_CriticalSection, TRUE, SOURCE_LOCATION);				
	if ( buffer == NULL)
		(const_cast<GDALFormat *>(this))->CreateLineBuffer(iNum);

	int iChannel = iLayer;
	GDALDataType gdalDataType   ;
 	int iBands;

	Map mp(fnMap);

	DetermineOutputType(fnMap, iBands, gdalDataType);

	for( int i = 0; i< iNum; ++i)
	{
	 ((double *)buffer)[i] = (double )(buf[i]);
	}		 

	funcs.rasterIO(currentLayer, GF_Write, iFrom,iLine, iNum,1,(double*)buffer, iNum, 1 , GDT_Float64,0,0);	
}

void GDALFormat::PutLineVal(const FileName& fnMap,long iLine, const LongBuf& buf, long iFrom, long iNum) 
{
	ILWISSingleLock lock(&m_CriticalSection, TRUE, SOURCE_LOCATION);				
	if ( buffer == NULL)
		(const_cast<GDALFormat *>(this))->CreateLineBuffer(iNum);

	GDALDataType gdalDataType;
	int iBands;
	int iChannel = iLayer;

	Map mp(fnMap);
	DetermineOutputType(fnMap, iBands, gdalDataType);
	int i=0;

	switch(gdalDataType)
	{
		case GDT_Int16:
			for( i = 0; i< iNum; ++i)
				((short *)buffer)[i] = (short)buf[i] ;
			break;
		case GDT_UInt16 :
			for( i = 0; i< iNum; ++i)
				((unsigned int *)buffer)[i] = (unsigned int)(buf[i]);
			break;
		case GDT_UInt32:
			for( i = 0; i< iNum; ++i)
				((unsigned int *)buffer)[i] = (unsigned int)(buf[i]);
		case GDT_Int32:
			for( i = 0; i< iNum; ++i)
				((int *)buffer)[i] = (int)(buf[i]);

    case GDT_Byte:
			for( i = 0; i< iNum; ++i)
				buffer[i] = buf[i];
			break;
	};	

	if ( !f4BytesInt)
	  funcs.rasterIO(currentLayer, GF_Write, iFrom,iLine, iNum, 1,(unsigned char *)buffer,iNum, 1 , GDT_Int32,0,0);
	else
	  funcs.rasterIO(currentLayer, GF_Write, iFrom,iLine, iNum, 1,(int *)buffer, iNum, 1 , GDT_Int32,0,0);
}

void GDALFormat::PutLineRaw(const FileName& fnMap, long iLine, const LongBuf& buf, long iFrom, long iNum) 
{
	ILWISSingleLock lock(&m_CriticalSection, TRUE, SOURCE_LOCATION);			
	if ( buffer == NULL)
		(const_cast<GDALFormat *>(this))->CreateLineBuffer(iNum);

	Map mp(fnMap);
	RangeInt ri = mp->dvrs().riMinMax();
	short iUnd = ri.iHi() + 1;
	int iChannel = iLayer;
	
  int i=0;
  GDALDataType gdalDataType = funcs.getDataType(dataSet);
	switch(gdalDataType)
	{
		case GDT_Int16:
		case GDT_UInt16:
		{
			DomainSort *pdsrt = mp->dm()->pdsrt();
			double rStretch = 1.0;
			for( i = 0; i< iNum; ++i)
			{
				if ( gdalDataType == GDT_Int16)
				{
					if ( mp->dvrs().fValues())				
						((short *)buffer)[i] = (short)(mp->dvrs().iValue(buf[i]));
					else
						((short *)buffer)[i] = (short)( buf[i] != iUNDEF ? buf[i] : iUnd );					
				}
				else
				{
					if ( mp->dvrs().fValues())				
						((unsigned int *)buffer)[i ] = (unsigned short)(mp->dvrs().iValue(buf[i]));
					else
						((unsigned int *)buffer)[i ] = (unsigned short)(buf[i]);							
				}					
					
			}			
		}
		case GDT_Int32:
		case GDT_UInt32:
		{
			DomainSort *pdsrt = mp->dm()->pdsrt();
			double rStretch = 1.0;
			for( i = 0; i< iNum; ++i)
			{
				if ( gdalDataType == GDT_Int32)
				{
					if ( mp->dvrs().fValues())				
						((long *)buffer)[i] = (long)(mp->dvrs().iValue(buf[i]));
					else
						((long *)buffer)[i] = (long)( buf[i] != iUNDEF ? buf[i] : iUnd );					
				}
				else
				{
					if ( mp->dvrs().fValues())				
						((unsigned int *)buffer)[i] = (unsigned int)(mp->dvrs().iValue(buf[i]));
					else
						((unsigned int *)buffer)[i ] = (unsigned int)(buf[i]);							
				}					
					
			}			
		}		
		break;
    case GDT_Byte:
		{
			DomainSort *pdsrt = mp->dm()->pdsrt();
			double rStretch = 1.0;
			if ( pdsrt )
			{
				rStretch = 255.0 / pdsrt->iSize();
			}					
			for( i = 0; i< iNum; ++i)
			{
				if ( mp->dvrs().fValues())
					buffer[i] = mp->dvrs().iValue(buf[i]);
				else
				{
					buffer[i] = min(255, buf[i] * rStretch);
				}					
			}				
		}			
		break;
	};

	
	if ( !f4BytesInt)
	  funcs.rasterIO(currentLayer, GF_Write, iFrom,iLine, iNum,1,(unsigned char *)buffer, iNum, 1 , GDT_Int32,0,0);
	else
	  funcs.rasterIO(currentLayer, GF_Write, iFrom,iLine, iNum, 1,(int *)buffer, iNum, 1 , GDT_Int32,0,0);
}

void GDALFormat::PutLineRaw(const FileName& fnMap, long iLine, const IntBuf& buf, long iFrom, long iNum) 
{
	ILWISSingleLock lock(&m_CriticalSection, TRUE, SOURCE_LOCATION);				
	if ( buffer == NULL)
		(const_cast<GDALFormat *>(this))->CreateLineBuffer(iNum);

	Map mp(fnMap);
	for( int i = 0; i< iNum; ++i)
		((short *)buffer)[i] = (short)(mp->dvrs().iValue(buf[i]));

	int iChannel = iLayer;
    funcs.rasterIO(currentLayer, GF_Write, iFrom,iLine, iNum, 1,(int *)buffer, iNum, 1 , GDT_Int32,0,0);
}

void GDALFormat::PutLineRaw(const FileName& fnMap, long iLine, const ByteBuf& buf, long iFrom, long iNum) 
{
	ILWISSingleLock lock(&m_CriticalSection, TRUE, SOURCE_LOCATION);				
	if ( buffer == NULL)
		(const_cast<GDALFormat *>(this))->CreateLineBuffer(iNum);

	int iChannel = iLayer;
	Map mp(fnMap);
	
	for( int i = 0; i< iNum; ++i)
		buffer[i] = mp->dvrs().iValue(buf[i]);	

    funcs.rasterIO(currentLayer, GF_Write, iFrom,iLine, iNum,1, buffer, iNum, 1 , GDT_Byte,0,0);

}

void GDALFormat::SetForeignGeoTransformations(const CoordSystem &cs, const GeoRef& grf)
{
	ILWISSingleLock lock(&m_CriticalSection, TRUE, SOURCE_LOCATION);				
	double a1=0.0, a2=0.0, b1=1.0, b2=1.0;
	char geosys[17];
	memset( geosys, 0, 17);

	if (grf.fValid() && !grf->fGeoRefNone())
	{
		CoordBounds cb = grf->cb();
		RowCol rcSize = grf->rcSize();
		a1 = cb.cMin.x;
		b1 = cb.cMax.y;
		b2 = - cb.height() / rcSize.Row;
		a2 = cb.width() / rcSize.Col;
	}						
}

int GDALFormat::iFormatID(const String& sFormat) const
{
	for(map<int, FormatInfo>::const_iterator cur = mapFormatInfo.begin(); cur != mapFormatInfo.end(); ++cur)
	{
		String sF = (*cur).second.sFormatName;
		if ( fCIStrEqual( sF, sFormat) )
			return (*cur).first;
	}	
	return iUNDEF;
}

FormatInfo	GDALFormat::fiGetFormatInfo(const String sExt) const
{
	for(map<int, FormatInfo>::const_iterator cur = mapFormatInfo.begin(); cur != mapFormatInfo.end(); ++cur)
	{
		String sE = (*cur).second.sExt;
		if ( fCIStrEqual( sE, sExt) )	
		{
			return (*cur).second;
		}			
	}		

	return FormatInfo();
}

bool GDALFormat::fIsSupported(const FileName& fn, ForeignFormat::Capability cbType) const
{
	if ( cbType == cbEDIT)
		return funcs.access(dataSet) == GA_Update;

	return true;
}

bool GDALFormat::fIsCollection(const FileName& fnForeignObject) const 
{ 
	return true; 
}

void GDALFormat::ReadParameters(const FileName& fnObj, ParmList& pm)
{
	ForeignFormat::ReadParameters(fnObj, pm);
	String sV;
	if ( !pm.fExist("method")) {
		ObjectInfo::ReadElement("ForeignFormat","Method",fnObj,sV);
		pm.Add(new Parm("method", sV));
	}
	if ( !pm.fExist("layer")) {
		sV = "";
		ObjectInfo::ReadElement("ForeignFormat","Layer",fnObj, sV);
		pm.Add( new Parm("layer",sV));
	}																		   
	iLayer = pm.sGet("layer").iVal();
}

void GDALFormat::Store(IlwisObject ob, int iLayerIndex) {
	ForeignFormat::Store(ob);
	ob->WriteElement("ForeignFormat","Method","GDAL");
	ob->WriteElement("ForeignFormat","Layer",iLayerIndex);
}

bool GDALFormat::fMatchType(const String& , const String& sType)
{
	// Templates object type name
	return sType == "ILWIS ForeignCollections";
}

void GDALFormat::getImportFormats(vector<ImportFormat>& formats) {
	for(int i = 0; i < funcs.getDriverCount(); ++i) {
		GDALDriverH driv = funcs.getDriver(i);
		if ( driv) {
			String drivName("%s", funcs.getDriverLongName(driv));
			String shrtName("%s", funcs.getDriverShortName(driv));
			GDALImportFormat frm;
			frm.name = drivName;
			frm.shortName = shrtName;
			frm.type = ImportFormat::ifRaster;
			const char *cext = funcs.getMetaDataItem(driv,GDAL_DMD_EXTENSION,NULL);
			String ext("%s", cext != NULL ? cext : "*");
			frm.ext = ext;
			frm.provider = frm.method = "GDAL";
			frm.useasSuported = true;
			frm.ui = NULL;
			formats.push_back(frm);
		}
	}
	for(int i = 0; i < funcs.ogrGetDriverCount(); ++i) {
		OGRSFDriverH driv = funcs.ogrGetDriver(i);
		if ( driv) {
			String name("%s", funcs.ogrGetDriverName(driv));
			GDALImportFormat frm;
			frm.name = name;
			frm.type = ImportFormat::ifSegment | ImportFormat::ifPoint | ImportFormat::ifPolygon;
			frm.provider = frm.method = "OGR";
			frm.useasSuported = false;
			frm.ui = NULL;
			frm.ext = "*.*";
			if ( name == "ESRI Shapefile")
				frm.ext = "shp";
			frm.command["import"] = "gdalogrimport";
			formats.push_back(frm);
		}
	}
	sort(formats.begin(), formats.end());
}

void GDALFormat::ogr(const String& name, const String& source, const String& target){
	OGRDataSourceH hDS = funcs.ogrOpen( source.sUnQuote().c_str(), FALSE, NULL );	
	//OGRDataSourceH hDS = funcs.ogrOpen( "WFS:http://www.cartociudad.es/wfs-vial/services", FALSE, NULL );	
	if ( hDS) {
		String error;
		int layerCount = funcs.ogrGetLayerCount(hDS);
		for(int layer = 0; layer < layerCount ; ++layer) {
			OGRLayerH hLayer = funcs.ogrGetLayer(hDS, layer);
			if ( hLayer) {
				Feature::FeatureType ftype = getFeatureType(hLayer);
				if ( ftype == Feature::ftUNKNOWN) {
					error += String("layer %d", layer);
					continue;
				}
				Tranquilizer trq;
				trq.SetText(String(TR("Importing %S").c_str(), name));

				fnBaseOutputName = createFileName(target,ftype,layerCount, layer);

				OGRSpatialReferenceH hSRS = funcs.ogrGetSpatialRef(hLayer);
				CoordSystem csy;
				if ( hSRS != 0) {
					char * wkt[30000];
					char *wkt2 = (char *)wkt;
					funcs.exportToWkt(hSRS,&wkt2);
					csy = getCoordSystemFrom(hSRS, wkt2);
					} else
					csy = CoordSystem("unknown");

				int featureCount = funcs.ogrGetFeatureCount(hLayer,TRUE);
				Domain dm(fnBaseOutputName, featureCount, dmtID, "feature");
				CoordBounds cb = getLayerCoordBounds(hLayer);
				csy->cb = cb;
				BaseMap bmp = createBaseMap(fnBaseOutputName, ftype, dm, csy, cb);
				bmp->fErase = true;

				GeometryFiller *filler = 0;
				if ( ftype ==  Feature::ftPOINT)
					filler = new PointFiller(funcs, bmp);
				else if ( ftype == Feature::ftSEGMENT)
					filler = new SegmentFiller(funcs, bmp);
				else if ( ftype == Feature::ftPOLYGON)
					filler = new PolygonFiller(funcs, bmp);
				
				if ( bmp.fValid()) {
					OGRFeatureDefnH hFeatureDef = funcs.ogrGetLayerDefintion(hLayer);
					if ( hFeatureDef) {
						Table tbl;
						createTable(fnBaseOutputName, dm, hFeatureDef, hLayer, tbl);
						if ( tbl.fValid()) {
							bmp->SetAttributeTable(tbl);
						}
						OGRFeatureH hFeature;
						int rec = 1;
						funcs.ogrResetReading(hLayer);
						trq.Start();
						while( (hFeature = funcs.ogrGetNextFeature(hLayer)) != NULL ){
								OGRGeometryH hGeometry = funcs.ogrGetGeometryRef(hFeature);
								filler->fillFeature(hGeometry, rec);

								if ( trq.fUpdate(rec, featureCount)) { 
									delete filler;
									return;
								}
						}
						bmp->Store();
						bmp->fErase = false;
					}
				}
				delete filler;
			}
		}
		if ( error.size() != 0) {
			throw ErrorObject(String(TR("Errors in %S").c_str(), error));
		}
		
	}

}

struct ScannedColumn {
	ScannedColumn() { useClass = true; min=1e308; max=-1e308; }
	String name;
	vector<String> strings;
	vector<double> values;
	double min;
	double max;
	bool useClass;
	OGRFieldType type;

};
void GDALFormat::createTable(const FileName& fn, const Domain& dm,OGRFeatureDefnH hFeatureDef, OGRLayerH hLayer, Table& tbl) {
	tbl = Table(FileName(fn, ".tbt"),dm);
	int columnCount = funcs.ogrGetFieldCount(hFeatureDef);
	vector<ScannedColumn> columns(columnCount);
	OGRFeatureH hFeature;
	funcs.ogrResetReading(hLayer);
	int size = 0;
	Tranquilizer trq;
	int sz = dm->pdsrt()->iSize();
	trq.SetText(String(TR("Importing table %S").c_str(), fn.sFile));
	trq.Start();
	while( (hFeature = funcs.ogrGetNextFeature(hLayer)) != NULL ){
		if ( trq.fUpdate(size, sz)) {
			tbl = Table();
			return ;
		}

		for(int field=0; field < columnCount; ++field) {
			OGRFieldDefnH hFieldDefn = funcs.ogrGetFieldDefinition(hFeatureDef, field);
			OGRFieldType type = funcs.ogrGetFieldType(hFieldDefn);
			String name("%s", funcs.ogrGetFieldName(hFieldDefn));
			columns[field].name = name;
			columns[field].type = type;

			if ( type ==  OFTInteger || type ==  OFTReal) {
				double v = funcs.ogrrVal(hFeature, field);
				columns[field].min = min(v,columns[field].min); 
				columns[field].max = max(v,columns[field].max); 
				columns[field].values.push_back(v);
			} else if ( type ==  OFTString) {
				String v("%s",funcs.ogrsVal(hFeature, field));
				columns[field].strings.push_back(v);

				columns[field].useClass &= v.find(" ") != string::npos ;
				if ( v.size() > 0)
					columns[field].useClass &= !isdigit((unsigned char)(v[0])) ;
			} else if ( type == OFTDate || type == OFTDateTime) {
				int year, month, day,hour,minute, second;
					int r = funcs.ogrtVal(hFeature, field,&year, &month, &day, &hour,&minute, &second,0);
					if ( r == 1) {
						ILWIS::Time t(year, month, day, hour, minute, second);
						columns[field].min = min((double)t,columns[field].min); 
						columns[field].max = max((double)t,columns[field].max);
						columns[field].values.push_back((double)t);
					}
			}

		}
		++size;
	}
	for(int column = 0; column < columnCount; ++column) {
		OGRFieldType type = columns[column].type;
		Column col;
		if ( type ==  OFTInteger) {
			DomainValueRangeStruct dvrs(Domain("value"),ValueRangeInt((long)columns[column].min, (long)columns[column].max));
			col = tbl->colNew(columns[column].name, dvrs);
		} else if (type ==  OFTReal) {
			double rStep = 0; // = rRound(abs(columns[column].max - columns[column].min) * 0.0001); // rounds too much, e.g. when min = 100, max = 100000000
			DomainValueRangeStruct dvrs(Domain("value"),ValueRangeReal(columns[column].min, columns[column].max,rStep));
			col = tbl->colNew(columns[column].name, dvrs);
		} else if (type ==  OFTString) {
			Domain dom("String");
			if ( columns[column].useClass) {
				dom = createSortDomain(columns[column].name, columns[column].strings);
				dom->Store();
			}
			col = tbl->colNew(columns[column].name, dom);
		} else if ( type == OFTDate || type == OFTDateTime) {
			DomainValueRangeStruct dvrs(Domain("time"),ValueRangeReal(columns[column].min, columns[column].max,0));
			col = tbl->colNew(columns[column].name, dvrs);
		}
		if ( col.fValid()) {
			for(int rec = 0; rec < size; ++rec) {
				if ( type != OFTString) {
					if ( columns[column].values.size() == 0)
						continue;
					col->PutVal(rec + 1,columns[column].values[rec]);
				} else {
					if ( columns[column].strings.size() == 0)
						continue;
					col->PutVal(rec + 1, columns[column].strings[rec]);
				}
			}
		}
	}
}

Domain GDALFormat::createSortDomain(const String& name, const vector<String>& values) {
	set<String> names;
	Domain dom;
	for(int i = 0; i < values.size(); ++i) {
		names.insert(values[i]);
	}
	FileName fn(FileName::fnUnique(name),".dom");
	if ( names.size() < 100) {
		dom = Domain(fn, names.size(),dmtCLASS);
	} else {
		dom = Domain(fn, names.size(), dmtID);
	}
	
	for(set<String>::const_iterator cur = names.begin(); cur != names.end(); ++cur) {
		dom->pdsrt()->iAdd((*cur), true);
	}

	return dom;
}


Feature::FeatureType GDALFormat::getFeatureType(OGRLayerH hLayer) const{

	OGRwkbGeometryType type = funcs.ogrGetLayerGeometryType(hLayer);
	if ( type == wkbPoint || type == wkbMultiPoint || type == wkbPoint25D || type == wkbMultiPoint25D)
		return Feature::ftPOINT;

	if ( type == wkbPolygon || type == wkbMultiPolygon || type == wkbPolygon25D || type == wkbMultiPolygon25D)
		return Feature::ftPOLYGON;

	if ( type == wkbLineString || type == wkbMultiLineString || type == wkbLineString25D || type == wkbMultiLineString25D)
		return Feature::ftSEGMENT;

	return Feature::ftUNKNOWN;
}

FileName GDALFormat::createFileName( const String& name, Feature::FeatureType ftype, int layerCount, int layer) {
	//String transformedName;
	//FileName fnTemp(name);
	//String nameTemp = fnTemp.
	//for(int i=0; i< name.size(); ++i) {
	//	char lastChar = name[i];
	//	if ( lastChar == '.' || lastChar==':' || lastChar == '/' || lastChar == '\\' || lastChar ==' ' || lastChar=='-')
	//		transformedName += '_';
	//	else
	//		transformedName += lastChar;
	//}
	FileName fn( name);
	if ( ftype == Feature::ftPOINT)
		fn.sExt = ".mpp";
	if ( ftype == Feature::ftPOLYGON)
		fn.sExt = ".mpa";
	if ( ftype == Feature::ftSEGMENT)
		fn.sExt = ".mps";
	if ( layerCount == 1)
		return fn;
	fn.sFile += String("_%d", layer);

	return fn;
}

BaseMap GDALFormat::createBaseMap(const FileName& fn, Feature::FeatureType ftype, const Domain& dm, const CoordSystem& csy, const CoordBounds& cb) {
	if ( ftype == Feature::ftPOINT)
		return PointMap(fn,csy, cb, DomainValueRangeStruct(dm));
	if ( ftype == Feature::ftPOLYGON)
		return PolygonMap(fn,csy, cb, DomainValueRangeStruct(dm));
	if ( ftype == Feature::ftSEGMENT)
		return SegmentMap(fn,csy, cb, DomainValueRangeStruct(dm));
}

CoordBounds GDALFormat::getLayerCoordBounds(OGRLayerH hLayer) {
	OGREnvelope	envelope;
	OGRErr err = funcs.ogrGetLayerExtent(hLayer, &envelope, TRUE);
	if ( err == OGRERR_NONE) {
		return CoordBounds(Coord(envelope.MinX, envelope.MinY), Coord(envelope.MaxX, envelope.MaxY));
	}

	return CoordBounds();
}

//-----------------------------------------------------
void GeometryFiller::fillFeature(OGRGeometryH hGeometry, int& rec, bool isMulti) {
	if ( hGeometry) {
		fillGeometry(hGeometry, rec);
		long count = funcs.ogrGetSubGeometryCount(hGeometry);
		for(int i =0; i < count; ++i) {
			OGRGeometryH hSubGeometry = funcs.ogrGetSubGeometry(hGeometry, i);
			if ( hSubGeometry) {
				fillGeometry(hSubGeometry, rec);
			}
		}
	}
}

void PointFiller::fillGeometry(OGRGeometryH hGeom, int& rec) {
	ILWIS::Point *p = CPOINT(bmp->newFeature());
	double x,y,z;
	funcs.ogrGetPoints(hGeom, 0,&x,&y,&z);
	p->setCoord(Coord(x,y,x));
	p->PutVal((long)rec++);
}



void SegmentFiller::fillGeometry(OGRGeometryH hGeom, int& rec) {
	int count = funcs.ogrGetNumberOfPoints(hGeom);
	if ( count == 0)
		return;

	ILWIS::Segment *s = CSEGMENT(bmp->newFeature());

	CoordinateArraySequence *seq = new CoordinateArraySequence(count);
	for(int i = 0; i < count; ++i) {
		double x,y,z;
		funcs.ogrGetPoints(hGeom, i,&x,&y,&z);
		Coord c(x,y,z);
		seq->setAt(c, i);
	}
	s->PutCoords(seq);
	s->PutVal((long)rec++);
}

void PolygonFiller::fillFeature(OGRGeometryH hGeometry, int& rec, bool isMulti) {
	try {
		if ( hGeometry) {
			long count = funcs.ogrGetSubGeometryCount(hGeometry);
			OGRwkbGeometryType tp = funcs.ogrGetGeometryType(hGeometry);
			if ( tp == wkbPolygon || tp == wkbPolygon25D ){
				fillPolygon(count, rec, hGeometry);
				if ( isMulti == false) // for multis de raw remains the same (same record).
					++rec;
			}
			else {
				for(int i = 0; i < count; ++i) {
					OGRGeometryH hSubGeometry = funcs.ogrGetSubGeometry(hGeometry, i);
					fillFeature(hSubGeometry, rec, count > 1);
				}
			}
		}
	} catch ( geos::util::IllegalArgumentException& ) {
		// we ignore errors during import, polygons are skipped
	}
}

void PolygonFiller::fillPolygon(int count, int rec, OGRGeometryH hGeometry) {
	ILWIS::Polygon *p = 0;
	bool first = true;
	for(int i =0; i < count; ++i) {
		OGRGeometryH hSubGeometry = funcs.ogrGetSubGeometry(hGeometry, i);
		if ( hSubGeometry) {
			LinearRing *ring = getRing(hSubGeometry);
			if ( ring) {
				const CoordinateSequence * seq = ring->getCoordinates();
				bool isCC = geos::algorithm::CGAlgorithms::isCCW(seq);
				delete seq;
				if ( first) {
					p = CPOLYGON(bmp->newFeature());
					p->PutVal((long)rec);
					p->addBoundary(ring);
					first = false;
				}
				else
					if ( p)
						p->addHole(ring);
			}
		}
	}
}

LinearRing *PolygonFiller::getRing(OGRGeometryH hSubGeometry) {
	OGRGeometryH hGeom = hSubGeometry;
	int count = funcs.ogrGetNumberOfPoints(hGeom);
	if ( count == 0) {
		hGeom = funcs.ogrGetSubGeometry(hGeom, 0);
		if ( hGeom == 0)
			return 0;
		count = funcs.ogrGetNumberOfPoints(hGeom);
		//OGRGeometryH hGeom2 = funcs.ogrGetSubGeometry(hGeom, 0);
		//if ( hGeom2) {
		//	int count2 = funcs.ogrGetNumberOfPoints(hGeom2);
		//	++count;

		//}
	}
	if ( count == 0)
		return  0;
	CoordinateArraySequence *seq = new CoordinateArraySequence();
	for(int i = 0; i < count; ++i) {
		double x,y,z;
		funcs.ogrGetPoints(hGeom, i,&x,&y,&z);
		Coord c(x,y,z);
		seq->add(c,false);
	}
	if ( seq->size() < 3 || seq->front() != seq->back())
		return 0;
	return new LinearRing(seq, new GeometryFactory());
}


