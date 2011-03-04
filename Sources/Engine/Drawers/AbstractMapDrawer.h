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
		Table getAtttributeTable() const;
		void setAttributeTable(const Table& tbl);
		Column getAtttributeColumn() const;
		void setAttributeColumn(const String& name);
		bool useAttributeTable() const;
		void setUseAttributeTable(bool yesno);
		RangeReal getStretchRangeReal() const;
		bool useInternalDomain() const;
		virtual String description() const;
		virtual String iconName(const String& subtype="?") const;
//		Ilwis::Record AbstractMapDrawer::rec(const Coord& crd);
		virtual void inactivateOtherPalettes(Palette * palette);

	protected:
		Table attTable;
		Column attColumn;
		bool useAttTable;
		bool internalDomain;
		BaseMap bm;

		AbstractMapDrawer(DrawerParameters *parms, const String& name);
		String store(const FileName& fnView, const String& parenSection) const;
		void load(const FileName& fnView, const String& parenSection);
	};


	#define CMAPDRW(root,i) (dynamic_cast<AbstractMapDrawer *>(root->getDrawer(i)))

}