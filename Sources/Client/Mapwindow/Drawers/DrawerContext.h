#pragma once

#include <gl/gl.h>
#include <gl/glu.h>
#include "Engine\SpatialReference\Coordsys.h"
//#include "Engine\Map\Feature.h"
#include "Engine\Domain\dm.h"

class MapCompositionDoc;

namespace ILWIS {
	class RootDrawer;
	class _export DrawerContext {
	public:
		~DrawerContext();
		DrawerContext(MapCompositionDoc *, RootDrawer *dr);
		CoordBounds getCoordBoundsView() const { return cbView; }
		CoordBounds getCoordBoundsZoom() const; 
		CoordBounds getMapCoordBounds() const;
		CoordSystem getCoordinateSystem() const { return cs;}
		RowCol getViewPort() const { return pixArea; }
		void setCoordinateSystem(const CoordSystem& _cs, bool overrule) ;
		void setCoordBoundsView(const CoordBounds& _cb, bool overrule); 
		void setCoordBoundsZoom(const CoordBounds& _cb);
		void setCoordBoundsMap(const CoordBounds& cb);
		void setZoom(const CRect& rct);
		void setViewPort(const RowCol& rc);
		Coord screenToWorld(const RowCol& rc);
		RowCol worldToScreen(const Coord& crd);
		double getAspectRatio() const;
		bool initOpenGL(CDC *dc);
		void clear();
		RootDrawer *getRootDrawer() const;
		void setRootDrawer(RootDrawer *dr);
		void set3D(bool yeno);
		bool is3D() const;
		void setViewPoint(const Coord& c);
		void setEyePoint(const Coord& c);
		Coord getViewPoint() const;
		Coord getEyePoint() const;
	    double getFakeZ() const;
		MapCompositionDoc * getDocument() const;
		GLint getMaxTextureSize() const { return maxTextureSize; };

		void TakeContext(bool urgent);
		void ReleaseContext();
		bool isUrgentRequestWaiting();
		void InvalidateWindow();
		bool fUrgentRequestWaiting; // intentionally public!! used in volatile* to abort texture generation in thread

	private:
		void setProjection(const CoordBounds& cb);
		void setEyePoint();
		void modifyCBZoomView(double dv, double dz, double f);

		CoordBounds cbView;
		CoordBounds cbZoom;
		CoordBounds cbMap;
		CoordSystem cs;
		RowCol pixArea;
		double aspectRatio;
		RootDrawer *rootDrawer;
		bool threeD;
		Coordinate eyePoint;
		Coordinate viewPoint;
		double fakeZ;
		MapCompositionDoc *doc;
		GLint maxTextureSize;

		HDC m_hdc;
		HGLRC m_hrc;
		CWnd * m_wnd;
		bool glBusy;
		bool fGLInitialized;
	};
}
