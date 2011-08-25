#pragma once


ILWIS::NewDrawer *createPointLayerDrawer(ILWIS::DrawerParameters *parms);

class FieldColor;

namespace ILWIS{

struct PointProperties;

struct SymbolRotationInfo {
	SymbolRotationInfo() : clockwise(true) {}
		String rotationColumn;
		bool clockwise;
		RangeReal rr;
};

class _export PointLayerDrawer : public FeatureLayerDrawer {
	public:
		enum Scaling{sNONE, sLOGARITHMIC, sLINEAR};
		ILWIS::NewDrawer *createPointLayerDrawer(DrawerParameters *parms);

		PointLayerDrawer(DrawerParameters *parms);
		virtual ~PointLayerDrawer();
		HTREEITEM  configure(LayerTreeView  *tv, HTREEITEM parent);
		void prepare(PreparationParameters *parms);
		void getDrawerFor(const Feature* feature,vector<NewDrawer *>& featureDrawers);
		GeneralDrawerProperties *getProperties();
		void setRotationInfo(const SymbolRotationInfo& sC) ;
		SymbolRotationInfo getRotationInfo() const;

	
	protected:
		String store(const FileName& fnView, const String& parenSection) const;
		void load(const FileName& fnView, const String& parenSection);
		virtual NewDrawer *createElementDrawer(PreparationParameters *pp, ILWIS::DrawerParameters* parms) const;
		void setDrawMethod(DrawMethod method=drmINIT);
		void setSymbolization(CWnd *parent);
		PointProperties *properties;
		SymbolRotationInfo rotationInfo;


	};

}