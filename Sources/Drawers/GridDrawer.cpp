#include "Client\Headers\formelementspch.h"
#include "Client\FormElements\syscolor.h"
#include "Client\FormElements\FieldIntSlider.h"
#include "Engine\Base\Round.h"
#include "Engine\Map\basemap.h"
#include "Client\Mapwindow\MapPaneView.h"
#include "Engine\Map\Point\ilwPoint.h"
#include "Client\Mapwindow\Drawers\DrawerContext.h"
#include "Client\Mapwindow\Drawers\ComplexDrawer.h"
#include "Client\Mapwindow\Drawers\SimpleDrawer.h" 
#include "Client\Ilwis.h"
#include "Engine\Base\System\RegistrySettings.h"
#include "Client\Mapwindow\MapCompositionDoc.h"
#include "Client\Mapwindow\Drawers\RootDrawer.h"
#include "Client\Mapwindow\Drawers\AbstractObjectdrawer.h"
#include "Client\Mapwindow\Drawers\AbstractMapDrawer.h"
#include "Client\Mapwindow\LayerTreeView.h"
#include "Client\Mapwindow\LayerTreeItem.h" 
#include "Drawers\LineDrawer.h"
#include "Drawers\GridDrawer.h"
#include "Client\Editors\Utils\line.h"
#include "Client\FormElements\fldcolor.h"
#include "drawers\linedrawer.h"
#include "Headers\Hs\Drwforms.hs"

using namespace ILWIS;

ILWIS::NewDrawer *createGridDrawer(DrawerParameters *parms) {
	return new GridDrawer(parms);
}

GridDrawer::GridDrawer(DrawerParameters *parms) : 
ComplexDrawer(parms,"GridDrawer")
{
	setDrawMethod(drmSINGLE); // default;
	id = name = "GridDrawer";
	rDist = rUNDEF;
	setActive(false);
}

GridDrawer::~GridDrawer() {
}

HTREEITEM GridDrawer::configure(LayerTreeView  *tv, HTREEITEM parent) {
	int iImg = IlwWinApp()->iImage(".grid");
	CTreeCtrl& tc = tv->GetTreeCtrl();
	HTREEITEM htiGrid = tc.InsertItem("Grid",iImg,iImg,TVI_ROOT,TVI_FIRST);
	DisplayOptionTreeItem *item = new DisplayOptionTreeItem(tv,TVI_ROOT,this,
					(SetCheckFunc)&GridDrawer::gridActive,
					(DisplayOptionItemFunc)&GridDrawer::gridOptions);
	item->setTreeItem(htiGrid);
	tc.SetCheck(htiGrid, isActive() );
	tc.SetItemData(htiGrid, (DWORD_PTR)item);

	return parent;
}

void GridDrawer::gridOptions(CWnd *parent) {
	new GridForm(parent, this);
}

void GridDrawer::gridActive(void *v, LayerTreeView *tv) {
	bool value = *(bool *)v;
	setActive(value);
	MapCompositionDoc* doc = tv->GetDocument();
	doc->mpvGetView()->Invalidate();
}

bool GridDrawer::draw(bool norecursion, const CoordBounds& cbArea) const{
	return ComplexDrawer::draw(norecursion, cbArea);
}

void GridDrawer::prepare(PreparationParameters *pp) {
	ILWISSingleLock csl(&cs, TRUE, SOURCE_LOCATION);
	String sVal;
	Coord c, cMin, cMax;
	Color clr;
	clear();
	CoordBounds cbMap = getDrawerContext()->getMapCoordBounds();
	cMin = cbMap.cMin;
	cMax = cbMap.cMax;
	if ( rDist == rUNDEF)
		rDist = rRound((cMax.x - cMin.x) / 7);

	int iCntX = (long)((cMax.x - cMin.x) / rDist);
	int iCntY = (long)((cMax.y - cMin.y) / rDist);

	Coord c1, c2;
	c1.y = cMin.y;
	c2.y = cMax.y;
	for (double x = ceil(cMin.x / rDist) * rDist; x < cMax.x ; x += rDist)
	{
		c1.x = c2.x = x;
		AddGridLine(c1, c2);
	}

	c1.x = cMin.x;
	c2.x = cMax.x;
	for (double y = ceil(cMin.y / rDist) * rDist; y < cMax.y ; y += rDist)
	{
		c1.y = c2.y = y;
		AddGridLine(c1, c2);
	}
}

