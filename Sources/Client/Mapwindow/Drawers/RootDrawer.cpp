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
#include "Client\Mapwindow\Drawers\CanvasBackgroundDrawer.h"
#include "RootDrawer.h"

using namespace ILWIS;

RootDrawer::RootDrawer() {
    drawcontext = new ILWIS::DrawerContext(this);
	ILWIS::DrawerParameters dp(drawcontext, this);
	ILWIS::PreparationParameters pp(RootDrawer::ptALL,0);
	addPreDrawer(1,IlwWinApp()->getDrawer("CanvasBackgroundDrawer", &pp, &dp));
	addPostDrawer(900,IlwWinApp()->getDrawer("MouseClickInfoDrawer", &pp, &dp));
	addPostDrawer(800,IlwWinApp()->getDrawer("GridDrawer", &pp, &dp));
}

RootDrawer::~RootDrawer() {
	delete drawcontext;
}

void  RootDrawer::prepare(PreparationParameters *pp){
	bool v1 = pp->type & RootDrawer::ptINITOPENGL;
	bool v2 = pp->type & RootDrawer::ptALL;
	if ( pp->dc && (  v1 || v2 )) {
		if ( getDrawerContext()->initOpenGL(pp->dc)) {
			DrawerParameters dp(getDrawerContext(), this);
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
				  0.0, 0.0, 1.0 );
	}

	return ComplexDrawer::draw(norecursion, cb);

}

void RootDrawer::addDataSource(void *) {
}

HTREEITEM RootDrawer::configure(LayerTreeView  *tv, HTREEITEM parent) {
	CTreeCtrl& tc = tv->GetTreeCtrl();
	int iImg = IlwWinApp()->iImage("3D");
	HTREEITEM hti = tc.InsertItem("3D",iImg,iImg,TVI_ROOT,TVI_FIRST);
	tc.SetItemData(hti, (DWORD_PTR)new DisplayOptionTreeItem(tv,hti,this,
		(SetCheckFunc)&RootDrawer::SetthreeD));	
	tc.SetCheck(hti, getDrawerContext()->is3D());
	return parent;
}

void RootDrawer::SetthreeD(void *v, LayerTreeView *tv) {
	bool value = *(bool *)(v);
	getDrawerContext()->set3D(value);
	MapCompositionDoc* doc = tv->GetDocument();
	set3D(value,tv);
	doc->mpvGetView()->Invalidate();
}










