#include "Client\Headers\formelementspch.h"
#include "Client\Ilwis.h"
#include "Engine\Map\basemap.h"
#include "Client\Mapwindow\MapPaneView.h"
#include "ComplexDrawer.h"
#include "Engine\Base\System\RegistrySettings.h"
#include "Client\Mapwindow\MapCompositionDoc.h"
#include "Client\Mapwindow\LayerTreeView.h"
#include "Client\Mapwindow\LayerTreeItem.h"
#include "Client\Mapwindow\Drawers\RootDrawer.h"
#include "Client\Mapwindow\Drawers\AbstractMapDrawer.h"
#include "Client\Mapwindow\Drawers\DrawerContext.h"
#include "RootDrawer.h"

using namespace ILWIS;

RootDrawer::RootDrawer(MapCompositionDoc *doc) {
	drawercontext = new ILWIS::DrawerContext(doc);
	ILWIS::DrawerParameters dp(this, this);
	ILWIS::PreparationParameters pp(RootDrawer::ptALL,0);
	addPreDrawer(1,IlwWinApp()->getDrawer("CanvasBackgroundDrawer", &pp, &dp));
	addPostDrawer(900,IlwWinApp()->getDrawer("MouseClickInfoDrawer", &pp, &dp));
	addPostDrawer(800,IlwWinApp()->getDrawer("GridDrawer", &pp, &dp));
	setTransparency(rUNDEF);
	setName("RootDrawer");
	threeD = false;
	aspectRatio = 0;
}

RootDrawer::~RootDrawer() {
	delete drawercontext;
}

void  RootDrawer::prepare(PreparationParameters *pp){
	bool v1 = pp->type & RootDrawer::ptINITOPENGL;
	bool v2 = pp->type & RootDrawer::ptALL;
	if ( pp->dc && (  v1 || v2 )) {
		if ( getDrawerContext()->initOpenGL(pp->dc)) {
			pp->type |= NewDrawer::ptGEOMETRY;
			CWnd * wnd = pp->dc->GetWindow();
			CRect rct;
			wnd->GetClientRect(&rct);
			RowCol rc(rct.Height(), rct.Width());
			setViewPort(rc);
		}
	}
	if ( !(pp->type & RootDrawer::ptINITOPENGL)) {
		for(map<String,NewDrawer *>::iterator cur = preDrawers.begin(); cur != preDrawers.end(); ++cur) {
			(*cur).second->prepare(pp);
		}

		for(int i=0; i < drawers.size(); ++i) {
			drawers[i]->prepare(pp);
		}

		for(map<String,NewDrawer *>::iterator cur = postDrawers.begin(); cur != postDrawers.end(); ++cur) {
			(*cur).second->prepare(pp);
		}
	}
		
}

String RootDrawer::addDrawer(NewDrawer *drw) {
	AbstractMapDrawer *mapdrw = dynamic_cast<AbstractMapDrawer *>(drw);
	if ( mapdrw) {
		CoordBounds cb = mapdrw->getBaseMap()->cb();
		vector<NewDrawer *> allDrawers;
		getDrawers(allDrawers);
		for(int i = 0; i < allDrawers.size(); ++i) {
			AbstractMapDrawer *drw = dynamic_cast<AbstractMapDrawer *>(allDrawers.at(i));
			if ( drw) {
				cb += drw->getBaseMap()->cb();
			}
		}
		setCoordBoundsMap(cb);
	}
	return ComplexDrawer::addDrawer(drw);
}

void RootDrawer::addCoordBounds(const CoordBounds& cb, bool overrule){
		setCoordBoundsView(cb, overrule);
}

/*
	Note: calls to RootDrawer::draw are meaningless without an OpenGL context in the current thread.
	Therefore all calls to RootDrawer::draw must be preceded by a call to DrawerContext::TakeContext and followed by a call to ReleaseContext
*/
bool RootDrawer::draw(bool norecursion, const CoordBounds& cb) const{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);		// Clear The Screen And The Depth Buffer
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	if (is3D()) {
		Coord cView = getViewPoint();
		Coord cEye = getEyePoint();
		gluLookAt(cEye.x, cEye.y, cEye.z,
			      cView.x, cView.y, cView.z, 
				  0, 0, 1.0 );
	}

	return ComplexDrawer::draw(norecursion, cb);

}

void RootDrawer::addDataSource(void *) {
}

void RootDrawer::timedEvent(UINT timerID) {
	for(DrawerMap::iterator cur = drawersById.begin(); cur != drawersById.end(); ++cur) {
		(*cur).second->timedEvent(timerID);
	}
}

