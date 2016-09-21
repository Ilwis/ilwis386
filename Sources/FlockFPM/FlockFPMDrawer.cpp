/***************************************************************
ILWIS integrates image, vector and thematic data in one unique 
and powerful package on the desktop. ILWIS delivers a wide 
range of feautures including import/export, digitizing, editing, 
analysis and display of data as well as production of 
quality mapsinformation about the sensor mounting platform

Exclusive rights of use by 52°North Initiative for Geospatial 
Open Source Software GmbH 2007, Germany

Copyright (C) 2007 by 52°North Initiative for Geospatial
Open Source Software GmbH

Author: Jan Hendrikse, Willem Nieuwenhuis,Wim Koolhoven 
Bas Retsios, Martin Schouwenburg, Lichun Wang, Jelle Wind 

Contact: Martin Schouwenburg; schouwenburg@itc.nl; 
tel +31-534874371

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
version 2 as published by the Free Software Foundation.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program (see gnu-gpl v2.txt); if not, write to
the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
Boston, MA 02111-1307, USA or visit the web page of the Free
Software Foundation, http://www.fsf.org.

Created on: 2007-02-8
***************************************************************/
// FlockFPMDrawer.cpp : implementation file
//
//////////////////////////////////////////////////////////////////////

#include "Headers\toolspch.h"
#include "Engine\Map\basemap.h"
#include "Engine\Drawers\RootDrawer.h"
#include "Drawers\LayerDrawer.h"
#include "Drawers\FeatureLayerDrawer.h"

#include "SpaceTimeCube\SpaceTimePathDrawer.h"
#include "Engine\Drawers\DrawerContext.h"

#include "LCMFlockViz.h"
#include "FlockFPMDrawer.h"
#include "Engine\Map\Point\ilwPoint.h"
#include "Drawers\DrawingColor.h" 

using namespace ILWIS;

ILWIS::NewDrawer *createFlockFPMDrawer(DrawerParameters *parms) {
	return new FlockFPMDrawer(parms);
}

FlockFPMDrawer::FlockFPMDrawer(DrawerParameters *parms)
: SpaceTimeDrawer(parms,"FlockFPMDrawer")
, textureOffset(1.0/((double)PALETTE_TEXTURE_SIZE * 2.0))
, textureRange((double)PALETTE_TEXTURE_SIZE / ((double)PALETTE_TEXTURE_SIZE + 1.0))
{
	id = name = "FlockFPMDrawer";
	setAlpha(1); // opaque
	mu = 0;
	epsilon = 200;
	delta = 0;
	rSampleInterval = rUNDEF;
	setSingleColor(Color(255, 0, 0));
	setUseRpr(false);
	SetNrSteps(10);
}

FlockFPMDrawer::~FlockFPMDrawer() {
}

void FlockFPMDrawer::setSpaceTimePathDrawer(SpaceTimePathDrawer * _stpdrw) {
	stpdrw = _stpdrw;
}

void FlockFPMDrawer::setSampleInterval(double _rSampleInterval) {
	rSampleInterval = _rSampleInterval;
}

const double FlockFPMDrawer::getSampleInterval() const
{
	return rSampleInterval;
}

const unsigned int FlockFPMDrawer::getMu() const
{
	return mu;
}

const double FlockFPMDrawer::getEpsilon() const
{
	return epsilon;
}

const unsigned int FlockFPMDrawer::getDelta() const
{
	return delta;
}

