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
	class SVGElement ;

	class SVGLoader : public map<String, SVGElement *> {
	public:
		SVGLoader();
		~SVGLoader();
		void load();
	private:
		String getAttributeValue(XERCES_CPP_NAMESPACE::DOMNamedNodeMap *map, const String& key) const;
		void parseFile(const FileName& fn);
		static map<String,Color> svgcolors;
	};

}