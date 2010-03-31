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
/* SegmentMapRemoveNodes
   Copyright Ilwis System Development ITC
   jan 1996, Jan Hendrikse
	Last change:  JEL   6 May 97    6:02 pm
*/

#include "SegmentApplications\SEGCLEAN.H"
#include "Engine\Base\DataObjects\valrange.h"
#include "Engine\Domain\dmsort.h"
#include "Headers\Err\Ilwisapp.err"
#include "Headers\Htp\Ilwisapp.htp"
#include "Headers\Hs\segment.hs"

IlwisObjectPtr * createSegmentMapCleaning(const FileName& fn, IlwisObjectPtr& ptr, const String& sExpr, vector<void *> parms ) {
	if ( sExpr != "")
		return (IlwisObjectPtr *)SegmentMapCleaning::create(fn, (SegmentMapPtr &)ptr, sExpr);
	else
		return (IlwisObjectPtr *)new SegmentMapCleaning(fn, (SegmentMapPtr &)ptr);
}

const double rEPSILON = 1e-6;
const char* SegmentMapCleaning::sSyntax() {
  return "SegmentMapCleaning(segmap)";
}

SegmentMapCleaning* SegmentMapCleaning::create(const FileName& fn, SegmentMapPtr& p, const String& sExpr)
{
  Array<String> as(1);
  if (!IlwisObjectPtr::fParseParm(sExpr, as))
    throw ErrorExpression(sExpr, sSyntax());
  SegmentMap smp(as[0], fn.sPath());
  return new SegmentMapCleaning(fn, p, smp);
}

SegmentMapCleaning::SegmentMapCleaning(const FileName& fn, SegmentMapPtr& p)
: SegmentMapVirtual(fn, p)
{
  fNeedFreeze = false;
  String sColName;
  ReadElement("SegmentMapCleaning", "SegmentMap", smp);
  Init();
  objdep.Add(smp.ptr());
}

SegmentMapCleaning::SegmentMapCleaning(const FileName& fn, SegmentMapPtr& p, const SegmentMap& sm)
: SegmentMapVirtual(fn, p, sm->cs(),sm->cb(),sm->dvrs()), smp(sm)
{
  fNeedFreeze = true;
  Init();
  objdep.Add(smp.ptr());
  if (!fnObj.fValid())
    objtime = objdep.tmNewest();
  if (smp->fTblAttSelf())
    SetAttributeTable(smp->tblAtt());
}

void SegmentMapCleaning::Store()
{
  SegmentMapVirtual::Store();
  WriteElement("SegmentMapVirtual", "Type", "SegmentMapCleaning");
  WriteElement("SegmentMapCleaning", "SegmentMap", smp);
}

SegmentMapCleaning::~SegmentMapCleaning()
{
}

String SegmentMapCleaning::sExpression() const
{
  return String("SegmentMapCleaning(%S)", smp->sNameQuoted(true, fnObj.sPath()));
}

bool SegmentMapCleaning::fDomainChangeable() const
{
  return false;
}

bool SegmentMapCleaning::fValueRangeChangeable() const
{
  return false;
}

void SegmentMapCleaning::Init()
{
  htpFreeze = htpSegmentMapCleaningT;
  sFreezeTitle = "SegmentMapCleaning";
}

void SegmentMapCleaning::AutoDelete(int i,BaseSegment * seg) 
{
    seg->Delete(true);
    acrdFirst[i] = crdUNDEF;
    acrdLast[i] = crdUNDEF;
} //  } AutoDelete }

void SegmentMapCleaning::ReReadFirstLast(int i, BaseSegment *seg)
{
  CoordBounds cb;
  long iSegnr =i;
  if ( seg->fDeleted() ) {
    acrdFirst[iSegnr] = crdUNDEF;
    acrdLast[iSegnr] = crdUNDEF;
    return;
  }
  acrdFirst[iSegnr] = seg->crdBegin();
  acrdLast[iSegnr] = seg->crdEnd();
  cb = seg->crdBounds();
  if ( cb.cMin == cb.cMax ) {
    AutoDelete(i,seg);
    return;
  }
  acrdMinC[iSegnr] = cb.cMin;
  acrdMaxC[iSegnr] = cb.cMax;
} // ReReadFirstLast }

void SegmentMapCleaning::CheckAndSnap(const Coord& crdPoint, long iStart,
                                Array<Coord>& acrdPoints)
{
  double rD2;
  long iNrCrd;
  CoordBuf  crdBuf(1000);

  for ( int k = 0; k < smp->iSeg(); ++k)   {
	  BaseSegment *seg2 = smp->seg(k);
	  if ( seg2 == NULL || !seg2->fValid())
		  continue;
    if ( acrdPoints[k] == crdUNDEF)   continue;
    rD2 = rDist2(crdPoint, acrdPoints[k]);//dist from given seg endpnt to
    if ( rD2 == 0 )  continue;               //to other seg2 endpnt
    if ( rD2 < rMaxDistSqr) {              //is small enough
      acrdPoints[k] = crdPoint;                //to replace the other one
        //Put_Segment_FL(SegMapNr, i, acrdFirst[iS], acrdLast[iS]);
      CoordinateSequence *crdBuf = seg2->getCoordinates();
      crdBuf[0] = acrdFirst[k];
      crdBuf[iNrCrd-1] = acrdLast[k];
      seg2->PutCoords(iNrCrd, crdBuf);
///      SelfCheck(seg2);
    }
  }
} // CheckAndSnap }

void SegmentMapCleaning::Snapping(int i, BaseSegment * seg)   
{
  long iNr = i;
  CheckAndSnap(acrdFirst[iNr], iNr+1, acrdFirst); //try curr FirstNode with all following FNodes
  CheckAndSnap(acrdFirst[iNr], iNr, acrdLast);    //try it with all following LNodes its own included
  CheckAndSnap(acrdLast[iNr], iNr+1, acrdFirst);  //try curr LastNode with all following FNodes
  CheckAndSnap(acrdLast[iNr], iNr+1, acrdLast);   //try curr LastNode with all following LNodes
} // Snapping }

