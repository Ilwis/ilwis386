#include "Client\Headers\formelementspch.h"
#include "Client\FormElements\fldlist.h"
#include "Client\FormElements\FldOneSelectTextOnly.h"
#include "Client\Mapwindow\Drawers\RootDrawer.h"
#include "Client\MapWindow\Drawers\ComplexDrawer.h"
#include "Client\Ilwis.h"
#include "Engine\Representation\Rpr.h"
#include "Client\Mapwindow\Drawers\AbstractMapDrawer.h"
#include "Client\Mapwindow\LayerTreeView.h"
#include "Client\Mapwindow\MapPaneViewTool.h"
#include "Client\Mapwindow\Drawers\DrawerTool.h"
#include "Client\Mapwindow\LayerTreeItem.h" 
#include "Drawers\SetDrawer.h"
#include "Drawers\AnimationDrawer.h"
#include "Drawers\ValueSlicer.h"
#include "Drawers\RasterSetDrawer.h"
#include "Client\Mapwindow\Drawers\DrawerContext.h"
#include "Drawers\InteractiveRepresentationTool.h"
#include "Drawers\SetDrawerTool.h"
#include "Drawers\AnimationTool.h"


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
	SetDrawerTool *sdrwt = dynamic_cast<SetDrawerTool *>(tool);
	AnimationTool *adrwt = dynamic_cast<AnimationTool *>(tool);
	if ( !sdrwt && !adrwt)
		return false;
	SetDrawer *sdrw = dynamic_cast<SetDrawer *>(tool->getDrawer());
	AnimationDrawer *adrw = dynamic_cast<AnimationDrawer *>(tool->getDrawer());
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
	SetDrawer *sdr = (SetDrawer *)adr;
	AnimationDrawer *animdrw = dynamic_cast<AnimationDrawer *>(adr);
	if ( animdrw)
		sdr = ((SetDrawer *)animdrw->getDrawer(0));
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


