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
#include "CanvasBackgroundDrawer.h"
#include "Client\Ilwis.h"

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

bool CanvasBackgroundDrawer::draw(bool norecursion, const CoordBounds& cb) const{
	CoordBounds cbView = getDrawerContext()->getCoordBoundsView();
	CoordBounds cbMap = getDrawerContext()->getMapCoordBounds();
	glColor4d(outside.redP(), outside.greenP(), outside.blueP(),getTransparency());
	bool is3D = getDrawerContext()->is3D();
	double z = is3D ? -getDrawerContext()->getFakeZ() : 0;
	glBegin(GL_QUADS);						
		glVertex3f(cbView.MinX(), cbView.MinY(),z);				
		glVertex3f(cbView.MinX(), cbView.MaxY(),z);				
		glVertex3f(cbMap.MinX(), cbView.MaxY(),z);				
		glVertex3f(cbMap.MinX(), cbView.MinY(),z);
	glEnd();

	glBegin(GL_QUADS);						
		glVertex3f(cbMap.MaxX(), cbView.MinY(),z);				
		glVertex3f(cbMap.MaxX(), cbView.MaxY(),z);				
		glVertex3f(cbView.MaxX(), cbView.MaxY(),z);				
		glVertex3f(cbView.MaxX(), cbView.MinY(),z);
	glEnd();

	glColor4f(inside.redP(), inside.greenP(), inside.blueP(),getTransparency());
	glBegin(GL_QUADS);						
		glVertex3f(cbMap.MinX(), cbMap.MinY(),z);				
		glVertex3f(cbMap.MinX(), cbMap.MaxY(),z);				
		glVertex3f(cbMap.MaxX(), cbMap.MaxY(),z);				
		glVertex3f(cbMap.MaxX(), cbMap.MinY(),z);
	glEnd();

	return true;
}

//------------------------------------ UI -----------------------------------------
HTREEITEM CanvasBackgroundDrawer::configure(LayerTreeView  *tv, HTREEITEM parent) {
	HTREEITEM hti = InsertItem(tv,TVI_ROOT,"Background Area","MapPane", TVI_LAST);
	ComplexDrawer::configure(tv,hti);

	DisplayOptionTreeItem *item = new DisplayOptionTreeItem(tv,hti,this, 
					(DisplayOptionItemFunc)&CanvasBackgroundDrawer::displayOptionOutsideColor);
	InsertItem("Outside map","SingleColor",item, -1);

	item = new DisplayOptionTreeItem(tv,hti,this,
					(DisplayOptionItemFunc)&CanvasBackgroundDrawer::displayOptionInsideColor);
	InsertItem("Inside map","SingleColor",item, -1);

	return hti;
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

