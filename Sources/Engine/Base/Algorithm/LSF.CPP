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
#define LSF_C
#include "Headers\toolspch.h"
#include "Engine\Base\Algorithm\Lsf.h"
#include "Engine\Base\Algorithm\Lstsqrs.h"
#include "Headers\Err\Ilwisapp.err"

#define EPS10 1e-10

LeastSquaresFit::LeastSquaresFit(const CVector& cvXData, const CVector& cvYData, int iTerms)
: cvX(cvXData), cvY(cvYData), 
  cvSolution(iTerms), mtBasis(cvXData.iRows(), iTerms),
  cvW(cvXData), cvZ(cvYData),
  iNrTerms(iTerms), 
  iNrPoints(cvXData.iRows())
{
}

LeastSquaresFit::~LeastSquaresFit()
{
}

static void ErrorTooLessPoints()
{
  throw ErrorObject(String(SDATErrLSFNotEnoughPoints), 
                    errLeastSquaresFit + 1);
}

static void ErrorTooLessTerms()
{
  throw ErrorObject(String(SDATErrLSFNotEnoughTerms), 
                    errLeastSquaresFit + 2);
}

static void ErrorMoreTermsThanPoints()
{
  throw ErrorObject(String(SDATErrLSFPointsTermsMismatch), 
                    errLeastSquaresFit + 3);
}

static void ErrorNoPossibleSolution()
{
  throw ErrorObject(String(SDATErrLSFNoSolution), 
                    errLeastSquaresFit + 4);
}

void LeastSquaresFit::Compute()
{
  InitializeAndFormBasisVectors();
  CreateAndSolveEquations();
  FinalizeSolution();
}

void LeastSquaresFit::InitializeAndFormBasisVectors()
{
  if (iNrPoints < 2)
    ErrorTooLessPoints();
  if (iNrTerms <= 1)
    ErrorTooLessTerms();
  if (iNrTerms >= iNrPoints)
    ErrorMoreTermsThanPoints();
  for (int i = 0; i < cvSolution.iRows(); ++i)
    cvSolution(i) = 0;
  Transform();
  CreateBasisFunctions();      
}

void LeastSquaresFit::CreateAndSolveEquations()
{
  cvSolution = LeastSquares(mtBasis, cvZ);
}

void LeastSquaresFit::FinalizeSolution()
{
  double rSum = 0;
  for (int index = 0; index < iNrPoints; ++index) {
    double rYFit = 0;
    for (int term = 0; term < iNrTerms; ++term) 
      rYFit += cvSolution(term) * mtBasis(index, term);
    InverseTransform(rYFit);
    rSum += (rYFit - cvY(index)) * (rYFit - cvY(index));
  }
  if (iNrPoints == iNrTerms)
    _rStdDev = 0;
  else 
    _rStdDev = sqrt(rSum / (iNrPoints - iNrTerms));  
    
  TransformSolution();
}

void LeastSquaresFit::Transform()
{
  // default implementation is empty
  // because constructor already copies cvX to cvW and cvY to cvZ
}

void LeastSquaresFit::TransformSolution()
{
  // default implementation is empty
}

void LeastSquaresFit::InverseTransform(double rYFit)
{
  // default implementation is empty
}


LeastSquaresFitPolynomial::LeastSquaresFitPolynomial(const CVector& cvXData, const CVector& cvYData, int order)
: LeastSquaresFit(cvXData, cvYData, order)
{
  Compute();
}

void LeastSquaresFitPolynomial::WritePoly(int iTerm, ostream& os) const
{
  double r = cvSolution(iTerm);
  if (abs(r) < EPS10)  r = 0.0;
  os << r;
  iTerm += 1;
  if (iTerm == iNrTerms)
    return;
  os << " + X * (";
  WritePoly(iTerm, os);
  os << ")";  
}

