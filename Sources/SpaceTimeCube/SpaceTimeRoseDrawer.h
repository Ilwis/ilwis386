#pragma once

#include "SpaceTimePathDrawer.h"

void executeStcCommand(const String& cmd);
ILWIS::NewDrawer *createSpaceTimeRoseDrawer(ILWIS::DrawerParameters *parms);

namespace ILWIS {

class _export SpaceTimeRoseDrawer : public SpaceTimePathDrawer {
	public:
		ILWIS::NewDrawer *createSpaceTimeRoseDrawer(DrawerParameters *parms);

		SpaceTimeRoseDrawer(DrawerParameters *parms);
		void executeStcCommand(const String& cmd);
		virtual ~SpaceTimeRoseDrawer();
		virtual void prepare(PreparationParameters *parms);
		virtual void drawXY() const;
		static const list<SpaceTimeRoseDrawer*> & getSpaceTimeRoseDrawers();

	protected:
		virtual void drawObjects(const int steps, GetHatchFunc getHatchFunc) const;
		void drawItem(Coord head, double rHead, double rHeadAngle, float rsHead, Coord tail, double rTail, double rTailAngle, float rsTail) const;

	private:
		static list<SpaceTimeRoseDrawer*> lstrd;
		bool fIDSelected(String & sID) const;
		bool fTimeSelected(String & sID, double time) const;
		map<String, list<pair<double, double>>> timeSelection;
	};
}