void FlockFPMDrawer::prepare(PreparationParameters *pp) {
	SpaceTimeDrawer::prepare(pp);
	if ((pp->type & RootDrawer::ptGEOMETRY) || (pp->type & NewDrawer::ptRESTORE)) {
		Tranquilizer trq(TR("Reading point data"));
		timestamps.clear();
		const vector<Feature *> & features = stpdrw->getFeatures();
		const TimeBounds * timeBounds = stpdrw->getTimeBounds();
		const bool fUseGroup = stpdrw->fGetUseGroup();
		long numberOfFeatures = features.size();
		String sLastGroupValue = fUseGroup && (numberOfFeatures > 0) ? stpdrw->getGroupValue(features[0]) : "";
		bool fUseIDColumn = (sLastGroupValue.iVal() != iUNDEF);
		const Column & colTime = stpdrw->getTimeAttribute();
		//colTime->rValue(f->iValue())
		//Time(rrMinMax.rHi());
		int trajectoryId = 0;
		//int timeframe = 0;
		CoordSystem & csy = stpdrw->getCoordSystem();
		trajectoryIDToGLuint.clear();
		long iStart = 0;
		for(long i = 0; i < numberOfFeatures; ++i) {
			Feature *feature = features[i];
			if (fUseGroup && sLastGroupValue != stpdrw->getGroupValue(feature)) {
				SamplePoints(features, iStart, i - 1, fUseIDColumn ? sLastGroupValue.iVal() : trajectoryId, colTime, csy);
				trajectoryIDToGLuint[fUseIDColumn ? sLastGroupValue.iVal() : trajectoryId] = trajectoryId;
				sLastGroupValue = stpdrw->getGroupValue(feature);
				++trajectoryId;
				//timeframe = 0;
				iStart = i;
			}
			//ILWIS::Point *point = (ILWIS::Point *)feature;
			//double z = stpdrw->getTimeValue(feature);
			//Coordinate crd = *(((ILWIS::Point *)feature)->getCoordinate());
			//crd = getRootDrawer()->glConv(csy, crd); // this can be here or after flocking
			//Point2d point (crd.x, crd.y);
			//point.setUserData(trajectoryId);
			//timemap[timeframe] = z;
			//timestamps[timeframe++].push_back(point);
			if (i % 100 == 0)
				trq.fUpdate(i, numberOfFeatures);
		}
		if (iStart < numberOfFeatures - 1) {
			SamplePoints(features, iStart, numberOfFeatures - 1, fUseIDColumn ? sLastGroupValue.iVal() : trajectoryId, colTime, csy);
			trajectoryIDToGLuint[fUseIDColumn ? sLastGroupValue.iVal() : trajectoryId] = trajectoryId;
		}
		trq.fUpdate(numberOfFeatures, numberOfFeatures);
		mu = 0; // this will force recomputation of the disks at the next call of recomputeFlocks()
	}
	if (pp->type & NewDrawer::pt3D) {
	}
}

void FlockFPMDrawer::SamplePoints(const vector<Feature *> & features, const long iStart, const long iEnd, const int id, const Column & colTime, const CoordSystem & csy) {
	long i = iStart;
	Feature *point1 = features[i];
	double time = colTime->rrMinMax().rLo();
	int timeframe = 0;
	//double lastTime = stpdrw->getTimeValue(features[iEnd]);
	double lastTime = colTime->rValue(features[iEnd]->iValue());
	while (time <= lastTime && i <= iEnd) {
		//double point1Time = stpdrw->getTimeValue(point1);
		double point1Time = colTime->rValue(point1->iValue());
		if (time > point1Time && i < iEnd) {
			Feature *point2 = features[i + 1];
			//double point2Time = stpdrw->getTimeValue(point2);
			double point2Time = colTime->rValue(point2->iValue());
			if (time < point2Time) {
				double w1 = (point2Time - time) / (point2Time - point1Time);
				double w2 = (time - point1Time) / (point2Time - point1Time);
				Coordinate crd1 = *(((ILWIS::Point *)point1)->getCoordinate());
				Coordinate crd2 = *(((ILWIS::Point *)point2)->getCoordinate());
				Coordinate crd (w1 * crd1.x + w2 * crd2.x, w1 * crd1.y + w2 * crd2.y); // linear interpolation
				crd = getRootDrawer()->glConv(csy, crd); // this can be here or after flocking
				Point2d point (crd.x, crd.y);
				point.setUserData(id);
				//timemap[timeframe] = time;
				timestamps[timeframe++].push_back(point);
				time += rSampleInterval;
			} else {
				++i;
				point1 = point2;
			}
		} else if (time == point1Time) {
			Coordinate crd = *(((ILWIS::Point *)point1)->getCoordinate());
			crd = getRootDrawer()->glConv(csy, crd); // this can be here or after flocking
			Point2d point (crd.x, crd.y);
			point.setUserData(id);
			//timemap[timeframe] = time;
			timestamps[timeframe++].push_back(point);
			time += rSampleInterval;
		} else if (time < point1Time) {
			++timeframe;
			time += rSampleInterval;
		}
	}
}

