/***************************************************************
 ILWIS integrates image, vector and thematic data in one unique 
 and powerful package on the desktop. ILWIS delivers a wide 
 range of feautures including import/export, digitizing, editing, 
 analysis and display of data as well as production of 
 quality mapsinformation about the sensor mounting platform
 
 Exclusive rights of use by 52�North Initiative for Geospatial 
 Open Source Software GmbH 2007, Germany

 Copyright (C) 2007 by 52�North Initiative for Geospatial
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
#include "Engine\Base\System\Appcont.h"
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
#include "Engine\Base\DataObjects\RemoteXMLObject.h"
#include "DataExchange\gdalincludes\gdal.h"
#include "Engine\DataExchange\GdalProxy.h"

#include "DataExchange\gdalincludes\gdal.h"
#include "DataExchange\gdalincludes\gdal_frmts.h"
#include "DataExchange\gdalincludes\cpl_vsi.h"
#include "DataExchange\gdalincludes\ogr_srs_api.h"

#include "Engine\SpatialReference\GrcWMs.h"
#include "Engine\SpatialReference\GrcOSM.h"
#include "DataExchange\OpenStreetMapFormat.h"


OpenStreetMapFormat::OpenStreetMapFormat()
: urlOpenStreetMap("")
, rxo(0)
,grfOpenStreetMap(0)
{
	gdalDataSet = NULL;
	grfOpenStreetMap = NULL;
}

OpenStreetMapFormat::OpenStreetMapFormat(const FileName& fn, ParmList& pm)
: urlOpenStreetMap("")
, rxo(0)
,grfOpenStreetMap(0)
{

	URL url(pm.sGet("url"));
	urlOpenStreetMap = url;
	gdalDataSet = NULL;
	if ( pm.fExist("georef")) {
		grf = GeoRef(FileName(pm.sGet("georef"),true));
		grfOpenStreetMap =grf->pgOSM();
		grfOpenStreetMap->SetRetrieveProc(this, (RetrieveImageProc)&OpenStreetMapFormat::retrieveImage);
	}
	if ( pm.fExist("url"))
		urlOpenStreetMap = URL(pm.sGet("url"));

	LayerInfo info;
	GetRasterInfo(info, "");
	String ilwDir = getEngine()->getContext()->sStdDir();
	String path = ilwDir + "\\basemaps\\PseudoMercator.csy";
	pseudoMercator = CoordSystem(path);
	llwgs84 = CoordSystem("LatLonWgs84.csy");
}

OpenStreetMapFormat::~OpenStreetMapFormat() {
	//delete image;
	if (rxo != 0)
		delete rxo;
	if ( gdalDataSet != NULL)
		getEngine()->gdal->close(gdalDataSet);
}

ForeignFormat* CreateQueryObjectOpenStreetMap()
{
	return new OpenStreetMapFormat();
}


void OpenStreetMapFormat::ReadParameters(const FileName& fnObj, ParmList& pm) 
{
	ForeignFormat::ReadParameters(fnObj, pm);
	pm.Add(new Parm("layer", "openstreetmap"));
	FileName fnGeo;
	ObjectInfo::ReadElement("ForeignFormat","GeoRef",fnObj,fnGeo);
	if ( fnGeo.sFile != "")
		grf = GeoRef(fnGeo);
	pm.Add(new Parm("georef", fnGeo.sRelative()));
	String sV;
	ObjectInfo::ReadElement("ForeignFormat","URL",fnObj,sV);
	urlOpenStreetMap = URL(sV);
	pm.Add(new Parm("url", urlOpenStreetMap.sVal()));
}

ForeignFormat *CreateImportObjectOpenStreetMap(const FileName& fnFO, ParmList& pm) {

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

		ff =  new OpenStreetMapFormat(fnFO, pm);
		return ff;
	}
	catch (ErrorObject& err)
	{
		delete ff;		
		throw err;
	}		
	return NULL;
}

struct OpenStreetMapThreadData {
	OpenStreetMapThreadData(URL u) : url(u), fIsThreaded(true) {}
	URL url;
	ParmList pm;
	Directory dir;
	bool fIsThreaded;

};




bool OpenStreetMapFormat::fIsCollection(const String& sForeignObject) const
{
	return false;
}

bool OpenStreetMapFormat::fMatchType(const String& fnFileName, const String& sType)
{
	return sType == "ILWIS OpenStreetMap";
}

String OpenStreetMapFormat::getMapRequest(const CoordBounds& cb2, const RowCol rc) const{


	CoordBounds cb = llwgs84->cbConv(pseudoMercator,cb2);


	double z  = 360.0 / cb.width();
	double p1 = log10( z);
	double p2 = log10(2.0);
	int zoom =  ceil(p1/p2) - 1;
	zoom = min(zoom,18);
	String url = urlOpenStreetMap.sVal();
	int index1 = url.find("[");
	int index2 = url.find("]");
	String serverIds = url.sSub(index1 + 1, index2 - index1 - 1);
	int ind = rand() % serverIds.size();
	String n = serverIds[ind];
	double x = (cb.middle().x + cb.MinX()) / 2.0;
	int xtile = long2tilex(x, zoom);
	double y = (cb.middle().y + cb.MinY())/2.0;
	int ytile = lat2tiley(y, zoom);
	url = url.substr(0,index1) + n + url.substr(index2+1);
	url += String("%d/%d/%d.png",zoom,xtile,ytile);
	TRACE(String("[ %d,%d ]\n", xtile, ytile).c_str());

	return url;
}

void OpenStreetMapFormat::GetRasterInfo(LayerInfo& inf, String sLayers) {

	FileName fnBase("openstreetmap.mpr");

	CoordSystem csy("LatLonWGS84");
	inf.csy = csy;
	inf.dvrsMap = DomainValueRangeStruct(Domain("color"));
	inf.fnObj = fnBase;

	CoordBounds cb ;

	inf.cbActual = inf.cbMap = cb;

	FileName fnGeo("openstreetmap.grf");
	inf.fnObj = FileName(fnBase, ".mpr");
	double ratio = cb.width()/ cb.height();
	MinMax mm(RowCol(0,0),RowCol(1000, (int)(1000*ratio))); // dummy size
	grf.SetPointer(new GeoRefOSM(fnGeo, csy, mm, true, cb.cMin, cb.cMax)); 
	inf.grf = grf;

}


void OpenStreetMapFormat::Store(IlwisObject obj) {
	ForeignFormat::Store(obj);
	obj->WriteElement("ForeignFormat","Method","OpenStreetMap");
	if ( grf.fValid())
		obj->WriteElement("ForeignFormat","GeoRef",grf->fnObj);
	if ( urlOpenStreetMap.sVal() != "" )
		obj->WriteElement("ForeignFormat","URL", urlOpenStreetMap.sVal());
}

int OpenStreetMapFormat::long2tilex(double lon, int z) const
{ 
	double p1 = (lon + 180.0) / 360.0;
	double p2 = pow(2.0, z);
	double p3 = p1 * p2;
	double p4 = (int)(floor(p3));
	return (int)(p4); 
}
 
int OpenStreetMapFormat::lat2tiley(double lat, int z) const
{
	if ( lat > 84)
		lat = 85;
	if ( lat < -84)
		lat = -85;
	double p1 = tan(lat * M_PI/180.0);
	double p2 = cos(lat * M_PI/180.0);
	double p3 = pow(2.0, z);
	double p4 = log( p1 + 1.0 / p2 );
	double p5 = 1.0 - p4 / M_PI;
	double p6 = p5 / 2.0;
	int tiley =  (int)(floor(p6 * p3)); 
	return tiley;

}

bool OpenStreetMapFormat::retrieveImage() {
	if(!grfOpenStreetMap){
		throw ErrorObject(TR("Georeference not correctly set"));
		return false;
	}

	CoordBounds cb2 = grfOpenStreetMap->cbWMSRequest();

	String sExpr = getMapRequest(cb2, grfOpenStreetMap->rcWMSRequest());
	if (rxo == 0)
		rxo = new RemoteObject();
	rxo->getRequest(sExpr);
	MemoryStruct *image;
	image = rxo->get();
	if ( image == 0) {
		return false;
	}

	if(image->memory[0] !=  137 && image->memory[1] != 'P' && image->memory[2] != 'N' && image->memory[3]!= 'G') {
		if ( image->memory[6] != 'J' && image->memory[7] != 'F' && image->memory[8] != 'I' && image->memory[9] != 'F')
			return false;

	}
	getEngine()->gdal->registerAll();
	//getEngine()->gdal->registerPNG();

	getEngine()->gdal->vsiClose( getEngine()->gdal->vsiFileFromMem( "/vsimem/work.dat", image->memory,
                                       image->size, FALSE ) );

	void *hDS = (void *)getEngine()->gdal->open( "/vsimem/work.dat", GA_ReadOnly );

	const_cast<GDALDatasetH>(gdalDataSet) = hDS;

	int c = getEngine()->gdal->count(gdalDataSet);

	return true;

}

void OpenStreetMapFormat::GetLineRaw(long iLine, ByteBuf&, long iFrom, long iNum) const {
}

void OpenStreetMapFormat::GetLineRaw(long iLine, IntBuf&,  long iFrom, long iNum) const {
}

void OpenStreetMapFormat::GetLineRaw(long iLine, LongBuf& buf, long iFrom, long iNum) const {
	unsigned char *data1 = new unsigned char[iNum];
	unsigned char *data2 = new unsigned char[iNum];
	unsigned char *data3 = new unsigned char[iNum];
	int fact = 1;

	GDALRasterBandH  gdalRasterBand = getEngine()->gdal->getBand( gdalDataSet, 1);
	GDALDataType tp = getEngine()->gdal->getDataType(gdalRasterBand);

	getEngine()->gdal->rasterIO(gdalRasterBand,GF_Read, iFrom, iLine, iNum, 1, data1, iNum, 1, GDT_Byte, 0, 0);								
	gdalRasterBand = getEngine()->gdal->getBand( gdalDataSet, 2);
	if ( gdalRasterBand)
		getEngine()->gdal->rasterIO(gdalRasterBand,GF_Read, iFrom, iLine, iNum, 1, data2, iNum, 1, GDT_Byte, 0, 0);								
	gdalRasterBand = getEngine()->gdal->getBand( gdalDataSet, 3);
	if ( gdalRasterBand)
		getEngine()->gdal->rasterIO(gdalRasterBand, GF_Read, iFrom, iLine, iNum, 1, data3, iNum, 1, GDT_Byte, 0, 0);

	if ( gdalRasterBand) {
		for(int j = 0; j < iNum; ++j) {
			buf[j] = (data1[j]) | (data2[j] << 8) | (data3[j] << 16);
		}
	} else {
		for(int j = 0; j < iNum; ++j) {
			buf[j] = data1[j];
		}
	}

	delete [] data1;
	delete [] data2;
	delete [] data3;
}

void OpenStreetMapFormat::GetLineVal(long iLine, LongBuf&, long iFrom, long iNum) const {
}

void OpenStreetMapFormat::GetLineVal(long iLine, RealBuf&, long iFrom, long iNum) const {
}

long OpenStreetMapFormat::iRaw(RowCol) const {
	return iUNDEF;
}

long OpenStreetMapFormat::iValue(RowCol) const {
	return iUNDEF;
}

double OpenStreetMapFormat::rValue(RowCol) const {
	return rUNDEF;
}







