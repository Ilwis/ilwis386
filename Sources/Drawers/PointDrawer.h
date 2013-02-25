#pragma once

#include "Engine\Drawers\SVGElements.h"

ILWIS::NewDrawer *createPointDrawer(ILWIS::DrawerParameters *parms);

namespace ILWIS{

class IVGElement;
class TextDrawer;

struct _export PointProperties : public GeneralDrawerProperties {
public:
	enum Scaling{sNONE, sLOGARITHMIC, sLINEAR};
	enum RadiusArea{sRADIUS, sAREA};

	PointProperties(double t=0, const String& sym = "open-rectangle", Color clr=colorUNDEF) : 
		thickness(t),drawColor(clr), 
		ignoreColor(false), 
		threeDOrientation(false), 
		stretchScale(1.0), scaleMode(sNONE), radiusArea(sRADIUS),
		useDirection(false),
		angle(0),
		exaggeration(1.0),
		labelColumn(sUNDEF),
		scale(100.0),
		symbol(DEFAULT_POINT_SYMBOL_TYPE)
		{}

	PointProperties(PointProperties *lp) { set(lp);}
	void set(PointProperties *lp) {
		thickness=lp->thickness; 
		symbol=lp->symbol;
		drawColor=lp->drawColor;
		ignoreColor=lp->ignoreColor; 
		threeDOrientation = lp->threeDOrientation;
		stretchScale = lp->stretchScale;
		stretchRange = lp->stretchRange;
		stretchColumn = lp->stretchColumn;
		scaleMode = lp->scaleMode;
		radiusArea = lp->radiusArea;
		useDirection = lp->useDirection;
		angle = (angle != 0 && lp->useDirection) ? angle : lp->angle;
		exaggeration = lp->exaggeration;
		scale = lp->scale;
	}

	bool operator!=(const PointProperties& pp) {
		return thickness != pp.thickness ||
		symbol != pp.symbol ||
		scale != pp.scale ||
		exaggeration != pp.exaggeration ||
		drawColor != pp.drawColor ||
		ignoreColor != pp.ignoreColor || 
		threeDOrientation != pp.threeDOrientation ||
		stretchScale != pp.stretchScale ||
		stretchRange != pp.stretchRange ||
		stretchColumn != pp.stretchColumn ||
		scaleMode != pp.scaleMode ||
		radiusArea != pp.radiusArea ||
		useDirection != pp.useDirection ||
		angle != pp.angle ||
		labelColumn != pp.labelColumn;
	}

	double scaling() const {
		return scale * stretchScale;
	}

	double thickness;
	String symbol;
	double scale;
	double exaggeration; // for stretched point maps
	Color drawColor;
	bool ignoreColor;
	boolean threeDOrientation;
	double stretchScale;
	RangeReal stretchRange;
	String stretchColumn;
	Scaling scaleMode;
	RadiusArea radiusArea;
	bool useDirection;
	double angle;
	String labelColumn;


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
		virtual void select(bool yesno);
		void setGeneralProperties(GeneralDrawerProperties *);
		CoordBounds getBounds() const { return CoordBounds(cNorm, cNorm); }
	protected:
		PointDrawer(DrawerParameters *parms, const String& name);
		void calcSize();

		void drawRectangle(const IVGAttributes* attributes, double z) const;
		void drawEllipse(const IVGAttributes* attributes, double z) const;
		void drawLine(const IVGAttributes* attributes, double z) const;
		void drawPolygon(const IVGAttributes* attributes, double z) const;
		void drawPath(const IVGAttributes* attributes, double z) const;
		void transform(const IVGAttributes* attributes) const;

		Coord cNorm;
		const IVGElement *element;
		double width, height;
		PointProperties properties;
	};
}