#pragma once

#include "Engine\Base\DataObjects\XMLDocument.h"

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

	class IVGAttributes {
	public:
		enum ShapeType{sRECTANGLE, sCIRCLE,sELLIPSE,sLINE,sPOLYLINE,sPOLYGON,sPATH, sCOMPOUND, sUNKNOWN};

		IVGAttributes(ShapeType t=sUNKNOWN) : type(t) {
			strokewidth = rwidth = rheight = rx = ry = 0;
			opacity = 1;
			strokewidth = 1;
			fillColor = colorUSERDEF;
			strokeColor =Color(0,0,0);
		}
		virtual bool isPolygon() const;
		Color fillColor, strokeColor;
		double opacity, rx, ry, rwidth, rheight, strokewidth;
		vector<Coord> points;
		vector<pair<unsigned int, vector<Coord>>> triangleStrips;
		ShapeType type;
		CoordBounds bounds;
		vector<Transform> transformations;

	};

	class _export IVGElement : public vector<IVGAttributes *> {
	public:
		enum Type{ivgPOINT, ivgHATCH, ivgNORTHARROW};

		IVGElement(const String& _id);
		IVGElement(IVGAttributes::ShapeType t, const String& _id);
		~IVGElement();
		bool draw( const CoordBounds& cbArea) const { return true;}
		void parse(const pugi::xml_node& node);
		void parseNode(const pugi::xml_node& node, IVGAttributes* attributes);
		CoordBounds getCb() const { return cb; }
		double getDefaultScale() const { return defaultScale; }
		IVGElement::Type getType() const { return type;}
		const byte * getHatch() const;
		const byte * getHatchInverse() const;

	protected:
		void initSvgData();
		void parseTransform(IVGAttributes* attributes, const String& tranform);
		Color getColor(const String& name) const;
		String getAttributeValue(const pugi::xml_node& node, const String& key) const;
		String parseStyle(const String& style,IVGAttributes* attributes);
		void parseHatch(const pugi::xml_node& node);
		void normalizePositions();
		double defaultScale;
	private:
		static map<String, Color> svgcolors;
		CoordBounds cb;
		String id;
		IVGElement::Type type;
		byte *hatch;
		byte *hatchInverse;
	};
}