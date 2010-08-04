#pragma once

class FieldColor;

ILWIS::NewDrawer *createLineFeatureDrawer(ILWIS::DrawerParameters *parms);

namespace ILWIS{

class LineFeatureDrawer : public LineDrawer {
	public:
		LineFeatureDrawer(ILWIS::DrawerParameters *parms);
		~LineFeatureDrawer();
		virtual bool draw(bool norecursion = false, const CoordBounds& cbArea=CoordBounds()) const;
		void prepare(PreparationParameters *);
		void setDataSource(void *f);
		void addDataSource(void *f,int options=0);
	protected:
		LineFeatureDrawer(DrawerParameters *parms, const String& name);
		Feature *feature;

	};

}