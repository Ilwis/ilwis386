#include "Client\Headers\formelementspch.h"
#include "Client\ilwis.h"
#include "Engine\base\system\engine.h"
#include <map>
#include "Client\Mapwindow\Drawers\SimpleDrawer.h"
#include "Client\Mapwindow\Drawers\ComplexDrawer.h"
#include "Client\Mapwindow\Drawers\SVGLoader.h"
#include "Client\Mapwindow\Drawers\SVGElements.h"


using namespace ILWIS;
using namespace XERCES_CPP_NAMESPACE;

//-----------------------------
SVGLoader::SVGLoader() {
}
SVGLoader::~SVGLoader() {
}
void SVGLoader::load() {
	String ilwDir = getEngine()->getContext()->sIlwDir();
	String pathToSvg = ilwDir + "\\Resources\\symbols\\*.svg";
	XERCES_CPP_NAMESPACE::XMLPlatformUtils::Initialize();
	try{

		CFileFind finder;
		BOOL fFound = finder.FindFile(pathToSvg.scVal());
		while(fFound) {
			fFound = finder.FindNextFile();
			FileName fnSvg(finder.GetFilePath());
			parseFile(fnSvg);
		}
	XMLPlatformUtils::Terminate();
	}catch( xercesc::XMLException& e ) {
		throw ErrorObject(CString(e.getMessage()));
	}

}

void SVGLoader::parseFile(const FileName& fn) {
	XercesDOMParser *parser = new XercesDOMParser;
	parser->setValidationScheme(XercesDOMParser::Val_Auto);
	parser->setDoNamespaces(false);
	parser->setDoSchema(false);
	parser->setValidationSchemaFullChecking(false);
	parser->setCreateEntityReferenceNodes(false);

	parser->parse(fn.sFullPath().scVal());
	XERCES_CPP_NAMESPACE::DOMDocument* xmlDoc = parser->getDocument();
	XERCES_CPP_NAMESPACE::DOMElement* elementRoot = xmlDoc->getDocumentElement();
	String rootName = CString(elementRoot->getNodeName());
	XERCES_CPP_NAMESPACE::DOMNodeList* children = elementRoot->getChildNodes();
	for(int i = 0; i < children->getLength(); ++i) {
		DOMNode* node = children->item(i);
		DOMNamedNodeMap *map = node->getAttributes();
		if (! map)
			continue;
		ILWIS::DrawerParameters dp(0, 0);
		String nodeName = CString(node->getNodeName());
		String id = fn.sFile;
		SVGElement *element = (SVGElement *)IlwWinApp()->getDrawer(id, 0, &dp);
		if ( element) {
			element->parse(node);
			if ( element->getId() != "")
				(*this)[element->getId()+"|ilwis38"] = element;
			break;
		}
	}
}

String SVGLoader::getAttributeValue(DOMNamedNodeMap *map, const String& key) const{
	if ( !map)
		return "";
	XMLCh *str = XERCES_CPP_NAMESPACE::XMLString::transcode(key.scVal());
	XERCES_CPP_NAMESPACE::DOMNode* attnode = map->getNamedItem(str);
	delete str;
	if ( attnode) {
		String value = CString(attnode->getTextContent());
		return value;
	}
	return "";
}


