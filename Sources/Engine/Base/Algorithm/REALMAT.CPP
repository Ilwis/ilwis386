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
/*
// $Log: /ILWIS 3.0/Tools/REALMAT.CPP $
 * 
 * 5     16-08-01 18:59 Koolhoven
 * EigenVec() now also sets size of vEigVals 
 * 
 * 4     3/10/01 14:04 Retsios
 * Second attempt to do the same .. silly $Logger
 * 
 * 3     3/10/01 13:59 Retsios
 * Made the file compilable (wrong comment format used)
 * 
 * 2     9-03-01 17:56 Koolhoven
 * in operator == and != return false when number of rows or cols is not
 * equal, instead of giving an assertion error
 */
// Revision 1.2  1998/03/13 14:14:54  Wim
// In RealMatrix::Solve() only delete iRPivot after its last use.
//
/* tls/realmat.c
// MapList Interface for ILWIS 2.0
// october 1994, by Jan-Willem Rensman
// (c) Computer Department ITC
	Last change:  WK   28 Aug 98    4:37 pm
*/

//#include  "AdditionalHeaders\PCHeaders\dat.h"
#define REALMAT_C
#include "Headers\toolspch.h"
#include "Engine\Base\Algorithm\Realmat.h"
#include "Headers\Hs\DAT.hs"

#define errMat      3800
#define errMatSNG   (errMat + 1)
#define errMatSYM   (errMat + 2)
#define errMatITR   (errMat + 3)

class IMPEXP WhatErrorMatrix : public WhatError {
public:
  WhatErrorMatrix(IlwisError e) :
    WhatError(e) {}
  virtual String sWhat() const;
};

String WhatErrorMatrix::sWhat() const
{
  switch( err ) {
    case errMatSNG:
      return SDATErrSingular;
    case errMatSYM:
      return SDATErrNotSymmetrical;
    case errMatITR:
      return SDATErrTooManyIterations;
  }
  return "";
}

class IMPEXP ErrorMatrix : public ErrorObject {
public:
  ErrorMatrix(int e)
   : ErrorObject(WhatErrorMatrix(e))
   { where.SetTitle(SDATTitleMatrixError); }
};

RealMatrix::RealMatrix()
: BaseMatrix<double>(0, 0)
{
}

RealMatrix::RealMatrix(int iRow, int iCol)
: BaseMatrix<double>(iRow, iCol)
{
}

RealMatrix::RealMatrix(int iRow)
: BaseMatrix<double>(iRow, iRow)
{
}

RealMatrix::RealMatrix(const RealMatrix& a)
: BaseMatrix<double>(a)
{
}

RealMatrix::~RealMatrix()
{
}

void RealMatrix::operator  = (const RealMatrix& a)
{
   BaseMatrix<double>::operator = (a);
}

void RealMatrix::operator *= (const RealMatrix& b)
{
   assert(iCols() == b.iRows());  //Programmer's mistake.
   RealMatrix a(*this);               //    Should be checked before calculation.
   assert(p);
   delete [] p;
   _iRows = a.iRows();
   _iCols = b.iCols();
   _iLen = (long)iRows() * iCols();
   p = (double *) ::operator new ((unsigned long)(iLen() * sizeof(double)));
   for (int m = 0; m < iRows(); ++m)
   {
      for (int n = 0; n < iCols(); ++n)
      {
         double rProdSum = 0;
         for (int i = 0; i < a.iCols(); ++i)
            rProdSum += a.rVal(m, i) * b.rVal(i, n);
         rVal(m, n) = rProdSum;
      }
   }
}

void RealMatrix::operator += (const RealMatrix& b)
{
   assert(iCols() == b.iCols());  //Programmer's mistake.
   assert(iRows() == b.iRows());  //    Should be checked before calculation.
   for (long i = 0; i < iLen(); ++i)
   {
      rVal(i) += b.rVal(i);
   }
}

void RealMatrix::operator -= (const RealMatrix& b)
{
   assert(iCols() == b.iCols());  //Programmer's mistake.
   assert(iRows() == b.iRows());  //    Should be checked before calculation.
   for (long i = 0; i < iLen(); ++i)
   {
      rVal(i) -= b.rVal(i);
   }
}

void RealMatrix::operator *= (double c)
{
   for (long i = 0; i < iLen(); ++i)
   {
      rVal(i) *= c;
   }
}

