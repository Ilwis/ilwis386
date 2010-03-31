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
//$Log: EQDCON.H $
//Revision 1.3  1998/09/16 17:26:42  Wim
//22beta2
//
//Revision 1.2  1997/08/27 16:45:54  janh
//changed member *en into en[5]
//
/* ProjectionLambertConformalConic
   Copyright Ilwis System Development ITC
   may 1996, by Jan Hendrikse
	Last change:  JHE  27 Aug 97   11:42 am
*/

#ifndef ILWPRJLEQDCON_H
#define ILWPRJLEQDCON_H
#include "Engine\SpatialReference\CONIC.H"

class DATEXPORT ProjectionEquiDistConic: public ProjectionConic
{
public:
  ProjectionEquiDistConic(const Ellipsoid&);
  virtual XY xyConv(const PhiLam&) const;
  virtual PhiLam plConv(const XY&) const;
  virtual void Prepare();
private:
  double en[5];
};

#endif // ILWPRJLEQDCON_H





