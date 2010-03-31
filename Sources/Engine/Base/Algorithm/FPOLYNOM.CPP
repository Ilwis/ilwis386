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
// fpolynom.c
// iFindPolynom
// by Wim Koolhoven
// (c) Computer Department ITC

#define FPOLYNOM_C
#include "Headers\toolspch.h"
#include "Engine\Base\Algorithm\Fpolynom.h"
//		Original language : fortran
//	  Copied from Dipix subroutine ZSPOLY.FTN
// Pascal source FPOLYNOM.PAS by Wim Koolhoven 1988-1991
// 17/2/94 translated from Pascal to C++ by Wim Koolhoven

#define MAXTERMS 10

int iFindPolynom(int iTerms, int iPoints, const Coord* Indep, const Coord* Depen,
		 Coeff &coeffX, Coeff &coeffY)
{
  double Matrix[MAXTERMS][MAXTERMS], InvMat[MAXTERMS][MAXTERMS];
  double DU[MAXTERMS], DV[MAXTERMS], PolyProd[MAXTERMS];
  double DX[4], DY[4];
  double D0, D1;
  int point, row, col, term, MaxPower, i;

  const int PowerX[] = {0,1,0,1,2,0,3,2,1,0};
  const int PowerY[] = {0,0,1,1,0,2,0,1,2,3};
  const double MinVal = 1E-10; // was originally (in Fortran) 1e-10 and since ILWIS.1 (Pascal) it was 1e-3

  //  Set no. of terms in Polynomial and Valid Order

  if ( iPoints < iTerms ) return -2;

  MaxPower = PowerY[iTerms-1];

  //  Initialize Matrix and Dependent Vectors to Zero
  //  and Create Identity Matrix for Inversion
  for (term = 0; term < iTerms; ++term)
  {
    DU[term] = 0;
    DV[term] = 0;
    for (row = 0; row < iTerms; ++row)
    {
      Matrix[term][row] = 0;
      InvMat[term][row] = 0;
    }
    InvMat[term][term] = 1;
  }

  //  Build Matrix and Dependent Vector
  for ( point = 0; point < iPoints; ++point)
  {

    //	Set up Products of X and Y
    DX[0] = 1;
    DY[0] = 1;
    for (i = 0; i < MaxPower; ++i)
    {
      DX[i+1] = DX[i] * Indep[point].x;
      DY[i+1] = DY[i] * Indep[point].y;
    }
    for (term = 0; term < iTerms; ++term)
      PolyProd[term] = DX[PowerX[term]] * DY[PowerY[term]];

    //	Increment First Diagonal term in Matrix
    //	and First Element in Both Dependent Vectors
    Matrix[0][0] += 1;
    DU[0] += Depen[point].x;
    DV[0] += Depen[point].y;

    //	Increment Next Diagonal term in Matrix
    //	and Next Elements in Dependent Vectors
    for (row = 1; row < iTerms; ++row)
    {
      D0 = PolyProd[row];
      Matrix[row][row] += D0 * D0;
      DU[row] += Depen[point].x * D0;
      DV[row] += Depen[point].y * D0;

      //  Increment Remainder of row up to Diagonal term
      //  and Copy to Corresponding column
      for (col = 0; col < row; ++col)
      {
	Matrix[row][col] += D0 * PolyProd[col];
	Matrix[col][row] = Matrix[row][col];
      } // col
    } // row
  } // point

  //  Start Matrix Inversion, points Next Diagonal Element Too Small ?
  for (term = 0; term < iTerms; ++term)
  {
    D0 = Matrix[term][term];
    if (abs(D0) <= MinVal) return -3;

    //	Divide This row by its Diagonal Element
    for (row = 0; row < iTerms; ++row)
    {
      Matrix[term][row] /= D0;
      InvMat[term][row] /= D0;
    }

    //	Subtract Appropriate Multiple of This row from All Other rows
    for (row = 0; row < iTerms; ++row)
      if (row != term )
      {
	D1 = Matrix[row][term];
	for ( col = 0; col < iTerms; ++col)
        {
	  Matrix[row][col] -= Matrix[term][col] * D1;
	  InvMat[row][col] -= InvMat[term][col] * D1;
	} // col
      } // row
  } // term

  for (term = 0; term < MAXTERMS; ++term) {
    coeffX[term] = 0;
    coeffY[term] = 0;
  }
  //  Apply Inverse to Both Dependent Vectors and Give Coefficients
  for (term = 0; term < iTerms; ++term)
  {
    D0 = 0;
    D1 = 0;
    for (row = 0; row < iTerms; ++row)
    {
      D0 += DU[row] * InvMat[term][row];
      D1 += DV[row] * InvMat[term][row];
    }
    coeffX[term] = D0;
    coeffY[term] = D1;
  }
  return 0;
}








