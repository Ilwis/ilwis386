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
   Least Squares Fit
   january 1997, by Wim Koolhoven
   (c) Ilwis System Development, ITC
	Last change:  WK   28 Aug 98    4:13 pm
*/
#include "Engine\Base\Algorithm\Realmat.h"

#undef IMPEXP
#ifdef ILWISENGINE
#define IMPEXP __export
#else
#define IMPEXP __import
#endif

class IMPEXP LeastSquaresFit
{
protected:
  LeastSquaresFit(const CVector& cvXData, const CVector& cvYData, int iTerms);
public:
  virtual ~LeastSquaresFit();
  double rStdDev() { return _rStdDev; }
  virtual String sFormula() const=0;
  virtual double rCalc(double X) const=0;
protected:
  void Compute();
  virtual void Transform();
  virtual void CreateBasisFunctions()=0;
  virtual void InverseTransform(double rYFit);
  virtual void TransformSolution();
  const CVector cvX, cvY;
  CVector cvSolution, cvW, cvZ;
  RealMatrix mtBasis;
  double rConstant, rMultiplier;
  const int iNrPoints, iNrTerms;
private:  
  void InitializeAndFormBasisVectors();
  void CreateAndSolveEquations();
  void FinalizeSolution();
  double _rStdDev;
};

class IMPEXP LeastSquaresFitPolynomial: public LeastSquaresFit
{ // Y = a0 + a1 * X + a2 * X^2 + ...
public:
  LeastSquaresFitPolynomial(const CVector& cvXData, const CVector& cvYData, int iOrder);
  virtual String sFormula() const;
  virtual double rCalc(double X) const;
protected:
  virtual void Transform();
  virtual void CreateBasisFunctions();
  virtual void TransformSolution();
private:  
  void WritePoly(int iTerm, ostream& os) const;
  double rFuncPoly(int iTerm, double rX) const;
};

class IMPEXP LeastSquaresFitTrigonometric: public LeastSquaresFit
{ // Y = a0 + a1 * cos(X) + a2 * sin(X) + ...
public:
  LeastSquaresFitTrigonometric(const CVector& cvXData, const CVector& cvYData, int iTerms);
  virtual String sFormula() const;
  virtual double rCalc(double X) const;
protected:
  virtual void CreateBasisFunctions();
private:  
  void WriteFourier(int iTerm, ostream& os) const;
  double rFuncFourier(int iTerm, double rX) const;
};

class IMPEXP LeastSquaresFitPower: public LeastSquaresFit
{ // Y = a0 * X^a1
public:
  LeastSquaresFitPower(const CVector& cvXData, const CVector& cvYData);
  virtual String sFormula() const;
  virtual double rCalc(double X) const;
protected:
  virtual void Transform();
  virtual void CreateBasisFunctions();
  virtual void InverseTransform(double rYFit);
  virtual void TransformSolution();
};

class IMPEXP LeastSquaresFitExponential: public LeastSquaresFit
{ // Y = a0 * exp(a1 * X)
public:
  LeastSquaresFitExponential(const CVector& cvXData, const CVector& cvYData);
  virtual String sFormula() const;
  virtual double rCalc(double X) const;
protected:
  virtual void Transform();
  virtual void CreateBasisFunctions();
  virtual void InverseTransform(double rYFit);
  virtual void TransformSolution();
};

class IMPEXP LeastSquaresFitLogarithmic: public LeastSquaresFit
{ // Y = a0 * log(a1 * X)
public:
  LeastSquaresFitLogarithmic(const CVector& cvXData, const CVector& cvYData);
  virtual String sFormula() const;
  virtual double rCalc(double X) const;
protected:
  virtual void Transform();
  virtual void CreateBasisFunctions();
  virtual void TransformSolution();
};









