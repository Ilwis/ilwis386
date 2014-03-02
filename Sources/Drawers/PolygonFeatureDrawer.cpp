#include "Headers\toolspch.h"
#include "Engine\Map\basemap.h"
#include "Engine\Map\Polygon\POL.h"
#include "Engine\Drawers\ComplexDrawer.h"
#include "Engine\Drawers\SimpleDrawer.h" 
#include "Engine\Base\System\RegistrySettings.h"
#include "Engine\Drawers\RootDrawer.h"
#include "Engine\Drawers\SpatialDataDrawer.h"
#include "Engine\Drawers\DrawerContext.h"
#include "Drawers\featuredatadrawer.h"
#include "Drawers\LayerDrawer.h"
#include "Drawers\LineDrawer.h"
#include "drawers\linedrawer.h"
#include "drawers\polygondrawer.h"
#include "Drawers\FeatureLayerDrawer.h"
#include "Drawers\PolygonLayerDrawer.h"
#include "geos\algorithm\CGAlgorithms.h"
#include "Drawers\DrawingColor.h" 
#include "Engine\Base\Algorithm\TriangulationAlg\gpc.h"
#include "Engine\Base\Algorithm\TriangulationAlg\Triangulator.h"
#include "Drawers\PolygonFeatureDrawer.h"
#include "Engine\Drawers\ZValueMaker.h"
#include "Engine\Drawers\SVGLoader.h"
#include "Engine\Drawers\SVGElements.h"
#include "Engine\Drawers\OpenGLText.h"
#include "Engine\Drawers\TextDrawer.h"
#include "Engine\Representation\Rprclass.h"
#include "Engine\Drawers\SVGPath.h"


using namespace ILWIS;

ILWIS::NewDrawer *createPolygonFeatureDrawer(DrawerParameters *parms) {
	if ( parms->managed) {
		if ( parms->parent && parms->parent->getType() == "PolygonFeatureDrawer") {
			PolygonFeatureDrawer *pfdr =  ((FeatureLayerDrawer *)parms->parent)->allocator<PolygonFeatureDrawer>()->allocate();
			pfdr->setDrawerParameters(parms);
			return pfdr;
		}
	}
	return new PolygonFeatureDrawer(parms);
}

PolygonFeatureDrawer::PolygonFeatureDrawer() : PolygonDrawer(0,"PolygonFeatureDrawer") {
}

PolygonFeatureDrawer::PolygonFeatureDrawer(DrawerParameters *parms) : PolygonDrawer(parms,"PolygonFeatureDrawer") {
	setDrawMethod(NewDrawer::drmRPR);
	PreparationParameters pp(NewDrawer::ptGEOMETRY | NewDrawer::ptRENDER , 0);
	boundary = (LineDrawer *)NewDrawer::getDrawer("LineFeatureDrawer", &pp, parms);
	tri = new MapPolygonTriangulator(static_cast<PolygonLayerDrawer *>(parentDrawer)->getTesselator());
}

PolygonFeatureDrawer::PolygonFeatureDrawer(DrawerParameters *parms, const String& name) : PolygonDrawer(parms,name) {
	setDrawMethod(NewDrawer::drmRPR);
	PreparationParameters pp(NewDrawer::ptGEOMETRY | NewDrawer::ptRENDER , 0);
	boundary = (LineDrawer *)NewDrawer::getDrawer("LineFeatureDrawer", &pp, parms);
	tri = new MapPolygonTriangulator(static_cast<PolygonLayerDrawer *>(parentDrawer)->getTesselator());
}

PolygonFeatureDrawer::~PolygonFeatureDrawer() {
	delete tri;
}

bool PolygonFeatureDrawer::draw(const DrawLoop drawLoop, const CoordBounds& cbArea) const {
	if ( label) {
		Coord c = feature->centroid();
		PolygonLayerDrawer *polygonLayer = static_cast<PolygonLayerDrawer *>(parentDrawer);
		BaseMapPtr *bmpptr = ((SpatialDataDrawer *)polygonLayer->getParentDrawer())->getBaseMap();
	    CoordSystem csy = polygonLayer->getCoordSystem();
		c = getRootDrawer()->glConv(csy,c);
		CoordBounds cb = label->getTextExtent();
		c.x = c.x - cb.width() / 2;
		label->setCoord(c);
	}
	return PolygonDrawer::draw(drawLoop, cbArea);
}

void PolygonFeatureDrawer::addDataSource(void *f, int options) {
	feature = (Feature *)f;
	if ( boundary){
		boundary->addDataSource(feature);
		cb = feature->cbBounds();
	}
}

Feature *PolygonFeatureDrawer::getFeature() const {
	return feature;
}

void* PolygonFeatureDrawer::getDataSource() const{
	return (void *) feature;
}


