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
/* $Log: /ILWIS 3.0/GeoReference/SolveOblique.cpp $
 * 
 * 3     23-09-99 18:13 Hendrikse
 * Made new class SolveOblique for use by GeoRefCTPplanar and
 * CoordSystemTiePoints (from contents of tools\solveobl.c)
 * 
 * 2     23-09-99 10:57 Hendrikse
 * header comment corrected
 * 
 * 1     23-09-99 10:22 Hendrikse
 * this file takes the algorithm iFindOblique from tls\solveobl.c and will
 * contain a parent class for GeoRefCTPplanar and CoordSystemTiePoints. 
 * Hence Tools\solveobl.cpp and *.h will be obsolete
// Revision 1.2  1997/08/06 18:18:28  Wim
// Catch errors from LeastSquares and report it as being a singular matrix
//
/*
   Find the solution for oblique photos
*/
#define SOLVEOBL_C
#include "Headers\toolspch.h"
#include "Engine\Base\Algorithm\Lstsqrs.h"
#include "Engine\SpatialReference\SolveOblique.h"

SolveOblique::SolveOblique()
{}

int SolveOblique::iFindOblique(int iPoints,
              const Coord* Indep, const Coord* Depen, Coeff &Coef)

{
  if (iPoints < 4) return -2;
  int N = 2 * iPoints;
  int M = 8;

  RealMatrix A(N, M);
  CVector b(N);

  for (int i = 0; i < iPoints; ++i) {
    for (int j = 0; j < 8; ++j) {
      A(2*i,j) = 0;
      A(2*i+1,j) = 0;
    }
    A(2*i, 0) = Indep[i].x;
    A(2*i, 1) = Indep[i].y;
    A(2*i, 2) = 1;
    A(2*i, 6) = - Depen[i].x * Indep[i].x;
    A(2*i, 7) = - Depen[i].x * Indep[i].y;
    A(2*i+1  , 3) = Indep[i].x;
    A(2*i+1  , 4) = Indep[i].y;
    A(2*i+1  , 5) = 1;
    A(2*i+1  , 6) = - Depen[i].y * Indep[i].x;
    A(2*i+1  , 7) = - Depen[i].y * Indep[i].y;
    b(2*i) = Depen[i].x;
    b(2*i+1  ) = Depen[i].y;
  }

  try {
    CVector vec = LeastSquares(A, b);
    for (int i = 0; i < 8; ++i)
      Coef[i] = vec(i);
  }
  catch (ErrorObject&) {
    return -3;
  }
  return 0;
}








