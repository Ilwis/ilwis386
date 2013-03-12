#pragma once

#include "TemporalDrawer.h"
#include "SizableDrawer.h"

namespace ILWIS {

typedef void (FeatureLayerDrawer::*GetHatchFunc)(RepresentationClass * prc, long iRaw, const byte* &hatch) const;

class SpaceTimeElementsDrawer;

#define PALETTE_TEXTURE_SIZE 256

class _export SpaceTimeDrawer : public FeatureLayerDrawer, public TemporalDrawer, public SizableDrawer {
	public:
		ILWIS::NewDrawer *createSpaceTimeDrawer(DrawerParameters *parms);

		SpaceTimeDrawer(DrawerParameters *parms, const String& name);
		virtual ~SpaceTimeDrawer();
		HTREEITEM  configure(LayerTreeView  *tv, HTREEITEM parent);
		virtual void prepare(PreparationParameters *parms);
		virtual void prepareChildDrawers(PreparationParameters *pp);
		GeneralDrawerProperties *getProperties();
		bool draw(const DrawLoop drawLoop, const CoordBounds& cbArea) const;
		virtual void setRepresentation(const Representation& rp);
		void SetNrSteps(int steps);
		int iNrSteps();
		SpaceTimeElementsDrawer * getAdditionalElementsDrawer();
		BaseMapPtr *getBasemap();
		void SetTimePosVariable(double * _timePos);
		void SetClipTPlus(bool fClip);
		void SetClipTMinus(bool fClip);
		virtual void drawFootprint() const = 0;

	protected:
		String store(const FileName& fnView, const String& parentSection) const;
		void load(const FileName& fnView, const String& currentSection);
		vector<GLuint> getSelectedObjectIDs(const CRect& rect) const;
		virtual vector<GLuint> getObjectIDs(vector<long> & iRaws) const = 0;
		virtual int getNearestEnabledObjectIDIndex(vector<GLuint> & objectIDs) const = 0;
		virtual vector<GLuint> getEnabledObjectIDs(vector<GLuint> & objectIDs) const = 0;
		virtual Feature * getFeature(GLuint objectID) const = 0;
		virtual void getRaws(GLuint objectID, vector<long> & raws) const = 0;
		virtual String getInfo(const Coord& c) const;
		virtual void select(const CRect& rect, vector<long> & selectedRaws, SelectionMode selectionMode);
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
		vector<GLuint> selectedObjectIDs;
		map<long, GLuint> *subDisplayLists;
		bool *fHatching;
		SpaceTimeElementsDrawer * spaceTimeElementsDrawer;
	private:
		Column prevAttColumn;
		bool prevUseAttColumn;
		int nrSteps;
		double * timePos;
		CCriticalSection * csDraw;
		PointProperties *ppcopy;
		bool fClipTPlus;
		bool fClipTMinus;
	};
}