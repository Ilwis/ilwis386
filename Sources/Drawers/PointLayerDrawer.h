#pragma once


ILWIS::NewDrawer *createPointLayerDrawer(ILWIS::DrawerParameters *parms);

class FieldColor;

namespace ILWIS{

class _export PointLayerDrawer : public FeatureLayerDrawer {
	public:
		ILWIS::NewDrawer *createPointLayerDrawer(DrawerParameters *parms);

		PointLayerDrawer(DrawerParameters *parms);
		virtual ~PointLayerDrawer();
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