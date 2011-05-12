#include "headers/toolspch.h"
#include "Engine\Base\DataObjects\ilwisobj.h"
#include "Engine\Base\System\Engine.h"
#include "HttpServer\command.h"
#include "HttpServer\mongoose.h"
#include "httpserver\RequestHandler.h"
#include "httpserver\WPSHandler.h"
#include "httpserver\WPSGetCapabilities.h"
#include "Engine\Base\System\module.h"
#include "Engine\Applications\ModuleMap.h"
#include "httpserver\XMLDocument.h"
#include <xqilla/xqilla-dom3.hpp>
#include <xqilla\simple-api\XQQuery.hpp>
#include <xqilla\simple-api\XQilla.hpp>
#include <xqilla/context/DynamicContext.hpp>
#include <xercesc\dom\DOMLSSerializer.hpp>


using namespace ILWIS;
using namespace XERCES_CPP_NAMESPACE;
#define TEXT_SIZE 100000


WPSGetCapabilities::WPSGetCapabilities(struct mg_connection *c, const struct mg_request_info *ri, const map<String, String>& kvps)
: WPSHandler(c,ri,kvps)
{
}

void WPSGetCapabilities::writeResponse(IlwisServer *server) const{
	try{
	wchar_t result[TEXT_SIZE];
	//XMLPlatformUtils::Initialize();
	XQilla *xqilla = new XQilla();
	DynamicContext *context = xqilla->createContext(XQilla::XSLT2_FULLTEXT);
	String urlXSLT("file:///%SResources/wps_ilwis_getcapabilities.xsl",getEngine()->getContext()->sIlwDir());
	XQQuery *query = xqilla->parseFromURI(urlXSLT.toWChar(result),context);
	String urlXML("file:///%SResources/operations_rastermap_and_table_applications.xml",getEngine()->getContext()->sIlwDir());
	Sequence seq = context->resolveDocument(urlXML.toWChar(result));
    if(!seq.isEmpty() && seq.first()->isNode()) {
            context->setContextItem(seq.first());
            context->setContextPosition(1);
            context->setContextSize(1);
    }

	Result qresult = query->execute(context);
	Item::Ptr item;
	String txt;
    while(item = qresult->next(context)) {
			txt +=  UTF8(item->asString(context));
    }
	XMLDocument doc(txt);
	txt = doc.toString();
	char *buf = new char[txt.size() + 1];
	memset(buf,0,txt.size() + 1);
	memcpy(buf,txt.scVal(), txt.size());
	mg_write(getConnection(), buf, txt.size()+1);
	delete [] buf;
	delete xqilla;
	//delete context;
	//delete query; // wil also delete the context

	} catch (XQillaException& e) {
		CString s(e.getMessage());
		throw ErrorObject(String(s));
	}

	
}

