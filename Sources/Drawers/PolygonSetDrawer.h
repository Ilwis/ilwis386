#pragma once


ILWIS::NewDrawer *createPolygonSetDrawer(ILWIS::DrawerParameters *parms);

class FieldColor;

namespace ILWIS{

class PolygonSetDrawer : public FeatureSetDrawer {
	public:
		ILWIS::NewDrawer *createPolygonSetDrawer(DrawerParameters *parms);

		PolygonSetDrawer(DrawerParameters *parms);
		virtual ~PolygonSetDrawer();
		virtual void prepare(PreparationParameters *pp);
		HTREEITEM  configure(LayerTreeView  *tv, HTREEITEM parent);

	protected:
		virtual NewDrawer *createElementDrawer(PreparationParameters *pp, ILWIS::DrawerParameters* parms) const;
		void setDrawMethod(DrawMethod method=drmINIT);
	};
}