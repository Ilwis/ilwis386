#pragma once


ILWIS::NewDrawer *createFeatureSetDrawer(ILWIS::DrawerParameters *parms);

class FieldColor;
class DisplayOptionColorItem;

namespace ILWIS{

class _export FeatureSetDrawer : public SetDrawer {
	friend class SetSingleColorForm;

	public:
		ILWIS::NewDrawer *createFeatureSetDrawer(DrawerParameters *parms);

		FeatureSetDrawer(DrawerParameters *parms, const String& name);
		virtual ~FeatureSetDrawer();
		virtual void prepare(PreparationParameters *pp);
		String getMask() const;
		void setMask(const String& sM);
		void setSingleColor(const Color& c);
		Color getSingleColor() const;
		HTREEITEM  configure(LayerTreeView  *tv, HTREEITEM parent);
		virtual NewDrawer *createElementDrawer(PreparationParameters *pp, ILWIS::DrawerParameters* parms) const { return 0;}
		void getFeatures(vector<Feature *>& features) const;
		virtual void addDataSource(void *,int options=0);
		void *getDataSource() const;

	protected:
		void displayOptionMask(CWnd *parent);
		void displayOptionSingleColor(CWnd *parent);
		void setSingleColorMap(void *value, LayerTreeView *tree);
		String store(const FileName& fnView, const String& parenSection) const;
		void load(const FileName& fnView, const String& parenSection);

		String mask;
		bool useMask;
		Color singleColor;
		DisplayOptionColorItem *colorItem;
		BaseMap fbasemap;
	
	};
	class SetMaskForm : public DisplayOptionsForm {
	public:
		SetMaskForm(CWnd *wPar, FeatureSetDrawer *dr);
		void apply();
	private:
		String mask;
		FieldString *fs;

	};

	class SetSingleColorForm : public DisplayOptionsForm {
	public:
		SetSingleColorForm(CWnd *wPar, FeatureSetDrawer *dr);
		void apply();
	private:
		FieldColor *fc;
		Color c;

	};
}