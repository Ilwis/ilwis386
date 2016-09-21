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
// LCMFlockViz.h: interface for the LCMFlockViz class.
//
//////////////////////////////////////////////////////////////////////

#ifndef LCMFLOCKVIZ_H
#define LCMFLOCKVIZ_H

#include "Point2d.h"
#include "DiskInfo.h"
//#include "Index.h"
#include "Disk.h"

#include <vector>
#include <map>
#include <set>
using namespace std;

class DiskCentres {
public:
	DiskCentres(Point2d & dc1, Point2d & dc2)
	: diskCentre1(dc1)
	, diskCentre2(dc2)
	, fValid(true)
	{
	};
	DiskCentres()
	: diskCentre1(Point2d(0, 0))
	, diskCentre2(Point2d(0, 0))
	, fValid(false)
	{
	};

	const Point2d diskCentre1;
	const Point2d diskCentre2;
	const bool fValid;
};

class LCMFlockViz
{

public:
	LCMFlockViz();
	unsigned int readPoints(char * filename, map<int, vector<Point2d>> & timestamps);
	void flockFinder(const map<int, vector<Point2d>> & timestamps, unsigned int mu, double epsilon, unsigned int delta, Tranquilizer & trq);
	int checkFlocks(unsigned int mu, unsigned int delta, vector<vector<Coord>> & flocks, map<int, vector<int>> & flockIDs, vector<vector<int>> & trajectoryIDs, Tranquilizer & trq);

private:
	void getGrid(vector<Point2d> & points, map<int, map<int, vector<Point2d>>> & grid, double epsilon) const;
	const DiskCentres calculateDisks(const Point2d & p1, const Point2d & p2, const double r2) const;
	void getDisks(map<int, map<int, vector<Point2d>>> & grid, unsigned int mu, double epsilon, int time, map<int, vector<int>> & database, vector<DiskInfo> & dbdisks);
	const double precision;
	unsigned int cid;
	map<int, vector<int>> database;
	vector<DiskInfo> dbdisks;
};

#endif // LCMFLOCKVIZ_H