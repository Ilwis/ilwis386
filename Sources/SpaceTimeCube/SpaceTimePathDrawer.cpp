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
#include "SpaceTimePathDrawer.h"
#include "Drawers\PointDrawer.h"
#include "Drawers\PointFeatureDrawer.h"
#include "Engine\Representation\Rprclass.h"

using namespace ILWIS;

ILWIS::NewDrawer *createSpaceTimePathDrawer(DrawerParameters *parms) {
	return new SpaceTimePathDrawer(parms);
}

SpaceTimePathDrawer::SpaceTimePathDrawer(DrawerParameters *parms)
: SpaceTimeDrawer(parms,"SpaceTimePathDrawer")
{
}

SpaceTimePathDrawer::~SpaceTimePathDrawer() {
}

struct sort_pair_first_value { 
    bool operator()(const std::pair<double, Feature*> &left, const std::pair<double, Feature*> &right) { 
        return left.first < right.first;
    } 
}; 

struct sort_pair_first_string { 
    bool operator()(const std::pair<String, Feature*> &left, const std::pair<String, Feature*> &right) { 
        return left.first < right.first;
    } 
}; 

void SpaceTimePathDrawer::prepare(PreparationParameters *parms){
	if (!isActive())
		return;
	SpaceTimeDrawer::prepare(parms);
	FeatureDataDrawer *mapDrawer = (FeatureDataDrawer *)parentDrawer;
	if ( (parms->type & RootDrawer::ptGEOMETRY) || (parms->type & NewDrawer::pt3D)) {
		features.clear();
		long numberOfFeatures = basemap->iFeatures();
		if (numberOfFeatures != iUNDEF) {
			if (fUseSort) {
				if (fSortValues) {
					vector<std::pair<double, Feature*>> sortedFeatures;
					for(long i = 0; i < numberOfFeatures; ++i) {
						Feature *feature = CFEATURE(basemap->getFeature(i));
						if ( feature && feature->fValid())
							sortedFeatures.push_back(std::pair<double, Feature*>(getSortValue(feature), feature));
					}
					std::stable_sort(sortedFeatures.begin(), sortedFeatures.end(), sort_pair_first_value());
					for(vector<std::pair<double, Feature*>>::const_iterator it = sortedFeatures.begin(); it != sortedFeatures.end(); ++it)
						features.push_back((*it).second);
				} else { // sort by String
					vector<std::pair<String, Feature*>> sortedFeatures;
					for(long i = 0; i < numberOfFeatures; ++i) {
						Feature *feature = CFEATURE(basemap->getFeature(i));
						if ( feature && feature->fValid())
							sortedFeatures.push_back(std::pair<String, Feature*>(getSortString(feature), feature));
					}
					std::stable_sort(sortedFeatures.begin(), sortedFeatures.end(), sort_pair_first_string());
					for(vector<std::pair<String, Feature*>>::const_iterator it = sortedFeatures.begin(); it != sortedFeatures.end(); ++it)
						features.push_back((*it).second);
				}
			} else {
				for(long i = 0; i < numberOfFeatures; ++i) {
					Feature *feature = CFEATURE(basemap->getFeature(i));
					if ( feature && feature->fValid() && feature->rValue() != rUNDEF)
						features.push_back(feature);
				}
			}
		}
	}
}

