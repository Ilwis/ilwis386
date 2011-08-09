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
/* ProjectionUTM
   Copyright Ilwis System Development ITC
   march 1995, by Wim Koolhoven
	Last change:  JEL  19 Oct 96   12:57 pm
*/

#include "Engine\SpatialReference\UTM.H"
#include "Engine\Base\DataObjects\ERR.H"
#include "Headers\Hs\proj.hs"

ProjectionUTM::ProjectionUTM(const Ellipsoid& ell)
: ProjectionTransverseMercator(ell)
{
  fUseParam[pvX0] = false;
  fUseParam[pvY0] = false;
  fUseParam[pvLON0] = false;
  fUseParam[pvLAT0] = false;
  fUseParam[pvK0] = false;
  fUseParam[pvZONE] = true;
  fUseParam[pvNORTH] = true;
  k0 = 0.9996;
  phi0 = 0;
  x0 = 500000.;
  y0 = 0;
  Param(pvZONE, 1);
}

void ProjectionUTM::Param(ProjectionParamValue pv, long iValue)
{
  ProjectionPtr::Param(pv, iValue);
  if (pvNORTH == pv) {
    if (fNorth)
      y0 = 0;
    else
      y0 = 10000000.0;  
  }
  else if (pvZONE == pv) {
    if (iNr <= 0 || iNr > 60)  {
      iNr = 1;
      InvalidUtmZone();
    }
    int iZone = iNr - 1;
    lam0 = (iZone + .5) * M_PI / 30. - M_PI;  
  }
}

long ProjectionUTM::iMaxZoneNr() const
{
	return 60;
}

String ProjectionUTM::sInvalidZone() const
{
   return TR("UTM zones are numbered 1..60");
}

