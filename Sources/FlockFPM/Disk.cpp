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
// Disk.cpp.h : implementation file
//
//////////////////////////////////////////////////////////////////////

#include "Disk.h"

Disk::Disk(const Point2d & _centre)
: centre(_centre)
, count(0)
, subset(false)
{
}

void Disk::addPoint(const Point2d & point) {
	points.push_back(point);
	pointsIDs.insert(point.getUserData());
	++count;
}
/*
vector<int> Disk::getPointsIDs() const {
	vector<int> pointsIDs;
	for (vector<Point2d>::const_iterator it = points.begin(); it != points.end(); ++it)
		pointsIDs.push_back(it->getUserData());
	return pointsIDs;
}
*/

const double Disk::getX() const {
	return centre.getX();
}

const double Disk::getY() const {
	return centre.getY();
}

const bool Disk::isWithinDistance(const Point2d & other, double distance) const {
	return centre.isWithinDistance(other, distance);
}

const bool Disk::operator == (const Disk & d) const {
	return ((centre.getX() == d.centre.getX()) && (centre.getY() == d.centre.getY()));
}

const bool Disk::operator < (const Disk & d) const {
	if (centre.getX() < d.centre.getX())
		return true;
	else if (centre.getX() > d.centre.getX())
		return false;
	else if (centre.getY() < d.centre.getY())
		return true;
	else
		return false;
}

const bool Disk::isSubset() const {
	return subset;
}

void Disk::setSubset(bool _subset) {
	subset = _subset;
}

