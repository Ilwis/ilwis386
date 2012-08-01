#pragma once

#include "TemporalDrawer.h"
#include "SizableDrawer.h"

namespace ILWIS {

typedef void (FeatureLayerDrawer::*GetHatchFunc)(RepresentationClass * prc, long iRaw, const byte* &hatch) const;

class _export SpaceTimeDrawer : public FeatureLayerDrawer, public TemporalDrawer, public SizableDrawer {
	public:
		enum Scaling{sNONE, sLOGARITHMIC, sLINEAR};
		ILWIS::NewDrawer *createSpaceTimeDrawer(DrawerParameters *parms);

		SpaceTimeDrawer(DrawerParameters *parms, const String& name);
		virtual ~SpaceTimeDrawer();
		HTREEITEM  configure(LayerTreeView  *tv, HTREEITEM parent);
		virtual void prepare(PreparationParameters *parms);
		virtual void prepareChildDrawers(PreparationParameters *pp);
		GeneralDrawerProperties *getProperties();
		bool draw( const CoordBounds& cbArea) const;
		virtual void setRepresentation(const Representation& rp);
		void SetNrSteps(int steps);
		int iNrSteps();

	protected:
		String store(const FileName& fnView, const String& parentSection) const;
		void load(const FileName& fnView, const String& currentSection);
		GLuint getSelectedObjectID(const Coord& c) const;
		virtual NewDrawer *createElementDrawer(PreparationParameters *pp, ILWIS::DrawerParameters* parms) const;
		void setDrawMethod(DrawMethod method=drmINIT);
		RangeReal getValueRange(Column attributeColumn) const;
		void getHatch(RepresentationClass * prc, long iRaw, const byte* &hatch) const;
		void getHatchInverse(RepresentationClass * prc, long iRaw, const byte* &hatchInverse) const;
		virtual void drawObjects(const int steps, GetHatchFunc getHatchFunc) const = 0;
		Coord projectOnCircle(Coord AB, double r, double f) const;
		Coord normalize(Coord c) const;
		Coord cross(Coord c1, Coord c2) const;
		GLuint * displayList;
		bool * fRefreshDisplayList;
		GLuint * texture;
		bool * fRefreshTexture;
		BaseMapPtr *basemap;
		bool fValueMap;
		CoordBounds cube;
		vector<long> disabledRaws;
		bool *fHatching;
	private:
		Column prevAttColumn;
		bool prevUseAttColumn;
		int nrSteps;
		CCriticalSection * csDraw;
	};
}