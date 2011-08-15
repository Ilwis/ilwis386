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
#include "Headers\toolspch.h"

//#pragma warning( disable : 4715 )

#include <set>
#include "Engine\Base\DataObjects\ilwisobj.h"
#include "Engine\SpatialReference\Coordsys.h"
#include "Engine\SpatialReference\Csproj.h"
#include "Engine\SpatialReference\Cslatlon.h"
#include "Engine\SpatialReference\Ellips.h"
#include "Engine\Base\File\objinfo.h"
#include "Engine\SpatialReference\DATUM.H"
#include "Engine\Base\File\objinfo.h"
#include "Engine\SpatialReference\prj.h"
#include "Engine\SpatialReference\UTM.H"
#include "Engine\SpatialReference\Grctppla.h"
#include "Engine\DataExchange\ForeignFormat.h"
#include "Engine\SpatialReference\Grcornrs.h"
#include "Engine\Base\File\Directory.h"
#include "Engine\Map\Raster\Map.h"
#include "Engine\Applications\objvirt.h"
#include "Engine\Applications\ModuleMap.h"
#include "Engine\Base\System\Engine.h"
#include "Engine\Base\DataObjects\Tranq.h"
#include "Headers\constant.h"
#include "Headers\messages.h"
#include "Headers\Hs\IMPEXP.hs"
#include "Headers\Hs\proj.hs"
#include "Headers\Htp\Ilwismen.htp"

#include "Headers\toolspch.h"
//#include <xercesc/parsers/SAXParser.hpp>
//#include <xercesc/framework/MemBufInputSource.hpp>
//#include <xercesc/util/OutOfMemoryException.hpp>
//#include <xercesc/sax/HandlerBase.hpp>

#include "Engine\Base\DataObjects\URL.h"
#include "Engine\DataExchange\WMSCollection.h"
#include "Engine\Base\DataObjects\RemoteXMLObject.h"
#include "DataExchange\WMSGetCapabilities.h"
#include "DataExchange\gdalincludes\gdal.h"
//#include "Engine\DataExchange\gdalincludes\gdal_frmts.h"

#include "DataExchange\gdalincludes\gdal.h"
#include "DataExchange\gdalincludes\gdal_frmts.h"
#include "DataExchange\gdalincludes\cpl_vsi.h"
#include "DataExchange\gdalincludes\ogr_srs_api.h"

#include "Engine\SpatialReference\GrcWMS.h"
#include "DataExchange\WMSFormat.h"


WMSFormat::WMSFormat() : urlWMS("") {
	gdalDataSet = NULL;
	grfWMS = NULL;
}

WMSFormat::WMSFormat(const FileName& fn, ParmList& pm) : urlWMS("") {

	String sC = pm.sGet("collection");
	FileName fnCollection(sC);
	WMSCollection wms(fnCollection);

	if ( wms.fValid() && !pm.fExist("getcapabilities"))
		pm.Add(new Parm("url", wms->getCapabilities().sVal()));

	URL url(pm.sGet("url"));
	urlWMS = url;
	gdalDataSet = NULL;
	if ( pm.fExist("layer"))
		layers = pm.sGet("layer");
	if ( pm.fExist("georef")) {
		grf = GeoRef(FileName(pm.sGet("georef")));
		grfWMS = grf->pgWMS();
	}
	if ( pm.fExist("url"))
		urlWMS = URL(pm.sGet("url"));
	if ( pm.fExist("srs"))
		srsName = pm.sGet("srs");
	Init();

	if ( wms.fValid()) {
		LayerInfo info;
		layerName = wms->getLayerName(fn);
		if ( layerName == "") {
			layerName = fn.sFile;
		} else {
			if ( pm.fExist("extras")) {
				String rest = pm.sGet("extras");
				Array<String> parts;
				Split(rest,parts, ",");
				for(unsigned int i = 0; i < parts.size(); ++i) {
					FileName fnextra(parts[i]);
					String n = wms->getLayerName(fnextra);
					layerName += "+" + n;
				}
			}
		}
		GetRasterInfo(info, layerName);
		FileName fnMap = FileName(info.fnObj, ".mpr");
		Map map = Map(fnMap, info);	
		if ( !pm.fExist("import"))
			map->SetUseAs(true);
		else
		{
			map->SetUseAs(false);			
		}			
		map->Store();
		map->gr()->Store();
		Store(map);
		wms->Add(map->gr());
		wms->Add(map->cs());
		wms->Add(map->fnObj, layerName);
	}
}

