#include "Client\Headers\formelementspch.h"
#include "Engine\Map\basemap.h"
#include "Engine\Map\Point\ilwPoint.h"
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
#include "Drawers\SetDrawer.h"
#include "Drawers\FeatureSetDrawer.h"
#include "Client\Editors\Utils\line.h"
#include "Drawers\LineSetDrawer.h"
#include "drawers\linedrawer.h"
#include "Drawers\FeatureLayerDrawer.h"
//#include "Client\Mapwindow\Drawers\PointMapDrawerForm.h"
#include "Headers\Hs\Drwforms.hs"

using namespace ILWIS;

ILWIS::NewDrawer *createLineSetDrawer(DrawerParameters *parms) {
	return new LineSetDrawer(parms);
}

LineSetDrawer::LineSetDrawer(DrawerParameters *parms) : 
	FeatureSetDrawer(parms,"LineSetDrawer"),
	linestyle(ldtSingle),
	linethickness(1),
	styleItem(0)
{
	setDrawMethod(drmSINGLE); // default;
}

LineSetDrawer::~LineSetDrawer() {
}

NewDrawer *LineSetDrawer::createElementDrawer(PreparationParameters *pp, ILWIS::DrawerParameters* parms) const{
	return IlwWinApp()->getDrawer("LineFeatureDrawer",pp, parms);

}

void LineSetDrawer::setDrawMethod(DrawMethod method) {
	if ( method == drmINIT) {
		if ( useInternalDomain() || !rpr.fValid())
			setDrawMethod(drmSINGLE);
		else 
			setDrawMethod(drmRPR);

	} else
		drm = method;
}

LineDspType LineSetDrawer::getLineStyle() const {
	return linestyle ;
}
int LineSetDrawer::getLineThickness() const {
	return linethickness;
}

String LineSetDrawer::store(const FileName& fnView, const String& parentSection) const{
	String currentSection = "LineSetDrawer::" + parentSection;
	FeatureSetDrawer::store(fnView, currentSection);
	ObjectInfo::WriteElement(currentSection.scVal(),"LineStyle",fnView, linestyle);
	ObjectInfo::WriteElement(currentSection.scVal(),"LineThickness",fnView, linethickness);

	return currentSection;
}

void LineSetDrawer::load(const FileName& fnView, const String& parentSection){
	String currentSection = parentSection;
	FeatureSetDrawer::load(fnView, currentSection);
	int ls;
	ObjectInfo::ReadElement(currentSection.scVal(),"LineStyle",fnView, ls);
	ls = (LineDspType)linestyle;
	ObjectInfo::ReadElement(currentSection.scVal(),"LineThickness",fnView, linethickness);

}

//--------------------------------- UI ----------------------------
void LineSetDrawer::prepare(PreparationParameters *parm){
	FeatureSetDrawer::prepare(parm);
	for(int i=0; i < drawers.size(); ++i) {
		LineDrawer *ld = (LineDrawer *)drawers.at(i);
		ld->setThickness(linethickness);
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

void LineSetDrawer::modifyLineStyleItem(LayerTreeView  *tv, bool remove) {
	if (!remove) {
		if ( styleItem == 0 ) {
		DisplayOptionTreeItem *item = new DisplayOptionTreeItem(tv,portrayalItem,this,(DisplayOptionItemFunc)&LineSetDrawer::displayOptionSetLineStyle);
		String linestyle("Line style");
		styleItem = InsertItem(linestyle,"LineStyle", item, -1);
		}
	} else {
		if ( styleItem) {
			tv->GetTreeCtrl().DeleteItem(styleItem);
			styleItem = 0;
		}
	}
}
HTREEITEM LineSetDrawer::configure(LayerTreeView  *tv, HTREEITEM parent) {
	HTREEITEM hti = FeatureSetDrawer::configure(tv,parent);
    FeatureLayerDrawer *fdr = dynamic_cast<FeatureLayerDrawer *>(getParentDrawer());
	BaseMap mp = fdr->getBaseMap();
	if ( IlwisObject::iotObjectType(mp->fnObj) == IlwisObject::iotPOLYGONMAP )
		setSingleColor(Color(0,0,0));
	if ( rpr.fValid() && !rpr->prc()) {
		modifyLineStyleItem(tv);
	}
	return hti;
}

void LineSetDrawer::displayOptionSetLineStyle(CWnd *parent) {
	new LineStyleForm(parent, this);
}

//-----------------------------------------------
LineStyleForm::LineStyleForm(CWnd *par, LineSetDrawer *ldr) 
	: DisplayOptionsForm(ldr, par, "Line Style")
{

  fi = new FieldReal(root, "Line thickness", &ldr->linethickness, ValueRange(1.0,100.0));
  flt = new FieldLineType(root, SDCUiLineType, &ldr->linestyle);

  create();
}

void  LineStyleForm::apply() {
	fi->StoreData();
	flt->StoreData();
	PreparationParameters pp(NewDrawer::ptRENDER);
	drw->prepare(&pp);
	updateMapView();
}
