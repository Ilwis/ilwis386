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

#include "Engine\SpatialReference\csdirect.h"
#include "Engine\Base\DataObjects\ObjectStructure.h"
#include "Engine\Base\objdepen.h"


CoordSystemDirect::CoordSystemDirect(const FileName& fn)
: CoordSystemPtr(fn)
{
  ReadElement("CoordSystemDirect", "CoordSystem", csOther);
  ObjectDependency objdep;
  objdep.Add(csOther);
  objdep.Store(this);
}

CoordSystemDirect::CoordSystemDirect(const FileName& fn, const CoordSystem& csRef)
: CoordSystemPtr(fn, 1),
  csOther(csRef)
{
}

CoordSystemDirect::~CoordSystemDirect()
{
}

void CoordSystemDirect::Store()
{
  CoordSystemPtr::Store();
  WriteElement("CoordSystemDirect", "CoordSystem", csOther);
  ObjectDependency objdep;
  objdep.Add(csOther);
  objdep.Store(this);
}

bool CoordSystemDirect::fConvertFrom(const CoordSystem& cs) const
{
  if (cs->fEqual(*this))
    return true;
  return csOther->fConvertFrom(cs);
}

bool CoordSystemDirect::fConvertTo(const CoordSystemPtr* cs) const
{
  if (cs->fEqual(*this))
    return true;
  return fInverse() && csOther->fConvertTo(cs);
}

Coord CoordSystemDirect::cConv(const CoordSystem& cs, const Coord& crd) const
{
  if (cs->fEqual(*this))
    return crd;
  Coord c = csOther->cConv(cs, crd);
  return cConvFromOther(c);
}

Coord CoordSystemDirect::cInverseConv(const CoordSystemPtr* cs, const Coord& crd) const
{
  if (cs->fEqual(*this))
    return crd;
  Coord c = cConvToOther(crd);
  return csOther->cInverseConv(cs, c);
}

bool CoordSystemDirect::fLatLon2Coord() const
{
  return csOther->fLatLon2Coord();
}

bool CoordSystemDirect::fCoord2LatLon() const
{
  return fInverse() && csOther->fCoord2LatLon();
}

LatLon CoordSystemDirect::llConv(const Coord& crd) const
{
  Coord c = cConvToOther(crd);
  return csOther->llConv(c);
}

Coord CoordSystemDirect::cConv(const LatLon& ll) const
{
  Coord c = csOther->cConv(ll);
  return cConvFromOther(c);
}

bool CoordSystemDirect::fInverse() const
{
  return false;
}

Coord CoordSystemDirect::cConvToOther(const Coord&) const
{
  return Coord();
}

Coord CoordSystemDirect::cConvFromOther(const Coord&) const
{
  return Coord();
}

void CoordSystemDirect::SetCoordSystemOther(const CoordSystem& cs)
{
  csOther = cs;
  Updated();
}

void CoordSystemDirect::GetObjectDependencies(Array<FileName>& afnObjDep)
{
  CoordSystemPtr::GetObjectDependencies(afnObjDep);
  AddFileName(csOther->fnObj, afnObjDep);
}

void CoordSystemDirect::GetObjectStructure(ObjectStructure& os)
{
	CoordSystemPtr::GetObjectStructure( os );
	if ( os.fGetAssociatedFiles() )
	{
		os.AddFile(fnObj, "CoordSystemDirect", "CoordSystem");
	}		
}	