void RealMatrix::operator += (double c)
{
   for (long i = 0; i < iLen(); ++i)
   {
      rVal(i) += c;
   }
}

void RealMatrix::operator -= (double c)
{
   for (long i = 0; i < iLen(); ++i)
   {
      rVal(i) -= c;
   }
}

bool operator == (const RealMatrix& a, const RealMatrix& b)
{
	if (a.iCols() != b.iCols())
		return false;
	if (a.iRows() == b.iRows())
		return false;
	for (long i=0; i < a.iLen(); ++i)
		if ( abs(a.rVal(i) - b.rVal(i)) >= DBL_EPSILON ) return false;
	return true;
}

bool operator != (const RealMatrix& a, const RealMatrix& b)
{
	if (a.iCols() != b.iCols())
		return false;
	if (a.iRows() == b.iRows())
		return false;
   for (long i=0; i < a.iLen(); ++i)
      if ( abs(a.rVal(i) - b.rVal(i)) >= DBL_EPSILON ) return true;
   return false;
}

void RealMatrix::PutColVec (int i, const CVector& v)
{
   assert(iRows() == v.iLen());
   for (int j = 0; j < iRows(); ++j) rVal(j, i) = v(j);
}

void RealMatrix::PutRowVec (int i, const RVector& v)
{
   assert(iCols() == v.iLen());
   for (int j = 0; j < iCols(); ++j) rVal(i, j) = v(j);
}

CVector RealMatrix::GetColVec (int i) const
{
   CVector v(iRows());
   for (int j = 0; j < iRows(); ++j) v(j) = rVal(j, i);
   return v;
}

RVector RealMatrix::GetRowVec (int i) const
{
   RVector v(iCols());
   for (int j = 0; j < iCols(); ++j) v(j) = rVal(i, j);
   return v;
}

void RealMatrix::Transpose()
{
   RealMatrix a(*this);
   assert(p);
   delete [] p;
   _iRows = a.iCols();
   _iCols = a.iRows();
   _iLen = a.iLen();
   p = (double *) ::operator new ((unsigned long)(iLen() * sizeof(double)));
   for (int m = 0; m < iRows(); ++m)
   {
      for (int n = 0; n < iCols(); ++n)
         rVal(m, n) = a.rVal(n, m);
   }
}

void RealMatrix::ElemProd(const RealMatrix& b)
{
   assert(iCols() == b.iCols());  //Programmer's mistake.
   assert(iRows() == b.iRows());  //    Should be checked before calculation.
   for (long i = 0; i < iLen(); ++i)
   {
      rVal(i) *= b.rVal(i);
   }
}



/***************************************************************************/
/*                         RealMatrix class implementations                  */
/***************************************************************************/




void RealMatrix::LUDcmp(int* iRowPivot, bool& fEven)
// L-U Decomposition of a matrix: in place!! so original matrix is destroyed!!!!
// in : matrix aij:  a11  a12  a13  a14
//                   a21  a22  a23  a24   =
//                   a31  a32  a33  a34
//                   a41  a42  a43  a44
//
//  out: matrices bij:                  and cij:
//                   1    0    0    0          c11  c12  c13  c14
//                   b21  1    0    0     *    0    c22  c23  c24
//                   b31  b32  1    0          0    0    c33  c34
//                   b41  b42  b43  1          0    0    0    c44
//
// the two output matrices are put in place of aij (the original one) like this:
//                   c11  c12  c13  c14
//                   b21  c22  c23  c24
//                   b31  b32  c33  c34
//                   b41  b42  b43  c44
//
// Det(a) now becomes: c11 * c22 * c33 * c44
// Invert(a) is calculated by backsubstitution (LUBksb()) of unit vectors.
// see p. 46 of Numerical Recipes.
// iRowPivot keeps track of the rows affected by partial pivoting
// during decomposition, and fEven records if the number of row interchanges
// is even or not. For specific details: read the souce or the book.
//
// Errors: -1  -> Matrix singular.

