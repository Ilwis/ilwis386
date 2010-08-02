#include "headers\toolspch.h"
#include "Client\Mapwindow\Drawers\DrawerContext.h"
#include "RootDrawer.h"

using namespace ILWIS;

DrawerContext::DrawerContext(RootDrawer *dr) : mhRC(0),aspectRatio(0), rootDrawer(dr) {
}

bool DrawerContext::initOpenGL(CDC *dc) {
	if ( mhRC)
		return false;// no init needed, already done

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
	glClearColor(0.75,0.75,0.75,0.0);

	CWnd * wnd = dc->GetWindow();
	CRect rct;
	wnd->GetClientRect(&rct);
	RowCol rc(rct.Height(), rct.Width());
	setViewPort(rc);

	return true;
}

void DrawerContext::setViewPort(const RowCol& rc) {
	if (  aspectRatio  != 0 && pixArea.Col != iUNDEF) {
		// this code adapts the cbZoom if the window size changes
		if ( aspectRatio <= 1.0) { // y > x
			if ( rc.Col != pixArea.Col){ // make sure the zoomsize is changed if the cols changes
				double f = (double)rc.Col / pixArea.Col;
				double w = cbZoom.width();
				double deltax = w * f;
				cbZoom.cMax.x = cbZoom.cMin.x + deltax;
			}
			if ( rc.Row != pixArea.Row) { // make sure the zoomsize is changed if the cols change
				double f = (double)rc.Row / pixArea.Row;
				double w = cbZoom.width();
				double deltax = w / f;
				cbZoom.cMax.x = cbZoom.cMin.x + deltax;
			}
	
		} else { // x < y
			if ( rc.Row != pixArea.Row){
				double f = (double)rc.Row / pixArea.Row;
				double h = cbZoom.height();
				double deltay = h * f;
				cbZoom.cMax.y = cbZoom.cMin.y + deltay;
			}
			if ( rc.Col != pixArea.Col) {
				double f = (double)rc.Col / pixArea.Col;
				double h = cbZoom.height();
				double deltay = h / f;
				cbZoom.cMax.y = cbZoom.cMin.y + deltay;
			}
		}
	}
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
		double w = _cb.width();
		double h = _cb.height();
		double delta = 0;
		if ( aspectRatio <= 1.0) {
			double pixwidth = (double)pixArea.Row * aspectRatio;
			double fracofWidth = 1.0 - (pixArea.Col - pixwidth) / pixArea.Col;
			double crdWidth = w / fracofWidth;
			double delta = (crdWidth - w) / 2.0;
			cbView =  CoordBounds(Coord(_cb.MinX() - delta,_cb.MinY()), 
			                  Coord(_cb.MaxX() + delta,_cb.MaxY()));
		} else {
			double pixheight = (double)pixArea.Col / aspectRatio;
			double fracofHeight = 1.0 - abs(pixArea.Row - pixheight) / (double)pixArea.Row;
			double crdHeight = h / fracofHeight;
			double delta = (crdHeight - h) / 2.0;
			cbView =  CoordBounds(Coord(_cb.MinX(),_cb.MinY()  - delta), 
			                      Coord(_cb.MaxX(),_cb.MaxY()  + delta));

		}
		cbZoom = cbView;
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho(cbView.cMin.x,cbView.cMax.x,cbView.cMin.y,cbView.cMax.y,-1,1.0);

	} else {
		cbView += _cb;
		aspectRatio = cbView.width()/ cbView.height();
	}
	
}

void DrawerContext::setCoordBoundsZoom(const CoordBounds& cb) {
	cbZoom = cb;
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(cbZoom.cMin.x,cbZoom.cMax.x,cbZoom.cMin.y,cbZoom.cMax.y,-1,1.0);
}

void DrawerContext::setCoordBoundsMap(const CoordBounds& cb) {
	cbMap = cb;
}
CoordBounds DrawerContext::getMapCoordBounds() const{
	return cbMap;
}

double DrawerContext::getAspectRatio() const {
	return aspectRatio;
}

Coord DrawerContext::screenToWorld(const RowCol& rc) {

	double fractX = (double)rc.Col / pixArea.Col;
	double fractY = (double)rc.Row / pixArea.Row;
	double x = cbZoom.MinX() + cbZoom.width() * fractX;
	double y = cbZoom.MaxY() - cbZoom.height() * fractY;


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
	Coord c1,c2;
	if ( rect.Width() == 0 || rect.Height() == 0) { // case of clicking on the map in zoom mode
		Coord c = screenToWorld(RowCol(rect.top, rect.left));
		CoordBounds cb = cbZoom; // == cbView ? cbMap : cbZoom;
		double w = cb.width() / (2.0 * 1.41);
		double h = cb.height() / (2.0 * 1.41);
		c1.x = c.x - w;
		c1.y = c.y - h;
		c2.x = c.x + w;
		c2.y = c.y + h;

	}
	else {
		c1 = screenToWorld(RowCol(rect.top, rect.left));
		c2 = screenToWorld(RowCol(rect.bottom, rect.right));
	}
	CoordBounds cb(c1,c2);
	setCoordBoundsZoom(cb);
}

CoordBounds DrawerContext::getCoordBoundsZoom() const  {
	return cbZoom;
}

RootDrawer *DrawerContext::getRootDrawer() const {
	return rootDrawer;
}


