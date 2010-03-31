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
/* CoordSystemTiePoints
   Copyright Ilwis System Development ITC
   april 1998, by Wim Koolhoven
	Last change:  JH   17 Sep 99    8:14 am
*/

#include "Engine\SpatialReference\csctp.h"
#include "Engine\SpatialReference\Coordsys.h"
#include "Engine\SpatialReference\CoordSystemTiePoints.H"
#include "Engine\SpatialReference\SolveOblique.h"
#include "Engine\Base\Algorithm\Fpolynom.h"
#include "Engine\Base\DataObjects\ERR.H"

#define EPS10 1.e-10

CoordSystemTiePoints::CoordSystemTiePoints(const FileName& fn)
: CoordSystemCTP(fn)
{
  String s = "Affine";
  ReadElement("CoordSystemTiePoints", "Transformation", s);
  if (s == "Conform")
    transf = CONFORM;
  else if (s == "Affine")  
    transf = AFFINE;
  else if (s == "SecondOrder")  
    transf = SECONDORDER;
  else if (s == "FullSecondOrder")  
    transf = FULLSECONDORDER;
  else if (s == "ThirdOrder")  
    transf = THIRDORDER;
  else if (s == "Projective")  
    transf = PROJECTIVE;
  else
    transf = AFFINE;
  //rmJR2C = RealMatrix(2);
  rmJC2R = RealMatrix(2);
  Compute();    
}

CoordSystemTiePoints::CoordSystemTiePoints(const FileName& fn, const CoordSystem& csRef, const FileName& fnRefMap)
: CoordSystemCTP(fn, csRef, fnRefMap)
{
  transf = AFFINE;
  //rmJR2C = RealMatrix(2);
  rmJC2R = RealMatrix(2);
/*
  try {
    BaseMap map(fnRefMap);
    if (map.fValid())
      cb = map->cb();
  }
  catch (ErrorObject&) {
  } */
}

CoordSystemTiePoints::~CoordSystemTiePoints()
{
  if (fErase)
    tblCTP->fErase = true;
}

void CoordSystemTiePoints::Store()
{
  if (fValid()) {
    SetAdditionalInfo(sFormula());
    SetAdditionalInfoFlag(true);
  }
  else {
    SetAdditionalInfoFlag(false);
  }
  CoordSystemCTP::Store();
  WriteElement("CoordSystem", "Type", "TiePoints");

  String s;
  switch (transf) {
    case CONFORM:         s = "Conform";         break;
    case AFFINE:          s = "Affine";          break;
    case SECONDORDER:     s = "SecondOrder";     break;
    case FULLSECONDORDER: s = "FullSecondOrder"; break;
    case THIRDORDER:      s = "ThirdOrder";      break;
    case PROJECTIVE:      s = "Projective";      break;
  }
  WriteElement("CoordSystemTiePoints", "Transformation", s);
}

String CoordSystemTiePoints::sType() const
{
  return "Coordinate System TiePoints";
}

