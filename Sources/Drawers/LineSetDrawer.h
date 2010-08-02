#pragma once


ILWIS::NewDrawer *createLineSetDrawer(ILWIS::DrawerParameters *parms);

class FieldColor;

namespace ILWIS{

class LineSetDrawer : public FeatureSetDrawer {
	public:
		ILWIS::NewDrawer *createLineSetDrawer(DrawerParameters *parms);

		LineSetDrawer(DrawerParameters *parms);
		virtual ~LineSetDrawer();
		HTREEITEM  configure(LayerTreeView  *tv, HTREEITEM parent);

	protected:
		virtual NewDrawer *createElementDrawer(PreparationParameters *pp,ILWIS::DrawerParameters* parms) const;
		void setDrawMethod(DrawMethod method=drmINIT);
	};
}