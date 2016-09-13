#include "Client\Headers\formelementspch.h"
#include "Client\FormElements\fldcolor.h"
#include "Engine\Drawers\RootDrawer.h"
#include "Engine\Drawers\ComplexDrawer.h"
#include "Client\Ilwis.h"
#include "Engine\Representation\Rpr.h"
#include "Engine\Drawers\SpatialDataDrawer.h"
#include "Client\Mapwindow\LayerTreeView.h"
#include "Client\Mapwindow\MapPaneViewTool.h"
#include "Client\MapWindow\Drawers\DrawerTool.h"
#include "DrawersUI\ColorTool.h"
#include "Drawers\LayerDrawer.h"
#include "Drawers\DrawingColor.h" 
#include "Drawers\FeatureLayerDrawer.h"
#include "Client\Mapwindow\LayerTreeItem.h" 
#include "Engine\Drawers\DrawerContext.h"
#include "DrawersUI\NonRepresentationTool.h"

DrawerTool *createNonRepresentationToolTool(ZoomableView* zv, LayerTreeView *view, NewDrawer *drw) {
	return new NonRepresentationToolTool(zv, view, drw);
}

NonRepresentationToolTool::NonRepresentationToolTool(ZoomableView* zv, LayerTreeView *view, NewDrawer *drw) : 
	DrawerTool("NonRepresentationToolTool", zv, view, drw)
{
}

NonRepresentationToolTool::~NonRepresentationToolTool() {
}

bool NonRepresentationToolTool::isToolUseableFor(ILWIS::DrawerTool *tool) { 

	if ( dynamic_cast<NonRepresentationToolTool *>(tool->getParentTool()) != 0) 
		return false;
	if ( dynamic_cast<ColorTool *>(tool) == 0)
		return false;

	FeatureLayerDrawer *sdrw = dynamic_cast<FeatureLayerDrawer *>(tool->getDrawer());
	if (!sdrw)
		return false;
	Representation rpr = sdrw->getRepresentation();
	bool isAcceptable = true;
	if ( rpr.fValid()) {
		isAcceptable = !(rpr->prv() || rpr->prg());
	}
	if ( isAcceptable)
		parentTool = tool;
	return isAcceptable;
}

HTREEITEM NonRepresentationToolTool::configure( HTREEITEM parentItem) {
	int iImgLeg = IlwWinApp()->iImage("Picture");
	htiNode = tree->GetTreeCtrl().InsertItem(TR("Fixed Colors").c_str(), iImgLeg, iImgLeg, parentItem);
	FeatureLayerDrawer *sdrw = dynamic_cast<FeatureLayerDrawer *>(drawer);
	BaseMap *bmp = (BaseMap *)sdrw->getDataSource();
	ColorTool *ctool = (ColorTool *)parentTool;
	bool useSingleColor = sdrw->getDrawMethod() == NewDrawer::drmSINGLE;
	bool useRpr = sdrw->getDrawMethod() == NewDrawer::drmRPR;
	DisplayOptionRadioButtonItem *colorItem = new DisplayOptionRadioButtonItem("Single color", tree,htiNode,drawer);
	colorItem->setDoubleCickAction(this,(DTDoubleClickActionFunc)&NonRepresentationToolTool::displayOptionSingleColor);
	colorItem->setCheckAction(ctool,ctool->getColorCheck(), (DTSetCheckFunc)&ColorTool::setcheckRpr);
	colorItem->setState(useSingleColor);
	HTREEITEM singleColorItem = insertItem("Single Color","SingleColor",colorItem, -1);
	if ( (*bmp)->dm()->pdid() || (*bmp)->dm()->pdUniqueID()) {
		DisplayOptionRadioButtonItem *item = new DisplayOptionRadioButtonItem("Multiple Colors", tree,htiNode,drawer);	
		item->setCheckAction(ctool,ctool->getColorCheck(), (DTSetCheckFunc)&ColorTool::setcheckRpr);
		item->setDoubleCickAction(this,(DTDoubleClickActionFunc)&NonRepresentationToolTool::displayOptionMultiColor);
		item->setState(!useRpr && !useSingleColor);
		HTREEITEM multiColorItem = insertItem("Multiple Colors","MultipleColors",item, -1);
	}


	DrawerTool::configure(htiNode);

	return htiNode;
}

