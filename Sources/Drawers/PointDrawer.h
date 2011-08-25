#pragma once

#include "Engine\Drawers\SVGElements.h"

ILWIS::NewDrawer *createPointDrawer(ILWIS::DrawerParameters *parms);

namespace ILWIS{

class SVGElement;

struct _export PointProperties : public GeneralDrawerProperties {
public:
	enum Scaling{sNONE, sLOGARITHMIC, sLINEAR};
	static String defaultSymbol;

	PointProperties(double t=0, const String& sym = "open-rectangle", Color clr=colorUNDEF) : 
		thickness(t),drawColor(clr), 
		ignoreColor(false),scale(1.0), 
		threeDOrientation(false), 
		stretchScale(1.0), scaleMode(sNONE),
		useDirection(false),
		angle(0)
		{}

	PointProperties(PointProperties *lp) { set(lp);}
	void set(PointProperties *lp) {
		thickness=lp->thickness; 
		symbol=lp->symbol;
		drawColor=lp->drawColor;
		ignoreColor=lp->ignoreColor; 
		scale = lp->scale;
		threeDOrientation = lp->threeDOrientation;
		stretchScale = lp->stretchScale;
		stretchRange = lp->stretchRange;
		stretchColumn = lp->stretchColumn;
		scaleMode = lp->scaleMode;
		useDirection = lp->useDirection;
		angle = lp->angle;

	}

	double scaling() const {
		return scale * stretchScale;
	}

	double thickness;
	String symbol;
	double scale;
	Color drawColor;
	bool ignoreColor;
	boolean threeDOrientation;
	double stretchScale;
	RangeReal stretchRange;
	String stretchColumn;
	Scaling scaleMode;
	bool useDirection;
	double angle;

	String store(const FileName& fnView, const String& parenSection) const;
	void load(const FileName& fnView, const String& parenSection);
};

class _export PointDrawer : public SimpleDrawer {
	public:
		PointDrawer(ILWIS::DrawerParameters *parms);
		~PointDrawer();
		virtual bool draw( const CoordBounds& cbArea=CoordBounds()) const;
		void prepare(PreparationParameters *);
		void shareVertices(vector<Coord *>& coords);
		void setCoord(const Coord& crd);
		GeneralDrawerProperties *getProperties();
	protected:
		PointDrawer(DrawerParameters *parms, const String& name);
		void calcSize();

		void drawRectangle(const SVGAttributes* attributes, double z) const;
		void drawEllipse(const SVGAttributes* attributes, double z) const;
		void drawLine(const SVGAttributes* attributes, double z) const;
		void drawPolygon(const SVGAttributes* attributes, double z) const;
		void drawPath(const SVGAttributes* attributes, double z) const;
		void transform(const SVGAttributes* attributes) const;

		Coord cNorm;
		const SVGElement *element;
		double width, height;
		PointProperties properties;
	};
}