{
   assert(iRows() == iCols());
   int i, iMax, j, k;
   double rBig, rDum, rSum, rTemp;
   CVector vecV(iRows());
   fEven = true;
   for (i = 0; i < iRows(); ++i)
   {
     rBig=0.0;
     for ( j = 0; j < iRows(); ++j)
        if ((rTemp = abs(rVal(i, j))) > rBig) rBig = rTemp;
     if (abs(rBig) < DBL_EPSILON)
     { throw ErrorMatrix(errMatSNG); }  // Matrix singular!
     vecV(i) = 1.0 / rBig;
   }
   for (j = 0; j < iRows(); ++j)
   {
     for (i = 0; i < j; ++i)
     {
       rSum = rVal(i, j);
       for (k = 0; k < i; ++k) rSum -= rVal(i, k) * rVal(k, j);
       rVal(i, j) = rSum;
     }
     rBig = 0.0;
     for (i = j; i < iRows(); ++i)
     {
       rSum = rVal(i, j);
       for (k = 0; k < j; ++k)
         rSum -= rVal(i, k) * rVal(k, j);
       rVal(i, j) = rSum;
       if ((rDum = vecV(i) * abs(rSum)) >= rBig)
       {
         rBig = rDum;
         iMax = i;
       }
     }
     if (j != iMax)
     {
       for (k = 0; k < iRows(); ++k)
       {
         rDum = rVal(iMax, k);
         rVal(iMax, k) = rVal(j, k);
         rVal(j, k) = rDum;
       }
       fEven = (!fEven);
       vecV(iMax) = vecV(j);
     }
     iRowPivot[j] = iMax;
     if (abs(rVal(j, j)) < DBL_EPSILON)
     { throw ErrorMatrix(errMatSNG); } // Matrix singular!
     if (j != (iRows()-1))
     {
       rDum = 1.0 / rVal(j, j);
       for (i = j+1; i < iRows(); ++i) rVal(i, j) *= rDum;
     }
   }
   return;
}

// LUBksb yields the solution vector {x} to the equation: [a]{x}={b}.
// The vector {b} is backsubstituted, and in it's place the vector {x}
// is returned. The vector {b} (and after the function call: {x}) is
// vecSubstVec here. The array iRowPivot is returned by the LUDcmp
// function, and used here.
void  RealMatrix::LUBksb(int* iRowPivot, CVector& vecSubstVec)
{
   assert(iRows() == iCols());
   int i, ii = -1, ip, j;
   double rSum;
   for (i = 0; i < iRows(); ++i)
   {
     ip = iRowPivot[i];
     rSum = vecSubstVec(ip);
     vecSubstVec(ip) = vecSubstVec(i);
     if (ii != -1)
       for (j = ii; j <= i-1; ++j) rSum -= rVal(i, j) * vecSubstVec(j);
     else if (abs(rSum) >= DBL_EPSILON) ii = i;
     vecSubstVec(i) = rSum;
   }
   for (i = iRows()-1 ; i >= 0; --i)
   {
     rSum = vecSubstVec(i);
     for (j = i+1; j < iRows(); ++j) rSum -= rVal(i, j) * vecSubstVec(j);
     vecSubstVec(i) = rSum / rVal(i, i);
   }
 }

void  RealMatrix::Solve(const CVector& vecB, CVector& vecX)
{
   assert(iRows() == iCols());
   assert(iRows() == vecB.iLen());
   RealMatrix a(*this);
   int* iRPivot;
   iRPivot = new int[iRows()];
   bool   fEven;
   try {
     a.LUDcmp(iRPivot, fEven);
   }
   catch (const ErrorObject& err) {
     delete [] iRPivot;
     throw err;
   }
   vecX = vecB;
   a.LUBksb(iRPivot, vecX);
   delete [] iRPivot;
}

bool  RealMatrix::fSingular() const
{
   assert(iRows() == iCols());
   int* iRPivot;
   iRPivot = new int[iRows()];
   bool   fEven;
   RealMatrix a(*this);
   try {
     a.LUDcmp(iRPivot, fEven);
   }
   catch (const ErrorObject& ) {
     delete [] iRPivot;
     return true;
   }
   delete [] iRPivot;
   return false;
}

bool  RealMatrix::fSymmetric() const
{
   assert(iRows() == iCols());
   for (int r = 1; r < iRows(); ++r)
      for (int c = r - 1; c < iRows() - 1; ++c)
         if (abs(rVal(r, c) - rVal(c, r)) > DBL_EPSILON) return false;
   return true;
}

