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
/* ProjectionGKColombia
   Copyright Ilwis System Development ITC
   march 1998, by Jan Hendrikse
	Last change:  WK    9 Apr 98    3:46 pm
*/

#include "Engine\SpatialReference\gkcolomb.h"
#include "Engine\Base\DataObjects\ERR.H"
#include "Headers\Hs\proj.hs"

ProjectionGKColombia::ProjectionGKColombia(const Ellipsoid& ell)
: ProjectionTransverseMercator(ell)
{
  fUseParam[pvX0] = false;
  fUseParam[pvY0] = false;
  fUseParam[pvLON0] = false;
  fUseParam[pvLAT0] = false;
  fUseParam[pvK0] = false;
  fUseParam[pvZONE] = true;
  k0 = 1.0;
  Param(pvLAT0, 4.5990472); //Latit of Bogota Observatory 4:35:56.57
  x0 = 1000000; // false Easting for all zones
  y0 = 1000000; // false Northing for all zones
  Param(pvZONE, 1L);
}

void ProjectionGKColombia::Param(ProjectionParamValue pv, long iValue)
{
  ProjectionPtr::Param(pv, iValue);
  if (pvZONE == pv) {
    if (iNr <= 0 || iNr > 4)  {
      iNr = 1;
      InvalidGKColombiaZone();
    }
    double rLam0;
    switch (iNr) {
      case 1: rLam0 = -77.0809722; break;// CM zone West, 77:04:51:30 W
      case 2: rLam0 = -74.0809722; break;// CM zone Center 74:04:51:30   
      case 3: rLam0 = -71.0809722; break;// CM zone East    71:04:51:30
      case 4: rLam0 = -68.0809722; break;// CM zone East-East68:04:51:30
    }  
    Param(pvLON0,rLam0);
  }
}

void ProjectionGKColombia::Param(ProjectionParamValue pv, double rValue)
{
  ProjectionTransverseMercator::Param(pv, rValue);
}  // this function is added because compiler doesn't distinguish
   // between Param(pv, iValue) and Param(pv, rValue)

Datum* ProjectionGKColombia::datumDefault() const
{
  return new MolodenskyDatum("Bogota Observatory", "");
}

long ProjectionGKColombia::iMaxZoneNr() const
{
	return 4;
}

String ProjectionGKColombia::sInvalidZone() const
{
   return SPRJErrWrongGaussColombiaZone;
}


