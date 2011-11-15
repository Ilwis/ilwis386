#include "Headers\toolspch.h"
#include "Engine\Map\basemap.h"
#include "ComplexDrawer.h"
#include "Engine\Drawers\DrawerContext.h"
#include "Engine\Drawers\RootDrawer.h"
#include "Engine\Drawers\SpatialDataDrawer.h"
#include "Engine\Drawers\SelectionRectangle.h"
#include "Engine\Drawers\ZValueMaker.h"


using namespace ILWIS;

RootDrawer::RootDrawer() : ComplexDrawer(0,"RootDrawer"){
	drawercontext = new ILWIS::DrawerContext();
	ILWIS::DrawerParameters dp(this, this);
	ILWIS::PreparationParameters pp(RootDrawer::ptALL,0);
	ComplexDrawer *annotationDrawers = (ComplexDrawer *)NewDrawer::getDrawer("AnnotationDrawers", &pp, &dp);
	if ( annotationDrawers) {
		addPostDrawer(800, annotationDrawers);
		annotationDrawers->addPostDrawer(200,NewDrawer::getDrawer("GridDrawer", &pp, &dp));
		annotationDrawers->addPostDrawer(300,NewDrawer::getDrawer("GraticuleDrawer", &pp, &dp));
	}
	backgroundDrawer = NewDrawer::getDrawer("CanvasBackgroundDrawer", &pp, &dp);

	setTransparency(1.0);
	setName("RootDrawer");
	threeD = false;
	aspectRatio = 0;
	selectionDrawer = 0;
	rotX = rotY = rotZ = 0;
	zoom3D = 1.0;
	rootDrawer = this;
	initRestore = false;

}

RootDrawer::~RootDrawer() {
	clear();
	delete backgroundDrawer;
	delete drawercontext; // delete drawers before drawercontext, some drawers still work on the GL context and clean it up
}

void  RootDrawer::prepare(PreparationParameters *pp){
	bool v1 = pp->type & RootDrawer::ptINITOPENGL;
	bool v2 = pp->type & RootDrawer::ptALL;
	if ( pp->dc && (  v1 || v2 )) {
		if ( ((pp->contextMode & DrawerContext::mFORCEINIT)!=0) && getDrawerContext())
			getDrawerContext()->ReleaseContext();
		if ( getDrawerContext()->initOpenGL(pp->dc->m_hDC, pp->dc->GetWindow(), pp->contextMode)) {
			pp->type |= NewDrawer::ptGEOMETRY;
			CWnd * wnd = pp->dc->GetWindow();
			if ( wnd) {
				CRect rct;
				wnd->GetClientRect(&rct);
				RowCol rc(rct.Height(), rct.Width());
				setViewPort(rc);
			}
		}
	}
	if ( !(pp->type & RootDrawer::ptINITOPENGL)) {
		backgroundDrawer->prepare(pp);

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
	SpatialDataDrawer *mapdrw = dynamic_cast<SpatialDataDrawer *>(drw);
	if ( mapdrw && mapdrw->getBaseMap() != 0) {
		CoordBounds ncb = mapdrw->cb();
		//addCoordBounds(mapdrw->getBaseMap()->cs(), cb);
		//cb += ncb;
		CoordBounds cb = cbView;
		cb += ncb;
		if ( ! (cb == cbView))
			setCoordBoundsView(cb,true);

	}
	return ComplexDrawer::addDrawer(drw);
}

void RootDrawer::addCoordBounds(const CoordSystem& _cs, const CoordBounds& cb, bool overrule){
	if ( overrule || cbMap.fUndef()) {
		CoordBounds ncb = cs.fEqual(_cs) ? cb : cs->cbConv(_cs,cb);
		//ncb += cbMap;
		cbMap += ncb;
	}
	setCoordBoundsView(cbMap, overrule);
}

/*
Note: calls to RootDrawer::draw are meaningless without an OpenGL context in the current thread.
Therefore all calls to RootDrawer::draw must be preceded by a call to DrawerContext::TakeContext and followed by a call to ReleaseContext
*/

void RootDrawer::setupDraw() const{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);		// Clear The Screen And The Depth Buffer
	if (threeD)
		glEnable(GL_DEPTH_TEST);
	else
		glDisable(GL_DEPTH_TEST);

	glViewport(0,0,pixArea.Col, pixArea.Row);
	setProjection(cbZoom);

	glMatrixMode(GL_MODELVIEW);
}