void SegmentMapCleaning::Snap(Coord& crdPoint)   
{
  long i;
  double rD2;

  for (i = 1; i < pms->iSeg(); i++)   {
    if ( acrdFirst[i] == crdUNDEF)   continue;
    rD2 = rDist2(crdPoint, acrdFirst[i]);
    if ( rD2 < rMaxDistSqr ) {
      crdPoint = acrdFirst[i];
      return;
    }
    rD2 = rDist2(crdPoint, acrdLast[i]);
    if ( rD2 < rMaxDistSqr ) {
      crdPoint = acrdLast[i];
      return;
    }
  }
} // Snap }


double rAzim(const BaseSegment * seg, bool fRear)
{
  long iNrCrd;
  CoordBuf crdBuf;
  Coord crdCrd0, crdCrd1;
  long iDx, iDy;
  double rAz;

  seg->GetCoords(iNrCrd, crdBuf);
  if (fRear) {
    crdCrd0 = crdBuf[iNrCrd-1];
    crdCrd1 = crdBuf[iNrCrd-2];
  }
  else {
    crdCrd0 = crdBuf[0];
    crdCrd1 = crdBuf[1];
  }
  iDx = (long)(crdCrd1.x - crdCrd0.x);  iDy = (long)(crdCrd1.y - crdCrd0.y);
  if (iDx == 0) {
    if (iDy > 0)
      rAz = 0.5 * M_PI;
    else
      rAz = 1.5 * M_PI;
  }
  else {
    rAz = atan2((double)iDy,(double)iDx);
    if (iDx > 0) {
      if (iDy < 0)
	rAz = 2 * M_PI + rAz;
    }
    else
      rAz += M_PI;
  }
  return rAz;
} //{ Azim }

void SegmentMapCleaning::AutoDeleteShortest(int k, BaseSegment * seg, BaseSegment * seg2)
{                           // deletes shortest out of 2 segments seg and seg2
  double rLen1, rLen2;
  CoordBuf crdBuf, crdCjBuf;
  long iNrCrd, iNcrdj;

  seg->GetCoords(iNrCrd, crdBuf,true);
  seg2->GetCoords(iNcrdj, crdCjBuf,true);
  rLen1 = 0;
  for (long i = 1; i < iNrCrd ; i++)
    rLen1 += rDist(crdBuf[i-1], crdBuf[i]);
  rLen2 = 0;
  for (long i =  1; i < iNcrdj; i++)
    rLen2 += rDist(crdCjBuf[i-1], crdCjBuf[i]);
  if (rLen1 < rLen2) AutoDelete(k,seg);
    else AutoDelete(k,seg2);
} // end AutoDeleteShortest


void SegmentMapCleaning::AutoRemove(BaseSegment * seg, Coord crdPoint,  long iA, long iB,
                  CoordBuf& crdBuf, long& iNrCrd) {
  // Everything before exclusive A is ok,
  //  presumably also everything after inclusive B.
  //  Delete everything in between and replace by point crdPoint }
  crdBuf[iA] = crdPoint;
  for (long i = 0; i < iNrCrd-iB; i++)
    crdBuf[iA+1+i] = crdBuf[iB+i];
  iNrCrd += iA - iB + 1;
  seg->PutCoords(iNrCrd, crdBuf);
  /// SelfCheck(seg);
  //WriteLn('AutoRemove: ', Nr:0, ' has now ', Ncrdrd:0, ' coordinates');
} // AutoRemove }

bool SegmentMapCleaning::fPossibleTrim(int k, BaseSegment * seg, Coord crdPoint, int j, BaseSegment * seg2, long iA,
                     long  iB, CoordBuf& crdBuf, CoordBuf& crdCj,
		     long& iNrCrd, long& iNcrdj)
{  /* Hidden intersection, check if close to Node,
    = Overshoot and snapped, if so
    remove everything between iA and Node,
    do same for second segment with iB  */
  double rLenF, rLenL, rD, rDL;
  bool fFF, fFL, fLF, fLL;
  long iNr = k;
  long iNr2 = j;
  fFF = (acrdFirst[iNr] == acrdFirst[iNr2]);
  fFL = (acrdFirst[iNr] == acrdLast[iNr2]);
  fLF = (acrdLast[iNr] == acrdFirst[iNr2]);
  fLL = (acrdLast[iNr] == acrdLast[iNr2]);
  if ( fFF || fLF ) { rD = rDist2(crdPoint, acrdFirst[iNr2]); }
  if ( fFL || fLL ) {
    if ( !(acrdFirst[iNr2] == acrdLast[iNr2]) ) {
      rDL = rDist2(crdPoint, acrdLast[iNr2]);
      if ( fFF || fLF ) {
        if ( rD < rDL )   {
          fFL = false;
          fLL = false;
        }
        else {
          fFF = false;
          fLF = false;
          rD = rDL;
        }
      }
      else rD = rDL;
    } // end if ( !(acrdFirst[iNr2] == acrdLast[iNr2]) )
  }
  if ( ! ((fFF || fFL ) || ( fLF || fLL)))  return false;
  rLenF = rDist2(crdPoint, crdBuf[iA]);
  for (long i = 1; i < iA-2 ; i++ )
    rLenF = rLenF + rDist2(crdBuf[i-1], crdBuf[i]);
  rLenL = rDist2(crdPoint, crdBuf[iA]);
  for (long i = iA + 1; i < iNrCrd; i++)
    rLenL = rLenL + rDist2(crdBuf[i-1], crdBuf[i]);
  if ( rLenF < rLenL ) {
    if ( ! (fFF || fFL) ) return false;
    if ( rD > 3 * rLenL ) return false;
    for (long i = 0; i < iNrCrd - iA ; i++) //all points of seg before iA, except 1st point
      crdBuf[1+i] = crdBuf[iA+i];            // are removed; the rest are shifted iA-1 pos leftward
    iNrCrd = iNrCrd - iA + 1;             // Nr of points in seg updated
  }
  else {
    if ( ! (fLF || fLL) ) return false;
    if ( rD > 3 * rLenF ) return false;
    crdBuf[iA] = crdBuf[iNrCrd-1];          //all points of seg after iA, except last point, removed
    iNrCrd = iA+1;                        // iA points remain (truncation); Nr of points updated
  }
  seg->PutCoords( iNrCrd, crdBuf);
  SelfCheck(k,seg);
  if ( (fFF || fLF) && (fFL || fLL) ) {
    rLenF = rDist2(crdPoint, crdCj[iB-1]);
    for (long i = 1; i < iB; i++)
      rLenF = rLenF + rDist2(crdCj[i-1], crdCj[i]);
      rLenL = rDist2(crdPoint, crdCj[iB]);
      for (long i = iB+1; i < iNcrdj; i++)
    rLenL = rLenL + rDist2(crdCj[i-1], crdCj[i]);
    if ( rLenF < rLenL ) {
      fFL = false;
      fLL = false;
    }
    else {
      fFF = false;
      fLF = false;
    }
  }
  if ( fFF || fLF ) {
    for (long i = 0; i < iNcrdj - iB; i++ )   //all points of seg2 before iB, except 1st point
      crdCj[1+i] = crdCj[iB+i];           // are removed; the rest are shifted iB-1 pos leftward
    iNcrdj = iNcrdj - iB + 1;             // Nr of points in seg2 updated
  }
  else {
    crdCj[iB] = crdCj[iNcrdj-1];        //all points of seg2 after iB, except last point, removed;
    iNcrdj = iB+1;                    // iB points remain (truncation); Nr of points updated
  }
  seg2->PutCoords(iNcrdj, crdCj);
  SelfCheck(j,seg2);
  return true;
} // fPossibleRemove

