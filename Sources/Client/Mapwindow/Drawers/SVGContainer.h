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

namespace ILWIS {

	class SVGElement {
	public:
		SVGElement(const FileName& fn) ;
		SVGElement(XERCES_CPP_NAMESPACE::DOMNode* node);
	};

	class SVGContainer : public map<String, SVGElement *> {
	public:
		SVGContainer();
		~SVGContainer();
		void load();
	private:
		void parseFile(const FileName& fn);

	};

}