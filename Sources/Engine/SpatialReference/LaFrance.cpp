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
/* ProjectionLaFrance
   Copyright Ilwis System Development ITC
   march 1998, by Jan Hendrikse
	Last change:  WK    9 Apr 98    3:39 pm
*/

#include "Engine\SpatialReference\LaFrance.h"
#include "Engine\Base\DataObjects\ERR.H"
#include "Headers\Hs\proj.hs"

ProjectionLaFrance::ProjectionLaFrance(const Ellipsoid& ell)
: ProjectionLambConfConic(ell)
{
  fUseParam[pvX0] = false;
  fUseParam[pvY0] = false;
  fUseParam[pvLON0] = false;
  fUseParam[pvLAT0] = false;
  fUseParam[pvLAT1] = false;
  fUseParam[pvLAT2] = false;
  fUseParam[pvK0] = false;
  fUseParam[pvZONE] = true;
  Param(pvLON0, 2.3372291667); // meridian of Paris in degrees
  Param(pvZONE, 1L);            // North-zone as default
}

void ProjectionLaFrance::Param(ProjectionParamValue pv, long iValue)
{
  ProjectionPtr::Param(pv, iValue);
  
  if (pvZONE == pv) {
    if (iNr <= 0 || iNr > 4)  {
      iNr = 1;
      InvalidLaFranceZone();
    }
    double rPhi0;
    switch (iNr) {
      case 1: rPhi0 = 49.5;
              x0 = 600000.0;
              y0 = 1200000.0;
              k0 = 0.99987734; break;
      case 2: rPhi0 = 46.8;
              x0 = 600000.0;
              y0 = 2200000.0;
              k0 = 0.99987742; break;
      case 3: rPhi0 = 44.1;
              x0 = 600000.0;
              y0 = 3200000.0; 
              k0 = 0.9998775;break;
      case 4: rPhi0 = 42.165;
              x0 = 234358.0;
              y0 = 4185861.369;
              k0 = 0.99994471;break;
    }
    Param(pvLAT0,rPhi0);
    Param(pvLAT1,rPhi0);
    Param(pvLAT2,rPhi0);
  }
}

void ProjectionLaFrance::Param(ProjectionParamValue pv, double rValue)
{
  ProjectionLambConfConic::Param(pv, rValue);
}  // this function is added because compiler doesn't distinguish
   // between Param(pv, iValue) and Param(pv, rValue)

Datum* ProjectionLaFrance::datumDefault() const
{
  return new MolodenskyDatum("NTF (Nouvelle Triangulation de France)", "");
}

long ProjectionLaFrance::iMaxZoneNr() const
{
	return 4;
}

String ProjectionLaFrance::sInvalidZone() const
{
   return TR("Lambert France zones are numbered 1..4");
}


