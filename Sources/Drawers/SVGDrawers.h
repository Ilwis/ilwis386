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
	class  SVGRectangle : public SVGElement {
	public:
		SVGRectangle(ILWIS::DrawerParameters *parms);
		virtual void parse(XERCES_CPP_NAMESPACE::DOMNode* node);
		void drawSVG(const CoordBounds& cbElement,const NewDrawer *dr, double z=0) const;
	protected:
		SVGRectangle(ILWIS::DrawerParameters *parms, const String& name);
	
	};

	class  SVGEllipse : public SVGElement {
	public:
		SVGEllipse(ILWIS::DrawerParameters *parms);
		virtual void parse(XERCES_CPP_NAMESPACE::DOMNode* node);
		void drawSVG(const CoordBounds& cbElement,const NewDrawer *dr, double z=0) const;
	protected:
		SVGEllipse(ILWIS::DrawerParameters *parms, const String& name);
		double radius, radiusx,radiusy;
		int cx,cy;
	
	};
}

