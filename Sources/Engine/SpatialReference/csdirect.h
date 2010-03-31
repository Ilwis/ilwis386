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
/* CoordSystemDirect
   Copyright Ilwis System Development ITC
   april 1998, by Wim Koolhoven
	Last change:  WK   17 Apr 98   12:08 pm
*/

#ifndef ILWCSDIRECT_H
#define ILWCSDIRECT_H
#include "Engine\SpatialReference\Coordsys.h"

class CoordSystemDirect: public CoordSystemPtr
{
public:
  CoordSystemDirect(const FileName&);
  CoordSystemDirect(const FileName&, const CoordSystem& csRef);
  _export ~CoordSystemDirect();
  virtual void _export Store();
  virtual bool _export fConvertFrom(const CoordSystem&) const;
  virtual bool _export fConvertTo(const CoordSystemPtr*) const;
  virtual Coord _export cConv(const CoordSystem&, const Coord&) const;
  virtual Coord _export cInverseConv(const CoordSystemPtr*, const Coord&) const;
  virtual bool _export fLatLon2Coord() const;
  virtual bool _export fCoord2LatLon() const;
  virtual LatLon _export llConv(const Coord&) const;
  virtual Coord _export cConv(const LatLon&) const;
	virtual void  GetObjectDependencies(Array<FileName>& afnObjDep);
  void SetCoordSystemOther(const CoordSystem&);
	void GetObjectStructure(ObjectStructure& os)	;
  CoordSystem csOther;
protected:
  virtual bool _export fInverse() const;
  virtual Coord _export cConvFromOther(const Coord&) const;
  virtual Coord _export cConvToOther(const Coord&) const;
};

#endif // ILWCSDIRECT_H