String CoordSystemTiePoints::sFormula() const
{
  String s = "Transformation: ";
  switch (transf) {
    case CONFORM:
      s &= "Conform";
      break;
    case AFFINE:
      s &= "Affine";
      break;
    case SECONDORDER:
      s &= "SecondOrder";
      break;
    case FULLSECONDORDER:
      s &= "FullSecondOrder";
      break;
    case THIRDORDER:
      s &= "ThirdOrder";
      break;
    case PROJECTIVE:
      s &= "Projective";
      break;
  }
	s &= String("\r\nNr of Ground Control Points: %i", tblCTP->iRecs());
  s &= String("\r\nNr of Active Ground Control Points: %i",iActive); 
	s &= String("\r\nShift of Ground Coords relX,relY to Centroid:");
  s &= String("\r\nX = relX - %.3f m", rAvgRefX);
  s &= String("\r\nY = relY - %.3f m", rAvgRefY);
	s &= String("\r\nEquations (from Ground to Picture):");
  s &= "\r\noutX = ";
  if (PROJECTIVE == transf) {
    s &= String("%.3g + \r\n(%.6g * X + %.6g * Y + %.6g) \r\n/ (%.6g * X + %.6g * Y + 1)",
      rAvgX,
      rCoeffY[0], rCoeffY[1], rCoeffY[2], rCoeffY[6], rCoeffY[7]);
  }
  else {
    s &= String("%.3g + %.6g * X + %.6g * Y", rAvgX + rCoeffX[0], rCoeffX[1], rCoeffX[2]);
    if (transf >= SECONDORDER)
      s &= String(" + %.6g * X * Y", rCoeffX[3]);
    if (transf >= FULLSECONDORDER)
      s &= String(" + %.6g * X^2 + %.6g * Y^2", rCoeffX[4], rCoeffX[5]);
    if (transf == THIRDORDER)
      s &= String(" + %.6g * X^3 + %.6g * X^2 * Y + %.6g * X * Y^2 + %.6g * Y3",
        rCoeffX[6], rCoeffX[7], rCoeffX[8], rCoeffX[9]);
  }
  s &= "\r\noutY = ";
  if (PROJECTIVE == transf) {
    s &= String("%.3g + \r\n(%.6g * X + %.6g * Y + %.6g) \r\n/ (%.6g * X + %.6g * Y + 1)",
      rAvgX,
      rCoeffY[3], rCoeffY[4], rCoeffY[5], rCoeffY[6], rCoeffY[7]);
  }
  else {
    s &= String("%.3g + %.6g * X + %.6g * Y", rAvgY + rCoeffY[0], rCoeffY[1], rCoeffY[2]);
    if (transf >= SECONDORDER)
      s &= String(" + %.6g * X * Y", rCoeffY[3]);
    if (transf >= FULLSECONDORDER)
      s &= String(" + %.6g * X^2 + %.6g * Y^2", rCoeffY[4], rCoeffY[5]);
    if (transf == THIRDORDER)
      s &= String(" + %.6g * X^3 + %.6g * X^2 * Y + %.6g * X * Y^2 + %.6g * Y3",
        rCoeffY[6], rCoeffY[7], rCoeffY[8], rCoeffY[9]);
  }
  if (transf <= AFFINE) {
    double rAzimuth = atan2(-rCoeffX[2],rCoeffX[1]);
    s &= "\r\nRotation of coordinate systems:";
    s &= String("\r\n %.4g degrees", rAzimuth * 180.0/M_PI);
    s &= String("\r\n\r\n Inverse Transformation:");
		s &= String("\r\nShift of Picture pX,pY to Centroid:");
    s &= String("\r\nX = pX - %.0f ", rAvgX);
    s &= String("\r\nY = pY - %.0f ", rAvgY);
    s &= "\r\nrelX = ";
    s &= String("%.3f + %.3f * X + %.3f * Y", rAvgRefX + rCoeffRefX[0], rCoeffRefX[1], rCoeffRefX[2]);
    s &= "\r\nrelY = ";
    s &= String("%.3f + %.3f * X + %.3f * Y", rAvgRefY + rCoeffRefY[0], rCoeffRefY[1], rCoeffRefY[2]);
		double rScaleX = sqrt(rCoeffRefX[1]*rCoeffRefX[1] + rCoeffRefX[2]*rCoeffRefX[2]);
		s &= String("\r\nScale in X direction : %.3f m per picture unit", rScaleX);
		double rScaleY = sqrt(rCoeffRefY[1]*rCoeffRefY[1] + rCoeffRefY[2]*rCoeffRefY[2]);
    s &= String("\r\nScale in Y direction : %.3f m per picture unit", rScaleY);
	}  
  return s;
}


