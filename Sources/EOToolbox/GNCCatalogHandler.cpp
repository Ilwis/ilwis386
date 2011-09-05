#include "headers/toolspch.h"
#include "Engine\Base\DataObjects\ilwisobj.h"
#include "Engine\Base\System\Engine.h"
#include "HttpServer\IlwisServer.h"
#include "HttpServer\mongoose.h"
#include "httpserver\RequestHandler.h"
#include "GNCCatalogHandler.h"
#include "Engine\Base\System\module.h"
#include "Engine\Applications\ModuleMap.h"
#include "Engine\Base\DataObjects\URL.h"
#include "Engine\Base\DataObjects\Downloader.h"
#include "Engine\Base\DataObjects\XMLDocument.h"
#include "EOToolbox\EOtoolbox.h"
#include <regex> 


using namespace ILWIS;
using namespace std::tr1;

RequestHandler *GNCCatalogHandler::createHandler(struct mg_connection *c, const struct mg_request_info *request_info, const map<String, String>& kvps, IlwisServer *serv) {
	return new GNCCatalogHandler(c, request_info,kvps, serv);
}

GNCCatalogHandler::GNCCatalogHandler(struct mg_connection *c, const struct mg_request_info *ri, const map<String, String>& _kvps, IlwisServer *serv)
: RequestHandler("GNCCatalogHandler", c,ri,_kvps, serv)
{
	timeFormats["yyyy"] ="((19|20)\\d\\d)"; 
	timeFormats["yyyymm"] = timeFormats["yyyy"] + "(0[1-9]|1[012])";
	timeFormats["yyyymmdd"] = timeFormats["yyyymm"] + "(0[1-9]|[12][0-9]|3[01])";
	timeFormats["yyyymmddhhmm"] = timeFormats["yyyymmdd"] + "((([0-1]?[0-9])|([2][0-3]))([0-5]?[0-9]))";
	timeFormats["yyyymmdecdec"] = timeFormats["yyyymm"] + "([0-3]\\d)";
	timeFormats["yyyymm-dec"] = timeFormats["yyyymm"] + "{-}[0-3]\\d";

	config.add("Extensions\\EO-toolbox", "EORequestHandler");

}

bool GNCCatalogHandler::doCommand() {
	FileName fnConfig(getConfigValue("GNC:ServiceContext:GNCConfigFile"));
	EOToolbox eoTool;
	if ( fnConfig.fExist()) {
		eoTool.ReadConfigFile(fnConfig);
		map<String, String>::const_iterator iter = kvps.find("product");
		if ( iter != kvps.end()) {
			String product = (*iter).second;
			//int index = product.find_last_of(":");
			//product = product.substr(index + 1, product.size() - index);
			FormatInfo info = eoTool.get(product);
			String timeformat = info.format;
			String folderIn = info.fnInput.sPath();
			String regex = makeRegEx(info.filePattern);
			addFolder(folderIn,regex, timeformat,gncInfo);

		}
	} else {
		return false;
	}
	return true;

}

String GNCCatalogHandler::makeRegEx(const String& pat) {
	String result;
	for(int i = 0; i < pat.size(); ++i) {
		char c = pat[i];
		if ( i ==0 && c == '*')
			result += "^.*";
		else if ( i == pat.size() - 1 &&c == '*' )
			result += ".*$";
		else if ( c == '*')
			result += "(*)+";
		else
			result += c;
	}

	return result;
}

void GNCCatalogHandler::writeResponse() const{
	ILWIS::XMLDocument doc;
	pugi::xml_node first = doc.addNodeTo(doc,"GNC_Catalog");
	first.append_attribute("Name") = getConfigValue("Server:Context:Name").c_str();
	first.append_attribute("URL") = String("%S/gnc",getConfigValue("Server:Context:Name")).c_str();
	for(int i=0; i < gncInfo.size(); ++i) {
		GNCFileInfo fi = gncInfo[i];
		pugi::xml_node node = doc.addNodeTo(first,"File");
		doc.addNodeTo(node,"Filename",fi.fn.sFile + fi.fn.sExt);
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

void GNCCatalogHandler::collectCatalog(const String& folder, const String& format, vector<GNCFileInfo>& results){


}

void GNCCatalogHandler::addFolder(const String& folder, const String& nameFormat,const String& timeFormat, vector<GNCFileInfo>& results) {
	CFileFind finder;
	String pattern = folder + "\\*.*";
	BOOL fFound = finder.FindFile(pattern.c_str());
	while(fFound) {
		fFound = finder.FindNextFile();
		if (!finder.IsDirectory())
		{
			FileName fnNew (finder.GetFilePath());
			addFile(fnNew, nameFormat, timeFormat, results);
		} else {
			FileName fnNew (finder.GetFilePath());
			if ( fnNew.sFile != "." && fnNew.sFile != ".." && fnNew.sFile != "")
				addFolder(String(fnNew.sFullPath()), nameFormat, timeFormat, results);
		}
	}
}

void GNCCatalogHandler::addFile(const FileName& fn, const String& nameFrmt, const String& timeFormat, vector<GNCFileInfo>& results) {
	String file = fn.sFile;
	String format = nameFrmt;
	file.toLower();
	format.toLower();
	regex namePat(format.c_str());
	match_results<std::string::const_iterator> nameMatches;
	bool valid = regex_search(file,nameMatches,namePat);
	if ( !valid)
		return;
		

	String exp = timeFormats[timeFormat];
	//String exp = "^.*(((19|20)\\d\\d(0[1-9]|1[012])(0[1-9]|[12][0-9]|3[01]))((([0-1]?[0-9])|([2][0-3]))([0-5]?[0-9]))).*$";
	if ( exp == "")
		return;
	regex pattern(exp.c_str());
	match_results<std::string::const_iterator> matches;
	valid = regex_search(file,matches,pattern);
	if ( valid) {
		int pos = matches.position(0);
		String p1 = file.substr(pos,timeFormat.size());
		CFileStatus st;
		CFile::GetStatus(fn.sFullPath().c_str(), st);
		GNCFileInfo inf;
		inf.fn = fn;
		inf.imageTime = ILWIS::Time(p1);
		inf.size = st.m_size;
		results.push_back(inf);
	}


}

bool GNCCatalogHandler::needsResponse() const {
	return true;
}
