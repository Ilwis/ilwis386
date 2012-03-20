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
/* GeoRefCorners
   Copyright Ilwis System Development ITC
   march 1995, by Wim Koolhoven
	Last change:  WK   11 Jun 98    2:02 pm
*/

#ifndef ILWGRCORNERS_H
#define ILWGRCORNERS_H
#include "Engine\SpatialReference\Grsmpl.h"

class GeoRefCorners: public GeoRefSmpl
{
  friend class GeoRef;
  friend class GeoRefPtr;
  static GeoRefCorners* create(const FileName& fn, const String& sExpression);
public:
  GeoRefCorners(const FileName& fn, const CoordSystem& cs, 
                RowCol rc, bool fCoc,
                const Coord& cMin, const Coord& cMax)
    : GeoRefSmpl(fn,cs,rc), fCornersOfCorners(fCoc),
      crdMin(cMin), crdMax(cMax)
    { Compute(); }
  GeoRefCorners(const FileName& fn, RowCol rc, bool fCoc,
                const Coord& cMin, const Coord& cMax)
    : GeoRefSmpl(fn, CoordSystem(), rc), fCornersOfCorners(fCoc),
      crdMin(cMin), crdMax(cMax)
    { _rc = rc; Compute(); }
  static const char* sSyntax();
  virtual String _export sName(bool fExt = false, const String& sDirRelative = "") const;
  virtual String _export sType() const;
  virtual void _export Store();
  virtual void _export Rotate(bool fSwapRows, bool fSwapCols, bool fRotate);
  virtual bool _export fNorthOriented() const;
  bool _export isEqual(const GeoRef& grf) const ;
protected:
  GeoRefCorners(const FileName&);
//  virtual void Enlarge(double rEnlFac);
//private:
public:
  void _export Compute();
  bool fCornersOfCorners;  // false = center of cornerpixels
  Coord crdMin, crdMax;
};


#endif // ILWGRCORNERS_H




