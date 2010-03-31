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
/* DomainCoord
   Copyright Ilwis System Development ITC
   oct 1996, by Jelle Wind
	Last change:  JEL  10 Dec 96   10:18 am
*/

#ifndef ILWDOMCOORD_H
#define ILWDOMCOORD_H
#include "Engine\Domain\dm.h"
#include "Engine\SpatialReference\Coordsys.h"

class DomainCoord: public DomainPtr
{
  friend class DomainPtr;
protected:
  virtual void Store();
  virtual bool fEqual(const IlwisObjectPtr& ptr) const;
public:
  _export DomainCoord(const FileName& fn);
  DomainCoord(const FileName& fn, const CoordSystem& csy,bool _t3d=false);
  virtual String sType() const;
  virtual StoreType stNeeded() const;
  bool fValid(const String&) const;
  const CoordSystem _export &cs() const;
  void SetCoordSystem(const CoordSystem& cs);
  String _export sValue(const Coord&, short iWidth=-1, short iDec=-1) const;
  Coord _export cValue(const String&) const;
  bool fConvertFrom(const CoordSystem&) const;
  Coord cConv(const CoordSystem&, const Coord&) const;
  virtual String sName(bool fExt, const String& sDirRelative) const;
	CoordSystem _export csyCoordSys() const;
	bool f3D() const;
	void set3D(bool yesno);
private:
  CoordSystem csy;
  bool threeD;
};

#endif // ILWDOMPICTURE_H





