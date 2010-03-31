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
// $Log: /ILWIS 3.0/SegmentMap/SGFRMRSP.cpp $
 * 
 * 12    1-12-05 17:21 Retsios
 * Solved memory leaks
 * 
 * 11    29-11-00 14:17 Koolhoven
 * produced columns are now called Area1 and Area2 instead of Pol1 and
 * Pol2
 * 
 * 10    29-11-00 12:09 Koolhoven
 * option in commandline is single|unique instead of single|composite
 * 
 * 9     27-11-00 15:16 Koolhoven
 * in case of composite name now also produce attribute table 
 * 
 * 8     31-08-00 11:53a Martin
 * adminstration of added coords is not quite correct. ia added one copy
 * action for the buffer (to a correct size). This works but is is a hack.
 * 
 * 7     8-03-00 17:53 Wind
 * removed 1000 coordinates limit
 * 
 * 6     10-12-99 11:49a Martin
 * removed internal rowcols and replaced them by true coords
 * 
 * 5     9/29/99 10:49a Wind
 * added case insensitive comparison
 * 
 * 4     9/29/99 10:35a Wind
 * added case insensitive string comparison
 * 
 * 3     9/08/99 12:02p Wind
 * comments
 * 
 * 2     9/08/99 10:27a Wind
 * adpated to use of quoted file names in sExpression()
*/
// Revision 1.10  1998/09/16 17:25:20  Wim
// 22beta2
//
// Revision 1.9  1997/09/09 10:59:40  martin
// Changed the domain(type) from which the domains are asked in sNewName
//
// Revision 1.8  1997/09/02 14:29:35  martin
// Names of segments are now constructed of only codes where available. (else the name).
//
// Revision 1.7  1997/09/01 17:48:25  Wim
// Make internal domain Class if input map has domain Class,
// otherwise make an ID domain.
//
// Revision 1.6  1997-08-21 17:22:10+02  Wim
// Protect pdsrt->iAdd() against already existing items
//
// Revision 1.5  1997-07-30 10:25:46+02  Wim
// Init domain only in fFreezing() function, otherwise no proper storing
//
// Revision 1.4  1997-07-29 18:54:09+02  Wim
// syntax errors
//
// Revision 1.3  1997-07-29 18:22:31+02  Wim
// fSingleName to distinguish between "single" or "composite" naming
// of the segments.
//
// Revision 1.2  1997-07-29 16:46:02+02  Wim
// Internal domain now has 1 item: "Segments"
//
/* SegmentMapFromRasAreaBnd
   Copyright Ilwis System Development ITC
   oct. 1996, by Jelle Wind
	Last change:  MS    9 Sep 97   11:57 am
*/                                                                      
#define ILWSEGFRMRASPOL_C

#include "Headers\toolspch.h"
#include "SegmentApplications\SegBound.H"
#include "SegmentApplications\SGFRMRSP.H"
#include "Engine\Base\DataObjects\valrange.h"
#include "Engine\Domain\DomainUniqueID.h"
#include "Headers\Err\Ilwisapp.err"
#include "Headers\Htp\Ilwisapp.htp"
#include "Headers\Hs\segment.hs"

IlwisObjectPtr * createSegmentMapFromRasAreaBnd(const FileName& fn, IlwisObjectPtr& ptr, const String& sExpr, vector<void *> parms ) {
	if ( sExpr != "")
		return (IlwisObjectPtr *)SegmentMapFromRasAreaBnd::create(fn, (SegmentMapPtr &)ptr, sExpr);
	else
		return (IlwisObjectPtr *)new SegmentMapFromRasAreaBnd(fn, (SegmentMapPtr &)ptr);
}

static void TooManySegments(const FileName& fnObj)
{
  throw ErrorObject(WhatError(SSEGErrTooManySegmentsCreated, errSegmentMapFromRasAreaBnd), fnObj);
}

const char* SegmentMapFromRasAreaBnd::sSyntax() {
  return "SegmentMapFromRasAreaBnd(rasmap,8|4,smooth|nosmooth,single|unique)";
}