void SegmentMapCleaning::SelfCheck(int k, BaseSegment * seg) {
  Coord crdA1, crdA2, crdB1, crdB2, crdDA, crdDB, crdMinPoint, crdMaxPoint;
  double rD2, rX, rY, crdA, crdB, rD;
  long iNrCrd, i, iLo, iHi;
  
  Start:
  CoordBuf crdBuf;
  ReReadFirstLast(k,seg);
  long iNr =k;
  if ( acrdFirst[iNr] == crdUNDEF)  return;
  seg->GetCoords( iNrCrd, crdBuf,true);

  crdA1 = crdBuf[0];
  iLo = 0;
  for (i = 1 ; i < iNrCrd - 1; i++)  { // find all points clustered around Firstpnt
    rD2 = rDist2(crdA1, crdBuf[i]);           // of seg  nearer then rD2
    if ( rD2 > rMaxDistSqr )  break;
    iLo = i;                           // index of last point in this cluster
  }                                    // iLo points can be removed
  crdA1 = crdBuf[iNrCrd-1];
  iHi = iNrCrd-1;
  for (i = iNrCrd -2 ; i > 0; i--)   { // find all points clustered around Lastpnt
    rD2 = rDist2(crdA1, crdBuf[i]);           // of seg  nearer then rD2
    if ( rD2 > rMaxDistSqr )  break;
    iHi = i;                           // index of first point in this cluster
  }                                    // iNrCrd-1-iHi points can be removed
                                       // thus iHi - iLo +1 points left

  if ( (iLo > 0) || (iHi < iNrCrd-1) ) {   // at least 1 of the endpoints is
                                           // surrounded by clustered points
    if ( iLo > 1 ) {
      for (i = 1 ; i <= iHi - iLo ; i++) {   // refill the segment buffer
        crdBuf[i] = crdBuf[iLo + i];          // while shifting the iHi-iLo interm values
      }                                    // iLo positions to the left
    }
    crdBuf[iHi - iLo] = crdBuf[iNrCrd-1];    // preserving the last point
    iNrCrd = iHi - iLo + 1;                // resize the segment
    seg->PutCoords(iNrCrd, crdBuf);
    ReReadFirstLast(k,seg);
  }

  if (( iNrCrd == 2 ) &&  (acrdFirst[iNr] == acrdLast[iNr]))  { // seg length == 0
    AutoDelete(k,seg);
    return;
  }
  crdA2 = crdBuf[0];
  for (i = 1 ; i < iNrCrd - 1; i++ )  {
    crdA1 = crdA2;
    crdA2 = crdBuf[i];
    crdMinPoint.x = min(crdA1.x, crdA2.x);
    crdMaxPoint.x = max(crdA1.x, crdA2.x);
    crdMinPoint.y = min(crdA1.y, crdA2.y);
    crdMaxPoint.y = max(crdA1.y, crdA2.y);
    crdDA.x = crdA2.x - crdA1.x;
    crdDA.y = crdA2.y - crdA1.y;
    crdA = (double)(crdA1.x) * crdDA.y - (double)(crdA1.y) * crdDA.x;
    crdB2 = crdA2;
    for (long j = i + 1; j < iNrCrd; j++ )  {
      crdB1 = crdB2;
      crdB2 = crdBuf[j];
      long iSqrLength = 0;
      if ( (crdB1.x > crdMaxPoint.x) && (crdB2.x > crdMaxPoint.x)) continue;// piece B1B2 lies
                                                    //completely to the rightof boundingbox of A1A2
      if ( (crdB1.y > crdMaxPoint.y) && (crdB2.y > crdMaxPoint.y)) continue; // etc
      if ( (crdB1.x < crdMinPoint.x) && (crdB2.x < crdMinPoint.x)) continue;
      if ( (crdB1.y < crdMinPoint.y) && (crdB2.y < crdMinPoint.y)) continue;
      crdDB.x = crdB2.x - crdB1.x;
      crdDB.y = crdB2.y - crdB1.y;
      iSqrLength += (long)(crdDB.x * crdDB.x + crdDB.y * crdDB.y);
      crdB = (double)(crdB1.x) * crdDB.y - (double)(crdB1.y) * crdDB.x;
      rD  = (double)(crdDA.y) * crdDB.x - (double)(crdDB.y) * crdDA.x;

      if (  fabs(rD) < rEPSILON ) {	 // Parallel }
        if ( (i == 1) && (j == iNrCrd-1)) continue;  // Fst = Lst (Node) }
        if  (j == i+1) continue;  // redundant point (by RasVec) }
        if ( crdDA.y == 0 ) {
          if ( (crdB2.x < crdA1.x) ^ (crdB2.x < crdA2.x) )  // B2 between A1 and A2
          {
         // BaseSegment * overlays itself -> Remove false polygon
//WriteLn('AR1');
      ///   if ( iSqrLength < rMaxDistSqr )
              AutoRemove(seg, crdB2, i, j, crdBuf, iNrCrd);
      ///   else
      ///     seg->segSplit(i,crdB2);
            goto Start;
          }
          if ( (crdA2.x < crdB1.x) ^ (crdA2.x < crdB2.x) )  // A2 between B1 and B2
          {
             // BaseSegment * overlays itself -> Remove false polygon
//WriteLn('AR2');
            AutoRemove(seg, crdA2, i, j, crdBuf, iNrCrd);
            goto Start;
          }
        }
        else {    //  when crdDA.y != 0
          rX = crdA1.x + (crdB2.y - crdA1.y) * crdDA.x / crdDA.y;
          if (  fabs(rX - crdB2.x) < rEPSILON ) {
           // BaseSegment * overlays itself -> Remove false polygon
//!WriteLn('AR3');
            AutoRemove(seg, crdB2, i, j, crdBuf, iNrCrd);
            goto Start;
          }
        } // end else
      }
      else {  // not Parallel }
        if ( j == i + 1) continue;
        if ( (i == 1) && (j == iNrCrd-1) ) {  // A1 is 1st point, B2 is Last
          if ( crdA1 == crdB2) continue;  }  // -> Closed BaseSegment *
         /* To compute intersection point:
    Y = Y1 + (X - X1) * dy / dx
  */
        rX = (crdA * crdDB.x - crdB * crdDA.x) / rD;                  // x-coord if inters
        if ( (rX < crdMinPoint.x) || (rX > crdMaxPoint.x)) continue; // outside bounding strip of A1A2
        if ( (rX < crdB1.x) && (rX < crdB2.x)) continue;         // beyond endpoints of
        if ( (rX > crdB1.x) && (rX > crdB2.x) ) continue;        // piece B1B2 itself
        rY = (crdA * crdDB.y - crdB * crdDA.y) / rD;
        if ( (rY < crdMinPoint.y) || (rY > crdMaxPoint.y) ) continue;
        if ( (rY < crdB1.y) && (rY < crdB2.y) ) continue;
        if ( (rY > crdB1.y) && (rY > crdB2.y) ) continue;
        crdB1.x = rounding(rX);
        crdB1.y = rounding(rY);
        // BaseSegment * crosses itself -> Remove false polygon if very small
//WriteLn('AR4');
    ///  if ( iSqrLength < rMaxDistSqr )
        AutoRemove(seg, crdB1, i, j, crdBuf, iNrCrd);
    ///  else                      // if not small create extra nodes at crdB1
    ///    seg->segSplit(j-1,crdB1);   // make 2 new segments, one after the selfcrossing
    ///    seg->segSplit(i-1,crdB1);   // and one being the loop
        goto Start;
      } // end else  (not Parallel)
    } //  end for j = i+1 to iNrCrd-1
  } //  end for i = 1 to iNrCrd-2
} // end SelfCheck }