vector<GLuint> FlockFPMDrawer::getObjectIDs(vector<long> & iRaws) const
{
	vector<GLuint> objectIDs;
	//vector<GLuint> glIds = stpdrw->getObjectIDs(iRaws);
	//map<GLuint, int>::const_iterator element = gluInttoTrajectoryID.find(glId);
	//if (element != gluInttoTrajectoryID.end()) {
	//	int trajId = element->second);
	//}
	return objectIDs;
}

int FlockFPMDrawer::getNearestEnabledObjectIDIndex(vector<GLuint> & objectIDs) const
{
	return 0; // objectIDs are sorted from near to far
}

vector<GLuint> FlockFPMDrawer::getEnabledObjectIDs(vector<GLuint> & objectIDs) const
{
	return objectIDs;
}

Feature * FlockFPMDrawer::getFeature(GLuint objectID) const
{
	return 0;
}

void FlockFPMDrawer::getRaws(GLuint objectID, vector<long> & raws) const
{
	// construct raws array
	// objectID is the flockID; for one objectID we need to do a look-up to the corresponding trajectoryIDs (space time paths)
	const vector<int> & finalPoints = trajectoryIDs[objectID];
	for (int i = 0; i < finalPoints.size(); ++i) {
		int trajId = finalPoints[i];
		map<int, GLuint>::const_iterator element = trajectoryIDToGLuint.find(trajId);
		if (element != trajectoryIDToGLuint.end()) {
			GLuint glId = element->second;
			stpdrw->getRaws(glId, raws);
		}
	}
}

String FlockFPMDrawer::getInfo(const Coord& c) const
{
	if ( !hasInfo() || !isActive() )
		return "";
	String info;
	RowCol rc = rootDrawer->WorldToScreen(c);
	CRect rect (rc.Col, rc.Row, rc.Col + 1, rc.Row + 1);
	vector<GLuint> objectIDs = getSelectedObjectIDs(rect);
	int i = getNearestEnabledObjectIDIndex(objectIDs);
	if (i < objectIDs.size()) {
		GLuint objectID = objectIDs[i];
		// construct info
		info = String("FlockID = %d; Members = [", objectID);
		const vector<int> & finalPoints = trajectoryIDs[objectID];
		for (unsigned int i = 0; i < finalPoints.size() - 1; ++i)
			info = info + String("%d, ", finalPoints[i]);
		info = info + String("%d]", finalPoints[finalPoints.size() - 1]);
	}
	return info;
}