Coord CoordSystemTiePoints::cConvFromOther(const Coord& c) const
{
  if (!fValid() || c.fUndef())
    return Coord();

  Coord crd = c;
  crd.x -= rAvgRefX;
  crd.y -= rAvgRefY;
  double rX = rAvgX;
  double rY = rAvgY;
  double X2, Y2, XY;
  switch (transf) {
    case THIRDORDER:
      // fall through
    case FULLSECONDORDER:
      X2 = crd.x * crd.x;
      Y2 = crd.y * crd.y;
      // fall through
    case SECONDORDER:
      XY = crd.x * crd.y;
      break;
  }
  switch (transf) {
    case THIRDORDER:
      rX += (rCoeffX[6] * crd.x + rCoeffX[7] * crd.y) * X2 +
            (rCoeffX[8] * crd.x + rCoeffX[9] * crd.y) * Y2;
      rY += (rCoeffY[6] * crd.x + rCoeffY[7] * crd.y) * X2 +
            (rCoeffY[8] * crd.x + rCoeffY[9] * crd.y) * Y2;
      // fall through
    case FULLSECONDORDER:
      rX += rCoeffX[4] * X2 + rCoeffX[5] * Y2;
      rY += rCoeffY[4] * X2 + rCoeffY[5] * Y2;
      // fall through
    case SECONDORDER:
      rX += rCoeffX[3] * XY;
      rY += rCoeffY[3] * XY;
      // fall through
    case AFFINE:
      // fall through
    case CONFORM:
      rX += rCoeffX[0];
      rY += rCoeffY[0];
      rX += rCoeffX[1] * crd.x + rCoeffX[2] * crd.y;
      rY += rCoeffY[1] * crd.x + rCoeffY[2] * crd.y;
      break;
    case PROJECTIVE:
      rX += (rCoeffY[0] * crd.x + rCoeffY[1] * crd.y + rCoeffY[2]) /
            (rCoeffY[6] * crd.x + rCoeffY[7] * crd.y + 1);
      rY += (rCoeffY[3] * crd.x + rCoeffY[4] * crd.y + rCoeffY[5]) /
            (rCoeffY[6] * crd.x + rCoeffY[7] * crd.y + 1);
      break;
  }
  return Coord(rX, rY);
}

Coord CoordSystemTiePoints::cConvToOther(const Coord& c) const
{
  if (!fValid() || c.fUndef())
    return Coord();

  Coord crd = c;
  crd.x -= rAvgX;
  crd.y -= rAvgY;
  Coord cRes;
  switch (transf) {
    case CONFORM:
    case AFFINE:
      cRes = crdInverseOfAffine(crd);
      break;
    case SECONDORDER:
    case FULLSECONDORDER:
    case THIRDORDER:
      cRes = crdInverseOfHigherOrder(crd);
      break;
    case PROJECTIVE:
      cRes = crdInverseOfProjective(crd);
      break;
  }
  cRes.x += rAvgRefX;
  cRes.y += rAvgRefY;
  return cRes;
}

