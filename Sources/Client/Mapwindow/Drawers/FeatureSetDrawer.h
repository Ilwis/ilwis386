#pragma once


ILWIS::NewDrawer *createFeatureSetDrawer(ILWIS::DrawerParameters *parms);

class FieldColor;

namespace ILWIS{

class _export FeatureSetDrawer : public SetDrawer {
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
		virtual NewDrawer *createElementDrawer(PreparationParameters *pp, ILWIS::DrawerParameters* parms) const = 0;

	protected:
		void displayOptionMask(CWnd *parent);
		void displayOptionSingleColor(CWnd *parent);
		String mask;
		bool useMask;
		Color singleColor;

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