SegmentMapFromRasAreaBnd* SegmentMapFromRasAreaBnd::create(const FileName& fn, SegmentMapPtr& p, const String& sExpr)
{
  Array<String> as;
  int iParms = IlwisObjectPtr::iParseParm(sExpr, as);
  if (iParms != 4)
    ExpressionError(sExpr, sSyntax());
  Map map(as[0], fn.sPath());
  String s8Con = as[1];
  bool f8Con = true;
  bool fSmooth = true;
  if ((s8Con != "4") && (s8Con != "8"))
    ExpressionError(sExpr, sSyntax());
  else
    f8Con =  s8Con == "8";
  bool fSmoothParm = true;
  String sSmooth = as[2];
  if (!fCIStrEqual(sSmooth, "smooth") && !fCIStrEqual(sSmooth, "nosmooth"))
    ExpressionError(sExpr, sSyntax());
  else
    fSmooth = fCIStrEqual(sSmooth, "smooth");
  bool fSingle;
  String sNaming = as[3];
  if (fCIStrEqual("single", sNaming))
    fSingle = true;
  else if (fCIStrEqual("unique", sNaming) || fCIStrEqual("composite", sNaming))
    fSingle = false;
  else
    ExpressionError(sExpr, sSyntax());

  return new SegmentMapFromRasAreaBnd(fn, p, map, f8Con, fSmooth, fSingle);
}

SegmentMapFromRasAreaBnd::SegmentMapFromRasAreaBnd(const FileName& fn, SegmentMapPtr& p)
: SegmentMapVirtual(fn, p)
{
  fNeedFreeze = true;
  String sColName;
  try {
    ReadElement("SegmentMapFromRasAreaBnd", "Map", map);
  }
  catch (const ErrorObject& err) {  // catch to prevent invalid object
    err.Show();
    return;
  }
  ReadElement("SegmentMapFromRasAreaBnd", "EightCon", fEightCon);
  ReadElement("SegmentMapFromRasAreaBnd", "Smoothing", fSmooth);
  String sNaming;
  ReadElement("SegmentMapPolBoundaries", "Naming", sNaming);
  fSingleName = ("single" == sNaming);
  Init();
  objdep.Add(map.ptr());
}

SegmentMapFromRasAreaBnd::SegmentMapFromRasAreaBnd(const FileName& fn, SegmentMapPtr& p, const Map& mp, 
                                              bool f8Con, bool fSmth, bool fSingle)
: SegmentMapVirtual(fn, p, mp->cs(),mp->cb(),mp->dvrs()),
  map(mp), fEightCon(f8Con), fSmooth(fSmth), fSingleName(fSingle)
{
  if (map->gr()->fGeoRefNone())
    throw ErrorGeoRefNone(map->gr()->fnObj, errSegmentMapFromRasAreaBnd+1);

  DomainType dmt = fSingleName ? dmtCLASS : dmtUNIQUEID;
	Domain dom(fnObj, 0, dmt, "Boundary");
  SetDomainValueRangeStruct(dom);
	if (!fSingleName) {
	  FileName fnAtt(fnObj, ".tbt", true);
		tblAtt = Table(fnAtt, dom);
		col1 = Column(tblAtt, "Area1", map->dvrs());
		col2 = Column(tblAtt, "Area2", map->dvrs());
		SetAttributeTable(tblAtt);
	}
  SetDomainValueRangeStruct(dom);
  fNeedFreeze = true;
  ptr.SetAlfa(cb());
  Init();
  objdep.Add(map.ptr());
  if (!fnObj.fValid()) // 'inline' object
    objtime = objdep.tmNewest();
  //fEightCon = true; 21/7/97 Wim: Why was this line ever added ?????
}

void SegmentMapFromRasAreaBnd::Store()
{
  SegmentMapVirtual::Store();
  WriteElement("SegmentMapVirtual", "Type", "SegmentMapFromRasAreaBnd");
  WriteElement("SegmentMapFromRasAreaBnd", "Map", map);
  WriteElement("SegmentMapFromRasAreaBnd", "EightCon", fEightCon);
  WriteElement("SegmentMapFromRasAreaBnd", "Smoothing", fSmooth);
  WriteElement("SegmentMapPolBoundaries", "Naming", fSingleName ? "single" : "unique");
}

SegmentMapFromRasAreaBnd::~SegmentMapFromRasAreaBnd()
{
}

String SegmentMapFromRasAreaBnd::sExpression() const
{
  String s8Con, sSmooth;
  if (fEightCon)
    s8Con = "8";
  else
    s8Con = "4";
  if (fSmooth)
    sSmooth = "Smooth";
  else
    sSmooth = "NoSmooth";
  return String("SegmentMapFromRasAreaBnd(%S,%S,%S,%s)",
                map->sNameQuoted(false, fnObj.sPath()),
                s8Con, sSmooth,
                fSingleName ? "single" : "unique");
}

