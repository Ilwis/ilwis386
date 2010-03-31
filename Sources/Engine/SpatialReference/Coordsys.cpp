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
/*$Log: /ILWIS 3.0/CoordinateSystem/Coordsys.cpp $
 * 
 * 16    8-09-00 3:43p Martin
 * added function for setting the update status and getting the
 * objectstructure
 * 
 * 15    28/08/00 14:55 Willem
 * the fUnknown() function now checks case insensitive
 * 
 * 14    8-05-00 18:22 Hendrikse
 * raised the default in the constr of CoordSystemPtr:
 *  _iWidth = 28;
 * I am not sure wether it makes a difference in practice
 * 
 * 13    3-03-00 4:32p Martin
 * corrected check for which coordsystem is present
 * 
 * 12    21/02/00 9:53 Willem
 * Removed the CoordSystem Unknown check again, because this breaks
 * existing code
 * 
 * 11    7-12-99 12:22 Wind
 * lock around creation of CoordSystem unknown.csy
 * 
 * 10    3-11-99 12:23 Hendrikse
 * Adde checks if (fUnknown() || cs->fUnknown())
 * 		return false; in both fConvertTo() and fConvertFrom()
 * 
 * 9     29-10-99 12:56 Wind
 * case sensitive stuff
 * 
 * 8     22-10-99 12:56 Wind
 * thread save access (not yet finished)
 * 
 * 7     13-10-99 11:47 Hendrikse
 * added coordsystemDirectLinear 3 times
 * 
 * 6     1-10-99 17:58 Hendrikse
 * replaced CoordSystemTiePoints by CoordSystemCTP (typing error)
 * 
 * 5     29-09-99 13:28 Hendrikse
 * added #includes CoordSystemTiePoints.h and CoordSystemOrthoPhoto.h
 * 
 * 4     7-09-99 3:34p Martin
 * added 2.22 changes
 * 
 * 3     3/15/99 3:36p Martin
 * 
 * 2     3/11/99 12:16p Martin
 * Added support for Case insesitive 
//Revision 1.8  1998/10/12 10:49:50  Wim
//Show too large coordinates in sValue() with %g instead of %f
//to prevent that ridiculous coordinates look awful.
//
//Revision 1.7  1998-09-16 18:22:46+01  Wim
//22beta2
//
//Revision 1.6  1997/11/28 18:01:20  janh.ilwis.itc
//I made the last else condition in cConv(CoordSys&) stricter so that no datum shift can happen
//whenever the input or output coordsys is LatLon because it has no sense in the current ilwis
//For that we should implement the possibility of different latlon systems
//
//Revision 1.5  1997/11/21 12:24:36  janh
//*** empty log message ***
//
//Revision 1.4  1997/11/20 10:30:11  martin
//The second create function did not check in the system directory
//
//Revision 1.3  1997/09/22 12:15:15  Wim
//In sValue() also use dec if wid=0
//
//Revision 1.2  1997-09-12 18:36:50+02  janh
//Removed pl.Adjustlon() from cConv to detect UNDEF crds in DrawGraticule
//
/* CoordSystem, CoordSystemPtr
   Copyright Ilwis System Development ITC
   october 1995, by Wim Koolhoven
	Last change:  J    21 Oct 99   10:03 am
*/

#include "Engine\SpatialReference\Coordsys.h"
#include "Engine\SpatialReference\csbonly.h"
#include "Engine\SpatialReference\csctp.h"
#include "Engine\SpatialReference\CoordSystemTiePoints.H"
#include "Engine\SpatialReference\csformul.h"
#include "Engine\SpatialReference\Csproj.h"
#include "Engine\SpatialReference\Cslatlon.h"
#include "Engine\SpatialReference\Csdiff.H"
#include "Engine\SpatialReference\CoordSystemOrthoPhoto.h"
#include "Engine\SpatialReference\CoordSystemDirectLinear.h"
#include "Engine\Base\System\mutex.h"
#include "Engine\Applications\objvirt.h"
#include "Engine\Applications\ModuleMap.h"
#include "Engine\Base\System\Engine.h"


IlwisObjectPtrList CoordSystem::listCS;


