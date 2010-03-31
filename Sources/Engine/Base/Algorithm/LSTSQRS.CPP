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
// $Log: LSTSQRS.C $
// Revision 1.2  1997/08/06 18:13:10  Wim
// Just to initialize
//
/* tls/lstsqrs.c
// Least Squares for ILWIS 2.0
// january 1995, by Wim Koolhoven
// (c) Ilwis System Development, ITC
	Last change:  WK   28 Aug 98    4:13 pm
*/
#define LSTSQRS_C
#include "Headers\toolspch.h"
#include "Engine\Base\Algorithm\Lstsqrs.h"

CVector LeastSquares(const RealMatrix& A, const CVector& b)
{
  assert(A.iRows() >= A.iCols());
  assert(A.iRows() == b.iRows());
  int i, j, k;
  int M, N;
  M = A.iCols();
  N = A.iRows();

  RealMatrix AtA(M);
  for (i = 0; i < M; ++i)
    for (j = 0; j < M; ++j) {
      double rSum = 0;
      for (k = 0; k < N; ++k)
	rSum += A(k,i) * A(k,j);
      AtA(i,j) = rSum;
    }

  CVector Atb(M);
  for (i = 0; i < M; ++i) {
    double rSum = 0;
    for (j = 0; j < N; ++j)
      rSum += A(j,i) * b(j);
    Atb(i) = rSum;
  }

  CVector vRes(M);
  AtA.Solve(Atb, vRes);
//  vRes.SetError(AtA.err());
  return vRes;
}








