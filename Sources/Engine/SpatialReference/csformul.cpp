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
/* CoordSystemFormula
   Copyright Ilwis System Development ITC
   april 1998, by Wim Koolhoven
	Last change:  WK   17 Apr 98   12:09 pm
*/

#include "Engine\SpatialReference\csformul.h"
#include "Engine\Scripting\Calc.h"

CoordSystemFormula::CoordSystemFormula(const FileName& fn)
: CoordSystemDirect(fn),
  insX(0), insY(0), insInvX(0), insInvY(0)
{
  init();
}

void CoordSystemFormula::init() // load (again) from disk
{
  eTransf = eConformal;
  a11 = 1;
  a12 = 0;
  a21 = 0;
  a22 = 1;
  rScale = 1;
  rRot = 0;
  rScaleX = rScaleY = 1;
  rSkewX = rSkewY = 0;
  c0 = cOther0 = Coord(0,0);
  sExprX = "X";
  sExprY = "Y";
// read all!
  String sType;
  ReadElement("CoordSystemFormula", "Type", sType);
  if (fCIStrEqual("Conformal" , sType)) {
    eTransf = eConformal;
    ReadElement("CoordSystemFormula", "Scale", rScale);
    ReadElement("CoordSystemFormula", "Rotation", rRot);
  }
  else if (fCIStrEqual("Diffscaling" , sType)) {
    eTransf = eDiffScaling;
    ReadElement("CoordSystemFormula", "Scale X", rScaleX);
    ReadElement("CoordSystemFormula", "Scale Y", rScaleY);
  }
  else if (fCIStrEqual("Skew X" , sType)) {
    eTransf = eSkewX;
    ReadElement("CoordSystemFormula", "Skew X", rSkewX);
  }
  else if (fCIStrEqual("Skew Y" , sType)) {
    eTransf = eSkewY;
    ReadElement("CoordSystemFormula", "Skew Y", rSkewY);
  }
  else if (fCIStrEqual("Affine" , sType)) {
    eTransf = eAffine;
    ReadElement("CoordSystemFormula", "a11", a11);
    ReadElement("CoordSystemFormula", "a12", a12);
    ReadElement("CoordSystemFormula", "a21", a21);
    ReadElement("CoordSystemFormula", "a22", a22);
  }
  else if (fCIStrEqual("UserDefFormula" , sType)) {
    eTransf = eUserDef;
    ReadElement("CoordSystemFormula", "X", sExprX);
    ReadElement("CoordSystemFormula", "Y", sExprY);
    ReadElement("CoordSystemFormula", "Inverse X", sExprInvX);
    ReadElement("CoordSystemFormula", "Inverse Y", sExprInvY);
  }
  ReadElement("CoordSystemFormula", "Other Origin", cOther0);
  ReadElement("CoordSystemFormula", "Origin", c0);
  Calc();
}

CoordSystemFormula::CoordSystemFormula(const FileName& fn, const CoordSystem& csRef)
: CoordSystemDirect(fn, csRef),
  insX(0), insY(0), insInvX(0), insInvY(0)
{
  eTransf = eConformal;
  a11 = 1;
  a12 = 0;
  a21 = 0;
  a22 = 1;
  rScale = 1;
  rRot = 0;
  rScaleX = rScaleY = 1;
  rSkewX = rSkewY = 0;
  c0 = cOther0 = Coord(0,0);
}

CoordSystemFormula::~CoordSystemFormula()
{
  if (insX)
    delete insX;
  if (insY)
    delete insY;
  if (insInvX)
    delete insInvX;
  if (insInvY)
    delete insInvY;
}

