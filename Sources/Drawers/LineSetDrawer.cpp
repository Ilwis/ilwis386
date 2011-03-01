#include "Client\Headers\formelementspch.h"
#include "Engine\Map\basemap.h"
#include "Engine\Map\Point\ilwPoint.h"
#include "Client\Mapwindow\Drawers\ComplexDrawer.h"
#include "Client\Mapwindow\Drawers\SimpleDrawer.h"
#include "drawers\linedrawer.h"
#include "Client\Ilwis.h"
#include "Engine\Base\System\RegistrySettings.h"
#include "Client\Mapwindow\MapCompositionDoc.h"
#include "Client\Mapwindow\Drawers\RootDrawer.h"
#include "Client\Mapwindow\Drawers\AbstractMapDrawer.h"
#include "Client\Mapwindow\MapPaneViewTool.h"
#include "Client\Mapwindow\Drawers\DrawerTool.h"
#include "Client\Mapwindow\LayerTreeView.h"
#include "Client\Mapwindow\LayerTreeItem.h" 
#include "Drawers\SetDrawer.h"
#include "Drawers\FeatureSetDrawer.h"
#include "Client\Editors\Utils\line.h"
#include "Drawers\LineSetDrawer.h"
#include "Drawers\FeatureLayerDrawer.h"
#include "drawers\linefeaturedrawer.h"
#include "Headers\Hs\Drwforms.hs"

using namespace ILWIS;

ILWIS::NewDrawer *createLineSetDrawer(DrawerParameters *parms) {
	return new LineSetDrawer(parms);
}

LineSetDrawer::LineSetDrawer(DrawerParameters *parms) : 
	FeatureSetDrawer(parms,"LineSetDrawer")
{
	setDrawMethod(drmSINGLE); // default;
	lproperties.ignoreColor = true; // color is determined by rpr, not by form
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

String LineSetDrawer::store(const FileName& fnView, const String& parentSection) const{
	String currentSection = "LineSetDrawer::" + parentSection;
	FeatureSetDrawer::store(fnView, currentSection);
	ObjectInfo::WriteElement(currentSection.scVal(),"LineStyle",fnView, lproperties.linestyle);
	ObjectInfo::WriteElement(currentSection.scVal(),"LineThickness",fnView, lproperties.thickness);

	return currentSection;
}

void LineSetDrawer::load(const FileName& fnView, const String& parentSection){
	String currentSection = parentSection;
	FeatureSetDrawer::load(fnView, currentSection);
	ObjectInfo::ReadElement(currentSection.scVal(),"LineStyle",fnView, lproperties.linestyle);
	ObjectInfo::ReadElement(currentSection.scVal(),"LineThickness",fnView, lproperties.thickness);

}

void LineSetDrawer::getDrawerFor(const Feature* feature,vector<NewDrawer *>& featureDrawers) {
	for(int i=0; i< getDrawerCount(); ++i) {
		LineFeatureDrawer *pfdrw = dynamic_cast<LineFeatureDrawer *>(getDrawer(i));
		if ( pfdrw) {
			if ( feature->getGuid() == pfdrw->getFeature()->getGuid())
				featureDrawers.push_back(pfdrw);
		}
	}
}

GeneralDrawerProperties *LineSetDrawer::getProperties() {
	return &lproperties;
}

void LineSetDrawer::prepare(PreparationParameters *parm){
	FeatureSetDrawer::prepare(parm);
	if ( parm->type == NewDrawer::ptRENDER) {
		for(int i=0; i < drawers.size(); ++i) {
			LineDrawer *ld = (LineDrawer *)drawers.at(i);
			LineProperties *props = (LineProperties *)ld->getProperties();
			props->linestyle = lproperties.linestyle;
			props->thickness = lproperties.thickness;
			if (!lproperties.ignoreColor)
				props->drawColor = lproperties.drawColor;
		}
	}
}






