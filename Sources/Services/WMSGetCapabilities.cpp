#include "headers/toolspch.h"
#include "Engine\Base\DataObjects\ilwisobj.h"
#include "Engine\Base\System\Engine.h"
#include "HttpServer\IlwisServer.h"
#include "HttpServer\mongoose.h"
#include "Engine\Base\DataObjects\XMLDocument.h"
#include "httpserver\RequestHandler.h"
#include "Services\WMSGetCapabilities.h"
#include "Engine\Base\System\module.h"
#include "Engine\Applications\ModuleMap.h"
#include "Engine\Map\basemap.h"


using namespace ILWIS;

RequestHandler *WMSGetCapabilities::createHandler(struct mg_connection *c, const struct mg_request_info *request_info, const map<String, String>& kvps, IlwisServer *serv) {
	return new WMSGetCapabilities(c, request_info,kvps, serv);
}

WMSGetCapabilities::WMSGetCapabilities(struct mg_connection *c, const struct mg_request_info *ri, const map<String, String>& kvps, IlwisServer *serv)
: RequestHandler("WMSGetCapabilitiesHandler", c,ri,kvps, serv)
{
	config.add("Services", "WMSHandlers");
}

void WMSGetCapabilities::writeResponse() const{
	ILWIS::XMLDocument doc;
	pugi::xml_node first = doc.addNodeTo(doc,"WMT_MS_Capabilities");

	first.append_attribute("service") = "WMS";
	first.append_attribute("version") = "1.1.1";
	first.append_attribute("xml:lang") = "en-CA";
	first.append_attribute("updateSequence") = "0";
	
	pugi::xml_node ser = doc.addNodeTo(doc,"Service");
	doc.addNodeTo(ser,"Name", "WMS");
	doc.addNodeTo(ser,"Title",getConfigValue("WMS:ServiceContext:Title"));
	doc.addNodeTo(ser,"Abstract",getConfigValue("WMS:ServiceContext:Abstract"));
	pugi::xml_node kw = doc.addNodeTo(ser,"Keywords");
	String keywords = getConfigValue("WMS:ServiceContext:Keywords");
	Array<String> words;
	Split(keywords,words,";");
	for(int i = 0; i < words.size(); ++i) {
		doc.addNodeTo(kw, "Keyword",words[i]);
	} 
	pugi::xml_node con = doc.addNodeTo(ser,"ContactInformation");
	pugi::xml_node conP = doc.addNodeTo(con, "ContactPersonPrimary");
	doc.addNodeTo(conP,"ContactPerson",getConfigValue("WMS:ServiceContext:ProviderContactName"));
	doc.addNodeTo(conP,"ContactOrganization",getConfigValue("WMS:ServiceContext:ProviderName"));
	doc.addNodeTo(conP,"ContactElectronicMailAddress",getConfigValue("WMS:ServiceContext:ProviderEMail"));

	pugi::xml_node cap = doc.addNodeTo(ser, "Capability");
	pugi::xml_node req = doc.addNodeTo(cap, "Request");
	pugi::xml_node gcap = doc.addNodeTo(req, "GetCapabilities");
	doc.addNodeTo(gcap,"Format","text/xml");
	pugi::xml_node dcp = doc.addNodeTo(gcap,"DCPType");
	pugi::xml_node http = doc.addNodeTo(dcp,"HTTP");
	pugi::xml_node get = doc.addNodeTo(http,"Get");
	pugi::xml_node ores = doc.addNodeTo(get,"OnlineResource");
	ores.append_attribute("xmlns:xlink")="http://www.w3.org/1999/xlink";
	ores.append_attribute("xlink:type")="simple";
	ores.append_attribute("xlink:href")=getConfigValue("WMS:ServiceContext:GetCapabilities").c_str();

	gcap = doc.addNodeTo(req, "GetMap");
	doc.addNodeTo(gcap,"Format","image/png");
	dcp = doc.addNodeTo(gcap,"DCPType");
	http = doc.addNodeTo(dcp,"HTTP");
	get = doc.addNodeTo(http,"Get");
	ores = doc.addNodeTo(get,"OnlineResource");
	ores.append_attribute("xmlns:xlink")="http://www.w3.org/1999/xlink";
	ores.append_attribute("xlink:type")="simple";
	ores.append_attribute("xlink:href")=getConfigValue("WMS:ServiceContext:GetMap").c_str();


	pugi::xml_node exc = doc.addNodeTo(ser,"Exception");
	doc.addNodeTo(exc,"Format","XML");

	int n = getConfigValue("WMS:ServiceContext:NumberOfCatalogs").iVal();
	for(int i=0; i < n; ++i) {
		String catalogDef("WMS:%S", getConfigValue(String("WMS:ServiceContext:Catalog%d",i)));
		Array<String> parts;
		Split(catalogDef,parts,";");

		pugi::xml_node layer = doc.addNodeTo(ser,"Layer");
		String location =  parts[1];
		String filter = parts.size() > 2 ? parts[2].sHead(",") : "*.*";
		bool recursive = parts.size() > 2 ? parts[2].sTail(",").fVal() : false;
		doc.addNodeTo(layer,"Title", parts[0]);
		list<FileName> files;
		if ( filter != sUNDEF) {
			handleFilteredCatalog(location,filter, false, files);
			for(list<FileName>::iterator cur=files.begin(); cur != files.end(); ++cur) {
				handleFile(layer, doc, *cur);
			}
		}
	}
	String txt = doc.toString();
	char *buf = new char[txt.size() + 1];
	memset(buf,0,txt.size() + 1);
	memcpy(buf,txt.c_str(), txt.size());
	writeHeaders("text/xml", txt.size());
	mg_write(getConnection(), buf, txt.size());
	delete [] buf;
}


