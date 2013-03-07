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
/* Projection, ProjectionPtr
   Copyright Ilwis System Development ITC
   march 1995, by Wim Koolhoven
	Last change:  WK    9 Apr 98    2:10 pm
*/

#include "Headers\Err\ILWISDAT.ERR"
#include "Engine\SpatialReference\Coordsys.h"
#include "Engine\Base\System\engine.h"
#include "Engine\SpatialReference\PRJCALC.H"
#include "Engine\SpatialReference\AEQD.H"
#include "Engine\SpatialReference\aitoff.h"
#include "Engine\SpatialReference\ALBERS.H"
#include "Engine\SpatialReference\azimutha.h  "
#include "Engine\SpatialReference\BONNE.H"
#include "Engine\SpatialReference\CASSINI.H"
#include "Engine\SpatialReference\Dutchrd.h"
#include "Engine\SpatialReference\ECKERT1.H"
#include "Engine\SpatialReference\ECKERT2.H"
#include "Engine\SpatialReference\ECKERT3.H"
#include "Engine\SpatialReference\EQDCON.H"
#include "Engine\SpatialReference\EOV_Hungary.h"
#include "Engine\SpatialReference\CONIC.H"
#include "Engine\SpatialReference\CC.H"
#include "Engine\SpatialReference\gauboaga.h "
#include "Engine\SpatialReference\genpersp.h  "
#include "Engine\SpatialReference\gkcolomb.h "
#include "Engine\SpatialReference\gkgerman.h "
#include "Engine\SpatialReference\GNOMONIC.H"
#include "Engine\SpatialReference\Goode.h"
#include "Engine\SpatialReference\GoodeINT.h"
#include "Engine\SpatialReference\GeoStatSat.h"
#include "Engine\SpatialReference\GeoStatSatView.h"
#include "Engine\SpatialReference\HammerAitoff.H"
#include "Engine\SpatialReference\LCONCON.H"
#include "Engine\SpatialReference\CYLEQARE.H"
#include "Engine\SpatialReference\LPOLEQA.H"
#include "Engine\SpatialReference\LAEA.H"
#include "Engine\SpatialReference\lafrance.h "
#include "Engine\SpatialReference\MERC.H"
#include "Engine\SpatialReference\MERCscaled.h"
#include "Engine\SpatialReference\MILLER.H"
#include "Engine\SpatialReference\MOLL.H"
#include "Engine\SpatialReference\Oblimerc.h"
#include "Engine\SpatialReference\ORTHOGR.H"
#include "Engine\SpatialReference\PC.H"
#include "Engine\SpatialReference\PLCARREE.H"
#include "Engine\SpatialReference\PLRECTAN.H"
#include "Engine\SpatialReference\POLYCON.H"
#include "Engine\SpatialReference\ROBINSON.H"
#include "Engine\SpatialReference\SCHREIB.H"
#include "Engine\SpatialReference\SINUS.H"
#include "Engine\SpatialReference\SINUSINT.H"
#include "Engine\SpatialReference\SINUSIN2.H"
#include "Engine\SpatialReference\SINUSIN3.H"
#include "Engine\SpatialReference\STEREO.H"
#include "Engine\SpatialReference\TMERC.H"
#include "Engine\SpatialReference\UPS.H"
#include "Engine\SpatialReference\UTM.H"
#include "Engine\SpatialReference\VANDG.H"
#include "Engine\SpatialReference\VANHUUT3.H"
#include "Engine\SpatialReference\stereopolar.h"
#include "Engine\SpatialReference\verticpersp.h"
#include "Engine\SpatialReference\MSGpersp.h"
#include "Engine\SpatialReference\wagner7.h"
#include "Engine\SpatialReference\Winkel1.H"
#include "Engine\SpatialReference\Winkel2.H"
#include "Engine\SpatialReference\Winkel3.H"
#include "Engine\SpatialReference\PseudoMercator.h"

#include "Headers\Hs\proj.hs"

#define EPS10 1.e-10
#define EPS7 1.e-7

void PhiLam::AdjustLon()
{
  while (Lam >  M_PI) Lam -= 2 * M_PI;  
  while (Lam < -M_PI) Lam += 2 * M_PI;  
}

IlwisObjectPtrList Projection::listPrj;

Projection::Projection()
: IlwisObject(listPrj) 
{}

Projection::Projection(const Projection& prj)
: IlwisObject(listPrj, prj.pointer())
{}

