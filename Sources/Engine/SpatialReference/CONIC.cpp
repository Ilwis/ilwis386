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
/* ProjectionConic
   Copyright Ilwis System Development ITC
   may 1996, by Jan Hendrikse
	Last change:  JHE  28 May 96    9:14 pm
*/

#include "Engine\SpatialReference\CONIC.H"

ProjectionConic::ProjectionConic(const Ellipsoid& ell)
: ProjectionPtr(ell)
{
  fEll = true;
  fUseParam[pvLAT0] = true; 
  fUseParam[pvLAT1] = true;
  fUseParam[pvLAT2] = true;
  fUseParam[pvK0] = true;
  Param(pvLAT0, 0 ); // necessary?
}

ProjectionConic::ProjectionConic()
: ProjectionPtr()
{
  fEll = false;
  fUseParam[pvLAT0] = true; 
  fUseParam[pvLAT1] = true;
  fUseParam[pvLAT2] = true;
  fUseParam[pvK0] = true;
  Param(pvLAT0, 0 ); // necessary?
}


void ProjectionConic::Param(ProjectionParamValue pv, double rValue)
{
  ProjectionPtr::Param(pv, rValue);
  //if (2 != sscanf(sBuf, "%lf,%lf", &rMajor, &f1))
    //throw ErrorObject("Invalid Ellipsoid Definition", 999);
  if (pvLAT1 == pv) {
    rCosPhi1 = cos(rPhi1);
    rSinPhi1 = sin(rPhi1);
  }
  if (pvLAT2 == pv) {
    rCosPhi2 = cos(rPhi2);
    rSinPhi2 = sin(rPhi2);
  }    
}            





