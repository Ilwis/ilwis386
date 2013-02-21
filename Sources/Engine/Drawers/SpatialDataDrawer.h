#pragma once

#include "Engine\Map\Raster\MapList\maplist.h"
#include "Engine\Base\DataObjects\ObjectCollection.h"

class FieldRepresentation;
class SetChecks;
class FieldColumn;

namespace ILWIS{

class DrawingColor;

class _export SpatialDataDrawer : public ComplexDrawer {
	friend class SpatialDataDrawerForm;

	public:
		SpatialDataDrawer(DrawerParameters *parms);
		virtual ~SpatialDataDrawer();
		virtual void prepare(PreparationParameters *pp);
		void addDataSource(void *bm, int options=0);
		void *getDataSource() const;
		BaseMapPtr *getBaseMap(int index = 0) const;
		IlwisObjectPtr *SpatialDataDrawer::getObject() const;
		Representation getRepresentation(int index = 0) const;
		void setRepresentation(const Representation& rp);
		Table getAtttributeTable(int index = 0) const;
		void setAttributeTable(const Table& tbl);
		Column getAtttributeColumn() const;
		void setAttributeColumn(const String& name);
		bool useAttributeTable() const;
		void setUseAttributeTable(bool yesno);
		RangeReal getStretchRangeReal(int index = 0) const;
		RangeReal getRange() const;
		bool useInternalDomain() const;
		virtual String description() const;
		virtual String iconName(const String& subtype="?") const;
		virtual CoordBounds cb();
		IlwisObjectPtr *getSourceSupportObject(IlwisObject::iotIlwisObjectType type);
		set<Feature *> getSelectedFeatures() const;
		virtual void setTresholdColor(const Color&clr) {}
		virtual void setTresholdRange(const RangeReal& tr, bool single=false) {}

	protected:
		Table attTable;
		Column attColumn;
		bool useAttTable;
		bool internalDomain;
		BaseMap bm;
		ObjectCollection oc;
		MapList mpl;
		CoordBounds cbBounds;
		IlwisObject obj;

		SpatialDataDrawer(DrawerParameters *parms, const String& name);
		String store(const FileName& fnView, const String& parenSection) const;
		void load(const FileName& fnView, const String& parenSection);
	};


	#define CMAPDRW(root,i) (dynamic_cast<SpatialDataDrawer *>(root->getDrawer(i)))

}