#pragma once

class FieldRepresentation;
class SetChecks;

namespace ILWIS{

enum FontStyle { FS_ITALIC    = 0x0001, 
                 FS_UNDERLINE	= 0x0002,
                 FS_STRIKEOUT	= 0x0004 };

class DrawingColor;

class _export AbstractMapDrawer : public AbstractObjectDrawer {
	friend class AbstractMapDrawerForm;

	public:
		enum StretchMethod { smLINEAR, smLOGARITHMIC };

		AbstractMapDrawer(DrawerParameters *parms);
		virtual ~AbstractMapDrawer();
		virtual void prepare(PreparationParameters *pp);
		void setDataSource(void *bm, int options=0);
		BaseMap getBaseMap() const;
		Representation getRepresentation() const;
		void setRepresentation(const Representation& rp);
		bool isLegendUsefull() const;
		RangeReal getStretchRangeReal() const;
		void setStretchRangeReal(const RangeReal& rr);
		RangeInt getStretchRangeInt() const;
		void setStretchRangeInt(const RangeInt& rr);
		bool isStretched() const;
		RangeReal getLegendRange() const;
		void setLegendRange(const RangeReal& rr);
		Table getAtttributeTable() const;
		void setAttributeTable(const Table& tbl);
		Column getAtttributeColumn() const;
		void setAttributeColumn(const String& name);
		bool useAttributeTable() const;
		void setUseAttributeTable(bool yesno);
		StretchMethod getStretchMethod() const;
		void setStretchMethod(StretchMethod sm);
		DrawingColor *drwColor() const;

	protected:
		Representation rpr;
		RangeReal rrStretch;
		RangeInt riStretch;
		RangeReal rrLegendRange;
		bool stretched;
		StretchMethod stretchMethod;
		Table attTable;
		Column attColumn;
		bool useAttTable;
		DrawingColor *drawColor;
		SetChecks *colorCheck;

		HTREEITEM  configure(LayerTreeView  *tv, HTREEITEM parent);
		virtual HTREEITEM SetColors(LayerTreeView  *tv, HTREEITEM parent,const BaseMap& bm);
		AbstractMapDrawer(DrawerParameters *parms, const String& name);

	};


	class RepresentationForm : public DisplayOptionsForm {
		public:
		RepresentationForm(CWnd *wPar, AbstractMapDrawer *dr);
		void apply(); 
	private:
		String rpr;
		FieldRepresentation *fldRpr;

	};

	class ChooseAttributeColumnForm : public DisplayOptionsForm {
		public:
		ChooseAttributeColumnForm(CWnd *wPar, AbstractMapDrawer *dr);
		void apply(); 
	private:
		Table attTable;
		String attColumn;
	};

	class SetStretchForm : public DisplayOptionsForm {
		public:
		SetStretchForm(CWnd *wPar, AbstractMapDrawer *dr);
		void apply(); 
	private:
		RangeReal rr;
		RangeInt ri;
		bool fReal;
		FieldRangeReal *strReal;
		FieldRangeInt *strInt;

	};

}