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
#include "Client\Mapwindow\LayerTreeView.h"
#include "Client\Mapwindow\LayerTreeItem.h" 
#include "Client\Editors\Utils\line.h"
#include "Drawers\LineDrawer.h"
#include "Drawers\GridDrawer.h"
#include "Client\FormElements\fldcolor.h"
#include "Client\Mapwindow\Drawers\ZValueMaker.h"
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
	setTransparency(0.3);
	threeD = false;
	linethickness = 1;
	color = Color(0,0,0);
	linestyle = ldtSingle;
}

GridDrawer::~GridDrawer() {
}


bool GridDrawer::draw(bool norecursion, const CoordBounds& cbArea) const{
	return ComplexDrawer::draw(norecursion, cbArea);
}

void GridDrawer::prepare(PreparationParameters *pp) {
	if (  pp->type & RootDrawer::ptGEOMETRY){ 
		String sVal;
		Coord c, cMin, cMax;
		Color clr;
		clear();
		getZMaker()->setThreeDPossible(true);
		CoordBounds cbMap = getRootDrawer()->getMapCoordBounds();
		cMin = cbMap.cMin;
		cMax = cbMap.cMax;
		double maxz = min(cbMap.width(), cbMap.height()) / 2.0;
		double zplanes =  threeD ? 4 : 0;
		if ( rDist == rUNDEF)
			rDist = rRound((cMax.x - cMin.x) / 7);

		Coord c1, c2;
	
		prepareGrid(maxz,zplanes, rDist,cMax, cMin);
		if ( threeD) {
			prepareVerticals(maxz,rDist,cMax, cMin);
		}
	}
	if ( pp->type & NewDrawer::ptRENDER) {
		for(int i=0; i < drawers.size(); ++i) {
			LineDrawer *ld = (LineDrawer *)drawers.at(i);
			ld->setThickness(linethickness);
			ld->setDrawColor(color);
			switch(linestyle) {
			case ldtDot:
				ld->setLineStyle(0xAAAA); break;
			case ldtDash:
				ld->setLineStyle(0xF0F0); break;
			case ldtDashDot:
				ld->setLineStyle(0x6B5A); break;
			case ldtDashDotDot:
				ld->setLineStyle(0x56B5); break;
			default:
				ld->setLineStyle(0xFFFF);
			}
		}
	}
}

