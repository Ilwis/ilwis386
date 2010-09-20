#include "Client\Headers\formelementspch.h"
#include "Client\FormElements\fldcolor.h"
#include "Engine\Map\basemap.h"
#include "Engine\Base\System\RegistrySettings.h"
#include "Client\Mapwindow\MapCompositionDoc.h"
#include "Client\Mapwindow\Drawers\DrawerContext.h"
#include "Client\MapWindow\Drawers\ComplexDrawer.h"
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
	outside2D = Color(179,179,179);
	inside2D = Color(255,255,255);
	outside3D = Color(235,242,235);
	inside3D = Color(245,245,245);
}

CanvasBackgroundDrawer::~CanvasBackgroundDrawer() {
}
void  CanvasBackgroundDrawer::prepare(PreparationParameters *pp){
	ComplexDrawer::prepare(pp);

}

bool CanvasBackgroundDrawer::draw(bool norecursion, const CoordBounds& cb) const{
	CoordBounds cbView = getRootDrawer()->getCoordBoundsView();
	CoordBounds cbMap = getRootDrawer()->getMapCoordBounds();
	bool is3D = getRootDrawer()->is3D();
	if ( is3D) {
		glColor4d(outside3D.redP(), outside3D.greenP(), outside3D.blueP(),getTransparency());
	}
	else {
		glColor4d(outside2D.redP(), outside2D.greenP(), outside2D.blueP(),getTransparency());
	}
	double z = is3D ? -getRootDrawer()->getFakeZ() : 0;
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

	if ( is3D) {
		glColor4d(inside3D.redP(), inside3D.greenP(), inside3D.blueP(),getTransparency());
	}
	else {
		glColor4d(inside2D.redP(), inside2D.greenP(), inside2D.blueP(),getTransparency());
	}
	glBegin(GL_QUADS);						
		glVertex3f(cbMap.MinX(), cbMap.MinY(),z);				
		glVertex3f(cbMap.MinX(), cbMap.MaxY(),z);				
		glVertex3f(cbMap.MaxX(), cbMap.MaxY(),z);				
		glVertex3f(cbMap.MaxX(), cbMap.MinY(),z);
	glEnd();

	return true;
}

String CanvasBackgroundDrawer::store(const FileName& fnView, const String& parentSection) const{
	ComplexDrawer::store(fnView, getType());
	ObjectInfo::WriteElement(getType().scVal(),"InSideColor2D",fnView, inside2D);
	ObjectInfo::WriteElement(getType().scVal(),"InSideColor3D",fnView, inside3D);
	ObjectInfo::WriteElement(getType().scVal(),"OutSideColor2D",fnView, outside2D);
	ObjectInfo::WriteElement(getType().scVal(),"OutSideColor3D",fnView, outside3D);
	return getType();
}

void CanvasBackgroundDrawer::load(const FileName& fnView, const String& parentSection){
	ComplexDrawer::load(fnView, getType());
	ObjectInfo::ReadElement(getType().scVal(),"InSideColor2D",fnView, inside2D);
	ObjectInfo::ReadElement(getType().scVal(),"InSideColor3D",fnView, inside3D);
	ObjectInfo::ReadElement(getType().scVal(),"OutSideColor2D",fnView, outside2D);
	ObjectInfo::ReadElement(getType().scVal(),"OutSideColor3D",fnView, outside3D);
}

//------------------------------------ UI -----------------------------------------
HTREEITEM CanvasBackgroundDrawer::configure(LayerTreeView  *tv, HTREEITEM parent) {
	HTREEITEM hti = InsertItem(tv,TVI_ROOT,"Background Area","MapPane", TVI_LAST);
	ComplexDrawer::configure(tv,hti);
	bool is3D = getRootDrawer()->is3D();

	DisplayOptionColorItem *item = new DisplayOptionColorItem("Outside", tv,hti,this, 
					(DisplayOptionItemFunc)&CanvasBackgroundDrawer::displayOptionOutsideColor);
	item->setColor(is3D ? outside3D : outside2D);
	InsertItem("Outside map","SingleColor",item, -1);

	item = new DisplayOptionColorItem("Inside", tv,hti,this,
					(DisplayOptionItemFunc)&CanvasBackgroundDrawer::displayOptionInsideColor);
	InsertItem("Inside map","SingleColor",item, -1);
	item->setColor(is3D ? inside3D : inside2D);

	return hti;
}

void CanvasBackgroundDrawer::displayOptionOutsideColor(CWnd *parent) {
	new SetColorForm("Outside map", parent, this, getRootDrawer()->is3D() ? &outside3D : &outside2D);
}

void CanvasBackgroundDrawer::displayOptionInsideColor(CWnd *parent) {
	new SetColorForm("Inside map", parent, this, getRootDrawer()->is3D() ? &inside3D : &inside2D);
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