void NonRepresentationToolTool::displayOptionMultiColor() {
	new SetMultipleColorForm(tree, (FeatureLayerDrawer *)drawer);
}

void NonRepresentationToolTool::displayOptionSingleColor() {
	new SetSingleColorForm(tree, (FeatureLayerDrawer *)drawer);
}

String NonRepresentationToolTool::getMenuString() const {
	return TR("Fixed Color Scheme");
}
//------------------------------------------------
SetSingleColorForm::SetSingleColorForm(CWnd *wPar, FeatureLayerDrawer *dr) : 
	DisplayOptionsForm(dr, wPar,String("Single draw color for %S",dr->getName())),
	c(dr->getSingleColor())
{
	c.alpha() = 255 - c.alpha(); // inverse the alpha, for FieldColor
	fc = new FieldColor(root, "Draw color", &c);
	create();
}

void  SetSingleColorForm::apply() {
	fc->StoreData();
	Color clr (c);
	clr.alpha() = 255 - clr.alpha(); // inverse the alpha again, for displaying
	((FeatureLayerDrawer *)drw)->setSingleColor(clr);
	LayerDrawer *lyerdrw = dynamic_cast<LayerDrawer *>(drw);
	if (lyerdrw)
		lyerdrw->setUseRpr(false);
	PreparationParameters parm(NewDrawer::ptRENDER, 0);
	drw->prepareChildDrawers(&parm);
	updateMapView();
}
//-------------------------------------------------------------
SetMultipleColorForm::SetMultipleColorForm(CWnd *wPar, FeatureLayerDrawer *dr) : 
	DisplayOptionsForm(dr, wPar,String("Color Scheme")),
	choice(1)
{
	loadColorSets("");
	choice = ((FeatureLayerDrawer *)drw)->getDrawingColor()->colorSet();
	colors = ((FeatureLayerDrawer *)drw)->getDrawingColor()->multiColors();
	fo = new FieldOneSelectString(root, "Color Schemes", &choice, sets);
	rg = new RadioGroup(root,"Size color set",&colors);
	new RadioButton(rg,"8");
	new RadioButton(rg,"16");
	new RadioButton(rg,"32");

	create();
}
void SetMultipleColorForm::loadColorSets(const String& folder) {
	String pathToColorSet;
	if ( folder == "") {
		String ilwDir = getEngine()->getContext()->sIlwDir();
		pathToColorSet = ilwDir + "\\System\\*.ics";
	} else
		pathToColorSet = folder;

	CFileFind finder;
	BOOL fFound = finder.FindFile(pathToColorSet.c_str());
	vector<FileName> icsfiles;
	while(fFound) {
		fFound = finder.FindNextFile();
		if (finder.IsDirectory())
		{
			FileName fnFolder(finder.GetFilePath());
			if ( fnFolder.sFile != "." && fnFolder.sFile != ".." && fnFolder.sFile != "")
				loadColorSets(fnFolder.sFullPath());
		}
		else {
			FileName fnCS(finder.GetFilePath());
			icsfiles.push_back(fnCS);
		}
	}
	sets.resize(icsfiles.size());
	for(int i = 0; i < icsfiles.size(); ++i) {
		ifstream file(icsfiles[i].sPhysicalPath().c_str());
		if ( file.is_open()) {
			int index = iUNDEF;
			String line;
			file >> line;
			String head = line.sHead("=");
			String tail=line.sTail("=");
			if ( head == "index") {
				index = tail.iVal();
				sets[index] = icsfiles[i].sFile;
			}
			file.close();
		}
	}

}
void  SetMultipleColorForm::apply() {
	fo->StoreData();
	rg->StoreData();
	((FeatureLayerDrawer *)drw)->getDrawingColor()->setColorSet(choice);
	((FeatureLayerDrawer *)drw)->getDrawingColor()->setMultiColors(colors);
	LayerDrawer *lyerdrw = dynamic_cast<LayerDrawer *>(drw);
	if (lyerdrw)
		lyerdrw->setUseRpr(false);
	PreparationParameters parm(NewDrawer::ptRENDER, 0);
	drw->prepareChildDrawers(&parm);
	updateMapView();
}