int CoordSystemTiePoints::Compute()
{
  _fValid = false;
  int i, iNr;
  int iRecs = tblCTP->iRecs();
  if (iRecs == 0)
    return -1;
  int iRes = 0;
  Coord* crd = new Coord[iRecs];
  Coord* crdRef = new Coord[iRecs];
  double rSumX = 0;
  double rSumY = 0;
  double rSumRefX = 0;
  double rSumRefY = 0;
  iNr = 0;
  for (i = 1; i <= iRecs; ++i) {
    if (fActive(i)) {
      bool fAct = true;
      double r = colX->rValue(i);
      if (r == rUNDEF || abs(r) > 1e20)
        fAct = false;
      if (fAct) {
        r = colY->rValue(i);
        if (r == rUNDEF || abs(r) > 1e20)
          fAct = false;
      }  
      if (fAct) {
        r = colRefX->rValue(i);
        if (r == rUNDEF || abs(r) > 1e20)
          fAct = false;
      }  
      if (fAct) {
        r = colRefY->rValue(i);
        if (r == rUNDEF || abs(r) > 1e20)
          fAct = false;
      }  
      if (!fAct)
        SetActive(i, false);
      else {   
        rSumX += crd[iNr].x = colX->rValue(i);
        rSumY += crd[iNr].y = colY->rValue(i);
        crdRef[iNr].x = colRefX->rValue(i);
        crdRef[iNr].y = colRefY->rValue(i);
        rSumRefX += crdRef[iNr].x;
        rSumRefY += crdRef[iNr].y;
        iNr += 1;
      }  
    }  
  }  
	iActive = iNr; // added to provide additional info 
  if (iNr == 0) 
    iRes = -1;
  else if (iNr < iMinNr()) 
    iRes = -2;
  if (0 == iRes) {
    rAvgX = rSumX / iNr;
    rAvgY = rSumY / iNr;
    rAvgRefX = rSumRefX / iNr;
    rAvgRefY = rSumRefY / iNr;
    for (i = 0; i < iNr; ++i) {
      crd[i].x -= rAvgX;
      crd[i].y -= rAvgY;
      crdRef[i].x -= rAvgRefX;
      crdRef[i].y -= rAvgRefY;
    }
    if (transf == CONFORM) {
      double sxr, sxc, syr, syc, srr, scc, sxx, syy;
      sxr = sxc = syr = syc = srr = scc = sxx = syy = 0;
      i = 0;
      for (i = 0; i < iNr; ++i) {
        double c = crd[i].x;
        double r = crd[i].y;
        double x = crdRef[i].x;
        double y = crdRef[i].y;
        sxx += x * x;
        syy += y * y;
        scc += c * c;
        srr += r * r;
        sxc += x * c;
        syc += y * c;
        sxr += x * r;
        syr += y * r;
      }
      if (abs(scc + srr) < 1 || abs(sxx + syy) < EPS10)
        iRes = -3;
      else {  
        double a = (sxc + syr) / (sxx + syy);
        double b = (syc - sxr) / (sxx + syy);
        rCoeffX[1] =  a;
        rCoeffX[2] =  b;
        rCoeffY[1] = -b;
        rCoeffY[2] =  a;
        rCoeffX[0] =  0;
        rCoeffY[0] =  0;
        double rDet = a * a + b * b;
        rCoeffRefY[1] =  a / rDet;
        rCoeffRefY[2] =  b / rDet;
        rCoeffRefX[1] = -b / rDet;
        rCoeffRefX[2] =  a / rDet;
        rCoeffRefY[0] =  0;
        rCoeffRefX[0] =  0;
      }
    }
    else if (transf == PROJECTIVE) {
      iRes = iFindOblique(iNr, crd, crdRef, rCoeffRefY);
      if (iRes == 0)
        iRes = iFindOblique(iNr, crdRef, crd, rCoeffY);
    }
    else {
      int iTerms = iMinNr();
      iRes = iFindPolynom(iTerms, iNr, crdRef, crd, rCoeffX, rCoeffY);
      if (iRes == 0)
        iRes = iFindPolynom(iTerms, iNr, crd, crdRef, rCoeffRefX, rCoeffRefY);

    }
  }  
  delete [] crd;
  delete [] crdRef;
  _fValid = iRes == 0;
  return iRes;
}

int CoordSystemTiePoints::iMinNr() const
{
  switch (transf) {
    case CONFORM:
      return 2; 
    case AFFINE:
      return 3;
    case SECONDORDER:
      return 4; 
    case FULLSECONDORDER:
      return 6; 
    case THIRDORDER:
      return 10;
    case PROJECTIVE:
      return 4;   
    default:
      return 0;  
  };
}

