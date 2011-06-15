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
// $Log: /ILWIS 3.0/Table/Autcsemv.cpp $
 * 
 * 7     20-12-99 11:57 Wind
 * forgotten stuff from port of 2.23 code
 * 
 * 6     9-09-99 2:40p Martin
 * ported 2.22 stuff
 * 
 * 5     9-09-99 11:12a Martin
 * added 2.22 stuff
 * 
 * 4     9/08/99 12:54p Wind
 * changed constructor calls FileName(fn, sExt, true) to FileName(fn,
 * sExt)
 * or changed FileName(fn, sExt, false) to FileName(fn.sFullNameQuoted(),
 * seExt, false)
 * to ensure that proper constructor is called
 * 
 * 3     9/08/99 11:55a Wind
 * comments
 * 
 * 2     9/08/99 10:27a Wind
 * adpated to use of quoted file names in sExpression()
*/
// Revision 1.3  1998/09/16 17:25:32  Wim
// 22beta2
//
// Revision 1.2  1997/10/04 12:39:13  Wim
// ValueRange of HorCorr and VertCorr is now -1:1 instead of 0:1
//
/* TableAutoCorrSemiVar
   Copyright Ilwis System Development ITC
   april 1995, by Jelle Wind
	Last change:  WK    1 Jul 98   10:01 am
*/

#include "Headers\toolspch.h"
#include "Applications\Table\AUTCSEMV.H"
#include "Engine\Table\COLSTORE.H"
#include "Engine\Table\tblstore.h"
#include "Engine\Applications\MAPVIRT.H"
#include "Engine\Base\DataObjects\valrange.h"
#include "Headers\Htp\Ilwisapp.htp"
#include "Headers\Err\Ilwisapp.err"
#include "Headers\Hs\tbl.hs"

const char* TableAutoCorrSemiVar::sSyntax()
{
  return "TableAutoCorrSemiVar(map[,maxpixelshift])";
}

int TableAutoCorrSemiVar::iMaxShiftDefault(const Map& map)
{
  RowCol rc = map->rcSize();
  return min(20L, 1 + min(rc.Row, rc.Col)/5L); // defaults always betw 1 and 20 (incl)
}

int TableAutoCorrSemiVar::iMaxMapSize(const Map& map)
{
  RowCol rc = map->rcSize();
  return max(rc.Row, rc.Col);

}

IlwisObjectPtr * createTableAutoCorrSemiVar(const FileName& fn, IlwisObjectPtr& ptr, const String& sExpr, vector<void *> parms ) {
	if ( sExpr != "")
		return (IlwisObjectPtr *)TableAutoCorrSemiVar::create(fn, (TablePtr &)ptr, sExpr);
	else
		return (IlwisObjectPtr *)new TableAutoCorrSemiVar(fn, (TablePtr &)ptr);
}

TableAutoCorrSemiVar* TableAutoCorrSemiVar::create(const FileName& fn, TablePtr& p, const String& sExpr)
{
  Array<String> as;
  short iParms = IlwisObjectPtr::iParseParm(sExpr, as);
  if (iParms > 2|| iParms < 1)
    throw ErrorExpression(sExpr, sSyntax());
  Map mp = Map(as[0], fn.sPath());
  long iMaxShift = iMaxShiftDefault(mp);
  if (iParms > 1) {
    iMaxShift = as[1].iVal();
    //if (iMaxShift < 1)
  }  
  return new TableAutoCorrSemiVar(fn, p, mp, iMaxShift);
}

TableAutoCorrSemiVar::TableAutoCorrSemiVar(const FileName& fn, TablePtr& p)
: TableVirtual(fn, p)
{
  FileName fnMap;
  ReadElement("TableAutoCorrSemiVar", "Map", map);
  objdep.Add(map.ptr());
  ReadElement("TableAutoCorrSemiVar", "MaxPixelShift", iMaxPixelShift);
  fNeedFreeze = true;
  Init();
  sFreezeTitle = "TableAutoCorrSemiVar";
  htpFreeze = "ilwisapp\\auto_correlation_semivariance_algorithm.htm";
}

