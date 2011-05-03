#include "Client\Headers\formelementspch.h"
#include "Client\FormElements\fldlist.h"
#include "Client\FormElements\FldOneSelectTextOnly.h"
#include "Engine\Drawers\RootDrawer.h"
#include "Engine\Drawers\ComplexDrawer.h"
#include "Client\Ilwis.h"
#include "Engine\Representation\Rpr.h"
#include "Engine\Drawers\SpatialDataDrawer.h"
#include "Client\Mapwindow\LayerTreeView.h"
#include "Client\Mapwindow\MapPaneViewTool.h"
#include "Client\MapWindow\Drawers\DrawerTool.h"
#include "Client\Mapwindow\LayerTreeItem.h" 
#include "Drawers\LayerDrawer.h"
#include "Drawers\SetDrawer.h"
#include "ValueSlicer.h"
#include "Drawers\RasterDataDrawer.h"
#include "Engine\Drawers\DrawerContext.h"
#include "InteractiveRepresentationTool.h"
#include "DrawersUI\LayerDrawerTool.h"
#include "DrawersUI\SetDrawerTool.h"
#include "DrawersUI\AnimationTool.h"


DrawerTool *createInteractiveRepresentationTool(ZoomableView* zv, LayerTreeView *view, NewDrawer *drw) {
	return new InteractiveRepresentationTool(zv, view, drw);
}

InteractiveRepresentationTool::InteractiveRepresentationTool(ZoomableView* zv, LayerTreeView *view, NewDrawer *drw) : 
	DrawerTool("InteractiveRepresentationTool", zv, view, drw)
{
}

InteractiveRepresentationTool::~InteractiveRepresentationTool() {
}

bool InteractiveRepresentationTool::isToolUseableFor(ILWIS::DrawerTool *tool) {
	LayerDrawerTool *sdrwt = dynamic_cast<LayerDrawerTool *>(tool);
	AnimationTool *adrwt = dynamic_cast<AnimationTool *>(tool);
	if ( !sdrwt && !adrwt)
		return false;
	LayerDrawer *sdrw = dynamic_cast<LayerDrawer *>(tool->getDrawer());
	SetDrawer *adrw = dynamic_cast<SetDrawer *>(tool->getDrawer());
	RangeReal rr = adrw ? adrw->getStretchRangeReal() : sdrw->getStretchRangeReal();
	if ( rr.fValid())
		parentTool = tool;
	return rr.fValid();
}

HTREEITEM InteractiveRepresentationTool::configure( HTREEITEM parentItem) {
	if ( !active)
		return parentItem;

	DisplayOptionTreeItem * itemSlicing = new DisplayOptionTreeItem(tree, parentItem,drawer);
	itemSlicing->setDoubleCickAction(this, (DTDoubleClickActionFunc )&InteractiveRepresentationTool::rasterSlicing);
	htiNode = insertItem(TR("Interactive Slicing"),"Slicing",itemSlicing);
	DrawerTool::configure(htiNode);

	return htiNode;
}

void InteractiveRepresentationTool::rasterSlicing() {
	new InterActiveSlicing(tree,drawer);
}

String InteractiveRepresentationTool::getMenuString() const {
	return TR("Interactive Representation");
}

//----------------------------------------------------------
InterActiveSlicing::InterActiveSlicing(CWnd *par, NewDrawer *adr) 
	: DisplayOptionsForm2((ComplexDrawer *)adr, par, TR("Slicing"))
{
	LayerDrawer *sdr = (LayerDrawer *)adr;
	SetDrawer *animdrw = dynamic_cast<SetDrawer *>(adr);
	if ( animdrw)
		sdr = ((LayerDrawer *)animdrw->getDrawer(0));
	vs = new ValueSlicerSlider(root, sdr);
	FieldGroup *fg = new FieldGroup(root);
	fldSteps = new FieldOneSelectTextOnly(fg, &steps);
	fldSteps->SetCallBack((NotifyProc)&InterActiveSlicing::createSteps);
	fldSteps->Align(vs, AL_UNDER);
	fldSteps->SetWidth(vs->psn->iWidth/3);
	FlatIconButton *fb = new FlatIconButton(fg,"Save","",(NotifyProc)&InterActiveSlicing::saveRpr,fnRpr);
	fb->Align(fldSteps, AL_AFTER);


	create();
}

int InterActiveSlicing::saveRpr(Event *ev) {
	CFileDialog filedlg (FALSE, "*.rpr", "*.rpr",OFN_HIDEREADONLY|OFN_NOREADONLYRETURN | OFN_LONGNAMES, "Ilwis Representation (*.rpr)|*.rpr||", NULL);
	if ( filedlg.DoModal() == IDOK) {
		String name(filedlg.GetPathName());
		vs->setFileNameRpr(FileName(name));
	}
	return 1;
}

int InterActiveSlicing::createSteps(Event*) {
	if (fldSteps->ose->GetCount() == 0) {
		for(int i = 2 ; i <= 10; ++i)
			fldSteps->AddString(String("%d",i));
		fldSteps->ose->SelectString(0,"2");
	} else {
		int mapIndex = fldSteps->ose->GetCurSel();
		if ( mapIndex != -1) {
			vs->setNumberOfBounds(mapIndex +2);
		}
		drw->getRootDrawer()->getDrawerContext()->doDraw();
	}
	return 1;
}