void GridDrawer::AddGridLine(Coord c1, Coord c2)
{
	ILWIS::DrawerParameters dp(drawcontext, this);
	PreparationParameters pp(NewDrawer::ptGEOMETRY);
	GridLine *line = (GridLine *)IlwWinApp()->getDrawer("GridLine", &pp, &dp);
	line->addDataSource(&c1);
	line->addDataSource(&c2);
	line->setDrawColor(color);
	drawers.push_back(line);

}

//void GridDrawer::DrawCurvedLine(Coord c1, Coord c2)
//{
//	Coord cStep, crd;
//	int iSteps = 500;
//	cStep.x = (c2.x - c1.x) / iSteps;
//	cStep.y = (c2.y - c1.y) / iSteps;
//	p = psn->pntPos(c1);
//	if (p.x == shUNDEF || p.y == shUNDEF)
//		fPointOk = false;
//	else {
//		fPointOk = true;
//		cdc->MoveTo(p);
//	}
//	crd = c1;
//	for (int i = 0; i < iSteps; ++i) {
//		crd += cStep;
//		p = psn->pntPos(crd);
//		if (p.x == shUNDEF || p.y == shUNDEF)
//			fPointOk = false;
//		else if (fPointOk) 
//			cdc->LineTo(p);
//		else {
//			fPointOk = true;
//			cdc->MoveTo(p);
//		}
//	}
//}
//---------------------------------------------
ILWIS::NewDrawer *createGridLine(DrawerParameters *parms) {
	return new GridLine(parms);
}

GridLine::GridLine(DrawerParameters *parms) : LineDrawer(parms,"GridLine"){
	drawColor = SysColor(COLOR_WINDOWTEXT);
}

GridLine::~GridLine(){
}

bool GridLine::draw(bool norecursion, const CoordBounds& cbArea) const{
	return LineDrawer::draw(norecursion, cbArea);
}

void GridLine::prepare(PreparationParameters *pp){
	LineDrawer::prepare(pp);
	if ( pp->type & NewDrawer::ptGEOMETRY)
		clear();

}

void GridLine::addDataSource(void *crd, int options) {
	Coord c = *((Coord *)crd);
	cb += c;
	if (lines.size() == 0)
		lines.push_back(new CoordinateArraySequence());
	lines.at(0)->add(c);
}

//-------------------------------
GridForm::GridForm(CWnd *par, GridDrawer *gdr) 
	: DisplayOptionsForm(gdr, par, SDCTitleGrid),
	transparency(100 *(1.0-gdr->getTransparency()))
{
  iImg = IlwWinApp()->iImage(".grid");

  fr = new FieldReal(root, SDCUiGridDistance, &gdr->rDist, ValueRange(0.0,1e10,0.001));
 // new FieldLineType(root, SDCUiLineType, &gdr->ldt);
  fc = new FieldColor(root, SDCUiColor, &gdr->color);
  slider = new FieldIntSliderEx(root,"Transparency(0-100)", &transparency,ValueRange(0,100),true);

  create();
}

void  GridForm::apply() {
	fc->StoreData();
	fr->StoreData();
	slider->StoreData();
	PreparationParameters pp(NewDrawer::ptRENDER);
	drw->setTransparency(1.0 - (double)transparency/100.0);
	drw->prepare(&pp);
	updateMapView();
}

//------------------------------------------------------



