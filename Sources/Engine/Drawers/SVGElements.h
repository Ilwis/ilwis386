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

//#define colorUNDEF Color(1,2,3)
#define colorUSERDEF Color(3,2,1)

namespace ILWIS {
	struct SVGAttributes {
		enum ShapeType{sRECTANGLE, sCIRCLE,sELLIPSE,sLINE,sPOLYLINE,sPOLYGON,sPATH, sCOMPOUND, sUNKNOWN};
		SVGAttributes(ShapeType t=sUNKNOWN) : type(t) {
			ox = oy = strokewidth = rwidth = rheight = rx = ry = cx = cy = 0;
			borderThickness = opacity = 1;
			fillColor = colorUSERDEF;
			strokeColor =Color(0,0,0);
		}
		int borderThickness;
		Color fillColor, strokeColor;
		double opacity,ox, oy, rx, ry,cx,cy, rwidth, rheight, strokewidth;
		vector<Coord> points;
		vector<Coord> triangles;
		ShapeType type;
	};

	class _export SVGElement : public vector<SVGAttributes> {
	public:
		SVGElement(const String& _id);
		SVGElement(SVGAttributes::ShapeType t, const String& _id);
		bool draw( const CoordBounds& cbArea) const { return true;}
		void parse(XERCES_CPP_NAMESPACE::DOMNode* node);
		void parseNode(XERCES_CPP_NAMESPACE::DOMNode* node,SVGAttributes& attributes);

	protected:
		void initSvgData();
		Color getColor(const String& name) const;
		String getAttributeValue(XERCES_CPP_NAMESPACE::DOMNamedNodeMap *map, const String& key) const;
		String parseStyle(const String& style,SVGAttributes& attributes);


	private:
		static map<String, Color> svgcolors;
		String id;
	};
}