#include "Client\Headers\formelementspch.h"
#include "Client\FormElements\fldcolor.h"
#include "Engine\Map\basemap.h"
#include "Engine\Base\System\RegistrySettings.h"
#include "Client\Mapwindow\MapCompositionDoc.h"
#include "Client\Mapwindow\Drawers\DrawerContext.h"
#include "Client\MapWindow\Drawers\ComplexDrawer.h"
#include "Client\Mapwindow\Drawers\AbstractObjectdrawer.h"
#include "Client\Mapwindow\Drawers\AbstractMapDrawer.h"
#include "Client\Mapwindow\LayerTreeView.h"
#include "Client\Mapwindow\LayerTreeItem.h"
#include "Client\Mapwindow\Drawers\CanvasBackgroundDrawer.h"

using namespace ILWIS;

ILWIS::NewDrawer *createCanvasBackgroundDrawer(DrawerParameters *parms) {
	return new CanvasBackgroundDrawer(parms);
}

CanvasBackgroundDrawer::CanvasBackgroundDrawer(DrawerParameters *parms) : ComplexDrawer(parms,"CanvasBackgroundDrawer"){
	id = name = "CanvasBackgroundDrawer";
	outside = Color(179,179,179);
	inside = Color(255,255,255);
}

CanvasBackgroundDrawer::~CanvasBackgroundDrawer() {
}
void  CanvasBackgroundDrawer::prepare(PreparationParameters *pp){
	ComplexDrawer::prepare(pp);

}

HTREEITEM CanvasBackgroundDrawer::configure(LayerTreeView  *tv, HTREEITEM parent) {
	ComplexDrawer::configure(tv,parent);
	DisplayOptionTreeItem *item = new DisplayOptionTreeItem(tv,parent,this, 
					(DisplayOptionItemFunc)&CanvasBackgroundDrawer::displayOptionOutsideColor);
	InsertItem("Outside map","SingleColor",item, -1);

	item = new DisplayOptionTreeItem(tv,parent,this,
					(DisplayOptionItemFunc)&CanvasBackgroundDrawer::displayOptionInsideColor);
	InsertItem("Inside map","SingleColor",item, -1);

	return parent;
}

bool CanvasBackgroundDrawer::draw(bool norecursion, const CoordBounds& cb) const{
	CoordBounds cbView = getDrawerContext()->getCoordBoundsView();
	glColor3d(outside.redP(), outside.greenP(), outside.blueP());
	double z =0.0;
	glBegin(GL_QUADS);						
		glVertex3f(cbView.MinX(), cbView.MinY(),z);				
		glVertex3f(cbView.MinX(), cbView.MaxY(),z);				
		glVertex3f(cbView.MaxX(), cbView.MaxY(),z);				
		glVertex3f(cbView.MaxX(), cbView.MinY(),z);
	glEnd();

	CoordBounds cbMap = getDrawerContext()->getMapCoordBounds();

	glColor3f(inside.redP(), inside.greenP(), inside.blueP());
	glBegin(GL_QUADS);						
		glVertex3f(cbMap.MinX(), cbMap.MinY(),z);				
		glVertex3f(cbMap.MinX(), cbMap.MaxY(),z);				
		glVertex3f(cbMap.MaxX(), cbMap.MaxY(),z);				
		glVertex3f(cbMap.MaxX(), cbMap.MinY(),z);
	glEnd();

	return true;
}

void CanvasBackgroundDrawer::displayOptionOutsideColor(CWnd *parent) {
	new SetColorForm("Outside map", parent, this, &outside);
}

void CanvasBackgroundDrawer::displayOptionInsideColor(CWnd *parent) {
	new SetColorForm("Inside map", parent, this, &inside);
}

//------------------------------------------------
SetColorForm::SetColorForm(const String& title, CWnd *wPar, CanvasBackgroundDrawer *dr, Color* color) : 
	DisplayOptionsForm(dr, wPar,title)
{
	fc = new FieldColor(root, "Draw color", color);
	create();
}

void  SetColorForm::apply() {
	fc->StoreData();
	updateMapView();
}

