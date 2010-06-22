#include "Client\Headers\formelementspch.h"
#include "Drawer_n.h"
#include "Engine\Map\Feature.h"
#include "Engine\Map\Basemap.h"
#include "Engine\Map\Raster\Map.h"
#include "Engine\Base\System\RegistrySettings.h"
#include "Client\Mapwindow\MapCompositionDoc.h"
#include "Client\Mapwindow\Drawers\RootDrawer.h"
#include "Client\Mapwindow\Drawers\AbstractObjectdrawer.h"
#include "Client\Mapwindow\Drawers\AbstractMapDrawer.h"
#include "Client\Mapwindow\Drawers\DrawingColor.h" 
#include "Client\Mapwindow\Drawers\featurelayerdrawer.h"
#include "Client\Mapwindow\Drawers\FeatureDrawer.h"

using namespace ILWIS;

FeatureDrawer::FeatureDrawer(DrawerParameters *parms, const String& ty) : AbstractDrawer(parms, ty){
	color1 = Color(0,176,20);
	color2 = Color(0,0,0);
	//IlwisSettings settings("DefaultSettings");
	//clr0 = settings.clrValue("NoColor", clr0);
	//clr = settings.clrValue("YesColor", clr);
}

FeatureDrawer::~FeatureDrawer() {
}

void FeatureDrawer::setDataSource(void *p){
	feature = (Feature *)p;
}


void FeatureDrawer::prepare(PreparationParameters *p) {
	if (  p->type == ptALL ||  p->type == ptRENDER) {
		AbstractMapDrawer *amd = (AbstractMapDrawer *)p->parentDrawer;
		color1 = amd->drwColor()->clrRaw(feature->iValue());

	}
}
//void FeatureDrawer::setColor(const BaseMap& basemap, ILWIS::FeatureLayerDrawer *fdr) {
//	Domain dm = basemap->dm();
//	DomainValue *pdv = dm->pdv();
//	Representation rpr = fdr->getRepresentation();
//	NewDrawer::DrawMethod drm = fdr->getDrawMethod();
//	double rVal = rUNDEF;
//	int iRaw = iUNDEF;
//	if (!rpr.fValid())
//		drm = drmSINGLE;
//	if (basemap->dm()->pdvi() || basemap->dm()->pdvr()) {
//		rVal = feature->rValue();
//		iRaw = longConv(rVal); // for multiple colors
//		if (rVal == rUNDEF)
//			return;
//	}
//	else {
//		iRaw = feature->iValue();
//		if (iRaw == iUNDEF) return;
//		if (fdr->useAttributeTable()) {
//			Column colAtt = fdr->getAtttributeColumn();
//			if (colAtt->dm()->pdvi() || colAtt->dm()->pdvr()) {
//				rVal = colAtt->rValue(iRaw);
//				iRaw = longConv(rVal); // for multiple colors
//				if (rVal == rUNDEF) return;
//			}
//			else {
//				iRaw = colAtt->iRaw(iRaw);
//				if (iRaw == iUNDEF) return;
//			}
//		}
//	}
//	switch(drm) {
//		case NewDrawer::drmSINGLE:
//			color1 = fdr->getSingleColor();
//			break;
//		case NewDrawer::drmMULTIPLE: 
//			color1 = clrRaw(iRaw);
//			break;
//		case NewDrawer::drmRPR:
//			if (basemap->dm()->pdv()) {
//				if (fdr->isStretched())
//					if ( fdr->getStretchRangeReal().fValid())
//						color1 = rpr->clr(rVal,fdr->getStretchRangeReal());
//					else{
//						//lineColor = rpr->clr(rVal,fdr->getStretchRangeInt());
//					}
//				else  
//					color1 = rpr->clr(rVal);
//			}   
//			else 
//				color1 = rpr->clrRaw(iRaw);
//
//			break;
//
//	}
//}

