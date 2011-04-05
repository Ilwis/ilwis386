#include "headers/toolspch.h"
#include "XQuila\xqilla\xqilla-dom3.hpp"
#include <xercesc/framework/StdOutFormatTarget.hpp>
#include "xqilla\exceptions\XMLParseException.hpp"
#include "httpserver\XMLDocument.h"

using namespace ILWIS;
using namespace XERCES_CPP_NAMESPACE;

ILWIS::XMLDocument::XMLDocument(const String& xml) : resolver(0),parser(0),xqillaImplementation(0) {
	XQillaPlatformUtils::initialize();
	xqillaImplementation = DOMImplementationRegistry::getDOMImplementation(X("XPath2 3.0"));
	parser = xqillaImplementation->createLSParser(DOMImplementationLS::MODE_SYNCHRONOUS, 0);
	parser->getDomConfig()->setParameter(XMLUni::fgDOMNamespaces, true);
	parser->getDomConfig()->setParameter(XMLUni::fgXercesSchema, true);
	parser->getDomConfig()->setParameter(XMLUni::fgDOMValidateIfSchema, true);
	XMLCh* testData = XMLString::transcode(xml.scVal());
	DOMLSInput *inp = xqillaImplementation->createLSInput();
	inp->setStringData(testData);
	document = parser->parse(inp);
}

ILWIS::XMLDocument::~XMLDocument() {
	if ( parser)
		parser->release();
//	document->release();
//	if ( resolver)
//		resolver->release();
}
void ILWIS::XMLDocument::addNameSpace(const String& name, const String& uri) {
	if (!resolver)
		resolver =  (XQillaNSResolver*)document->createNSResolver (document->getDocumentElement());
	resolver->addNamespaceBinding(X(name.scVal()),X(uri.scVal()));
}

void ILWIS::XMLDocument::executeXPathExpression(const String& xpathexpr, vector<String>& results) const {
	try{
		AutoRelease<DOMXPathExpression> expression(document->createExpression(X(xpathexpr.scVal()), resolver));
		AutoRelease<DOMXPathResult> result(expression->evaluate(document, DOMXPathResult::ITERATOR_RESULT_TYPE, 0));

		AutoRelease<DOMLSSerializer> serializer(xqillaImplementation->createLSSerializer());
		AutoRelease<DOMLSOutput> output(xqillaImplementation->createLSOutput());
		StdOutFormatTarget target;
		output->setByteStream(&target);
		while(result->iterateNext()) {
			XMLCh* res1 = serializer->writeToString(result->getNodeValue());
			CString s(res1);
			results.push_back(String(s));
		}
	} catch (XQillaException& e) {
		CString s(e.getMessage());
		throw ErrorObject(String(s));
	}
}

