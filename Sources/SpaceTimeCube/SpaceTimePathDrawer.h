#pragma once

#include "TemporalDrawer.h"
#include "SizableDrawer.h"
#include "SortableDrawer.h"
#include "GroupableDrawer.h"

ILWIS::NewDrawer *createSpaceTimePathDrawer(ILWIS::DrawerParameters *parms);

class FieldColor;

namespace ILWIS{

class _export SpaceTimePathDrawer : public FeatureLayerDrawer, public TemporalDrawer, public SizableDrawer, public SortableDrawer, public GroupableDrawer {
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
		void SetNrSteps(int steps);
		int iNrSteps();

	protected:
		String store(const FileName& fnView, const String& parenSection) const;
		void load(const FileName& fnView, const String& parenSection);
		virtual NewDrawer *createElementDrawer(PreparationParameters *pp, ILWIS::DrawerParameters* parms) const;
		void setDrawMethod(DrawMethod method=drmINIT);
		RangeReal getValueRange(Column attributeColumn) const;
		Coord projectOnCircle(Coord AB, double r, double f) const;
		Coord normalize(Coord c) const;
		Coord cross(Coord c1, Coord c2) const;
		GLuint * displayList;
		bool * fRefreshDisplayList;
		GLuint * texture;
		bool * fRefreshTexture;
		BaseMapPtr *basemap;
		bool fValueMap;
		vector<Feature *> features;
		CoordBounds cube;
		vector<int> disabledRaws;
	private:
		Column prevAttColumn;
		bool prevUseAttColumn;
		int nrSteps;
	};
}