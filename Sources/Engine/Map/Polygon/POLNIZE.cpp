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
// $Log: /ILWIS 3.0/PolygonMap/POLNIZE.cpp $
 * 
 * 6     5-02-01 19:26 Koolhoven
 * Polygonize() now checks topologies on fwd and bwd links being equal to
 * iUNDEF instead of zero. When was this changed ?
 * 
 * 5     1-03-00 12:42p Martin
 * removed last remnants of old style file handling
 * 
 * 4     13-12-99 12:38 Wind
 * changed crdL and crcF in Polygonize() to zero based
 * 
 * 3     10-12-99 11:49a Martin
 * removed internal coordinates and replaced them by true coords
 * 
 * 2     9/08/99 11:57a Wind
 * comments
*/
// Revision 1.4  1998/09/16 17:25:54  Wim
// 22beta2
//
// Revision 1.3  1998-05-11 12:47:09+01  Wim
// Correct area and length of surrounding polygon after subtracting
// islands
//
/* PolygonMapStore, Polygonize
   Copyright Ilwis System Development ITC
   june 1997, by Wim Koolhoven
   Last change:  WK   11 May 98   12:43 pm
*/

#include "Engine\Map\Polygon\POLSTORE.H"
#include "Engine\Map\Polygon\POLAREA.H"
#include "Headers\Err\Ilwisapp.err"
#include "Headers\Hs\polygon.hs"

static void Azim0Error()
{
  throw ErrorObject(SPOLErrAzimZeroImpossible, errPolygonMapFromSegment);
}

static void NoTopFoundError()
{
  throw ErrorObject(SPOLErrNoTopologyFound, errPolygonMapFromSegment+1);
}

bool PolygonMapStore::Polygonize(bool fCheck, bool fAutoCorrect, Tranquilizer& trq)
{
  KeepOpen(true);
  if (fCheck)
    if (!smp()->fCheckForPolygonize("", fAutoCorrect, trq)) {
      KeepOpen(false);
      return false;
    }

  long iTop = smp()->iSeg();
  trq.SetText(SPOLTextPolygonizing);
  if (trq.fUpdate(0, iTop)) {
    KeepOpen(false);
    return false;
  }
  ArrayLarge<Coord> crdF(iTop+1), crdL(iTop+1); // zero based
  Segment s;
  for (s = smp()->segFirst(); s.fValid(); ++s) {
    int i = s.iCurr();
    crdF[i] = s.crdBegin();
    crdL[i] = s.crdEnd();
  }

  for (Topology tp = top(1); tp.fValid(); ++tp) {
    if (trq.fUpdate(tp.iCurr(), iTop)) {
      KeepOpen(false);
      return false;
    }
    if (iUNDEF == tp.Fwl())
      FindPolygon(tp,crdF,crdL,trq);
    if (iUNDEF == tp.Bwl())
      FindPolygon(-tp,crdF,crdL,trq);
  }
  trq.SetText(SPOLTextInclusions);
  long iPols = iPol();
  for (ilwPolygon pol = polFirst(); pol.fValid(); ++pol) {
    if (trq.fUpdate(pol.id, iPols)) {
      KeepOpen(false);
      return false;
    }
    double rPolArea = pol.rArea();
    if (rPolArea > 0)
      continue;
    ilwPolygon polFound;
    double rAreaFound = 1e100;
    Coord crd = pol.topStart().crdBegin();
    for (ilwPolygon p = polFirst(); p.fValid(); ++p) {
      if (trq.fAborted()) {
        KeepOpen(false);
        return false;
      }
      if (pol.iCurr() == p.iCurr())
        continue;
      if (p.fContains(crd)) {
        double rArea = p.rArea();
        if (rArea > 1.0001 * abs(rPolArea) && rArea < rAreaFound) {
          rAreaFound = rArea;
          polFound = p;
        }
      }
    }
    if (polFound.fValid()) {
      Topology topStart = polFound.topStart();
      Topology top = topStart;
      while (top.topNext() != topStart)
        top = top.topNext();
      Topology topPolStart = pol.topStart();
      top.TopNext(topPolStart);
      Topology t;
      Topology tNext = topPolStart;
      do {
        t = tNext;
        if (t.fForward())
          t.LeftPol(polFound.id);
        else
          t.RightPol(polFound.id);
        tNext = t.topNext();
      } while (tNext != topPolStart);
      t.TopNext(topStart);
      rAreaFound += rPolArea;  // rPolArea is negative
      // correct area and length of surrounding polygon
      double rArea = polFound.rArea() + pol.rArea();
      polFound.Area(rArea);
      double rLen = polFound.rLen() + pol.rLen();
      polFound.Len(rLen);
    }
  }
  KeepOpen(false);
  if (trq.fUpdate(iPols, iPols))
    return false;
  return true;
}

void PolygonMapStore::FindPolygon(Topology topStart,
                   const ArrayLarge<Coord>& crdF,
                   const ArrayLarge<Coord>& crdL,
                   Tranquilizer& trq)
{
  ilwPolygon pol = polNew();
  pol.TopStart(topStart);
  Topology topCurr = topStart;
  do {
    bool fCurrForward = topCurr.fForward();
    Coord crdCurr = fCurrForward ? topCurr.crdEnd() : topCurr.crdBegin();
    long iCurr = topCurr.iCurr();
//    double rAzimCurr = topCurr.rAzim(fCurrForward);
    double rAzimCurr = topCurr.rAzim(true);
    long iFound = 0;
    double rAzimFound = 0;

    int iSeg = smp()->iSeg();
    for (int i = 1; i <= iSeg; ++i) {
      if (trq.fAborted())
        return;
      if (crdF[i] == crdCurr &&
          !(!fCurrForward && (iCurr == i))) {
        Segment s = smp()->seg(i);
        double r = s.rAzim(false);
        r -= rAzimCurr;
        if (r == 0)
          Azim0Error();
      	if (r < 0)
      	  r += 2 * M_PI;
        if (r > rAzimFound) {
          iFound = s.iCurr();
          rAzimFound = r;
        }
      }
      if (crdL[i] == crdCurr &&
          !(fCurrForward && (iCurr == i))) {
        Segment s = smp()->seg(i);
        double r = s.rAzim(true);
        r -= rAzimCurr;
        if (r == 0)
          Azim0Error();
      	if (r < 0)
      	  r += 2 * M_PI;
        if (r > rAzimFound) {
          iFound = -s.iCurr();
          rAzimFound = r;
        }
      }
    }
    if (iFound == 0)
      NoTopFoundError();
    if (fCurrForward) {
      topCurr.Fwl(iFound);
      topCurr.LeftPol(pol.iCurr());
    }
    else {
      topCurr.Bwl(iFound);
      topCurr.RightPol(pol.iCurr());
    }
    topCurr = Topology(this, iFound);
  } while (topCurr != topStart);
  PolArea pa;
  pa.Pol(pol);
  pol.Area(pa.rArea());
  pol.Len(pa.rLength());
  pol.SetBounds();
}