String LeastSquaresFitPolynomial::sFormula() const
{
//  String sText = String("Equation of Fitting Polynomial Function:\r\n");
  String sText = String("Y =");
  int iTerm = 0;
  double r = cvSolution(0);
  if (abs(r) < EPS10)  r = 0.0;
  String sPolyn = String("  %.6f", r);
  for ( iTerm = 1; iTerm < iNrTerms; iTerm++) {
    r = cvSolution(iTerm);
    if (abs(r) < EPS10)  r = 0.0;
    if (iTerm == 1)
      sPolyn &= String("  + %.6f * X", r);
    else
      sPolyn &= String("  + %.6f * X^%d", r, iTerm);
  }
  /*char sBuf[400];
  ostrstream str(sBuf, 200);
  WritePoly(0,str);
  String s(str.str());
  */
  sText &= sPolyn;
  return sText;
}

double LeastSquaresFitPolynomial::rFuncPoly(int iTerm, double rX) const
{
  if (iTerm + 1 == iNrTerms)
    return cvSolution(iTerm);
  else
    return cvSolution(iTerm) + rX * rFuncPoly(iTerm+1, rX);  
}

double LeastSquaresFitPolynomial::rCalc(double rX) const
{
  return rFuncPoly(0, rX);
}

void LeastSquaresFitPolynomial::Transform()
{
  int i;
  double rXMin = cvX(0);
  double rXMax = cvX(0);
  for (i = 1; i < iNrPoints; ++i) {
    if (rXMin > cvX(i))
      rXMin = cvX(i);
    else if (rXMax < cvX(i))
      rXMax = cvX(i);
  }    
  rMultiplier = 2.0 / (rXMax - rXMin);
  rConstant = - rMultiplier * (rXMax + rXMin) / 2.0;
  for (i = 0; i < iNrPoints; ++i)
    cvW(i) = rMultiplier * cvX(i) + rConstant;
//  ZDAta := YData;
}

void LeastSquaresFitPolynomial::CreateBasisFunctions()
{
  int i, j;
  for (i = 0; i < iNrPoints; ++i) {
    mtBasis(i, 0) = 1;
    mtBasis(i, 1) = cvW(i);
    for (j = 2; j < iNrTerms; ++j)
      mtBasis(i, j) = 2 * cvW(i) * mtBasis(i, j - 1) - mtBasis(i, j - 2);
  }    
}

void LeastSquaresFitPolynomial::TransformSolution()
{
  int index, term;
  double rSum;
  CVector cvOldConversion(iNrTerms), cvConversion(iNrTerms);
  CVector cvOldSolution(cvSolution);
  for (int i = 0; i < iNrTerms; ++i)
    cvOldConversion(i) = 0;
  for (index = 0; index < iNrTerms; ++index) {
    rSum = 0;
    if (index > 0)
      cvConversion(index - 1) = 0;
    for (term = index; term < iNrTerms; ++term) {
      if (term == 0)
        cvConversion(term) = 1.0;
      else if (term == 1)
        if (index == 0)
          cvConversion(term) = rConstant;
        else
          cvConversion(term) = rMultiplier;
      else
        cvConversion(term) = 2 * rMultiplier * cvOldConversion(term - 1)
                           + 2 * rConstant * cvConversion(term - 1)
                           - cvConversion(term - 2);
      rSum += cvConversion(term) * cvOldSolution(term);
    }  
    cvSolution(index) = rSum;
    cvOldConversion = cvConversion;
  }
}

LeastSquaresFitTrigonometric::LeastSquaresFitTrigonometric(
  const CVector& cvXData, const CVector& cvYData, int iTerms)
: LeastSquaresFit(cvXData, cvYData, iTerms)
{
  if (iNrTerms <= 2)
    ErrorTooLessTerms();
  Compute();
}