WMSFormat::~WMSFormat() {
	//delete image;
	if ( gdalDataSet != NULL)
		funcs.close(gdalDataSet);
	if ( trq )
	{
		trq->Stop();
		delete trq;
		trq = NULL;
	}
}

ForeignFormat* CreateQueryObjectWMS()
{
	return new WMSFormat();
}


void WMSFormat::Init() {
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
				funcs.registerJPEG = (GDALAllRegisterFunc)GetProcAddress(hm,"GDALRegister_JPEG");
				funcs.registerPNG = (GDALAllRegisterFunc)GetProcAddress(hm,"GDALRegister_PNG");
				funcs.open = (GDALOpenFunc)GetProcAddress(hm,"_GDALOpen@8");
				funcs.getBand = (GDALGetRasterBandFunc)GetProcAddress(hm,"_GDALGetRasterBand@8");
				funcs.newSRS = (OSRNewSpatialReferenceFunc)GetProcAddress(hm, "_OSRNewSpatialReference@4");
				funcs.srsImportFromEPSG = (OSRImportFromEPSGFunc)GetProcAddress(hm,"_OSRImportFromEPSG@8");
				funcs.isProjected = (OSRIsProjectedFunc)GetProcAddress(hm,"OSRIsProjected");
				funcs.rasterIO = (GDALRasterIOFunc)GetProcAddress(hm,"_GDALRasterIO@48");
				funcs.getAttribute = (OSRGetAttrValueFunc)GetProcAddress(hm,"_OSRGetAttrValue@12");
				funcs.finderLoc = (CPLPushFinderLocationFunc)GetProcAddress(hm,"CPLPushFinderLocation");
				funcs.vsiClose = (VSIFCloseLFunc)GetProcAddress(hm,"VSIFCloseL");
				funcs.vsiFileFromMem = (VSIFileFromMemBufferFunc)GetProcAddress(hm,"VSIFileFromMemBuffer");
				
			} 
		}
	}
	if (0 == trq)
		trq = new Tranquilizer();
	trq->SetDelayShow(false);
	trq->SetNoStopButton(true);
	trq->SetTitle(TR("Opening WMS"));
	trq->fText(TR("Opening WMS"));
}

void WMSFormat::ReadParameters(const FileName& fnObj, ParmList& pm) 
{
	ForeignFormat::ReadParameters(fnObj, pm);
	ObjectInfo::ReadElement("ForeignFormat","Layer",fnObj,layerName);
	pm.Add(new Parm("layer", layerName));
	FileName fnGeo;
	ObjectInfo::ReadElement("ForeignFormat","GeoRef",fnObj,fnGeo);
	if ( fnGeo.sFile != "")
		grf = GeoRef(fnGeo);
	pm.Add(new Parm("georef", fnGeo.sRelative()));
	ObjectInfo::ReadElement("ForeignFormat","SRS",fnObj,srsName);
	pm.Add(new Parm("srs", srsName));
	String sV;
	ObjectInfo::ReadElement("ForeignFormat","URL",fnObj,sV);
	urlWMS = URL(sV);
	pm.Add(new Parm("url", urlWMS.sVal()));
}

