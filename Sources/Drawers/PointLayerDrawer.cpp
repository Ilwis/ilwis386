#include "Headers\toolspch.h"
#include "Engine\Map\basemap.h"
#include "Engine\Map\Point\ilwPoint.h"
#include "Engine\Drawers\ComplexDrawer.h"
#include "Engine\Drawers\SimpleDrawer.h" 
#include "Engine\Domain\dmsort.h"
#include "Engine\Base\System\RegistrySettings.h"
#include "Engine\Drawers\RootDrawer.h"
#include "Engine\Drawers\SpatialDataDrawer.h"
#include "Drawers\LayerDrawer.h"
#include "Drawers\FeatureLayerDrawer.h"
#include "Drawers\PointLayerDrawer.h"
#include "Drawers\PointDrawer.h"
#include "Drawers\PointFeatureDrawer.h"
#include "Headers\Hs\Drwforms.hs"

using namespace ILWIS;

ILWIS::NewDrawer *createPointLayerDrawer(DrawerParameters *parms) {
	return new PointLayerDrawer(parms);
}

PointLayerDrawer::PointLayerDrawer(DrawerParameters *parms) : 
	FeatureLayerDrawer(parms,"PointLayerDrawer")
{
	properties = new PointProperties();
	properties->scale = 0;
}

PointLayerDrawer::~PointLayerDrawer() {
	delete properties;
}

NewDrawer *PointLayerDrawer::createElementDrawer(PreparationParameters *pp, ILWIS::DrawerParameters* parms) const{
	return NewDrawer::getDrawer("PointFeatureDrawer", pp,parms);

}

void PointLayerDrawer::prepare(PreparationParameters *parm){
	FeatureLayerDrawer::prepare(parm);
	SpatialDataDrawer *mapDrawer = (SpatialDataDrawer *)parentDrawer;
	Table tbl = mapDrawer->getAtttributeTable();
	DomainSort *dmsrt = 0;
	Column rotColumn;
	RangeReal rr;
	if ( tbl.fValid()) {
		if ( rotationInfo.rotationColumn != "")
			rotColumn = tbl->col(rotationInfo.rotationColumn);
		if ( rotColumn.fValid()) {
			if ( rotColumn->dm()->pdv() ==0)
				rotColumn = Column();
			else {
				dmsrt = tbl->dm()->pdsrt();
				rr = rotationInfo.rr.fValid() ? rotationInfo.rr : rotColumn->rrMinMax();
			}
		}

	}

	if ( (parm->type & NewDrawer::ptRENDER) != 0) {
		for(int i=0; i < drawers.size(); ++i) {
			PointFeatureDrawer *ld = (PointFeatureDrawer *)drawers.at(i);
			if ( !ld) 
				continue;
			PointProperties *props = (PointProperties *)ld->getProperties();
			if ( properties->scale == 0) {
					if ( drawers.size() < 25)
						properties->scale = 1.0;
					else if ( drawers.size() < 100)
						properties->scale = 0.6;
					else
						properties->scale = 0.3;
			}
			props->set(properties);
			if ( rotColumn.fValid() ) {
				Feature *feature = ld->getFeature();
				long iRaw = feature->iValue();
				long iKey = dmsrt->iKey(dmsrt->iOrd(iRaw));
				double v = rotColumn->rValue(iKey);
				if ( v != rUNDEF) {
					double f = rotationInfo.clockwise ? 1.0 : -1.0;
					double angle = f * 360.0 * ( v - rr.rLo())  / rr.rWidth();
					props->angle = angle;
				}
				
			}
			if (!properties->ignoreColor)
				props->drawColor = properties->drawColor;
			ld->prepare(parm);
		}
	} else if ( (parm->type & NewDrawer::pt3D) != 0) {
		prepareChildDrawers(parm);
	}
}

String PointLayerDrawer::store(const FileName& fnView, const String& parentSection) const{
	String currentSection = getType() + "::" + parentSection;
	FeatureLayerDrawer::store(fnView, currentSection);
	properties->store(fnView, currentSection);
	ObjectInfo::WriteElement(currentSection.c_str(),"RotationColumn",fnView, rotationInfo.rotationColumn);
	ObjectInfo::WriteElement(currentSection.c_str(),"RotationOrientation",fnView, rotationInfo.clockwise);
	ObjectInfo::WriteElement(currentSection.c_str(),"RotationRange",fnView, rotationInfo.rr);



	return currentSection;
}

void PointLayerDrawer::load(const FileName& fnView, const String& parentSection){
	String currentSection = getType() + "::" + parentSection;
	FeatureLayerDrawer::load(fnView, currentSection);
	properties->load(fnView, currentSection);
	ObjectInfo::ReadElement(currentSection.c_str(),"RotationColumn",fnView, rotationInfo.rotationColumn);
	ObjectInfo::ReadElement(currentSection.c_str(),"RotationOrientation",fnView, rotationInfo.clockwise);
	ObjectInfo::ReadElement(currentSection.c_str(),"RotationRange",fnView, rotationInfo.rr);

}

void PointLayerDrawer::setDrawMethod(DrawMethod method) {
	if ( method == drmINIT) { 
		if ( useInternalDomain() || !rpr.fValid()) {
			setDrawMethod(drmSINGLE);
		}
		else if ( rpr.fValid()) {
			setDrawMethod(drmRPR);
		}
	}
	else
		drm = method; 
}

void PointLayerDrawer::getDrawerFor(const Feature* feature,vector<NewDrawer *>& featureDrawers) {
	for(int i=0; i< getDrawerCount(); ++i) {
		PointFeatureDrawer *pfdrw = dynamic_cast<PointFeatureDrawer *>(getDrawer(i));
		if ( pfdrw) {
			if ( feature->getGuid() == pfdrw->getFeature()->getGuid())
				featureDrawers.push_back(pfdrw);
		}
	}
}

GeneralDrawerProperties *PointLayerDrawer::getProperties() {
	return properties;
}

SymbolRotationInfo PointLayerDrawer::getRotationInfo() const {
	return rotationInfo;
}

void PointLayerDrawer::setRotationInfo(const SymbolRotationInfo& sC) {
	rotationInfo = sC;
}



//-----------------------------------------------------------------