bool SegmentMapFromRasAreaBnd::fDomainChangeable() const
{
  return false;
}

void SegmentMapFromRasAreaBnd::Init()
{
  htpFreeze = htpSegmentMapFromRasAreaBndT;
  sFreezeTitle = "SegmentMapFromRasAreaBnd";
}


bool SegmentMapFromRasAreaBnd::fFreezing()
{
  DomainSort* pdsrt = dm()->pdsrt();
  pdsrt->Resize(0); // clean up domain
  if (fSingleName)
    pdsrt->iAdd("Boundaries");
	else {
		if (!tblAtt.fValid()) {
			FileName fnAtt(fnObj, ".tbt", true);
  		tblAtt = Table(fnAtt, dm());
			SetAttributeTable(tblAtt);
		}
		else
			tblAtt->CheckNrRecs();
		col1 = tblAtt->col("Area1");
		if (!col1.fValid())
			col1 = Column(tblAtt, "Area1", map->dvrs());
		col2 = tblAtt->col("Area2");
		if (!col2.fValid())
			col2 = Column(tblAtt, "Area2", map->dvrs());
		colLength = tblAtt->col("Length");
		if (!colLength.fValid())
			colLength = tblAtt->colNew("Length",DomainValueRangeStruct(0, 1e307, 0.001));	
	}

  cleanupList.clear();
  long i, j;
  trq.SetText(SSEGTextExtractSegments);
  long iLines = map->iLines();
  long iCols = map->iCols();  
  iBufInp.Size(iCols, 1, 1);  // rasvec var: line
  iBufInpPrev.Size(iCols, 1, 1); // rasvec var: last_line
  dbBufPrev.Size(iCols, 1, 1); // rasvec var: prev
  dbBufCurr.Size(iCols, 1, 1); // rasvec var: cur
  dbBufNext.Size(iCols, 1, 1); // rasvec var: next
  sbHoriz.Size(iCols, 1, 1); // rasvec var: segline[RIGHT]
  sbVert.Size(iCols, 1, 1); // rasvec var: segline[DOWN]
  for (j=-1; j <= iCols; ++j) {
    iBufInpPrev[j] = iUNDEF;
    dbBufPrev[j] = dbNONE;
    sbHoriz[j] = 0;
    sbVert[j] = 0;
  }
  for (i=0; i < iLines + 1; ++i) {
    if (trq.fUpdate(i, iLines))
      return false;
    map->GetLineRaw(i, iBufInp);
    iBufInp[-1] = iUNDEF;
    iBufInp[iCols] = iUNDEF;
    // calc dbBufCurr with horizontal directions by comparing vertical differences of pixel rows
    // if upper pixel > lower pixel then direction is RIGHT
    for (j=-1; j <= iCols; ++j) {
      if ( i == iLines + 1 )
        iBufInp[j] = iUNDEF;   //  fill the extra bottom line with Undefs
      if (iBufInpPrev[j]==iBufInp[j])
        dbBufCurr[j] = dbNONE;
      else if (iBufInpPrev[j]>iBufInp[j])
        dbBufCurr[j] = dbRIGHT;
      else
        dbBufCurr[j] = dbLEFT;
    } 
    // calc dbBufNext with vertical directions by comparing horizontal differences of pixel columns
    // if left pixel > right pixel then direction is UP
    dbBufNext[-1] = dbNONE;
    for (j=0; j <= iCols; ++j) {
      if (iBufInp[j-1]==iBufInp[j])
        dbBufNext[j] = dbNONE;
      else if (iBufInp[j-1]>iBufInp[j])
        dbBufNext[j] = dbUP;
      else
        dbBufNext[j] = dbDOWN;
    } 
    
    for (j=0; j <= iCols; ++j) {
      byte b = 0;  
      // b indicates for 4 pixel boundaries where pixels are different
      //
      //          2
      //     pix  |  pix
      //          |
      //     4 ------- 1
      //          |
      //     pix  |  pix
      //          8
      //
      if (dbBufCurr[j] != dbNONE)
        b |= 1;
      if (dbBufPrev[j] != dbNONE)
        b |= 2;
      if (dbBufCurr[j-1] != dbNONE)
        b |= 4;
      if (dbBufNext[j] != dbNONE)
        b |= 8;
      switch (b) {
        case 0: {
            sbHoriz[j] = 0;
            sbVert[j] = 0;
          }
          break;
        case 7: case 11: case 13: case 14 : {
            NewNode(i, j, b);
          }  
          break;
        case 15 : {
            if (!fEightCon)
              NewNode(i, j, b);
            else {
              bool f1 = iBufInpPrev[j]==iBufInp[j-1];
              bool f2 = iBufInpPrev[j-1]==iBufInp[j];
              if (f1 && f2) {
                if (iBufInp[j-1] > iBufInp[j])
                  f2 = false;
                else
                  f1 = false;
              }  
              if (f1) {
                AppendLeftUp(i, j); // b = 6
                //b = 9
                sbVert[j] = sbHoriz[j] = sbNewInBetween(j);
                
              }
              else if (f2) {
                AppendUp(i, j, 1/*3*/); 
                SegBound* h = sbHoriz[j];
                AppendLeft(i, j, 0/*12*/);
                sbHoriz[j] = h;
              }
              else
                NewNode(i, j, b);
            }
            break;
          }
        case 3: case 10 : { // append to upper segment 
            AppendUp(i,j,b);
          }
          break;    
        case 5: case 12 : { // append to left segment 
            AppendLeft(i,j,b);
          }
          break;    
        case 6 : {//  append to left and up (become same segment) 
            AppendLeftUp(i,j);
          }
          break;
        case 9 : {
            sbVert[j] = sbHoriz[j] = sbNewInBetween(j);
          }
          break;
        case 1: case 2: case 4: case 8 : // does not occur 
          break;
      }    
    }
    // swap buffers to be able to use them in the next line
    Swap(iBufInp, iBufInpPrev);
    Swap(dbBufNext, dbBufPrev);
  }
	if (!fSingleName) {
		col1->SetOwnedByTable(true);
		col1->SetReadOnly(true);
		col2->SetOwnedByTable(true);
		col2->SetReadOnly(true);
		colLength->SetOwnedByTable(true);
		colLength->SetReadOnly(true);
		tblAtt->Updated();
	}

	// release allocated memory .. cleanup of SegBound objects
	for (list<SegBound*>::iterator it = cleanupList.begin(); it != cleanupList.end(); ++it)
		delete (*it);

  return true;
}
/*
void SegmentMapFromRasAreaBnd::DetLink(DirBound db1, DirBound db2, DirBound db3)
{
  long iSegNr;
  if (fSegExist[Dir1]) {
    if (fSegExist[Dir2]) {
      iSegNr = sbSeg[Dir2]->iSegNr;
      if (!fBeginSeg[Dir2])
        iSegNr = -iSegNr;
    }
    else {
      iSegNr = sbSeg[Dir3]->iSegNr;
      if (!fBeginSeg[Dir3])
        iSegNr = -iSegNr;
    }
    if (fBeginSeg[Dir1])
//    pt_segm[Dir1]^.top.bwl := snr
    else
//    pt_segm[Dir1]^.top.fwl := snr;
  }
}
*/
void SegmentMapFromRasAreaBnd::NewNode(long iLine, long iCol, byte b)
{
  Array<bool> fSegExist(dbRIGHT+1);
  Array<bool> fBeginSeg(dbRIGHT+1);
  Array<SegBound*> sbSeg(dbRIGHT+1);
  fSegExist[dbRIGHT] = b & 1;
  fSegExist[dbUP   ] = (b & 2)!=0;
  fSegExist[dbLEFT ] = (b & 4)!=0;
  fSegExist[dbDOWN ] = (b & 8)!=0;
  if (fSegExist[dbRIGHT]) {// new segment to the right 
//    GetSegm(pt_segm[RIGHT]);
    sbSeg[dbRIGHT] = sbNewWithOneEnd(iLine, iCol, true, (bool &)fBeginSeg[dbRIGHT]);
    sbHoriz[iCol] = sbSeg[dbRIGHT];
  }
  else
    sbHoriz[iCol] = 0;
  if (fSegExist[dbUP]) { // end of segment up 
    sbSeg[dbUP] = sbVert[iCol];
    EndOfSegment(iLine, iCol, *sbSeg[dbUP], true, (bool &)fBeginSeg[dbUP]);
  }
  if (fSegExist[dbLEFT]) { //end of segment to the left 
    sbSeg[dbLEFT] = sbHoriz[iCol - 1];
    EndOfSegment(iLine, iCol, *sbSeg[dbLEFT], false, (bool &)fBeginSeg[dbLEFT]);
  }
  if (fSegExist[dbDOWN]) { // new segment down }
    sbSeg[dbDOWN] = sbNewWithOneEnd(iLine, iCol, false, (bool &)fBeginSeg[dbDOWN]);
    sbVert[iCol] = sbSeg[dbDOWN];
  }
  else
    sbVert[iCol] = 0;

//  DetLink(RIGHT, DOWN, LEFT);
//  DetLink(UP, RIGHT, DOWN);
//  DetLink(LEFT, UP, RIGHT);
//  DetLink(DOWN, LEFT, UP);

  if (fSegExist[dbUP]) // end of segment up 
    StoreSegm(*sbSeg[dbUP]);
  if (fSegExist[dbLEFT]) // end of segment to the left 
    StoreSegm(*sbSeg[dbLEFT]);
}