CoordSystem::CoordSystem()
: IlwisObject(listCS) //, FileName(String("unknown.csy"), FileName::fnPathOnly(getEngine()->getContext()->sStdDir())))
{
  ILWISSingleLock sl(&listCS.csAccess, TRUE, SOURCE_LOCATION); // prevent other threads from entering
  FileName fn = FileName(String("unknown.csy"), FileName::fnPathOnly(getEngine()->getContext()->sStdDir()));
  CoordSystem cs(fn);
  SetPointer(cs.ptr());
}

CoordSystem::CoordSystem(const CoordSystem& cs)
: IlwisObject(listCS, cs.pointer())
{
}

CoordSystem::CoordSystem(const FileName& filnam)
: IlwisObject(listCS)
{
  FileName fn = FileName(filnam, ".csy");
  CoordSystemPtr* p = CoordSystem::pGet(fn);
  if (p) { // if already open return it
    SetPointer(p);
    return;
  }
  MutexFileName mut(fn);
  p = CoordSystem::pGet(fn);
  if (p)  // if already open return it
    SetPointer(p);
  else
    SetPointer(CoordSystemPtr::create(fn));
}

CoordSystem::CoordSystem(const FileName& fn, const String&)
: IlwisObject(listCS)
{
  SetPointer(new CoordSystemPtr(FileName(fn, ".csy"),0));
}

CoordSystem::CoordSystem(const char* sExpr)
: IlwisObject(listCS)
{
  if (fCIStrEqual(sExpr , "Differential"))
    SetPointer(new CoordSystemDifferential());
  else {
    FileName fn = sExpr;
    fn.sExt = ".csy";
    CoordSystem cs(fn);
    SetPointer(cs.ptr());
  }
}

CoordSystem::CoordSystem(const String& sExpr)
: IlwisObject(listCS)
{
  if (fCIStrEqual(sExpr , "Differential"))
    SetPointer(new CoordSystemDifferential());
  else {
    FileName fn = sExpr;
    fn.sExt = ".csy";
    CoordSystem cs(fn);
    SetPointer(cs.ptr());
  }
}

CoordSystem::CoordSystem(const String& sExpr, const String& sPath)
: IlwisObject(listCS, CoordSystemPtr::create(FileName::fnPathOnly(sPath),sExpr))
{
  FileName fn = FileName(sExpr);
  fn.Dir(sPath);
  CoordSystem cs(fn);
  SetPointer(cs.ptr());
}

CoordSystemPtr* CoordSystem::pGet(const FileName& fn)
{
  return static_cast<CoordSystemPtr*>(listCS.pGet(fn));
}

CoordSystemPtr* CoordSystemPtr::create(const FileName& fn)
{
  FileName filnam = fn;
  filnam.sExt = ".csy";
  if (!File::fExist(filnam)) { // check std dir
    filnam.Dir(getEngine()->getContext()->sStdDir());
    if (!File::fExist(filnam))
      NotFoundError(fn);
  }
  CoordSystemPtr* p = CoordSystem::pGet(filnam);
  if (p) // if already open return it
    return p;
  MutexFileName mut(fn);
  p = CoordSystem::pGet(fn);
  if (p) // if already open return it
    return p;

  String sType = "Projection";
  ObjectInfo::ReadElement("CoordSystem", "Type", filnam, sType);
  if (fCIStrEqual("BoundsOnly" , sType))
    return new CoordSystemBoundsOnly(filnam);
  if (fCIStrEqual("Projection" , sType))
    return new CoordSystemProjection(filnam);
  if (fCIStrEqual("LatLon" , sType))
    return new CoordSystemLatLon(filnam);
  if (fCIStrEqual("Formula" , sType))
    return new CoordSystemFormula(filnam);
  if (fCIStrEqual("TiePoints" , sType))
    return new CoordSystemTiePoints(filnam);
  if (fCIStrEqual("OrthoPhoto" , sType))
    return new CoordSystemOrthoPhoto(filnam);
  if (fCIStrEqual("DirectLinear" , sType))
    return new CoordSystemDirectLinear(filnam);

  InvalidTypeError(fn, "CoordSystem", sType);
  return 0;
}