Projection::Projection(const FileName& fn)
: IlwisObject(listPrj)
{
  if (!pointer())
    SetPointer(ProjectionPtr::create(fn, Ellipsoid()));
}

Projection::Projection(const String& sName)
: IlwisObject(listPrj, ProjectionPtr::create(sName, Ellipsoid()))
{}

Projection::Projection(const FileName& fn, const Ellipsoid& ell)
: IlwisObject(listPrj)
{
  if (!pointer())
    SetPointer(ProjectionPtr::create(fn, ell));
}

Projection::Projection(const String& sName, const Ellipsoid& ell)
: IlwisObject(listPrj, ProjectionPtr::create(sName, ell))
{}

String ProjectionPtr::sName(bool fExt, const String& sDir) const
{
  if (fnObj.sFile.length())
    return IlwisObjectPtr::sName(fExt, sDir);
  else
    return _sName;  
}

ProjectionPtr* ProjectionPtr::create(const FileName& fn, const Ellipsoid& ell)
{
  if (!File::fExist(fn))
    NotFoundError(fn);
  String sType;
  ObjectInfo::ReadElement("Projection", "Type", fn, sType);
  if ("ProjectionCalc" == sType)
    return new ProjectionCalc(fn, ell);
  return create(sType, ell);
}  

ProjectionPtr* ProjectionPtr::create(const String& sName, const Ellipsoid& ell)
{
  ProjectionPtr* ptr;
  if (_stricmp("Central Cylindrical", sName.c_str()) == 0)
    ptr = new ProjectionCentralCylindrical();
	else if (_stricmp("Aitoff", sName.c_str()) == 0)
    ptr = new ProjectionAitoff();
  else if (_stricmp("Albers EqualArea Conic", sName.c_str()) == 0)
    ptr = new ProjectionAlbersEqualAreaConic(ell); 
  else if (_stricmp("Azimuthal Equidistant", sName.c_str()) == 0)
    ptr = new ProjectionAzimEquidistant(ell);
  else if (_stricmp("DutchRD", sName.c_str()) == 0)
    ptr = new ProjectionDutchRD(ell);  
  else if (_stricmp("Bonne", sName.c_str()) == 0)
    ptr = new ProjectionBonne(ell);
  else if (_stricmp("Cassini", sName.c_str()) == 0)
    ptr = new ProjectionCassini(ell);
  else if (_stricmp("Eckert1", sName.c_str()) == 0)
    ptr = new ProjectionEckert1();
  else if (_stricmp("Eckert2", sName.c_str()) == 0)
    ptr = new ProjectionEckert2();
  else if (_stricmp("Eckert3", sName.c_str()) == 0)
    ptr = new ProjectionEckert3();
	else if (_stricmp("EOV Hungary", sName.c_str()) == 0)
    ptr = new ProjectionEOVHungary(ell);
  else if (_stricmp("Equidistant Conic", sName.c_str()) == 0)
    ptr = new ProjectionEquiDistConic(ell);
  else if (_stricmp("Gauss-Boaga Italy", sName.c_str()) == 0)
    ptr = new ProjectionGaussBoaga(ell); 
  else if (_stricmp("General Perspective", sName.c_str()) == 0)
    ptr = new ProjectionGeneralPerspective(ell); 
  else if (_stricmp("Gauss Colombia", sName.c_str()) == 0)
    ptr = new ProjectionGKColombia(ell); 
  else if (_stricmp("Gauss-Krueger Germany", sName.c_str()) == 0)
    ptr = new ProjectionGKGermany(ell); 
  else if (_stricmp("Gnomonic", sName.c_str()) == 0)
    ptr = new ProjectionGnomonic();
  else if (_stricmp("Goode", sName.c_str()) == 0)
    ptr = new ProjectionGoode(ell);
	else if (_stricmp("Goode Interrupted", sName.c_str()) == 0)
    ptr = new ProjectionGoodeInterrupted(ell);
	else if (_stricmp("GeoStationary Satellite", sName.c_str()) == 0)
    ptr = new ProjectionGeoStationarySatellite(ell);
	else if (_stricmp("GeoStationary Satellite View (PROJ4)", sName.c_str()) == 0)
    ptr = new ProjectionGeoStationarySatelliteView(ell);
  else if (_stricmp("Hammer Aitoff", sName.c_str()) == 0)
    ptr = new	ProjectionHammerAitoff(ell);
  else if (_stricmp("Lambert Conform Conic France", sName.c_str()) == 0)
    ptr = new ProjectionLaFrance(ell);   
  else if (_stricmp("Lambert Azimuthal EqualArea", sName.c_str()) == 0)
    ptr = new ProjectionLambAzimEqualArea(ell);
  else if (_stricmp("Lambert Conformal Conic", sName.c_str()) == 0)
    ptr = new ProjectionLambConfConic(ell);
  else if (_stricmp("Lambert Polar EqualArea", sName.c_str()) == 0)
    ptr = new ProjectionLPolarEqualArea(ell);
  else if (_stricmp("Lambert Cylind EqualArea", sName.c_str()) == 0)
    ptr = new ProjectionLCylindEqualArea(ell);
  else if (_stricmp("Mercator", sName.c_str()) == 0)
    ptr = new ProjectionMercator(ell);
	else if (_stricmp("Mercator Scaled", sName.c_str()) == 0)
    ptr = new ProjectionMercatorScaled(ell);
  else if (_stricmp("Miller", sName.c_str()) == 0)
    ptr = new ProjectionMillerCylindrical();
  else if (_stricmp("Mollweide", sName.c_str()) == 0)
    ptr = new ProjectionMollweide(ell);
	else if (_stricmp("MSG Perspective", sName.c_str()) == 0)
    ptr = new MSGPerspective(ell); 
  else if (_stricmp("Oblique Mercator", sName.c_str()) == 0)
    ptr = new ProjectionObliqueMercator(ell);
  else if (_stricmp("Orthographic", sName.c_str()) == 0)
    ptr = new ProjectionOrthoGraphic();
//  else if ("Plate Carée" == sName || "Plate Caree" == sName.c_str())
//    ptr = new ProjectionPlateCaree();
  else if (_stricmp("Plate Carree", sName.c_str()) == 0)
    ptr = new ProjectionPlateCarree();
  else if (_stricmp("Plate Rectangle", sName.c_str()) == 0)
    ptr = new ProjectionPlateRectangle();
  else if (_stricmp("PolyConic", sName.c_str()) == 0)
    ptr = new ProjectionPolyConic(ell);       
  else if (_stricmp("Robinson", sName.c_str()) == 0)
    ptr = new ProjectionRobinson();
  else if ("Schreiber Netherlands" == sName.c_str())
    ptr = new ProjectionSchreiber();
  else if (_stricmp("Sinusoidal", sName.c_str()) == 0)
    ptr = new ProjectionCylindSinusoidal(ell);
  else if (_stricmp("Sinusoidal Interrupted", sName.c_str()) == 0)
    ptr = new ProjectionCylindSinusInterrupt();
  else if (_stricmp("Sinusoidal 2 x Interrupted", sName.c_str()) == 0)
    ptr = new ProjectionCylindSinusInterrupt2();
  else if (_stricmp("Sinusoidal 3 x Interrupted", sName.c_str()) == 0)
    ptr = new ProjectionCylindSinusInterrupt3();
  else if (_stricmp("StereoGraphic", sName.c_str()) == 0)
    ptr = new ProjectionStereoGraphic(ell);
  else if (_stricmp("StereoPolar", sName.c_str()) == 0)
	ptr = new ProjectionStereoPolar(ell);
  else if (_stricmp("Transverse Mercator", sName.c_str()) == 0)
    ptr = new ProjectionTransverseMercator(ell);
  else if (_stricmp("UPS", sName.c_str()) == 0)
    ptr = new ProjectionUPS(ell);
  else if (_stricmp(sName.c_str(), "UTM") == 0)
    ptr = new ProjectionUTM(ell);
  else if (_stricmp("VanderGrinten", sName.c_str()) == 0)
    ptr = new ProjectionVanderGrinten();
  else if (_stricmp("VanHuut", sName.c_str()) == 0)
    ptr = new ProjectionVanHuut3();
	else if (_stricmp("Vertical Perspective", sName.c_str()) == 0)
    ptr = new ProjectionVerticalPerspective(ell); 
	else if (_stricmp("Wagner VII", sName.c_str()) == 0)
    ptr = new ProjectionWagner7();
	else if (_stricmp("Winkel I", sName.c_str()) == 0)
    ptr = new ProjectionWinkel1();
	else if (_stricmp("Winkel II", sName.c_str()) == 0)
    ptr = new ProjectionWinkel2();
	else if (_stricmp("Winkel Triple", sName.c_str()) == 0)
    ptr = new ProjectionWinkel3();
	else if (_stricmp("Pseudo Mercator", sName.c_str()) == 0)
    ptr = new ProjectionPseudoMercator(ell);
  else  
    InvalidTypeError(FileName(), "Projection", sName.c_str());
  ptr->_sName = sName; 
  String sPath = getEngine()->getContext()->sIlwDir();
  sPath &= "\\Resources\\Def\\Projs.def";
  char buf[1024];
  GetPrivateProfileString("Projections",sName.c_str(),"",buf,1024, sPath.c_str());
  String sid(buf);
  sid = sid.sTail(":");
  if ( sid != "")
	  ptr->identification = sid;
  return ptr;
}

