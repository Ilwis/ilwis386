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
/* $Log: /ILWIS 3.0/GeoReference/Grsub.cpp $
 * 
 * 12    13-12-02 18:36 Hendrikse
 * implemented virtual bool fGeoRefNone() const;
 * 
 * 11    11/26/01 12:33 Willem
 * Getting the object structure for dependent data of an object is now
 * handled by IlwisObject instead of the individual objects
 * 
 * 10    13-08-01 17:05 Koolhoven
 * GeoRefSubMap, Factor and MirrorRotate now have a fDependent() function
 * which always returns true. Ensuring that they will have a Dependeny tab
 * in the property form
 * 
 * 9     24-01-01 12:40p Martin
 * related georef is also copied
 * 
 * 8     18-10-00 18:35 Hendrikse
 * added 2 calls to SetTopLeft  in CalcSize
 * 
 * 7     23-02-00 12:40 Wind
 * bug in georefsubmapcorners (size was not calculated correctly and
 * offset was set to (0,0))
 * 
 * 6     24/01/00 17:33 Willem
 * The GeoRefSubMap::create function now reads the correct ODF section to
 * determine the GRSM type
 * 
 * 5     21/01/00 16:17 Willem
 * - Changed "BottomLeft" to "BottomRight"
 * - The Store() function of GRSubCorners is now correct
 * - The SetCrd* functions in GRSubCoords now both recalculate the
 * internal
 *   variables
 * 
 * 4     22-12-99 10:29 Wind
 * error in GeoRefSubMapCorners
 * 
 * 3     23-11-99 12:58 Wind
 * added georefsubmapcorners and gerorefsubmapcoords
 * 
 * 2     19/08/99 17:28 Willem
 * Extended the interface with getter/setter functions
*/
// Revision 1.3  1998/09/16 17:24:43  Wim
// 22beta2
//
// Revision 1.2  1997/08/25 08:41:02  Wim
// Added ObjectDependency in Store() to be sure that Copy() works
// properly
//
/* GeoRefSubMap
   Copyright Ilwis System Development ITC
   may 1996, by Jelle Wind
	Last change:  WK   11 Jun 98    2:04 pm
*/
#include "Engine\SpatialReference\Gr.h"
#include "Engine\SpatialReference\Grsub.h"
#include "Engine\Base\DataObjects\ObjectStructure.h"
#include "Engine\Base\objdepen.h"

GeoRefSubMap::GeoRefSubMap(const FileName& fn)
: GeoRefPtr(fn)
{
  ReadElement("GeoRefSubMap", "GeoRef", gr);
  if (!gr.fValid())
    gr = GeoRef(rcSize());
  String smr;  
  ReadElement("GeoRefSubMap", "Start", rcStart);
}

GeoRefSubMap::GeoRefSubMap(const FileName& fn, const GeoRef& grf, RowCol rcStrt, RowCol rcSize)
: GeoRefPtr(fn, grf->cs(), rcSize), rcStart(rcStrt),
  gr(grf)
{
}
GeoRefPtr* GeoRefSubMap::create(const FileName& filnam)
{
  String sType;
  ObjectInfo::ReadElement("GeoRefSubMap", "Type", filnam, sType);
  if (fCIStrEqual("GeoRefSubMapCorners" , sType))
    return new GeoRefSubMapCorners(filnam);
  if (fCIStrEqual("GeoRefSubMapCoords" , sType))
    return new GeoRefSubMapCoords(filnam);
  return new GeoRefSubMap(filnam);
}

String GeoRefSubMap::sType() const
{
  return "GeoReference SubMap";
}

void GeoRefSubMap::Store()
{
  GeoRefPtr::Store();
  WriteElement("GeoRef", "Type", "GeoRefSubMap");
  WriteElement("GeoRefSubMap", "GeoRef", gr);
  WriteElement("GeoRefSubMap", "Start", rcStart);
  ObjectDependency objdep;
  objdep.Add(gr);
  objdep.Store(this);
}

RowCol GeoRefSubMap::rcTopLeft() const
{
	return rcStart;
}

void GeoRefSubMap::SetTopLeft(RowCol rcTopLeft)
{
	rcStart = rcTopLeft;
}

void GeoRefSubMap::SetRowCol(RowCol rcSize)
{
	GeoRefPtr::SetRowCol(rcSize);
}

void GeoRefSubMap::Coord2RowCol(const Coord& c, double& rRow, double& rCol) const
{
  gr->Coord2RowCol(c, rRow, rCol);
  rRow -= rcStart.Row;
  rCol -= rcStart.Col;
}

void GeoRefSubMap::RowCol2Coord
  (double rRow, double rCol, Coord& c) const
{
  rRow += rcStart.Row;
  rCol += rcStart.Col;
  gr->RowCol2Coord(rRow, rCol, c);
}

double GeoRefSubMap::rPixSize() const
{
  return gr->rPixSize();
}

bool GeoRefSubMap::fEqual(const IlwisObjectPtr& ptr) const
{
  if (!GeoRefPtr::fEqual(ptr))
    return false;
  const GeoRefSubMap* grsb = dynamic_cast<const GeoRefSubMap*>(&ptr);
  if (0 == grsb)
    return false;
  return (rcStart == grsb->rcStart) && (gr == grsb->gr);
}

bool GeoRefSubMap::fLinear() const
{
  return gr->fLinear();
}

bool GeoRefSubMap::fNorthOriented() const
{
  return gr->fNorthOriented();
}

