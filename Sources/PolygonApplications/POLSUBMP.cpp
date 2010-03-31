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
/* PolygonMapSubMap
   Copyright Ilwis System Development ITC
   feb 1997, Jelle Wind
    Last change:  JEL   6 May 97    5:59 pm
*/

#include "Engine\Base\Algorithm\Clipline.h"
#include "Applications\Polygon\POLSUBMP.H"
#include "Engine\Base\DataObjects\valrange.h"
#include "Engine\Base\File\Directory.h"
#include "Engine\Base\Algorithm\Qsort.h"
#include "Headers\Err\Ilwisapp.err"
#include "Engine\Base\File\BaseCopier.h"
#include "Engine\Base\File\ObjectCopier.h"
#include "Headers\Htp\Ilwisapp.htp"
#include "Headers\Hs\polygon.hs"

const char* PolygonMapSubMap::sSyntax() {
  return "PolygonMapSubMap(polmap,crdminx,crdminy,crdmaxx,crdmaxy)";
}

PolygonMapSubMap* PolygonMapSubMap::create(const FileName& fn, PolygonMapPtr& p,
                                                 const String& sExpr)
{
  Array<String> as(5);
  if (!IlwisObjectPtr::fParseParm(sExpr, as))
    throw ErrorExpression(sExpr, sSyntax());
  PolygonMap pmp(as[0], fn.sPath());
  CoordBounds cb(Coord(as[1].rVal(), as[2].rVal()), Coord(as[3].rVal(), as[4].rVal()));
  return new PolygonMapSubMap(fn, p, pmp, cb);
}

PolygonMapSubMap::PolygonMapSubMap(const FileName& fn, PolygonMapPtr& p)
: PolygonMapVirtual(fn, p)
{
  fNeedFreeze = false;
  ReadElement("PolygonMapSubMap", "PolygonMap", pmp);
  Init();
  objdep.Add(pmp.ptr());
}

PolygonMapSubMap::PolygonMapSubMap(const FileName& fn, PolygonMapPtr& p, const PolygonMap& pm,
                                   const CoordBounds& cb)
: PolygonMapVirtual(fn, p, pm->cs(),cb,pm->dvrs()), pmp(pm)
{
  fNeedFreeze = true;
  Init();
  objdep.Add(pmp.ptr());
  if (!fnObj.fValid())
    objtime = objdep.tmNewest();
  if (pmp->fTblAttSelf())
    SetAttributeTable(pmp->tblAtt());
}

void PolygonMapSubMap::Store()
{
  PolygonMapVirtual::Store();
  WriteElement("PolygonMapVirtual", "Type", "PolygonMapSubMap");
  WriteElement("PolygonMapSubMap", "PolygonMap", pmp);
}

PolygonMapSubMap::~PolygonMapSubMap()
{
}

String PolygonMapSubMap::sExpression() const
{
  return String("PolygonMapSubMap(%S,%g,%g,%g,%g)", pmp->sNameQuoted(false, fnObj.sPath()),
                                    cb().MinX(), cb().MinY(), cb().MaxX(), cb().MaxY());
}

bool PolygonMapSubMap::fDomainChangeable() const
{
  return false;
}

bool PolygonMapSubMap::fValueRangeChangeable() const
{
  return false;
}

void PolygonMapSubMap::Init()
{
  htpFreeze = htpPolygonMapSubMapT;
  sFreezeTitle = "PolygonMapSubMap";
}

class CoordBoundsSplitCrd {
public:
  CoordBoundsSplitCrd(const CoordBounds& _cb, Array<Coord>& _acrd)
    : cb(_cb), acrd(_acrd)
    { 
    }
  const CoordBounds& cb;
  Array<Coord>& acrd;
};

static bool fLessSplitCrd(long a, long b, void* p)
{
  // sorting coords on boundary clockwise, starting on top left
  CoordBoundsSplitCrd *cbsc = static_cast<CoordBoundsSplitCrd*>(p);
  Coord crd1 = cbsc->acrd[a];
  Coord crd2 = cbsc->acrd[b];
  if (crd1.y == cbsc->cb.MaxY()) { // first on top bound
    if (crd2.y == crd1.y)  // both on top bound
      return crd1.x < crd2.x;
    return true;
  }
  else if (crd1.x == cbsc->cb.MaxX()) { // first on right bound
    if (crd2.x == crd1.x)  // both on right bound
      return crd1.y > crd2.y;
    return crd2.y != cbsc->cb.MaxY();
  }
  else if (crd1.y == cbsc->cb.MinY()) { // first on bottom bound
    if (crd2.y == crd1.y)  // both on bottom bound
      return crd1.x > crd2.x;
    return (crd2.y != cbsc->cb.MaxY()) && (crd2.x != cbsc->cb.MaxX());
  } 
   // first on left bound
  if (crd2.x == cbsc->cb.MinX())  // both on left bound
    return crd1.y < crd2.y;
  return (crd2.y != cbsc->cb.MaxY()) && (crd2.x != cbsc->cb.MaxX()) && (crd2.y != cbsc->cb.MinY());
}