ProjectionPtr::ProjectionPtr(const FileName& fn, const Ellipsoid& el)
: IlwisObjectPtr(fn), ell(el)
{
  init();
}
  
ProjectionPtr::ProjectionPtr(const Ellipsoid& el)
: ell(el)
{
  init();
}

static Ellipsoid ellDummy;


ProjectionPtr::ProjectionPtr()
: ell(ellDummy)
{
  init();
} 

void ProjectionPtr::init()     // initialize proj parameters with default values
{
  fEll = false;
  f2Coord = f2LatLon = true;
  x0 = y0 = 0;
  k0 = 1;
  lam0 = 0;
  phi0 = 0;
	phits = 0;								// latitude of true scale
  rPhi1 = M_PI/6;
  rPhi2 = M_PI/3;
  fNorth = true;             // default hemisphere choice for UTM and UPS projection
  iNr = 1;                   // default UTM zone
  rHeight = 6000000.0;       // default dist of persp center from the earth surface in meters
  rTilt = 0;
  rAzimYaxis = 0;            // def azimuth of y-axis of gen persp projection plane
  fTilted = false;
  rAzimCLine = 0;            // def azimuth of central line in obl mercator
  fNoriented = true;         // default orientation of map in Oblique Mercator
  for (int i = (int)pvNONE; i < (int)pvLAST; ++i)
    fUseParam[i] = false;
  fUseParam[pvX0] = true;  
  fUseParam[pvY0] = true;  
  fUseParam[pvLON0] = true;
}
  
