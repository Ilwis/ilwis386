#pragma once

ILWIS::NewDrawer *createSVGDrawer(ILWIS::DrawerParameters *parms);

namespace ILWIS{

class SVGDrawer : public SimpleDrawer {
	public:
		SVGDrawer(ILWIS::DrawerParameters *parms);
		virtual bool draw(bool norecursion = false, const CoordBounds& cbArea=CoordBounds()) const;
		void prepare(PreparationParameters *);
	
	protected:
		SVGDrawer(DrawerParameters *parms, const String& name);
		HTREEITEM configure(LayerTreeView  *tv, HTREEITEM parent) ;



	};
}