ForeignFormat *CreateImportObjectWMS(const FileName& fnFO, ParmList& pm) {

	if ( pm.iSize() < 2 ) // existing object
	{
		String sMethod;
		if ( ObjectInfo::ReadElement("ForeignFormat", "Method", fnFO, sMethod) != 0)	// oldstyle expressions
		{
			ForeignFormat *ff = ForeignFormat::Create(sMethod);
			if ( ff )
				ff->ReadParameters(fnFO, pm);
			delete ff;
		}	
	}
	ForeignFormat *ff = NULL;
	try
	{

		ff =  new WMSFormat(fnFO, pm);
		return ff;
	}
	catch (ErrorObject& err)
	{
		delete ff;		
		throw err;
	}		
	return NULL;
}

struct WMSThreadData {
	WMSThreadData(URL u) : url(u), fIsThreaded(true) {}
	URL url;
	WMSCollection *fptr;
	ParmList pm;
	Directory dir;
	bool fIsThreaded;

};

void WMSFormat::PutDataInCollection(ForeignCollectionPtr* col, ParmList& pm)
{

	WMSThreadData *data = new WMSThreadData(urlWMS);
	data->fptr = new WMSCollection(col->fnObj, pm);
	data->dir = Directory(getEngine()->sGetCurDir());
	for(int i = 0; i < pm.parms.size(); ++i) {
		Parm *p = pm.parms[i];
		if ( p->pmt() == pmtFixed)
			data->pm.Add(new Parm(i, p->sVal()));
		else
			data->pm.Add(new Parm(p->sOpt(), p->sVal()));
	}

	if ( pm.fExist("nothreads") ) {
		data->fIsThreaded = false;
		PutDataInThread((VOID *)data);
	}
	else
		::AfxBeginThread(PutDataInThread, (VOID *)data);
}

UINT WMSFormat::PutDataInThread(LPVOID lp) {
	WMSThreadData *data = (WMSThreadData *)lp;
	try{
		if ( data->fIsThreaded)
			getEngine()->InitThreadLocalVars();
		getEngine()->SetCurDir(data->dir.sFullPath());

		WMSGetCapabilities capabilities(data->url);
		vector<WMSLayerInfo *> layers = capabilities.getLayerInfo();
		PutData(layers, data->fptr->ptr(), data->pm);
		
		if ( data->fIsThreaded )
			getEngine()->RemoveThreadLocalVars();
		getEngine()->PostMessage(ILW_READCATALOG, 0, 0);
		delete data->fptr;  // remove the foreign collection (because passed as pointer)
		delete data;
		data = 0;
	}
	catch(ErrorObject& err)
	{
		FileName fnCol = (*(data->fptr))->fnObj;
		FileName *fn = new FileName(fnCol);								
		AfxGetApp()->GetMainWnd()->SendMessage(ILWM_CLOSECOLLECTION, (WPARAM)fn, 0);
		if ( fnCol != FileName() )
		{
			String sCmd("del %S -quiet -force", fnCol.sFullPath());
			getEngine()->Execute(sCmd);
			AfxGetApp()->GetMainWnd()->PostMessage(ILW_READCATALOG, WP_RESUMEREADING, 0);		
		}
		if (data)
			delete data->fptr;
		delete data;	
		getEngine()->RemoveThreadLocalVars();					
		err.Show("WMS Error");
	}

	return 1;
}

void WMSFormat::PutData(vector<WMSLayerInfo *> layers, WMSCollectionPtr* col, ParmList& pm) {
	Tranquilizer trq;
	trq.SetDelayShow(false);
	trq.SetNoStopButton(true);
	trq.SetTitle(TR("Opening WMS"));
	trq.SetText(TR("Filling Catalog"));
	long count = 0;
	for(vector<WMSLayerInfo *>::iterator cur = layers.begin(); cur != layers.end(); ++cur,++count) {
		trq.fUpdate(count, layers.size());
		WMSLayerInfo *info = *cur;
		if ( info->layers.size() == 0 ) {
			FileName fnObj(makeCompatible(info->name), ".mpr");
			col->Add(fnObj, info->name);
		} else {
				if ( info->name == "")
					PutData(info->layers, col, pm);
				else {
					FileName fnObj(makeCompatible(info->name), ".ioc");
					WMSCollection col2(fnObj, sTypeWMSCollection, pm);
					PutData(info->layers, col2.ptr(), pm);
					col->Add(fnObj, makeCompatible(info->name));
					fnObj = FileName(makeCompatible(info->name), ".mpr");
					col->Add(fnObj, info->name);
				}
				
		}
	}
}