void GridDrawer::prepareVerticals(double maxz, double rDist,const Coord& cMax, const Coord& cMin) {
	Coord c1, c2;
	for (double x = ceil(cMin.x / rDist) * rDist; x < cMax.x ; x += rDist)
	{
		for (double y = ceil(cMin.y / rDist) * rDist; y < cMax.y ; y += rDist)
		{
			c1.x = x;
			c1.y = y;
			c1.z =  getRootDrawer()->getFakeZ();
			c2.x = x;
			c2.y = y;
			c2.z = maxz;
			AddGridLine(c1, c2);
		}
	}
	c1 = cMin;
	c1.z  = getRootDrawer()->getFakeZ();
	c2 = c1;
	c2.z = maxz;
	AddGridLine(c1,c2);
	c1.x = cMin.x;
	c1.y = cMax.y;
	c1.z  = getRootDrawer()->getFakeZ();
	c2 = c1;
	c2.z = maxz;
	AddGridLine(c1,c2);
	c1 = cMax;
	c1.z  = getRootDrawer()->getFakeZ();
	c2 = c1;
	c2.z = maxz;
	AddGridLine(c1,c2);
	c1.x = cMax.x;
	c1.y = cMin.y;
	c1.z  = getRootDrawer()->getFakeZ();
	c2 = c1;
	c2.z = maxz;

}
void GridDrawer::prepareGrid(double maxz, double zplanes, double rDist, const Coord& cMax, const Coord& cMin ) {
	Coord c1, c2;
	double z = 	getRootDrawer()->getFakeZ();
	double zdist = maxz / zplanes;
	for(int i=0; i <= zplanes; ++i) {
		c1.z = c2.z = z;
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
		if ( threeD) {
			AddGridLine(Coord(cMin.x,cMin.y,z), Coord(cMin.x, cMax.y,z));
			AddGridLine(Coord(cMin.x,cMax.y,z), Coord(cMax.x, cMax.y,z));
			AddGridLine(Coord(cMax.x,cMax.y,z), Coord(cMax.x, cMin.y,z));
			AddGridLine(Coord(cMax.x,cMin.y,z), Coord(cMin.x, cMin.y,z));
		}
		z += zdist;
	}
}
void GridDrawer::AddGridLine(Coord c1, Coord c2)
{
	ILWIS::DrawerParameters dp(getRootDrawer(), this);
	PreparationParameters pp(NewDrawer::ptGEOMETRY);
	GridLine *line = (GridLine *)IlwWinApp()->getDrawer("GridLine", &pp, &dp);
	line->addDataSource(&c1);
	line->addDataSource(&c2);
	line->setDrawColor(color);
	addDrawer(line);
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

String GridDrawer::store(const FileName& fnView, const String& parentSection) const{
	ComplexDrawer::store(fnView, getType());

	return getType();
}

void GridDrawer::load(const FileName& fnView, const String& parenSection){
}
//------------------------------------------- UI -------------------------------

HTREEITEM GridDrawer::configure(LayerTreeView  *tv, HTREEITEM parent) {
	DisplayOptionTreeItem *item = new DisplayOptionTreeItem(tv,TVI_ROOT,this,
					(SetCheckFunc)&GridDrawer::gridActive,
					(DisplayOptionItemFunc)&GridDrawer::gridOptions);
	HTREEITEM htiGrid = InsertItem("Grid",".grid",item, isActive(),TVI_FIRST);
	ComplexDrawer::configure(tv, htiGrid);

	item = new DisplayOptionTreeItem(tv,htiGrid,this,(DisplayOptionItemFunc)&GridDrawer::displayOptionSetLineStyle);
	InsertItem("Line style","LineStyle", item, -1);
	item = new DisplayOptionTreeItem(tv, htiGrid, this, (SetCheckFunc)&GridDrawer::grid3D);
	InsertItem("3D Grid","3D",item,threeD);

	return parent;
}

void GridDrawer::displayOptionSetLineStyle(CWnd *parent) {
	new GridLineStyleForm(parent, this);
}

void GridDrawer::grid3D(void *v, LayerTreeView *tv) {
	threeD = *(bool *)v;
	MapCompositionDoc* doc = tv->GetDocument();
	PreparationParameters pp(NewDrawer::ptGEOMETRY);
	prepare(&pp);
	doc->mpvGetView()->Invalidate();
}

void GridDrawer::gridOptions(CWnd *parent) {
	new GridForm(parent, this);
}

void GridDrawer::gridActive(void *v, LayerTreeView *tv) {
	bool value = *(bool *)v;
	setActive(value);
	PreparationParameters pp(NewDrawer::ptGEOMETRY);
	prepare(&pp);
	MapCompositionDoc* doc = tv->GetDocument();
	doc->mpvGetView()->Invalidate();
}
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
	: DisplayOptionsForm(gdr, par, SDCTitleGrid)
	
{
  fr = new FieldReal(root, SDCUiGridDistance, &gdr->rDist, ValueRange(0.0,1e10,0.001));
  fc = new FieldColor(root, SDCUiColor, &gdr->color);

  create();
}

void  GridForm::apply() {
	fc->StoreData();
	fr->StoreData();
	PreparationParameters pp(NewDrawer::ptRENDER);
	drw->prepare(&pp);
	updateMapView();
}

//------------------------------------------------------

GridLineStyleForm::GridLineStyleForm(CWnd *par, GridDrawer *ldr) 
	: DisplayOptionsForm(ldr, par, "Line Style")
{

  fi = new FieldReal(root, "Line thickness", &ldr->linethickness, ValueRange(1.0,100.0));
  flt = new FieldLineType(root, SDCUiLineType, &ldr->linestyle);
  fc = new FieldColor(root, "Line color",&ldr->color);

  create();
}

void  GridLineStyleForm::apply() {
	fi->StoreData();
	flt->StoreData();
	fc->StoreData();
	PreparationParameters pp(NewDrawer::ptRENDER);
	drw->prepare(&pp);
	updateMapView();
}