void SegmentMapFromRasAreaBnd::AppendLeftUp(long iLine, long iCol)
/* Appends codes of point (iLine, iCol) to segment to the left of point. 
   The segment above the point is chained to the left segment.   
   Is called if type of point = 6 (line from left to up or line  
   from up to left.                                              */
{
  enum WhatDel { DELNONE, DELLEFT, DELUP } del;
  SegBound* sbUp = sbVert[iCol];
  SegBound* sbLeft = sbHoriz[iCol - 1];
  if (sbLeft != sbUp) {
    del = DELNONE;
    if (dbBufCurr[iCol - 1]==dbLEFT) { //line comes from above, goes left }
      if (sbUp->fBeginSeg) { 
        sbUp->dlChain.append(sbLeft->dlChain);
        del = DELLEFT;
        sbUp->fEndSeg = sbLeft->fEndSeg;
        sbUp->crdTo = sbLeft->crdTo;
        StoreSegm(*sbUp);
      }
      else { // sbUp->fBeginSeg == false
        sbLeft->dlChain.insert(sbUp->dlChain);
        del = DELUP;
      }
    }
    else { // line comes from left, goes to above 
        if (sbUp->fEndSeg) {
          sbUp->dlChain.insert(sbLeft->dlChain);
          del = DELLEFT;
          sbUp->fBeginSeg = sbLeft->fBeginSeg;
          sbUp->crdFrom = sbLeft->crdFrom;
          StoreSegm(*sbUp);
        }
        else {
          sbLeft->dlChain.append(sbUp->dlChain);
          del = DELUP;
        }
    }
    // adjust sbHoriz and sbVert by replacing the deleted SegBound with the other one
    if (del == DELLEFT) {
      for (long i = sbHoriz.iLower(); i <= sbHoriz.iUpper(); i++) {
        if (sbHoriz[i] == sbLeft)
          sbHoriz[i] =  sbUp;
        if (sbVert[i] == sbLeft)
          sbVert[i] = sbUp;
      }
    }
    else if (del == DELUP) {
      for (long i = sbHoriz.iLower(); i <= sbHoriz.iUpper(); i++) {
        if (sbHoriz[i] == sbUp)
          sbHoriz[i] =  sbLeft;
        if (sbVert[i] == sbUp)
          sbVert[i] = sbLeft;
      }
    }
    else {
      StoreSegm(*sbLeft);
      StoreSegm(*sbUp);
    }
  }
  else { // segment is contour of island 
    // create new node; start and end of segment 
    // modify segment 
    sbUp->fBeginSeg = true;
    sbUp->crdFrom.x = iCol; sbUp->crdFrom.y = iLine;
    sbUp->fEndSeg = true;
    sbUp->crdTo.x = iCol; sbUp->crdTo.y = iLine;
    StoreSegm(*sbUp);
  }
  sbHoriz[iCol] = 0;
  sbVert[iCol] = 0;
}

