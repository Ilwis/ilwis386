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
// dat/matrix.h
// Matrix Interface for ILWIS 2.0
// october 1994, by Jan-Willem Rensman
// (c) Computer Department ITC

#ifndef ILWMATRIX_H
#define ILWMATRIX_H

#include "Engine\Base\Algorithm\Basemat.h"

#undef IMPEXP
#ifdef ILWISENGINE
#define IMPEXP __export
#else
#define IMPEXP __import
#endif

class IMPEXP RealMatrix;

class IMPEXP CVector;       //Base type CVector (column vector).
                         //Defined as r*1 matrix. Derived from RealMatrix.
class IMPEXP RVector;       //Transposed CVector (row vector).
                         //Defined as 1*c matrix. Derived from CVector.


class IMPEXP RealMatrix : public BaseMatrix<double>
                //No conditions preset for this base type matrix
                //The elememts are of 'double' type
{

public:
  RealMatrix();
  RealMatrix(int iRow, int iCol);
  RealMatrix(int iRow);
  RealMatrix(const RealMatrix& a);
  ~RealMatrix();

  void           operator  = (const RealMatrix&);
  void           operator *= (const RealMatrix&);
  void           operator += (const RealMatrix&);
  void           operator -= (const RealMatrix&);
  void           operator *= (double);
  void           operator += (double);
  void           operator -= (double);
  void           PutColVec (int, const CVector&);
  void           PutRowVec (int, const RVector&);
  CVector        GetColVec (int) const;
  RVector        GetRowVec (int) const;
  friend RealMatrix  _export operator  * (const RealMatrix& a, const RealMatrix& b);
  friend RealMatrix  _export operator  + (const RealMatrix& a, const RealMatrix& b);
  friend RealMatrix  _export operator  - (const RealMatrix& a, const RealMatrix& b);
  friend RealMatrix  _export operator  * (const RealMatrix& a, double c);
  friend RealMatrix  _export operator  * (double c, const RealMatrix& a);
  friend RealMatrix  _export operator  / (const RealMatrix& a, double c);
  friend bool _export operator == (const RealMatrix&, const RealMatrix&);
  friend bool _export operator != (const RealMatrix&, const RealMatrix&);

  String sType() const { return "Matrix"; }
  bool fSquare() const { return iRows() == iCols(); }

   //Transposing of a matrix
  void Transpose();

   //Element multiplication: m11=a11*b11, m12=a12*b12, m13=a13*b13, ...
  void ElemProd(const RealMatrix&);
  double& rVal (int i) { return tVal(i); }
  double  rVal (int i) const { return tVal(i); }

protected:
  double& rVal (int iR, int iC) { return tVal(iR, iC); }
  double  rVal (int iR, int iC) const { return tVal(iR, iC); }

/*  
};


class SqrMatrix: public Matrix   //These are matrices with nrRows = nrCols
                                 //For Square matrices the following
                                 //matrix functions are meaningful:
                                 //- Determinant of a matrix: Det(a).
                                 //- Trace of a matrix: = a11 + a22 + a33 +...
                                 //- Inverse of a matrix. If (Det(a) != 0) !!!
                                 //
                                 //For the symmetric matrix type
                                 //a(i, j) = a(j, i). There is a
                                 //function to test this.
                                 //Of course, this is always a
                                 //square matrix.
                                 //For the symmetric matrices there are
                                 //functions to calculate eigen values
                                 //and eigen vectors.


{
	Last change:  WK   28 Aug 98    4:37 pm
*/
public:
//  SqrMatrix(int iSize = 1) : Matrix(iSize, iSize) {}
//  SqrMatrix(const SqrMatrix& v) : Matrix(v) {}
//  SqrMatrix(const String& s, const bool fReadOnly)
//            : Matrix(s, fReadOnly) {}
/*
  friend SqrMatrix  operator  * (const SqrMatrix& a, const SqrMatrix& b);
  friend SqrMatrix  operator  + (const SqrMatrix& a, const SqrMatrix& b);
  friend SqrMatrix  operator  - (const SqrMatrix& a, const SqrMatrix& b);
  friend SqrMatrix  operator  * (const SqrMatrix& a, double c);
  friend SqrMatrix  operator  * (double c, const SqrMatrix& a);
  friend SqrMatrix  operator  / (const SqrMatrix& a, double c);*/
  double rDet() const;
  double rTrace() const;
  void Invert();  // Errors: errMatSNG  -> Matrix singular.

  void EigenVal(CVector&); // errors: errMatSYM: nonsymmetric matrix
                           //   errMatITR: too many iterations in Tqli
  void EigenVec(RealMatrix&, CVector&); // errors: errMatSYM: nonsymmetric matrix
                                       //   errMatITR: too many iterations in Tqli
  bool fSingular() const;
  bool fSymmetric() const;

  // Solve the linear set of equations [A]{x}={b} by the LU-Decomposition
  // and backsubstitution as in the private functions below. The first vector
  // contains the vector {b}, and the second vector the solution vector {x}.
  void Solve(const CVector&, CVector&);

protected:

  // Lower-Upper decomposition of a matrix according to 'Numerical Recipes',
  // Press, William H. et al., Cambridge University press, 2nd Ed. p.46
  // Errors: -1  -> Matrix singular.
  void LUDcmp(int*, bool&); //Error: errMatSNG: matix singular

  // Backsubstitution of vector {b} in LUdecomposed matrix [a] to obtain
  // the solution vector {x} to [a]{x}={b}. {x} is given back in place of
  // {b}, which is destroyed. Numerical Recipes p.47.
  void LUBksb(int*, CVector&);

  // Tred2 makes use of the householder method to reduce a symmetric
  // matrix to it's tridiagonal form. according to Numerical Recipes p. 474.
  void Tred2(CVector&, CVector&, const bool);

private:

  // Tqli calculates Eigen values, and if required, also the correspoding
  // Eigen vectors. according to Numerical Recipes p. 480.
  void Tqli(CVector&, CVector&, const bool); //Error: errMatITR: too many
                                                        // iterations in Tqli
  // function used by Tqli
  double dPythag(double a, double b) const;
};

