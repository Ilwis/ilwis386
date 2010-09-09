#include "Client\Headers\formelementspch.h"
#include "Client\Ilwis.h"
#include "Engine\Map\basemap.h"
#include "Client\Mapwindow\MapPaneView.h"
#include "Client\Mapwindow\Drawers\DrawerContext.h"
#include "ComplexDrawer.h"
#include "Engine\Base\System\RegistrySettings.h"
#include "Client\Mapwindow\MapCompositionDoc.h"
#include "Client\Mapwindow\LayerTreeView.h"
#include "Client\Mapwindow\LayerTreeItem.h"
#include "Client\Mapwindow\Drawers\RootDrawer.h"
#include "Client\Mapwindow\Drawers\AbstractObjectdrawer.h"
#include "Client\Mapwindow\Drawers\AbstractMapDrawer.h"
#include "RootDrawer.h"

using namespace ILWIS;

RootDrawer::RootDrawer(MapCompositionDoc *doc) {
    drawcontext = new ILWIS::DrawerContext(doc, this);
	ILWIS::DrawerParameters dp(drawcontext, this);
	ILWIS::PreparationParameters pp(RootDrawer::ptALL,0);
	addPreDrawer(1,IlwWinApp()->getDrawer("CanvasBackgroundDrawer", &pp, &dp));
	addPostDrawer(900,IlwWinApp()->getDrawer("MouseClickInfoDrawer", &pp, &dp));
	addPostDrawer(800,IlwWinApp()->getDrawer("GridDrawer", &pp, &dp));
	setTransparency(rUNDEF);
	setName("RootDrawer");
}

RootDrawer::~RootDrawer() {
	delete drawcontext;
}

void  RootDrawer::prepare(PreparationParameters *pp){
	bool v1 = pp->type & RootDrawer::ptINITOPENGL;
	bool v2 = pp->type & RootDrawer::ptALL;
	if ( pp->dc && (  v1 || v2 )) {
		if ( getDrawerContext()->initOpenGL(pp->dc)) {
			//DrawerParameters dp(getDrawerContext(), this);
			pp->type |= NewDrawer::ptGEOMETRY;
			ComplexDrawer::prepare(pp);
		}
	}
	if ( !(pp->type & RootDrawer::ptINITOPENGL))
		ComplexDrawer::prepare(pp);
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
		getDrawerContext()->setCoordBoundsMap(cb);
	}
	return ComplexDrawer::addDrawer(drw);
}

void RootDrawer::setCoordSystem(const CoordSystem& cs, bool overrule){
	if ( drawcontext)
		drawcontext->setCoordinateSystem(cs, overrule);
}
void RootDrawer::addCoordBounds(const CoordBounds& cb, bool overrule){
	if ( drawcontext)
		drawcontext->setCoordBoundsView(cb, overrule);
}

bool RootDrawer::draw(bool norecursion, const CoordBounds& cb) const{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);		// Clear The Screen And The Depth Buffer
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	if (getDrawerContext()->is3D()) {
		Coord cView = getDrawerContext()->getViewPoint();
		Coord cEye = getDrawerContext()->getEyePoint();
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

void RootDrawer::store(const FileName& fnView, const String parenSection, SubType subtype) const{
	ObjectInfo::WriteElement("RootDrawer","CoordinateSystem",fnView, getDrawerContext()->getCoordinateSystem());
	ObjectInfo::WriteElement("RootDrawer","CoordBoundsZoom",fnView, getDrawerContext()->getCoordBoundsZoom());
	ObjectInfo::WriteElement("RootDrawer","CoordBoundsView",fnView, getDrawerContext()->getCoordBoundsView());
	ObjectInfo::WriteElement("RootDrawer","CoordBoundsMap",fnView, getDrawerContext()->getMapCoordBounds());
	ObjectInfo::WriteElement("RootDrawer","AspectRatio",fnView, getDrawerContext()->getAspectRatio());
	ObjectInfo::WriteElement("RootDrawer","EyePoint",fnView, getDrawerContext()->getEyePoint());
	ObjectInfo::WriteElement("RootDrawer","ViewPoint",fnView, getDrawerContext()->getViewPoint());
	ObjectInfo::WriteElement("RootDrawer","ViewPort",fnView, getDrawerContext()->getViewPort());

	ComplexDrawer::store(fnView, "", subtype);
}

void RootDrawer::load(const FileName& fnView, const String parenSection){
}
//----------------------------------UI-------------------------------------
HTREEITEM RootDrawer::configure(LayerTreeView  *tv, HTREEITEM parent) {
	DisplayOptionTreeItem *item = new DisplayOptionTreeItem(tv,parent,this,
		(SetCheckFunc)&RootDrawer::SetthreeD);	
	return InsertItem("3D","3D",item,getDrawerContext()->is3D(),TVI_FIRST);
}

void RootDrawer::SetthreeD(void *v, LayerTreeView *tv) {
	bool value = *(bool *)(v);
	getDrawerContext()->set3D(value);
	MapCompositionDoc* doc = tv->GetDocument();
	set3D(value,tv);
	doc->mpvGetView()->Invalidate();
}