void SegmentMapFromRasAreaBnd::AppendUp(long iLine, long iCol, byte b)
// Append chain codes of (iLine,iRow) to segment above point
{
  SegBound* sb = sbVert[iCol];
  DirBound db;
  if ((dbBufCurr[iCol]==dbLEFT) || ((b != 1)) && (dbBufNext[iCol]==dbUP)) {
    // line goes up 
    if (b & 1) 
      db = dbLEFT;
    else
      db = dbUP;
    if (sb->crFirst().dbCode == db)
      sb->crFirst().iLength++;
    else 
      sb->Insert(db, 1); // insert at begin
  }  
  else { // line goes down 
    if (b & 1) 
      db = dbRIGHT;
    else
      db = dbDOWN;
    if (sb->dlChain.last().dbCode == db)
      sb->crLast().iLength++;
    else 
      sb->Append(db, 1); // append at end
  }  
  if (b == 10) {
    sbHoriz[iCol] = 0;
    sbVert[iCol] = sb;
 }
 else {
    sbVert[iCol] = 0;
    sbHoriz[iCol] = sb;
 }
 StoreSegm(*sb);
}

void SegmentMapFromRasAreaBnd::AppendLeft(long iLine, long iCol, byte b)
// Append chain code from (iLine, iCol) to segment to the left.
{
  SegBound* sb = sbHoriz[iCol - 1];
  DirBound db;
  if (((dbBufCurr[iCol]==dbLEFT) && (b != 0)) || (dbBufNext[iCol]==dbUP)) {
    // line goes left 
    if (b & 1) 
      db = dbLEFT;
    else
      db = dbUP;
    if (sb->crFirst().dbCode == db)
      sb->crFirst().iLength++;
    else 
      sb->Insert(db, 1); // insert at begin
  }  
  else { // line goes right 
    if (b & 1) 
      db = dbRIGHT;
    else
      db = dbDOWN;
    if (sb->dlChain.last().dbCode == db)
      sb->crLast().iLength++;
    else 
      sb->Append(db, 1); // append at end
  }  
  if (b == 5) {
    sbVert[iCol] = 0;
    sbHoriz[iCol] = sb;
 }
 else {
    sbHoriz[iCol] = 0;
    sbVert[iCol] = sb;
 }
 StoreSegm(*sb);
}

