#include "headers\toolspch.h"
#include "Client\Mapwindow\Drawers\DrawerContext.h"

using namespace ILWIS;

DrawerContext::DrawerContext() : mhRC(0),aspectRatio(0){
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

	CWnd * wnd = dc->GetWindow();
	CRect rct;
	wnd->GetClientRect(&rct);
	RowCol rc(rct.Height(), rct.Width());
	setViewPort(rc);

	return true;
}

void DrawerContext::setViewPort(const RowCol& rc) {
	pixArea = rc;
	glViewport(0,0,rc.Col, rc.Row);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
    glOrtho(cbZoom.cMin.x,cbZoom.cMax.x,cbZoom.cMin.y,cbZoom.cMax.y,-1,1.0);
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
		cbMap = _cb;
		aspectRatio = cbMap.width()/ cbMap.height();
		double pxar = (double)pixArea.Col / pixArea.Row;
		if ( pxar >= 1.0) {
			double w = _cb.width();
			double h = _cb.height();
			double pixwidth = (double)pixArea.Row * aspectRatio;
			double fracofWidth = 1.0 - (pixArea.Col - pixwidth) / pixArea.Col;
			double crdWidth = w / fracofWidth;
			double deltax = (crdWidth - w) / 2.0;
			cbView =  CoordBounds(Coord(_cb.MinX() - deltax,_cb.MinY()), 
			                      Coord(_cb.MaxX() + deltax,_cb.MaxY()));
			cbZoom = cbView;
			glMatrixMode(GL_PROJECTION);
			glLoadIdentity();
			glOrtho(cbView.cMin.x,cbView.cMax.x,cbView.cMin.y,cbView.cMax.y,-1,1.0);
		}

	} else {
		cbView += _cb;
		aspectRatio = cbView.width()/ cbView.height();
	}
	
}

void DrawerContext::setCoordBoundsZoom(const CoordBounds& cb) {
	cbZoom = cb;
}

CoordBounds DrawerContext::getMapCoordBoubnds() const{
	return cbMap;
}

double DrawerContext::getAspectRatio() const {
	return aspectRatio;
}

Coord DrawerContext::screenToWorld(const RowCol& rc) {

	double fractX = (double)rc.Col / pixArea.Col;
	double fractY = (double)rc.Row / pixArea.Row;
	double x = cbZoom.MinX() + cbZoom.width() * fractX;
	double y = cbZoom.MinY() + cbZoom.height() * fractY;

	return Coord(x,y);
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
	cbZoom = CoordBounds(c1,c2);

	glMatrixMode(GL_PROJECTION);
	glOrtho(cbZoom.cMin.x,cbZoom.cMax.x,cbZoom.cMin.y,cbZoom.cMax.y,-1,1.0);
}

CoordBounds DrawerContext::getCoordBoundsZoom() const  {
	return cbZoom;
}
