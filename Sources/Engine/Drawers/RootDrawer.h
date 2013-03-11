#pragma once;

#include "ComplexDrawer.h"
#include "Engine\SpatialReference\Gr.h"

class MapCompositionDoc;


namespace ILWIS {
	struct Extension{
		Extension() : left(0), right(0), top(0), bottom(0){}
		Extension(double l, double r, double t, double b) : left(l), right(r), top(t), bottom(b) {}

		double left,right,top,bottom;
	};
	class DrawerContext;

	class _export RootDrawer : public ComplexDrawer {
	public:
		RootDrawer();
		~RootDrawer();
		virtual void prepare(PreparationParameters *pp);
		String addDrawer(NewDrawer *drw, bool overrule=true);
		void addCoordBounds(const CoordSystem& _cs, const CoordBounds& cb, bool overrule=true);
		bool draw(const CoordBounds& cb=CoordBounds()) const;
		void timedEvent(UINT timerID);
		String store(const FileName& fnView, const String parenSection) const;
		void load(const FileName& fnView, const String parenSection);
		CoordBounds getCoordBoundsView() const { return cbView; }
		CoordBounds getCoordBoundsZoom() const; 
		CoordBounds getMapCoordBounds() const;
		CoordSystem getCoordinateSystem() const { return cs;}
		GeoRef getGeoReference() const { return gr; }
		RowCol getViewPort() const { return pixArea; }
		void setCoordinateSystem(const CoordSystem& _cs, bool overrule=false);
		void setGeoreference(const GeoRef& _gr, bool overruleMapBounds=false);
		void clearGeoreference();
		void setCoordBoundsView(const CoordBounds& _cb, bool overrule=false); 
		void setCoordBoundsZoom(const CoordBounds& _cb);
		void setCoordBoundsMap(const CoordBounds& cb);
		void setZoom(const CRect& rct);
		void setViewPort(const RowCol& rc);
		Coord screenToOpenGL(const RowCol& rc);
		Coord screenToWorld(const RowCol& rc);
		RowCol OpenGLToScreen(const Coord& crd);
		RowCol WorldToScreen(const Coord& crd);
		bool fConvNeeded(const CoordSystem& _cs) const;
		Coord glConv(const CoordSystem& _cs, const Coord& _crd) const;
		Coord glConv(const Coord& _crd) const;
		vector<Coord> glConv(const CoordSystem& _cs, const vector<Coord> & _crds) const;
		vector<Coord> glConv(const vector<Coord> & _crds) const;
		Coord glToWorld(const CoordSystem& _cs, const Coord& _crd) const;
		Coord glToWorld(const Coord& _crd) const;
		double getAspectRatio() const;
		DrawerContext *getDrawerContext() { return drawercontext; }
		DrawerContext *getDrawerContext() const { return drawercontext; }
		void setDrawerContext(DrawerContext * context) {drawercontext = context;};
		void setTopDrawer(NewDrawer *newDrawer);
		int getZIndex() const;
		void setZIndex(int);
		void setupDraw() const;
		NewDrawer *getBackgroundDrawer() const { return backgroundDrawer; }
		Extension extension() const;
		void setExtension(const Extension& ext);
		

		//void clear();
		void set3D(bool yeno);
		bool is3D() const;
		void setViewPoint(const Coord& c);
		void setEyePoint(const Coord& c);
		Coord getViewPoint() const;
		Coord getEyePoint() const;
		double getFakeZ() const;
		void setRotationAngles(double rx, double ry, double rz);
		void getRotationAngles(double& rx, double& ry, double& rz);
		void setTranslate(double tx, double ty, double tz);
		void getTranslate(double& tx, double& ty, double& tz);
		double getZoom3D() const;
		void setZoom3D(double v);

	private:
		void setEyePoint();
		void modifyCBZoomView(double dv, double dz, double f);
		void calcCanvas();
		void addDataSource(void *);
		void SetthreeD(void *v, LayerTreeView *tv);
		void debug();
		void initLight();
			void SetAmbientColor();

	void SetDiffuseColor();
	void SetSpecularColor();

		CoordBounds cbView;
		CoordBounds cbZoom;
		CoordBounds cbMap;
		Extension ext;
		CoordSystem cs;
		GeoRef gr;
		bool fUseGeoRef;
		RowCol pixArea;
		bool threeD;
		Coordinate eyePoint;
		Coordinate viewPoint;
		double fakeZ;
		double windowAspectRatio;
		double mapAspectRatio;
		double rotX, rotY, rotZ;
		double translateX, translateY, translateZ;
		double zoom3D;
		NewDrawer *topDrawer;
		ILWIS::DrawerContext *drawercontext;
		NewDrawer *backgroundDrawer; 
		bool initRestore;
	};
}