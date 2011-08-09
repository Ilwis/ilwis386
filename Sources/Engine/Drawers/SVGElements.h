#pragma once

#include "Engine\Base\DataObjects\XMLDocument.h"

//#define colorUNDEF Color(1,2,3)
#define colorUSERDEF Color(3,2,1)

namespace ILWIS {
	struct Transform {
		enum TransformType{tROTATE,tTRANSLATE,tMATRIX,tSCALE};
		vector<double> parameters;
		TransformType type;
	};


	//struct ESize {
	//	ESize(double x=iUNDEF, double y=iUNDEF) : cx(x), cy(y) {}
	//	double cx;
	//	double cy;
	//};

	class SVGAttributes {
	public:
		enum ShapeType{sRECTANGLE, sCIRCLE,sELLIPSE,sLINE,sPOLYLINE,sPOLYGON,sPATH, sCOMPOUND, sUNKNOWN};

		SVGAttributes(ShapeType t=sUNKNOWN) : type(t) {
			strokewidth = rwidth = rheight = rx = ry = 0;
			borderThickness = opacity = 1;
			fillColor = colorUSERDEF;
			strokeColor =Color(0,0,0);
		}
		int borderThickness;
		Color fillColor, strokeColor;
		double opacity, rx, ry, rwidth, rheight, strokewidth;
		vector<Coord> points;
		vector<vector<Coord> > triangleStrips;
		ShapeType type;
		CoordBounds bounds;
		vector<Transform> transformations;

	};

	class _export SVGElement : public vector<SVGAttributes *> {
	public:
		SVGElement(const String& _id);
		SVGElement(SVGAttributes::ShapeType t, const String& _id);
		bool draw( const CoordBounds& cbArea) const { return true;}
		void parse(const pugi::xml_node& node);
		void parseNode(const pugi::xml_node& node, SVGAttributes* attributes);
		CoordBounds getCb() const { return cb; }

	protected:
		void initSvgData();
		void parseTransform(SVGAttributes* attributes, const String& tranform);
		Color getColor(const String& name) const;
		String getAttributeValue(const pugi::xml_node& node, const String& key) const;
		String parseStyle(const String& style,SVGAttributes* attributes);


	private:
		static map<String, Color> svgcolors;
		CoordBounds cb;
		String id;
	};
}