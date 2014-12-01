#pragma once

#include "SortableDrawer.h"
#include "GroupableDrawer.h"
#include "SpaceTimeDrawer.h"

ILWIS::NewDrawer *createSpaceTimePathDrawer(ILWIS::DrawerParameters *parms);

namespace ILWIS {

class _export SpaceTimePathDrawer : public SpaceTimeDrawer, public SortableDrawer, public GroupableDrawer {
	public:
		ILWIS::NewDrawer *createSpaceTimePathDrawer(DrawerParameters *parms);

		SpaceTimePathDrawer(DrawerParameters *parms);
		virtual ~SpaceTimePathDrawer();
		virtual void prepare(PreparationParameters *parms);
		const vector<Feature *> & getFeatures() const;
		virtual void getRaws(GLuint objectID, vector<long> & raws) const;
		virtual void drawFootprint() const;

	protected:
		String store(const FileName& fnView, const String& parentSection) const;
		void load(const FileName& fnView, const String& currentSection);
		virtual void drawObjects(const int steps, GetHatchFunc getHatchFunc) const;
		virtual vector<GLuint> getObjectIDs(vector<long> & iRaws) const;
		virtual int getNearestEnabledObjectIDIndex(vector<GLuint> & objectIDs) const;
		virtual vector<GLuint> getEnabledObjectIDs(vector<GLuint> & objectIDs) const;
		virtual Feature * getFeature(GLuint objectID) const;
		vector<Feature *> features;
		vector<long> *objectStartIndexes;
		const double textureOffset;
		const double textureRange;
	};
}