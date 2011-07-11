#include "headers/toolspch.h"
#include "Engine\Base\DataObjects\ilwisobj.h"
#include "Engine\Base\System\Engine.h"
#include "HttpServer\command.h"
#include "HttpServer\mongoose.h"
#include "httpserver\RequestHandler.h"
#include "httpserver\OWSHandler.h"
#include "httpserver\WPSHandler.h"
#include "Engine\Base\System\module.h"
#include "Engine\Applications\ModuleMap.h"
#include "Engine\Base\DataObjects\XMLDocument.h"
#include "httpserver\WPSGetCapabilities.h"



using namespace ILWIS;


WPSGetCapabilities::WPSGetCapabilities(struct mg_connection *c, const struct mg_request_info *ri, const map<String, String>& kvps)
: WPSHandler(c,ri,kvps)
{
}

bool WPSGetCapabilities::doCommand() {
	return true;
}

void WPSGetCapabilities::writeResponse(IlwisServer *server) const{
	ILWIS::XMLDocument doc;
	doc.set_name("wps:Capabilities");
	createHeader(doc, "http://www.opengis.net/wps/1.0.0 ../wpsGetCapabilities_response.xsd"); 


	pugi::xml_node si = doc.addNodeTo(doc,"ows:ServiceIdentification");
	doc.addNodeTo(si,"ows:Title", getConfigValue("WPS:ServiceIdentification:Title"));
	pugi::xml_node kw = doc.addNodeTo(si,"ows:Keywords");

	Array<String> words;
	String keywords = getConfigValue("WPS:ServiceIdentification:Keywords");
	Split(keywords,words,";");
	for(int i = 0; i < words.size(); ++i) {
		doc.addNodeTo(kw,"ows:Keyword", words[i]);
	}
	pugi::xml_node sp = doc.addNodeTo(si,"ServiceProvider"); 
	doc.addNodeTo(sp, "ows:ProviderName",getConfigValue("WPS:Provider:Site"));
	pugi::xml_node contact = doc.addNodeTo(sp,"ows:ServiceContact");
	doc.addNodeTo(contact,"ows:IndividualName",getConfigValue("WPS:Provider:ContactName"));
	doc.addNodeTo(contact,"PositionName",getConfigValue("WPS:Provider:Position"));
	pugi::xml_node cInfo = doc.addNodeTo(contact,"ows:ContactInfo");
	pugi::xml_node cPhone = doc.addNodeTo(cInfo,"ows:Phone");
	doc.addNodeTo(cPhone, "ows:Voice",getConfigValue("WPS:Provider:VoicePhone"));
	pugi::xml_node address = doc.addNodeTo(cInfo,"ows:Address");
	doc.addNodeTo(address,"ows:DeliveryPoint",getConfigValue("WPS:Provider:DeliveryPoint"));
	doc.addNodeTo(address,"ows:City",getConfigValue("WPS:Provider:City"));
	doc.addNodeTo(address,"ows:AdministrativeArea",getConfigValue("WPS:Provider:AdministrativeArea"));
	doc.addNodeTo(address,"ows:PostalCode",getConfigValue("WPS:Provider:PostalCode"));
	doc.addNodeTo(address,"ows:Country",getConfigValue("WPS:Provider::Country"));
	doc.addNodeTo(address,"ows:ElectronicMailAddress",getConfigValue("WPS:Provider::EMail"));


	// further needed for later date
	pugi::xml_node meta = doc.addNodeTo(si,"ows:OperationsMetadata");
	pugi::xml_node oper = doc.addNodeTo(meta,"ows:Operation");
	oper.append_attribute("name") = "GetCapabilities";
	pugi::xml_node dcp = doc.addNodeTo(oper, "ows:DCP");
	pugi::xml_node http = doc.addNodeTo(dcp,"ows:HTTP");
	doc.addNodeTo(http,"ows:Get").append_attribute("xlink:href") = getConfigValue("WPS:OperationMetadata:GetCapabilities").scVal();
		
	oper = doc.addNodeTo(meta,"ows:Operation");
	oper.append_attribute("name") = "DescribeProcess";
	dcp = doc.addNodeTo(oper, "ows:DCP");
	http = doc.addNodeTo(dcp,"ows:HTTP");
	doc.addNodeTo(http,"ows:Get").append_attribute("xlink:href") = getConfigValue("WPS:OperationMetadata:DescribeProcess").scVal();

	oper = doc.addNodeTo(meta,"ows:Operation");
	oper.append_attribute("name") = "Execute";
	dcp = doc.addNodeTo(oper, "ows:DCP");
	http = doc.addNodeTo(dcp,"ows:HTTP");
	doc.addNodeTo(http,"ows:Get").append_attribute("xlink:href") = getConfigValue("WPS:OperationMetadata:Execute").scVal();

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
	memcpy(buf,txt.scVal(), txt.size());
	mg_write(getConnection(), buf, txt.size()+1);
	delete [] buf;
}

bool WPSGetCapabilities::needsResponse() const{
	return true;
}