static void SwapSplitCrd(long a, long b, void* p)
{
  CoordBoundsSplitCrd *cbsc = static_cast<CoordBoundsSplitCrd*>(p);
  Coord c = cbsc->acrd[a];
  cbsc->acrd[a] = cbsc->acrd[b];
  cbsc->acrd[b] = c;
}

static bool fUsefullTopology(Topology top, bool fRealValues)
{
  Polygon polLeft = top.polLeft();
  Polygon polRight = top.polRight();
  bool fLeftValid = polLeft.fValid();
  bool fRightValid = polRight.fValid();
  if (fLeftValid) {
    if (fRealValues)
      fLeftValid = polLeft.rValue() != rUNDEF;
    else
      fLeftValid = polLeft.iRaw() != iUNDEF;
  }
  if (fRightValid) {
    if (fRealValues)
      fRightValid = polRight.rValue() != rUNDEF;
    else
      fRightValid = polRight.iRaw() != iUNDEF;
}
  return fLeftValid || fRightValid;
}

bool PolygonMapSubMap::fFreezing()
{
  Init();
  StoreType stp = st();
  bool fClip = (cb().MinX() > pmp->cb().MinX()) || (cb().MaxX() < pmp->cb().MaxX()) ||
               (cb().MinY() > pmp->cb().MinY()) ||  (cb().MaxY() < pmp->cb().MaxY());
  if (fClip) {
    Array<Coord> acrdSplit;
    CoordBuf crdBuf(1000);
    // copy polygon map to temp map
    FileName fnTmp = fnObj;
    fnTmp.sFile = "plsbtmp";
    fnTmp = FileName::fnUnique(fnTmp);
	ObjectCopier::CopyFiles(pmp->fnObj, fnTmp, true); 
    trq.SetTitle(sFreezeTitle);
    PolygonMap pmTmp(fnTmp);
    pmTmp->fChanged = true;
    Coord crdMin = cb().cMin;
    Coord crdMax = cb().cMax;
    long iPols = pmp->iPol();
    trq.SetText(SPOLTextCheckOuterPolygons);
    for (long i=1; i <= iPols; ++i) {
      if (trq.fUpdate(i, iPols))
        return false;
      Polygon pol = pmp->pol(i);
      if (!pol.fValid())
        continue;
      CoordBounds cbo = pol.cbBounds();
      if ((cbo.MinY() < crdMin.y) &&
          (cbo.MaxY() > crdMax.y) &&
          (cbo.MinX() < crdMin.x) &&
          (cbo.MaxX() < crdMax.x)) {
        pol.Delete();
      }
    }
    // split topologies at clip boundary
    trq.SetText(SPOLTextScanTopologies);
    LongArrayLarge aiTop;
    for (Segment seg=pmTmp->smp()->segFirst(); seg.fValid(); ++seg) {
      if (trq.fUpdate(seg.iCurr(), pmTmp->smp()->iSeg()))
        return false;
      aiTop &= seg.iCurr();
    }
    trq.SetText(SPOLTextSplitTopologies);
    for (unsigned long i=0; i < aiTop.iSize(); ++i) {
      if (trq.fUpdate(i, aiTop.iSize()))
        return false;
      Topology top = pmTmp->top(aiTop[i]);
      top.ClipSplit(cb(), acrdSplit);
    }
    if (trq.fUpdate(aiTop.iSize(), aiTop.iSize()))
        return false;
    // add new topologies to connect splitted points
    // first sort Split Coordinates along CoordBounds
    trq.SetText(SPOLTextSortSplitPoints);
    CoordBoundsSplitCrd cbsc(cb(), acrdSplit);
    ::QuickSort(0, acrdSplit.iSize()-1, fLessSplitCrd, SwapSplitCrd, (void*)&cbsc);
    trq.SetText(SPOLTextSplitPolygons);
    // add topologies between split coordinates
    int ii=0;
    for (unsigned long i=0; i < acrdSplit.iSize(); ++i) {
      if (trq.fUpdate(i, acrdSplit.iSize()))
        return false;
      Coord crd1 = acrdSplit[i];
      Coord crd2;
      if (i < acrdSplit.iSize()-1)
        crd2 = acrdSplit[i+1];
      else
        crd2 = acrdSplit[0];
      int iCrd = 2;
      crdBuf[0] = crd1;
      if ((crd1.y != crd2.y) && (crd1.x != crd2.x)) {
        if (crd1.y == crdMin.y)
          crdBuf[1].y = crdMin.y;
        else
          crdBuf[1].y = crdMax.y;
        if ((crd1.y == crdMin.y) || (crd1.y == crdMax.y))
          crdBuf[1].y = crd1.y;
        else
          crdBuf[1].y = crd2.y;
        if ((crd1.x == crdMin.x) || (crd1.x == crdMax.x))
          crdBuf[1].x = crd1.x;
        else
          crdBuf[1].x = crd2.x;
        iCrd = 3;
      }
      crdBuf[iCrd-1] = crd2;
      Topology top = pmTmp->topNewBoundary(String(), iCrd, crdBuf);
      // remove it if it is boundary of two undef polygons
      if (!fUsefullTopology(top, dvrs().fRealValues()))
        if (ii<=1) { ++ii;
        pmTmp->DeleteTopology(top);
        }

    }
    trq.SetText(SPOLTextRemoveOuterPolygons);
    // remove deleted polygons and topologies
    CoordBounds cbMap(cb());
    for (Polygon pol=pmTmp->polFirst(); pol.fValid(); ++pol) {
      if (trq.fUpdate(pol.iCurr(), pmTmp->iPol()))
        return false;
      CoordBounds cbPol = pol.cbBounds();
      if ((cbPol.MinY() < cbMap.MinY()) ||
          (cbPol.MaxY() > cbMap.MaxY()) ||
          (cbPol.MinX() < cbMap.MinX()) ||
          (cbPol.MaxX() > cbMap.MaxX())) {
        pol.PutRaw(iUNDEF);
        // delete all topologies of polygon that have 
        // two 'undefined' polygons as left and right pol
        Topology top = pol.topStart();
        Topology ts = top;
        do {
          if (!fUsefullTopology(top, dvrs().fRealValues()))
            top.Delete(true);
          top = top.topNext();  
        } while (top.iCurr() != ts.iCurr());
        pol.Delete();
      }  
    }

    // compact result
    trq.SetText(SPOLTextCompactPolygonMap);
    PolygonMap pmp;
    pmp.SetPointer(&ptr);
    if (!pmTmp->fCompact(pmp, trq))
      return false;
  }
  else {
    trq.SetText(SPOLTextCopyPolygons);
    long iPol = pmp->iPol();
    for (long i=1; i <= iPol; ++i) {
      if (trq.fUpdate(i, iPol))
        return false;
      Polygon polOld = pmp->pol(i);
      Polygon polNew = pms->polNew();
      polNew.CopyPolygon(polOld);
      if (stp == stREAL)
        polNew.PutVal(polOld.rValue());
      else
        polNew.PutRaw(polOld.iRaw());
    }
    trq.fUpdate(iPol, iPol);
    CoordBuf crdBuf(1000);
    CoordBuf cBuf(1000);
    long iSeg = pmp->smp()->iSeg();
    long iNr;
    trq.SetText(SPOLTextCopyTopology);
    for (long i=1; i <= iSeg; ++i) {
      if (trq.fUpdate(i, iSeg))
        return false;
      Topology topOld = pmp->top(i);
      topOld.GetCoords(iNr, crdBuf);
      Topology topNew = pms->topNew(i);
      topNew.CopyTopology(topOld);
      if (!topOld.fValid())
        topNew.Delete(true);
      else
        topNew.PutCoords(iNr, crdBuf);
    }
  }
/*
  Segment seg, segOut;
  long  iCrdOut;
  RowColBuf rcBufOut(1000);
  CoordBounds cbClip = cb();
  Array<CoordBuf> acrdBuf;
  
  for (seg = smp->segFirst();seg.fValid(); ++seg) {
    if ( (trq.fUpdate(seg.iCurr(), smp->iSeg())) )
      return false;

    seg.Clip(cbClip, acrdBuf);
    for ( long i = 0; i < acrdBuf.iSize(); i++ ) // treat all buffers in array
    {                                            // from nr 0 to nr iSize-1
      iCrdOut = acrdBuf[i].iSize();
      segOut = ptr->segNew();          // make a new segm in new seg map
      seg.WorldCoors2InternalCoors(acrdBuf[i], rcBufOut, iCrdOut);
      segOut.PutCoors(iCrdOut, rcBufOut);
      if (fUseRaw)
        segOut.PutRaw(seg.iRaw());     // take over the raw val of orig segment
      else
        segOut.PutVal(seg.rValue());   // take over the (numeric) val of orig segment
    }
  }
*/
  return true;            // freezing returns true if no errors occurred
}




