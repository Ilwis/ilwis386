#pragma once

#include "SpaceTimeDrawer.h"

ILWIS::NewDrawer *createStationsDrawer(ILWIS::DrawerParameters *parms);

namespace ILWIS {

class _export StationsDrawer : public SpaceTimeDrawer {
	public:
		ILWIS::NewDrawer *createStationsDrawer(DrawerParameters *parms);

		StationsDrawer(DrawerParameters *parms);
		virtual ~StationsDrawer();
		virtual void drawFootprint() const;
		virtual void drawXT() const;
		virtual void drawXY() const;
		virtual void drawYT() const;

	protected:
		String store(const FileName& fnView, const String& parentSection) const;
		void load(const FileName& fnView, const String& currentSection);
		virtual void drawObjects(const int steps, GetHatchFunc getHatchFunc) const;
		virtual vector<GLuint> getObjectIDs(vector<long> & iRaws) const;
		virtual int getNearestEnabledObjectIDIndex(vector<GLuint> & objectIDs) const;
		virtual vector<GLuint> getEnabledObjectIDs(vector<GLuint> & objectIDs) const;
		virtual Feature * getFeature(GLuint objectID) const;
		virtual void getRaws(GLuint objectID, vector<long> & raws) const;
	private:
		const double textureOffset;
		const double textureRange;
	};
}