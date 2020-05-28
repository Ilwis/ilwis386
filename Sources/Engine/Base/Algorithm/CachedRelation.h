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

 Created on: 2020-05-27
 ***************************************************************/

#ifndef CACHEDRELATION_H
#define CACHEDRELATION_H

#include <Geos.h>
#include <map>

class CachedRelation
{
public:
	CachedRelation() {};
	bool within(Geometry * g1, Geometry * g2) {
		std::map<std::pair<Geometry*,Geometry*>,bool>::iterator item = cachedWithin.find(std::pair<Geometry*,Geometry*>(g1,g2));
		if (item != cachedWithin.end()) {
			return item->second;
		} else {
			std::map<std::pair<Geometry*,Geometry*>,bool>::iterator item = cachedWithin.find(std::pair<Geometry*,Geometry*>(g2,g1));
			if (item != cachedWithin.end()) {
				bool within = item->second;
				if (within) { // return false, unless equal
					within = g2->equalsExact(g1); // two equal geometries are always "within" eachother
					cachedWithin[std::pair<Geometry*,Geometry*>(g1,g2)] = within;
					return within;
				}
			}
			bool within = g1->within(g2);
			cachedWithin[std::pair<Geometry*,Geometry*>(g1,g2)] = within;
			return within;
		}
	};
private:
	std::map<std::pair<Geometry*,Geometry*>,bool> cachedWithin;
};

#endif // CACHEDRELATION_H