bool WMSFormat::fIsCollection(const String& sForeignObject) const
{
	return true;
}

bool WMSFormat::fMatchType(const String& fnFileName, const String& sType)
{
	return sType == "ILWIS WMSCollection";
}

WMSLayerInfo* WMSFormat::find(vector<WMSLayerInfo *> layers, const String& sName) {
	for(vector<WMSLayerInfo *>::iterator cur = layers.begin(); cur != layers.end(); ++cur) {
		WMSLayerInfo *lyr = *cur;
		if ( lyr->name == sName) {
				return lyr;
		} else {
			if ( lyr->layers.size() > 0)  {
				WMSLayerInfo * l = find(lyr->layers, sName);
			    if ( l != NULL)
					return l;
			}
		}
	}
	return NULL;
}

CoordSystem WMSFormat::getCoordSystem(const FileName& fnBase, const String& srsName) {
	String path = getEngine()->getContext()->sIlwDir();
	path += "Resources\\gdal_data";
//	OGRSpatialReference oSRS;
	//funcs.finderLoc(path.c_str());

	OGRSpatialReferenceH handle = funcs.newSRS(NULL);
	OGRErr err = funcs.srsImportFromEPSG( handle, srsName.sTail(":").iVal());
	if ( err == OGRERR_UNSUPPORTED_SRS )
		throw ErrorObject(String("The SRS %S is not supported", srsName));

	String datumName(funcs.getAttribute(handle, "Datum",0));
	//map<String, ProjectionConversionFunctions>::iterator where = mpCsyConvers.find(projectionName);

	FileName fnCsy(FileName::fnUnique(FileName(fnBase, ".csy")));

	CoordSystemViaLatLon *csv=NULL;
	if ( funcs.isProjected(handle)) {
		CoordSystemProjection *csp =  new CoordSystemProjection(fnCsy, 1);
		String dn = Datum::WKTToILWISName(datumName);
		if ( dn == "")
			throw ErrorObject("Datum can't be transformed to an ILWIS known datum");
		csp->datum = new MolodenskyDatum(dn,"");
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

String WMSFormat::getMapRequest(const CoordBounds& cb, const String& layers, const String& srsName, const RowCol rc) const{
	double ratio = cb.width()/ cb.height();
	String mapRequest = "http://" + urlWMS.getPath();
	String mp = urlWMS.getQueryValue("map");
	mapRequest += "?";
	if(mp != "")
		mapRequest += "map=" + mp + "&";
	String version = urlWMS.getQueryValue("version");
	if ( version == "")
		version="1.1";
	mapRequest += "SERVICE=WMS&VERSION=" + version + "&";
    mapRequest += "REQUEST=GetMap&";
	mapRequest += "BBOX=" + String("%f,%f,%f,%f", cb.MinX(), cb.MinY(),cb.MaxX(),cb.MaxY()) + "&";
	mapRequest += "SRS=" + srsName + "&";
	mapRequest += "FORMAT=image/jpeg&";
	mapRequest += String("HEIGHT=%d&", rc.Row);
	mapRequest += "WIDTH=" + String("%i&",rc.Col);
	mapRequest += "Layers=" + toLegalLayer(layers) + "&";
	mapRequest += "Styles=";

	return mapRequest;
}

void WMSFormat::GetRasterInfo(LayerInfo& inf, String sLayers) {
	WMSGetCapabilities capabilities(URL(urlWMS.sVal()));
	vector<WMSLayerInfo *> layers = capabilities.getLayerInfo();
	if ( layers.size() == 0)
		throw ErrorObject("No layers could be deduced from GetCapabilities request");

	String firstName = sLayers.sHead("+");
	WMSLayerInfo *layer = find(layers, firstName);
	if ( layer == NULL)
		throw ErrorObject(String("No layer found for %S", sLayers));

	FileName fnBase(FileName::fnUnique(FileName(makeCompatible(sLayers), ".mpr")));

	pair<String, CoordBounds > start;
	bool containsWGS84 = false;
	WMSLayerInfo *srsLayer = findRelevantSrsLayer(layers, layer,NULL);
	for(map<String, CoordBounds>::iterator p = srsLayer->srs.begin() ; p != srsLayer->srs.end(); ++p) {
		if ( (*p).second.fValid())
			start = *p;
		if ( (*p).first == "EPSG:4326")
			containsWGS84 = true;

	}
	bool validSRSDef= start.first != "" || start.second.fValid();
	if (  !validSRSDef && !containsWGS84) {
		throw ErrorObject("Can't render this layer; no valid entries found for either SRS or Bounding box");
	}

	srsName = validSRSDef ? start.first : "EPSG:4326";
	CoordSystem csy = getCoordSystem(fnBase, srsName);
	inf.csy = csy;
	inf.dvrsMap = DomainValueRangeStruct(Domain("color"));
	inf.fnObj = fnBase;

	CoordBounds cb = validSRSDef ? start.second : layer->bbLatLon;

	inf.cbActual = inf.cbMap = cb;

	FileName fnGeo(FileName::fnUnique(FileName(fnBase, ".grf")));
	inf.fnObj = FileName(fnBase, ".mpr");
	double ratio = cb.width()/ cb.height();
	MinMax mm(RowCol(0,0),RowCol(1500, (int)(1500*ratio))); // dummy size
	grf.SetPointer(new GeoRefCornersWMS(fnGeo, csy, mm, true, cb.cMin, cb.cMax)); 
	inf.grf = grf;

}
WMSLayerInfo *WMSFormat::findRelevantSrsLayer(vector<WMSLayerInfo *> layers, WMSLayerInfo *targetlayer, WMSLayerInfo *srsLayer) {
		
	for(vector<WMSLayerInfo *>::iterator cur = layers.begin(); cur != layers.end(); ++cur) {
		WMSLayerInfo *lyr = *cur;
		if ( lyr->name == targetlayer->name) {
			if ( lyr->srs.size() == 0)
				return srsLayer;
			else
				return lyr;
		} else {
			if ( lyr->layers.size() > 0)  {
				if (lyr->srs.size() > 0) {
					srsLayer = lyr;
				}
				WMSLayerInfo * l = findRelevantSrsLayer(lyr->layers, targetlayer, srsLayer);
			    if ( l != NULL)
					return l;
				else
					return srsLayer;
			}
		}
	}
	return NULL;
}

void WMSFormat::HandleError(const String& sErr) const{
	throw ErrorObject("GetMap request returned a xml document, not a valid image",sErr);
}

void WMSFormat::Store(IlwisObject obj) {
	ForeignFormat::Store(obj);
	obj->WriteElement("ForeignFormat","Method","WMS");
	if ( layerName != "")
		obj->WriteElement("ForeignFormat","Layer", layerName);
	if ( grf.fValid())
		obj->WriteElement("ForeignFormat","GeoRef",grf->fnObj);
	if ( srsName != "")
		obj->WriteElement("ForeignFormat","SRS",srsName);
	if ( urlWMS.sVal() != "" )
		obj->WriteElement("ForeignFormat","URL", urlWMS.sVal());
}

void WMSFormat::retrieveImage() const {
	CoordBounds cb2 = grf->cb();
	String sExpr = getMapRequest(cb2, layers, srsName, grf->rcSize());
	RemoteObject rxo(sExpr);
	MemoryStruct *image;
	image = rxo.get();
	if(image->memory[0] == '<' && image->memory[1] == '?' && image->memory[2] == 'x' && image->memory[3]== 'm') {
		String error(image->memory);
		HandleError(error);
	}
	//funcs.registerAll();
	funcs.registerJPEG();

	funcs.vsiClose( funcs.vsiFileFromMem( "/vsimem/work.dat", image->memory,
                                       image->size, FALSE ) );

	void *hDS = (void *)funcs.open( "/vsimem/work.dat", GA_ReadOnly );

	if ( hDS == NULL) {
		funcs.registerPNG();
		funcs.vsiClose( funcs.vsiFileFromMem( "/vsimem/work.dat", image->memory,
                                       image->size, FALSE ) );

		hDS = (void *)funcs.open( "/vsimem/work.dat", GA_ReadOnly );
		if ( hDS == NULL)
			throw ErrorObject("GetMap request returned not a valid image");
	}

	const_cast<GDALDatasetH>(gdalDataSet) = hDS;
}

void WMSFormat::GetLineRaw(long iLine, ByteBuf&, long iFrom, long iNum) const {
}

void WMSFormat::GetLineRaw(long iLine, IntBuf&,  long iFrom, long iNum) const {
}

void WMSFormat::GetLineRaw(long iLine, LongBuf& buf, long iFrom, long iNum) const {
	if ( grfWMS->hasChanged() || gdalDataSet == NULL)
		retrieveImage();

	unsigned char *data1 = new unsigned char[iNum];
	unsigned char *data2 = new unsigned char[iNum];
	unsigned char *data3 = new unsigned char[iNum];
	int fact = 1;

	GDALRasterBandH  gdalRasterBand = funcs.getBand( gdalDataSet, 1);

	funcs.rasterIO(gdalRasterBand,GF_Read, iFrom, iLine, iNum, 1, data1, iNum, 1, GDT_Byte, 0, 0);								
	gdalRasterBand = funcs.getBand( gdalDataSet, 2);
    funcs.rasterIO(gdalRasterBand,GF_Read, iFrom, iLine, iNum, 1, data2, iNum, 1, GDT_Byte, 0, 0);								
	gdalRasterBand = funcs.getBand( gdalDataSet, 3);
    funcs.rasterIO(gdalRasterBand, GF_Read, iFrom, iLine, iNum, 1, data3, iNum, 1, GDT_Byte, 0, 0);
	
	for(int j = 0; j < iNum; ++j) {
		buf[j] = (data1[j]) | (data2[j] << 8) | (data3[j] << 16);
	}

	delete data1;
	delete data2;
	delete data3;

}

void WMSFormat::GetLineVal(long iLine, LongBuf&, long iFrom, long iNum) const {
}

void WMSFormat::GetLineVal(long iLine, RealBuf&, long iFrom, long iNum) const {
}

long WMSFormat::iRaw(RowCol) const {
	return iUNDEF;
}

long WMSFormat::iValue(RowCol) const {
	return iUNDEF;
}

double WMSFormat::rValue(RowCol) const {
	return rUNDEF;
}

String WMSFormat::makeCompatible(const String& name) {
	String result1 = "";
	for(string::const_iterator p=name.begin() ; p != name.end(); ++p) {
		if ( *p == '/' || *p == ':' || *p == '.' || *p == '\\' ) result1 += "_";
		else result1 += *p;
	}

	return result1;
}

String WMSFormat::toLegalLayer(const String& name) const {
	String result1 = "";
	for(string::const_iterator p=name.begin() ; p != name.end(); ++p) {
		if ( *p == '+') result1 += ",";
		else result1 += *p;
	}

	return result1;
}

FileName WMSFormat::createCollectionName(const String& name, ParmList& pm) {
	URL url(name);
	FileName fnNew = url.toFileName(true);

	FileName fnIOC = FileName::fnUnique(FileName(fnNew, ".ioc")); 
	String sMap = url.getQueryValue("map");
	if ( sMap != "") {
		String newName = "";
		Array<String> parts;
		SplitOn(sMap, parts, "/\\");
		sMap = parts[parts.iSize() - 1];
		for(string::const_iterator p=sMap.begin() ; p != sMap.end(); ++p) {
			if(isalpha(*p))
				newName += *p;
			else
				newName += '_';
		}
		fnIOC.sFile += newName;
	}
	return fnIOC;
}