Coord CoordSystemTiePoints::crdInverseOfAffine(const Coord& crd) const
{
  // Solving the following Coord2RowCol equations for X and Y:
  // outX = aX + by + c    (I)
  // outY = fX + gY + h   (II)
  // becomes:
  //  a X + b Y = outX - c
  //  f X + g Y = outY - h
  // Using Cramers determinants rule:
  // det = ag - bf ;  detX = (outX -c)g - (outY -h)b
  //    detY = (outY -h)a - (outX -c)f
  // yields X = detX/det and Y = detY / det  provided  det <> 0
  double a = rCoeffX[1];
  double b = rCoeffX[2];
  double c = rCoeffX[0];
  double f = rCoeffY[1];
  double g = rCoeffY[2];
  double h = rCoeffY[0];
  double rDet = a * g - b * f;
  Coord cRes;
  if (abs(rDet) < EPS10)
    return crdUNDEF;
  else  {
    cRes.x = ((crd.x - c) * g - (crd.y - h) * b ) / rDet;
    cRes.y = ((crd.y - h) * a - (crd.x - c) * f ) / rDet;
    return cRes;
  }
}

Coord CoordSystemTiePoints::crdInverseOfSecondOrderBilinear(const Coord& crd) const
{
  // Solving the following Coord2YX equations for X and Y:
  // outX = a + bX + cY + dXY    (I )
  // outY = e + fX + gY + hXY    (II)
  // by substitution of Y = (outX - a - bX) / (c + dX) into  eq II
  // we get the quadratic equation in X :
  // (df -bh)X*X + (de + cf -bg -ah +h outX -d outY)X + ce -ag + g outX - c outY = 0
  // compressed into  pX*X + qX + r = 0
  double a = rCoeffX[0];
  double b = rCoeffX[1];
  double c = rCoeffX[2];
  double d = rCoeffX[3];
  double e = rCoeffY[0];
  double f = rCoeffY[1];
  double g = rCoeffY[2];
  double h = rCoeffY[3];
  double p = d * f  - b * h;
  double q = d * e + c * f  - b * g  - a * h  + h * crd.x  - d * crd.y;
  double r = c * e - a * g  + g * crd.x  - c * crd.y;

  Coord cRes;
  if (abs(p) < EPS10)  {// equation not quadratic
    if (abs(q) < EPS10)  // no solution because crd.x undefined
      return crdUNDEF;
    else {
      cRes.x = r / q;
      if (abs(c + d * cRes.x) > EPS10)
        cRes.y = (crd.x - a - b * cRes.x) / (c + d * cRes.x);
      else
        return crdUNDEF;
      return cRes;
    }
  }  // end if equation not quadratic
  else  {      // real bilinear case (quadratic)
    double discrim = q * q - 4 * p * r;
    if (discrim < 0.0) return crdUNDEF;  // no real solution
    if (abs(discrim) < EPS10) {  // single solution for crd.x (2 coinciding solutions)
      cRes.x = ( - 0.5 * q / p);  //
      if (abs(c + d * cRes.x) > EPS10)
        cRes.y = (crd.x - a - b * cRes.x) / (c + d * cRes.x);
      else
        return crdUNDEF;
      return cRes;
    }
    else  {                      // double solution for crd.x
      Coord crd1, crd2;
      bool fY1Exists = false; 
      bool fY2Exists = false;
      crd1.x = -0.5 * (q + sqrt(discrim)) / p;
      crd2.x = -0.5 * (q - sqrt(discrim)) / p;
      if (abs(c + d * crd1.x) > EPS10) {
        crd1.y = (crd.x - a - b * crd1.x) / (c + d * crd1.x);
        fY1Exists = true;
      }
      if (abs(c + d * crd2.x) > EPS10) {
        crd2.y = (crd.x - a - b * crd2.x) / (c + d * crd2.x);
        fY2Exists = true;
      }
      if (!fY1Exists)
        if (!fY2Exists)
          return crdUNDEF; // no valid crd.y values
        else
          return crd2;      // single crd.y value
      else if (!fY2Exists)
        return crd1;      // single crd.y value
      else {                // 2 (distinct) solutions for crd
        double rLength1Sq = crd1.x * crd1.x  +  crd1.y + crd1.y;
        double rLength2Sq = crd2.x * crd2.x  +  crd2.y + crd2.y;
        if (rLength1Sq < rLength2Sq) // select nearest point
          return crd1;
        else
          return crd2;
      }    // end comparing two distinct solutions
    }
  }
}