void CoordSystemFormula::Store()
{
  CoordSystemDirect::Store();
  WriteElement("CoordSystem", "Type", "Formula");
  WriteElement("CoordSystemFormula", "Other Origin", cOther0);
  WriteElement("CoordSystemFormula", "Origin", c0);
  switch (eTransf) {
    case eConformal:
      WriteElement("CoordSystemFormula", "Type", "Conformal");
      WriteElement("CoordSystemFormula", "Scale", rScale);
      WriteElement("CoordSystemFormula", "Rotation", rRot);
      break;
    case eDiffScaling:
      WriteElement("CoordSystemFormula", "Type", "DiffScaling");
      WriteElement("CoordSystemFormula", "Scale X", rScaleX);
      WriteElement("CoordSystemFormula", "Scale Y", rScaleY);
      break;
    case eSkewX:
      WriteElement("CoordSystemFormula", "Type", "Skew X");
      WriteElement("CoordSystemFormula", "Skew X", rSkewX);
      break;
    case eSkewY:
      WriteElement("CoordSystemFormula", "Type", "Skew Y");
      WriteElement("CoordSystemFormula", "Skew Y", rSkewY);
      break;
    case eAffine:
      WriteElement("CoordSystemFormula", "Type", "Affine");
      WriteElement("CoordSystemFormula", "a11", a11);
      WriteElement("CoordSystemFormula", "a12", a12);
      WriteElement("CoordSystemFormula", "a21", a21);
      WriteElement("CoordSystemFormula", "a22", a22);
      break;
    case eUserDef:
      WriteElement("CoordSystemFormula", "Type", "UserDefFormula");
      WriteElement("CoordSystemFormula", "X", sExprX);
      WriteElement("CoordSystemFormula", "Y", sExprY);
      WriteElement("CoordSystemFormula", "Inverse X", sExprInvX);
      WriteElement("CoordSystemFormula", "Inverse Y", sExprInvY);
      break;
  }
}

void CoordSystemFormula::Calc()
{
  if (eAffine == eTransf)
    return;
  a11 = a22 = 1;
  a12 = a21 = 0;
  switch (eTransf) {
    case eConformal: {
      double rPhi = rRot * M_PI / 180;
      double c = cos(rPhi);
      double s = sin(rPhi);
      double k = rScale;
      a11 = k * c;
      a12 = k * s;
      a21 = -k * s;
      a22 = k * c;
    } break;
    case eDiffScaling:
      a11 = rScaleX;
      a22 = rScaleY;
      break;
    case eSkewX: {
      double rAlpha = rSkewX * M_PI / 180;
      a12 = tan(rAlpha);
    } break;
    case eSkewY: {
      double rBeta = rSkewY * M_PI / 180;
      a21 = tan(rBeta);
    } break;
    case eUserDef:
      if (insX)
        delete insX;
      try {
        insX = Calculator::instExprXY(sExprX);
      }
      catch (const ErrorObject& err) {
        err.Show();
        insX = 0;
      }
      if (insY)
        delete insY;
      try {
        insY = Calculator::instExprXY(sExprY);
      }
      catch (const ErrorObject& err) {
        err.Show();
        insY = 0;
      }
      if (insInvX)
        delete insInvX;
      insInvX = 0;
      try {
        if (sExprInvX[0])
          insInvX = Calculator::instExprXY(sExprInvX);
      }
      catch (const ErrorObject& err) {
        err.Show();
        insInvX = 0;
      }
      if (insInvY)
        delete insInvY;
      insInvY = 0;
      try {
        if (sExprInvY[0])
          insInvY = Calculator::instExprXY(sExprInvY);
      }
      catch (const ErrorObject& err) {
        err.Show();
        insInvY = 0;
      }
      break;
  }
}

String CoordSystemFormula::sType() const
{
  return "Coordinate System Formula";
}

double CoordSystemFormula::rDeterm() const
{
  return a11 * a22 - a12 * a21;
}

bool CoordSystemFormula::fInverse() const
{
  if (eTransf == eUserDef) {
    return false;
  }
  else {
    double rDet = rDeterm();
    if (abs(rDet) < 1e-12)
      return false;
    return true;
  }
}

Coord CoordSystemFormula::cConvFromOther(const Coord& crd) const
{
  if (crd.fUndef())
    return crd;
  Coord c = crd;
  c -= cOther0;
  double rX = c.x;
  double rY = c.y;
  if (eTransf != eUserDef) {
    c.x = a11 * rX + a12 * rY;
    c.y = a21 * rX + a22 * rY;
  }
  else {
    if (0 == insX || 0 == insY)
      return Coord();
    c.x = insX->rCalcVal(rX, rY);
    c.y = insY->rCalcVal(rX, rY);
  }
  c += c0;
  return c;
}

Coord CoordSystemFormula::cConvToOther(const Coord& crd) const
{
  if (crd.fUndef())
    return crd;
  Coord c = crd;
  c -= c0;
  double rX = c.x;
  double rY = c.y;
  if (eTransf != eUserDef) {
    double rDet = rDeterm();
    if (abs(rDet) < 1e-12)
      return Coord();
    c.x = ( a22 * rX - a12 * rY) / rDet;
    c.y = (-a21 * rX + a11 * rY) / rDet;
  }
  else {
    if (0 == insInvX || 0 == insInvY)
      return Coord();
    c.x = insInvX->rCalcVal(rX, rY);
    c.y = insInvY->rCalcVal(rX, rY);
    return Coord();
  }
  c += cOther0;
  return c;
}




