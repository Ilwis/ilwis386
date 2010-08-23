#include "Client\Headers\formelementspch.h"
#include "Client\Mapwindow\Drawers\DrawerContext.h"
#include "RootDrawer.h"

using namespace ILWIS;

DrawerContext::DrawerContext(RootDrawer *dr) : mhRC(0),aspectRatio(0), rootDrawer(dr), threeD(false),fakeZ(0) {
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

void DrawerContext::modifyCBZoomView(double dv, double dz, double f) {
	double deltaxv = dv * f;
	double deltaxz = dz * f;
	Coord cMiddle = cbZoom.middle();
	cbView.cMin.x = cMiddle.x - deltaxv / 2.0;
	cbView.cMax.x = cMiddle.x + deltaxv / 2.0;
	cbZoom.cMin.x = cMiddle.x - deltaxz / 2.0;
	cbZoom.cMax.x = cMiddle.x + deltaxz / 2.0;
}

void DrawerContext::setViewPort(const RowCol& rc) {
	if (  aspectRatio  != 0 && pixArea.Col != iUNDEF) {
		// this code adapts the cbZoom if the window size changes
		if ( aspectRatio <= 1.0) { // y > x
			if ( rc.Col != pixArea.Col){ // make sure the zoomsize is changed if the cols changes
				modifyCBZoomView(cbView.width(), cbZoom.width(),(double)rc.Col / pixArea.Col); 
			}
			if ( rc.Row != pixArea.Row) { // make sure the zoomsize is changed if the cols change
				modifyCBZoomView(cbView.width(), cbZoom.width(),(double)pixArea.Col / rc.Col); 
			}
	
		} else { // x < y
			if ( rc.Row != pixArea.Row){
				modifyCBZoomView(cbView.height(), cbZoom.height(),pixArea.Col / (double)rc.Col ); 

			}
			if ( rc.Col != pixArea.Col) {
				modifyCBZoomView(cbView.height(), cbZoom.height(),(double)rc.Col / pixArea.Col); 
			}
		}
	}
	pixArea = rc;
	glViewport(0,0,rc.Col, rc.Row);
	setProjection(cbZoom);
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
			cbView =  CoordBounds(Coord(_cb.MinX() - delta,_cb.MinY(),0), 
			                  Coord(_cb.MaxX() + delta,_cb.MaxY(),0));
		} else {
			double pixheight = (double)pixArea.Col / aspectRatio;
			double fracofHeight = 1.0 - abs(pixArea.Row - pixheight) / (double)pixArea.Row;
			double crdHeight = h / fracofHeight;
			double delta = (crdHeight - h) / 2.0;
			cbView =  CoordBounds(Coord(_cb.MinX(),_cb.MinY()  - delta,0), 
			                      Coord(_cb.MaxX(),_cb.MaxY()  + delta,0));

		}
		cbZoom = cbView;
		setViewPoint(cbView.middle());
		setEyePoint();
		setProjection(cbView);
	} else {
		cbView += _cb;
		aspectRatio = cbView.width()/ cbView.height();
	}
	fakeZ = cbView.width() * 0.001;
	
}

void DrawerContext::setCoordBoundsZoom(const CoordBounds& cb) {
	cbZoom = cb;
	setViewPoint(cbZoom.middle());
	setEyePoint();
	setProjection(cb);
}

void DrawerContext::setEyePoint() {
	eyePoint.x = viewPoint.x - cbZoom.width() ;
	eyePoint.y = viewPoint.y - cbZoom.height();
	eyePoint.z = cbZoom.width() * 2;
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

	GLint viewport[4];
	double modelview[16];
	double projection[16];
	double winX, winY, winZ;
	double posX, posY, posZ;

	glGetDoublev( GL_MODELVIEW_MATRIX, modelview );
	glGetDoublev( GL_PROJECTION_MATRIX, projection );
	glGetIntegerv( GL_VIEWPORT, viewport );

	winX = (double)rc.Col;
	winY = (double)viewport[3] - (double)rc.Row;
	glReadPixels( winX, int(winY), 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &winZ );

	gluUnProject( winX, winY, winZ, modelview, projection, viewport, &posX, &posY, &posZ);

	return Coord(posX, posY, 0); 

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
	c1.z = c2.z = 0;
	CoordBounds cb(c1,c2);
	setCoordBoundsZoom(cb);
}

CoordBounds DrawerContext::getCoordBoundsZoom() const  {
	return cbZoom;
}

RootDrawer *DrawerContext::getRootDrawer() const {
	return rootDrawer;
}

void DrawerContext::setProjection(const CoordBounds& cb) {
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	if ( threeD) {
		double zBase = max(abs(eyePoint.x - viewPoint.x), abs(eyePoint.y - eyePoint.y));
		gluPerspective(40, aspectRatio,zBase/2.0, 4.0 * zBase);
	} else {
		glOrtho(cb.cMin.x,cb.cMax.x,cb.cMin.y,cb.cMax.y,-1,1);
	}
}

void DrawerContext::set3D(bool yesno) {
	if ( yesno != threeD) {
		threeD = yesno;
		setEyePoint();
		setProjection(cbZoom);

	}
	if ( threeD) {
		glEnable(GL_DEPTH_TEST);
	}
	else {
		glDisable(GL_DEPTH_TEST);
	}
}
bool DrawerContext::is3D() const {
	return threeD;
}

void DrawerContext::setViewPoint(const Coord& c){
	viewPoint = c;
}
void DrawerContext::setEyePoint(const Coord& c){
	eyePoint = c;
	setProjection(cbZoom);
}
Coord DrawerContext::getViewPoint() const{
	return viewPoint;
}
Coord DrawerContext::getEyePoint() const{
	return eyePoint;
}

double DrawerContext::getFakeZ() const {
	return fakeZ;
}



