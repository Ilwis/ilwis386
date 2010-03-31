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
/* $Log: /ILWIS 3.0/SegmentMap/CNFRMRSV.H $
 * 
 * 5     10-11-05 11:37 Retsios
 * The application now requires the step-value between iso-lines (instead
 * of the number of iso-lines). This seems more intuitive to a user.
 * 
 * 4     9-11-05 17:57 Retsios
 * Solve memory leaks
 * 
 * 3     9-11-05 12:39 Retsios
 * Re-order the segments in the segmentmap so that the ones with higher
 * iso-values are always on top of segments with lower iso-values
 * 
 * 2     8-11-05 18:21 Retsios
 * Initial seriously working version
 * 
 * 1     7-11-05 18:21 Retsios
 * 
 * 5     27-11-00 15:17 Koolhoven
 * in case of composite name now also produce attribute table 
 * 
 * 4     8-03-00 17:53 Wind
 * removed 1000 coordinates limit
 * 
 * 3     10-12-99 11:51a Martin
 * //->/*
 * 
 * 2     10-12-99 11:49a Martin
 * removed internal rowcols and replaced them by true coords
// Revision 1.4  1998/09/16 17:25:20  Wim
// 22beta2
//
// Revision 1.3  1997/09/02 14:31:01  martin
// Function added to construct names for segments
//
// Revision 1.2  1997/07/29 16:22:31  Wim
// fSingleName to distinguish between "single" or "composite" naming
// of the segments.
//
/* SegmentMapFromRasPol
   Copyright Ilwis System Development ITC
   oct 1996, by Jelle Wind
	Last change:  MS    2 Sep 97    3:17 pm
*/

#ifndef ILWCONFRMRASVAL_H
#define ILWCONFRMRASVAL_H
#include "Engine\Applications\SEGVIRT.H"
#include "Engine\Map\Raster\Map.h"

IlwisObjectPtr * createSegmentMapFromRasValueBnd(const FileName& fn, IlwisObjectPtr& ptr, const String& sExpr, vector<void *> parms=vector<void*>() );
IlwisObjectPtr * createSegmentMapFromRasAreaBnd(const FileName& fn, IlwisObjectPtr& ptr, const String& sExpr, vector<void *> parms );

//enum DirBound { dbNONE, dbUP, dbDOWN, dbLEFT, dbRIGHT };
//
//class ChainRec {
//public:
//  ChainRec() { iLength = 1; dbCode = dbNONE; }
//  ChainRec(DirBound db, long iLen){ iLength = iLen; dbCode = db; }  
//  DirBound dbCode;  
//  long iLength;
//};  
//
//inline ChainRec undef(const ChainRec&) 
//{ return ChainRec(); }
//
//
//class SegBound {
//public:  
////  long iSegNr;
////  long iNrCh, iBegCh, iEndCh;
//  bool fBeginSeg, fEndSeg;
//  Coord crdFrom, crdTo;
//  DList<ChainRec> dlChain;
//  double rLeftVal, rRightVal;
//  void Insert(DirBound db, long iLength) {
//    dlChain.insert(ChainRec(db, iLength));
//  }  
//  void Append(DirBound db, long iLength) {
//    dlChain.append(ChainRec(db, iLength));
//  }  
//  ChainRec& crFirst() {
//    return dlChain.first();
//  }  
//  ChainRec& crLast() {
//    return dlChain.last();
//  }  
//};

class SegBound;

class DATEXPORT SegmentMapFromRasValueBnd: public SegmentMapVirtual
{
  friend class SegmentMapVirtual;
public:
  SegmentMapFromRasValueBnd(const FileName&, SegmentMapPtr& p);
  SegmentMapFromRasValueBnd(const FileName& fn, SegmentMapPtr& p, const Map& mp, double rStartVal, double rEndVal, double rStepVal, bool f8Con, bool fSmooth);
  ~SegmentMapFromRasValueBnd();
  static const char* sSyntax();
  virtual String sExpression() const;
  virtual void Store();
  virtual bool fFreezing();
  virtual bool fDomainChangeable() const;
  static SegmentMapFromRasValueBnd* create(const FileName&, SegmentMapPtr& p, const String& sExpression);
  void Init();
private:
  void NewNode(long iLine, long iCol, byte bt, int iCurrentIsoNr);
  void AppendLeftUp(long iLine, long iCol, int iCurrentIsoNr);
  void AppendUp(long iLine, long iCol, byte bt, int iCurrentIsoNr);
  void AppendLeft(long iLine, long iCol, byte bt, int iCurrentIsoNr);
  SegBound* sbNewInBetween(long iCol);
  SegBound* sbNewWithOneEnd(long iLine, long iCol, bool fRight, bool &fBegin);
  void EndOfSegment(long iLine, long iCol, SegBound& sb, bool fUp, bool& fBegin, int iCurrentIsoNr);
  void StoreSegm(const SegBound& sb, int& iCurrentIsoNr);
  void StoreSegm(CoordBuf& cBuf, long& iCrd, double& rVal);
	int iComparePixels(const double& rFirst, const double& rSecond, const double& rCurrentIsoVal);
//  long iNewSegNr();
  Map map;
  RealBufExt rBufInp;     // rasvec 1.4 var: line
  RealBufExt rBufInpPrev; // rasvec 1.4 var: last_line
  ByteBufExt * dbBufPrev;   // rasvec 1.4 var: prev
  ByteBufExt dbBufCurr;   // rasvec 1.4 var: cur
  ByteBufExt dbBufNext;   // rasvec 1.4 var: next
  bool fEightCon, fSmooth;
  long iSegNr;
  BufExt<SegBound*> *sbHoriz, *sbVert;  // rasvec 1.4 vars: segline[RIGHT] and segline[DOWN]
	Table tblAtt;
	Column col1, col2, colLength;
	double m_rIsoStartVal;
	double m_rIsoEndVal;
	double m_rIsoStepVal;
	list<CoordBuf> *tmpSegmentMap; // remember coordinates here so that they can be stored in the order we want
	list <SegBound*> cleanupList; // to gather the SegBounds that are new-ed and should be deleted
};

#endif




