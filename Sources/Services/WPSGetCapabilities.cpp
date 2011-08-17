#include "headers/toolspch.h"
#include "Engine\Base\DataObjects\ilwisobj.h"
#include "Engine\Base\System\Engine.h"
#include "HttpServer\IlwisServer.h"
#include "HttpServer\mongoose.h"
#include "httpserver\RequestHandler.h"
#include "httpserver\OWSHandler.h"
#include "Services\WPSHandler.h"
#include "Engine\Base\System\module.h"
#include "Engine\Applications\ModuleMap.h"
#include "Engine\Base\DataObjects\XMLDocument.h"
#include "Services\WPSGetCapabilities.h"



using namespace ILWIS;

RequestHandler *WPSGetCapabilities::createHandler(struct mg_connection *c, const struct mg_request_info *request_info, const map<String, String>& kvps, IlwisServer *serv) {
	return new WPSGetCapabilities(c, request_info,kvps, serv);
}

WPSGetCapabilities::WPSGetCapabilities(struct mg_connection *c, const struct mg_request_info *ri, const map<String, String>& kvps, IlwisServer *serv)
: WPSHandler(c,ri,kvps, serv)
{
}

bool WPSGetCapabilities::doCommand() {
	return true;
}

void WPSGetCapabilities::writeResponse(IlwisServer *server) const{
	ILWIS::XMLDocument doc;
	doc.set_name("wps:Capabilities");
	pugi::xml_node capaNode = doc.addNodeTo(doc,"wps:Capabilities");
	createHeader(doc, "http://www.opengis.net/wps/1.0.0 ../wpsGetCapabilities_response.xsd"); 


	pugi::xml_node si = doc.addNodeTo(capaNode,"ows:ServiceIdentification");
	doc.addNodeTo(si,"ows:Title", getConfigValue("WPS:ServiceContext:Title"));
	pugi::xml_node kw = doc.addNodeTo(si,"ows:Keywords");

	Array<String> words;
	String keywords = getConfigValue("WPS:ServiceContext:Keywords");
	Split(keywords,words,";");
	for(int i = 0; i < words.size(); ++i) {
		doc.addNodeTo(kw,"ows:Keyword", words[i]);
	}
	pugi::xml_node sp = doc.addNodeTo(si,"ServiceProvider"); 
	doc.addNodeTo(sp, "ows:ProviderName",getConfigValue("WPS:ServiceContext:ProviderSite"));
	pugi::xml_node contact = doc.addNodeTo(sp,"ows:ServiceContact");
	doc.addNodeTo(contact,"ows:IndividualName",getConfigValue("WPS:ServiceContext:ProviderContactName"));
	doc.addNodeTo(contact,"PositionName",getConfigValue("WPS:ServiceContext:ProviderPosition"));
	pugi::xml_node cInfo = doc.addNodeTo(contact,"ows:ContactInfo");
	pugi::xml_node cPhone = doc.addNodeTo(cInfo,"ows:Phone");
	doc.addNodeTo(cPhone, "ows:Voice",getConfigValue("WPS:ServiceContext:ProviderVoicePhone"));
	pugi::xml_node address = doc.addNodeTo(cInfo,"ows:Address");
	doc.addNodeTo(address,"ows:DeliveryPoint",getConfigValue("WPS:ServiceContext:ProviderDeliveryPoint"));
	doc.addNodeTo(address,"ows:City",getConfigValue("WPS:ServiceContext:City"));
	doc.addNodeTo(address,"ows:AdministrativeArea",getConfigValue("WPS:ServiceContext:ProviderAdministrativeArea"));
	doc.addNodeTo(address,"ows:PostalCode",getConfigValue("WPS:ServiceContext:ProviderPostalCode"));
	doc.addNodeTo(address,"ows:Country",getConfigValue("WPS:ServiceContext::ProviderCountry"));
	doc.addNodeTo(address,"ows:ElectronicMailAddress",getConfigValue("WPS:ServiceContext:ProviderEMail"));


	// further needed for later date
	pugi::xml_node meta = doc.addNodeTo(si,"ows:OperationsMetadata");
	pugi::xml_node oper = doc.addNodeTo(meta,"ows:Operation");
	oper.append_attribute("name") = "GetCapabilities";
	pugi::xml_node dcp = doc.addNodeTo(oper, "ows:DCP");
	pugi::xml_node http = doc.addNodeTo(dcp,"ows:HTTP");
	String serv = getConfigValue("WPS:ServiceContext:GetCapabilities") + "?";
	doc.addNodeTo(http,"ows:Get").append_attribute("xlink:href") = serv.c_str();
		
	oper = doc.addNodeTo(meta,"ows:Operation");
	oper.append_attribute("name") = "DescribeProcess";
	dcp = doc.addNodeTo(oper, "ows:DCP");
	http = doc.addNodeTo(dcp,"ows:HTTP");
	serv = getConfigValue("WPS:ServiceContext:DescribeProcess") + "?";
	doc.addNodeTo(http,"ows:Get").append_attribute("xlink:href") = serv.c_str();

	oper = doc.addNodeTo(meta,"ows:Operation");
	oper.append_attribute("name") = "ExecuteProcess";
	dcp = doc.addNodeTo(oper, "ows:DCP");
	http = doc.addNodeTo(dcp,"ows:HTTP");
	serv = getConfigValue("WPS:ServiceContext:ExecuteProcess") + "?";
	doc.addNodeTo(http,"ows:Get").append_attribute("xlink:href") = serv.c_str();

	pugi::xml_node offerings = doc.addNodeTo(si, "wps:ProcessOfferings");

	vector<CommandInfo *> infos;
	Engine::modules.getCommandInfo("*", infos);
	for(vector<CommandInfo *>::iterator cur=infos.begin(); cur != infos.end(); ++cur) {
		CommandInfo *info = (*cur);
		if ( info->metadata != NULL) {
			ApplicationQueryData query;
			query.queryType = "WPSMETADATA";
			ApplicationMetadata amd = (info->metadata)(&query);
			if ( amd.wpsxml != "") {
				pugi::xml_node process = doc.addNodeTo(offerings,"wps:Process");


				String xml = amd.wpsxml;
				vector<String> results;
				ILWIS::XMLDocument xmldoc(xml);
				xmldoc.addNameSpace("ows","http://www.opengis.net/ows/1.1");
				xmldoc.addNameSpace("wps","http://www.opengis.net/wps/1.0.0");
				String tx = xmldoc.toString();
				xmldoc.executeXPathExpression("//wps:ProcessDescription/ows:Identifier/text()",results);
				if ( results.size() > 0) {
					doc.addNodeTo(process,"ows:Identifier",results[0]);
				}
				results.clear();
				xmldoc.executeXPathExpression("//wps:ProcessDescription/ows:Title/text()",results);
				if ( results.size() > 0) {
					doc.addNodeTo(process,"ows:Ttile",results[0]);
				}
				results.clear();
				xmldoc.executeXPathExpression("//wps:ProcessDescription/ows:Abstract/text()",results);
				if ( results.size() > 0) {
					doc.addNodeTo(process,"ows:Abstract",results[0]);
				}
			}
		}
	}
	String txt = doc.toString();
	char *buf = new char[txt.size() + 1];
	memset(buf,0,txt.size() + 1);
	memcpy(buf,txt.c_str(), txt.size());
	mg_write(getConnection(), buf, txt.size()+1);
	delete [] buf;
}

bool WPSGetCapabilities::needsResponse() const{
	return true;
}



