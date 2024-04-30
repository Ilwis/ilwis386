#pragma once

#include "ComplexDrawer.h"
#include "Engine\SpatialReference\Gr.h"

class MapCompositionDoc;
class OpenGLText;

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
		void addCoordBounds(const CoordSystem& _cs, const CoordBounds& cb, bool extend=true);
		bool draw(const CoordBounds&) const;
		void timedEvent(UINT timerID);
		String store(const FileName& fnView, const String section) const;
		void load(const FileName& fnView, const String section);
		CoordBounds getCoordBoundsView() const { return cbView; }
		CoordBounds getCoordBoundsZoom() const; 
		CoordBounds getMapCoordBounds() const;
		CoordBounds getCoordBoundsViewExt() const;
		CoordBounds getCoordBoundsZoomExt() const; 
		CoordBounds getMapCoordBoundsExt() const;
		CoordSystem getCoordinateSystem() const { return cs;}
		GeoRef getGeoReference() const { return gr; }
		RowCol getViewPort() const { return pixArea; }
		void setCoordinateSystem(const CoordSystem& _cs, bool overrule=false);
		void setGeoreference(const GeoRef& _gr, bool overruleMapBounds=false);
		void clearGeoreference();
		void setCoordBoundsView(const CoordBounds& _cb, bool overrule=false); 
		void setCoordBoundsZoom(const CoordBounds& _cb);
		void setCoordBoundsMap(const CoordBounds& cb);
		void setCoordBoundsViewExt(CoordBounds & _cb); // for Edit/Copy to temporarily override and suppress gray areas
		void setCoordBoundsZoomExt(CoordBounds & _cb); // for Edit/Copy to temporarily override and suppress gray areas
		void setZoom(const CRect& rct);
		void setViewPort(const RowCol& rc, bool fNoZoom); // noZoom preserves the zoomfactor when resizing the window; set to "false" for Edit/Copy
		Coord screenToOpenGL(const RowCol& rc); // rc is a screen position; it will be converted to the RootDrawer's grf/cs for OpenGL use
		Coord screenToWorld(const RowCol& rc); // rc is a screen position; it will be converted to the RootDrawer's cs
		RowCol OpenGLToScreen(const Coord& crd); // crd is an OpenGL coordinate expressed in the RootDrawer's grf/cs; it will be converted to a screen position
		RowCol WorldToScreen(const Coord& crd); // crd is a coordinate expressed in the RootDrawer's cs; it will be converted to a screen position
		bool fConvNeeded(const CoordSystem& _cs) const;
		Coord glConv(const CoordSystem& _cs, const Coord& _crd) const; // _crd is expressed in _cs (given by map); it will be converted to the RootDrawer's grf/cs for OpenGL use
		Coord glConv(const Coord& _crd) const; // _crd is expressed in the RootDrawer's cs; it will be converted to the RootDrawer's grf/cs for OpenGL use
		vector<Coord> glConv(const CoordSystem& _cs, const vector<Coord> & _crds) const;
		vector<Coord> glConv(const vector<Coord> & _crds) const;
		Coord glToWorld(const CoordSystem& _cs, const Coord& _crd) const; // _crd is an OpenGL coordinate expressed in the RootDrawer's grf/cs; it will be converted to _cs (a map's)
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
		void setAnnotationFont(OpenGLText *f);
		void setAnnotationWhitespace(bool fWhitespace);
		void setAnnotationBorder(bool fBorder);
		bool fWhitespace() const;
		bool fUsesTopDrawer() const;

		//void clear();
		void set3D(bool yeno);
		bool is3D() const;
		void setViewPoint(const Coord& c);
		Coord getViewPoint() const;
		void setRotationAngles(double rx, double ry, double rz);
		void getRotationAngles(double& rx, double& ry, double& rz);
		void setTranslate(double tx, double ty, double tz);
		void getTranslate(double& tx, double& ty, double& tz);
		void deltaTranslate(double deltax, double deltay, double deltaz);
		double getZoom3D() const;
		void setZoom3D(double v);
		void deltaZoom3D(double deltav);
		void SetSkyColor(Color & clr);

	private:
		void modifyZoomX(double rFactor);
		void modifyZoomY(double rFactor);
		void RecenterZoomHorz(CoordBounds & cbZoom, const CoordBounds & cbMap);
		void RecenterZoomVert(CoordBounds & cbZoom, const CoordBounds & cbMap);
		void addDataSource(void *);
		void SetthreeD(void *v, LayerTreeView *tv);
		void debug();
		void initLight();
		void SetAmbientColor();
		void SetDiffuseColor();
		void SetSpecularColor();
		void RecomputeAnnotationBorder();

		CoordBounds cbView; // Meant for computing scrollbars, cbView == max(cbMap,cbZoom), considering the x and y direction independently. Its x/y aspect ratio is not directly related to the one of cbZoom or cbMap.
		CoordBounds cbZoom; // The bbox coordinates visible in the current MapWindow, from bottomleft to topright. Its x/y aspect ratio is always the same as the one of the MapWindow.
		CoordBounds cbMap; // The bbox coordinates of the current map-composition.
		CoordBounds cbViewExt;
		CoordBounds cbZoomExt;
		CoordBounds cbMapExt;
		bool fAnnotationWhitespace;
		bool fAnnotationBorder;
		OpenGLText *annotationFont;
		Extension ext;
		CoordSystem cs;
		GeoRef gr;
		bool fUseGeoRef;
		RowCol pixArea;
		bool threeD;
		Coord viewPoint;
		double windowAspectRatio;
		double mapAspectRatio;
		double rotX, rotY, rotZ;
		double translateX, translateY, translateZ;
		double zoom3D;
		NewDrawer *topDrawer;
		ILWIS::DrawerContext *drawercontext;
		NewDrawer *backgroundDrawer; 
		Color skyColor;
		bool fDrawing;
		bool fRequestAnnotationRecompute;
	};
}