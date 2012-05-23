#pragma once

#include "TemporalDrawer.h"
#include "SizableDrawer.h"

ILWIS::NewDrawer *createSpaceTimePathDrawer(ILWIS::DrawerParameters *parms);

class FieldColor;

namespace ILWIS{

struct PointProperties;

class _export SpaceTimePathDrawer : public FeatureLayerDrawer, public TemporalDrawer, public SizableDrawer {
	public:
		enum Scaling{sNONE, sLOGARITHMIC, sLINEAR};
		ILWIS::NewDrawer *createSpaceTimePathDrawer(DrawerParameters *parms);

		SpaceTimePathDrawer(DrawerParameters *parms);
		virtual ~SpaceTimePathDrawer();
		HTREEITEM  configure(LayerTreeView  *tv, HTREEITEM parent);
		void prepare(PreparationParameters *parms);
		virtual void prepareChildDrawers(PreparationParameters *pp);
		void getDrawerFor(const Feature* feature,vector<NewDrawer *>& featureDrawers);
		GeneralDrawerProperties *getProperties();
		bool draw( const CoordBounds& cbArea) const;
		virtual void setRepresentation(const Representation& rp);
		//void setScaleMode(Scaling mode) { scaleMode = mode; }
		//Scaling getScaleMode() const { return scaleMode; } 
		//bool usingDirection() const { return useDirection; }
		//void setUseDirection(bool yesno) { useDirection = yesno; }

	protected:
		String store(const FileName& fnView, const String& parenSection) const;
		void load(const FileName& fnView, const String& parenSection);
		virtual NewDrawer *createElementDrawer(PreparationParameters *pp, ILWIS::DrawerParameters* parms) const;
		void setDrawMethod(DrawMethod method=drmINIT);
		RangeReal getValueRange(Column attributeColumn) const;
		double scaleThickness(double v, Column & col, RangeReal & rr) const;
		Coord projectOnCircle(Coord AB, double r, double f) const;
		Coord normalize(Coord c) const;
		Coord cross(Coord c1, Coord c2) const;
		PointProperties *properties;
		GLuint * displayList;
		bool * fRefreshDisplayList;
		GLuint * texture;
		bool * fRefreshTexture;
		BaseMapPtr *basemap;
		bool fRealMap;
	private:
		Column prevAttColumn;
		bool prevUseAttColumn;
	};
}