double  RealMatrix::rDet() const
{
   assert(iRows() == iCols());
   int* iRPivot;
   iRPivot = new int[iRows()];
   bool   fEven;
    RealMatrix a(*this);
   double rProd = 1.0;
   try {
     a.LUDcmp(iRPivot, fEven);
   }
   catch (const ErrorObject&) {
     delete [] iRPivot;
     return 0.0;
   }
   delete [] iRPivot;
   if (!fEven) rProd = -rProd;
   for (int i = 0; i < iRows(); ++i) rProd *= a(i, i);
   return rProd;
}

double  RealMatrix::rTrace() const
{
   assert(iRows() == iCols());
   double rTrce = 0.0;
   for (int i = 0; i < iRows(); ++i) rTrce += rVal(i, i);
   return rTrce;
}

void  RealMatrix::Invert()
{
   assert(iRows() == iCols());
    RealMatrix a(*this);
   int* iRPivot;
   iRPivot = new int[iRows()];
   CVector vecSubst(iRows());
   bool   fEven;
   try {
     a.LUDcmp(iRPivot, fEven);
   }
   catch (const ErrorObject& err) {
     delete [] iRPivot;
     throw err;
   }
   for (int j = 0; j < iRows(); ++j)
   {
      for (int i = 0; i < iRows(); ++i) vecSubst(i) = 0.0;
      vecSubst(j) = 1.0;
      a.LUBksb(iRPivot, vecSubst);
      for (int i = 0; i < iRows(); ++i) rVal(i, j) = vecSubst(i);
   }
   delete [] iRPivot;
   return;
}


// according to Numerical Recipes p. 474.
void  RealMatrix::Tred2(CVector& d, CVector& e, const bool fEigVecs)
{
  assert(iRows() == iCols());
  int l, k, j, i;
  double scale, hh, h, g, f;

  for (i = iRows()-1; i >= 1; --i)
   {
    l = i - 1;
    h = scale = 0.0;
    if (l > 0)
      {
      for (k = 0; k <= l; ++k)
        scale += abs(rVal(i, k));
      if (abs(scale) < DBL_EPSILON)
        e(i) = rVal(i, l);
      else
         {
        for (k = 0; k <= l; ++k)
            {
          rVal(i, k) /= scale;
          h += rVal(i, k) * rVal(i, k);
        }
        f = rVal(i, l);
        g = (f >= 0.0 ? -sqrt(h) : sqrt(h));
        e(i) = scale * g;
        h -= f * g;
        rVal(i, l) = f - g;
        f = 0.0;
        for (j = 0; j <= l; ++j)
        {
          if (fEigVecs) rVal(j, i) = rVal(i, j) / h;
          g = 0.0;
          for (k = 0; k <= j; ++k)
            g += rVal(j, k) * rVal(i, k);
          for (k = j + 1; k <= l; ++k)
            g += rVal(k, j) * rVal(i, k);
          e(j) = g / h;
          f += e(j) * rVal(i, j);
        }
        hh = f / (h + h);
        for (j = 0; j <= l; ++j)
        {
          f = rVal(i, j);
          e(j) = g = e(j) - hh * f;
          for (k = 0; k <= j; ++k)
             rVal(j, k) -= (f * e(k) + g * rVal(i, k));
        }
      }
    }
    else e(i) = rVal(i, l);
    d(i) = h;
  }
  if (fEigVecs) d(0) = 0.0;
  e(0) = 0.0;
  if (fEigVecs)
  {
      for (i = 0; i < iRows(); ++i)
      {
       l = i - 1;
       if (d(i) > DBL_EPSILON)
         {
         for (j = 0; j <= l; ++j)
            {
           g = 0.0;
           for (k = 0; k <= l; ++k)
             g += rVal(i, k) * rVal(k, j);
           for (k = 0; k <= l; ++k)
             rVal(k, j) -= g * rVal(k, i);
         }
       }
       d(i) = rVal(i, i);
       rVal(i, i) = 1.0;
       for (j = 0; j <= l; ++j) rVal(j, i) = rVal(i, j) = 0.0;
     }
   }
   else for (i = 0; i < iRows(); ++i) d(i) = rVal(i, i);
}

