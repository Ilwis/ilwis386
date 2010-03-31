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
/* ProjectionCalc
   Copyright Ilwis System Development ITC
   march 1995, by Wim Koolhoven
	Last change:  JEL   6 Jul 97    4:59 pm
*/

#define PRJCALC_C
#include "Engine\SpatialReference\PRJCALC.H"
#include "Engine\Scripting\Calc.h"
#include "Engine\Scripting\Calcvar.h"

ProjectionCalc::ProjectionCalc(const FileName& fn, const Ellipsoid& ell)
: ProjectionPtr(fn, ell)
{
  f2Coord = f2LatLon = true;
  instPhiLam2Coord = instCoord2PhiLam = 0;
//  cvrlX = cvrlY = cvrlPhi = cvrlLam = cvrcX = cvrcY = cvrcPhi = cvrcLam = 0;
  FileName fnF;
  ReadElement("ProjectionCalc", "LatLon2Coord", fnF);
  instPhiLam2Coord = Calculator::instCoordFunc(fnF.sFullName());
  ReadElement("ProjectionCalc", "Coord2LatLon", fnF);
  instCoord2PhiLam = Calculator::instCoordFunc(fnF.sFullName());
}

/*
void ProjectionCalc::Store()
{
  ProjectionPtr::Store();
  WriteElement("Projection", "Type", "ProjectionCalc");
}
*/

ProjectionCalc::~ProjectionCalc()
{
  if (instPhiLam2Coord)
    delete instPhiLam2Coord;
  if (instCoord2PhiLam)
    delete instCoord2PhiLam;
}

XY ProjectionCalc::xyConv(const PhiLam& pl) const
{
  if (0 == instPhiLam2Coord)
    return XY();
  Coord crdOut;
  instPhiLam2Coord->CalcCoords(Coord(pl.Phi, pl.Lam), crdOut);
  XY xy;
  xy.x = crdOut.x;
  xy.y = crdOut.y;
  return xy;

/*  XY xy;
  if (0 != instPhiLam2Coord && cvrcPhi.fValid() && cvrcLam.fValid() && cvrcX.fValid() && cvrcY.fValid()) {
    pcvrcPhi->PutVal(pl.Phi);
    pcvrcLam->PutVal(pl.Lam);
    instPhiLam2Coord->Execute();
    xy.x = cvrcX->rValue();
    xy.y = cvrcY->rValue();
  }  */
//  return xy;
}

PhiLam ProjectionCalc::plConv(const XY& xy) const
{
//  PhiLam pl;
  if (0 == instPhiLam2Coord)
    return PhiLam();
  Coord crdOut;
  instCoord2PhiLam->CalcCoords(Coord(xy.x, xy.y), crdOut);
  PhiLam pl;
  pl.Phi = crdOut.x;
  pl.Lam = crdOut.y;
  return pl;
//if (0 != instCoord2PhiLam && 0 != cvrlPhi && 0 != cvrlLam && 0 != cvrlX && 0 != cvrlY) {
/*if (0 != instPhiLam2Coord && cvrlPhi.fValid() && cvrlLam.fValid() && cvrlX.fValid() && cvrlY.fValid()) {
    pcvrlX->PutVal(xy.x);
    pcvrlY->PutVal(xy.y);
    instCoord2PhiLam->Execute();
    pl.Phi = cvrlPhi->rValue();
    pl.Lam = cvrlLam->rValue();
  }*/
//  return pl;
}

void ProjectionCalc::Ell(const Ellipsoid& ell)
{
  for (int i = 0; i < acvPhiLam2Coord.iSize(); ++i) {
    CalcVarReal* cvr = dynamic_cast<CalcVarReal*>(acvPhiLam2Coord[i].ptr);
    if (0 == cvr)
      continue;
    String s = cvr->sName();
    if (s == "a") 
      cvr->PutVal(ell.a);
    else if (s == "b") 
      cvr->PutVal(ell.b);
    else if (s == "e2") 
      cvr->PutVal(ell.e2);
    else if (s == "f") 
      cvr->PutVal(ell.f);
  }
  for (int i = 0; i < acvCoord2PhiLam.iSize(); ++i) {
    CalcVarReal* cvr = dynamic_cast<CalcVarReal*>(acvCoord2PhiLam[i].ptr);
    if (0 == cvr)
      continue;
    String s = cvr->sName();
    if (s == "a") 
      cvr->PutVal(ell.a);
    else if (s == "b") 
      cvr->PutVal(ell.b);
    else if (s == "e2") 
      cvr->PutVal(ell.e2);
    else if (s == "f") 
      cvr->PutVal(ell.f);
  }
}

