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
/* $Log: /ILWIS 3.0/SegmentMap/SGFRMRSP.H $
 * 
 * 6     1-12-05 17:21 Retsios
 * Solved memory leaks
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

#ifndef ILWSEGFRMRASPOL_H
#define ILWSEGFRMRASPOL_H
#include "Engine\Applications\SEGVIRT.H"
#include "Engine\Map\Raster\Map.h"

class DATEXPORT SegmentMapFromRasAreaBnd: public SegmentMapVirtual
{
  friend class SegmentMapVirtual;
public:
  SegmentMapFromRasAreaBnd(const FileName&, SegmentMapPtr& p);
  SegmentMapFromRasAreaBnd(const FileName& fn, SegmentMapPtr& p, const Map& mp, bool f8Con, bool fSmooth, bool fSingle);
  ~SegmentMapFromRasAreaBnd();
  static const char* sSyntax();
  virtual String sExpression() const;
  virtual void Store();
  virtual bool fFreezing();
  virtual bool fDomainChangeable() const;
  static SegmentMapFromRasAreaBnd* create(const FileName&, SegmentMapPtr& p, const String& sExpression);
  void Init();
private:
  void NewNode(long iLine, long iCol, byte bt);
  void AppendLeftUp(long iLine, long iCol);
  void AppendUp(long iLine, long iCol, byte bt);
  void AppendLeft(long iLine, long iCol, byte bt);
  SegBound* sbNewInBetween(long iCol);
  SegBound* sbNewWithOneEnd(long iLine, long iCol, bool fRight, bool &fBegin);
  void EndOfSegment(long iLine, long iCol, SegBound& sb, bool fUp, bool& fBegin);
  void StoreSegm(const SegBound& sb);
  void StoreSegm(const SegBound& sb, CoordBuf& cBuf, long& iCrd);
//  long iNewSegNr();
  Map map;
  LongBufExt iBufInp;     // rasvec 1.4 var: line
  LongBufExt iBufInpPrev; // rasvec 1.4 var: last_line
  ByteBufExt dbBufPrev;   // rasvec 1.4 var: prev
  ByteBufExt dbBufCurr;   // rasvec 1.4 var: cur
  ByteBufExt dbBufNext;   // rasvec 1.4 var: next
  bool fEightCon, fSmooth;
  long iSegNr;
  BufExt<SegBound*> sbHoriz, sbVert;  // rasvec 1.4 vars: segline[RIGHT] and segline[DOWN]
  bool fSingleName;
	Table tblAtt;
	Column col1, col2, colLength;
	list <SegBound*> cleanupList; // to gather the SegBounds that are new-ed and should be deleted
};

#endif