CoordSystemPtr* CoordSystemPtr::create(const FileName& fn, const String& sExpression)
{
  if (sExpression == "Differential")
    return new CoordSystemDifferential();
  if (fn.sFile.length() == 0) { // no file name
    // check if sExpression is an existing coordsys on disk
    FileName fnCsy(sExpression, ".csy", true);
    if (File::fExist(fnCsy)) { 
      CoordSystemPtr* p = CoordSystem::pGet(fnCsy);
      if (p) // if already open return it
        return p;
      return CoordSystemPtr::create(fnCsy);
    }
    fnCsy.Dir(getEngine()->getContext()->sStdDir());
    if (!File::fExist(fnCsy))
      NotFoundError(fn);
    CoordSystemPtr* p = CoordSystem::pGet(fnCsy);
    if (p) 
      return p;
    return CoordSystemPtr::create(fnCsy);
  }
  NotFoundError(FileName(sExpression));
  return 0;
}

CoordSystemPtr::CoordSystemPtr()
: IlwisObjectPtr(FileName("unknown", ".csy", true))
{
  _iWidth = 28;
  _iDec = 2;
  rUnitSize = 1;
}

CoordSystemPtr::CoordSystemPtr(const FileName& fn)
: IlwisObjectPtr(fn)
{
  _iWidth = iReadElement("CoordSystem", "Width");
  _iDec = iReadElement("CoordSystem", "Decimals");
  ReadElement("CoordSystem", "CoordBounds", cb);
  rUnitSize = 1;
  ReadElement("CoordSystem", "UnitSize", rUnitSize);
  if (rUnitSize < 1e-50)
    rUnitSize = 1;
}
  
CoordSystemPtr::CoordSystemPtr(const FileName& fn, int)
: IlwisObjectPtr(fn,true, ".csy")
{
  _iWidth = 28;
  _iDec = 2;
  rUnitSize = 1; // metric
  fChanged = true;
}
  
String CoordSystemPtr::sType() const
{
  return "Coordinate System";
}

void CoordSystemPtr::Store()
{
  IlwisObjectPtr::Store();
  WriteElement("Ilwis", "Type", "CoordSystem");
  WriteElement("Domain", "Type", "DomainCoord");
  WriteElement("CoordSystem", "CoordBounds", cb);
  WriteElement("CoordSystem", "Width", (long)iWidth());
  WriteElement("CoordSystem", "Decimals", (long)iDec());
  if (rUnitSize != 0)
    WriteElement("CoordSystem", "UnitSize", rUnitSize);
}

String CoordSystemPtr::sValue(const Coord& c, short wid, short dec) const
{
  if (wid < 0)
    wid = iWidth();
  if (dec < 0)
    dec = iDec();
    
  String s;
  if (wid < 0)
    return "";
  else if (c.fUndef())
    if (wid == 0)
      return "(?,?)";
    else  
      return String("%*s", wid, "(?,?)");
  else if (wid == 0)
    if (abs(c.x) > 1e12 || abs(c.y) > 1e12)
      s = String("(%.*g,%.*g)", dec, c.x, dec, c.y);
    else
      s = String("(%.*f,%.*f)", dec, c.x, dec, c.y);
  else {
    short w = wid;
    w -= 3;
    w /= 2;
    if (abs(c.x) > 1e12 || abs(c.y) > 1e12)
      s = String("(%*.*g,%*.*g)", w, dec, c.x, w, dec, c.y);
    else
      s = String("(%*.*f,%*.*f)", w, dec, c.x, w, dec, c.y);
    short dif = s.length() - wid;
//    if (dif <= 2*dec)
    if (dif > 0)
      if (abs(c.x) > 1e12 || abs(c.y) > 1e12)
        s = String("(%*.*g,%*.*g)", w, max(0,dec-(dif+1)/2), c.x, w, max(0,dec-(dif+1)/2), c.y);
      else
        s = String("(%*.*f,%*.*f)", w, max(0,dec-(dif+1)/2), c.x, w, max(0,dec-(dif+1)/2), c.y);
//    else {
 //     s = String("(%f,%f)", c.x, c.y);
      dif = s.length() - wid;
      if (dif > 0)
        s = String('*', wid);
 //   }
  }  
  return s;
}

Coord CoordSystemPtr::cValue(const String& s) const
{
  Coord crd;
  if (2 != sscanf(s.scVal(), "(%lg,%lg)", &crd.x, &crd.y))
    if (2 != sscanf(s.scVal(), "%lg,%lg", &crd.x, &crd.y))
      sscanf(s.scVal(), "%lg%lg", &crd.x, &crd.y);
  return crd;
}

