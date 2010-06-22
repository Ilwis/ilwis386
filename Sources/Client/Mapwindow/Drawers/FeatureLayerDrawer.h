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
		void setDataSource(void *bmap,int options);

	protected:
		HTREEITEM SetColors(LayerTreeView  *tv, HTREEITEM parent,const BaseMap& bm);
		String mask;
		Color singleColor;

	};

	class SetSingleColorForm : public DisplayOptionsForm {
		public:
		SetSingleColorForm(CWnd *wPar, FeatureLayerDrawer *dr);
		void apply(); 
	private:
		Color c;
		FieldColor* fc;
	};
}