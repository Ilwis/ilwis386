#pragma once


ILWIS::NewDrawer *createPointSetDrawer(ILWIS::DrawerParameters *parms);

class FieldColor;

namespace ILWIS{

class PointSetDrawer : public FeatureSetDrawer {
	public:
		ILWIS::NewDrawer *createPointSetDrawer(DrawerParameters *parms);

		PointSetDrawer(DrawerParameters *parms);
		virtual ~PointSetDrawer();
		HTREEITEM  configure(LayerTreeView  *tv, HTREEITEM parent);

	protected:
		String store(const FileName& fnView, const String& parenSection, SubType subtype) const;
		void load(const FileName& fnView, const String& parenSection);
		virtual NewDrawer *createElementDrawer(PreparationParameters *pp, ILWIS::DrawerParameters* parms) const;
		void setDrawMethod(DrawMethod method=drmINIT);
	};
}