void GeoRefSubMap::GetObjectStructure(ObjectStructure& os)
{
	GeoRefPtr::GetObjectStructure( os );
	if ( os.fGetAssociatedFiles() )
	{
		os.AddFile(fnObj, "GeoRefSubMap", "GeoRef");
	}		
	
}

bool GeoRefSubMap::fDependent() const
{
  return true;
}

bool GeoRefSubMap::fGeoRefNone() const
{
   return gr->fGeoRefNone();
}

void GeoRefSubMap::BreakDependency()
{
	if (gr->pgc()) {
		FileName fnTemp = FileName::fnUnique(fnObj);
		GeoRef grfNew (fnTemp, String("GeoRefCorners(%li,%li,1,%g,%g,%g,%g)", rcSize().Row, rcSize().Col, cb().cMin.x, cb().cMin.y, cb().cMax.x, cb().cMax.y)); // cb() results by definition in a corners-of-corners GeoRef
		grfNew->SetCoordSystem(gr->cs());
		grfNew->Store();
		MoveFileEx(fnTemp.sFullPath().c_str(), fnObj.sFullPath().c_str(), MOVEFILE_REPLACE_EXISTING);
	}
}

//-------------------------------------------------------------------------

GeoRefSubMapCorners::GeoRefSubMapCorners(const FileName& fn)
: GeoRefSubMap(fn)
{
  ReadElement("GeoRefSubMapCorners", "BottomRight", _rcBotRight);
}

GeoRefSubMapCorners::GeoRefSubMapCorners(const FileName& fn, const GeoRef& grf, RowCol rcStart, RowCol rcbl)
: GeoRefSubMap(fn, grf, rcStart, RowCol(rcbl.Row-rcStart.Row+1, rcbl.Col-rcStart.Col+1)),
  _rcBotRight(rcbl)
{
}

String GeoRefSubMapCorners::sType() const
{
  return "GeoReference SubMapCorners";
}

void GeoRefSubMapCorners::Store()
{
  GeoRefSubMap::Store();
  WriteElement("GeoRefSubMap", "Type", "GeoRefSubMapCorners");
  WriteElement("GeoRefSubMapCorners", "BottomRight", _rcBotRight);
}
  

bool GeoRefSubMapCorners::fEqual(const IlwisObjectPtr& ptr) const
{
  if (!GeoRefPtr::fEqual(ptr))
    return false;
  const GeoRefSubMapCorners* grsb = dynamic_cast<const GeoRefSubMapCorners*>(&ptr);
  if (0 == grsb)
    return false;
  return (rcTopLeft() == grsb->rcTopLeft()) && (_rcBotRight == grsb->rcBotRight()) && (gr == grsb->gr);
}


GeoRefSubMapCoords::GeoRefSubMapCoords(const FileName& fn)
: GeoRefSubMap(fn)
{
  ReadElement("GeoRefSubMapCoords", "Coord1", _crd1);
  ReadElement("GeoRefSubMapCoords", "Coord2", _crd2);
  CalcSize(_crd1, _crd2);
}

GeoRefSubMapCoords::GeoRefSubMapCoords(const FileName& fn, const GeoRef& grf, const Coord& crd1, const Coord& crd2)
: GeoRefSubMap(fn, grf, RowCol(0,0), RowCol(0, 0)), _crd1(crd1), _crd2(crd2)
{
  CalcSize(_crd1, _crd2);
}

String GeoRefSubMapCoords::sType() const
{
  return "GeoReference SubMapCoords";
}

void GeoRefSubMapCoords::Store()
{
  GeoRefSubMap::Store();
  WriteElement("GeoRefSubMap", "Type", "GeoRefSubMapCoords");
  WriteElement("GeoRefSubMapCoords", "Coord1", _crd1);
  WriteElement("GeoRefSubMapCoords", "Coord2", _crd2);
}


void GeoRefSubMapCoords::CalcSize(const Coord& crd1, const Coord& crd2)
{
	SetTopLeft(RowCol(0,0));
  RowCol rc1 = rcConv(crd1);
  RowCol rc2 = rcConv(crd2);
  RowCol rcOpp, rcSiz, rcOffset;
  rcOffset.Row = min(rc1.Row, rc2.Row);
  rcOffset.Col = min(rc1.Col, rc2.Col);
  rcOpp.Row = max(rc1.Row, rc2.Row);
  rcOpp.Col = max(rc1.Col, rc2.Col);
  rcSiz.Row = rcOpp.Row - rcOffset.Row;
  rcSiz.Col = rcOpp.Col - rcOffset.Col;
  SetTopLeft(rcOffset);
  SetRowCol(rcSiz);
}

bool GeoRefSubMapCoords::fEqual(const IlwisObjectPtr& ptr) const
{
  if (!GeoRefPtr::fEqual(ptr))
    return false;
  const GeoRefSubMapCoords* grsb = dynamic_cast<const GeoRefSubMapCoords*>(&ptr);
  if (0 == grsb)
    return false;
  return (_crd1 == grsb->crd1()) && (_crd2 == grsb->crd2()) && (gr == grsb->gr);
}

void GeoRefSubMapCoords::SetCrd1(const Coord& crd) 
{
	_crd1 = crd; 
	CalcSize(_crd1, _crd2);
}

void GeoRefSubMapCoords::SetCrd2(const Coord& crd)
{
	_crd2 = crd;
	CalcSize(_crd1, _crd2);
}
