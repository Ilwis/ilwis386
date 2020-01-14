#include "Headers\toolspch.h"
#include "Engine\Map\basemap.h"
#include "Engine\Map\Point\pnt.h"
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
#include "Engine\Drawers\OpenGLText.h"
#include "Engine\Drawers\TextDrawer.h"
#include "Headers\Hs\Drwforms.hs"

using namespace ILWIS;

ILWIS::NewDrawer *createPointLayerDrawer(DrawerParameters *parms) {
	return new PointLayerDrawer(parms);
}

PointLayerDrawer::PointLayerDrawer(DrawerParameters *parms) : 
	FeatureLayerDrawer(parms,"PointLayerDrawer")
{
	properties = new PointProperties();
	IlwisSettings settings("DefaultSettings");
	Color color = settings.clrValue("SingleColorPoints", Color()); // do not compare with colorUNDEF, because then we can't assign the black color; the registry has the inverse alpha
	if (color != Color()) {
		singleColor = color;
		singleColor.alpha() = 255 - singleColor.alpha();
	}
}

PointLayerDrawer::~PointLayerDrawer() {
	delete (MemoryManager<PointFeatureDrawer>*)managedDrawers;
	delete properties;
}

NewDrawer *PointLayerDrawer::createElementDrawer(PreparationParameters *pp, ILWIS::DrawerParameters* parms) const{
	NewDrawer *drw =  NewDrawer::getDrawer("PointFeatureDrawer", pp,parms);
	drw->setGeneralProperties((GeneralDrawerProperties *)properties);

	return drw;
}

bool PointLayerDrawer::draw(const DrawLoop drawLoop, const CoordBounds& cbArea) const {
	FeatureLayerDrawer::draw(drawLoop, cbArea);
	getRootDrawer()->setZIndex(1 + getRootDrawer()->getZIndex()); // extra offset because of the contours in the point symbols (TODO: not all symbols may need this because no area is drawn; for complicated symbols the zIndex may need to be increased by more than 1);
	return true;
}

void PointLayerDrawer::prepare(PreparationParameters *parm){
	FeatureLayerDrawer::prepare(parm);
	SpatialDataDrawer *mapDrawer = (SpatialDataDrawer *)parentDrawer;
	BaseMapPtr *bmptr = ((BaseMap*)getDataSource())->ptr();
	Table tbl = bmptr->tblAtt();
	DomainSort *dmsrt = 0;
	Column rotColumn;
	RangeReal rr;
	if ( (parm->type & NewDrawer::ptGEOMETRY) != 0){
		properties->scale = 0; // force default sizing of points
	}
	if ( tbl.fValid() || bmptr->dm()->pdv()) {
		if ( rotationInfo.rotationColumn != "")
			rotColumn = tbl->col(rotationInfo.rotationColumn);
		if ( rotColumn.fValid()) {
			if ( rotColumn->dm()->pdv() ==0)
				rotColumn = Column();
			else {
				dmsrt = tbl->dm()->pdsrt();
				rr = rotationInfo.rr.fValid() ? rotationInfo.rr : rotColumn->rrMinMax();
			}
		} else
			rr = rotationInfo.rr;
	}

	if (parm->type & NewDrawer::ptRENDER || parm->type & NewDrawer::ptRESTORE) {
		for(int i=0; i < drawers.size(); ++i) {
			PointFeatureDrawer *ld = (PointFeatureDrawer *)drawers.at(i);
			if ( !ld) 
				continue;
			PointProperties *props = (PointProperties *)ld->getProperties();
			if ( properties->scale == 0) {
					if ( drawers.size() < 25)
						properties->scale = 100;
					else if ( drawers.size() < 100)
						properties->scale = 60;
					else
						properties->scale = 40;
			}
			props->set(properties);

			if ( !props->stretchRange.fValid()) {
				if ( tbl.fValid() && properties->stretchColumn != "") {
					Column col = tbl->col(properties->stretchColumn);
					if ( col.fValid())
						props->stretchRange = properties->stretchRange = col->rrMinMax();
				} else {
					props->stretchRange = properties->stretchRange = mapDrawer->getStretchRangeReal();
				}
			}
			if ( rotationInfo.rr.fValid() ) {
				Feature *feature = ld->getFeature();
				long iRaw = feature->iValue();
				double v;
				if ( rotColumn.fValid()) {
					long iKey = dmsrt->iKey(dmsrt->iOrd(iRaw));
					v = rotColumn->rValue(iKey);
				} else
					v =  bmptr->dvrs().rValue(iRaw);

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

String PointLayerDrawer::store(const FileName& fnView, const String& section) const{
	String currentSection = section + ":PointLayer";
	FeatureLayerDrawer::store(fnView, currentSection);
	properties->store(fnView, currentSection);
	ObjectInfo::WriteElement(currentSection.c_str(),"RotationColumn",fnView, rotationInfo.rotationColumn);
	ObjectInfo::WriteElement(currentSection.c_str(),"RotationOrientation",fnView, rotationInfo.clockwise);
	ObjectInfo::WriteElement(currentSection.c_str(),"RotationRange",fnView, rotationInfo.rr);

	return currentSection;
}

void PointLayerDrawer::load(const FileName& fnView, const String& section){
	String currentSection = section;
	FeatureLayerDrawer::load(fnView, currentSection);
	properties->load(fnView, currentSection);
	ObjectInfo::ReadElement(currentSection.c_str(),"RotationColumn",fnView, rotationInfo.rotationColumn);
	ObjectInfo::ReadElement(currentSection.c_str(),"RotationOrientation",fnView, rotationInfo.clockwise);
	ObjectInfo::ReadElement(currentSection.c_str(),"RotationRange",fnView, rotationInfo.rr);

}

void PointLayerDrawer::setDrawMethod(DrawMethod method) {
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
	properties->useDirection = sC.rr.fValid();
}



//-----------------------------------------------------------------





