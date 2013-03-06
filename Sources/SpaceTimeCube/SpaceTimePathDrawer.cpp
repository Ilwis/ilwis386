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
: SpaceTimeDrawer(parms,"PointLayerDrawer")
, objectStartIndexes(new vector<long>())
, textureOffset(1.0/((double)PALETTE_TEXTURE_SIZE * 2.0))
, textureRange((double)PALETTE_TEXTURE_SIZE / ((double)PALETTE_TEXTURE_SIZE + 1.0))
{
}

SpaceTimePathDrawer::~SpaceTimePathDrawer() {
	delete objectStartIndexes;
}

String SpaceTimePathDrawer::store(const FileName& fnView, const String& parentSection) const
{
	String currentSection = parentSection + "::" + getType();
	SpaceTimeDrawer::store(fnView, currentSection);
	storeSortable(fnView, currentSection);
	storeGroupable(fnView, currentSection);
	return currentSection;
}

void SpaceTimePathDrawer::load(const FileName& fnView, const String& currentSection)
{
	SpaceTimeDrawer::load(fnView, currentSection);
	loadSortable(fnView, currentSection);
	loadGroupable(fnView, currentSection);
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

const vector<Feature *> & SpaceTimePathDrawer::getFeatures() const
{
	return features;
}

vector<GLuint> SpaceTimePathDrawer::getObjectIDs(vector<long> & iRaws) const
{
	vector<GLuint> objectIDs;
	for (GLuint objectID = 0; objectID <= objectStartIndexes->size(); ++objectID) {
		long first = (objectID > 0) ? (*objectStartIndexes)[objectID - 1] : 0;
		long last = (objectID < objectStartIndexes->size()) ? (*objectStartIndexes)[objectID] : features.size();
		for (long i = first; i < last; ++i) {
			Feature *feature = features[i];
			if (feature != 0)
				if (find(iRaws.begin(), iRaws.end(), feature->iValue()) != iRaws.end()) {
					objectIDs.push_back(objectID);
					break;
				}
		}
	}
	return objectIDs;
}

int SpaceTimePathDrawer::getNearestEnabledObjectIDIndex(vector<GLuint> & objectIDs) const
{
	int i = 0; // objectIDs are already sorted from near to far, so we only need to check for disabled iRaws
	if (useAttColumn && getAtttributeColumn().fValid()) {
		while (i < objectIDs.size()) {
			GLuint objectID = objectIDs[i];
			long first = objectID > 0 ? (*objectStartIndexes)[objectID - 1] : 0;
			Feature * feature = features[first];
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

vector<GLuint> SpaceTimePathDrawer::getEnabledObjectIDs(vector<GLuint> & objectIDs) const
{
	if (useAttColumn && getAtttributeColumn().fValid()) {
		vector<GLuint> newObjectIDs;
		for (int i = 0; i < objectIDs.size(); ++i) {
			GLuint objectID = objectIDs[i];
			long first = objectID > 0 ? (*objectStartIndexes)[objectID - 1] : 0;
			Feature * feature = features[first];
			if (feature != 0) {
				long raw = feature->iValue();
				if (raw != iUNDEF) {
					raw = getAtttributeColumn()->iRaw(raw);
					if (find(disabledRaws.begin(), disabledRaws.end(), raw) != disabledRaws.end())
						continue;
				}
			}
			newObjectIDs.push_back(objectID);
		}
		return newObjectIDs;
	} else
		return objectIDs;
}

Feature * SpaceTimePathDrawer::getFeature(GLuint objectID) const
{
	long first = objectID > 0 ? (*objectStartIndexes)[objectID - 1] : 0;
	return features[first];
}

void SpaceTimePathDrawer::getRaws(GLuint objectID, vector<long> & raws) const
{
	// construct raws array
	long first = objectID > 0 ? (*objectStartIndexes)[objectID - 1] : 0;
	long last = objectID < objectStartIndexes->size() ? (*objectStartIndexes)[objectID] : features.size();
	for (long i = first; i < last; ++i) {
		Feature *feature = features[i];
		if (feature != 0)
			raws.push_back(feature->iValue());
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
	double cubeBottom = -0.0000000005; // inaccuracy of "time"
	double cubeTop = 0.000000001 + timeBounds->tMax() - timeBounds->tMin();
	*fHatching = false; // in case of a classmap, if any of the attributes uses hatching, we set fHatching to true
	GLuint objectID = 0;
	objectStartIndexes->clear();
	glInitNames();
	glPushName(objectID);
	map<long, GLuint>::iterator mapEntry = subDisplayLists->find(objectID);
	if (mapEntry == subDisplayLists->end()) {
		GLuint listID = glGenLists(1); // not compiled in the display list, but executed immediately
		(*subDisplayLists)[objectID] = listID;
	}
	glCallList((*subDisplayLists)[objectID]);
	if (steps == 1)
	{
		glBegin(GL_LINE_STRIP);
		String sLastGroupValue = fUseGroup && (numberOfFeatures > 0) ? getGroupValue(features[0]) : "";
		for(long i = 0; i < numberOfFeatures; ++i) {
			Feature *feature = features[i];
			if (fUseGroup && sLastGroupValue != getGroupValue(feature)) {
				sLastGroupValue = getGroupValue(feature);
				glEnd();
				objectStartIndexes->push_back(i);
				glLoadName(++objectID);
				map<long, GLuint>::iterator mapEntry = subDisplayLists->find(objectID);
				if (mapEntry == subDisplayLists->end()) {
					GLuint listID = glGenLists(1); // not compiled in the display list, but executed immediately
					(*subDisplayLists)[objectID] = listID;
				}
				glCallList((*subDisplayLists)[objectID]);
				glBegin(GL_LINE_STRIP);
			}
			ILWIS::Point *point = (ILWIS::Point *)feature;
			double z = getTimeValue(feature);
			if (z >= cubeBottom && z <= cubeTop) {
				Coord crd = *(point->getCoordinate());
				crd.z = z * cube.altitude() / (timeBounds->tMax() - timeBounds->tMin());
				crd = getRootDrawer()->glConv(csy, crd);
				if (fUseAttributeColumn)
					glTexCoord2f(textureOffset + textureRange * ((fValueMap ? attributeColumn->rValue(feature->iValue()) : attributeColumn->iRaw(feature->iValue())) - minMapVal) / width, 0.25f); // 0.25 instead of 0.5, so that no interpolation is needed in Y-direction (the value is taken from the center of the first row)
				else
					glTexCoord2f(textureOffset + textureRange * (feature->rValue() - minMapVal) / width, 0.25f); // 0.25 instead of 0.5, so that no interpolation is needed in Y-direction (the value is taken from the center of the first row)
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
			String sLastGroupValue = fUseGroup ? getGroupValue(features[0]) : "";
			Coord headPrevious;
			Coord tailPrevious;
			long start = 0;
			double z;
			Feature * feature;
			do {
				feature = features[start];
				if (fUseGroup && sLastGroupValue != getGroupValue(feature)) {
					sLastGroupValue = getGroupValue(feature);
					objectStartIndexes->push_back(start);
					glLoadName(++objectID);
				}
				z = getTimeValue(feature);
				++start;
			} while ((z < cubeBottom || z > cubeTop) && start < numberOfFeatures);
			ILWIS::Point *point = (ILWIS::Point *)feature;
			Coord head = *(point->getCoordinate());
			head.z = z * cube.altitude() / (timeBounds->tMax() - timeBounds->tMin());
			head = getRootDrawer()->glConv(csy, head);
			float rsHead = textureOffset + textureRange * (fUseAttributeColumn ? (((fValueMap ? attributeColumn->rValue(feature->iValue()) : attributeColumn->iRaw(feature->iValue())) - minMapVal) / width) : ((feature->rValue() - minMapVal) / width));
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
			bool fCutPath = false;
			for (long i = start; i < numberOfFeatures; ++i)
			{
				feature = features[i];
				if (fUseGroup && sLastGroupValue != getGroupValue(feature)) {
					sLastGroupValue = getGroupValue(feature);
					objectStartIndexes->push_back(i);
					glLoadName(++objectID);
					map<long, GLuint>::iterator mapEntry = subDisplayLists->find(objectID);
					if (mapEntry == subDisplayLists->end()) {
						GLuint listID = glGenLists(1); // not compiled in the display list, but executed immediately
						(*subDisplayLists)[objectID] = listID;
					}
					glCallList((*subDisplayLists)[objectID]);
					fCutPath = true;
				}
				z = getTimeValue(feature);
				if (z >= cubeBottom && z <= cubeTop)
				{
					ILWIS::Point *point = (ILWIS::Point *)feature;
					Coord tail = *(point->getCoordinate());
					tail = getRootDrawer()->glConv(csy, tail);
					tail.z = z * cube.altitude() / (timeBounds->tMax() - timeBounds->tMin());
					float rsTail = textureOffset + textureRange * (fUseAttributeColumn ? (((fValueMap ? attributeColumn->rValue(feature->iValue()) : attributeColumn->iRaw(feature->iValue())) - minMapVal) / width) : ((feature->rValue() - minMapVal) / width));
					double rTail = pathScale * getSizeValue(feature);

					if (fCutPath) {
						fCutPath = false;
						head = Coord(); // no plotting of connectionCoords in next loop
					} else
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
	glPopName();
}

void SpaceTimePathDrawer::drawFootprint() const
{
	Tranquilizer trq(TR("computing footprint"));
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
	double cubeBottom = -0.0000000005; // inaccuracy of time
	double cubeTop = 0.000000001 + timeBounds->tMax() - timeBounds->tMin();
	GLuint objectID = 0;
	glCallList((*subDisplayLists)[objectID]);
	glBegin(GL_LINE_STRIP);
	String sLastGroupValue = fUseGroup && (numberOfFeatures > 0) ? getGroupValue(features[0]) : "";
	for(long i = 0; i < numberOfFeatures; ++i) {
		Feature *feature = features[i];
		if (fUseGroup && sLastGroupValue != getGroupValue(feature)) {
			sLastGroupValue = getGroupValue(feature);
			glEnd();
			++objectID;
			glCallList((*subDisplayLists)[objectID]);
			glBegin(GL_LINE_STRIP);
		}
		ILWIS::Point *point = (ILWIS::Point *)feature;
		double z = getTimeValue(feature);
		if (z >= cubeBottom && z <= cubeTop) {
			Coord crd = *(point->getCoordinate());
			crd.z = 0; // z * cube.altitude() / (timeBounds->tMax() - timeBounds->tMin());
			crd = getRootDrawer()->glConv(csy, crd);
			if (fUseAttributeColumn)
				glTexCoord2f(textureOffset + textureRange * ((fValueMap ? attributeColumn->rValue(feature->iValue()) : attributeColumn->iRaw(feature->iValue())) - minMapVal) / width, 0.25f); // 0.25 instead of 0.5, so that no interpolation is needed in Y-direction (the value is taken from the center of the first row)
			else
				glTexCoord2f(textureOffset + textureRange * (feature->rValue() - minMapVal) / width, 0.25f); // 0.25 instead of 0.5, so that no interpolation is needed in Y-direction (the value is taken from the center of the first row)
			glVertex3f(crd.x, crd.y, crd.z);
		}
		if ( i % 100 == 0)
			trq.fUpdate(i, numberOfFeatures); 
	}
	glEnd();
}

//-----------------------------------------------------------------