bool SegmentMapCleaning::fSplitSeg(int k, BaseSegment * seg, long iPos, const Coord& crdPoint, BaseSegment **segNew)
{
  //CoordBounds cb;
  long iSegNr = k;
 //WriteLn('fSplitSeg ', Seg:0, ', pos = ', Pos:0);
  if ( ((acrdFirst[iSegNr] == crdPoint) || (acrdLast[iSegNr] == crdPoint)) ) {
    return false;
  }
  else {
    seg->segSplit(iPos, crdPoint,segNew);
    acrdFirst.Append(1); // allocate one more place in array to allow
    acrdLast.Append(1);  // to store First and Last Node of segNew
    acrdMinC.Append(1);  // and to store its bounding rect values
    acrdMaxC.Append(1);
    ///SelfCheck(segNew);  // this storage is done when SelfCheck calls
    ///SelfCheck(seg);     // ReReadFirstLast
    ReReadFirstLast(k,*segNew);
    ReReadFirstLast(k,seg);
    return true;
  }
} // fSplitSeg }

bool SegmentMapCleaning::fSplitOK(Coord crdPoint, Coord& crdSpl)
{
  Snap(crdSpl); 
  return (rDist2(crdPoint, crdSpl) > 2 * rMaxDistSqr); // helps this "2*" ?
} // fSplitOK }

void SegmentMapCleaning::FollowStep(const CoordBuf& crdBuf, const CoordBuf& crdBufCj, bool& fTogether,
                                    long& i1, long& i2, long iD1, long iD2,
                                    Coord& crdCurr, Coord& crdNext1, Coord& crdNext2)
{
  long iDX1, iDY1, iDX2, iDY2;
  double rD1, rD2, rDist, rDX, rDY, rWidthSqr;

  crdNext1 = crdBuf[i1+iD1];
  if (crdCurr == crdNext1 ) {  // piece of length 0 on main segment seg
    i1 = i1 + iD1;
    return;
  }
  crdNext2 = crdBufCj[i2+iD2];
  if (crdCurr == crdNext2 ) {   // piece of length 0 on other segment seg2
    i2 = i2 + iD2;
//WriteLn;
    return;
  }
  if ( crdNext1 == crdNext2 ) {   // next pieces on both segments coincide
    crdCurr  = crdNext1;          // shift current point
    i1 = i1 + iD1;              // shift counter on seg
    i2 = i2 + iD2;              // shift counter on seg2
//WriteLn;
    return;
  }

  iDX1 = (long)(crdNext1.x - crdCurr.x);
  iDY1 = (long)(crdNext1.y - crdCurr.y);
  iDX2 = (long)(crdNext2.x - crdCurr.x);
  iDY2 = (long)(crdNext2.y - crdCurr.y);

  rD1 = sqrt((double)(iDX1) * iDX1 + (double)(iDY1) * iDY1);  // hypothenuse, piece1
  rD2 = sqrt((double)(iDX2) * iDX2 + (double)(iDY2) * iDY2);  // hypothenuse, piece2

  rDist = min(rD1, rD2);   // shortest followstep

  if (( fabs(iDX1 / rD1 - iDX2 /rD2) > 0.1) || ( fabs(iDY1 / rD1 - iDY2 / rD2) > 0.1)) {
    fTogether = false;
    return;
  }

  rDX = iDX1 * rDist / rD1 - iDX2 * rDist / rD2;
  rDY = iDY1 * rDist / rD1 - iDY2 * rDist / rD2;
  rWidthSqr = rDX * rDX + rDY * rDY;

  if ( rWidthSqr > rFollowWidthSqr ) {
//WriteLn('>>Half');
    rDist = rDist * rFollowWidth / sqrt(rWidthSqr);
    fTogether = false;
    crdCurr.x = rounding(crdCurr.x + iDX1 * rDist / rD1);
    crdCurr.y = rounding(crdCurr.y + iDY1 * rDist / rD1);
  }
  else if ( rD1 < rD2 ) {
//WriteLn('>>1');
    crdCurr = crdNext1;
    i1 = i1 + iD1;
  }
  else { // rD2 <= rD1 }
//WriteLn('>>2');
    crdCurr = crdNext2;
    i2 = i2 + iD2;
  }
} // FollowStep }