void SpaceTimePathDrawer::drawObjects(const int steps, GetHatchFunc getHatchFunc) const
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
	long numberOfFeatures = features.size();
	double cubeBottom = 0;
	double cubeTop = timeBounds->tMax() - timeBounds->tMin();
	*fHatching = false; // in case of a classmap, if any of the attributes uses hatching, we set fHatching to true
	if (steps == 1)
	{
		glBegin(GL_LINE_STRIP);
		String sLastGroupValue = fUseGroup && (numberOfFeatures > 0) ? getGroupValue(features[0]) : "";
		for(long i = 0; i < numberOfFeatures; ++i) {
			Feature *feature = features[i];
			if (fUseGroup && sLastGroupValue != getGroupValue(feature)) {
				sLastGroupValue = getGroupValue(feature);
				glEnd();
				glBegin(GL_LINE_STRIP);
			}
			ILWIS::Point *point = (ILWIS::Point *)feature;
			double z = getTimeValue(feature);
			if (z >= cubeBottom && z <= cubeTop) {
				Coord crd = *(point->getCoordinate());
				crd.z = z * cube.altitude() / (timeBounds->tMax() - timeBounds->tMin());
				crd = getRootDrawer()->glConv(csy, crd);
				if (fUseAttributeColumn)
					glTexCoord2f(((fValueMap ? attributeColumn->rValue(feature->iValue()) : attributeColumn->iRaw(feature->iValue())) - minMapVal) / width, 0.25f); // 0.25 instead of 0.5, so that no interpolation is needed in Y-direction (the value is taken from the center of the first row)
				else
					glTexCoord2f((feature->rValue() - minMapVal) / width, 0.25f); // 0.25 instead of 0.5, so that no interpolation is needed in Y-direction (the value is taken from the center of the first row)
				glVertex3f(crd.x, crd.y, crd.z);
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
			Coord headPrevious;
			Coord tailPrevious;
			long start = 0;
			double z;
			Feature * feature;
			do {
				feature = features[start];
				z = getTimeValue(feature);
				++start;
			} while ((z < cubeBottom || z > cubeTop) && start < numberOfFeatures);
			ILWIS::Point *point = (ILWIS::Point *)feature;
			Coord head = *(point->getCoordinate());
			head.z = z * cube.altitude() / (timeBounds->tMax() - timeBounds->tMin());
			head = getRootDrawer()->glConv(csy, head);
			String sLastGroupValue = fUseGroup ? getGroupValue(feature) : "";
			float rsHead = fUseAttributeColumn ? (((fValueMap ? attributeColumn->rValue(feature->iValue()) : attributeColumn->iRaw(feature->iValue())) - minMapVal) / width) : ((feature->rValue() - minMapVal) / width);
			double rHead = pathScale * getSizeValue(feature);
			const byte * hatch = 0;
			RepresentationClass * prc = 0;
			if (fUseAttributeColumn) {
				Representation rpr = attributeColumn->dm()->rpr();
				if ( rpr.fValid() && rpr->dm()->pdc())
					prc = rpr->prc();
			}
			if (prc)
				(this->*getHatchFunc)(prc, attributeColumn->iRaw(feature->iValue()), hatch);
			for (long i = start; i < numberOfFeatures; ++i)
			{
				feature = features[i];
				z = getTimeValue(feature);
				if (z >= cubeBottom && z <= cubeTop)
				{
					ILWIS::Point *point = (ILWIS::Point *)feature;
					Coord tail = *(point->getCoordinate());
					tail = getRootDrawer()->glConv(csy, tail);
					tail.z = z * cube.altitude() / (timeBounds->tMax() - timeBounds->tMin());
					float rsTail = fUseAttributeColumn ? (((fValueMap ? attributeColumn->rValue(feature->iValue()) : attributeColumn->iRaw(feature->iValue())) - minMapVal) / width) : ((feature->rValue() - minMapVal) / width);
					double rTail = pathScale * getSizeValue(feature);

					bool fCutPath = false;
					if (fUseGroup && sLastGroupValue != getGroupValue(feature)) {
						sLastGroupValue = getGroupValue(feature);
						fCutPath = true;
					}

					if (!fCutPath)
					{
						if (!headPrevious.fUndef())
						{
							// connectionCoords
							glBegin(GL_TRIANGLE_STRIP);
							Coord ABprevious = tailPrevious;
							ABprevious -= headPrevious;
							Coord AB = tail;
							AB -= head;

							double f = 0;
							for (int step = 0; step < (1 + steps); ++step)
							{
								if (step == steps)
									f = 0; // close the cylinder, choose exactly the same angle as the fist time
								glTexCoord2f(rsHead, 0.25f); // 0.25 instead of 0.5, so that no interpolation is needed in Y-direction (the value is taken from the center of the first row)
								Coord normCircle = projectOnCircle(ABprevious, rHead, f);
								Coord normal = normalize(normCircle);
								glNormal3f(normal.x, normal.y, normal.z);
								normCircle += tailPrevious;
								glVertex3f(normCircle.x, normCircle.y, normCircle.z);
								glTexCoord2f(rsHead, 0.25f); // 0.25 instead of 0.5, so that no interpolation is needed in Y-direction (the value is taken from the center of the first row)
								normCircle = projectOnCircle(AB, rHead, f);
								normal = normalize(normCircle);
								glNormal3f(normal.x, normal.y, normal.z);
								normCircle += head;
								glVertex3f(normCircle.x, normCircle.y, normCircle.z);
								f += angleStep;
							}
							glEnd();
						}

						if (hatch) {
							*fHatching = true;
							glEnable(GL_POLYGON_STIPPLE);
							glPolygonStipple(hatch);
						} else
							glDisable(GL_POLYGON_STIPPLE);

						// cylinderCoords
						glBegin(GL_TRIANGLE_STRIP);
						Coord AB = tail;
						AB -= head;

						double f = 0;
						for (int step = 0; step < (1 + steps); ++step)
						{
							if (step == steps)
								f = 0; // close the cylinder, choose exactly the same angle as the fist time
							glTexCoord2f(rsHead, 0.25f); // 0.25 instead of 0.5, so that no interpolation is needed in Y-direction (the value is taken from the center of the first row)
							Coord normCircle = projectOnCircle(AB, rHead, f);
							Coord normal = normalize(normCircle);
							glNormal3f(normal.x, normal.y, normal.z);
							normCircle += head;
							glVertex3f(normCircle.x, normCircle.y, normCircle.z);
							glTexCoord2f(rsTail, 0.25f); // 0.25 instead of 0.5, so that no interpolation is needed in Y-direction (the value is taken from the center of the first row)
							normCircle = projectOnCircle(AB, rTail, f);
							normal = normalize(normCircle);
							glNormal3f(normal.x, normal.y, normal.z);
							normCircle += tail;
							glVertex3f(normCircle.x, normCircle.y, normCircle.z);
							f += angleStep;
						}

						glEnd();
					}
					// continue
					headPrevious = head;
					tailPrevious = tail;
					head = tail;
					rsHead = rsTail;
					rHead = rTail;
					if (prc) {
						hatch = 0;
						(this->*getHatchFunc)(prc, attributeColumn->iRaw(feature->iValue()), hatch);
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