String LeastSquaresFitTrigonometric::sFormula() const
{
//String sText = String("%S:\r\n", SDATMsgLSFFourierFitFormula);
  String sText("Y = ");
  int iTerm = 0;
  double r = cvSolution(0);
  if (abs(r) < EPS10)  r = 0.0;
  String sFourier = String("  %.3f", r);
  for ( iTerm = 1; iTerm < iNrTerms; iTerm++) {
    r = cvSolution(iTerm);
    if (abs(r) < EPS10)  r = 0.0;
    if (iTerm % 2)  {
      sFourier &= String("  + %.3f * cos(", r);
      if (iTerm == 1)
        sFourier &= String("X)");
      else
        sFourier &= String("%dX)", (iTerm+1)/2 );
    }
    else {
      sFourier &= String("  + %.3f * sin(", r);
      if (iTerm == 2)
        sFourier &= String("X)\r\n");
      else
        sFourier &= String("%dX)\r\n", iTerm/2 );
    }
  }
  /*char sBuf[400];
  ostrstream str(sBuf, 200);
  WriteFourier(0,str);
  String s(str.str());
  */
  sText &= sFourier;
  return sText;
}

void LeastSquaresFitTrigonometric::WriteFourier(int iTerm, ostream& os) const
{
  double r = cvSolution(iTerm);
  if (abs(r) < EPS10)  r = 0.0;
  os << r;
  if (iTerm > 0) {
    if (iTerm % 2)
      os << " * cos(";
    else  
      os << " * sin(";
    if (iTerm > 2)
      os << (iTerm+1) / 2 << " * X)";  
  }    
  if (iTerm + 1 == iNrTerms)
    return;
  os << " + ";
  WriteFourier(iTerm+1, os);  
}

double LeastSquaresFitTrigonometric::rCalc(double rX) const
{
  return rFuncFourier(0, rX);
}

double LeastSquaresFitTrigonometric::rFuncFourier(int iTerm, double rX) const
{
  double rVal = cvSolution(iTerm);
  if (iTerm >= 1)
    if (iTerm % 2)
      rVal *= cos((iTerm+1)/2 * rX);
    else  
      rVal *= sin((iTerm+1)/2 * rX);
  if (iTerm + 1 == iNrTerms)
    return rVal;
  rVal += rFuncFourier(iTerm+1, rX);
  return rVal;  
}

void LeastSquaresFitTrigonometric::CreateBasisFunctions()
{
  int i, j;
  for (i = 0; i < iNrPoints; ++i) {
    mtBasis(i, 0) = 1;
    mtBasis(i, 1) = cos(cvW(i));
    mtBasis(i, 2) = sin(cvW(i));
    for (j = 3; j < iNrTerms; ++j)
      if (j % 2)
        mtBasis(i, j) = mtBasis(i, 2) * mtBasis(i, j - 2)
                      - mtBasis(i, 3) * mtBasis(i, j - 1);
      else  
        mtBasis(i, j) = mtBasis(i, 3) * mtBasis(i, j - 3)
                      + mtBasis(i, 2) * mtBasis(i, j - 2);
  }    
}

LeastSquaresFitPower::LeastSquaresFitPower(const CVector& cvXData, const CVector& cvYData)
: LeastSquaresFit(cvXData, cvYData, 2)
{
  Compute();
}

double LeastSquaresFitPower::rCalc(double rX) const
{
  if (rX < 0)
    return rUNDEF;
  return cvSolution(0) * pow(rX, cvSolution(1));  
}

String LeastSquaresFitPower::sFormula() const
{
//  String sText = String("%S:\r\n", SDATMsgLSFPowerFitFormula);
  String sText("Y = ");
  double rS0 = cvSolution(0);
  double rS1 = cvSolution(1);
  if (abs(rS0) < EPS10)  rS0 = 0.0;
  if (abs(rS1) < EPS10)  rS1 = 0.0;
  String s = String("%g * pow(X, %g)", rS0, rS1);
  sText &= s;
  return sText;
}

void LeastSquaresFitPower::Transform()
{
  if (cvY(0) < 0)
    rMultiplier = -1;
  else  
    rMultiplier = 1;
  double rYVal;  
  for (int i = 0; i < iNrPoints; ++i) {
    if (cvX(i) <= 0)
      ErrorNoPossibleSolution();
    rYVal = rMultiplier * cvY(i);
    if (rYVal <= 0)
      ErrorNoPossibleSolution();
    cvW(i) = log(cvX(i));
    cvZ(i) = log(rYVal);  
  }  
}

void LeastSquaresFitPower::InverseTransform(double rYFit)
{
  double rY = rYFit;
  rYFit = rMultiplier * exp(rY);
}

