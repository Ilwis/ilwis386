#pragma once

ILWIS::NewDrawer *createPolygonDrawer(ILWIS::DrawerParameters *parms);

namespace ILWIS{

class PolygonDrawer : public FeatureDrawer {
	public:
		PolygonDrawer(ILWIS::DrawerParameters *parms);
		~PolygonDrawer();
		virtual void draw(bool norecursion = false);
		void prepare(PreparationParameters *);
	protected:
		PolygonDrawer(DrawerParameters *parms, const String& name);
		void prepareList();
		void fff();

		double **exterior;
		vector<double **> holes; 
		int exteriorPoints;
		vector<int> holePoints;
		GLUtesselator *tesselator;
		int listIndex;

	};
}