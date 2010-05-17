#pragma once


ILWIS::NewDrawer *createFeatureLayerDrawer(ILWIS::DrawerParameters *parms);

class FieldColor;

namespace ILWIS{

class FeatureLayerDrawer : public AbstractMapDrawer {
	public:
		ILWIS::NewDrawer *createFeatureLayerDrawer(DrawerParameters *parms);

		FeatureLayerDrawer(DrawerParameters *parms);
		virtual ~FeatureLayerDrawer();
		virtual void prepare(PreparationParameters *pp);
		String getMask() const;
		void setMask(const String& sM);
		void setSingleColor(const Color& c);
		Color getSingleColor() const;
		HTREEITEM  configure(LayerTreeView  *tv, HTREEITEM parent);

	protected:
		HTREEITEM SetColors(LayerTreeView  *tv, HTREEITEM parent,const BaseMap& bm);
		String mask;
		Color singleColor;

	};

	class SetSingleColorForm : public FormBaseDialog {
		public:
		SetSingleColorForm(CWnd *wPar, FeatureLayerDrawer *dr);
		int exec();
		afx_msg virtual void OnCancel(); 
	private:
		Color c;
		FeatureLayerDrawer *drw;
		FieldColor* fc;
		LayerTreeView *view;
	};
}