bool RootDrawer::draw( const CoordBounds& cb) const{
	if ( selectionDrawer) {
		selectionDrawer->draw();
	}
	else {
		const_cast<RootDrawer *>(this)->setZIndex(0);
		setupDraw();
		glLoadIdentity();
		if (is3D()) {
			gluLookAt(eyePoint.x, eyePoint.y, eyePoint.z, viewPoint.x, viewPoint.y, viewPoint.z, 0, 1.0, 0 );
			//glPushMatrix();	
			glTranslatef(viewPoint.x,viewPoint.y, 0);
			glRotatef(rotY,-1,0,0);				// Rotate on y
			glRotatef(rotX,0,0,-1);				// Rotate on x
			glTranslatef(-viewPoint.x,-viewPoint.y, 0);
		}
		// due to the way how transparency works in opengl the backgroundrawer has to be drawn at different moments depending on the view93d or not) 
		if (! is3D())
			backgroundDrawer->draw(cb);
		ComplexDrawer::draw( cb);
		if (is3D())
			backgroundDrawer->draw(cb);

		//if ( is3D())
		//	glPopMatrix();
	}
	return true;

}

int RootDrawer::getZIndex() const {
	return getZMaker()->getZOrder();
}

void RootDrawer::setZIndex(int n) {
	getZMaker()->setZOrder(n, fakeZ);
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
	ObjectInfo::WriteElement("RootDrawer","XRotation",fnView, rotX);
	ObjectInfo::WriteElement("RootDrawer","YRotation",fnView, rotY);
	ObjectInfo::WriteElement("RootDrawer","ZRotation",fnView, rotZ);
	ObjectInfo::WriteElement("RootDrawer","Zoom3D",fnView, zoom3D);
	ObjectInfo::WriteElement("RootDrawer","Is3D",fnView, threeD);

	ComplexDrawer::store(fnView, "RootDrawer");

	return "RootDrawer";
}

void RootDrawer::load(const FileName& fnView, const String parenSection){
	CoordSystem csy;
	ObjectInfo::ReadElement("RootDrawer","CoordinateSystem",fnView, csy);
	CoordBounds cbZ,cbV,cbM;
	ObjectInfo::ReadElement("RootDrawer","CoordBoundsZoom",fnView, cbZ);
	ObjectInfo::ReadElement("RootDrawer","CoordBoundsView",fnView, cbV);
	ObjectInfo::ReadElement("RootDrawer","CoordBoundsMap",fnView, cbM);
	/*double aspect;
	ObjectInfo::ReadElement("RootDrawer","AspectRatio",fnView, aspect);*/
	Coord eyePoint,viewPoint;
	ObjectInfo::ReadElement("RootDrawer","EyePoint",fnView, eyePoint);
	ObjectInfo::ReadElement("RootDrawer","ViewPoint",fnView, viewPoint);
	RowCol viewPort;
	ObjectInfo::ReadElement("RootDrawer","ViewPort",fnView, viewPort);
	ObjectInfo::ReadElement("RootDrawer","Is3D",fnView, threeD);
	setCoordinateSystem(csy, true);
	setViewPort(viewPort);
	setCoordBoundsMap(cbM);
	setCoordBoundsView(cbV, true);
	setCoordBoundsZoom(cbZ);

	ComplexDrawer::load(fnView,"RootDrawer");

	ObjectInfo::ReadElement("RootDrawer","XRotation",fnView, rotX); 
	ObjectInfo::ReadElement("RootDrawer","YRotation",fnView, rotY);
	ObjectInfo::ReadElement("RootDrawer","ZRotation",fnView, rotZ);
	ObjectInfo::ReadElement("RootDrawer","Zoom3D",fnView, zoom3D);

	setEyePoint(eyePoint);
	setViewPoint(viewPoint);
	set3D(threeD);
	initRestore = true;
}

