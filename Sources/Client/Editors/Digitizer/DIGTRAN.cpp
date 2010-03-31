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
/* DigTransform
// largely based on afftran.pas of ILWIS 1
// by Wim Koolhoven
// (c) ILWIS Department ITC
	Last change:  WK   18 Feb 97    5:23 pm
*/
#define DIGTRAN_C
#include "Headers\toolspch.h"
#include "Client\Editors\Digitizer\DIGTRAN.H"

void DigTransform::CenterCoords()
{
  if (iPoints == 0)
    return;
  int i;
  crdDigC.x = crdDigC.y = crdWorldC.x = crdWorldC.y = 0;
  for (i = 0; i < iPoints; ++i) {
    crdDigC.x	+= dp[i].crdDig.x;
    crdDigC.y	+= dp[i].crdDig.y;
    crdWorldC.x += dp[i].crdWorld.x;
    crdWorldC.y += dp[i].crdWorld.y;
  }
  crdDigC.x /= iPoints;
  crdDigC.y /= iPoints;
  crdWorldC.x /= iPoints;
  crdWorldC.y /= iPoints;
  for (i = 0; i < iPoints; ++i) {
    dp[i].crdDig.x   -= crdDigC.x;
    dp[i].crdDig.y   -= crdDigC.y;
    dp[i].crdWorld.x -= crdWorldC.x;
    dp[i].crdWorld.y -= crdWorldC.y;
  }
}

void DigTransform::NormalCoords()
{
  for (int i = 0; i < iPoints; ++i) {
    dp[i].crdDig.x   += crdDigC.x;
    dp[i].crdDig.y   += crdDigC.y;
    dp[i].crdWorld.x += crdWorldC.x;
    dp[i].crdWorld.y += crdWorldC.y;
  }
}

int DigTransform::SolveCoeff(bool fAffine)
{
  double u, v, x, y;
  double suu, suv, svv, sxu, sxv, syu, syv, sxx, sxy, syy;
  suu = suv = svv = sxu = sxv = syu = syv = sxx = sxy = syy = 0;
  for (int i = 0; i < iPoints; ++i) {
    u = dp[i].crdDig.x;
    v = dp[i].crdDig.y;
    x = dp[i].crdWorld.x;
    y = dp[i].crdWorld.y;
    suu += u * u;
    suv += u * v;
    svv += v * v;
    sxu += x * u;
    sxv += x * v;
    syu += y * u;
    syv += y * v;
    sxx += x * x;
    sxy += x * y;
    syy += y * y;
  }
  if (fAffine) {
    if (abs(sxx * syy - sxy * sxy) < rEpsilon)
      return -2;
    double rDet = suu * svv - suv * suv;
    if (abs(rDet) < 1)
      return -2;
    x10 = (sxu * svv - sxv * suv) / rDet;
    x01 = (sxv * suu - sxu * suv) / rDet;
    y10 = (syu * svv - syv * suv) / rDet;
    y01 = (syv * suu - syu * suv) / rDet;
  }
  else {
    double rDet = suu + svv;
    if (abs(rDet) < 1)
      return -2;
    x10 = (sxu + syv) / rDet;
    x01 = (sxv - syu) / rDet;
    y10 = -x01;
    y01 = x10;
  }
  x0  = crdWorldC.x - (x10 * crdDigC.x + x01 * crdDigC.y);
  y0  = crdWorldC.y - (y10 * crdDigC.x + y01 * crdDigC.y);
  return 0;
}

int DigTransform::DeltaSigma(bool fAffine)
{
  double rDet, a, b, c, d, e, f;
  double u, v, x, y, du, dv, du2v2;
  rDet = x10 * y01 - x01 * y10;
  if (abs(rDet) < 1e-20)
    return -2;
  a = y01 / rDet;
  b = -x01 / rDet;
  c = crdDigC.x - (a * crdWorldC.x + b * crdWorldC.y);
  d = -y10 / rDet;
  e = x10 / rDet;
  f = crdDigC.y - (d * crdWorldC.x + e * crdWorldC.y);
  du2v2 = 0;
  for (int i = 0; i < iPoints; ++i) {
    u = dp[i].crdDig.x;
    v = dp[i].crdDig.y;
    x = dp[i].crdWorld.x;
    y = dp[i].crdWorld.y;
    du = u - (a * x + b * y + c);
    dv = v - (d * x + e * y + f);
    du2v2 += du * du + dv * dv;
    dp[i].crdDelta.x = du;
    dp[i].crdDelta.y = dv;
  }
  int iUsed = fAffine ? 3 : 2;
  if (iPoints > iUsed)
    rSigma = sqrt(du2v2 / 2 / (iPoints - iUsed));
  else
    rSigma = 0;
  return 0;
}

int DigTransform::Transform(bool fAffine)
{
  fOk = false;
  if (iPoints < (fAffine ? 3 : 2))
    return -1;
  CenterCoords();
  int iRet = SolveCoeff(fAffine);
  if (iRet) return iRet;
  NormalCoords();
  iRet = DeltaSigma(fAffine);
  if (iRet) return iRet;
  fOk = true;
  return 0;
}

void DigTransform::SetEpsilon(bool fLatlon)
{ 
	rEpsilon = fLatlon ? 10e-8 : 1;
}