double  RealMatrix::dPythag(double a, double b) const
{
  double absa, absb, dsqrarg;
  absa = abs(a);
  absb = abs(b);
  if (absa > absb) return absa * sqrt(1.0 +
               ((dsqrarg = (absb / absa)) == 0.0 ? 0.0 : dsqrarg * dsqrarg));
  else return (absb == 0.0 ? 0.0 : absb * sqrt(1.0 +
               ((dsqrarg = (absa / absb)) == 0.0 ? 0.0 : dsqrarg * dsqrarg)));
}

void  RealMatrix::Tqli(CVector& d, CVector& e, const bool fEigVecs)
{
  int m, l, iter, i, k;
  double s, r, p, g, f, dd, c, b;

  for (i = 1; i < iRows(); ++i) e(i-1) = e(i);
  e(iRows()-1) = 0.0;
  for (l = 0; l < iRows(); ++l)
  {
    iter = 0;
    do
    {
      for (m = l; m < iRows()-1; ++m)
      {
        dd = abs(d(m)) + abs(d(m+1));
        if (abs((double)(abs(e(m)) + dd) - dd) < DBL_EPSILON) break;
      }
      if (m != l)
      {
        if (iter++ == 30)
        { throw ErrorMatrix(errMatITR); }  // error: Too many iterations!!
        g = (d(l+1) - d(l)) / (2.0 * e(l));
        r = dPythag(g, 1.0);
        g = d(m) - d(l) + e(l) / (g + ((g) >= 0.0 ? abs(r) : -abs(r)));
        s = c = 1.0;
        p = 0.0;
        for (i = m-1; i >= l; --i)
        {
          f = s * e(i);
          b = c * e(i);
          e(i+1) = (r = dPythag(f, g));
          if (abs(r) < DBL_EPSILON)
          {
            d(i+1) -= p;
            e(m) = 0.0;
            break;
          }
          s = f / r;
          c = g / r;
          g = d(i+1) - p;
          r = (d(i) - g) * s + 2.0 * c * b;
          d(i+1) = g + (p = s * r);
          g = c * r - b;
          if (fEigVecs)
          {
             for (k = 0; k < iRows(); ++k)
             {
               f = rVal(k, i+1);
               rVal(k, i+1) = s * rVal(k, i) + c * f;
               rVal(k, i) = c * rVal(k, i) - s * f;
             }
          }
        }
        if ((abs(r) < DBL_EPSILON) && (i >= l)) continue;
        d(l) -= p;
        e(l) = g;
        e(m) = 0.0;
      }
    }
    while (m != l);
  }
}

void  RealMatrix::EigenVal(CVector& vEigVals)
{
   assert(iRows() == iCols());
   if (!fSymmetric())
   { throw ErrorMatrix(errMatSYM); } // error: Matrix nonsymmetric!!
   bool fEigenVecs = false;
   CVector vSubDiag(iRows());
    RealMatrix mtDum(*this);
   mtDum.Tred2(vEigVals, vSubDiag, fEigenVecs);
   mtDum.Tqli(vEigVals, vSubDiag, fEigenVecs);
}

void  RealMatrix::EigenVec( RealMatrix& mEigVecs, CVector& vEigVals)
{
   assert(iRows() == iCols());
   if (!fSymmetric())
   { throw ErrorMatrix(errMatSYM); } // error: Matrix nonsymmetric!!
   bool fEigenVecs = true;
   vEigVals = CVector(iRows());
   CVector vSubDiag(iRows());
   mEigVecs = *this;
   mEigVecs.Tred2(vEigVals, vSubDiag, fEigenVecs);
   mEigVecs.Tqli(vEigVals, vSubDiag, fEigenVecs);
}


/***************************************************************************/
/*                        CVector class implementations                    */
/***************************************************************************/



void CVector::operator  = (const RVector& v)
{
   assert(p);
   delete [] p;
   _iRows = v.iCols();
   _iCols = v.iRows();
   _iLen = v.iLen();
   p = (double *) ::operator new ((unsigned long)(iLen() * sizeof(double)));
   if (!p) return;
   memcpy(p, v.p, iLen() * sizeof(double));
}

double  CVector::rNorm() const
{
   CVector vecDum(*this);
   return sqrt(rDotProd(vecDum));
}

double CVector::rDistance(const CVector& v) const
{
   assert(iLen() == v.iLen());
   double rSum = 0;
   for (long i = 0; i < iLen(); ++i)
      rSum += (rVal(i) - v.rVal(i)) * (rVal(i) - v.rVal(i));
   return sqrt(rSum);
}

