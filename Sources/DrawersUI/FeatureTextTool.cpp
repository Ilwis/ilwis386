#include "Client\Headers\formelementspch.h"
#include "Client\FormElements\fldcol.h"
#include "Client\FormElements\fldcolor.h"
#include "Client\FormElements\objlist.h"
#include "Engine\Drawers\RootDrawer.h"
#include "Engine\Drawers\ComplexDrawer.h"
#include "Client\Ilwis.h"
#include "Engine\Representation\Rpr.h"
#include "Engine\Drawers\SpatialDataDrawer.h"
#include "Client\Mapwindow\LayerTreeView.h"
#include "Client\Mapwindow\MapPaneViewTool.h"
#include "Client\MapWindow\Drawers\DrawerTool.h"
#include "Client\Mapwindow\LayerTreeItem.h" 
#include "Engine\Drawers\DrawerContext.h"
#include "Client\Mapwindow\MapPaneView.h"
#include "Client\Mapwindow\MapCompositionDoc.h"
#include "Drawers\LayerDrawer.h"
#include "Drawers\FeatureLayerDrawer.h"
#include "Drawers\PointLayerDrawer.h"
#include "Engine\Drawers\SVGLoader.h"
#include "Engine\Drawers\SimpleDrawer.h" 
#include "drawers\pointdrawer.h"
#include "DrawersUI\FeatureTextTool.h"
#include "DrawersUI\LayerDrawerTool.h"
#include "DrawersUI\SetDrawerTool.h"
#include "DrawersUI\PointSymbolizationTool.h"
#include "FeatureTextTool.h"
#include "Drawers\SetDrawer.h"
#include "DrawersUI\PolygonSetTool.h"
#include "Engine\Drawers\OpenGLText.h"
#include "Engine\Drawers\TextDrawer.h"
#include "Headers\Hs\Drwforms.hs"


DrawerTool *createFeatureTextTool(ZoomableView* zv, LayerTreeView *view, NewDrawer *drw) {
	return new FeatureTextTool(zv, view, drw);
}

FeatureTextTool::FeatureTextTool(ZoomableView* zv, LayerTreeView *view, NewDrawer *drw) : 
	DrawerTool("FeatureTextTool", zv, view, drw)
{
		active = true;
}

FeatureTextTool::~FeatureTextTool() {
}

bool FeatureTextTool::isToolUseableFor(ILWIS::DrawerTool *tool) { 

	ComplexDrawer* cdrw = dynamic_cast<ComplexDrawer*>(drawer);
	if (!cdrw)
		return false;
	PointSymbolizationTool *pst = dynamic_cast<PointSymbolizationTool *>(tool);
	PolygonSetTool *poltool = dynamic_cast<PolygonSetTool *>(tool);
	SpatialDataDrawer *spdrw = cdrw->isSet() ?
		dynamic_cast<SpatialDataDrawer *>(drawer) :
		dynamic_cast<SpatialDataDrawer *>(drawer->getParentDrawer());
	if ( !(pst || poltool) || !spdrw )
		return false;
	BaseMapPtr *bmptr = spdrw->getBaseMap();
	parentTool = tool;
	if ( bmptr->fTblAtt()) {
		Table tblAtt = bmptr->tblAtt();
		if ( tblAtt.fValid()) {
			tbl = tblAtt;
		}
	}
	return true;
}

HTREEITEM FeatureTextTool::configure( HTREEITEM parentItem) {
	DisplayOptionTreeItem *item = new DisplayOptionTreeItem(tree,parentItem,drawer);
	item->setDoubleCickAction(this,(DTDoubleClickActionFunc)&FeatureTextTool::setScaling);
	item->setCheckAction(this, 0,(DTSetCheckFunc)&FeatureTextTool::makeActive);
	htiNode = insertItem(TR("Labels"),".atx",item, 0);
	DrawerTool::configure(htiNode);

	return htiNode;
}