void ProjectionPtr::Prepare()
{
}

void ProjectionPtr::Store()
{
  IlwisObjectPtr::Store();
  WriteElement("Ilwis", "Type", "Projection");
}  

String ProjectionPtr::sType()
{
  return "Projection";
}

XY ProjectionPtr::xyConv(const PhiLam&) const
{
  return XY();
}

PhiLam ProjectionPtr::plConv(const XY&) const
{
  return PhiLam();
}
  
String ProjectionPtr::sParamName(ProjectionParamValue pv) const
{
  switch (pv) {
    case pvX0:        return "False Easting";
    case pvY0:        return "False Northing";
    case pvLON0:      return "Central Meridian";
    case pvLATTS:     return "Latitude of True Scale";
    case pvLAT0:      return "Central Parallel";
    case pvLAT1:      return "Standard Parallel 1 ";
    case pvLAT2:      return "Standard Parallel 2 ";
    case pvK0:        return "Scale Factor";
    case pvNORTH:     return "Northern Hemisphere";
    case pvZONE:      return "Zone";
    case pvHEIGHT:    return "Height Persp. Center";
    case pvTILTED:    return "Tilted/Rotated Projection Plane";
    case pvTILT:      return "Tilt of Projection Plane";
    case pvAZIMYAXIS: return "Azim of Projection Y-Axis";
    case pvAZIMCLINE: return "Azim of Central Line of True Scale";
    case pvPOLE:      return "Pole of Oblique Cylinder";
    case pvNORIENTED: return "North Oriented XY Coord System";
    default:          return "Unknown Parameter";
  }  
}

