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
/* PolygonMapStore, third part
   Copyright Ilwis System Development ITC
   september 1998, by Willem Nieuwenhuis
   ----------
   This part contains several routines:
   - polInner(RowCol)  : point in polygon, the boundary is considered
                         not to belong to the polygon
   - polNewIsland      : a variant that accepts a series of coordinates
                         describing a polygon. The number of coordinates
                         is limited to the size of ArrayLarge (effectively
                         unlimited)
   - CheckIslandsInner : Check whether a new polygon surrounds one
                         or more islands. Internal checks consider
                         the boundaries of the polygons as outside area
   - fContainsInner    : Check whether a point lies within a polygon; the
                         boundary of the polygon is considered outside 
   - fCheckPointInner  : helper function for fContainsInner

   These routines allow the creation of polygons from series of coordinates 
   describing closed segments. They can overlap on the boundary, therefore
   it is necessary to consider the boundary as not belonging to the polygon.
*/

#include "Engine\Map\Polygon\POLSTORE.H"
#include "Engine\Map\Polygon\POLAREA.H"

bool fContainsInner(ilwPolygon& pol, Coord crd);
bool fCheckPointInner(Coord crdA, Coord crdB, Coord crd, long& iLeft, double& dy1);


ilwPolygon PolygonMapStore::polInner(Coord crd) const // point in polygon
{
 CSingleLock sl(&ptr.csAccess, TRUE);
  long id = 0;
  for (int i = 0; i < 5; ++i)
    if (iLastPol[i] >= 0) {
      ilwPolygon p = pol(iLastPol[i]);
      if (!p.fValid())
        continue;
      if (fContainsInner(p, crd)) {
        id = iLastPol[i];
        break;
      }
    }
  if (id == 0)
    for (ilwPolygon p = polFirst(); p.fValid(); ++p)
      if (fContainsInner(p, crd)) {
        id = p.iCurr();
        break;
      }
  const_cast<PolygonMapStore *>(this)->AddLastPol(id);
  return pol(id);
}

