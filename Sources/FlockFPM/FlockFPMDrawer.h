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
// FlockFPMDrawer.h: interface for the FlockFPMDrawer class.
//
//////////////////////////////////////////////////////////////////////

#pragma once
#include "SpaceTimeCube\SpaceTimeDrawer.h"
#include "Point2d.h"

ILWIS::NewDrawer *createFlockFPMDrawer(ILWIS::DrawerParameters *parms);

namespace ILWIS {

	class SpaceTimePathDrawer;

	class _export FlockFPMDrawer : public SpaceTimeDrawer {
	public:
		ILWIS::NewDrawer *createFlockFPMDrawer(DrawerParameters *parms);

		FlockFPMDrawer(DrawerParameters *parms);
		virtual ~FlockFPMDrawer();
		void prepare(PreparationParameters *pp);
		void setSpaceTimePathDrawer(SpaceTimePathDrawer * _stpdrw);
		void setSampleInterval(double _rSampleInterval);
		void recomputeFlocks(unsigned int _mu, double _epsilon, unsigned int _delta, Tranquilizer & trq);
		map<int, vector<int>> & getFlockIDs();
		vector<vector<int>> & getTrajectoryIDs();
		vector<vector<Coord>> & getFlocks();
		const double getSampleInterval() const;
		const unsigned int getMu() const;
		const double getEpsilon() const;
		const unsigned int getDelta() const;
		virtual void drawFootprint() const;
		virtual void drawXT() const;
		virtual void drawXY() const;
		virtual void drawYT() const;
	protected:
		String store(const FileName& fnView, const String& parentSection) const;
		void load(const FileName& fnView, const String& currentSection);
		virtual vector<GLuint> getObjectIDs(vector<long> & iRaws) const;
		virtual int getNearestEnabledObjectIDIndex(vector<GLuint> & objectIDs) const;
		virtual vector<GLuint> getEnabledObjectIDs(vector<GLuint> & objectIDs) const;
		virtual Feature * getFeature(GLuint objectID) const;
		virtual void getRaws(GLuint objectID, vector<long> & raws) const;
		virtual String getInfo(const Coord& c) const;
		virtual void drawObjects(const int steps, GetHatchFunc getHatchFunc) const;
		void SamplePoints(const vector<Feature *> & features, const long iStart, const long iEnd, const int id, const Column & colTime, const CoordSystem & csy);
		map<int, vector<Point2d>> timestamps; // map <time, trajectory>
		//map<int, double> timemap; // map <time(int), timeoffset>
		vector<vector<Coord>> flocks;
		map<int, vector<int>> flockIDs;
		map<int, GLuint> trajectoryIDToGLuint;
		vector<vector<int>> trajectoryIDs;
		SpaceTimePathDrawer *stpdrw;
		LCMFlockViz lfv;
	private:
		unsigned int mu;
		double epsilon;
		unsigned int delta;
		double rSampleInterval;
		const double textureOffset;
		const double textureRange;
	};
}