bool SegmentMapCleaning::fFollowing(int k, BaseSegment * seg)
{
  CoordBuf crdBuf, crdBufCj;
  long  iNrCrd, iNcrdj, i1, i2, iD1, iD2;
  Coord crdFst, crdLst, crdCurr, crdNext1, crdNext2;
  bool fTogether, fFF, fFL, fLF, fLL;
  long iNr = k;
 Start:
  if ( acrdFirst[iNr] == crdUNDEF )  return true;
  seg->GetCoords(iNrCrd, crdBuf,true);
  crdFst = crdBuf[0];
  crdLst = crdBuf[iNrCrd-1];
  for (int j = 0;  j < pms->iSeg(); ++j )   {
	  BaseSegment* seg2 = pms->seg(j);
	  if ( seg2 == NULL ||!seg2->fValid())
		  continue;
    if ( trq.fAborted() )  return false;
    if ( j == iNr ) continue;
    if ( acrdFirst[j] == crdUNDEF)  continue;
    fFF = (crdFst == acrdFirst[j]);
    fFL = (crdFst == acrdLast[j]);
    fLF = (crdLst == acrdFirst[j]);
    fLL = (crdLst == acrdLast[j]);
    if ( ! (fFF || (fFL || (fLF || fLL)))) continue;
    seg2->GetCoords( iNcrdj, crdBufCj, true);
    
    if ( fFF ) {
//Write('FF');
      fTogether = true;
      i1 = 0;    i2 = 0;    //starting position on seg and seg2 respectiv.
      iD1 = 1;    iD2 = 1;  //starting direction (both forward)
      crdCurr = crdFst;
      while (fTogether && (i1 < iNrCrd-1) && (i2 < iNcrdj-1))
        FollowStep(crdBuf, crdBufCj, fTogether, i1, i2, iD1, iD2, crdCurr, crdNext1, crdNext2);
      if ( ((i1 > 0) || (i2 > 0)) &&   fSplitOK(crdFst, crdCurr) ) {
        if ( fTogether && (i2 == iNcrdj-1) )    // seg2 is redundant subseg of seg
          AutoDelete(j,seg2);
        else {
          if ( fTogether && (i1 == iNrCrd-1) ) { // seg is redundant subseg of seg2
            AutoDelete(k,seg);
            BaseSegment * segNew;
            fSplitSeg( j, seg2, i2, crdCurr, &segNew);
            return true;
          }
          crdBufCj[0] = crdCurr;               // seg and seg2 are together until crdCurr
          iNcrdj = iNcrdj - i2;                // which is after point i2 on seg2
          for (long z = 1 ; z < iNcrdj; z++)
            crdBufCj[z] = crdBufCj[z+i2];      // the seperated part on seg2
          seg->PutCoords(iNcrdj, crdBufCj);     // becomes the new seg2
          SelfCheck(k,seg2);
        }
        BaseSegment * segNew;
        fSplitSeg(k, seg, i1, crdCurr, &segNew);         // the common part becomes seg
                                            // the rest of seg is a new seg3
        goto Start;
      }
    }

    if ( fFL ) {
//Write('FL');
      fTogether = true;
      i1 = 0;    i2 = iNcrdj-1;  // starting position on seg and seg2 respectiv.
      iD1 = 1;    iD2 = -1;     // starting directions, forward and backward resp.
      crdCurr = crdFst;
      while (fTogether && (i1 < iNrCrd-1) && (i2 > 0))
        FollowStep(crdBuf, crdBufCj, fTogether, i1, i2, iD1, iD2, crdCurr, crdNext1, crdNext2);
      if ( ((i1 > 0) || (i2 < iNcrdj-1)) &&  fSplitOK(crdFst, crdCurr) ) {
        if ( fTogether && (i2 == 0) )               // rDist2(crdLst,crdCurr)>2*MaxDistSqr
          AutoDelete(j, seg2);                   // after possible snapping-move of crdCurr
        else {                                // by one of the crdFst,crdLst nodes
          if ( fTogether && (i1 == iNrCrd-1) ) {
            AutoDelete(k,seg);
            BaseSegment * segNew;
            fSplitSeg(k,seg2, i2 - 1, crdCurr, &segNew);
            return true;
          }
          iNcrdj = i2 + 1;
          crdBufCj[iNcrdj - 1] = crdCurr;
          seg2->PutCoords(iNcrdj, crdBufCj);
          SelfCheck(k,seg2);
        }
        BaseSegment * segNew;
        fSplitSeg(j,seg, i1, crdCurr, &segNew);
        goto Start;
      }
    }
      
    if ( fLF ) {
//Write('LF');
      fTogether = true;
      i1 = iNrCrd-1; i2 = 0;
      iD1 = -1;    iD2 = 1;
      crdCurr = crdLst;
      while (fTogether && (i1 > 0) && (i2 < iNcrdj-1))
        FollowStep(crdBuf, crdBufCj, fTogether, i1, i2, iD1, iD2, crdCurr, crdNext1, crdNext2);
      if ( ((i1 < iNrCrd-1) || (i2 > 0)) &&  fSplitOK(crdLst, crdCurr) ) {
        if ( fTogether && (i2 == iNcrdj-1) )
          AutoDelete(k,seg2);
        else {
          if ( fTogether && (i1 == 0) ) {
            AutoDelete(k,seg);
            BaseSegment * segNew;
            fSplitSeg(j,seg2, i2, crdCurr, &segNew);
            return true;
          }
          crdBufCj[0] = crdCurr;
          iNcrdj = iNcrdj - i2;
          for (long k = 1; k < iNcrdj; k++)
            crdBufCj[k] = crdBufCj[k+i2];
          seg2->PutCoords(iNcrdj, crdBufCj);
          SelfCheck(j,seg2);
        }
        BaseSegment * segNew;
        fSplitSeg(k,seg, i1 - 1, crdCurr, &segNew);
        goto Start;
      }
    }

    if ( fLL ) {
//Write('LL');
      fTogether = true;
      i1 = iNrCrd-1; i2 = iNcrdj-1;
      iD1 = -1;    iD2 = -1;
      crdCurr = crdLst;
      while (fTogether && (i1 > 0) && (i2 > 0))
        FollowStep(crdBuf, crdBufCj, fTogether, i1, i2, iD1, iD2, crdCurr, crdNext1, crdNext2);
      if ( ((i1 < iNrCrd-1) || (i2 < iNcrdj-1)) &&  fSplitOK(crdLst, crdCurr) ) {
        if ( fTogether && (i2 == 0) )
          AutoDelete(k,seg2);
        else {
          if ( fTogether && (i1 == 0) ) {
            AutoDelete(k,seg);
            BaseSegment * segNew;
            fSplitSeg(j,seg2, i2 - 1, crdCurr, &segNew);
            return true;
          }
          iNcrdj = i2 + 1;
          crdBufCj[iNcrdj-1] = crdCurr;
          seg2->PutCoords(iNcrdj, crdBufCj);
          SelfCheck(k,seg2);
        }
        BaseSegment * segNew;
        fSplitSeg(k,seg, i1 - 1, crdCurr, &segNew);
        goto Start;
      }
    } // end if fLL
  }  // end for seg2
  return true;
} // end Following

