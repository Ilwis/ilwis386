#pragma once 

#include <xercesc/dom/DOM.hpp>
#include <xercesc/dom/DOMDocument.hpp>
#include <xercesc/dom/DOMDocumentType.hpp>
#include <xercesc/dom/DOMElement.hpp>
#include <xercesc/dom/DOMImplementation.hpp>
#include <xercesc/dom/DOMImplementationLS.hpp>
#include <xercesc/dom/DOMNodeIterator.hpp>
#include <xercesc/dom/DOMNodeList.hpp>
#include <xercesc/dom/DOMText.hpp>
#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc\dom\DOMImplementationLS.hpp>
#include <xercesc\Sax\InputSource.hpp>
#include <xercesc\framework\MemBufInputSource.hpp>


class XQillaNSResolver;

namespace ILWIS {
	class XMLDocument {
	public:
		XMLDocument(const String& xml);
		~XMLDocument();
		void executeXPathExpression(const String& xpathexpr, vector<String>& results) const;
		void addNameSpace(const String& name, const String& uri);
	private:
		XERCES_CPP_NAMESPACE::DOMImplementation *xqillaImplementation;
		XERCES_CPP_NAMESPACE::DOMLSParser *parser;
		XERCES_CPP_NAMESPACE::DOMDocument *document;
		XQillaNSResolver *resolver;

	};
}