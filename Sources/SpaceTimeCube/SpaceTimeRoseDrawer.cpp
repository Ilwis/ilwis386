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
#include "SpaceTimeRoseDrawer.h"
#include "Drawers\PointDrawer.h"
#include "Drawers\PointFeatureDrawer.h"
#include "Engine\Representation\Rprclass.h"
#include "Engine\Drawers\DrawerContext.h"

using namespace ILWIS;

void executeStcCommand(const String& cmd) {
	//String *_cmd = new String(cmd);
	//SpaceTimeRoseDrawer * roseDrawer = (SpaceTimeRoseDrawer*)(rootDrawer->getDrawer("SpaceTimeRoseDrawer"));
	const list<SpaceTimeRoseDrawer*> & lstrd = SpaceTimeRoseDrawer::getSpaceTimeRoseDrawers();
	for (list<SpaceTimeRoseDrawer*>::const_iterator it = lstrd.begin(); it != lstrd.end(); ++it) {
		SpaceTimeRoseDrawer * strd = *it;
		strd->executeStcCommand(cmd);
	}
}

const list<SpaceTimeRoseDrawer*> & SpaceTimeRoseDrawer::getSpaceTimeRoseDrawers()
{
	return lstrd;
}

list<SpaceTimeRoseDrawer*> SpaceTimeRoseDrawer::lstrd;

ILWIS::NewDrawer *createSpaceTimeRoseDrawer(DrawerParameters *parms) {
	return new SpaceTimeRoseDrawer(parms);
}

SpaceTimeRoseDrawer::SpaceTimeRoseDrawer(DrawerParameters *parms)
: SpaceTimePathDrawer(parms)
{
	lstrd.push_back(this);
}

SpaceTimeRoseDrawer::~SpaceTimeRoseDrawer() {
	lstrd.remove(this);
}

void SpaceTimeRoseDrawer::prepare(PreparationParameters *parms){
	if (!isActive())
		return;
	SpaceTimePathDrawer::prepare(parms);
	if (nrSteps < 2)
		nrSteps = 3;
}

void SpaceTimeRoseDrawer::drawObjects(const int steps, GetHatchFunc getHatchFunc) const
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
	glEnable(GL_CULL_FACE);
	//glCullFace(GL_BACK);        //render only front faces
	glInitNames();
	glPushName(objectID);
	map<long, GLuint>::iterator mapEntry = subDisplayLists->find(objectID);
	if (mapEntry == subDisplayLists->end()) {
		GLuint listID = glGenLists(1); // not compiled in the display list, but executed immediately
		(*subDisplayLists)[objectID] = listID;
	}
	glCallList((*subDisplayLists)[objectID]);
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
		double rHeadAngle = M_PI * (round(90.0 - getSize2Value(feature)) % 360) / 180.0; // change clockwise and north = 0 to counter-clockwise and east = 0
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
			if (!fIDSelected(sLastGroupValue))
				continue;
			if (z >= cubeBottom && z <= cubeTop)
			{
				ILWIS::Point *point = (ILWIS::Point *)feature;
				Coord tail = *(point->getCoordinate());
				tail = getRootDrawer()->glConv(csy, tail);
				tail.z = z * cube.altitude() / (timeBounds->tMax() - timeBounds->tMin());
				float rsTail = textureOffset + textureRange * (fUseAttributeColumn ? (((fValueMap ? attributeColumn->rValue(feature->iValue()) : attributeColumn->iRaw(feature->iValue())) - minMapVal) / width) : ((feature->rValue() - minMapVal) / width));
				double rTail = pathScale * getSizeValue(feature);
				double rTailAngle = M_PI * (round(90.0 - getSize2Value(feature)) % 360) / 180.0; // change clockwise and north = 0 to counter-clockwise and east = 0

				if (fCutPath) {
					fCutPath = false;
					head = Coord(); // no plotting of connectionCoords in next loop
				} else {
					if (hatch) {
						*fHatching = true;
						glEnable(GL_POLYGON_STIPPLE);
						glPolygonStipple(hatch);
					} else
						glDisable(GL_POLYGON_STIPPLE);

					// cylinderCoords
					if (fTimeSelected(sLastGroupValue, z))
						drawItem(head, rHead, rHeadAngle, rsHead, tail, rTail, rTailAngle, rsTail);
				}
				// continue
				headPrevious = head;
				tailPrevious = tail;
				head = tail;
				rsHead = rsTail;
				rHead = rTail;
				rHeadAngle = rTailAngle;
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
	glPopName();
	glDisable(GL_CULL_FACE);
}

