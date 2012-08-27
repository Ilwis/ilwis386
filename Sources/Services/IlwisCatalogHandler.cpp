#include "headers/toolspch.h"
#include "Engine\Base\DataObjects\ilwisobj.h"
#include "Engine\Base\System\Engine.h"
#include "HttpServer\IlwisServer.h"
#include "HttpServer\mongoose.h"
#include "httpserver\RequestHandler.h"
#include "IlwisCatalogHandler.h"
#include "Engine\Base\System\module.h"
#include "Engine\Applications\ModuleMap.h"
#include "Engine\Base\DataObjects\URL.h"
#include "Engine\Base\DataObjects\Downloader.h"
#include "Engine\Base\DataObjects\XMLDocument.h"
#include <regex> 


using namespace ILWIS;
using namespace std::tr1;

RequestHandler *IlwisCatalogHandler::createHandler(struct mg_connection *c, const struct mg_request_info *request_info, const map<String, String>& kvps, IlwisServer *serv) {
	return new IlwisCatalogHandler(c, request_info,kvps, serv);
}

IlwisCatalogHandler::IlwisCatalogHandler(struct mg_connection *c, const struct mg_request_info *ri, const map<String, String>& _kvps, IlwisServer *serv)
: RequestHandler("IlwisCatalogHandler", c,ri,_kvps, serv)
{
	config.add("Services", "CatalogHandler");

}

bool IlwisCatalogHandler::doCommand() {
	String rootData = getConfigValue("Catalog:ServiceContext:IlwisLocalRoot");
	String filter = kvps["filter"];
	String catalog = kvps["catalog"];
	String extList = makeExtensionList(filter);
	CFileFind finder;
	String path = rootData + "\\";
	if ( catalog.size() > 0)
		path += catalog + "\\";
	path += extList;
	BOOL fFound = finder.FindFile(path.c_str());
	while(fFound) {
		fFound = finder.FindNextFile();
		if (!finder.IsDirectory())
		{
			FileName fnNew (finder.GetFilePath());
			addFile(fnNew);
		}
	}   
    return true;

}

String IlwisCatalogHandler::makeExtensionList(const String& ext) {
	Array<String> parts;
	Split(ext, parts,",");
	String result;
	for(int i = 0; i < parts.size(); ++i) {
		if ( i > 0)
			result += ",";
		if( parts[i] == "rastermap" || parts[i] == "image/ilwisraster")
			result += "*.mpr";
		if( parts[i] == "maplist" || parts[i] == "image/ilwismaplist")
			result += "*.mpl";
		if( parts[i] == "polygonmap" || parts[i] == "image/ilwispolygon")
			result += "*.mpa";
		if( parts[i] == "segmentmap" || parts[i] == "image/ilwisline")
			result += "*.mps";
		if( parts[i] == "pointmap" || parts[i] == "image/ilwispoint")
			result += "*.mpp";
		if( parts[i] == "coordinatesystem")
			result += "*.csy";
		if( parts[i] == "georeference")
			result += "*.grf";
		if( parts[i] == "table" || parts[i] == "binary/ilwistable")
			result += "*.tbl";
		if( parts[i] == "domain")
			result += "*.dom";

	}
	return result;
}


void IlwisCatalogHandler::writeResponse() const{
	ILWIS::XMLDocument doc;
	pugi::xml_node first = doc.addNodeTo(doc,"Ilwis_Catalog");
	first.append_attribute("Name") = getConfigValue("Server:Context:Name").c_str();
	first.append_attribute("URL") = String("%S/gnc",getConfigValue("Server:Context:Name")).c_str();
	for(int i=0; i < results.size(); ++i) {
		FileInfo fi = results[i];
		pugi::xml_node node = doc.addNodeTo(first,"File");
		doc.addNodeTo(node,"Filename",fi.fn);
		doc.addNodeTo(node,"ImageTime",fi.imageTime.toString());
		doc.addNodeTo(node,"FileSize",String("%d",fi.size));
	}

	String txt = doc.toString();
	char *buf = new char[txt.size() + 1];
	memset(buf,0,txt.size() + 1);
	memcpy(buf,txt.c_str(), txt.size());
	mg_write(getConnection(), buf, txt.size()+1);
	delete [] buf;
}

void IlwisCatalogHandler::addFile(const FileName& fn) {
	String file = fn.sFile;
	file.toLower();
	CFileStatus st;
	CFile::GetStatus(fn.sFullPath().c_str(), st);
	FileInfo inf;
	inf.fn = fn.sFile + fn.sExt;
	inf.imageTime = ILWIS::Time(st.m_atime);
	inf.size = st.m_size;
	results.push_back(inf);
}

bool IlwisCatalogHandler::needsResponse() const {
	return true;
}