void WPSGetCapabilities::writeResponse2(IlwisServer *server) const{
	wchar_t result[TEXT_SIZE];
	DOMImplementation* dom = DOMImplementationRegistry::getDOMImplementation(XMLString::transcode("core"));
	XERCES_CPP_NAMESPACE::DOMDocument *doc = dom->createDocument(0, L"GetCapabilities", 0);

	createHeader(doc, "wpsGetCapabilities_response.xsd");
	XERCES_CPP_NAMESPACE::DOMElement *root = doc->getDocumentElement();

	XERCES_CPP_NAMESPACE::DOMElement *node1,*node2,*node3, *node4, *node5;

	node1 = doc->createElement(L"ows:ServiceIdentification");
	root->appendChild(node1);
	node2 = createTextNode(doc,"ows:Title",getConfigValue("WPS:ServiceIdentification:Title"));
	node1->appendChild(node2);
	node2 = createTextNode(doc,"ows:Abstract",getConfigValue("WPS:ServiceIdentification:Abstract"));
	node1->appendChild(node2);
	node2 = doc->createElement(L"ows:Keywords");
	Array<String> words;
	String keywords = getConfigValue("WPS:ServiceIdentification:Keywords");
	Split(keywords,words,";");
	for(int i = 0; i < words.size(); ++i) {
		node3 = createTextNode(doc,"ows:Keyword",words[i]);
		node2->appendChild(node3);
	}
	node1 = doc->createElement(L"ServiceProvider");
	root->appendChild(node1);
	node2 = createTextNode(doc,"ows:ProviderName",getConfigValue("WPS:Provider:Name"));
	node1->appendChild(node2);
	node2 = createTextNode(doc,"ows:ProviderName",getConfigValue("WPS:Provider:Site"));
	node1->appendChild(node2);
	node2 = doc->createElement(L"ows:ServiceContact");
	node1->appendChild(node2);
	node3 = createTextNode(doc,"ows:IndividualName",getConfigValue("WPS:Provider:ContactName"));
	node2->appendChild(node3);
	node3 = createTextNode(doc,"PositionName",getConfigValue("WPS:Provider:Position"));
	node2->appendChild(node3);
	node3 = doc->createElement(L"ows:ContactInfo");
	node2->appendChild(node3);
	node4 = doc->createElement(L"ows:Phone");
	node3->appendChild(node4);
	node5 = createTextNode(doc,"ows:Voice",getConfigValue("WPS:Provider:VoicePhone"));
	node4->appendChild(node5);
	// further needed for later date

	node1 = doc->createElement(L"ows:OperationsMetadata");
	root->appendChild(node1);
	node2 = doc->createElement(L"ows:Operation");
	node1->appendChild(node2);
	node2->setAttribute(L"name",L"GetCapabilities");
	node3 = doc->createElement(L"ows:DCP");
	node2->appendChild(node3);
	node4 = doc->createElement(L"ows:Get");
	node3->appendChild(node4);
	String link("xlink:%S?",getConfigValue("WPS:OperationMetadata:GetCapabilities"));
	node4->setAttribute(L"name", link.toWChar(result));

	node2 = doc->createElement(L"ows:Operation");
	node1->appendChild(node2);
	node2->setAttribute(L"name",L"DescribeProcess");
	node3 = doc->createElement(L"ows:DCP");
	node2->appendChild(node3);
	node4 = doc->createElement(L"ows:Get");
	node3->appendChild(node4);
	link = String("xlink:%S?",getConfigValue("WPS:OperationMetadata:DescribeProcess"));
	node4->setAttribute(L"name", link.toWChar(result));

	node2 = doc->createElement(L"ows:Operation");
	node1->appendChild(node2);
	node2->setAttribute(L"name",L"Execute");
	node3 = doc->createElement(L"ows:DCP");
	node2->appendChild(node3);
	node4 = doc->createElement(L"ows:Get");
	node3->appendChild(node4);
	link = String("xlink:%S?",getConfigValue("WPS:OperationMetadata:Execute"));
	node4->setAttribute(L"name", link.toWChar(result));

	node2 = doc->createElement(L"wps:ProcessOfferings");
	node1->appendChild(node2);
	vector<CommandInfo *> infos;
	Engine::modules.getCommandInfo("*", infos);
	for(vector<CommandInfo *>::iterator cur=infos.begin(); cur != infos.end(); ++cur) {
		CommandInfo *info = (*cur);
		if ( info->metadata != NULL) {
			ApplicationQueryData query;
			query.queryType = "WPSMETADATA";
			ApplicationMetadata amd = (info->metadata)(&query);
			if ( amd.wpsxml != "") {
				node3 = doc->createElement(L"wps:Process");
				node2->appendChild(node3);
				String xml = amd.wpsxml;
				vector<String> results;
				ILWIS::XMLDocument xmldoc(xml);
				xmldoc.addNameSpace("ows","http://www.opengis.net/ows/1.1");
				xmldoc.addNameSpace("wps","http://www.opengis.net/wps/1.0.0");
				xmldoc.executeXPathExpression("//ProcessDescription/ows:Identifier/text()",results);
				if ( results.size() > 0) {
					node4 = createTextNode(doc,"ows:Identifier",results[0]);
					node3->appendChild(node4);
				}
				results.clear();
				xmldoc.executeXPathExpression("//ProcessDescription/ows:Title/text()",results);
				if ( results.size() > 0) {
					node4 = createTextNode(doc,"ows:Title",results[0]);
					node3->appendChild(node4);
				}
				results.clear();
				xmldoc.executeXPathExpression("//ProcessDescription/ows:Abstract/text()",results);
				if ( results.size() > 0) {
					node4 = createTextNode(doc,"ows:Abstract",results[0]);
					node3->appendChild(node4);
				}
			}
		}
	}
	String txt = createOutput(doc);
	char *buf = new char[txt.size() + 1];
	memset(buf,0,txt.size() + 1);
	memcpy(buf,txt.scVal(), txt.size());
	mg_write(getConnection(), buf, txt.size()+1);
	delete [] buf;
}

bool WPSGetCapabilities::needsResponse() const{
	return true;
}