void SpaceTimeRoseDrawer::drawItem(Coord head, double rHead, double rHeadAngle, float rsHead, Coord tail, double rTail, double rTailAngle, float rsTail) const {
	bool fFixedColor = true;
	glBegin(GL_TRIANGLE_STRIP);

	Coord AB = tail;
	AB -= head;
	double rHeadAngle1 = rHeadAngle - M_PI / 32.0;
	double rHeadAngle2 = rHeadAngle + M_PI / 32.0;
	double rTailAngle1 = rTailAngle - M_PI / 32.0;
	double rTailAngle2 = rTailAngle + M_PI / 32.0;
	const CoordBounds& cbMap = getRootDrawer()->getMapCoordBounds();
	double pathScale = cbMap.width() / 50;
	const double rCenter = 0; // pathScale * properties->exaggeration / 20.0;

	if (fFixedColor)
		glTexCoord2f(rsHead, 0.25f);
	
	// left face
	Coord normCircle = projectOnCircle(AB, rHead, rHeadAngle1 - M_PI / 2.0);
	Coord normal = normalize(normCircle);
	glNormal3f(normal.x, normal.y, normal.z);
	if (!fFixedColor)
		glTexCoord2f(textureOffset, 0.25f);
	normCircle = projectOnCircle(AB, rCenter, rHeadAngle1);
	normCircle += head;
	glVertex3f(normCircle.x, normCircle.y, normCircle.z);
	if (!fFixedColor)
		glTexCoord2f(rsHead, 0.25f);
	normCircle = projectOnCircle(AB, rHead, rHeadAngle1);
	normCircle += head;
	glVertex3f(normCircle.x, normCircle.y, normCircle.z);
	if (!fFixedColor)
		glTexCoord2f(textureOffset, 0.25f);
	normCircle = projectOnCircle(AB, rCenter, rHeadAngle1);
	normCircle += tail;
	glVertex3f(normCircle.x, normCircle.y, normCircle.z);
	normCircle = projectOnCircle(AB, rHead, rHeadAngle1);
	normCircle += tail;
	if (!fFixedColor)
		glTexCoord2f(rsHead, 0.25f);
	glVertex3f(normCircle.x, normCircle.y, normCircle.z);

	// top face
	glNormal3f(AB.x, AB.y, AB.z);
	/*
	if (!fFixedColor)
		glTexCoord2f(textureOffset, 0.25f);
	glVertex3f(tail.x, tail.y, tail.z);
	if (!fFixedColor)
		glTexCoord2f(rsHead, 0.25f);
	glVertex3f(normCircle.x, normCircle.y, normCircle.z);
	normCircle = projectOnCircle(AB, rHead, rHeadAngle2);
	normCircle += tail;
	if (!fFixedColor)
		glTexCoord2f(textureOffset, 0.25f);
	glVertex3f(tail.x, tail.y, tail.z);
	if (!fFixedColor)
		glTexCoord2f(rsHead, 0.25f);
	glVertex3f(normCircle.x, normCircle.y, normCircle.z);
	*/

	if (rTailAngle1 > rHeadAngle1) {
		glEnd();
		glBegin(GL_TRIANGLE_STRIP);
		double rEndAngle = min(rHeadAngle2, rTailAngle1);
		normCircle = projectOnCircle(AB, rCenter, rHeadAngle1);
		normCircle += tail;
		glVertex3f(normCircle.x, normCircle.y, normCircle.z);
		normCircle = projectOnCircle(AB, rHead, rHeadAngle1);
		normCircle += tail;
		glVertex3f(normCircle.x, normCircle.y, normCircle.z);
		normCircle = projectOnCircle(AB, rCenter, rEndAngle);
		normCircle += tail;
		glVertex3f(normCircle.x, normCircle.y, normCircle.z);
		normCircle = projectOnCircle(AB, rHead, rEndAngle);
		normCircle += tail;
		glVertex3f(normCircle.x, normCircle.y, normCircle.z);
	}

	if (rTailAngle1<= rHeadAngle2 && rTailAngle2 >= rHeadAngle1 && rTail < rHead) { // overlap
		glEnd();
		glBegin(GL_TRIANGLE_STRIP);
		double rBeginAngle = max(rTailAngle1, rHeadAngle1);
		double rEndAngle = min(rTailAngle2, rHeadAngle2);
		normCircle = projectOnCircle(AB, rTail, rBeginAngle);
		normCircle += tail;
		glVertex3f(normCircle.x, normCircle.y, normCircle.z);
		normCircle = projectOnCircle(AB, rHead, rBeginAngle);
		normCircle += tail;
		glVertex3f(normCircle.x, normCircle.y, normCircle.z);
		normCircle = projectOnCircle(AB, rTail, rEndAngle);
		normCircle += tail;
		glVertex3f(normCircle.x, normCircle.y, normCircle.z);
		normCircle = projectOnCircle(AB, rHead, rEndAngle);
		normCircle += tail;
		glVertex3f(normCircle.x, normCircle.y, normCircle.z);
	}

	if (rTailAngle2 < rHeadAngle2) {
		glEnd();
		glBegin(GL_TRIANGLE_STRIP);
		double rBeginAngle = max(rHeadAngle1, rTailAngle2);
		normCircle = projectOnCircle(AB, rCenter, rBeginAngle);
		normCircle += tail;
		glVertex3f(normCircle.x, normCircle.y, normCircle.z);
		normCircle = projectOnCircle(AB, rHead, rBeginAngle);
		normCircle += tail;
		glVertex3f(normCircle.x, normCircle.y, normCircle.z);
		normCircle = projectOnCircle(AB, rCenter, rHeadAngle2);
		normCircle += tail;
		glVertex3f(normCircle.x, normCircle.y, normCircle.z);
		normCircle = projectOnCircle(AB, rHead, rHeadAngle2);
		normCircle += tail;
		glVertex3f(normCircle.x, normCircle.y, normCircle.z);
	}

	glEnd();
	glBegin(GL_TRIANGLE_STRIP);

	// right face
	Coord normCircle2 = projectOnCircle(AB, rHead, rHeadAngle2 + M_PI / 2.0);
	normal = normalize(normCircle2);
	glNormal3f(normal.x, normal.y, normal.z);
	if (!fFixedColor)
		glTexCoord2f(textureOffset, 0.25f);
	normCircle = projectOnCircle(AB, rCenter, rHeadAngle2);
	normCircle += tail;
	glVertex3f(normCircle.x, normCircle.y, normCircle.z);
	if (!fFixedColor)
		glTexCoord2f(rsHead, 0.25f);
	normCircle = projectOnCircle(AB, rHead, rHeadAngle2);
	normCircle += tail;
	glVertex3f(normCircle.x, normCircle.y, normCircle.z);
	if (!fFixedColor)
		glTexCoord2f(textureOffset, 0.25f);
	normCircle = projectOnCircle(AB, rCenter, rHeadAngle2);
	normCircle += head;
	glVertex3f(normCircle.x, normCircle.y, normCircle.z);
	if (!fFixedColor)
		glTexCoord2f(rsHead, 0.25f);
	normCircle = projectOnCircle(AB, rHead, rHeadAngle2);
	normCircle += head;
	glVertex3f(normCircle.x, normCircle.y, normCircle.z);

	// bottom face
	Coord BA = head;
	BA -= tail;
	glNormal3f(BA.x, BA.y, BA.z);
	/*
	if (!fFixedColor)
		glTexCoord2f(textureOffset, 0.25f);
	glVertex3f(head.x, head.y, head.z);
	if (!fFixedColor)
		glTexCoord2f(rsHead, 0.25f);
	glVertex3f(normCircle.x, normCircle.y, normCircle.z);
	normCircle = projectOnCircle(AB, rHead, rHeadAngle1);
	normCircle += head;
	if (!fFixedColor)
		glTexCoord2f(textureOffset, 0.25f);
	glVertex3f(head.x, head.y, head.z);
	if (!fFixedColor)
		glTexCoord2f(rsHead, 0.25f);
	glVertex3f(normCircle.x, normCircle.y, normCircle.z);
	*/

	// front face
	normCircle = projectOnCircle(AB, rHead, rHeadAngle1);
	normCircle += head;
	normCircle2 = projectOnCircle(AB, rHead, rHeadAngle);
	normal = normalize(normCircle2);
	glNormal3f(normal.x, normal.y, normal.z);
	glVertex3f(normCircle.x, normCircle.y, normCircle.z);
	normCircle = projectOnCircle(AB, rHead, rHeadAngle2);
	normCircle += head;
	glVertex3f(normCircle.x, normCircle.y, normCircle.z);
	normCircle = projectOnCircle(AB, rHead, rHeadAngle1);
	normCircle += tail;
	glVertex3f(normCircle.x, normCircle.y, normCircle.z);
	normCircle = projectOnCircle(AB, rHead, rHeadAngle2);
	normCircle += tail;
	glVertex3f(normCircle.x, normCircle.y, normCircle.z);

	glEnd();
}