String RootDrawer::store(const FileName& fnView, const String parenSection) const{
	ObjectInfo::WriteElement("RootDrawer","CoordinateSystem",fnView, getCoordinateSystem());
	ObjectInfo::WriteElement("RootDrawer","CoordBoundsZoom",fnView, getCoordBoundsZoom());
	ObjectInfo::WriteElement("RootDrawer","CoordBoundsView",fnView, getCoordBoundsView());
	ObjectInfo::WriteElement("RootDrawer","CoordBoundsMap",fnView, getMapCoordBounds());
	//ObjectInfo::WriteElement("RootDrawer","AspectRatio",fnView, getAspectRatio());
	ObjectInfo::WriteElement("RootDrawer","EyePoint",fnView, getEyePoint());
	ObjectInfo::WriteElement("RootDrawer","ViewPoint",fnView, getViewPoint());
	ObjectInfo::WriteElement("RootDrawer","ViewPort",fnView, getViewPort());

	ComplexDrawer::store(fnView, "RootDrawer");

	return "RootDrawer";
}

void RootDrawer::load(const FileName& fnView, const String parenSection){
	CoordSystem csy;
	ObjectInfo::ReadElement("RootDrawer","CoordinateSystem",fnView, csy);
	CoordBounds cbZoom,cbView,cbMap;
	ObjectInfo::ReadElement("RootDrawer","CoordBoundsZoom",fnView, cbZoom);
	ObjectInfo::ReadElement("RootDrawer","CoordBoundsView",fnView, cbView);
	ObjectInfo::ReadElement("RootDrawer","CoordBoundsMap",fnView, cbMap);
	/*double aspect;
	ObjectInfo::ReadElement("RootDrawer","AspectRatio",fnView, aspect);*/
	Coord eyePoint,viewPoint;
	ObjectInfo::ReadElement("RootDrawer","EyePoint",fnView, eyePoint);
	ObjectInfo::ReadElement("RootDrawer","ViewPoint",fnView, viewPoint);
	RowCol viewPort;
	ObjectInfo::ReadElement("RootDrawer","ViewPort",fnView, viewPort);
	setCoordinateSystem(csy, false);
	setViewPort(viewPort);
	setCoordBoundsMap(cbMap);
	setCoordBoundsView(cbView, false);
	setCoordBoundsZoom(cbZoom);
	setEyePoint(eyePoint);
	setViewPoint(viewPoint);

	ComplexDrawer::load(fnView,"RootDrawer");
	set3D(threeD);


}

void RootDrawer::modifyCBZoomView(double dv, double dz, double f) {
	double deltaxv = dv * f;
	double deltaxz = dz * f;
	Coord cMiddle = cbZoom.middle();
	cbView.cMin.x = cMiddle.x - deltaxv / 2.0;
	cbView.cMax.x = cMiddle.x + deltaxv / 2.0;
	cbZoom.cMin.x = cMiddle.x - deltaxz / 2.0;
	cbZoom.cMax.x = cMiddle.x + deltaxz / 2.0;
}

void RootDrawer::setViewPort(const RowCol& rc) {
	if (  aspectRatio  != 0.0 && pixArea.Col != iUNDEF) {
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
	drawercontext->TakeContext(true);
	glViewport(0,0,rc.Col, rc.Row);
	setProjection(cbZoom);
	drawercontext->ReleaseContext();
}

void RootDrawer::setCoordinateSystem(const CoordSystem& _cs, bool overrule){
	if (overrule || cs->fUnknown()) {
		cs = _cs;
	}
}

void RootDrawer::setCoordBoundsView(const CoordBounds& _cb, bool overrule){
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
		drawercontext->TakeContext(true);
		setProjection(cbView);
		drawercontext->ReleaseContext();
	} else {
		cbView += _cb;
		aspectRatio = cbView.width()/ cbView.height();
	}
	fakeZ = cbView.width() * 0.001;
	
}

void RootDrawer::setCoordBoundsZoom(const CoordBounds& cb) {
	cbZoom = cb;
	setViewPoint(cbZoom.middle());
	setEyePoint();
	drawercontext->TakeContext(true);
	setProjection(cb);
	drawercontext->ReleaseContext();
}

void RootDrawer::setEyePoint() {
	eyePoint.x = viewPoint.x - cbZoom.width() ;
	eyePoint.y = viewPoint.y - cbZoom.height();
	eyePoint.z = cbZoom.width() * 2;
}

