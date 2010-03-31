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
/* ProjectionCalc
   Copyright Ilwis System Development ITC
   march 1995, by Wim Koolhoven
	Last change:  JEL  18 May 97    9:42 pm
*/

#ifndef ILWPRJCALC_H
#define ILWPRJCALC_H
#include "Engine\SpatialReference\prj.h"
#include "Engine\Scripting\Instrucs.h"


class DATEXPORT ProjectionCalc: public ProjectionPtr
{
  friend class ProjectionCalc; 
public:
  ProjectionCalc(const FileName&, const Ellipsoid&);
  virtual ~ProjectionCalc();
  virtual XY xyConv(const PhiLam&) const;
  virtual PhiLam plConv(const XY&) const;
private:    
  void Ell(const Ellipsoid&);
  Array<String> asParams, asVars;
  Array<ProjectionParamType> appt;
  Array<CalcVariable> acvPhiLam2Coord, acvCoord2PhiLam;
  Instructions *instPhiLam2Coord, *instCoord2PhiLam;
  CalcVariable cvrlX, cvrlY, cvrlPhi, cvrlLam, cvrcX, cvrcY, cvrcPhi, cvrcLam;
  CalcVarReal *pcvrlX, *pcvrlY, *pcvrcPhi, *pcvrcLam;//, *cvrcX, *cvrcY, *cvrcPhi, *cvrcLam;
};



#endif // ILWPRJCALC_H





