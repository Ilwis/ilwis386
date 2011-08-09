#include "Headers\toolspch.h"
#include "Engine\Table\tbl.h"
#include "Engine\Map\basemap.h"
#include "Engine\Drawers\DrawerContext.h"
#include "Engine\Drawers\OpenGLText.h"
#include "Engine\Drawers\SimpleDrawer.h" 
#include "Engine\Drawers\TextDrawer.h"
#include "Engine\Drawers\MouseClickInfoDrawer.h" 
#include "Engine\Drawers\RootDrawer.h"
#include "Engine\Drawers\SpatialDataDrawer.h"
#include "Engine\Drawers\SimpleDrawer.h"


using namespace ILWIS;
ILWIS::NewDrawer *createMouseClickInfoDrawer(DrawerParameters *parms) {
	return new MouseClickInfoDrawer(parms);
}

MouseClickInfoDrawer::MouseClickInfoDrawer(DrawerParameters *parms) : 
	TextLayerDrawer(parms,"MouseClickInfoDrawer"),
	hasText(false)
{
	name = id = "MouseClickInfoDrawer";
	setFont(new OpenGLText(getRootDrawer(), "arial.ttf",20));
}

MouseClickInfoDrawer::~MouseClickInfoDrawer() {
}

void  MouseClickInfoDrawer::prepare(PreparationParameters *pp){
	TextLayerDrawer::prepare(pp);
	sInfo = "";
	if ( !activePoint.fUndef()) {
		for(int i =0; i < drawers.size(); ++i) {
			NewDrawer *drw = drawers[i];
			if ( drw->hasInfo() && drw->isActive()) {
				SpatialDataDrawer *amdrw = dynamic_cast<SpatialDataDrawer *>(drw);
				if ( amdrw) {
					BaseMapPtr *bm = amdrw->getBaseMap();
					vector<String> values = bm->vsValue(activePoint);
					if ( values.size() > 0 && values[0] != sUNDEF)
						sInfo += values[0];
				}
			}
		}
	}
}

bool MouseClickInfoDrawer::draw( const CoordBounds& cbArea) const {
	if (getFont() && !activePoint.fUndef())
		getFont()->renderText(activePoint, sInfo);
	return true;
}

void MouseClickInfoDrawer::addDataSource(void *v) {
	NewDrawer *drw = dynamic_cast<NewDrawer *>((NewDrawer *)v);
	if ( drw)
		drawers.push_back(drw);
}

void MouseClickInfoDrawer::setActivePoint(const Coord& c) {
	activePoint = c;
}

String MouseClickInfoDrawer::store(const FileName& fnView, const String& parentSection) const{
	TextLayerDrawer::store(fnView, getType());

	return getType();
}

void MouseClickInfoDrawer::load(const FileName& fnView, const String& parenSection){
}

HTREEITEM MouseClickInfoDrawer::configure(LayerTreeView  *tv, HTREEITEM parent){
	//CTreeCtrl& tc = tv->GetTreeCtrl();
	//int iImg = IlwWinApp()->iImage("info");
	//String sName = "Info";
	//HTREEITEM hti = tc.InsertItem(sName.c_str(),iImg,iImg,TVI_ROOT,TVI_FIRST);
	//tc.SetItemData(hti, (DWORD_PTR)new DrawerLayerTreeItem(tv, this));		
	//tc.SetCheck(hti, isActive());
	//return hti;
	return 0;
}