SegBound* SegmentMapFromRasAreaBnd::sbNewInBetween(long iCol)
// Creates a new segment without nodes but with two chain codes. 
// Is called if point has code 9 ((left,down) or (up,right)).  
{
  SegBound* sb = new SegBound;
  cleanupList.push_back(sb); // administration of objects to clean-up

//  sbHoriz[iCol] = sb;
//  sb->iSegNr = iNewSegNr();
  ChainRec crBeg, crEnd;
  if (dbBufCurr[iCol] == dbLEFT) {
    // left, down 
    crBeg.dbCode = dbLEFT;
    crEnd.dbCode = dbDOWN;
//  Top.PolL := line^[x]; Top.PolR := last_line^[x];
    sb->iLeftRaw = iBufInp[iCol];
    sb->iRightRaw = iBufInpPrev[iCol];
  }
  else { //up, right 
    crBeg.dbCode = dbUP;
    crEnd.dbCode = dbRIGHT;
//    Top.PolL := last_line^[x]; Top.PolR := line^[x];
    sb->iLeftRaw = iBufInpPrev[iCol];
    sb->iRightRaw = iBufInp[iCol];
  }
  sb->dlChain.insert(crBeg); // insert at begin
  sb->dlChain.append(crEnd); // append to end
  sb->fBeginSeg = sb->fEndSeg = false; 
  sb->crdFrom.x = sb->crdFrom.y = sb->crdTo.x = sb->crdTo.y = 0;
  return sb;
}

SegBound* SegmentMapFromRasAreaBnd::sbNewWithOneEnd(long iLine, long iCol, bool fRight, bool& fBegin)
// Creates new segment with a node at one end.                 
// If fRightSeg==true : it has to be a segment to the right of node,
// else a segment under the node.                                 
{
  DirBound db;
  if (fRight) 
    db = (DirBound)dbBufCurr[iCol];
  else
    db = (DirBound)dbBufNext[iCol];
  SegBound* sb = new SegBound;
  cleanupList.push_back(sb); // administration of objects to clean-up

//  sb->iSegNr = iNewSegNr();
  if ((db == dbRIGHT) || (db==dbDOWN)) { // start of segment 
    sb->Insert(db, 1); // at start
    sb->fBeginSeg = true;
    fBegin = true;
    sb->fEndSeg = false;
    sb->crdFrom.x = iCol; sb->crdFrom.y = iLine;
    sb->crdTo.x = sb->crdTo.y = 0;
  }
  else { // end of segment 
    sb->Append(db, 1); // at end
    sb->fBeginSeg = false;
    fBegin = false;
    sb->fEndSeg = true;
    sb->crdFrom.x = sb->crdFrom.y = 0;
    sb->crdTo.x = iCol; sb->crdTo.y = iLine;
  }  
  switch (db)  {
    case dbRIGHT : {
//            top.PolL := last_line^[x]; top.PolR := line^[x];
           sb->iLeftRaw = iBufInpPrev[iCol];
           sb->iRightRaw = iBufInp[iCol];
         }
         break;
    case dbUP : {
//              top.PolL := line^[x - 1]; top.PolR := line^[x];
           sb->iLeftRaw = iBufInp[iCol-1];
           sb->iRightRaw = iBufInp[iCol];
         }
         break;
    case dbLEFT : {
//              top.PolL := line^[x]; top.PolR := last_line^[x];
           sb->iLeftRaw = iBufInp[iCol];
           sb->iRightRaw = iBufInpPrev[iCol];
         }
         break;
    case dbDOWN : {
//                top.PolL := line^[x]; top.PolR := line^[x - 1];
           sb->iLeftRaw = iBufInp[iCol];
           sb->iRightRaw = iBufInp[iCol-1];
         }
         break;
    default : assert(0==1);
      break;
  }
  return sb;
}

