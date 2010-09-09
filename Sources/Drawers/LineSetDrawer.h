#pragma once


ILWIS::NewDrawer *createLineSetDrawer(ILWIS::DrawerParameters *parms);

class FieldColor;
class FieldLineType;
enum LineDspType;

namespace ILWIS{

class LineSetDrawer : public FeatureSetDrawer {
	friend class LineStyleForm;

	public:
		ILWIS::NewDrawer *createLineSetDrawer(DrawerParameters *parms);

		LineSetDrawer(DrawerParameters *parms);
		virtual ~LineSetDrawer();
		HTREEITEM  configure(LayerTreeView  *tv, HTREEITEM parent);
		LineDspType getLineStyle() const;
		int getLineThickness() const;
		void prepare(PreparationParameters *parm);
		void modifyLineStyleItem(LayerTreeView  *tv, bool remove=false);

	protected:
		virtual NewDrawer *createElementDrawer(PreparationParameters *pp,ILWIS::DrawerParameters* parms) const;
		void setDrawMethod(DrawMethod method=drmINIT);
		void displayOptionSetLineStyle(CWnd *parent);
		String store(const FileName& fnView, const String& parenSection, SubType subtype) const;
		void load(const FileName& fnView, const String& parenSection);

		LineDspType linestyle;
		double linethickness;
		HTREEITEM styleItem;
	};

	class LineStyleForm: public DisplayOptionsForm
	{
	public:
		LineStyleForm(CWnd *par, LineSetDrawer *gdr);
		void apply();
	private:
		FieldReal *fi;
		FieldLineType *flt;
	};
}