void RootDrawer::modifyCBZoomView(double dv, double dz, double f) {
	double deltaview = dv * f;
	double deltazoom = dz * f;
	Coord cMiddle = cbZoom.middle();
	if ( aspectRatio <= 1.0) {
		cbView.cMin.x = cMiddle.x - deltaview / 2.0;
		cbView.cMax.x = cMiddle.x + deltaview / 2.0;
		cbZoom.cMin.x = cMiddle.x - deltazoom / 2.0;
		cbZoom.cMax.x = cMiddle.x + deltazoom / 2.0;
	} else {
		cbView.cMin.y = cMiddle.y - deltaview / 2.0;
		cbView.cMax.y = cMiddle.y + deltaview / 2.0;
		cbZoom.cMin.y = cMiddle.y - deltazoom / 2.0;
		cbZoom.cMax.y = cMiddle.y + deltazoom / 2.0;
	}
}

void RootDrawer::setViewPort(const RowCol& rc) {
	if (  aspectRatio  != 0.0 && pixArea.Col != iUNDEF) {
		// this code adapts the cbZoom if the window size changes
		if ( aspectRatio <= 1.0) { // y > x
			if ( rc.Col != pixArea.Col){ // make sure the zoomsize is changed if the cols changes
				modifyCBZoomView(cbView.width(), cbZoom.width(),(double)rc.Col / pixArea.Col); 
			}
			if ( rc.Row != pixArea.Row) { // make sure the zoomsize is changed if the cols change
				//modifyCBZoomView(cbView.width(), cbZoom.width(),(double)pixArea.Col / rc.Col); 
				modifyCBZoomView(cbView.width(), cbZoom.width(), (double)pixArea.Row / (double)rc.Row ); 
			}

		} else { // x < y
			if ( rc.Row != pixArea.Row){
				modifyCBZoomView(cbView.height(), cbZoom.height(),(double)rc.Row / pixArea.Row ); 

			}
			if ( rc.Col != pixArea.Col) {
				modifyCBZoomView(cbView.height(), cbZoom.height(),(double)pixArea.Col / rc.Col ); 
			}
		}
	}
	pixArea = rc;
}

void RootDrawer::setCoordinateSystem(const CoordSystem& _cs, bool overrule){
	if (overrule || cs->fUnknown()) {
		if ( overrule) {
			cbMap = _cs->cbConv(cs, cbMap);
			cbZoom = _cs->cbConv(cs, cbZoom);
			cbView = _cs->cbConv(cs, cbView);
			setProjection(cbMap);
		}
		cs = _cs;
	}
}