TableAutoCorrSemiVar::TableAutoCorrSemiVar(const FileName& fn, TablePtr& p, Map& mp, long iMaxPixelShft)
: TableVirtual(fn, p, true), //FileName(fn, ".TB#", true), Domain("none")),
  map(mp), iMaxPixelShift(iMaxPixelShft)
{
//  _iOffset = 0;
//  _iRecs = iMaxPixelShift;
  if (!map->dvrs().fValues())
    throw ErrorValueDomain(map->dm()->sName(true, fnObj.sPath()), map->sTypeName(), errTableAutoCorrSemiVar);
  if (iMaxPixelShift <= 0)
    throw ErrorObject(WhatError(STBLErrMaxPixShiftNotNegative, errTableAutoCorrSemiVar+1), sTypeName());
  if (iMaxPixelShift > 8000)
    throw ErrorObject(WhatError(STBLErrMaxPixShiftTooLarge, errTableAutoCorrSemiVar+2), sTypeName());
  if (iMaxPixelShift > iMaxMapSize(map))
    throw ErrorObject(WhatError(STBLErrMaxPixShiftLargerThanMap, errTableAutoCorrSemiVar+3), sTypeName());

  objdep.Add(map.ptr());
  if (!fnObj.fValid())
    objtime = objdep.tmNewest();
  fNeedFreeze = true;
  sFreezeTitle = "TableAutoCorrSemiVar";
  htpFreeze = "ilwisapp\\auto_correlation_semivariance_algorithm.htm";
  ptr.SetDomain(Domain("none"));
}

TableAutoCorrSemiVar::~TableAutoCorrSemiVar()
{
}

void TableAutoCorrSemiVar::Store()
{
  TableVirtual::Store();
  WriteElement("TableVirtual", "Type", "TableAutoCorrSemiVar");
  WriteElement("TableAutoCorrSemiVar", "Map", map);
  WriteElement("TableAutoCorrSemiVar", "MaxPixelShift", iMaxPixelShift);
}

bool TableAutoCorrSemiVar::fFreezing()
{
  Init();
  ptr.iRecNew(iMaxPixelShift);

  Sx.Size(iRecs()+ 1);
  Sy.Size(iRecs()+ 1);
  Sxy.Size(iRecs()+ 1);
  Sxx.Size(iRecs()+ 1);
  Syy.Size(iRecs()+ 1);
  iM.Size(iRecs()+ 1);

  if (!fCalculate(map))
    return false;
  CalcAutoCorr(_colHorCorr);
  CalcSemiVar(_colHorVar);

  if (map->gr()->rPixSize() != rUNDEF) 
    CalcDistance(_colDistance);

  Map mpRotate(FileName::fnUnique(FileName(fnObj, ".mpr")), String("MapMirrorRotate(%S, Rotate90)", map->sNameQuoted()));
  mpRotate->gr()->fErase = true;
  mpRotate->fErase = true;
//  mpRotate->pmv()->Freeze();
  mpRotate->Calc();
//  if (!fFrozenValid())
  if (!ptr.fCalculated())
    return false;
  if (!fCalculate(mpRotate))
    return false;
  trq.SetTitle(sFreezeTitle);
  trq.setHelpItem(htpFreeze);
  CalcAutoCorr(_colVertCorr);
  CalcSemiVar(_colVertVar);

  Sx.Size(0);
  Sy.Size(0);
  Sxy.Size(0);
  Sxx.Size(0);
  Syy.Size(0);
  iM.Size(0);
//  fChanged = true;
  return true;
}

void TableAutoCorrSemiVar::Init()
{
  if (pts==0)
    return;
  
	if (map->gr()->rPixSize() != rUNDEF) 
	{  
    _colDistance = pts->col("Distance");
    if (!_colDistance.fValid()) {
      _colDistance = pts->colNew("Distance", Domain("distance"));
      _colDistance->sDescription = STBLMsgPointDistance;
    }
    _colDistance->SetReadOnly(true);
    _colDistance->SetOwnedByTable(true);
  }

  _colHorCorr = pts->col("HorCorr");
  if (!_colHorCorr.fValid()) {
    _colHorCorr = pts->colNew("HorCorr", Domain("value"), ValueRange(-1,1,0.001));
    _colHorCorr->sDescription = STBLMsgHorCorrelation;
  }
  _colHorCorr->SetReadOnly(true);
  _colHorCorr->SetOwnedByTable(true);
  _colVertCorr = pts->col("VertCorr");
  if (!_colVertCorr.fValid()) {
    _colVertCorr = pts->colNew("VertCorr", Domain("value"), ValueRange(-1,1,0.001));
    _colVertCorr->sDescription = STBLMsgVertCorrelation;
  }
  _colVertCorr->SetReadOnly(true);
  _colVertCorr->SetOwnedByTable(true);
  _colHorVar = pts->col("HorSemVar");
  if (!_colHorVar.fValid()) {
     _colHorVar = pts->colNew("HorSemVar", Domain("value"), ValueRange(0,1e10,0.01));
    _colHorVar->sDescription = STBLMsgHorSemVariance;
  }
  _colHorVar->SetOwnedByTable(true);
  _colHorVar->SetReadOnly(true);
  _colVertVar = pts->col("VertSemVar");
  if (!_colVertVar.fValid()) {
    _colVertVar = pts->colNew("VertSemVar", Domain("value"), ValueRange(0,1e10,0.01));
    _colVertVar->sDescription = STBLMsgVertSemVariance;
  }
  _colVertVar->SetOwnedByTable(true);
  _colVertVar->SetReadOnly(true);
}

