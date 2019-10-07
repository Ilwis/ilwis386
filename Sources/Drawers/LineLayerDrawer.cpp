#include "Headers\toolspch.h"
#include "Engine\Map\basemap.h"
#include "Engine\Base\System\Engine.h"
#include "Engine\Base\System\LOGGER.H"
#include "Engine\Map\Point\ilwPoint.h"
#include "Engine\Drawers\ComplexDrawer.h"
#include "Engine\Drawers\SimpleDrawer.h"
#include "drawers\linedrawer.h"
#include "Engine\Base\System\RegistrySettings.h"
#include "Engine\Drawers\RootDrawer.h"
#include "Engine\Drawers\SpatialDataDrawer.h"
#include "Drawers\LayerDrawer.h"
#include "Drawers\FeatureDataDrawer.h"
#include "Drawers\FeatureLayerDrawer.h"
#include "Drawers\LineLayerDrawer.h"
#include "drawers\linefeaturedrawer.h"
#include "Headers\Hs\Drwforms.hs"

using namespace ILWIS;

ILWIS::NewDrawer *createLineLayerDrawer(DrawerParameters *parms) {
	return new LineLayerDrawer(parms);
}

LineLayerDrawer::LineLayerDrawer(DrawerParameters *parms) : 
	FeatureLayerDrawer(parms,"LineLayerDrawer")
{
	setDrawMethod(drmSINGLE); // default;
	lproperties.ignoreColor = true; // color is determined by rpr, not by form
}

LineLayerDrawer::~LineLayerDrawer() {
	delete (MemoryManager<LineFeatureDrawer>*)managedDrawers;
}

NewDrawer *LineLayerDrawer::createElementDrawer(PreparationParameters *pp, ILWIS::DrawerParameters* parms) const{
	return NewDrawer::getDrawer("LineFeatureDrawer",pp, parms);

}

void LineLayerDrawer::setDrawMethod(DrawMethod method) {
	if ( method == drmINIT || method == drmNOTSET) {
		if (useInternalDomain())
			setDrawMethod(drmSINGLE);
		else if (rpr.fValid())
			setDrawMethod(drmRPR);
		else {
			BaseMapPtr *bmptr = ((BaseMap*)getDataSource())->ptr();
			if (bmptr->dm()->pdbool())
				setDrawMethod(drmBOOL);
			else
				setDrawMethod(drmSINGLE);
		}
	} else
		drm = method;
}

String LineLayerDrawer::store(const FileName& fnView, const String& section) const{
	String currentSection = section + ":LineLayer";
	FeatureLayerDrawer::store(fnView, currentSection);
	lproperties.store(fnView, currentSection);

	return currentSection;
}

void LineLayerDrawer::load(const FileName& fnView, const String& section){
	String currentSection = section;
	FeatureLayerDrawer::load(fnView, currentSection);
	lproperties.load(fnView, currentSection);
}

void LineLayerDrawer::getDrawerFor(const Feature* feature,vector<NewDrawer *>& featureDrawers) {
	for(int i=0; i< getDrawerCount(); ++i) {
		LineFeatureDrawer *pfdrw = dynamic_cast<LineFeatureDrawer *>(getDrawer(i));
		if ( pfdrw) {
			if ( feature->getGuid() == pfdrw->getFeature()->getGuid())
				featureDrawers.push_back(pfdrw);
		}
	}
}

GeneralDrawerProperties *LineLayerDrawer::getProperties() {
	return &lproperties;
}

void LineLayerDrawer::prepare(PreparationParameters *parm){
	FeatureLayerDrawer::prepare(parm);
	if ( (parm->type & NewDrawer::ptRENDER) != 0) {
		for(int i=0; i < drawers.size(); ++i) {
			LineDrawer *ld = (LineDrawer *)drawers.at(i);
			if ( ld) {
				bool setBoxes = (specialOptions & NewDrawer::sdoSymbolLineNode) != 0;
				ld->setSpecialDrawingOptions(NewDrawer::sdoSymbolLineNode, setBoxes );
				if ( !useRpr) {
					LineProperties *props = (LineProperties *)ld->getProperties();
					props->linestyle = lproperties.linestyle;
					if (!lproperties.ignoreColor)
						props->drawColor = lproperties.drawColor;
				}
			} else {
				getEngine()->getLogger()->LogLine(TR("Empty drawer in list"), Logger::lmERROR);
			}
		}
	} else if ( (parm->type & NewDrawer::pt3D) != 0) {
		prepareChildDrawers(parm);
	}
}