void RootDrawer::setCoordBoundsView(/*const CoordSystem& _cs,*/ const CoordBounds& cb, bool overrule){
	//CoordBounds cb = cs.fEqual(_cs) ? _cb : cs->cbConv(_cs,_cb);
	if ( overrule || cbView.fUndef()) {
		//cbMap = cb;
		aspectRatio = cbMap.width()/ cbMap.height();
		double w = cb.width();
		double h = cb.height();
		double delta = 0;
		if ( aspectRatio <= 1.0) {
			double pixwidth = (double)pixArea.Row * aspectRatio;
			double deltay = 0;
			if ( pixwidth > pixArea.Col) {
				deltay = cb.height() * ( pixwidth / pixArea.Col - 1.0);
				pixwidth = pixArea.Col;
			}
			double fracofWidth = 1.0 - (pixArea.Col - pixwidth) / pixArea.Col;
			double crdWidth = w / fracofWidth;
			double delta = (crdWidth - w) / 2.0;
			cbView =  CoordBounds(Coord(cb.MinX() - delta,cb.MinY() - deltay /2.0,0), 
				Coord(cb.MaxX() + delta,cb.MaxY() + deltay/ 2.0,0));
		} else {
			double pixheight = (double)pixArea.Col / aspectRatio;
			double deltax = 0;
			if ( pixheight > pixArea.Row) {
				deltax = cb.width() * ( pixheight / pixArea.Row - 1.0);
				pixheight = pixArea.Row;
			}
			double fracofHeight = 1.0 - abs(pixArea.Row - pixheight) / (double)pixArea.Row;
			double crdHeight = h / fracofHeight;
			double delta = (crdHeight - h) / 2.0;
			cbView =  CoordBounds(Coord(cb.MinX() - deltax /2.0,cb.MinY()  - delta,0), 
				Coord(cb.MaxX() + deltax /2.0,cb.MaxY()  + delta,0));

		}
		cbZoom = cbView;
		setViewPoint(cbView.middle());
		setEyePoint();
	} 
	fakeZ = cbView.width() * 0.0005;
	glMatrixMode(GL_MODELVIEW);
	if ( is3D()) {
		glLoadIdentity();
		if (is3D()) {
			if ( !initRestore) { // restore set rotX, etc. But the OnEntireMap would destroy these false; so for once  the init of values is skipped
				rotX= rotY = 0;
				zoom3D = 1.0;
			} else 
				initRestore = false;
			setCoordBoundsZoom(cbView);
			gluLookAt(eyePoint.x, eyePoint.y, eyePoint.z, viewPoint.x, viewPoint.y, viewPoint.z, 0, 1.0, 0 );
		}
	}

}

void RootDrawer::setCoordBoundsZoom(const CoordBounds& cbIn) {
	CoordBounds cb = cbIn;
	if ( cbZoom.fValid()) {
	// zooming never changes the shape of the mapwindow so any incomming zoom rectangle must conform to the shape of the existing mapwindow
		double factCur = cbZoom.width() / cbZoom.height();
		double factIn = cbIn.width() / cbIn.height();
		if ( abs(factCur - factIn) > 0.01 ) {
			if ( factCur < 1.0) {
				double newHeight = cbIn.width() / factCur;
				cb = CoordBounds(cbIn.cMin, Coord(cbIn.cMax.x, cbIn.cMin.y + newHeight));
			} else {
				double newWidth = cb.height() * factCur;
				cb = CoordBounds(cbIn.cMin, Coord(cbIn.cMin.x + newWidth, cbIn.cMax.y));
			}
		}
	}


	cbZoom = cb;
	setViewPoint(cbZoom.middle());
	setEyePoint();
}

