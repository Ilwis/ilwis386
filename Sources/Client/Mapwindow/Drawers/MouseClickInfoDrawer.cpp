#include "Client\Headers\formelementspch.h"
#include "Client\Mapwindow\Drawers\DrawerContext.h"
#include "Client\Base\OpenGLText.h"
#include "Client\Mapwindow\Drawers\SimpleDrawer.h" 
#include "Client\Mapwindow\Drawers\TextDrawer.h"
#include "Client\Mapwindow\Drawers\MouseClickInfoDrawer.h" 
#include "Client\Mapwindow\Drawers\RootDrawer.h"
#include "Client\Mapwindow\Drawers\AbstractMapDrawer.h"
#include "Client\Mapwindow\Drawers\SimpleDrawer.h"
#include "Client\Mapwindow\LayerTreeView.h"
#include "Client\Mapwindow\LayerTreeItem.h"
#include "Client\Ilwis.h"




using namespace ILWIS;
ILWIS::NewDrawer *createMouseClickInfoDrawer(DrawerParameters *parms) {
	return new MouseClickInfoDrawer(parms);
}

MouseClickInfoDrawer::MouseClickInfoDrawer(DrawerParameters *parms) : 
	TextSetDrawer(parms,"MouseClickInfoDrawer"),
	hasText(false)
{
	name = id = "MouseClickInfoDrawer";
	setFont(new OpenGLText("arial.ttf",20));
}

MouseClickInfoDrawer::~MouseClickInfoDrawer() {
}

void  MouseClickInfoDrawer::prepare(PreparationParameters *pp){
	TextSetDrawer::prepare(pp);
	sInfo = "";
	if ( !activePoint.fUndef()) {
		for(int i =0; i < drawers.size(); ++i) {
			NewDrawer *drw = drawers[i];
			if ( drw->hasInfo() && drw->isActive()) {
				AbstractMapDrawer *amdrw = dynamic_cast<AbstractMapDrawer *>(drw);
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

bool MouseClickInfoDrawer::draw(bool norecursion, const CoordBounds& cbArea) const {
	if (getFont() && !activePoint.fUndef())
		getFont()->renderText(getRootDrawer(),activePoint, sInfo);
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
	TextSetDrawer::store(fnView, getType());

	return getType();
}

void MouseClickInfoDrawer::load(const FileName& fnView, const String& parenSection){
}

HTREEITEM MouseClickInfoDrawer::configure(LayerTreeView  *tv, HTREEITEM parent){
	CTreeCtrl& tc = tv->GetTreeCtrl();
	int iImg = IlwWinApp()->iImage("info");
	String sName = "Info";
	HTREEITEM hti = tc.InsertItem(sName.scVal(),iImg,iImg,TVI_ROOT,TVI_FIRST);
	tc.SetItemData(hti, (DWORD_PTR)new DrawerLayerTreeItem(tv, this));		
	tc.SetCheck(hti, isActive());
	return hti;
}




