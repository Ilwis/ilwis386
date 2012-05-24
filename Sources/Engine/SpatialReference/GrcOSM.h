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
	Last change:  WK   11 Jun 98    2:02 pm
*/

#ifndef ILWGROSM_H
#define ILWGROSM_H
#include "Engine\SpatialReference\Grsmpl.h"

typedef bool (ForeignFormat::*RetrieveImageProc)();

class _export GeoRefOSM: public GeoRefCornersWMS
{
  friend class GeoRef;
  friend class GeoRefPtr;
  static GeoRefOSM* create(const FileName& fn, const String& sExpression);
public:
  GeoRefOSM(const FileName& fn, const CoordSystem& cs, 
                MinMax mx, bool fCoc,
                const Coord& cMin, const Coord& cMax)
				: GeoRefCornersWMS(fn,cs,mx,fCoc,cMin, cMax) {}
  GeoRefOSM(const FileName& fn, MinMax mx, bool fCoc,
                const Coord& cMin, const Coord& cMax)
    : GeoRefCornersWMS(fn, mx, fCoc, cMin, cMax)
    { }
  static const char* sSyntax();
  virtual String sType() const;
  virtual void Store();
  void setLocatBounds(const CoordSystem& csy,const CoordBounds& bnd);
  CoordBounds getLocalBounds() const;
  String sName(bool fExt, const String& sDirRelative) const;
protected:
	GeoRefOSM(const FileName& fn) ;
	CoordBounds localBounds;
};


#endif // ILWGRCORNERS_H