void SpaceTimeRoseDrawer::drawXY() const
{
	Tranquilizer trq(TR("computing XY lines"));
	long numberOfFeatures = features.size();
	double cubeBottom = -0.0000000005; // inaccuracy of "time"
	double cubeTop = 0.000000001 + timeBounds->tMax() - timeBounds->tMin();
	GLuint objectID = 0;
	glInitNames();
	glPushName(objectID);
	String sLastGroupValue = fUseGroup && (numberOfFeatures > 0) ? getGroupValue(features[0]) : "";
	Coord crd;
	for(long i = 0; i < numberOfFeatures; ++i) {
		Feature *feature = features[i];
		if (fUseGroup && sLastGroupValue != getGroupValue(feature)) {
			sLastGroupValue = getGroupValue(feature);
			glCallList((*subDisplayLists)[objectID]);
			if (!crd.fUndef()) {
				double zMin = cubeBottom * cube.altitude() / (timeBounds->tMax() - timeBounds->tMin());
				double zMax = cubeTop * cube.altitude() / (timeBounds->tMax() - timeBounds->tMin());
				glBegin(GL_LINES);
				glTexCoord2f(textureOffset, 0.25f); // 0.25 instead of 0.5, so that no interpolation is needed in Y-direction (the value is taken from the center of the first row)
				glVertex3f(crd.x, crd.y, zMin);
				glVertex3f(crd.x, crd.y, zMax);
				glEnd();
			}
			crd = Coord();
			glLoadName(++objectID);
		}
		ILWIS::Point *point = (ILWIS::Point *)feature;
		double z = getTimeValue(feature);
		if (z >= cubeBottom && z <= cubeTop) {
			if (crd.fUndef()) {
				crd = *(point->getCoordinate());
				crd.z = 0;
				crd = getRootDrawer()->glConv(csy, crd);
			}
		}
		if ( i % 100 == 0)
			trq.fUpdate(i, numberOfFeatures); 
	}
	glCallList((*subDisplayLists)[objectID]);
	if (!crd.fUndef()) {
		double zMin = cubeBottom * cube.altitude() / (timeBounds->tMax() - timeBounds->tMin());
		double zMax = cubeTop * cube.altitude() / (timeBounds->tMax() - timeBounds->tMin());
		glBegin(GL_LINES);
		glTexCoord2f(textureOffset, 0.25f); // 0.25 instead of 0.5, so that no interpolation is needed in Y-direction (the value is taken from the center of the first row)
		glVertex3f(crd.x, crd.y, zMin);
		glVertex3f(crd.x, crd.y, zMax);
		glEnd();
	}
	glPopName();
}