String ProjectionPtr::sParamUiName(ProjectionParamValue pv) const
{
  switch (pv) {
    case pvX0:        return TR("False &Easting");
    case pvY0:        return TR("False &Northing");
    case pvLON0:      return TR("&Central Meridian");
    case pvLATTS:     return TR("&Latitude of True Scale");
    case pvLAT0:      return TR("Central &Parallel");
    case pvLAT1:      return TR("Standard Parallel &1 ");
    case pvLAT2:      return TR("Standard Parallel &2 ");
    case pvK0:        return TR("&Scale Factor");
    case pvNORTH:     return TR("Northern &Hemisphere");
    case pvZONE:      return TR("&Zone");
    case pvHEIGHT:    return TR("Height &Persp. Center");
    case pvTILTED:    return TR("Tilted/&Rotated Projection Plane");
    case pvTILT:      return TR("&Tilt of Projection Plane");
    case pvAZIMYAXIS: return TR("Azim of Projection &Y-Axis");
    case pvAZIMCLINE: return TR("&Azim of Central Line of True Scale");
    case pvPOLE:      return TR("Pole of &Oblique Cylinder");
    case pvNORIENTED: return TR("&North Oriented XY Coord System");
    default:          return TR("Unknown Parameter");
  }  
}

ProjectionParamType ProjectionPtr::ppParam(ProjectionParamValue pv) const
{
  if (!fUseParam[(int)pv])
    return ppNONE;
  switch (pv) {
    case pvX0:        return ppREAL;
    case pvY0:        return ppREAL;
    case pvLON0:      return ppLON;
    case pvLATTS:     return ppLAT;
    case pvLAT0:      return ppLAT;
    case pvLAT1:      return ppLAT;
    case pvLAT2:      return ppLAT;
    case pvK0:        return ppSCALE;
    case pvNORTH:     return ppBOOL;
    case pvZONE:      return ppZONE;
    case pvHEIGHT:    return ppPOSREAL;
    case pvTILTED:    return ppBOOL;
    case pvTILT:      return ppACUTANGLE;
    case pvAZIMYAXIS: return ppANGLE;
    case pvAZIMCLINE: return ppANGLE;
    case pvPOLE:      return ppLATLON;
    case pvNORIENTED: return ppBOOL;
    default:          return ppNONE;
  }  
}

ProjectionParamType ProjectionPtr::ppAllParam(ProjectionParamValue pv) const
{
  switch (pv) {
    case pvX0:        return ppREAL;
    case pvY0:        return ppREAL;
    case pvLON0:      return ppLON;
    case pvLATTS:     return ppLAT;
    case pvLAT0:      return ppLAT;
    case pvLAT1:      return ppLAT;
    case pvLAT2:      return ppLAT;
    case pvK0:        return ppSCALE;
    case pvNORTH:     return ppBOOL;
    case pvZONE:      return ppZONE;
    case pvHEIGHT:    return ppPOSREAL;
    case pvTILTED:    return ppBOOL;
    case pvTILT:      return ppACUTANGLE;
    case pvAZIMYAXIS: return ppANGLE;
    case pvAZIMCLINE: return ppANGLE;
    case pvPOLE:      return ppLATLON;
    case pvNORIENTED: return ppBOOL;
    default:          return ppNONE;
  }  
}

long   ProjectionPtr::iParam(ProjectionParamValue pv) const   // ppINT
{
  switch (pv) {
    case pvNORTH:     return fNorth;
    case pvNORIENTED: return fNoriented;
    case pvZONE:      return iNr;
    case pvTILTED:    return fTilted;
    default:          return iUNDEF;
  }  
}

double ProjectionPtr::rParam(ProjectionParamValue pv) const   // ppLAT, ppLON, ppREAL, ppANGLE, ppACUTANGLE
{
  switch (pv) {
    case pvX0:     return x0;
    case pvY0:     return y0;
    case pvLON0:   return lam0 * 180 / M_PI;
    case pvLATTS:  return phits * 180 / M_PI;
    case pvLAT0:   return phi0 * 180 / M_PI;
    case pvLAT1:   return rPhi1 * 180 / M_PI;
    case pvLAT2:   return rPhi2 * 180 / M_PI; 
    case pvHEIGHT: return rHeight;
    case pvTILT:   return rTilt * 180 / M_PI;
    case pvAZIMYAXIS:  return rAzimYaxis * 180 / M_PI;
    case pvAZIMCLINE:  return rAzimCLine * 180 / M_PI;
    case pvK0:     return k0;
    default: return rUNDEF;
  }  
}

LatLon ProjectionPtr::llParam(ProjectionParamValue pv) const  // ppLATLON
{
  return LatLon();
}

Coord  ProjectionPtr::cParam(ProjectionParamValue pv) const   // ppCOORD
{
  return crdUNDEF;
}

