#pragma once


ILWIS::NewDrawer *createLineSetDrawer(ILWIS::DrawerParameters *parms);

class FieldColor;
class FieldLineType;
enum LineDspType;

namespace ILWIS{

class LineSetDrawer : public FeatureSetDrawer {

	public:
		ILWIS::NewDrawer *createLineSetDrawer(DrawerParameters *parms);

		LineSetDrawer(DrawerParameters *parms);
		virtual ~LineSetDrawer();
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