static inline void LineForm(Coord crdPnt1, Coord crdPnt2,
                            double& rA, double& rB, double& crd)
{
  rA = (crdPnt2.y - crdPnt1.y);
  rB = (crdPnt1.x - crdPnt2.x);
  crd = -crdPnt1.x * rA - crdPnt1.y * rB;
}

static inline bool fNonParallel(double rA1, double rB1, double crd1, double rA2, 
                           double rB2, double crd2, double rEpsil, Coord& crdIntersect)
{
  double rD = rA1 * rB2 - rA2 * rB1 ;
  if (fabs(rD) < rEpsil) return false;
  crdIntersect.x = longConv((rB1 * crd2 - rB2 * crd1)/rD);
  crdIntersect.y = longConv((rA2 * crd1 - rA1 * crd2)/rD);
  return true;
}

static bool fInnerOrEndPointIntersec(Coord crdP, Coord crdQ, Coord crdR, Coord crdS,
                                     double rEpsil, Coord& crdIntersect)
{
  double rA1, rB1, crd1, rA2, rB2, crd2;
  LineForm(crdP, crdQ, rA1, rB1, crd1);
  LineForm(crdR, crdS, rA2, rB2, crd2);
  if ( !fNonParallel( rA1, rB1, crd1, rA2, rB2, crd2, rEpsil, crdIntersect)) {
    return false;
  }
  CoordBounds cbPQ(crdP, crdQ);
  cbPQ.Check();   // this is superfluous, already done by constructor MinMax
  CoordBounds cbRS(crdR, crdS);
  cbRS.Check();
  return cbPQ.fContains(crdIntersect) && cbRS.fContains(crdIntersect);
}

bool SegmentMapCleaning::fHiddenIntersec(int k, BaseSegment * seg, bool& fFoundNewIntersPnt)
{
  CoordBuf crdBuf, crdCj;
  long iNrCrd, iNcrdj;
  Coord crdA1, crdA2, crdB1, crdB2, crdIntersecPnt;
  BaseSegment * segNew, *seg2New;
  CoordBounds  cbSeg = seg->crdBounds();
  bool fFoundCommonPnt = false;
  fFoundNewIntersPnt = false;
  if (acrdFirst[k] == crdUNDEF) return false;  //
  seg->GetCoords(iNrCrd, crdBuf,true);
//  for (BaseSegment * seg2 = ptr->segFirst(); seg2->fValid(); ++seg2 ) {
//nextseg:
  int j =0;
  BaseSegment * seg2 = pms->seg(j);
  do  {
    if (trq.fAborted() ) return false;
    if (k == j ) { 
      seg2 = pms->seg(++j); continue;
    }
    if (acrdFirst[j] == crdUNDEF) {
       seg2 = pms->seg(++j); continue;
    }
    CoordBounds  cbSeg2 = seg2->crdBounds();
    if ( !cbSeg.fContains(cbSeg2)) { //no overlap of seg,seg2 possible
       seg2 = pms->seg(++j); continue;
    }
    seg2->GetCoords(iNcrdj, crdCj,true);
    if ((iNrCrd == 2) && ( iNcrdj == 2)) { // 2 coinciding 1-piece segments
      if (((crdBuf[0] == crdCj[0]) && (crdBuf[1] == crdCj[1])) ||
        ((crdBuf[0] == crdCj[1]) && (crdBuf[1] == crdCj[0])))  {
        AutoDelete(k,seg);
        fFoundNewIntersPnt = false;
        return true;
      }
    }
    crdA2 = crdBuf[0];
    for (long i = 1; i < iNrCrd; i++) { // i counts legs of BaseSegment * seg
      crdA1 = crdA2;
      crdA2 = crdBuf[i];
      CoordBounds cbA1A2(crdA1, crdA2);
      if ( !cbA1A2.fContains(cbSeg2)) continue; // A1A2 can't meet seg2
      crdB2 = crdCj[0];
      for (long k = 1; k < iNcrdj; k++) { // k counts legs of BaseSegment * seg2
        crdB1 = crdB2;
        crdB2 = crdCj[k];
        CoordBounds cbB1B2(crdB1, crdB2);
        if ( !cbB1B2.fContains(cbA1A2)) continue; // B1B2 can't meet A1A2
        fFoundCommonPnt = fInnerOrEndPointIntersec(
                       crdA1, crdA2, crdB1, crdB2, rEPSILON, crdIntersecPnt);
        if (fFoundCommonPnt)  {
          //if (! fPossibleTrim (seg, crdIntersecPnt, seg2, i, k, crdBuf,
          //                     crdCj, iNrCrd, iNcrdj)) { //== fPossibleRemove
          fFoundNewIntersPnt = fAutoSnapSplitBoth(crdIntersecPnt, seg, seg2, i, k, &segNew, &seg2New);
               // fFoundNewIntersPnt only true if at least 1 segment is really splitted in new ones
          //}
          if ( !fFoundNewIntersPnt ) continue;
          RemoveDuplicate(seg, j, seg2);
		  RemoveDuplicate(seg, pms->iSeg() -2, seg2New);
          RemoveDuplicate(seg2, pms->iSeg() -1, segNew);
          RemoveDuplicate(segNew, pms->iSeg() -2, seg2New);
        //  ++seg2;
          seg->GetCoords(iNrCrd, crdBuf,true);
          goto nextseg; //return true;
        }
      } // end for k = 0 ; i < iNrj  ;
    } // end for i = 0 ; i < iNrCrd ;
nextseg:
     seg2 = pms->seg(++j);
  } while (seg2->fValid());// for seg2 = ptr->segFirst() ;
  return true;
} // end HiddenIntersec
/*
bool SegmentMapCleaning::fHiddenIntersection(BaseSegment * seg)
                           */

