#pragma once

ILWIS::NewDrawer *createPointSymbolDrawer(ILWIS::DrawerParameters *parms);

namespace ILWIS {

class PointSymbolDrawer : public PointDrawer {
		public:
		PointSymbolDrawer(ILWIS::DrawerParameters *parms);
		virtual void draw(bool norecursion = false);
		void prepare(PreparationParameters *);
	protected:
		PointSymbolDrawer(DrawerParameters *parms, const String& name);
	};
}