void ProjectionPtr::Param(ProjectionParamValue pv, long iValue)
{
  switch (pv) {
    case pvNORTH: fNorth = iValue == 1; break;
    case pvNORIENTED: fNoriented = iValue == 1; break;
    case pvZONE:  iNr = iValue; break;
    case pvTILTED: fTilted = iValue == 1; break;
  }  
}

void ProjectionPtr::Param(ProjectionParamValue pv, double rValue)
{
  if (pvLAT0 == pv || pvLAT1 == pv || pvLAT2 == pv || pvLATTS == pv)
    if (abs(rValue) > 90)
      throw ErrorObject(TR("Latitudes are invalid outside [-90,90]"), 2210);
  if (pvK0 == pv)
    if (rValue <= EPS10)
      throw ErrorObject(TR("Scale factor should be larger than zero"), 2211); 
	if (pvHEIGHT == pv) 
    if (rValue <= EPS10)
			return;
  //    throw ErrorObject(TR("Height of perspective center must be larger than zero"), 2212);
	if (pvLON0 == pv) 
		if (abs(rValue) > 360)
      throw ErrorObject(TR("Longitudes are invalid outside [-360,360]"), 2213); 

  switch (pv) {
    case pvX0:       x0 = rValue; break;
    case pvY0:       y0 = rValue; break;
    case pvLON0:     lam0 = rValue * M_PI / 180; break;
    case pvLATTS:    phits = rValue * M_PI / 180; break;
    case pvLAT0:     phi0 = rValue * M_PI / 180; break;
    case pvLAT1:     rPhi1 = rValue * M_PI / 180; break;
    case pvLAT2:     rPhi2 = rValue * M_PI / 180; break;
    case pvHEIGHT:   rHeight = rValue; break;
    case pvTILT:     rTilt = rValue * M_PI / 180; break;
    case pvAZIMYAXIS:  rAzimYaxis = rValue * M_PI / 180; break;
    case pvAZIMCLINE:  rAzimCLine = rValue * M_PI / 180; break;
    case pvK0:    k0 = rValue; break;
  }  
}

void ProjectionPtr::Param(ProjectionParamValue pv, const LatLon&)
{}

void ProjectionPtr::Param(ProjectionParamValue pv, const Coord&)
{}

double ProjectionPtr::rHypot(double x, double y) const
{
  if ( x < 0.)
    x = -x;
  else if (x == 0.)
    return (y < 0. ? -y : y);
  if (y < 0.)
    y = -y;
  else if (y == 0.)
    return (x);
  if ( x < y ) 
  {
    x /= y;
    return ( y * sqrt( 1. + x * x ) );
  } 
  else {
    y /= x;
    return ( x * sqrt( 1. + y * y ) );
  }
}

double ProjectionPtr::msfn(double phi) const
{
  double sinPhi = sin(phi);
  double cosPhi = cos(phi);
  return cosPhi / sqrt (1. - ell.e2 * sinPhi * sinPhi);
}

double ProjectionPtr::tsfn(double phi) const
{
  double sinPhi = sin(phi);
  sinPhi *= ell.e;
  return tan (.5 * (M_PI_2 - phi)) / pow((1 - sinPhi) / (1 + sinPhi), .5 * ell.e);
}

/* determine small q from ellipsoidal phi and eccentric e */
// USGS Paper 1395 Washington 1987 Ch 3 auxil latitudes eq (3-12) 
# define EPSILON 1.0e-7
double ProjectionPtr::qsfn(double phi) const
{
	double con;
        double sinphi = sin(phi);
        double ecc = ell.e;
	if (ecc >= EPSILON) {
		con = ecc * sinphi;
		return ((1 - ell.e2) * (sinphi / (1. - con * con) -
		   (.5 / ecc) * log ((1. - con) / (1. + con))));
	} else
		return (sinphi + sinphi);
}

#define TOL 1.0e-10
#define N_ITER 15

// Geodetic latitude needed for Inverse of Conformal projections
// like Mercator, Lamb Conf Conic etc for ellipsoidal case
// See J.Snyder 'Map Projections, a Working manual' 
// USGS Paper 1395 Washington 1987 Ch 7 Mercator Proj  eq (7-9) 

