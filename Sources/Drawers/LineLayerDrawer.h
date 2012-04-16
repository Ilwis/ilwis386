#pragma once


ILWIS::NewDrawer *createLineLayerDrawer(ILWIS::DrawerParameters *parms);

class FieldColor;
class FieldLineType;

namespace ILWIS{

	class _export LineLayerDrawer : public ILWIS::FeatureLayerDrawer {

	public:
		ILWIS::NewDrawer *createLineLayerDrawer(DrawerParameters *parms);

		LineLayerDrawer(DrawerParameters *parms);
		virtual ~LineLayerDrawer();
		void prepare(PreparationParameters *parm);
		GeneralDrawerProperties *getProperties();


	protected:
		virtual NewDrawer *createElementDrawer(PreparationParameters *pp,ILWIS::DrawerParameters* parms) const;
		void setDrawMethod(DrawMethod method=drmINIT);
		void displayOptionSetLineStyle(CWnd *parent);
		String store(const FileName& fnView, const String& parenSection) const;
		void load(const FileName& fnView, const String& parenSection);
		void getDrawerFor(const Feature* feature,vector<NewDrawer *>& featureDrawers);

		LineProperties lproperties;
		HTREEITEM styleItem;
	};

}