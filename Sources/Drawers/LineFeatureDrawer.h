#pragma once

class FieldColor;

ILWIS::NewDrawer *createLineFeatureDrawer(ILWIS::DrawerParameters *parms);

namespace ILWIS{

class _export LineFeatureDrawer : public LineDrawer {
	public:
		LineFeatureDrawer(ILWIS::DrawerParameters *parms);
		~LineFeatureDrawer();
		virtual bool draw( const CoordBounds& cbArea=CoordBounds()) const;
		void prepare(PreparationParameters *);
		void setDataSource(void *f);
		void addDataSource(void *f,int options=0);
		Feature *getFeature() const;
		void* getDataSource() const;
	protected:
		LineFeatureDrawer(DrawerParameters *parms, const String& name);
		Feature *feature;
		DomainType dmt;

	};

}