double ProjectionPtr::phi2(double ts) const
{
  double eccnth, Phi, con, dphi;
  eccnth = .5 * ell.e;
  Phi = M_PI_2 - 2. * atan(ts);
  for (int i = 0; i < N_ITER; ++i) {
    con = ell.e * sin(Phi);
    dphi = M_PI_2 - 2. * atan (ts * pow((1 - con) / (1 + con), eccnth)) - Phi;
    Phi += dphi;
    if (abs(dphi) < TOL)
      return Phi;
  } 
  return rUNDEF;
}

// Geodetic latitude needed for Inverse of EqualArea projections
// like Albers EqArea Conic for ellipsoidal case
// (iterative form of authalic latitude)
// See J.Snyder 'Map Projections, a Working manual' 
// USGS Paper 1395 Washington 1987 Ch 3 auxil latitudes eq (3-16) 

double ProjectionPtr::phi1(double qs) const
{
	double rPhi, sinPhi, cosPhi, con, com, dphi;
	double e = ell.e;
	rPhi = asin (.5 * qs);
	if (e < EPS7)
		return( rPhi );
	for (int i = 0; i < N_ITER; ++i) 
	{
		sinPhi = sin (rPhi);
		cosPhi = cos (rPhi);
		con = e * sinPhi;
		com = 1. - con * con;
		dphi = .5 * com * com / cosPhi * (qs / (1.- e*e) -
		   sinPhi / com + .5 / e * log ((1. - con) /
		   (1. + con)));
		rPhi += dphi;
		if (abs(dphi) < TOL)
      return rPhi;// 
	} 
	return rUNDEF;
}
/* meridinal distance for ellipsoid and inverse
**	8th degree - accurate to < 1e-5 meters when used in conjuction
**		with typical major axis values.
**	Inverse determines phi to EPS (1e-11) radians, about 1e-6 seconds.
*/
#define C00 1.
#define C02 .25
#define C04 .046875
#define C06 .01953125
#define C08 .01068115234375
#define C22 .75
#define C44 .46875
#define C46 .01302083333333333333
#define C48 .00712076822916666666
#define C66 .36458333333333333333
#define C68 .00569661458333333333
#define C88 .3076171875
#define EPS 1e-11
#define MAX_ITER 10
#define EN_SIZE 5

void ProjectionPtr::enfn(double* en) const
{
  double t;
  en[0] = C00 - ell.e2 * (C02 + ell.e2 * (C04 + ell.e2 * (C06 + ell.e2 * C08)));
  en[1] = ell.e2 * (C22 - ell.e2 * (C04 + ell.e2 * (C06 + ell.e2 * C08)));
  en[2] = (t = ell.e2 * ell.e2) * (C44 - ell.e2 * (C46 + ell.e2 * C48));
  en[3] = (t *= ell.e2) * (C66 - ell.e2 * C68);
  en[4] = t * ell.e2 * C88;
}

double ProjectionPtr::mlfn(double phi, double sphi, double cphi, const double *en) const
{
  cphi *= sphi;
  sphi *= sphi;
  return(en[0] * phi - cphi * (en[1] + sphi*(en[2]
          + sphi*(en[3] + sphi*en[4]))));
}                

double ProjectionPtr::invmlfn(double arg, double es, const double *en) const
{
  double s, t, phi, k = 1. - es;
  int i;

  phi = arg;
  for (i = MAX_ITER; i ; --i) { /* rarely goes over 5 iterations */
    s = sin(phi);
    t = 1. - es * s * s;
    t = (mlfn(phi, s, cos(phi), en) - arg) / ( k * t * sqrt(t));
    phi -= t;
    if (fabs(t) < EPS)
      return phi;
  }
  return rUNDEF;
}

/* determine latitude from authalic latitude: */

# define P00 .33333333333333333333
# define P01 .17222222222222222222
# define P02 .10257936507936507936
# define P10 .06388888888888888888
# define P11 .06640211640211640211
# define P20 .01641501294219154443
#define APA_SIZE 3

void ProjectionPtr::authset(double *APA) const
{
  double t;
  double es = ell.e2;
  APA[0] = es * P00;
  t = es * es;
  APA[0] += t * P01;
  APA[1] = t * P10;
  t *= es;
  APA[0] += t * P02;
  APA[1] += t * P11;
  APA[2] = t * P20;

}
double ProjectionPtr::authlat(double beta, const double *APA) const
{
	double t = beta+beta;
	return(beta + APA[0] * sin(t) + APA[1] * sin(t+t) + APA[2] * sin(t+t+t));
}