Coord CoordSystemTiePoints::crdInverseOfProjective(const Coord& crd) const
{
  // Solving the following Coord2YX equations for X and Y:
  // outX = aX + by + c / ( dX + eY + 1)   (I)
  // outY = fX + gY + h / ( dX + eY + 1)   (II)
  // becomes:
  // ( a - d outX)X + (b - e outX)Y = outX - c
  // ( f - d outY)X + (g - e outY)Y = outY - h
  // compressed into  pX + qX = r  and sX + tY = u
  // Using Cramers determinants rule:
  // det = pt - qs ;  detX = rt - qu  and detY = pu - rs
  // yields X = detX/det and Y = detY / det  provided  det <> 0
  double a = rCoeffY[0];
  double b = rCoeffY[1];
  double c = rCoeffY[2];
  double d = rCoeffY[6];
  double e = rCoeffY[7];
  double f = rCoeffY[3];
  double g = rCoeffY[4];
  double h = rCoeffY[5];
  double p = a - d * crd.x;
  double q = b - e * crd.x;
  double r = crd.x  - c;
  double s = f - d * crd.y;
  double t = g - e * crd.y;
  double u = crd.y  - h;
  double rDet = p * t - q * s;
  Coord cRes;
  if (abs(rDet) < EPS10)
    return crdUNDEF;
  else  {
    cRes.x = ( r * t - q * u ) / rDet;
    cRes.y = ( p * u - r * s ) / rDet;
    return cRes;
  }
}

void CoordSystemTiePoints::MakeJacMatrix(const Coord &crdIn, RealMatrix & rmJ) const
{                     // Jacobian for 2nd/3rd order inversion
	 //double	c0 = coeffX[0];
  double	c1 = rCoeffX[1];
  double	c2 = rCoeffX[2];
  double	c3 = rCoeffX[3];
  double	c4 = rCoeffX[4];
  double	c5 = rCoeffX[5];
  double	c6 = rCoeffX[6];
  double	c7 = rCoeffX[7];
  double	c8 = rCoeffX[8];
  double	c9 = rCoeffX[9];
  //double	r0 = coeffY[0];
  double	r1 = rCoeffY[1];
  double	r2 = rCoeffY[2];
  double	r3 = rCoeffY[3];
  double	r4 = rCoeffY[4];
  double	r5 = rCoeffY[5];
  double	r6 = rCoeffY[6];
  double	r7 = rCoeffY[7];
  double	r8 = rCoeffY[8];
  double	r9 = rCoeffY[9];
  double X = crdIn.x;
  double Y = crdIn.y;
  // if transf == SECONDORDER or HIGHER
  rmJ(0,0) = c1 + c3 * Y;
	rmJ(0,1) = c2 + c3 * X;
	rmJ(1,0) = r1 + r3 * Y;
	rmJ(1,1) = r2 + r3 * X;
  if (transf == FULLSECONDORDER) {
    rmJ(0,0) +=  2 * c4 * X;
	  rmJ(0,1) +=  2 * c5 * Y;
	  rmJ(1,0) +=  2 * r4 * X;
	  rmJ(1,1) +=  2 * r5 * Y; 
	}
	if (transf == THIRDORDER) {
    double XX = X*X;
    double YX = X*Y;
	  double YY = Y*Y;
	  rmJ(0,0) +=  3 * c6 * XX  + 2 * c7 * YX  + c8 * YY;
	  rmJ(0,1) +=  c7 * XX  + 2 * c8 * YX  + 3 * c9 * YY;
	  rmJ(1,0) +=  3 * r6 * XX  + 2 * r7 * YX  + r8 * YY;
	  rmJ(1,1) +=  r7 * XX  + 2 * r8 * YX  + 3 * r9 * YY;
  }
}

