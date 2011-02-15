#pragma once

class FieldRepresentation;
class SetChecks;
class FieldColumn;

namespace ILWIS{

class DrawingColor;
class Palette;

class _export AbstractMapDrawer : public ComplexDrawer {
	friend class AbstractMapDrawerForm;

	public:
		AbstractMapDrawer(DrawerParameters *parms);
		virtual ~AbstractMapDrawer();
		virtual void prepare(PreparationParameters *pp);
		void addDataSource(void *bm, int options=0);
		void *getDataSource() const;
		BaseMapPtr *getBaseMap() const;
		Representation getRepresentation() const;
		void setRepresentation(const Representation& rp);
		RangeReal getStretchRangeReal() const;
		bool useInternalDomain() const;
		HTREEITEM  configure(LayerTreeView  *tv, HTREEITEM parent);
		virtual String description() const;
		virtual String iconName(const String& subtype="?") const;
		Ilwis::Record AbstractMapDrawer::rec(const Coord& crd);
		virtual void inactivateOtherPalettes(Palette * palette);

	protected:
		bool internalDomain;
		BaseMap bm;

		AbstractMapDrawer(DrawerParameters *parms, const String& name);
		void displayOptionAttColumn(CWnd *parent);
		void setColumnCheckumn(void *w, LayerTreeView *view);
		void setInfoMode(void *v,LayerTreeView *tv);
		String store(const FileName& fnView, const String& parenSection) const;
		void load(const FileName& fnView, const String& parenSection);
	};


	#define CMAPDRW(root,i) (dynamic_cast<AbstractMapDrawer *>(root->getDrawer(i)))

}