void ProjectionPtr::InvalidLat()
{
   throw  ErrorObject(TR("Latitudes are invalid outside [-90,90]"), errProjection);
}

void ProjectionPtr::InvalidStandardParallels()
{
   throw  ErrorObject(TR("Invalid Standard Parallels"), errProjection+1);
}

String ProjectionPtr::sInvalidZone() const
{
   return String("");
}

void ProjectionPtr::InvalidUtmZone()
{
   throw  ErrorObject(TR("UTM zones are numbered 1..60"), errProjection+2);
}

void ProjectionPtr::InvalidGaussBoagaZone()
{
   throw  ErrorObject(TR("Gauss-Boaga zones are numbered 1,2"), errProjection+3);
}

void ProjectionPtr::InvalidGKColombiaZone()
{
   throw  ErrorObject(TR("Gauss Colombia zones are numbered 1..4"), errProjection+6);
}

void ProjectionPtr::InvalidGKGermanyZone()
{
   throw  ErrorObject(TR("Gauss-Krueger zones are numbered 1,2,3"), errProjection+4);
}

void ProjectionPtr::InvalidLaFranceZone()
{
   throw  ErrorObject(TR("Lambert France zones are numbered 1..4"), errProjection+5);
}


Datum* ProjectionPtr::datumDefault() const
{
  return 0;
}

long ProjectionPtr::iMaxZoneNr() const
{
  return 0;
}

long ProjectionPtr::iGetZoneNr() const
{
	return iNr;
}

bool ProjectionPtr::fGetHemisphereN() const
{
	return fNorth;
}

double ProjectionPtr::rGetCentralScaleFactor() const
{
	return k0;
}
	
bool ProjectionPtr::fEqual(const IlwisObjectPtr& ptr) const
{
	if (this == &ptr)
		return true;
	const ProjectionPtr* proj = dynamic_cast<const ProjectionPtr*>(&ptr);
	if (0 == proj)
		return false;

	if (typeid(*this) != typeid(ptr))
		return false;

  if (proj->rParam(pvX0) != rParam(pvX0)) return false;
  if (proj->rParam(pvY0) != rParam(pvY0)) return false;
  if (proj->rParam(pvLON0) != rParam(pvLON0)) return false;
  if (proj->rParam(pvLATTS) != rParam(pvLATTS)) return false;
  if (proj->rParam(pvLAT0) != rParam(pvLAT0)) return false;
  if (proj->rParam(pvK0) != rParam(pvK0)) return false;
  if (proj->rParam(pvNORTH) != rParam(pvNORTH)) return false;
  if (proj->rParam(pvZONE) != rParam(pvZONE)) return false;
  if (proj->rParam(pvLAT1) != rParam(pvLAT1)) return false;
  if (proj->rParam(pvLAT2) != rParam(pvLAT2)) return false;
  if (proj->rParam(pvHEIGHT) != rParam(pvHEIGHT)) return false;
  if (proj->rParam(pvTILTED) != rParam(pvTILTED)) return false;
  if (proj->rParam(pvTILT) != rParam(pvTILT)) return false;
  if (proj->rParam(pvAZIMYAXIS) != rParam(pvAZIMYAXIS)) return false;
  if (proj->rParam(pvAZIMCLINE) != rParam(pvAZIMCLINE)) return false;
  if (proj->rParam(pvPOLE) != rParam(pvPOLE)) return false;
  if (proj->rParam(pvNORIENTED) != rParam(pvNORIENTED)) return false;
	return true;
}

String ProjectionPtr::getIdentification(bool wkt) {
	return identification;
}

Projection ProjectionPtr::WKTToILWISName(const String& wkt) {
	String swkt = wkt;
	swkt.toLower();
	int index = swkt.find("projection");
	if ( index != string::npos) {
		int index2 = swkt.find("]",index);
		int start = index + 12;
		String projname = swkt.substr(start, index2 - start - 1);
		index = swkt.find("spheroid");
		if ( index != 1) {
			index2 = swkt.find("]",index);
			int start = index + 10;
			String elldata = swkt.substr(start, index2 - start);
			Array<String> parts;
			Split(elldata, parts,",");
			String ellname = parts[0];
			double axis = parts[1].rVal();
			double f = parts[2].rVal();
			Ellipsoid ell(axis, f);
			if ( f != 0)
				ell.sName = ellname;
			return Projection(projname,ell);

		}
	}

	return Projection();
}