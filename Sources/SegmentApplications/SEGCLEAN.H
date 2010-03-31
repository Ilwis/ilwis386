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
/* SegmentMapCleaning
   Copyright Ilwis System Development ITC
   jan 1996, by Jan Hendrikse
	Last change:  JEL   6 May 97    5:59 pm
*/

#ifndef ILWSEGMAPCLEANING_H
#define ILWSEGMAPCLEANING_H
#include "Engine\Applications\SEGVIRT.H"
#include "Engine\Table\Col.h"

IlwisObjectPtr * createSegmentMapCleaning(const FileName& fn, IlwisObjectPtr& ptr, const String& sExpr, vector<void *> parms=vector<void*>() );

class DATEXPORT SegmentMapCleaning: public SegmentMapVirtual
{
  friend class SegmentMapVirtual;
public:
  SegmentMapCleaning(const FileName&, SegmentMapPtr& p);
  SegmentMapCleaning(const FileName& fn, SegmentMapPtr& p, const SegmentMap& sm);
  ~SegmentMapCleaning();
  static const char* sSyntax();
  virtual String sExpression() const;
  virtual void Store();
  virtual bool fFreezing();
  virtual bool fDomainChangeable() const;
  virtual bool fValueRangeChangeable() const;
  static SegmentMapCleaning* create(const FileName&, SegmentMapPtr& p, const String& sExpression);
  void Init();
private:
  SegmentMap smp, smpWork;
  long  iSegnr, iSeg2nr;
  double  rTunnelWidthSqr, rFollowWidthSqr, rFollowWidth;
  double rMaxDistSqr;
  Array<Coord> acrdFirst, acrdLast; // holding segmentNodes (First,Last points)
  Array<Coord> acrdMinC, acrdMaxC; // holding segment MinMax rc values
  void AutoDelete(int i,BaseSegment * seg);
  void ReReadFirstLast(int i,BaseSegment *seg);
  void CheckAndSnap(const Coord& crdPoint, long iStart, Array<Coord>& acrdPoints);
  void Snapping(int i,BaseSegment * seg);
  void Snap(Coord& crdPoint);
  double rAzim(const BaseSegment * seg, bool fRear);
  void AutoDeleteShortest(int i,BaseSegment * seg, BaseSegment * seg2);
  void AutoRemove(BaseSegment * seg, Coord crdPoint, long iA, long iB,
                  CoordBuf& crdBuf, long& iNrCrd);
  bool fPossibleTrim(int i, BaseSegment * seg, Coord crdPoint, int j, BaseSegment * seg2, long iA,
                     long  iB, CoordBuf& crdBuf, CoordBuf& crdCj,
		     long& iNrCrd, long& iNrCj);
  void SelfCheck(int k,BaseSegment * seg);
  bool fSplitSeg(int k, BaseSegment * seg, long iPos, const Coord& crdPoint, BaseSegment **segNew);
  bool fAutoSnapSplitBoth(Coord crdPoint, BaseSegment * seg, BaseSegment * seg2, long iA, long iB,
                            BaseSegment ** segNew, BaseSegment ** seg2New);
  void RemoveDuplicate(BaseSegment *, int k,BaseSegment *seg2);
  bool fSplitOK(Coord crdPoint, Coord& crdSpl);
  void FollowStep(const CoordBuf& crdBuf, const CoordBuf& crdBufCj, bool& fOK,
                                    long& i1, long& i2, long iD1, long iD2,
                                    Coord& crdCurr, Coord& crdNext1, Coord& crdNext2);
  bool fFollowing(int k, BaseSegment * seg);
  // bool fHiddenIntersection(BaseSegment * seg);
  bool fHiddenIntersec(int k, BaseSegment * seg, bool& fFoundNewIntersPnt);
  bool fHasInterCoorAsOtherSegsNode(int k, BaseSegment * seg);
  void Neating(int k, BaseSegment * seg);
  bool fGrab();
  bool fCleanUp();
  bool fCopy2();
};

#endif




