#pragma once;

#include "ComplexDrawer.h"

class MapCompositionDoc;


namespace ILWIS {
	class DrawerContext;
	class SelectionRectangle;

	class _export RootDrawer : public ComplexDrawer {
	public:
		RootDrawer(MapCompositionDoc *doc);
		~RootDrawer();
		virtual void prepare(PreparationParameters *pp);
		String addDrawer(NewDrawer *drw);
		void addCoordBounds(const CoordSystem& _cs, const CoordBounds& cb, bool overrule=true);
		bool draw( const CoordBounds& cb=CoordBounds()) const;
		//HTREEITEM configure(LayerTreeView  *tv, HTREEITEM parent);
		void timedEvent(UINT timerID);
		String store(const FileName& fnView, const String parenSection) const;
		void load(const FileName& fnView, const String parenSection);
		CoordBounds getCoordBoundsView() const { return cbView; }
		CoordBounds getCoordBoundsZoom() const; 
		CoordBounds getMapCoordBounds() const;
		CoordSystem getCoordinateSystem() const { return cs;}
		RowCol getViewPort() const { return pixArea; }
		void setCoordinateSystem(const CoordSystem& _cs, bool overrule=false) ;
		void setCoordBoundsView(const CoordSystem& _cs, const CoordBounds& _cb, bool overrule=false); 
		void setCoordBoundsZoom(const CoordBounds& _cb);
		void setCoordBoundsMap(const CoordBounds& cb);
		void setZoom(const CRect& rct);
		void setViewPort(const RowCol& rc);
		Coord screenToWorld(const RowCol& rc);
		RowCol worldToScreen(const Coord& crd);
		double getAspectRatio() const;
		DrawerContext *getDrawerContext() { return drawercontext; }
		DrawerContext *getDrawerContext() const { return drawercontext; }
		void setSelectionDrawer(SelectionRectangle *selDraw);
		int getZIndex() const;
		void setZIndex(int);

		//void clear();
		void set3D(bool yeno);
		bool is3D() const;
		void setViewPoint(const Coord& c);
		void setEyePoint(const Coord& c);
		Coord getViewPoint() const;
		Coord getEyePoint() const;
		double getFakeZ() const;
		void setBitmapRedraw(bool yesno);
		void setRotationAngles(double rx, double ry, double rz);
		void getRotationAngles(double& rx, double& ry, double& rz);
		double getZoom3D() const;
		void setZoom3D(double v);

	private:
		void setProjection(const CoordBounds& cb) const;
		void setEyePoint();
		void modifyCBZoomView(double dv, double dz, double f);
		void calcCanvas();
		void addDataSource(void *);
		void SetthreeD(void *v, LayerTreeView *tv);
		void debug();
		void swapBufferToScreen(const CRect& rct) const;
		void saveScreenBuffer(const CRect& rct);
		void bitmapBufferRedraw() const;
		void init3D();

		CoordBounds cbView;
		CoordBounds cbZoom;
		CoordBounds cbMap;
		CoordSystem cs;
		RowCol pixArea;
		bool threeD;
		Coordinate eyePoint;
		Coordinate viewPoint;
		double fakeZ;
		double aspectRatio;
		double rotX, rotY, rotZ;
		double zoom3D;
		SelectionRectangle *selectionDrawer;
		float * swapBitmap;
		ILWIS::DrawerContext *drawercontext;
		bool useBitmapRedraw;
	};
}