void TableAutoCorrSemiVar::UnFreeze()
{
  if (_colDistance.fValid()) {
    pts->RemoveCol(_colDistance);
    _colDistance = Column();
  }  
  if (_colHorCorr.fValid()) {
    pts->RemoveCol(_colHorCorr);
    _colHorCorr = Column();
  }  
  if (_colVertCorr.fValid()) {
    pts->RemoveCol(_colVertCorr);
    _colVertCorr = Column();
  }  
  if (_colHorVar.fValid()) {
    pts->RemoveCol(_colHorVar);
    _colHorVar = Column();
  }  
  if (_colVertVar.fValid()) {
    pts->RemoveCol(_colVertVar);
    _colVertVar = Column();
  }
  TableVirtual::UnFreeze();
}

bool TableAutoCorrSemiVar::fCalculate(const Map& map)
{
  long iCols = map->iCols();
  long iLines = map->iLines();
  long c1, c2;
  long iMaxSh=iMaxPixelShift; //  only for speed
//  long iOff = iOffset();
  // init
  for (c1=1; c1<=iRecs(); c1++) {
    Sx[c1]=0.0;  Sy[c1]=0.0;
    Sxy[c1]=0.0; Sxx[c1]=0.0;
    Syy[c1]=0.0; iM[c1]=0L;
  }

//  if (map->dvr() != 0) {
    // buffer for one line of map
    RealBuf rBuf(iCols);
    // array containing square of each pixel value
    RealBuf rsq(iCols);
    double rt;

    trq.SetText(STBLCalculate);
    for (long iLC=0; iLC<iLines; iLC++) {
      map->GetLineVal(iLC,rBuf);
      if (trq.fUpdate(iLC, iLines))
        return false;
      for (c1=0; c1<iCols; c1++) {
        rt = rBuf[c1];
        rsq[c1] = rt != rUNDEF ? rt*rt : 0.0;
      }

      for (c1=0; c1<iCols; c1++) {
        double r1= rBuf[c1];
        if (r1==rUNDEF)
          continue;
        for (c2 = min(iMaxSh,(iCols-c1-1)); c2>=1; c2--) {
        double r2= rBuf[c1+c2];
          if (r2==rUNDEF)
            continue;
          Sx[c2]  += r1;
          Sy[c2]  += r2;
          Sxx[c2] += rsq[c1];
          Syy[c2] += rsq[c1+c2];
          Sxy[c2] += r1*r2;
          iM[c2]++;
        }
      }
    }
  return true;
}

void TableAutoCorrSemiVar::CalcAutoCorr(Column& col)
{
  double r;
  for (long i=1; i <= iRecs(); i++ )
    if (iM[i] <= 0)
      col->PutVal(i, rUNDEF);
    else {
      r = (iM[i] * Sxx[i] - Sx[i] * Sx[i]) * (iM[i] * Syy[i]- Sy[i] * Sy[i]);
      if (r != 0)
        col->PutVal(i, (double)(iM[i] * Sxy[i] - Sx[i] * Sy[i]) / sqrt(r));
      else
        col->PutVal(i, rUNDEF);
    }
}

void TableAutoCorrSemiVar::CalcSemiVar(Column& col)
{
  for (long i=1; i <= iRecs(); i++ )
    if (iM[i] <= 0)
      col->PutVal(i, rUNDEF);
    else
      col->PutVal(i, (double)(Sxx[i] + Syy[i] - 2 * Sxy[i]) / (2 * iM[i]));
} 

void TableAutoCorrSemiVar::CalcDistance(Column& col)
{ 
  for (long i=1; i <= iRecs(); i++ )
    col->PutVal(i, (double)(i * map->gr()->rPixSize()));
}                                      // the last denominator was:  (2 * (iM[i] + i)));

String TableAutoCorrSemiVar::sExpression() const
{
  return String("TableAutoCorrSemiVar(%S,%i)",
                map->sNameQuoted(true, fnObj.sPath()), iMaxPixelShift);
}




