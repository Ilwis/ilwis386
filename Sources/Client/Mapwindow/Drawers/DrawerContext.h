#pragma once

#include <gl/gl.h>
#include <gl/glu.h>
#include "Engine\SpatialReference\Coordsys.h"
//#include "Engine\Map\Feature.h"
#include "Engine\Domain\dm.h"

namespace ILWIS {
	class _export DrawerContext {
	public:
		~DrawerContext();
		DrawerContext();
		CoordBounds getCoordBoundsView() const { return cbView; }
		CoordBounds getCoordBoundsZoom() const; 
		CoordBounds getMapCoordBoubnds() const;
		CoordSystem getCoordinateSystem() const { return cs;}
		RowCol getViewPort() const { return pixArea; }
		void setCoordinateSystem(const CoordSystem& _cs, bool overrule) ;
		void setCoordBoundsView(const CoordBounds& _cb, bool overrule); 
		void setCoordBoundsZoom(const CoordBounds& _cb);
		void setZoom(const CRect& rct);
		void setViewPort(const RowCol& rc);
		Coord screenToWorld(const RowCol& rc);
		RowCol worldToScreen(const Coord& crd);
		double getAspectRatio() const;
		bool initOpenGL(CDC *dc);
		void clear();

	private:

		CoordBounds cbView;
		CoordBounds cbZoom;
		CoordBounds cbMap;
		CoordSystem cs;
		RowCol pixArea;
		double aspectRatio;
		HGLRC mhRC;
	};
}
