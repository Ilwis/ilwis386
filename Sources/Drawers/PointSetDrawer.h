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
		void prepare(PreparationParameters *parms);
		void getDrawerFor(const Feature* feature,vector<NewDrawer *>& featureDrawers);
		void setSymbols(const String& symbol);

	protected:
		String store(const FileName& fnView, const String& parenSection) const;
		void load(const FileName& fnView, const String& parenSection);
		virtual NewDrawer *createElementDrawer(PreparationParameters *pp, ILWIS::DrawerParameters* parms) const;
		void setDrawMethod(DrawMethod method=drmINIT);
		void setSymbolization(CWnd *parent);
	};

	class PointSymbolizationForm : public DisplayOptionsForm {
		public:
		PointSymbolizationForm(CWnd *wPar, PointSetDrawer *dr);
		void apply(); 
	private:
		vector<string> names;
		long selection;
		FieldOneSelectString *fselect;


	};

}