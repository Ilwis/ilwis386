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

void addSVGDrawers(DrawerInfoVector *infos);

namespace ILWIS{
	class  SVGRectangleElement : public SVGElement {
	public:
		SVGRectangleElement(ILWIS::DrawerParameters *parms);
		virtual void parse(XERCES_CPP_NAMESPACE::DOMNode* node);
		virtual bool draw(bool norecursion = false, const CoordBounds& cbArea=CoordBounds()) const;
		void prepare(PreparationParameters *);
	protected:
		SVGRectangleElement(ILWIS::DrawerParameters *parms, const String& name);
		Color fillColor;
		Color strokeColor;
		int rx, ry, rwidth, rheight;

	};
}

