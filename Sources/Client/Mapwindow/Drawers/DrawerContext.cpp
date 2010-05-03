#include "headers\toolspch.h"
#include "Client\Mapwindow\Drawers\DrawerContext.h"

using namespace ILWIS;

DrawerContext::DrawerContext() : mhRC(0),aspectRatio(0),xview(0),yview(0),hview(0),wview(0){
}

bool DrawerContext::initOpenGL(CDC *dc) {
	if ( mhRC)
		return true;

	PIXELFORMATDESCRIPTOR pfd;
	ZeroMemory( &pfd, sizeof( pfd ) );
	pfd.nSize = sizeof( pfd );
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL |
				  PFD_DOUBLEBUFFER;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 24;
	pfd.cDepthBits = 16;
	pfd.iLayerType = PFD_MAIN_PLANE;
	int iFormat = ChoosePixelFormat( dc->m_hDC, &pfd );
	SetPixelFormat( dc->m_hDC, iFormat, &pfd );

	mhRC = wglCreateContext( dc->m_hDC );    
	wglMakeCurrent( dc->m_hDC, mhRC );
	glDisable(GL_DEPTH_TEST);
	glViewport(0,0,10000,10000);
	glClearColor(0.75,0.75,0.75,0.0);

	return true;
}

void DrawerContext::setViewPort(const RowCol& rc) {
	pixArea = rc;
	//if ( !mhRC || aspectRatio == 0)
	//	return ;
	//double delta = 1.0;
	//if ( aspectRatio <= 1.0 ) {
	//	if ( hview != rc.Row){
	//		delta = rc.Row * aspectRatio;
	//		xview = rc.Col / 2.0 - delta/2.0;
	//		yview = 0;
	//		wview  = delta;
	//		hview = rc.Row;
	//	}
	//} else {
	//	if ( wview != rc.Col){
	//		delta = rc.Col * aspectRatio;
	//		xview = 0;
	//		yview = rc.Row / 2.0 - delta/2.0;
	//		wview  = rc.Col;
	//		hview = delta;
	//	}
	//}
	//glViewport(xview,yview,wview, hview);
	glViewport(0,0,rc.Col, rc.Row);
}

DrawerContext::~DrawerContext() {
	clear();
}

void DrawerContext::clear() {

    if ( mhRC )
    {
        wglMakeCurrent( NULL, NULL );
        wglDeleteContext( mhRC );
		mhRC = 0;
    }

}

void DrawerContext::setCoordinateSystem(const CoordSystem& _cs, bool overrule){
	if (overrule || cs->fUnknown()) {
		cs = _cs;
	}
}

void DrawerContext::setCoordBoundsView(const CoordBounds& _cb, bool overrule){
	if ( overrule || cbView.fUndef()) {
		cbView = _cb;
		cbZoom = _cb;
		cbZoomWorld = toWorld(_cb);

	} else {
		cbView += _cb;
	}
	aspectRatio = cbView.width()/ cbView.height();
}

void DrawerContext::setCoordBoundsZoom(const CoordBounds& cb) {
	cbZoom = cb;
}

CoordBounds  DrawerContext::toWorld(const CoordBounds& cb) {
	if ( aspectRatio <= 1.0) {
		double vx = (double)pixArea.Row / pixArea.Col;
		double y1 = 2.0 * ( cb.MinY() - cbView.MinY()) / cbView.height() - 1.0;
		double y2 = 2.0 * ( cb.MaxY() - cbView.MinY()) / cbView.height() - 1.0;
		double x1 =  (2.0 *( cb.MinX() - cbView.MinX()) / cbView.width() - 1.0 ) * aspectRatio * vx;
		double x2 =  (2.0 *( cb.MaxX() - cbView.MinX()) / cbView.width() - 1.0 ) * aspectRatio * vx; 
		cbZoomWorld =  CoordBounds(Coord(x1,y1), Coord(x2,y2));
	}
	return cbZoomWorld;
}


double DrawerContext::getAspectRatio() const {
	return aspectRatio;
}

Coord DrawerContext::screenToWorld(const RowCol& rc) {

	int port[4];
	Coord c;
	//double zoom = cbZoom.width() / cbView.width();
	glGetIntegerv(GL_VIEWPORT, port);
	double vx = (double)pixArea.Row / pixArea.Col;
	double colIntern = rc.Col - ( 1.0 + cbZoomWorld.MinX()) * port[2]/2.0;
	double rowIntern = rc.Row - ( 1.0 + cbZoomWorld.MinY()) * port[3]/2.0;
	double deltax1 = colIntern/port[2];
	double deltay1 = rowIntern/port[3];
	deltax1 = cbZoom.MinX() + deltax1 * cbZoom.width();
	deltay1 = cbZoom.MinY() + deltay1 * cbZoom.height();
	double x1 =  (2.0 *( deltax1 - cbView.MinX()) / cbView.width() - 1.0 ) * aspectRatio * vx;
	double y1 = 2.0 * ( deltay1 - cbView.MinY()) / cbView.height() - 1.0;


	return Coord(x1,y1);
}

RowCol DrawerContext::worldToScreen(const Coord& crd){
	int vp[4];
	glGetIntegerv(GL_VIEWPORT, vp);
	int x = vp[0] + crd.x * vp[2];
	int y = vp[1] + crd.y * vp[3];
	return RowCol(y,x);
}

void DrawerContext::setZoom(const CRect& rect) {
	Coord c1 = screenToWorld(RowCol(rect.top, rect.left));
	Coord c2 = screenToWorld(RowCol(rect.bottom, rect.right));
	cbZoomWorld = CoordBounds(c1,c2);
	cbZoom.cMin.x = cbZoom.cMin.x + (1.0 + c1.x * cbZoom.width());
	cbZoom.cMin.y = cbZoom.cMin.y + (1.0 + c1.y * cbZoom.height());
	cbZoom.cMax.x = cbZoom.cMax.x + (1.0 + c2.x * cbZoom.width());
	cbZoom.cMax.y = cbZoom.cMax.y + (1.0 + c2.y * cbZoom.height());
}

CoordBounds DrawerContext::getCoordBoundsZoom(bool world) const  {
	if ( world)
		return cbZoomWorld;
	return cbZoom;
}
