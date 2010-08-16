#include "Client\Headers\formelementspch.h"
#include "Client\Mapwindow\Drawers\DrawerContext.h"
#include "Client\Base\OpenGLText.h"
#include "Client\Mapwindow\Drawers\SimpleDrawer.h" 
#include "Client\Mapwindow\Drawers\TextDrawer.h"
#include "Client\Mapwindow\Drawers\MouseClickInfoDrawer.h" 
#include "Client\Mapwindow\Drawers\RootDrawer.h"
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
	setFont(new OpenGLText("arial.ttf",25));
}

MouseClickInfoDrawer::~MouseClickInfoDrawer() {
}

void  MouseClickInfoDrawer::prepare(PreparationParameters *pp){
	TextSetDrawer::prepare(pp);
	sInfo = "";
	for(int i =0; i < maps.size(); ++i) {
		BaseMap bm = maps[i];
		vector<String> values = bm->vsValue(activePoint);
		if ( values.size() > 0)
			sInfo += values[0];
	}
}

bool MouseClickInfoDrawer::draw(bool norecursion, const CoordBounds& cbArea) const {
	if (getFont() && !activePoint.fUndef())
		getFont()->renderText(getDrawerContext(),activePoint, sInfo);
	return true;
}

void MouseClickInfoDrawer::addDataSource(void *v) {
	BaseMap *pbm = dynamic_cast<BaseMap *>((BaseMap *)v);
	if ( pbm)
		maps.push_back(*pbm);
}

void MouseClickInfoDrawer::setActivePoint(const Coord& c) {
	activePoint = c;
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