bool SegmentMapCleaning::fAutoSnapSplitBoth(Coord crdPoint, BaseSegment * seg, BaseSegment * seg2, long iA, long iB, BaseSegment **segNew, BaseSegment **seg2New)
{  // Split both segments
    bool fSpl1, fSpl2;
    fSpl1 = fSplitSeg(iA, seg, iA - 1, crdPoint, segNew) ;
    fSpl2 = fSplitSeg(iB, seg2, iB - 1, crdPoint, seg2New) ;
    return fSpl1 || fSpl2 ; // at least one must be really splitted to return true
} // fAutoSnapSplitBoth

void SegmentMapCleaning::RemoveDuplicate(BaseSegment * seg, int k, BaseSegment * seg2)
{
  if ( !seg->fValid() || !seg2->fValid()) return;
  long iNr1, iNr2;
  CoordBuf crdBuf1, crdBuf2;
  seg->GetCoords(iNr1,crdBuf1, true);
  seg2->GetCoords(iNr2,crdBuf2, true);
  if ( iNr1 != iNr2 ) return;
  for ( long j =0; j<iNr1 ;j++ ) {
    if (crdBuf1[j] != crdBuf2[j])
      break;
  }
  for (long j =0; j<iNr1 ;j++ ) {
    if (crdBuf1[j] != crdBuf2[iNr1-j-1])
      return;
  }
  AutoDelete(k,seg2);
}

bool SegmentMapCleaning::fHasInterCoorAsOtherSegsNode(int k, BaseSegment * seg)
{
  CoordBuf crdBuf;
  long iNrCrd;
  Coord crdPoint;

  seg->GetCoords(iNrCrd, crdBuf,true);
  // Split segment then coordinate is node
  for (long i = 1; i < iNrCrd - 1; i++ ) {
    crdPoint = crdBuf[i];
    for (long n = 1; n <= pms->iSeg(); n++ ) {
      if ((crdPoint == acrdFirst[n]) || (crdPoint == acrdLast[n])) {
   //WriteLn('Coordinate is node (', n:0, ')');
        BaseSegment * segNew;
        if (fSplitSeg(k,seg, i - 1, crdPoint, &segNew))
          return true;
      }
    }
  }
  return false;
}// { fHasIntecrdoorAsOtherSegsNode }


void SegmentMapCleaning::Neating(int k, BaseSegment * seg)
{
  CoordBuf crdBuf;
  long iNrCrd;
  long iS = k;
  if ( acrdFirst[iS] == crdUNDEF )  return;
  Snapping(k, seg);
  if ( acrdFirst[iS] == crdUNDEF )  return;
  seg->GetCoords(iNrCrd, crdBuf,true);
  seg->Tunnel(iNrCrd, crdBuf, rTunnelWidthSqr);
  seg->PutCoords(iNrCrd, crdBuf);
  ReReadFirstLast(k, seg);
  if ( acrdFirst[iS] == crdUNDEF )  return;
  SelfCheck(k, seg);
  if ( ( iNrCrd == 2 ) && (acrdFirst[iS] == acrdLast[iS]) )
    AutoDelete(k, seg);
}  // end Neating

bool SegmentMapCleaning::fGrab()  // from InPutMap to WorkMap;
{
  bool fUseRaw = st() != stREAL;
  long iCrd; 
  CoordBuf crdBuf;
  double rMaxDist, rWidth;
  rWidth = 0.25 * 6;
  rTunnelWidthSqr = rWidth * rWidth / 0.0625;
  rFollowWidthSqr = rTunnelWidthSqr;
  rMaxDist = rWidth;
  rMaxDistSqr = rMaxDist * rMaxDist / 0.0625;
  if ( rFollowWidthSqr > rMaxDistSqr / 3 )
    rFollowWidthSqr = rMaxDistSqr / 3;
  rFollowWidth = sqrt(rFollowWidthSqr);

  trq.SetText(SSEGTextGrabbing);
/*  acrdFirst.Resize(smp->iSeg(),1);   // allocate at least as many array places for First and
  acrdLast.Resize(smp->iSeg(),1);    // and Last Nodes as there are segments in the initial map
  acrdMinC.Resize(smp->iSeg(),1);
  acrdMaxC.Resize(smp->iSeg(),1);*/
  acrdFirst.Resize(smp->iSeg()+1);   // allocate at least as many array places for First and
  acrdLast.Resize(smp->iSeg()+1);    // and Last Nodes as there are segments in the initial map
  acrdMinC.Resize(smp->iSeg()+1);
  acrdMaxC.Resize(smp->iSeg()+1);
  for ( long i = 1; i <= smp->iSeg() ; i++){
    acrdFirst[i] = crdUNDEF;
    acrdLast[i] = crdUNDEF;
    acrdMinC[i] = crdUNDEF;
    acrdMaxC[i] = crdUNDEF;
  }
  for (int i = 0; i < smp->iSeg(); ++i )  {
	  BaseSegment *seg = smp->seg(i);
	  if ( seg == NULL || !seg->fValid())
		  continue;
    if ( trq.fUpdate(i,smp->iSeg()) )  
      return false;
    seg->GetCoords(iCrd,crdBuf,true);
    if (pms->fSegExist(iCrd, crdBuf, seg->crdBounds(), &trq))
      continue; // exclude exactly double segments
    BaseSegment * segWork = pms->segNew(); // new segment created
    segWork->PutCoords(iCrd,crdBuf);
    if  (fUseRaw)
      segWork->PutVal(seg->iValue());
    else
      segWork->PutVal(seg->rValue());
  }
  return true;
} // end fGrab

