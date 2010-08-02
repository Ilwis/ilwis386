#pragma once

class FieldRepresentation;
class SetChecks;
class FieldColumn;

namespace ILWIS{

class DrawingColor;

class _export AbstractMapDrawer : public AbstractObjectDrawer {
	friend class AbstractMapDrawerForm;

	public:
		AbstractMapDrawer(DrawerParameters *parms);
		virtual ~AbstractMapDrawer();
		virtual void prepare(PreparationParameters *pp);
		void addDataSource(void *bm, int options=0);
		BaseMap getBaseMap() const;
		Representation getRepresentation() const;
		void setRepresentation(const Representation& rp);
		Table getAtttributeTable() const;
		void setAttributeTable(const Table& tbl);
		Column getAtttributeColumn() const;
		void setAttributeColumn(const String& name);
		bool useAttributeTable() const;
		void setUseAttributeTable(bool yesno);
		RangeReal getStretchRangeReal() const;
		bool useInternalDomain() const;

	protected:
		Table attTable;
		Column attColumn;
		bool useAttTable;
		bool internalDomain;
		BaseMap bm;

		HTREEITEM  configure(LayerTreeView  *tv, HTREEITEM parent);
		AbstractMapDrawer(DrawerParameters *parms, const String& name);
		void displayOptionAttColumn(CWnd *parent);
		void setColumnCheckumn(void *w, LayerTreeView *view);
		void setInfoMode(void *v,LayerTreeView *tv);

	};


	class ChooseAttributeColumnForm : public DisplayOptionsForm {
		public:
		ChooseAttributeColumnForm(CWnd *wPar, AbstractMapDrawer *dr);
		void apply(); 
	private:
		Table attTable;
		String attColumn;
		FieldColumn *fc;
	};

}