void RootDrawer::setCoordBoundsMap(const CoordBounds& cb) {
	cbMap = cb;
}
CoordBounds RootDrawer::getMapCoordBounds() const{
	return cbMap;
}

double RootDrawer::getAspectRatio() const {
	return aspectRatio;
}

Coord RootDrawer::screenToWorld(const RowCol& rc) {

	GLint viewport[4];
	double modelview[16];
	double projection[16];
	double winX, winY;
	double posX, posY, posZ;
	float winZ;

	drawercontext->TakeContext(true);

	glGetDoublev( GL_MODELVIEW_MATRIX, modelview );
	glGetDoublev( GL_PROJECTION_MATRIX, projection );
	glGetIntegerv( GL_VIEWPORT, viewport );

	winX = (double)rc.Col;
	winY = (double)viewport[3] - (double)rc.Row;
	glReadPixels( winX, int(winY), 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &winZ );

	gluUnProject( winX, winY, winZ, modelview, projection, viewport, &posX, &posY, &posZ);

	drawercontext->ReleaseContext();

	double z = 0;
	if ( is3D()) {
		z = abs(posZ) < fakeZ ? fakeZ : posZ;
	}
	return Coord(posX, posY, z ); 

}

RowCol RootDrawer::worldToScreen(const Coord& crd){
	int vp[4];
	drawercontext->TakeContext(true);
	glGetIntegerv(GL_VIEWPORT, vp);
	drawercontext->ReleaseContext();
	int x = vp[0] + crd.x * vp[2];
	int y = vp[1] + crd.y * vp[3];
	return RowCol(y,x);
}

void RootDrawer::setZoom(const CRect& rect) {
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

CoordBounds RootDrawer::getCoordBoundsZoom() const  {
	return cbZoom;
}

/*
	Note: calls to RootDrawer::setProjection are meaningless without an OpenGL context in the current thread.
	Therefore all calls to RootDrawer::setProjection must be preceded by a call to DrawerContext::TakeContext and followed by a call to ReleaseContext
*/
void RootDrawer::setProjection(const CoordBounds& cb) {
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	if ( threeD) {
		double zBase = max(abs(eyePoint.x - viewPoint.x), abs(eyePoint.y - viewPoint.y));
		double w = max(cbZoom.width(), cbZoom.height());
		gluPerspective(40, aspectRatio,zBase/2.0, w * 4);
	} else {
		glOrtho(cb.cMin.x,cb.cMax.x,cb.cMin.y,cb.cMax.y,-1,1);
	}
	
}

void RootDrawer::set3D(bool yesno) {
	drawercontext->TakeContext(true);

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
	drawercontext->ReleaseContext();
}
bool RootDrawer::is3D() const {
	return threeD;
}

void RootDrawer::setViewPoint(const Coord& c){
	viewPoint = c;
}
void RootDrawer::setEyePoint(const Coord& c){
	eyePoint = c;
	drawercontext->TakeContext(true);
	setProjection(cbZoom);
	drawercontext->ReleaseContext();
}
Coord RootDrawer::getViewPoint() const{
	return viewPoint;
}
Coord RootDrawer::getEyePoint() const{
	return eyePoint;
}

double RootDrawer::getFakeZ() const {
	return fakeZ;
}

void RootDrawer::debug() {
	GLdouble m_projMatrix[16];
	GLdouble m_modelMatrix[16];
	GLint m_viewport[4]; // x,y,width,height
	memset(m_projMatrix, 0, 16 * 8);
	memset(m_modelMatrix, 0, 16 * 8);
	memset(m_viewport, 0, 4 * 4);
	// viewport

	// get the matrices and the viewport
	glGetDoublev(GL_MODELVIEW_MATRIX, m_modelMatrix);
	glGetDoublev(GL_PROJECTION_MATRIX, m_projMatrix);
	glGetIntegerv(GL_VIEWPORT, m_viewport);
}

//----------------------------------UI-------------------------------------
HTREEITEM RootDrawer::configure(LayerTreeView  *tv, HTREEITEM parent) {
	DisplayOptionTreeItem *item = new DisplayOptionTreeItem(tv,parent,this,
		(SetCheckFunc)&RootDrawer::SetthreeD);	
	return InsertItem("3D","3D",item,is3D(),TVI_FIRST);
}

void RootDrawer::SetthreeD(void *v, LayerTreeView *tv) {
	bool value = *(bool *)(v);

	set3D(value);
	MapCompositionDoc* doc = tv->GetDocument();
  

	make3D(value,tv);
	doc->mpvGetView()->Invalidate();
}