void SegmentMapFromRasAreaBnd::EndOfSegment(long iLine, long iCol, SegBound& sb, bool fUp, bool& fBegin)
{
  if ((fUp && (dbBufPrev[iCol] == dbUP)) ||
      (!fUp && (dbBufCurr[iCol-1] == dbLEFT))) { // begin of segment
    fBegin = true;
    sb.fBeginSeg = true;
    sb.crdFrom.x = iCol; sb.crdFrom.y = iLine;
  }
  else { // end of segment 
    fBegin = false;
    sb.fEndSeg = true;
    sb.crdTo.x = iCol; sb.crdTo.y = iLine;
  }  
}

/// StoreSegm function  without smoothing: 
/*

void SegmentMapFromRasAreaBnd::StoreSegm(const SegBound& sb)
{
  if (!sb.fBeginSeg || !sb.fEndSeg)
    return;
  CoordBuf cBuf(1000);
  RowColBuf rcBuf(1000);
  long iCrd;
  // calculate coordinates
  RowCol crdFrom = sb.crdFrom;
  map->gr()->RowCol2Coord(crdFrom.y, crdFrom.x, cBuf[0]);
  iCrd = 1;
  for (DLIter<ChainRec> iter(&const_cast<SegBound&>(sb).dlChain); iter.fValid(); iter++) {
    DirBound db = iter().dbCode;
    switch (db)  {
      case dbRIGHT : {
             crdFrom.x += iter().iLength;
           }
           break;
      case dbUP : {
             crdFrom.y -= iter().iLength;
           }
           break;
      case dbLEFT : {
             crdFrom.x -= iter().iLength;
           }
           break;
      case dbDOWN : {
             crdFrom.y += iter().iLength;
           }
           break;
      default : assert(0==1);
        break;
    }
    map->gr()->RowCol2Coord(crdFrom.y, crdFrom.x, cBuf[iCrd++]);
  }
  
  Segment seg = ptr->newFeature();
  seg.WorldCoors2InternalCoors(cBuf, rcBuf, iCrd);
  seg.PutCoors(iCrd, rcBuf);
  DomainSort* pdsrt = dm()->pdsrt();
  String sVal("%S | %S", map->dm()->sValueByRaw(sb.iLeftRaw,0), map->dm()->sValueByRaw(sb.iRightRaw,0));
  long iRaw = pdsrt->iAdd(sVal);
  seg.PutRaw(iRaw);
}
*/ 

/// Sore Segm function with smoothing:


