#pragma once

class FieldRepresentation;
class SetChecks;

namespace ILWIS{

enum FontStyle { FS_ITALIC    = 0x0001, 
                 FS_UNDERLINE	= 0x0002,
                 FS_STRIKEOUT	= 0x0004 };

class _export AbstractMapDrawer : public AbstractObjectDrawer {
	friend class AbstractMapDrawerForm;

	public:
		AbstractMapDrawer(DrawerParameters *parms);
		virtual ~AbstractMapDrawer();
		virtual void prepare(PreparationParameters *pp);
		void setDataSource(void *bm);
		BaseMap getBaseMap() const;
		Representation getRepresentation() const;
		void setRepresentation(const Representation& rp);
		bool isLegendUsefull() const;
		RangeReal getStretchRange() const;
		void setStretchRange(const RangeReal& rr);
		bool isStretched() const;
		RangeReal getLegendRange() const;
		void setLegendRange(const RangeReal& rr);
		Table getAtttributeTable() const;
		void setAttributeTable(const Table& tbl);
		Column getAtttributeColumn() const;
		void setAttributeColumn(const String& name);
		bool useAttributeTable() const;
		void setUseAttributeTable(bool yesno);

	protected:
		Representation rpr;
		RangeReal rrStretch;
		RangeReal rrLegendRange;
		bool stretched;
		Table attTable;
		Column attColumn;
		bool useAttTable;
		SetChecks *colorCheck;

		HTREEITEM  configure(LayerTreeView  *tv, HTREEITEM parent);
		virtual HTREEITEM SetColors(LayerTreeView  *tv, HTREEITEM parent,const BaseMap& bm);
		AbstractMapDrawer(DrawerParameters *parms, const String& name);

	};


	class RepresentationForm : public FormBaseDialog {
		public:
		RepresentationForm(CWnd *wPar, AbstractMapDrawer *dr);
		int exec();
		afx_msg virtual void OnCancel(); 
	private:
		String rpr;
		AbstractMapDrawer *drw;
		LayerTreeView *view;
		FieldRepresentation *fldRpr;

	};

	class ChooseAttributeColumnForm : public FormBaseDialog {
		public:
		ChooseAttributeColumnForm(CWnd *wPar, AbstractMapDrawer *dr);
		int exec();
		afx_msg virtual void OnCancel(); 
	private:
		Table attTable;
		String attColumn;
		AbstractMapDrawer *drw;

	};

	class SetStretchForm : public FormBaseDialog {
		public:
		SetStretchForm(CWnd *wPar, AbstractMapDrawer *dr);
		int exec();
		afx_msg virtual void OnCancel(); 
	private:
		RangeReal rr;
		AbstractMapDrawer *drw;

	};

}