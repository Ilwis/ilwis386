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
// Disk.h: interface for the Disk class.
//
//////////////////////////////////////////////////////////////////////

#ifndef DISK_H
#define DISK_H

#include "Point2d.h"

#include <vector>
#include <set>
using namespace std;

class Disk {

public:
	Disk(const Point2d & centre);
	void addPoint(const Point2d & point);
	//vector<int> getPointsIDs() const;
	const double getX() const;
	const double getY() const;
	const bool isWithinDistance(const Point2d & other, double distance) const;
	const bool operator == (const Disk & d) const;
	const bool operator < (const Disk & d) const;
	const bool isSubset() const;
	void setSubset(bool subset);
	unsigned int count;
	vector<Point2d> points;
	set<int> pointsIDs;

private:
	Point2d centre;
	bool subset;
};

#endif // DISK_H