// Add polygon. The coordinates describe a closed entity
// The number of points may exceed 1000, this function
// will handle this situation
// The limit is now only dictated by the capacity of ArrayLarge
//
// Assumptions:
//   This polygon does not intersect an already existing one
ilwPolygon PolygonMapStore::polNewIsland(const String& sVal, long iNr, const CoordArrayLarge& acrdPol)
{
 if (iNr <= 2)  // ilwPolygon should have at least 3 coordinates
    return ilwPolygon();
    
  PolArea pa;
  for (int i = 1; i < iNr; ++i)
    pa.Line(acrdPol[i - 1], acrdPol[i]);
  double rArea = pa.rArea();
  double rLen = pa.rLength();

  if (rArea < 0) {
    // change to forward linkage
    for (int i = 0; i < iNr / 2; ++i) {
      Coord crd = acrdPol[i];
      const_cast<Coord&>(acrdPol[i]) = acrdPol[iNr - 1 - i];
      const_cast<Coord&>(acrdPol[iNr - 1 - i]) = crd;
    }
    rArea = -rArea;
  }

  ilwPolygon polOuter = polInner(acrdPol[1]);  // is the new polygon an island
  
  ilwPolygon pNew = polNew();

  int iNrTops = (iNr - 1) / 951 + 1;

  CoordBuf crdBuf(1000);
  Topology tNew = topNew();
  Topology topPrev = tNew;

// Initialize the polygon information
  pNew.TopStart(tNew.iCurr());
  pNew.PutName(sVal);
  pNew.Area(rArea);
  pNew.Len(rLen);
  for (int iTop = 0; iTop < iNrTops; iTop++) {
    // extract points for the next topology from the buffer
    int iFirst = iTop * 950;
    int iLast  = min(iFirst + 950, iNr - 1);
    for (int iCp = iFirst; iCp <= iLast; iCp++)
      crdBuf[iCp - iFirst] = acrdPol[iCp];

    // Fill the new Topology with the coordinates
    tNew.PutCoords(iLast - iFirst + 1, crdBuf);

    // Set the forward and backward links:
    // for each Topology, topPrev being the previous except for the first time
    topPrev.Fwl(tNew.iCurr());
    tNew.Bwl(-topPrev.iCurr());

    tNew.LeftPol(pNew.id);
    tNew.RightPol(-1);    // if this is an island, this is filled in later

    // Only create new Topologies when required
    if (iTop < iNrTops - 1) {
      topPrev = tNew;
      tNew = topNew();
    }
  }
  // Close the polygon at beginning and end
  // and set the bounding box
  Topology topFst = pNew.topStart();
  tNew.Fwl(topFst.iCurr());   // finish the last forward link
  topFst.Bwl(-tNew.iCurr());  // finish the BWL chain
  pNew.SetBounds();
  
  // ilwPolygon is now built except for the (possible) right
  // (outer) polygon reference, so do that now
  // - topPrev contains a link to the last added Topology
  if (polOuter.fValid()) {         
    // The new polygon is an island, so set the right polygon
    // of all topologies to this outer ilwPolygon
    Topology topFirst = pNew.topStart();
    Topology top = topFirst;
    do {
      top.RightPol(polOuter.id);
      top = top.topNext();
    }
    while (top != topFirst);

    // after the loop topCurr is the last Topology in the outer ilwPolygon
    topFirst = polOuter.topStart();
    Topology topCurr;
    Topology topNext = topFirst;
    do {
      topCurr = topNext;
      topNext = topCurr.topNext();
    } 
    while (topNext != topFirst);

    // Adjust the link of the last Topology
    // of the outer ilwPolygon to point to the new island
    if (topCurr.fForward())
      topCurr.Fwl(-topPrev.iCurr());
    else
      topCurr.Bwl(-topPrev.iCurr());

    // Adjust the link of the last Topology in the island
    // to point to the first Topology in the outer ilwPolygon
    if (topFirst.fForward())
      topPrev.Bwl(topNext.iCurr());
    else
      topPrev.Bwl(-topNext.iCurr());

    // Decrease the area of the outer polygon with the
    // Area of the newly added ilwPolygon.
    double rA = polOuter.rArea();
    double rL = polOuter.rLen();
    rA -= rArea;
    rL += rLen;
    polOuter.Area(rA);
    polOuter.Len(rL);
  }

  CheckIslandsInner(pNew, polOuter, 0);
  return pNew;
}


