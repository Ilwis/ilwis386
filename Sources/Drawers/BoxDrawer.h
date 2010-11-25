#pragma once

class FieldColor;

ILWIS::NewDrawer *createBoxDrawer(ILWIS::DrawerParameters *parms);
#define BOX_DRAWER_ID 497

namespace ILWIS{

class BoxDrawer : public SimpleDrawer {
	public:
		BoxDrawer(ILWIS::DrawerParameters *parms);
		~BoxDrawer();
		virtual bool draw(bool norecursion = false, const CoordBounds& cbArea=CoordBounds()) const;
		void prepare(PreparationParameters *);
		void setDrawColor(const Color& col);
		void setBox(const CoordBounds& cbOuter, const CoordBounds& cbInner = CoordBounds());
	
	protected:
		void clear();
	
		vector< CoordBounds> boxes;
		Color drawColor;
	};





}