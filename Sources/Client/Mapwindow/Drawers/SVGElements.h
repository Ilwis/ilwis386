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

#define colorUNDEF Color(1,2,3)
#define colorUSERDEF Color(3,2,1)

namespace ILWIS {
	class _export SVGElement : public ComplexDrawer{
	public:
		enum ShapeType{sRECTANGLE, sCIRCLE,sELLIPSE,sLINE,sPOLYLINE,sPOLYGON,sPATH};

		SVGElement();
		bool draw(bool norecursion, const CoordBounds& cbArea) const { return true;}
		virtual void drawSVG(const CoordBounds& cbElement,const NewDrawer *dr, double z=0) const;
		virtual void parse(XERCES_CPP_NAMESPACE::DOMNode* node);
		void prepare(PreparationParameters *p) {};

	protected:
		SVGElement(DrawerParameters *parms, const String& name);
		void initSvgData();
		Color getColor(const String& name) const;
		String getAttributeValue(XERCES_CPP_NAMESPACE::DOMNamedNodeMap *map, const String& key) const;
		String parseStyle(const String& style);

		int ewidth, eheight;
		Color fillColor;
		Color strokeColor;
		int rx, ry, rwidth, rheight;
		int borderThickness;
		double opacity;

	private:
		static map<String, Color> svgcolors;
	};
}