void FlockFPMDrawer::drawObjects(const int steps, GetHatchFunc getHatchFunc) const {
	Tranquilizer trq(TR("computing triangles"));
	double cubeBottom = -0.0000000005; // should be 0 .. this is a workaround for accurracy problem of rrMinMax in Talbe ODF (otherwise the first sample is unwantedly excluded)
	double cubeTop = 0.000000001 + timeBounds->tMax() - timeBounds->tMin();
	double zScaleFactor = cube.altitude() / (timeBounds->tMax() - timeBounds->tMin());
	double timeOffset = stpdrw->getTimeAttribute()->rrMinMax().rLo() - timeBounds->tMin();

	glTexCoord2f(textureOffset, 0.25f); // 0.25 instead of 0.5, so that no interpolation is needed in Y-direction (the value is taken from the center of the first row)

	GLuint flockID = 0;
	glInitNames();
	glPushName(flockID);
	map<long, GLuint>::iterator mapEntry = subDisplayLists->find(flockID);
	if (mapEntry == subDisplayLists->end()) {
		GLuint listID = glGenLists(1); // not compiled in the display list, but executed immediately
		(*subDisplayLists)[flockID] = listID;
	}
	glCallList((*subDisplayLists)[flockID]);

	if (steps == 1) {
	} else {
		const double angleStep = 2.0 * M_PI / steps;
		for (vector<vector<Coord>>::const_iterator & flockIterator = flocks.begin(); flockIterator != flocks.end(); ++flockIterator) {
			const vector<Coord> & flock = *flockIterator;
			int iNrFlockCoords = flock.size();
			if (iNrFlockCoords > 1) {
				Coord head = flock[0];
				//double z1 = timemap.find(head.z)->second;
				double z1 = timeOffset + head.z * rSampleInterval;
				if (z1 >= cubeBottom && z1 <= cubeTop) {
					for (int i = 1; i < iNrFlockCoords; ++i) {
						const Coord & tail = flock[i];
						//double z2 = timemap.find(tail.z)->second;
						double z2 = timeOffset + tail.z * rSampleInterval;
						if (z2 >= cubeBottom && z2 <= cubeTop) {

							// cylinderCoords
							glBegin(GL_TRIANGLE_STRIP);

							double f = 0;
							for (int step = 0; step < (1 + steps); ++step)
							{
								if (step == steps)
									f = 0; // close the cylinder, choose exactly the same angle as the fist time
								Coord normCircle(epsilon*cos(f), epsilon*sin(f), 0);
								glNormal3f(cos(f), sin(f), 0);
								glVertex3f(normCircle.x + head.x, normCircle.y + head.y, normCircle.z + z1 * zScaleFactor);
								glVertex3f(normCircle.x + tail.x, normCircle.y + tail.y, normCircle.z + z2 * zScaleFactor);
								f += angleStep;
							}

							glEnd();
							head = tail;
							z1 = z2;
						}
					}
				}
			}
			glLoadName(++flockID);
			map<long, GLuint>::iterator mapEntry = subDisplayLists->find(flockID);
			if (mapEntry == subDisplayLists->end()) {
				GLuint listID = glGenLists(1); // not compiled in the display list, but executed immediately
				(*subDisplayLists)[flockID] = listID;
			}
			glCallList((*subDisplayLists)[flockID]);
		}
	}
	glPopName();
}

void FlockFPMDrawer::drawFootprint() const
{
}

void FlockFPMDrawer::drawXT() const
{
}

void FlockFPMDrawer::drawXY() const
{
}

void FlockFPMDrawer::drawYT() const
{
}

String FlockFPMDrawer::store(const FileName& fnView, const String& parentSection) const {
	String currentSection = parentSection + "::" + getType();
	ComplexDrawer::store(fnView, currentSection);
	return currentSection;
}

void FlockFPMDrawer::load(const FileName& fnView, const String& currentSection){
	ComplexDrawer::load(fnView, currentSection);
}

void FlockFPMDrawer::recomputeFlocks(unsigned int _mu, double _epsilon, unsigned int _delta, Tranquilizer & trq)
{
	bool fRecomputeDisks = (mu != _mu || epsilon != _epsilon);
	bool fRecomputeFlocks = fRecomputeDisks || delta != _delta;
	mu = _mu;
	epsilon = _epsilon;
	delta = _delta;
	if (fRecomputeDisks)
		lfv.flockFinder(timestamps, mu, epsilon, delta, trq);
	if (fRecomputeFlocks) {
		flocks.clear();
		flockIDs.clear();
		trajectoryIDs.clear();
		int numFlock = lfv.checkFlocks(mu, delta, flocks, flockIDs, trajectoryIDs, trq);
		*fRefreshDisplayList = true;
	}
}

map<int, vector<int>> & FlockFPMDrawer::getFlockIDs() {
	return flockIDs;
}

vector<vector<int>> & FlockFPMDrawer::getTrajectoryIDs() {
	return trajectoryIDs;
}

vector<vector<Coord>> & FlockFPMDrawer::getFlocks() {
	return flocks;
}