void SpaceTimeRoseDrawer::executeStcCommand(const String& cmd)
{
	if (cmd.iPos(String("selecttimes")) == 0) {
		timeSelection.clear();
		TimeParser parser ("YYYY/MM/DD hh/mm/ss");
		String cmdcopy = cmd;
		char * str = cmdcopy.sVal();
		str += 11;
		str = strchr(str, '(');
		if (str != 0) {
			bool fAbort = false;
			++str;
			str = strchr(str, '"');
			while (str != 0 && !fAbort) {
				++str;
				char * endquote = strchr(str, '"');
				if (endquote == 0) {
					fAbort = true;
					break;
				}
				*endquote = '\0';
				String sID (str);
				sID = sID.sTrimSpaces();
				str = endquote + 1;
				str = strchr(str, '(');
				if (str == 0) {
					fAbort = true;
					break;
				}
				++str;
				bool fNextID = false;
				while (!fNextID) {
					char * comma = strchr(str, ',');
					if (comma == 0) {
						fNextID = true;
						fAbort = true;
						break;
					}
					*comma = '\0';
					String start (str);
					start = start.sTrimSpaces();
					str = comma + 1;
					comma = strchr(str, ',');
					char * closingbracket = strchr(str, ')');
					if (comma > closingbracket)
						comma = 0;
					if (comma == 0) {
						fNextID = true;
						comma = closingbracket;
					}
					if (comma == 0) {
						fNextID = true;
						fAbort = true;
						break;
					}
					*comma = '\0';
					String end (str);
					end = end.sTrimSpaces();
					Time timeStart(start, parser);
					Time timeEnd(end, parser);
					timeSelection[sID].push_back(pair<double, double>(timeStart, timeEnd));
					str = comma + 1;
				}
				//str = strchr(str, ',');
				//if (str != 0)
					str = strchr(str, '"');
				//else
				//	fAbort = true;
			}
		}

		*fRefreshDisplayList = true;
		getRootDrawer()->getDrawerContext()->doDraw();
	}
}

bool SpaceTimeRoseDrawer::fIDSelected(String & sID) const
{
	if (timeSelection.size() == 0)
		return true;

	map<String, list<pair<double, double>>>::const_iterator & it = timeSelection.find(sID);
	return it != timeSelection.end();
}

bool SpaceTimeRoseDrawer::fTimeSelected(String & sID, double time) const
{
	if (timeSelection.size() == 0)
		return true;

	map<String, list<pair<double, double>>>::const_iterator & it = timeSelection.find(sID);
	if (it == timeSelection.end())
		return false;

	const list<pair<double, double>> & timeEntries = it->second;

	time += (double)(timeBounds->tMin()) - 0.0000000005; // inaccuracy of "time";

	for (list<pair<double, double>>::const_iterator & it2 = timeEntries.begin(); it2 != timeEntries.end(); ++it2) {
		if (time >= it2->first && time <= it2->second)
			return true;
	}

	return false;

}

//-----------------------------------------------------------------