/*
int ProjectionParamsCalc::iParams() const
{
  return asParams.iSize();
}

String ProjectionParamsCalc::sParamName(int i) const
{
  return asParams[i];
}

ProjectionParamType ProjectionParamsCalc::ppParam(int i) const
{
  return appt[i];
}

Lamg   ProjectionParamsCalc::iParam(int id) const   // ppINT
{
  if (appt[id] != ppINT)
    return iUNDEF;
  String sVar = asVars[id];
  for (int i = 0; i < acvPhiLam2Coord.iSize(); ++i) {
    CalcVarInt* cvi = dynamic_cast<CalcVarInt*>(acvPhiLam2Coord[i]);
    if (0 == cvi)
      continue;
    if (cvi->sName() == sVar) 
      return cvi->iValue();
  }
  for (i = 0; i < acvCoord2PhiLam.iSize(); ++i) {
    CalcVarInt* cvi = dynamic_cast<CalcVarInt*>(acvCoord2PhiLam[i]);
    if (0 == cvi)
      continue;
    if (cvi->sName() == sVar) 
      return cvi->iValue();
  }
  return iUNDEF;
}

double ProjectionParamsCalc::rParam(int id) const   // ppPhi, ppLam, ppREAL, ppANGLE
{
  if (appt[id] != ppPhi && appt[id] != ppLam && 
      appt[id] != ppREAL && appt[id] != ppANGLE)
    return rUNDEF;
  String sVar = asVars[id];
  for (int i = 0; i < acvPhiLam2Coord.iSize(); ++i) {
    CalcVarReal* cvr = dynamic_cast<CalcVarReal*>(acvPhiLam2Coord[i]);
    if (0 == cvr)
      continue;
    if (cvr->sName() == sVar) 
      return cvr->rValue();
  }
  for (i = 0; i < acvCoord2PhiLam.iSize(); ++i) {
    CalcVarReal* cvr = dynamic_cast<CalcVarReal*>(acvCoord2PhiLam[i]);
    if (0 == cvr)
      continue;
    if (cvr->sName() == sVar) 
      return cvr->rValue();
  }
  return rUNDEF;
}

PhiLam ProjectionParamsCalc::llParam(int id) const  // ppPhiLam
{
  if (appt[id] != ppPhiLam)
    return PhiLam();
  String sVar = asVars[id];
  String sPhi = sVar;
  sPhi &= "Phi";
  String sLam = sVar;
  sLam &= "Lam";
  PhiLam ll;
  for (int i = 0; i < acvPhiLam2Coord.iSize(); ++i) {
    CalcVarReal* cvr = dynamic_cast<CalcVarReal*>(acvPhiLam2Coord[i]);
    if (0 == cvr)
      continue;
    if (cvr->sName() == sPhi) 
      ll.Phi = cvr->rValue();
    else if (cvr->sName() == sLam) 
      ll.Lam = cvr->rValue();
  }
  if (!ll.fUndef())
    return ll;
  for (i = 0; i < acvCoord2PhiLam.iSize(); ++i) {
    CalcVarReal* cvr = dynamic_cast<CalcVarReal*>(acvCoord2PhiLam[i]);
    if (0 == cvr)
      continue;
    if (cvr->sName() == sPhi) 
      ll.Phi = cvr->rValue();
    else if (cvr->sName() == sLam) 
      ll.Lam = cvr->rValue();
  }
  return ll;
}

Coord  ProjectionParamsCalc::cParam(int id) const   // ppCOORD
{
  if (appt[id] != ppCOORD)
    return crdUNDEF;
  String sVar = asVars[id];
  String sX = sVar;
  sX &= "X";
  String sY = sVar;
  sY &= "Y";
  Coord crd;
  for (int i = 0; i < acvPhiLam2Coord.iSize(); ++i) {
    CalcVarReal* cvr = dynamic_cast<CalcVarReal*>(acvPhiLam2Coord[i]);
    if (0 == cvr)
      continue;
    if (cvr->sName() == sX) 
      crd.x = cvr->rValue();
    else if (cvr->sName() == sY) 
      crd.y = cvr->rValue();
  }
  if (!crd.fUndef())
    return crd;
  for (i = 0; i < acvCoord2PhiLam.iSize(); ++i) {
    CalcVarReal* cvr = dynamic_cast<CalcVarReal*>(acvCoord2PhiLam[i]);
    if (0 == cvr)
      continue;
    if (cvr->sName() == sX) 
      crd.x = cvr->rValue();
    else if (cvr->sName() == sY) 
      crd.y = cvr->rValue();
  }
  return crd;
}

void ProjectionParamsCalc::Param(int id, Lamg iVal)
{
  if (appt[id] != ppINT)
    return;
  String sVar = asVars[id];
  for (int i = 0; i < acvPhiLam2Coord.iSize(); ++i) {
    CalcVarInt* cvi = dynamic_cast<CalcVarInt*>(acvPhiLam2Coord[i]);
    if (0 == cvi)
      continue;
    if (cvi->sName() == sVar) 
      cvi->PutVal(iVal);
  }
  for (i = 0; i < acvCoord2PhiLam.iSize(); ++i) {
    CalcVarInt* cvi = dynamic_cast<CalcVarInt*>(acvCoord2PhiLam[i]);
    if (0 == cvi)
      continue;
    if (cvi->sName() == sVar) 
      cvi->PutVal(iVal);
  }
}

void ProjectionParamsCalc::Param(int id, double rVal)
{
  if (appt[id] != ppPhi && appt[id] != ppLam && 
      appt[id] != ppREAL && appt[id] != ppANGLE)
    return;
  String sVar = asVars[id];
  for (int i = 0; i < acvPhiLam2Coord.iSize(); ++i) {
    CalcVarReal* cvr = dynamic_cast<CalcVarReal*>(acvPhiLam2Coord[i]);
    if (0 == cvr)
      continue;
    if (cvr->sName() == sVar) 
      cvr->PutVal(rVal);
  }
  for (i = 0; i < acvCoord2PhiLam.iSize(); ++i) {
    CalcVarReal* cvr = dynamic_cast<CalcVarReal*>(acvCoord2PhiLam[i]);
    if (0 == cvr)
      continue;
    if (cvr->sName() == sVar) 
      cvr->PutVal(rVal);
  }
}

void ProjectionParamsCalc::Param(int id, const PhiLam& ll)
{
  if (appt[id] != ppPhiLam)
    return;
  String sVar = asVars[id];
  String sPhi = sVar;
  sPhi &= "Phi";
  String sLam = sVar;
  sLam &= "Lam";
  for (int i = 0; i < acvPhiLam2Coord.iSize(); ++i) {
    CalcVarReal* cvr = dynamic_cast<CalcVarReal*>(acvPhiLam2Coord[i]);
    if (0 == cvr)
      continue;
    if (cvr->sName() == sPhi) 
      cvr->PutVal(ll.Phi);
    else if (cvr->sName() == sLam) 
      cvr->PutVal(ll.Lam);
  }
  for (i = 0; i < acvCoord2PhiLam.iSize(); ++i) {
    CalcVarReal* cvr = dynamic_cast<CalcVarReal*>(acvCoord2PhiLam[i]);
    if (0 == cvr)
      continue;
    if (cvr->sName() == sPhi) 
      cvr->PutVal(ll.Phi);
    else if (cvr->sName() == sLam) 
      cvr->PutVal(ll.Lam);
  }
}

void ProjectionParamsCalc::Param(int id, const Coord& crd)
{
  if (appt[id] != ppCOORD)
    return;
  String sVar = asVars[id];
  String sX = sVar;
  sX &= "X";
  String sY = sVar;
  sY &= "Y";
  for (int i = 0; i < acvPhiLam2Coord.iSize(); ++i) {
    CalcVarReal* cvr = dynamic_cast<CalcVarReal*>(acvPhiLam2Coord[i]);
    if (0 == cvr)
      continue;
    if (cvr->sName() == sX) 
      cvr->PutVal(crd.x);
    else if (cvr->sName() == sY) 
      cvr->PutVal(crd.y);
  }
  for (i = 0; i < acvCoord2PhiLam.iSize(); ++i) {
    CalcVarReal* cvr = dynamic_cast<CalcVarReal*>(acvCoord2PhiLam[i]);
    if (0 == cvr)
      continue;
    if (cvr->sName() == sX) 
      cvr->PutVal(crd.x);
    else if (cvr->sName() == sY) 
      cvr->PutVal(crd.y);
  }
}
*/




