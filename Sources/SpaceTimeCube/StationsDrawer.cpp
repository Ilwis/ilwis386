#include "Headers\toolspch.h"
#include "Engine\Map\basemap.h"
#include "Engine\Map\Point\ilwPoint.h"
#include "Engine\Drawers\ComplexDrawer.h"
#include "Engine\Drawers\SimpleDrawer.h" 
#include "Engine\Base\System\RegistrySettings.h"
#include "Engine\Drawers\RootDrawer.h"
#include "Engine\Drawers\SpatialDataDrawer.h"
#include "Drawers\LayerDrawer.h"
#include "Drawers\featuredatadrawer.h"
#include "Drawers\FeatureLayerDrawer.h"
#include "Drawers\DrawingColor.h" 
#include "StationsDrawer.h"
#include "Drawers\PointDrawer.h"
#include "Drawers\PointFeatureDrawer.h"
#include "Engine\Representation\Rprclass.h"

using namespace ILWIS;

ILWIS::NewDrawer *createStationsDrawer(DrawerParameters *parms) {
	return new StationsDrawer(parms);
}

StationsDrawer::StationsDrawer(DrawerParameters *parms)
: SpaceTimeDrawer(parms,"StationsDrawer")
{
}

StationsDrawer::~StationsDrawer() {
}

void StationsDrawer::drawObjects(const int steps, GetHatchFunc getHatchFunc) const
{
	Tranquilizer trq(TR("computing triangles"));
	Column attributeColumn;
	bool fUseAttributeColumn = useAttributeColumn();
	if (fUseAttributeColumn) {
		attributeColumn = getAtttributeColumn();
		if (!attributeColumn.fValid())
			fUseAttributeColumn = false;
	}
	RangeReal rrMinMax = getValueRange(attributeColumn);
	double width = rrMinMax.rWidth();
	double minMapVal = rrMinMax.rLo();
	long numberOfFeatures = basemap->iFeatures();
	double cubeBottom = 0;
	double cubeTop = timeBounds->tMax() - timeBounds->tMin();
	*fHatching = false; // in case of a classmap, if any of the attributes uses hatching, we set fHatching to true
	double delta = cube.altitude() / 200;
	if (steps == 1)
	{
		glBegin(GL_LINES);
		for(long i = 0; i < numberOfFeatures; ++i) {
			Feature *feature = CFEATURE(basemap->getFeature(i));
			if ( feature && feature->fValid() && feature->rValue() != rUNDEF) {
				ILWIS::Point *point = (ILWIS::Point *)feature;
				double z = getTimeValue(feature);
				if (z >= cubeBottom && z <= cubeTop) {
					glBegin(GL_LINE_STRIP);
					Coord crd = *(point->getCoordinate());
					crd.z = z * cube.altitude() / (timeBounds->tMax() - timeBounds->tMin());
					crd = getRootDrawer()->glConv(csy, crd);
					if (fUseAttributeColumn)
						glTexCoord2f(((fValueMap ? attributeColumn->rValue(feature->iValue()) : attributeColumn->iRaw(feature->iValue())) - minMapVal) / width, 0.25f); // 0.25 instead of 0.5, so that no interpolation is needed in Y-direction (the value is taken from the center of the first row)
					else
						glTexCoord2f((feature->rValue() - minMapVal) / width, 0.25f); // 0.25 instead of 0.5, so that no interpolation is needed in Y-direction (the value is taken from the center of the first row)
					glVertex3f(crd.x, crd.y, crd.z - delta);
					glVertex3f(crd.x, crd.y, crd.z + delta);
				}
			}
			if ( i % 100 == 0)
				trq.fUpdate(i, numberOfFeatures); 
		}
		glEnd();
	}
	else
	{
		const double angleStep = 2.0 * M_PI / steps;
		const CoordBounds& cbMap = getRootDrawer()->getMapCoordBounds();
		double pathScale = cbMap.width() / 50;
		if (numberOfFeatures > 1) {
			const byte * hatch = 0;
			RepresentationClass * prc = 0;
			if (fUseAttributeColumn) {
				Representation rpr = attributeColumn->dm()->rpr();
				if ( rpr.fValid() && rpr->dm()->pdc())
					prc = rpr->prc();
			}
			for (long i = 0; i < numberOfFeatures; ++i)
			{
				Feature *feature = CFEATURE(basemap->getFeature(i));
				if ( feature && feature->fValid() && feature->rValue() != rUNDEF) {
					double z = getTimeValue(feature);
					if (z >= cubeBottom && z <= cubeTop)
					{
						ILWIS::Point *point = (ILWIS::Point *)feature;
						Coord pnt = *(point->getCoordinate());
						pnt = getRootDrawer()->glConv(csy, pnt);
						pnt.z = z * cube.altitude() / (timeBounds->tMax() - timeBounds->tMin());
						float rsPnt = fUseAttributeColumn ? (((fValueMap ? attributeColumn->rValue(feature->iValue()) : attributeColumn->iRaw(feature->iValue())) - minMapVal) / width) : ((feature->rValue() - minMapVal) / width);
						double rPnt = pathScale * getSizeValue(feature);

						if (prc) {
							hatch = 0;
							(this->*getHatchFunc)(prc, attributeColumn->iRaw(feature->iValue()), hatch);
						}
						if (hatch) {
							*fHatching = true;
							glEnable(GL_POLYGON_STIPPLE);
							glPolygonStipple(hatch);
						} else
							glDisable(GL_POLYGON_STIPPLE);

						// cylinderCoords
						glBegin(GL_TRIANGLE_STRIP);
						double f = 0;
						for (int step = 0; step < (1 + steps); ++step)
						{
							if (step == steps)
								f = 0; // close the cylinder, choose exactly the same angle as the fist time
							glTexCoord2f(rsPnt, 0.25f); // 0.25 instead of 0.5, so that no interpolation is needed in Y-direction (the value is taken from the center of the first row)
							Coord normCircle = Coord(rPnt * cos(f), rPnt * sin(f), 0);
							Coord normal = normalize(normCircle);
							glNormal3f(normal.x, normal.y, normal.z);
							glVertex3f(pnt.x + normCircle.x, pnt.y + normCircle.y, pnt.z - delta);
							glVertex3f(pnt.x + normCircle.x, pnt.y + normCircle.y, pnt.z + delta);
							f += angleStep;
						}

						glEnd();
					}
				}
				if ( i % 100 == 0)
					trq.fUpdate(i, numberOfFeatures); 
			}
			if ( hatch)
				glDisable(GL_POLYGON_STIPPLE);
		}
	}
}


//-----------------------------------------------------------------