bool SegmentMapCleaning::fCleanUp()
{
  trq.SetText(SSEGTextPrewash);
  for (int i=0; i < pms->iSeg(); ++i )  {
	  BaseSegment *segWork = pms->seg(i);
	  if ( segWork == NULL || !segWork->fValid())
		  continue;
    if ( trq.fUpdate(i,pms->iSeg()) )  return false;
    ReReadFirstLast(i,segWork);
  }
  
  trq.SetText(SSEGTextSnapping);
  for (int i=0; i < pms->iSeg(); ++i )  {
	  BaseSegment *segWork = pms->seg(i);
	  if ( segWork == NULL || !segWork->fValid())
		  continue;
    if ( trq.fUpdate(i,pms->iSeg()) )  return false;

    if ( acrdFirst[i] == crdUNDEF ) continue;
    Snapping(i,segWork);
  }
  
  trq.SetText(SSEGTextSelfCheckFalseLoops);
 for (int i=0; i < pms->iSeg(); ++i )  {
	  BaseSegment *segWork = pms->seg(i);
	  if ( segWork == NULL || !segWork->fValid())
		  continue;
    if ( trq.fUpdate(i,pms->iSeg()) )  return false;

    if ( acrdFirst[i] == crdUNDEF ) continue;
    SelfCheck(i,segWork);
  }

  trq.SetText(SSEGTextRemoveDoubleSegments);

  for (int i=0; i < pms->iSeg(); ++i )  {
	  BaseSegment *segWork = pms->seg(i);
	  if ( segWork == NULL || !segWork->fValid())
		  continue;
    if ( trq.fUpdate(i,pms->iSeg()) )  return false;

    if ( acrdFirst[i] == crdUNDEF ) continue;
    fFollowing(i,segWork);
  }
  
  trq.SetText(SSEGTextNodesAtIntersections);
  bool fNewNodeMade;
   for (int i=0; i < pms->iSeg(); ++i )  {
	  BaseSegment *segWork = pms->seg(i);
	  if ( segWork == NULL || !segWork->fValid())
		  continue;
    if ( trq.fUpdate(i,pms->iSeg()) )  return false;

    if ( acrdFirst[i] == crdUNDEF ) continue;
    fHiddenIntersec(i,segWork, fNewNodeMade);
  }

/*  trq.SetText("Removing New Double Segments");

  for (segWork = ptr->segFirst(); segWork.fValid(); ++segWork )  {
    if ( trq.fUpdate(segWork.iCurr(),ptr->iSeg()) )  return false;

    if ( acrdFirst[segWork.iCurr()] == crdUNDEF ) continue;
    fFollowing(segWork);
  }
*/

  trq.SetText(SSEGTextRemoveDoubleNodes);
   for (int i=0; i < pms->iSeg(); ++i )  {
	  BaseSegment *segWork = pms->seg(i);
	  if ( segWork == NULL || !segWork->fValid())
		  continue;
      if ( trq.fUpdate(i,pms->iSeg()) )  return false;
      Neating(i, segWork);
  }
  /*
  long iOldNr;
  do {
    long iSegNummer = segWork.iCurr();
    long iSegAantal = ptr->iSeg();
    bool fFirstPntUndef, fNewNodeMade;
    iOldNr = ptr->iSeg();
    trq.SetText("Rubbing / Finding Intersections");
    for (segWork = ptr->segFirst(); segWork.fValid(); ++segWork )  {
      iSegNummer = segWork.iCurr();
      iSegAantal = ptr->iSeg();
      if ( trq.fUpdate(iSegNummer) )  return false;
    S1:
      fFirstPntUndef = (acrdFirst[iSegNummer] == crdUNDEF);
      if ( fFirstPntUndef  ) continue;
      if ( !fFollowing(segWork)) return false;
      iSegAantal = ptr->iSeg();
      if ( (acrdFirst[iSegNummer] == crdUNDEF) ) continue;
      if ( fHasIntecrdoorAsOtherSegsNode(segWork) )  goto S1; // New Node is made
      if ( !fHiddenIntersec(segWork, fNewNodeMade) ) return false;
      if ( fNewNodeMade ) goto S1;
    }

    trq.SetText("Neating  ");
    for (segWork = ptr->segFirst(); segWork.fValid(); ++segWork )  {
      if ( trq.fUpdate(segWork.iCurr(),ptr->iSeg()) )  return false;
      Neating(segWork);
    }
  }  while (iOldNr != ptr->iSeg() ); // end do while iOldNr !=
  */
  return true;
} // end fCleanUp }

bool SegmentMapCleaning::fCopy2()
{
  /// long s = 1, ResSeg =  1;
  // double rAlfa, rBeta1, rBeta2;
  /// CoordBuf crdBuf(1000);
  /// long iNrCrd ;
  return true;
}

bool SegmentMapCleaning::fFreezing()
{
  Init();
  if ( !fGrab()) 
    return false;

  if ( !fCleanUp()) 
    return false;

  if ( !fCopy2()) 
    return false;

  return true;
}