/*
  Check whether the new ilwPolygon pNew is including already existing
  Polygons in the map. If so these are islands and need to be
  linked correctly. Also the area and length of the new ilwPolygon
  need to be corrected in that case.
*/
void PolygonMapStore::CheckIslandsInner(ilwPolygon& pNew, ilwPolygon& polOuter, Tranquilizer* trq)
{
  // polygons surrounding our new polygon ?
  CoordBounds cb = pNew.cbBounds();
  for (ilwPolygon pol = polFirst(); pol.fValid(); ++pol) {
    if (0 != trq)
      trq->fAborted();    // yield to OS
    if (pol.iCurr() == pNew.iCurr())
      continue;
      
    // the mmBounds of pNew should contain the mmBounds of the
    // ilwPolygon under test 'pol', otherwise this polygon can not be
    // contained inside pNew
    if (fContainsComplete(cb, pol.cbBounds())) {
      // if beginning or end of start topology of polygon are not inside outer, continue
      Topology ts = pol.topStart();
      Coord crd = ts.crdBegin();
      if (!fContainsInner(pNew, crd))
        continue;
      crd = ts.crdEnd();
      if (!fContainsInner(pNew, crd))
        continue;

      long iNr;
      CoordBuf buf(1000);
      ts.GetCoords(iNr, buf);
      crd.y = buf[0].y / 2 + buf[1].y / 2;
      crd.x = buf[0].x / 2 + buf[1].x / 2;
      if (!fContainsInner(pNew, crd))
        continue;
      
      // if polygons are direct neighbours, one is not 
      // an island in the other
      bool fBounding = false;
      Topology top = ts;
      long iTops = 0;
      do {
        if (0 != trq)
          trq->fAborted();  // Yield to OS

        ilwPolygon polTest;
        if (top.fForward())
          polTest = top.polRight();
        else
          polTest = top.polLeft();

        if (polTest.iCurr() == pNew.iCurr()) {
          fBounding = true;
          break;
        }
        top = top.topNext();
      } 
      while (top != ts);

      if (fBounding)
        continue;
        
      top = ts;
      iTops = 0;
      do {
        if (++iTops > 5000)
          TooMuchTopsError(pol);
        if (0 != trq)
          trq->fAborted();  // Yield to OS
          
        ilwPolygon polTest;
        if (top.fForward())
          polTest = top.polRight();
        else
          polTest = top.polLeft();
          
        if (polTest.iCurr() == iUNDEF
         || polTest.iCurr() == polOuter.iCurr()
         || polTest.rArea() < 0) {
          RelinkIsland(-top, pNew, trq);
          break;
        }
        top = top.topNext();
      } 
      while (top != ts);
    }
  }
}
bool fContainsInner(ilwPolygon& pol, Coord crd)
{
  if (pol.rArea() < 0)
    return false;
    
  CoordBounds cb = pol.cbBounds();
  if (!cb.fContains(crd)) 
    return false;

  CoordBuf buf(1000);
  Topology tStart = pol.topStart();
  Topology top = tStart;
//   'iLeft' counts the number of horizontal projections on lines 
//           through consecutive vertices lying on the left side
//           of the Point.Col. If this number is even at when all
//           vertices in the polygon have been checked the Point
//           lies inside the ilwPolygon, otherwise outside.
  long iLeft = 0;
  long iNr;
  double rDY;
  do {
    top.GetCoords(iNr, buf);
    if (top.fForward()) {
      rDY = buf[0].y - crd.y;
      for (int i = 1; i < iNr; ++i)
        if (fCheckPointInner(buf[i - 1], buf[i], crd, iLeft, rDY))
          return false;   // Point lies on boundary
    }
    else {
      rDY = buf[iNr - 1].y - crd.y;
      for (int i = iNr - 2; i >= 0; --i) 
        if (fCheckPointInner(buf[i + 1], buf[i], crd, iLeft, rDY)) 
          return false;   // Point lies on boundary
    }
    top = top.topNext();
  }
  while (top != tStart);
  return (iLeft % 2) != 0; // even == inside polygon

}

// check whether a point lies on the edge of a polygon
//   'dy1' contains the row difference of the vertex rcA and the
//         previous vertex
//   'iLeft' counts the number of horizontal projections on lines 
//           through consecutive vertices lying on the left side
//           of the Point.Col.
// Return value:
//   true  = Point lies on the edge
//   false = further checking necessary
bool fCheckPointInner(Coord crdA, Coord crdB, Coord crd, 
                 long& iLeft, double& dy1)
{
  double dy2, x, dxAB, dyAB;
  dy2 = (double) crdB.y - crd.y;
  if ( abs(dy2) < 0.001 )
    if ( crdB.x == crd.x ) 
      return true;    // Point lies on the edge on top of one vertex

  if ( abs(dy1) < 0.001 ) {
    if ( abs(dy2) < 0.001 )
      if ( (crdA.x < crd.x) ^ (crdB.x < crd.x) ) 
        return true;   // Point lies on horizontal edge in between
                       // two vertices
  }
  if ( (dy1 > 0) ^ (dy2 > 0) ) { // Point row lies in between the rows
                                 // of the vertices rcA and rcB
    dxAB = (double) crdB.x - crdA.x;
    dyAB = (double) crdB.y - crdA.y;
    x = crdA.x - dxAB * dy1 / dyAB; // project Point.Col on line through
                                     // vertices rcA and rcB
    if ( x < crd.x - 0.001 )
      iLeft += 1;
    else 
      if ( x < crd.x + 0.001 ) 
        return true;    // Point lies on an edge
  }
  dy1 = dy2;
  return false;
}