double  CVector::rDotProd(const CVector& v) const
{
   assert(iLen() == v.iLen());
   double rSum = 0.0;
   for (long i = 0; i < iLen(); ++i)
      rSum += rVal(i) * v.rVal(i);
   return rSum;
}

CVector CVector::vecCrossProd(const CVector& v) const
{
   assert(iLen() == 3);
   assert(v.iLen() == 3);
   CVector vRes(3);
   vRes.rVal(0) = rVal(1) * v.rVal(2) - rVal(2) * v.rVal(1);
   vRes.rVal(1) = rVal(2) * v.rVal(0) - rVal(0) * v.rVal(2);
   vRes.rVal(2) = rVal(0) * v.rVal(1) - rVal(1) * v.rVal(0);
   return vRes;
}

double  CVector::rAngleDeg(const CVector& v) const
{
    return (rAngleRad(v) / M_PI) * 180;  // 180 deg = 1 PI
}

double  CVector::rAngleRad(const CVector& v) const
{
   double cosTheta;
   cosTheta = rDotProd(v) / (rNorm() * v.rNorm());
   assert(abs(cosTheta) <= 1);
   return acos(cosTheta);
}



/***************************************************************************/
/*                        RVector class implementations                    */
/***************************************************************************/


void RVector::operator  = (const CVector& v)
{
   assert(p);
   delete [] p;
   _iRows = v.iCols();
   _iCols = v.iRows();
   _iLen = v.iLen();
   p = (double *) ::operator new ((unsigned long)(iLen() * sizeof(double)));
   if (!p) return;
   memcpy(p, v.p, iLen() * sizeof(double));
}

double  operator  * (const RVector& vR, const CVector& vC)
{
   assert(vR.iLen() == vC.iLen());
   double rPr = 0.0;
   for (long i = 0; i < vR.iLen(); ++i)
      rPr += vR(i) * vC(i);
   return rPr;
}

 RealMatrix  _export operator  * (const  RealMatrix& a, const  RealMatrix& b)
  {  RealMatrix m(a); m *= b; return m; }
 RealMatrix  _export operator  + (const  RealMatrix& a, const  RealMatrix& b)
  {  RealMatrix m(a); m += b; return m; }
 RealMatrix  _export operator  - (const  RealMatrix& a, const  RealMatrix& b)
  {  RealMatrix m(a); m -= b; return m; }
 RealMatrix  _export operator  * (const  RealMatrix& a, double c)
  {  RealMatrix m(a); m *= c; return m; }
 RealMatrix  _export operator  * (double c, const  RealMatrix& a)
  {  RealMatrix m(a); m *= c; return m; }
 RealMatrix  _export operator  / (const  RealMatrix& a, double c)
  { assert(abs(c) >= DBL_EPSILON);  RealMatrix m(a); m *= 1.0 / c; return m; }
CVector  operator  * (const CVector& a, const CVector& b)
   { CVector m(a); m *= b; return m; }
CVector  operator  + (const CVector& a, const CVector& b)
   { CVector m(a); m += b; return m; }
CVector  operator  - (const CVector& a, const CVector& b)
   { CVector m(a); m -= b; return m; }
CVector  operator  * (const CVector& a, double c)
   { CVector m(a); m *= c; return m; }
CVector  operator  * (double c, const CVector& a)
   { CVector m(a); m *= c; return m; }
CVector  operator  / (const CVector& a, double c)
   { assert(abs(c) >= DBL_EPSILON); CVector m(a); m *= 1.0 / c; return m; }
RVector  operator  * (const RVector& a, const RVector& b)
  { RVector m(a); m *= b; return m; }
RVector  operator  + (const RVector& a, const RVector& b)
  { RVector m(a); m += b; return m; }
RVector  operator  - (const RVector& a, const RVector& b)
  { RVector m(a); m -= b; return m; }
RVector  operator  * (const RVector& a, double c)
  { RVector m(a); m *= c; return m; }
RVector  operator  * (double c, const RVector& a)
  { RVector m(a); m *= c; return m; }
RVector  operator  / (const RVector& a, double c)
  { assert(abs(c) >= DBL_EPSILON); RVector m(a); m *= 1.0 / c; return m; }








