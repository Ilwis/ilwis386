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
: SpaceTimeDrawer(parms,"PointLayerDrawer")
, textureOffset(1.0/((double)PALETTE_TEXTURE_SIZE * 2.0))
, textureRange((double)PALETTE_TEXTURE_SIZE / ((double)PALETTE_TEXTURE_SIZE + 1.0))
{
}

StationsDrawer::~StationsDrawer() {
}

String StationsDrawer::store(const FileName& fnView, const String& parentSection) const
{
	String currentSection = parentSection + "::" + getType();
	SpaceTimeDrawer::store(fnView, currentSection);
	return currentSection;
}

void StationsDrawer::load(const FileName& fnView, const String& currentSection)
{
	SpaceTimeDrawer::load(fnView, currentSection);
}

vector<GLuint> StationsDrawer::getObjectIDs(vector<long> & iRaws) const
{
	vector<GLuint> objectIDs;
	long numberOfFeatures = basemap->iFeatures();
	for (GLuint objectID = 0; objectID < numberOfFeatures; ++objectID) {
		Feature * feature = CFEATURE(basemap->getFeature(objectID));
		if (feature != 0) {
			if (find(iRaws.begin(), iRaws.end(), feature->iValue()) != iRaws.end())
				objectIDs.push_back(objectID);
		}
	}
	return objectIDs;
}

int StationsDrawer::getNearestEnabledObjectIDIndex(vector<GLuint> & objectIDs) const
{
	int i = 0; // objectIDs are already sorted from near to far, so we only need to check for disabled iRaws
	if (useAttColumn && getAtttributeColumn().fValid()) {
		while (i < objectIDs.size()) {
			GLuint objectID = objectIDs[i];
			Feature * feature = CFEATURE(basemap->getFeature(objectID));
			if (feature != 0) {
				long raw = feature->iValue();
				if (raw != iUNDEF) {
					raw = getAtttributeColumn()->iRaw(raw);
					if (find(disabledRaws.begin(), disabledRaws.end(), raw) == disabledRaws.end())
						break;
				}
			}
			++i;
		}
	}

	return i;
}

vector<GLuint> StationsDrawer::getEnabledObjectIDs(vector<GLuint> & objectIDs) const
{
	if (useAttColumn && getAtttributeColumn().fValid()) {
		vector<GLuint> newObjectIDs;
		for (int i = 0; i < objectIDs.size(); ++i) {
			GLuint objectID = objectIDs[i];
			Feature * feature = CFEATURE(basemap->getFeature(objectID));
			if (feature != 0) {
				long raw = feature->iValue();
				if (raw != iUNDEF) {
					raw = getAtttributeColumn()->iRaw(raw);
					if (find(disabledRaws.begin(), disabledRaws.end(), raw) == disabledRaws.end())
						continue;
				}
			}
			newObjectIDs.push_back(objectID);
		}
		return newObjectIDs;
	} else
		return objectIDs;
}

Feature * StationsDrawer::getFeature(GLuint objectID) const
{
	return CFEATURE(basemap->getFeature(objectID));
}

void StationsDrawer::getRaws(GLuint objectID, vector<long> & raws) const
{
	// construct raws array (for stations the array has just one element)
	Feature * feature = CFEATURE(basemap->getFeature(objectID));
	if (feature != 0)
		raws.push_back(feature->iValue());
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
	double cubeBottom = -0.0000000005; // inaccuracy of time
	double cubeTop = 0.000000001 + timeBounds->tMax() - timeBounds->tMin();
	*fHatching = false; // in case of a classmap, if any of the attributes uses hatching, we set fHatching to true
	double delta = cube.altitude() / 200;
	glInitNames();
	glPushName(0);
	if (steps == 1)
	{
		for(long i = 0; i < numberOfFeatures; ++i) {
			Feature *feature = CFEATURE(basemap->getFeature(i));
			if ( feature && feature->fValid() && feature->rValue() != rUNDEF) {
				ILWIS::Point *point = (ILWIS::Point *)feature;
				double z = getTimeValue(feature);
				if (z >= cubeBottom && z <= cubeTop) {
					glLoadName(i);
					map<long, GLuint>::iterator mapEntry = subDisplayLists->find(i);
					if (mapEntry == subDisplayLists->end()) {
						GLuint listID = glGenLists(1); // not compiled in the display list, but executed immediately
						(*subDisplayLists)[i] = listID;
					}
					glCallList((*subDisplayLists)[i]);
					glBegin(GL_LINE_STRIP);
					Coord crd = *(point->getCoordinate());
					crd.z = z * cube.altitude() / (timeBounds->tMax() - timeBounds->tMin());
					crd = getRootDrawer()->glConv(csy, crd);
					if (fUseAttributeColumn)
						glTexCoord2f(textureOffset + textureRange * ((fValueMap ? attributeColumn->rValue(feature->iValue()) : attributeColumn->iRaw(feature->iValue())) - minMapVal) / width, 0.25f); // 0.25 instead of 0.5, so that no interpolation is needed in Y-direction (the value is taken from the center of the first row)
					else
						glTexCoord2f(textureOffset + textureRange * (feature->rValue() - minMapVal) / width, 0.25f); // 0.25 instead of 0.5, so that no interpolation is needed in Y-direction (the value is taken from the center of the first row)
					if (fTimeAttribute2) {
						glVertex3f(crd.x, crd.y, crd.z);
						double z2 = getTimeValue2(feature) * cube.altitude() / (timeBounds->tMax() - timeBounds->tMin());
						glVertex3f(crd.x, crd.y, z2);
					} else {
						glVertex3f(crd.x, crd.y, crd.z - delta);
						glVertex3f(crd.x, crd.y, crd.z + delta);
					}
					glEnd();
				}
			}
			if ( i % 100 == 0)
				trq.fUpdate(i, numberOfFeatures); 
		}
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
						double z1 = z * cube.altitude() / (timeBounds->tMax() - timeBounds->tMin());
						double z2;
						if (fTimeAttribute2) {
							z2 = getTimeValue2(feature) * cube.altitude() / (timeBounds->tMax() - timeBounds->tMin());
						} else {
							z2 = z1 + delta;
							z1 -= delta;
						}
						float rsPnt = textureOffset + textureRange * (fUseAttributeColumn ? (((fValueMap ? attributeColumn->rValue(feature->iValue()) : attributeColumn->iRaw(feature->iValue())) - minMapVal) / width) : ((feature->rValue() - minMapVal) / width));
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

						glLoadName(i);
						map<long, GLuint>::iterator mapEntry = subDisplayLists->find(i);
						if (mapEntry == subDisplayLists->end()) {
							GLuint listID = glGenLists(1); // not compiled in the display list, but executed immediately
							(*subDisplayLists)[i] = listID;
						}
						glCallList((*subDisplayLists)[i]);
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
							glVertex3f(pnt.x + normCircle.x, pnt.y + normCircle.y, z1);
							glVertex3f(pnt.x + normCircle.x, pnt.y + normCircle.y, z2);
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
	glPopName();
}

void StationsDrawer::drawFootprint() const
{
}

void StationsDrawer::drawXT() const
{
}

void StationsDrawer::drawXY() const
{
}

void StationsDrawer::drawYT() const
{
}

//-----------------------------------------------------------------





