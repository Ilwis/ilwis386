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
	Last change:  JEL   1 Jan 97    9:04 pm
*/

#include "Headers\toolspch.h"
#include "Engine\Domain\dmcoord.h"

DomainCoord::DomainCoord(const FileName& fn)
: DomainPtr(fn),
  threeD(false)
{
//  ReadElement("DomainCoord", "CoordSystem", csy);
  csy = CoordSystem(fn);
  _iWidth = csy->iWidth();
  _fRawAvail = false;
   ReadElement("DomainCoord","3D",threeD);
}

DomainCoord::DomainCoord(const FileName& fn, const CoordSystem& cs, bool _t3d)
: DomainPtr(fn, true), csy(cs),threeD(_t3d)
{
  _fRawAvail = false;
  fChanged = false;
  _iWidth = csy->iWidth();
  if (_iWidth <= 0)
    _iWidth = 29;
}

String DomainCoord::sType() const
{
  return "Domain Coord";
}

void DomainCoord::Store()
{
  DomainPtr::Store();
  WriteElement("Domain", "Type", "DomainCoord");
  WriteElement("Domain", "Width", (long)iWidth());
  WriteElement("DomainCoord","3D",threeD);
  if (csy->fChanged)
    csy->Store();
}

bool DomainCoord::fEqual(const IlwisObjectPtr& ptr) const
{
  const DomainCoord* dcc = dynamic_cast<const DomainCoord*>(&ptr);
  if (0 == dcc)
    return false;
  else if (&ptr == this)
    return true;
  DomainCoord* dc = const_cast<DomainCoord*>(dcc);
  CoordSystem cs1 = cs();
  CoordSystem cs2 = dc->cs();
  if (cs1.fValid() && cs2.fValid())
    return cs1 == cs2;
  return false;
}

bool DomainCoord::f3D() const {
	return threeD;
}
void  DomainCoord::set3D(bool yesno) {
	threeD = yesno;
}
StoreType DomainCoord::stNeeded() const
{ 
	if ( threeD)
		return stCRD3D;
	return stCRD; 
}

bool DomainCoord::fValid(const String& s) const
{
  return !cValue(s).fUndef();
}

const CoordSystem& DomainCoord::cs() const
{
  return csy;
}

void DomainCoord::SetCoordSystem(const CoordSystem& cs)
{
  if (csy != cs) {
    csy = cs;
    Updated();
  }  
}
String DomainCoord::sValue(const Coord& c, short wid, short dec) const
{
  if (wid < 0)
    wid = iWidth();
  return cs()->sValue(c, wid, dec);
}

Coord DomainCoord::cValue(const String& s) const
{
	if (csy.fValid())
    return csy->cValue(s);
	else
		return Coord();
}

bool DomainCoord::fConvertFrom(const CoordSystem& csy) const
{
	if (csy.fValid())
    return csy->fConvertFrom(csy);
	else
		return false;
}

Coord DomainCoord::cConv(const CoordSystem& csy, const Coord& crd) const
{
	if (csy.fValid())
	  return csy->cConv(csy, crd);
	else
		return Coord();
}

String DomainCoord::sName(bool fExt, const String& sDirRelative) const
{
  String s = IlwisObjectPtr::sName(fExt, sDirRelative);
  if (s.length() != 0)
    return s;
	if (csy.fValid())
	  return csy->sName(true, sDirRelative);
	else
		return s;
}

CoordSystem DomainCoord::csyCoordSys() const
{
	return csy;
}