Coord CoordSystemTiePoints::crdInverseOfHigherOrder(const Coord& crd) const
{
  // Solving the following Coord2YX 3rd oreder equations for X and Y:
  // outX = c0 + c1X + c2Y + c3XY + c4XX + c5YY + c6XXX + c7XXY + c8XYY + c9YYY  (I )
  // outY = r0 + r1X + r2Y + r3XY + r4XX + r5YY + r6XXX + r7XXY + r8XYY + r9YYY  (II)
  // 2nd order if c6 == c7 == c8 == c9 == r6 == r7 == r8 == r9 == 0
  Coord crdNext = Coord(0,0);     // initial crd guess
  double rColNext = rCoeffX[0]; // initial X
  double rRowNext = rCoeffY[0]; // initial Y
  double rRow = crd.y;
  double rCol = crd.x;
  double rDeltaCol = rCol - rColNext;
  double rDeltaRow = rRow - rRowNext;
  MakeJacMatrix(crdNext, rmJC2R);// linear appr matrix of Coord2RowCol in (0,0)
  double detJ = rmJC2R.rDet();
  if (abs(detJ) < EPS10)
    return crdUNDEF;
  else  {
    //rmJR2C = rmJC2R;no extra matrix needed 25/5/04(11/3/02 debugg in Grctppla.cpp)
    rmJC2R.Invert();
  }
  double rDeltaX = rmJC2R(0,0) * rDeltaCol + rmJC2R(0,1) * rDeltaRow;
  double rDeltaY = rmJC2R(1,0) * rDeltaCol + rmJC2R(1,1) * rDeltaRow;
  crdNext.x += rDeltaX;
  crdNext.y += rDeltaY;
  // crdNext is now 1st appr of RowCol2Coord(rRow, rCol,crd)
  double rGapStart = 2;
  Coord crdTrue; 
  Coord crdGap = Coord(rGapStart,rGapStart);
  Coord crd0 = Coord(0,0);
  int iCount = 0;
  while (!crdGap.fNear(crd0, 1) && iCount < 10) {
    MakeJacMatrix(crdNext, rmJC2R);
    detJ = rmJC2R.rDet();
    if (abs(detJ) < EPS10)
      return crdUNDEF;
    else  {
      //rmJR2C = rmJC2R;no extra matrix needed 25/5/04(11/3/02 debugg in Grctppla.cpp)
      rmJC2R.Invert();
    }
    rDeltaCol = rCol - rColNext;
    rDeltaRow = rRow - rRowNext;   // remaining error
    //improvement of crd found (linear approx) using inverse Jacobian matrix
    rDeltaX = rmJC2R(0,0) * rDeltaCol + rmJC2R(0,1) * rDeltaRow;
    rDeltaY = rmJC2R(1,0) * rDeltaCol + rmJC2R(1,1) * rDeltaRow;
    crdGap = Coord(rDeltaX, rDeltaY);
    crdNext.x += rDeltaX; //correction of crd with improvement
    crdNext.y += rDeltaY; //
    crdTrue.x = crdNext.x + rAvgX;//shift to true position for Coord2RowCol
    crdTrue.y = crdNext.y + rAvgY;
    //Coord2RowCol(crdTrue, rRowNext, rColNext);
    Coord cRes = cConvFromOther(crdTrue);
    rRowNext = cRes.y;
    rColNext = cRes.x;
    rRowNext -= rAvgY;  // shift back to center of gravity
    rColNext -= rAvgX;
    iCount++;
  }
  if (iCount < 10)
    return crdNext;
  else
    return crdUNDEF;
}