void FeatureTextTool::makeActive(void *v, HTREEITEM ) {
	if (((ComplexDrawer*)drawer)->isSet()) {
		for(int i = 0; i < ((ComplexDrawer*)drawer)->getDrawerCount(); ++i) {
			FeatureLayerDrawer *fldrw = dynamic_cast<FeatureLayerDrawer *>(((ComplexDrawer*)drawer)->getDrawer(i));
			if ( !fldrw)
				continue;
			TextLayerDrawer *texts = static_cast<TextLayerDrawer *>(fldrw->getDrawer(223, ComplexDrawer::dtPOST));
			if ( texts) {
				bool act = *(bool *)v;
				texts->setActive(act);
			}
		}
	} else {
		FeatureLayerDrawer *fldrw = dynamic_cast<FeatureLayerDrawer *>(drawer);
		if ( !fldrw)
			return;
		TextLayerDrawer *texts = static_cast<TextLayerDrawer *>(fldrw->getDrawer(223, ComplexDrawer::dtPOST));
		if ( texts) {
			bool act = *(bool *)v;
			texts->setActive(act);
		}
	}
	tree->GetDocument()->mpvGetView()->Invalidate();
}

void FeatureTextTool::setScaling() {
	new FeatureTextToolForm(tree, (FeatureLayerDrawer *)drawer, tbl);
}

String FeatureTextTool::getMenuString() const {
	return TR("Labels");
}

//---------------------------------------------------
FeatureTextToolForm::FeatureTextToolForm(CWnd *wPar, FeatureLayerDrawer *dr, const Table& _tbl):
DisplayOptionsForm(dr,wPar,TR("Labels")), tbl(_tbl), fscale(1.0), useAttrib(false), cb(0)
{
	TextLayerDrawer *texts = static_cast<TextLayerDrawer *>(dr->getDrawer(223, ComplexDrawer::dtPOST));
	if ( texts) {
		fscale = texts->getFontScale();
		clr = texts->getFont()->getColor();
		clr.alpha() = 255 - clr.alpha(); // inverse the alpha, for FieldColor
	}

	fontScale =  new FieldReal(root,TR("Font Scale"),&fscale,RangeReal(0.01,10.));
	fcolor = new FieldColor(root, TR("Font Color"), &clr);
	if ( tbl.fValid()) {
		cb = new CheckBox(root,"Attribute Column",&useAttrib);
		fcolumns = new FieldColumn(cb, "", tbl, &colName, dmCLASS | dmIDENT | dmIMAGE | dmVALUE | dmSTRING); 
		fcolumns->Align(cb, AL_AFTER);
	}
	create();
}


void FeatureTextToolForm::apply(){
	fontScale->StoreData();
	fcolumns->StoreData();
	fcolor->StoreData();
	cb->StoreData();
	PreparationParameters pp(NewDrawer::ptRENDER);
	if (drw->isSet()) {
		for(int i = 0; i < drw->getDrawerCount(); ++i) {
			FeatureLayerDrawer *dr = (FeatureLayerDrawer *) (drw->getDrawer(i));
			if ( cb && useAttrib)
				dr->setLabelAttribute(colName);
			else
				dr->setLabelAttribute("");
			TextLayerDrawer *texts = static_cast<TextLayerDrawer *>(dr->getDrawer(223, ComplexDrawer::dtPOST));
			if ( texts) {
				texts->setFontScale(fscale);
				clr.alpha() = 255 - clr.alpha(); // inverse the alpha again, for displaying
				texts->getFont()->setColor(clr);
			}
			dr->prepare(&pp);
		}
	} else {
		FeatureLayerDrawer *dr = static_cast<FeatureLayerDrawer *>(drw);
		if ( cb && useAttrib)
			dr->setLabelAttribute(colName);
		else
			dr->setLabelAttribute("");
		TextLayerDrawer *texts = static_cast<TextLayerDrawer *>(dr->getDrawer(223, ComplexDrawer::dtPOST));
		if ( texts) {
			texts->setFontScale(fscale);
			clr.alpha() = 255 - clr.alpha(); // inverse the alpha again, for displaying
			texts->getFont()->setColor(clr);
		}
		dr->prepare(&pp);
	}
	updateMapView();
}