void RootDrawer::setEyePoint() {
	eyePoint.x = viewPoint.x;;// - cbZoom.width() ;
	eyePoint.y = viewPoint.y - cbZoom.height() * 1.5;
	eyePoint.z = cbZoom.width() ;
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

	drawercontext->TakeContext();

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
	drawercontext->TakeContext();

	GLint viewport[4];
	double modelview[16];
	double projection[16];
	double posX, posY, posZ;

	glGetDoublev( GL_MODELVIEW_MATRIX, modelview );
	glGetDoublev( GL_PROJECTION_MATRIX, projection );
	glGetIntegerv( GL_VIEWPORT, viewport );

	int pppp = gluProject(crd.x, crd.y, 1,modelview, projection, viewport,&posX,&posY, &posZ);

	drawercontext->ReleaseContext();
	return RowCol(posY, posX);

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
void RootDrawer::setProjection(const CoordBounds& cb) const {
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	if ( threeD) {
		double zBase = max(abs(eyePoint.x - viewPoint.x), abs(eyePoint.y - viewPoint.y));
		double w = max(cbZoom.width(), cbZoom.height());
		gluPerspective(40*zoom3D, aspectRatio,zBase/2.0, w * 4);
	} else {
		glOrtho(cb.cMin.x,cb.cMax.x,cb.cMin.y,cb.cMax.y,-1,1);
	}

}

void RootDrawer::set3D(bool yesno) {
	if ( yesno != threeD) {
		threeD = yesno;
		setEyePoint();
	//	initLight();
	}
}
bool RootDrawer::is3D() const {
	return threeD;
}

void RootDrawer::setViewPoint(const Coord& c){
	viewPoint = c;
}
void RootDrawer::setEyePoint(const Coord& c){
	eyePoint = c;
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


void RootDrawer::setSelectionDrawer(SelectionRectangle *selDraw) {
	// selection drawer is owned by the areaselector, dont delete it here
	selectionDrawer = selDraw;
}


void RootDrawer::setRotationAngles(double rx, double ry, double rz){
	rotX = rx;
	rotY = ry;
	rotZ = rz;
}
void RootDrawer::getRotationAngles(double& rx, double& ry, double& rz){
	rx = rotX;
	ry = rotY;
	rz = rotZ;
}

double RootDrawer::getZoom3D() const{
	return zoom3D;
}

void RootDrawer::setZoom3D(double v){
	zoom3D = v;
}

void RootDrawer::initLight() {
	glEnable(GL_NORMALIZE);

	// Lights, material properties
	GLfloat	specularProperties[] = {0.1f, 0.1f, 0.1f, 1.0f};
	if ( is3D())
		glEnable(GL_DEPTH_TEST);
	else
		glDisable(GL_DEPTH_TEST);

	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specularProperties);

	glClearColor(1.0,1.0,1.0,0);
	glClearDepth(1.0);

	//GLfloat light0Position[] = {0.0f, 0.0f, 0.0f, 1.0f};
	GLfloat light0Position[] = {eyePoint.x, eyePoint.y, eyePoint.z, 1.0f};
	glLightfv(GL_LIGHT0, GL_POSITION, light0Position);
	glEnable(GL_COLOR_MATERIAL);

	glEnable(GL_LIGHT0);
	SetAmbientColor();
	SetDiffuseColor();
	SetSpecularColor();
	glLightModelf(GL_LIGHT_MODEL_TWO_SIDE, 1.0);

	glEnable(GL_POINT_SMOOTH);
	glEnable(GL_LINE_SMOOTH);
	glEnable(GL_POLYGON_SMOOTH);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	glHint(GL_LINE_SMOOTH_HINT,GL_NICEST);
	glLineWidth(1.0);
	glPointSize(3.0);

	glShadeModel(GL_SMOOTH);

	if (is3D())
		glEnable(GL_LIGHTING);
	else
		glDisable(GL_LIGHTING);

	// Default polygonmode
	glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
	//switch (pApp->m_nPolygonMode)
	//{
	//	case 0:
	//		glPolygonMode(GL_FRONT_AND_BACK,GL_POINT);
	//		break;
	//	case 1:
	//		glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
	//		break;
	//	case 2:
	//		glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
	//		break;
	//	default:
	//		{}
	//}
}

#define TESTCOL 1.0
void RootDrawer::SetAmbientColor()
{
	GLfloat	ambientLight[]  = {TESTCOL, TESTCOL, TESTCOL, 1.0f};
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);
}

void RootDrawer::SetDiffuseColor()
{
	GLfloat	diffuseLight[]  = {TESTCOL, TESTCOL, TESTCOL, 1.0f};
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight);
}

void RootDrawer::SetSpecularColor()
{
	GLfloat	specularLight[] = {1, 1, 1, 1.0f};
	glLightfv(GL_LIGHT0, GL_SPECULAR, specularLight);
}