void WMSGetCapabilities::handleFilteredCatalog(const String& location,const String& filter, bool recursive, list<FileName>& files) const{
	CFileFind finder;
	FileName pseudoName(filter);
	String pattern = location + "\\" + filter;
	BOOL fFound = finder.FindFile(pattern.c_str());
	while(fFound) {
		fFound = finder.FindNextFile();
		if (!finder.IsDirectory())
		{
			FileName fnNew(finder.GetFilePath());
			if ( pseudoName.sExt == "" || pseudoName.sExt == fnNew.sExt)
				files.push_back(fnNew);
		}
	}
}

void WMSGetCapabilities::handleFile(pugi::xml_node& layer,ILWIS::XMLDocument& doc, const FileName& fn) const{
	IlwisObject ob = IlwisObject::obj(fn);
	if ( !ob.fValid())
		return;
	if ( IOTYPEBASEMAP(fn)) {
		BaseMap bmp(fn);
		String name = fn.sFile;
		String desc = bmp->sDescr();
		if ( desc == "")
			desc = fn.sFile + fn.sExt;
		String id = bmp->cs()->getIdentification();
		if ( id != sUNDEF) {
			pugi::xml_node lyr = doc.addNodeTo(layer,"Layer");
			doc.addNodeTo(lyr, "Name",name);
			doc.addNodeTo(lyr,  "Title",desc);

			CoordBounds cb = bmp->cb();
			if ( bmp->cs()->fLatLon2Coord()) {
				LatLon llMin = bmp->cs()->llConv(cb.cMin);
				LatLon llMax = bmp->cs()->llConv(cb.cMax);
				if ( !(llMin.fUndef() || llMax.fUndef())) {
					//doc.addNodeTo(lyr,"SRS", "EPSG:4326");
					pugi::xml_node bb = doc.addNodeTo(lyr, "LatLonBoundingBox");
					String latmin("%f",llMin.Lat);
					String lonmin("%f",llMin.Lon);
					String latmax("%f",llMax.Lat);
					String lonmax("%f",llMax.Lon);
					bb.append_attribute("minx") = latmin.c_str();
					bb.append_attribute("miny") = latmin.c_str();
					bb.append_attribute("maxx") = lonmax.c_str();
					bb.append_attribute("maxy") = latmax.c_str();
				} 
			}
			if ( bmp->cs()->pcsViaLatLon() && !bmp->cs()->pcsLatLon()) {
				String id = bmp->cs()->getIdentification();
				String epsg = String("EPSG:%S",id);
				doc.addNodeTo(lyr,"SRS", epsg );
				if ( id.find("epsg") != string::npos)
					id = id.sTail("=");
				pugi::xml_node bb = doc.addNodeTo(lyr,"BoundingBox");
				bb.append_attribute("SRS") = epsg.c_str();
			
				String xmin("%f",cb.cMin.x);
				String ymin("%f",cb.cMin.y);
				String xmax("%f",cb.cMax.x);
				String ymax("%f",cb.cMax.y);
				bb.append_attribute("minx") = xmin.c_str();
				bb.append_attribute("miny") = ymin.c_str();
				bb.append_attribute("maxx") = xmax.c_str();
				bb.append_attribute("maxy") = ymax.c_str();
		
			}
		}
	}
}

bool WMSGetCapabilities::needsResponse() const{
	return true;
}


