#pragma once


ILWIS::NewDrawer *createPointSetDrawer(ILWIS::DrawerParameters *parms);

class FieldColor;

namespace ILWIS{

class _export PointSetDrawer : public FeatureSetDrawer {
	public:
		ILWIS::NewDrawer *createPointSetDrawer(DrawerParameters *parms);

		PointSetDrawer(DrawerParameters *parms);
		virtual ~PointSetDrawer();
		HTREEITEM  configure(LayerTreeView  *tv, HTREEITEM parent);
		void prepare(PreparationParameters *parms);
		void getDrawerFor(const Feature* feature,vector<NewDrawer *>& featureDrawers);
		void setSymbolProperties(const String& symbol, double scale);

	protected:
		String store(const FileName& fnView, const String& parenSection) const;
		void load(const FileName& fnView, const String& parenSection);
		virtual NewDrawer *createElementDrawer(PreparationParameters *pp, ILWIS::DrawerParameters* parms) const;
		void setDrawMethod(DrawMethod method=drmINIT);
		void setSymbolization(CWnd *parent);
	};

}