void LeastSquaresFitPower::CreateBasisFunctions()
{
  int i;
  for (i = 0; i < iNrPoints; ++i) {
    mtBasis(i, 0) = 1;
    mtBasis(i, 1) = cvW(i);
  }    
}  

void LeastSquaresFitPower::TransformSolution()
{
  cvSolution(0) = rMultiplier * exp(cvSolution(0));
}  


LeastSquaresFitExponential::LeastSquaresFitExponential(const CVector& cvXData, const CVector& cvYData)
: LeastSquaresFit(cvXData, cvYData, 2)
{
  Compute();
}

double LeastSquaresFitExponential::rCalc(double rX) const
{
  return cvSolution(0) * exp(cvSolution(1) * rX);  
}

String LeastSquaresFitExponential::sFormula() const
{
//  String sText = String("%S:\r\n", SDATMsgLSFExpFitFormula);
  String sText("Y = ");
  double rS0 = cvSolution(0);
  double rS1 = cvSolution(1);
  if (abs(rS0) < EPS10)  rS0 = 0.0;
  if (abs(rS1) < EPS10)  rS1 = 0.0;
  String s = String("%g * exp(%g * X)", rS0, rS1);
  sText &= s;
  return sText;
}



void LeastSquaresFitExponential::Transform()
{
  if (cvY(0) < 0)
    rMultiplier = -1;
  else  
    rMultiplier = 1;
  double rYVal;  
  for (int i = 0; i < iNrPoints; ++i) {
    rYVal = rMultiplier * cvY(i);
    if (rYVal <= 0)
      ErrorNoPossibleSolution();
    cvZ(i) = log(rYVal);  
  }  
}    

void LeastSquaresFitExponential::InverseTransform(double rYFit)
{
  double rY = rYFit;
  rYFit = rMultiplier * exp(rY);
}  

void LeastSquaresFitExponential::CreateBasisFunctions()
{
  int i;
  for (i = 0; i < iNrPoints; ++i) {
    mtBasis(i, 0) = 1;
    mtBasis(i, 1) = cvW(i);
  }    
}  

void LeastSquaresFitExponential::TransformSolution()
{
  cvSolution(0) = rMultiplier * exp(cvSolution(0));
}  

LeastSquaresFitLogarithmic::LeastSquaresFitLogarithmic(const CVector& cvXData, const CVector& cvYData)
: LeastSquaresFit(cvXData, cvYData, 2)
{
  Compute();
}

double LeastSquaresFitLogarithmic::rCalc(double rX) const
{
  rX *= cvSolution(1);
  if (rX <= 0)
    return rUNDEF;
  return cvSolution(0) * log(rX);  
}

String LeastSquaresFitLogarithmic::sFormula() const
{
//  String sText = String("%S:\r\n", SDATMsgLSFLogFitFormula);
  String sText("Y = ");
  double rS0 = cvSolution(0);
  double rS1 = cvSolution(1);
  if (abs(rS0) < EPS10)  rS0 = 0.0;
  if (abs(rS1) < EPS10)  rS1 = 0.0;
  String s = String("%g * log(%g * X)", rS0, rS1);
  sText &= s;
  return sText;
}


void LeastSquaresFitLogarithmic::Transform()
{
  if (cvX(0) < 0)
    rMultiplier = -1;
  else  
    rMultiplier = 1;
  double rXVal;  
  for (int i = 0; i < iNrPoints; ++i) {
    rXVal = rMultiplier * cvX(i);
    if (rXVal <= 0)
      ErrorNoPossibleSolution();
    cvW(i) = log(rXVal);  
  }  
}
  
void LeastSquaresFitLogarithmic::CreateBasisFunctions()
{
  int i;
  for (i = 0; i < iNrPoints; ++i) {
    mtBasis(i, 0) = cvW(i);
    mtBasis(i, 1) = 1;
  }    
}  

void LeastSquaresFitLogarithmic::TransformSolution()
{
  cvSolution(1) = rMultiplier * exp(cvSolution(1)/cvSolution(0));
}  