void PolygonFeatureDrawer::prepare(PreparationParameters *p){
	PolygonDrawer::prepare(p);
	PolygonLayerDrawer *polygonLayer = dynamic_cast<PolygonLayerDrawer *>(parentDrawer);
	if (  p->type & ptGEOMETRY || p->type & ptRESTORE) {
		CoordSystem csy = polygonLayer->getCoordSystem();
		if ( boundary) {
			boundary->prepare(p);
		}
		ILWIS::Polygon *polygon = (ILWIS::Polygon *)feature;
		if ( !polygon)
			return;
		cb = polygon->cbBounds();
		Coord c1 = getRootDrawer()->glConv(csy,cb.cMin);
		Coord c2 = getRootDrawer()->glConv(csy,cb.cMax);
		cb = CoordBounds(c1,c2);
		cb.getArea(); // initializes the area
		bool coordNeedsConversion = getRootDrawer()->fConvNeeded(csy);
		long *data;
		long *count;
		bool firstTime = polygonLayer->getTriangleData(&data, &count);
		if (data) { // triangulation file exists
			if ( firstTime) { // file has not been read yet
				tri->getTriangulation(data, count, triangleStrips);
				if (coordNeedsConversion) // triangulation data is in original projection; reproject to map window
					for (vector<pair<unsigned int, vector<Coord>>>::iterator & strip = triangleStrips.begin(); strip != triangleStrips.end(); ++strip)
						strip->second = rootDrawer->glConv(csy, strip->second);
			}
		} else {
			tri->getTriangulation(polygon, triangleStrips); // generate new
			if (coordNeedsConversion) // triangulation data is in original projection; reproject to map window
				for (vector<pair<unsigned int, vector<Coord>>>::iterator & strip = triangleStrips.begin(); strip != triangleStrips.end(); ++strip)
					strip->second = rootDrawer->glConv(csy, strip->second);
		}
	}
	if ( p->type & NewDrawer::pt3D || p->type & ptRESTORE) {
		ZValueMaker *zmaker = ((ComplexDrawer *)parentDrawer)->getZMaker();
		for(int i = 0; i < triangleStrips.size(); ++i) {
			for(int j = 0; j < triangleStrips[i].second.size(); ++j) {
				Coord &c = triangleStrips[i].second[j];
				double zv = zmaker->getValue(c,feature);
				c.z = zv;
			}
		}
		if ( boundary) {
			boundary->prepare(p);
			boundary->setSupportingDrawer(true);

		}
	}
	if (  p->type & ptRESTORE || p->type & RootDrawer::ptRENDER) {
		BaseMapPtr *bmpptr = ((SpatialDataDrawer *)polygonLayer->getParentDrawer())->getBaseMap();
		extrTransparency = polygonLayer->getExtrusionTransparency();
		drawColor = polygonLayer->getDrawingColor()->clrRaw(feature->iValue(), polygonLayer->getDrawMethod());
		if ( boundary) {
			LineProperties *lp = (LineProperties *)polygonLayer->getProperties();
			boundariesActive(polygonLayer->getShowBoundaries());
			areasActive(polygonLayer->getShowAreas());
			double rprTrans =  p->props.itemTransparency;
			setTransparencyArea(polygonLayer->getTransparencyArea() * rprTrans);
			((LineProperties *)boundary->getProperties())->linestyle = lp->linestyle;
			((LineProperties *)boundary->getProperties())->thickness = lp->thickness;
			double fvalue =  getFeature()->rValue();
			Representation rpr = polygonLayer->getRepresentation();
			if ( rpr.fValid() && rpr->dm()->dmt() == dmtCLASS) {
				String hatchName = rpr->prc()->sHatch(fvalue);
				if ( hatchName != sUNDEF) {
					const SVGLoader *loader = NewDrawer::getSvgLoader();
					SVGLoader::const_iterator cur = loader->find(p->props.hatchName);
					if ( cur == loader->end() || (*cur).second->getType() == IVGElement::ivgPOINT)
						return;

					hatch = (*cur).second->getHatch();
					hatchInverse = (*cur).second->getHatchInverse();

					backgroundColor = rpr->prc()->clrSecondRaw(fvalue);
					long transparent = Color(-2); // in the old days this was the transparent value
					if (backgroundColor.iVal() == transparent) 
						backgroundColor = colorUNDEF;
				}
			}

			for(int j =0 ; j < p->filteredRaws.size(); ++j) {
				int raw = p->filteredRaws[j];
				if ( fvalue == abs(raw)) {
					setActive(raw > 0);
				}
			}
			boundary->prepare(p);
			boundary->setSupportingDrawer(true);

			((LineProperties *)boundary->getProperties())->drawColor = lp->drawColor;
		}
		if ( bmpptr->fTblAtt()) {
			setTableSelection(bmpptr->tblAtt()->dm()->fnObj,feature->iValue(), p);
		}
	}
}

long PolygonFeatureDrawer::writeTriangleData(ofstream& file) {
	return tri->writeTriangleData(file);
}


