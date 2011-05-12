#include "Headers\toolspch.h"
#include "Engine\base\system\engine.h"
#include <map>
#include "Engine\Drawers\SVGContainer.h"


using namespace ILWIS;
using namespace XERCES_CPP_NAMESPACE;

SVGElement::SVGElement(const FileName& fn) {

}

SVGElement::SVGElement(DOMNode* node) {
}

//-----------------------------
SVGContainer::SVGContainer() {
}
SVGContainer::~SVGContainer() {
}
void SVGContainer::load() {
	String ilwDir = getEngine()->getContext()->sIlwDir();
	String pathToSvg = ilwDir + "\\Resources\\symbols\\*.svg";
	//XERCES_CPP_NAMESPACE::XMLPlatformUtils::Initialize();
	try{

		CFileFind finder;
		BOOL fFound = finder.FindFile(pathToSvg.scVal());
		while(fFound) {
			fFound = finder.FindNextFile();
			if (fFound)
			{
				FileName fnSvg(finder.GetFilePath());
				parseFile(fnSvg);
			}
		}
	XMLPlatformUtils::Terminate();
	}catch( xercesc::XMLException& e ) {
		throw ErrorObject(CString(e.getMessage()));
	}

}

void SVGContainer::parseFile(const FileName& fn) {
	XercesDOMParser *parser = new XercesDOMParser;
	parser->setValidationScheme(XercesDOMParser::Val_Auto);
	parser->setDoNamespaces(false);
	parser->setDoSchema(false);
	parser->setValidationSchemaFullChecking(false);
	parser->setCreateEntityReferenceNodes(false);

	parser->parse(fn.sFullPath().scVal());
}
