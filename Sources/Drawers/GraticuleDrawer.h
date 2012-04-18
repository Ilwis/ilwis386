#pragma once


ILWIS::NewDrawer *createGraticuleDrawer(ILWIS::DrawerParameters *parms);
ILWIS::NewDrawer *createGraticuleLine(ILWIS::DrawerParameters *parms);

class FieldColor;
enum LineDspType;

namespace ILWIS{

	class _export GraticuleDrawer : public ComplexDrawer {
	public:
		ILWIS::NewDrawer *createGraticuleDrawer(DrawerParameters *parms);

		GraticuleDrawer(DrawerParameters *parms);
		virtual ~GraticuleDrawer();
		bool draw( const CoordBounds& cbArea=CoordBounds()) const;
		void prepare(PreparationParameters *pp);
		void setGridSpacing(double v);
		double getGridSpacing() const{ return rDist;}
		GeneralDrawerProperties *getProperties();


	protected:
		void AddGraticuleLine(const CoordSystem &csy, const LatLon& ll1, const LatLon& ll2);
		void displayOptionSetLineStyle(CWnd *parent);
		void gridOptions(CWnd *parent);
		void displayOptionGrid3D(CWnd *parent);
		void gridActive(void *value, LayerTreeView *v);
		void prepareGrid( const CoordSystem &csy, double rDist, const LatLon& cMax, const LatLon& cMin );
		String store(const FileName& fnView, const String& parenSection) const;
		void load(const FileName& fnView, const String& parenSection);
		void prepareChildDrawers(PreparationParameters *parms);
		void calcBounds(const CoordBounds& cbMap, const CoordSystem& cs,  LatLon& llMin, LatLon& llMax);


		double          rDist;
		LineProperties	lproperties;
	};

	class GraticuleLine: public LineDrawer {
	public:
		GraticuleLine(DrawerParameters *parms);
		virtual ~GraticuleLine();
		bool draw( const CoordBounds& cbArea=CoordBounds()) const;
		void prepare(PreparationParameters *pp);
		void addDataSource(void *c, int options = 0);
	private:
	};

	//class GraticuleLineStyleForm: public DisplayOptionsForm
	//{
	//public:
	//	GraticuleLineStyleForm(CWnd *par, GraticuleDrawer *gdr);
	//	void apply();
	//private:
	//	FieldReal *fi;
	//	FieldLineType *flt;
	//	FieldColor *fc;
	//};

	
}