void SegmentMapFromRasAreaBnd::StoreSegm(const SegBound& sb)
{
  if (!sb.fBeginSeg || !sb.fEndSeg)
    return;
  CoordBuf cBuf;
  long iCrd = 0;
  if (fSmooth) {

    // calculate coordinates
    Coord crdFrom = sb.crdFrom;
    Coord crdTo = sb.crdTo;
    ArrayLarge<ChainRec> acr;
    for (DLIter<ChainRec> iter(&const_cast<SegBound&>(sb).dlChain); iter.fValid(); iter++) 
      acr &= iter();
    cBuf.Size(acr.iSize()*2); // should be sufficient
    bool fIsland = sb.crdFrom == sb.crdTo;
    double x, y, dx, dy;
    x = crdFrom.x;
    y = crdFrom.y;
    long iPrevStep, iCurrStep, iNextStep;
    if ( fIsland ) {
      ChainRec crLast = acr[acr.iSize()-1];
      iPrevStep = crLast.iLength;
      if ((crLast.dbCode == dbUP) || (crLast.dbCode == dbLEFT))
        iPrevStep = -iPrevStep;
    }
    else {
      iPrevStep = 0;
      cBuf[iCrd++] = crdFrom;
    }
    iCurrStep = acr[0].iLength;
    if ((acr[0].dbCode == dbUP) || (acr[0].dbCode == dbLEFT))
      iCurrStep = -iCurrStep;
    Coord cFirst = crdFrom;
    for (unsigned long i=0; i < acr.iSize(); ++i) {
      if ( i != acr.iSize() - 1 ) {
        iNextStep = acr[i+1].iLength;
        if ((acr[i+1].dbCode == dbUP) || (acr[i+1].dbCode == dbLEFT))
          iNextStep = -iNextStep;
      }
      else {
        if ( fIsland ) {
          iNextStep = acr[0].iLength;
          if ((acr[0].dbCode == dbUP) || (acr[0].dbCode == dbLEFT))
            iNextStep = -iNextStep;
        }
        else
          iNextStep = 0;
      }
      switch ( acr[i].dbCode )
      {
        case dbRIGHT: case dbLEFT : {
            dx = iCurrStep; dy = 0;
          }
          break;
        case dbUP: case dbDOWN :  {
            dx = 0; dy = iCurrStep;
          }
          break;

        default :
          assert(0==1);
          break;
      }
      long iNewCrd=0;
      if (abs(iCurrStep) == 1) {
        if ( iPrevStep != iNextStep ) {
          cBuf[iCrd++] = Coord(x + dx / 2, y + dy / 2);
          iNewCrd++;
        }
        if ( !fIsland && (iNextStep == 0 ) ) {
          cBuf[iCrd++] = Coord(x + dx, y + dy);
          iNewCrd++;
        }
      }
      else {
        if ( (abs(iPrevStep) == 1) || (iNextStep == 1) )
          cBuf[iCrd++] = Coord(x + dx / 2, y + dy / 2);
        if ( abs(iNextStep) != 1 )
          cBuf[iCrd++] = Coord(x + dx, y + dy);
      }
      if (fIsland && (iCrd == iNewCrd))
        cFirst = cBuf[0];
      x += dx;
      y += dy;
      iPrevStep = iCurrStep;
      iCurrStep = iNextStep;
    }
    if (fIsland) {
      if (cBuf[0] != cBuf[iCrd-1])
        cBuf[iCrd++] = cBuf[0];
    }
  }
  else {
    // count chains
    int iChains= 0;
    for (DLIter<ChainRec> iter1(&const_cast<SegBound&>(sb).dlChain); iter1.fValid(); iter1++) 
      iChains++;
    cBuf.Size(iChains+1);
    // calculate coordinates
    Coord crdCurr = sb.crdFrom;
    cBuf[iCrd++] = crdCurr;
    for (DLIter<ChainRec> iter(&const_cast<SegBound&>(sb).dlChain); iter.fValid(); iter++) {
      DirBound db = iter().dbCode;
      switch (db)  {
        case dbRIGHT : {
               crdCurr.x += iter().iLength;
             }
             break;
        case dbUP : {
               crdCurr.y -= iter().iLength;
             }
             break;
        case dbLEFT : {
               crdCurr.x -= iter().iLength;
             }
             break;
        case dbDOWN : {
               crdCurr.y += iter().iLength;
             }
             break;
        default : assert(0==1);
          break;
      }
      cBuf[iCrd++] = crdCurr;
    }
  }
	CoordBuf cBuf2(iCrd);
	for(int ic=0; ic < iCrd; ++ic)
		cBuf2[ic] = cBuf[ic];
	
  StoreSegm(sb, cBuf2, iCrd);
}


void SegmentMapFromRasAreaBnd::StoreSegm(const SegBound& sb, CoordBuf& cBuf, long& iCrd)
{
  if (iCrd == 0)
    return;
  for ( long j=0; j < iCrd; ++j)
    map->gr()->RowCol2Coord(cBuf[j].y, cBuf[j].x, cBuf[j]);

  ILWIS::Segment *seg = CSEGMENT(pms->newFeature());
  seg->PutCoords(iCrd, cBuf);
  if (fSingleName)
    seg->PutVal(1L);
  else 
	{
    DomainUniqueID* pdu = dm()->pdUniqueID();
		long iRaw = pdu->iAdd();
		seg->PutVal(iRaw);
		col1->PutRaw(iRaw, sb.iLeftRaw);
		col2->PutRaw(iRaw, sb.iRightRaw);
		colLength->PutVal(iRaw, seg->rLength());
  }
  iCrd = 0;
}