bool CoordSystemPtr::fConvertFrom(const CoordSystem& cs) const
{ 
//	if (fUnknown() || cs->fUnknown())
//		return false;
  if (cs->fEqual(*this))
    return true;
  if (fLatLon2Coord() && cs->fCoord2LatLon())
    return true;
  return cs->fConvertTo(this);
}

bool CoordSystemPtr::fConvertTo(const CoordSystemPtr* cs) const
{	
//	if (fUnknown() || cs->fUnknown())
//		return false;
  if (cs->fEqual(*this))
    return true;
  return false;
}

Coord CoordSystemPtr::cInverseConv(const CoordSystemPtr* cs, const Coord& crd) const
{
  if (cs->fEqual(*this))
    return crd;
  return Coord();
}

Coord CoordSystemPtr::cConv(const CoordSystem& cs, const Coord& crd) const
{
  if (cs->fEqual(*this))
    return crd;
  return cs->cInverseConv(this, crd);
}

bool CoordSystemPtr::fLatLon2Coord() const
{
  return false;
}

bool CoordSystemPtr::fCoord2LatLon() const
{
  return false;
}

LatLon CoordSystemPtr::llConv(const Coord&) const
{
  return LatLon();
}

Coord CoordSystemPtr::cConv(const LatLon&) const
{
  return crdUNDEF;
}

CoordBounds CoordSystemPtr::cbConv(const CoordSystem& csOld, const CoordBounds& cbOld)
{
  double rDX = cbOld.width()/10;
  double rDY = cbOld.height()/10;
  int iX, iY;
  Coord cXY;
  CoordBounds cbNew = CoordBounds();
  for ( cXY.x = cbOld.MinX(),iX = 0; iX <= 10; cXY.x += rDX, ++iX)
    for (cXY.y = cbOld.MinY(),iY = 0; iY <= 10; cXY.y += rDY, ++iY )
      cbNew += cConv( csOld, cXY);
  return cbNew;  
}

void CoordSystemPtr::Rename(const FileName& fnNew)
{
}

bool CoordSystemPtr::fUnknown() const
{
  return fCIStrEqual(fnObj.sFile, "unknown");
}

void CoordSystemPtr::GetObjectStructure(ObjectStructure& os)
{
	IlwisObjectPtr::GetObjectStructure(os);
}

void CoordSystemPtr::DoNotUpdate()
{
	IlwisObjectPtr::DoNotUpdate();
	
}

CoordSystemBoundsOnly* CoordSystemPtr::pcsBoundsOnly() const
{
  return dynamic_cast<CoordSystemBoundsOnly*> (const_cast<CoordSystemPtr*>(this));
}

CoordSystemViaLatLon* CoordSystemPtr::pcsViaLatLon() const
{
  return dynamic_cast<CoordSystemViaLatLon*> (const_cast<CoordSystemPtr*>(this));
}

CoordSystemLatLon* CoordSystemPtr::pcsLatLon() const
{
  return dynamic_cast<CoordSystemLatLon*> (const_cast<CoordSystemPtr*>(this));
}

CoordSystemProjection* CoordSystemPtr::pcsProjection() const
{
  return dynamic_cast<CoordSystemProjection*> (const_cast<CoordSystemPtr*>(this));
}

CoordSystemDirect* CoordSystemPtr::pcsDirect() const
{
  return dynamic_cast<CoordSystemDirect*> (const_cast<CoordSystemPtr*>(this));
}

CoordSystemFormula* CoordSystemPtr::pcsFormula() const
{
  return dynamic_cast<CoordSystemFormula*> (const_cast<CoordSystemPtr*>(this));
}

CoordSystemCTP* CoordSystemPtr::pcsCTP() const
{
  return dynamic_cast<CoordSystemCTP*> (const_cast<CoordSystemPtr*>(this));
}

CoordSystemTiePoints* CoordSystemPtr::pcsTiePoints() const
{
  return dynamic_cast<CoordSystemTiePoints*> (const_cast<CoordSystemPtr*>(this));
}

CoordSystemOrthoPhoto* CoordSystemPtr::pcsOrthoPhoto() const
{
  return dynamic_cast<CoordSystemOrthoPhoto*> (const_cast<CoordSystemPtr*>(this));
}

CoordSystemDirectLinear* CoordSystemPtr::pcsDirectLinear() const
{
  return dynamic_cast<CoordSystemDirectLinear*> (const_cast<CoordSystemPtr*>(this));
}




