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
 Bas Restsios, Martin Schouwenburg, Lichun Wang, Jelle Wind 

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
/* GeoRefOSM
   Copyright Ilwis System Development ITC
   march 1995, by Wim Koolhoven
	Last change:  WK   11 Jun 98    2:03 pm
*/
#include "Engine\SpatialReference\Gr.h"
#include "Engine\SpatialReference\Grsmpl.h"
#include "Engine\SpatialReference\Grcornrs.h"
#include "Engine\Table\tbl.h"
#include "Engine\DataExchange\ForeignFormat.h"
#include "Engine\SpatialReference\GrcWMS.h"
#include "Engine\SpatialReference\GrcOSM.h"

const char* GeoRefOSM::sSyntax()
{
  return "GeoRefOSM(rows,cols,cornersofcorners,minx,miny,maxx,maxy)\ncornersofcorners=0,1";
}

GeoRefOSM::GeoRefOSM(const FileName& fn) 
: GeoRefCornersWMS(fn)
{
}

String GeoRefOSM::sType() const
{
  return "GeoReference OSM WMS";
}

String GeoRefOSM::sName(bool fExt, const String& sDirRelative) const
{
  String s = IlwisObjectPtr::sName(fExt, sDirRelative);
  if (s.length() != 0)
    return s;
  return String("GeoRefOSM(%li,%li,%i,%g,%g,%g,%g)", 
                 this->rcSize().Row, rcSize().Col, fCornersOfCorners,
                 crdMin.x, crdMin.y, crdMax.x, crdMax.y);
}

void GeoRefOSM::Store()
{
	// never store anything
}
void GeoRefOSM::setLocatBounds(const CoordSystem& csySource, const CoordBounds& bnd){
	if ( ! csySource->pcsLatLon()) {
		localBounds = cs()->cbConv(csySource, bnd);
	} else
		localBounds = bnd;
}

CoordBounds GeoRefOSM::getLocalBounds() const{
	if ( localBounds.fValid())
		return localBounds;
	return cs()->cb;
}