class IMPEXP CVector: public RealMatrix     // The vector class is derived from the
                                 // matrix class for reasons of consistency
                                 // in operators. The general rules of matrix
                                 // calculation apply to vector calculation
                                 // if they are considered r*1 matrices, as
                                 // is implemented here.
{
protected:
   CVector(int , int iCol) : RealMatrix(1, iCol) {}

public:
   friend RVector;
   CVector(int iRow = 1) : RealMatrix(iRow, 1) {}
   CVector(const CVector& v) : RealMatrix(v) {}
   friend CVector  IMPEXP operator  * (const CVector& a, const CVector& b);
   friend CVector  IMPEXP operator  + (const CVector& a, const CVector& b);
   friend CVector  IMPEXP operator  - (const CVector& a, const CVector& b);
   friend CVector  IMPEXP operator  * (const CVector& a, double c);
   friend CVector  IMPEXP operator  * (double c, const CVector& a);
   friend CVector  IMPEXP operator  / (const CVector& a, double c);
   double&        operator () (int iL)         { return rVal(iL); }
   double         operator () (int iL) const   { return rVal(iL); }
   void operator  = (const CVector& v)    { BaseMatrix<double>::operator = (v); }
   void operator  = (const RVector&);

    //Returns the norm (length) of this* vector: Sqrt((u1*u1)+(u2*u2)+...)
   double  rNorm() const;

    //Returns the distance between this* vector and an other vector:
    //Sqrt(Sqr(u1-v1)+Sqr(u2-v2)+...))
   double rDistance(const CVector&) const;

    //Returns the dot (inner) product of this* vector and an other vector:
    //(u1*v1)+(u2*v2)+...
   double  rDotProd(const CVector&) const;

    //Returns the cross product of this* vector and an other vector:
    //i(u2v3-u3v2)+j(u3v1-u1v3)+k(u1v2-u2v1) !!!!! for n = 3 only!!!!
   CVector vecCrossProd(const CVector&) const;

    //Returns the angle between this* vector and an other vector in degrees:
    //0<=theta<=180
   double  rAngleDeg(const CVector&) const;

    //Returns the angle between this* vector and an other vector in radians:
    //0<=theta<=PI
   double  rAngleRad(const CVector&) const;
};


class IMPEXP RVector: public CVector
{
public:
  friend CVector;
  RVector(int iCol = 1) : CVector(1, iCol) {}
  RVector(const RVector& v) : CVector(v) {}
  friend RVector IMPEXP  operator  * (const RVector& a, const RVector& b);
  friend RVector IMPEXP  operator  + (const RVector& a, const RVector& b);
  friend RVector IMPEXP  operator  - (const RVector& a, const RVector& b);
  friend RVector IMPEXP  operator  * (const RVector& a, double c);
  friend RVector IMPEXP  operator  * (double c, const RVector& a);
  friend RVector IMPEXP  operator  / (const RVector& a, double c);
  void operator  = (const CVector&);
  void operator  = (const RVector& v)   { BaseMatrix<double>::operator = (v); }
  friend double  IMPEXP operator  * (const RVector&, const CVector&);
};

bool _export operator != (const RealMatrix&, const RealMatrix&);

#endif







