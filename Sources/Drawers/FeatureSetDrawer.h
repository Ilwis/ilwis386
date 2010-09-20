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


	protected:
		HTREEITEM make3D(bool yeno, LayerTreeView  *tvm);
		void displayOptionMask(CWnd *parent);
		void displayOptionSingleColor(CWnd *parent);
		void displayZOption3D(CWnd *parent);
		void displayZScaling(CWnd *parent);
		void setExtrusion(void *value, LayerTreeView *tree);
		String store(const FileName& fnView, const String& parenSection) const;
		void load(const FileName& fnView, const String& parenSection);

		String mask;
		bool useMask;
		Color singleColor;
		DisplayOptionColorItem *colorItem;
	
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

		class ZDataScaling : public DisplayOptionsForm {
		public:
		ZDataScaling(CWnd *wPar, SetDrawer *dr);
		void apply(); 
	private:
		int settransforms(Event *);
		FieldRealSliderEx *sliderScale;
		FieldRealSliderEx *sliderOffset;

		double zoffset;
		double zscale;
	};

	class DisplayZDataSourceForm : public DisplayOptionsForm {
		public:
		DisplayZDataSourceForm(CWnd *wPar, SetDrawer *dr);
		void apply(); 
	private:
		RadioGroup *rg;
		FieldMap *fmap;

		String colName;
		Table  attTable;
		String mapName;
		int sourceIndex;
		BaseMap bmp;
	};
}