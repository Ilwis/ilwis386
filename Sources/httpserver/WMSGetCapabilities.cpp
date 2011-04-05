#include "headers/toolspch.h"
#include "Engine\Base\DataObjects\ilwisobj.h"
#include "Engine\Base\System\Engine.h"
#include "HttpServer\command.h"
#include "HttpServer\mongoose.h"
#include "httpserver\RequestHandler.h"
#include "httpserver\WMSGetCapabilities.h"
#include "Engine\Base\System\module.h"
#include "Engine\Applications\ModuleMap.h"
#include "httpserver\XMLDocument.h"
#include "Engine\Map\basemap.h"
#include <xercesc\dom\DOMLSSerializer.hpp>


using namespace ILWIS;
using namespace XERCES_CPP_NAMESPACE;

WMSGetCapabilities::WMSGetCapabilities(struct mg_connection *c, const struct mg_request_info *ri, const map<String, String>& kvps)
: RequestHandler(c,ri,kvps)
{
}

void WMSGetCapabilities::writeResponse(IlwisServer *server) const{
	wchar_t result[250];
	XMLPlatformUtils::Initialize();
	DOMImplementation* dom = DOMImplementationRegistry::getDOMImplementation(XMLString::transcode("core"));
	XERCES_CPP_NAMESPACE::DOMDocument *doc = dom->createDocument(0, L"WMT_MS_Capabilities", 0);

	//createWPSHeader(doc, "WMSGetCapabilities_response.xsd");
	XERCES_CPP_NAMESPACE::DOMElement *root = doc->getDocumentElement();

	XERCES_CPP_NAMESPACE::DOMElement *node1,*node2,*node3, *node4, *node5, *node6, *node7;

	node1 = doc->createElement(L"Service");
	root->appendChild(node1);
	node2 = createTextNode(doc,"Name","WMS");
	node1->appendChild(node2);
	node2 = createTextNode(doc,"Title",getConfigValue("WMS:ServiceIdentification:Title"));
	node1->appendChild(node2);
	node2 = createTextNode(doc,"Abstract",getConfigValue("WMS:ServiceIdentification:Abstract"));
	node1->appendChild(node2);
	node2 = doc->createElement(L"Keywords");
	Array<String> words;
	String keywords = getConfigValue("WMS:ServiceIdentification:Keywords");
	Split(keywords,words,";");
	for(int i = 0; i < words.size(); ++i) {
		node3 = createTextNode(doc,"Keyword",words[i]);
		node2->appendChild(node3);
	}
	node1 = doc->createElement(L"ContactInformation");
	root->appendChild(node1);
	node2 = doc->createElement(L"ContactPersonPrimary");
	node1->appendChild(node2);

	node3 = createTextNode(doc,"ContactPerson",getConfigValue("WMS:Provider:ContactName"));
	node2->appendChild(node3);
	node3 = createTextNode(doc,"ContactOrganization",getConfigValue("WMS:Provider:Name"));
	node2->appendChild(node3);

	node2 = createTextNode(doc,"ContactElectronicMailAddress",getConfigValue("WMS:Provider:EMail"));
	node1->appendChild(node2);

	node1 = doc->createElement(L"Capability");
	root->appendChild(node1);
	node2 = doc->createElement(L"Request");
	node1->appendChild(node2);
	node3 = doc->createElement(L"GetCapabilities");
	node2->appendChild(node3);
	node4 = createTextNode(doc,"Format","text/xml");
	node3->appendChild(node4);
	node4 = doc->createElement(L"DCPType");
	node3->appendChild(node4);
	node5 = doc->createElement(L"HTTP");
	node4->appendChild(node5);
	node6 = doc->createElement(L"Get");
	node5->appendChild(node6);
	node7 = doc->createElement(L"OnlineResource");
	node6->appendChild(node7);
	node7->setAttribute(L"xmlns:xlink",L"http://www.w3.org/1999/xlink");
	node7->setAttribute(L"xlink:type",L"simple");
	node7->setAttribute(L"xlink:href",getConfigValue("WMS:OperationMetadata:GetCapabilities").toWChar(result));

	node3 = doc->createElement(L"GetMap");
	node2->appendChild(node3);
	node4 = createTextNode(doc,"Format","image/jpg");
	node3->appendChild(node4);
	node4 = doc->createElement(L"DCPType");
	node3->appendChild(node4);
	node5 = doc->createElement(L"HTTP");
	node4->appendChild(node5);
	node6 = doc->createElement(L"Get");
	node5->appendChild(node6);
	node7 = doc->createElement(L"OnlineResource");
	node6->appendChild(node7);
	node7->setAttribute(L"xmlns:xlink",L"http://www.w3.org/1999/xlink");
	node7->setAttribute(L"xlink:type",L"simple");
	node7->setAttribute(L"xlink:href",getConfigValue("WMS:OperationMetadata:GetMap").toWChar(result));

	node2 = doc->createElement(L"Exception");
	node1->appendChild(node2);
	node2 = createTextNode(doc,"Format","XML");
	node1->appendChild(node2);

	int n = getConfigValue("WMS:ServiceContext:NumberOfCatalogs").iVal();
	for(int i=0; i < n; ++i) {
		String catalog("WMS:%S", getConfigValue(String("WMS:ServiceContext:Catalog%d",i)));
		node2 = doc->createElement(L"Layer");
		node1->appendChild(node2);
		String location = getConfigValue(String("%S:Location", catalog));
		String filter = getConfigValue(String("%S:Filter", catalog));
		String title = getConfigValue(String("%S:Title", catalog));
		node3 = createTextNode(doc,"Title",title);
		node2->appendChild(node3);
		list<FileName> files;
		if ( filter != sUNDEF) {
			handleFilteredCatalog(location,filter, false, files);
			for(list<FileName>::iterator cur=files.begin(); cur != files.end(); ++cur) {
				handleFile(node3, doc, *cur);
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


void WMSGetCapabilities::handleFilteredCatalog(const String& location,const String& filter, bool recursive, list<FileName>& files) const{
	CFileFind finder;
	String pattern = location + "\\" + filter;
	BOOL fFound = finder.FindFile(pattern.scVal());
	while(fFound) {
		fFound = finder.FindNextFile();
		if (!finder.IsDirectory())
		{
			FileName fnNew(finder.GetFilePath());
			files.push_back(fnNew);
		}
	}
}

void WMSGetCapabilities::handleFile(XERCES_CPP_NAMESPACE::DOMElement *node, XERCES_CPP_NAMESPACE::DOMDocument *doc, const FileName& fn) const{
	wchar_t result[250];
	IlwisObject ob = IlwisObject::obj(fn);
	if ( !ob.fValid())
		return;
	if ( fn.sExt == ".mpr" || fn.sExt == ".mpp" || fn.sExt == ".mpa" || fn.sExt == ".mps") {
		XERCES_CPP_NAMESPACE::DOMElement *node1,*node2;
		BaseMap bmp(fn);
		String name = fn.sFile;
		String desc = bmp->sDescr();
		if ( desc == "")
			desc = fn.sFile + fn.sExt;
		String id = bmp->cs()->getIdentification();
		if ( id != sUNDEF) {
			node1 = doc->createElement(L"Layer");
			node->appendChild(node1);
			node2 = createTextNode(doc,"Name",name);
			node1->appendChild(node2);
			node2 = createTextNode(doc, "Title", desc);
			node1->appendChild(node2);
			CoordBounds cb = bmp->cb();
			if ( bmp->cs()->fLatLon2Coord()) {
				LatLon llMin = bmp->cs()->llConv(cb.cMin);
				LatLon llMax = bmp->cs()->llConv(cb.cMax);
				if ( !(llMin.fUndef() || llMax.fUndef())) {
					node2 = createTextNode(doc, "SRS", "4326");
					node1->appendChild(node2);
					node2 = doc->createElement(L"LatLonBoundingBox");
					node1->appendChild(node2);
					String latmin("%f",llMin.Lat);
					String lonmin("%f",llMin.Lon);
					String latmax("%f",llMax.Lat);
					String lonmax("%f",llMax.Lon);
					node2->setAttribute(L"minx",lonmin.toWChar(result));
					node2->setAttribute(L"miny",latmin.toWChar(result));
					node2->setAttribute(L"maxx",lonmax.toWChar(result));
					node2->setAttribute(L"maxy",latmax.toWChar(result));
				} 
			}
			if ( bmp->cs()->pcsViaLatLon() && !bmp->cs()->pcsLatLon()) {
				String id = bmp->cs()->getIdentification();
				if ( id.find("epsg") != string::npos)
					id = id.sTail("=");
				node2 = createTextNode(doc, "SRS", id);
				node1->appendChild(node2);
				node2 = doc->createElement(L"BoundingBox");
				node1->appendChild(node2);
				String epsg("%d",id);
				node2->setAttribute(L"SRS",epsg.toWChar(result));
				String xmin("%f",cb.cMin.x);
				String ymin("%f",cb.cMin.y);
				String xmax("%f",cb.cMax.x);
				String ymax("%f",cb.cMax.y);
				node2->setAttribute(L"minx",xmin.toWChar(result));
				node2->setAttribute(L"miny",ymin.toWChar(result));
				node2->setAttribute(L"maxx",xmax.toWChar(result));
				node2->setAttribute(L"maxy",ymax.toWChar(result));
			}
		}
	}
}


bool WMSGetCapabilities::needsResponse() const{
	return true;
}


