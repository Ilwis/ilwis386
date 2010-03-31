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
// $Log: /ILWIS 3.0/SegmentMap/CNFRMRSV.cpp $
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
#define ILWCONFRMRASVAL_C

#include "Headers\toolspch.h"
#include "SegmentApplications\SegBound.H"
#include "SegmentApplications\CNFRMRSV.H"
#include "Engine\Base\DataObjects\valrange.h"
#include "Engine\Domain\DomainUniqueID.h"
#include "Headers\Err\Ilwisapp.err"
#include "Headers\Htp\Ilwisapp.htp"
#include "Headers\Hs\segment.hs"

IlwisObjectPtr * createSegmentMapFromRasValueBnd(const FileName& fn, IlwisObjectPtr& ptr, const String& sExpr, vector<void *> parms ) {
	if ( sExpr != "")
		return (IlwisObjectPtr *)SegmentMapFromRasValueBnd::create(fn, (SegmentMapPtr &)ptr, sExpr);
	else
		return (IlwisObjectPtr *)new SegmentMapFromRasValueBnd(fn, (SegmentMapPtr &)ptr);
}

static void TooManySegments(const FileName& fnObj)
{
  throw ErrorObject(WhatError(SSEGErrTooManySegmentsCreated, errSegmentMapFromRasAreaBnd), fnObj);
}

const char* SegmentMapFromRasValueBnd::sSyntax() {
  return "SegmentMapFromRasValueBnd(rasmap,startval,endval,stepval,8|4,smooth|nosmooth)";
}

SegmentMapFromRasValueBnd* SegmentMapFromRasValueBnd::create(const FileName& fn, SegmentMapPtr& p, const String& sExpr)
{
  Array<String> as;
  int iParms = IlwisObjectPtr::iParseParm(sExpr, as);
  if (iParms != 6)
    ExpressionError(sExpr, sSyntax());
  Map map(as[0], fn.sPath());
	double rIsoStartVal = as[1].rVal();
	double rIsoEndVal = as[2].rVal();
	double rIsoStepVal = as[3].rVal();
  String s8Con = as[4];
  bool f8Con = true;
  bool fSmooth = true;
  if ((s8Con != "4") && (s8Con != "8"))
    ExpressionError(sExpr, sSyntax());
  else
    f8Con =  s8Con == "8";
  bool fSmoothParm = true;
  String sSmooth = as[5];
  if (!fCIStrEqual(sSmooth, "smooth") && !fCIStrEqual(sSmooth, "nosmooth"))
    ExpressionError(sExpr, sSyntax());
  else
    fSmooth = fCIStrEqual(sSmooth, "smooth");

  return new SegmentMapFromRasValueBnd(fn, p, map, rIsoStartVal, rIsoEndVal, rIsoStepVal, f8Con, fSmooth);
}

SegmentMapFromRasValueBnd::SegmentMapFromRasValueBnd(const FileName& fn, SegmentMapPtr& p)
: SegmentMapVirtual(fn, p)
{
  fNeedFreeze = true;
  String sColName;
  try {
    ReadElement("SegmentMapFromRasValueBnd", "Map", map);
  }
  catch (const ErrorObject& err) {  // catch to prevent invalid object
    err.Show();
    return;
  }
	ReadElement("SegmentMapFromRasValueBnd", "IsoStartValue", m_rIsoStartVal);
	ReadElement("SegmentMapFromRasValueBnd", "IsoEndValue", m_rIsoEndVal);
	ReadElement("SegmentMapFromRasValueBnd", "IsoStepValue", m_rIsoStepVal);
  ReadElement("SegmentMapFromRasValueBnd", "EightCon", fEightCon);
  ReadElement("SegmentMapFromRasValueBnd", "Smoothing", fSmooth);
  Init();
  objdep.Add(map.ptr());
}

SegmentMapFromRasValueBnd::SegmentMapFromRasValueBnd(const FileName& fn, SegmentMapPtr& p, const Map& mp, 
                                              double rStartVal, double rEndVal, double rIsoStepVal, bool f8Con, bool fSmth)
: SegmentMapVirtual(fn, p, mp->cs(),mp->cb(),mp->dvrs()),
  map(mp), m_rIsoStartVal(rStartVal), m_rIsoEndVal(rEndVal), m_rIsoStepVal(rIsoStepVal), fEightCon(f8Con), fSmooth(fSmth)
{
  if (map->gr()->fGeoRefNone())
    throw ErrorGeoRefNone(map->gr()->fnObj, errSegmentMapFromRasAreaBnd+1);

  SetDomainValueRangeStruct(map->dm());
  fNeedFreeze = true;
  ptr.SetAlfa(cb());
  Init();
  objdep.Add(map.ptr());
  if (!fnObj.fValid()) // 'inline' object
    objtime = objdep.tmNewest();
  //fEightCon = true; 21/7/97 Wim: Why was this line ever added ?????
}

void SegmentMapFromRasValueBnd::Store()
{
  SegmentMapVirtual::Store();
  WriteElement("SegmentMapVirtual", "Type", "SegmentMapFromRasValueBnd");
  WriteElement("SegmentMapFromRasValueBnd", "Map", map);
	WriteElement("SegmentMapFromRasValueBnd", "IsoStartValue", m_rIsoStartVal);
	WriteElement("SegmentMapFromRasValueBnd", "IsoEndValue", m_rIsoEndVal);
	WriteElement("SegmentMapFromRasValueBnd", "IsoStepValue", m_rIsoStepVal);
  WriteElement("SegmentMapFromRasValueBnd", "EightCon", fEightCon);
  WriteElement("SegmentMapFromRasValueBnd", "Smoothing", fSmooth);
}

SegmentMapFromRasValueBnd::~SegmentMapFromRasValueBnd()
{
}

String SegmentMapFromRasValueBnd::sExpression() const
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
  return String("SegmentMapFromRasValueBnd(%S,%lg,%lg,%lg,%S,%S)",
                map->sNameQuoted(false, fnObj.sPath()),
								m_rIsoStartVal, m_rIsoEndVal, m_rIsoStepVal,
                s8Con, sSmooth);
}

bool SegmentMapFromRasValueBnd::fDomainChangeable() const
{
  return false;
}

void SegmentMapFromRasValueBnd::Init()
{
  htpFreeze = htpSegmentMapFromRasAreaBndT;
  sFreezeTitle = "SegmentMapFromRasValueBnd";
}

int SegmentMapFromRasValueBnd::iComparePixels(const double& rFirst, const double& rSecond, const double& rCurrentIsoVal)
{
	// two pixels are different if one is on one side of the iso line, and the other is on the other side of it
	// "one side of the iso line" = pixel value is smaller
	// "other side of the iso line" = pixel value is greater or equal
	// two pixels that are both smaller or both greather or equal than the iso line, are considered equal for this comparison function
	int iRet = 0;
	if (rFirst < rCurrentIsoVal && rSecond >= rCurrentIsoVal)
		iRet = -1;
	else if (rFirst >= rCurrentIsoVal && rSecond < rCurrentIsoVal)
		iRet = 1;
	return iRet;
}

bool SegmentMapFromRasValueBnd::fFreezing()
{
	if (m_rIsoStepVal == 0) // would cause "div. by zero"
		return false;
	
	int iNrIsoLevels = (int)((m_rIsoEndVal - m_rIsoStartVal) / m_rIsoStepVal);
	
	if (iNrIsoLevels <= 0) // no lines to draw
		return false;
	
	dbBufPrev = new ByteBufExt [iNrIsoLevels];
	sbHoriz = new BufExt<SegBound*> [iNrIsoLevels];
	sbVert = new BufExt<SegBound*> [iNrIsoLevels];
	tmpSegmentMap = new list<CoordBuf> [iNrIsoLevels];
	cleanupList.clear();
	//
  long i, j;
  trq.SetText(SSEGTextExtractSegments);
  long iLines = map->iLines();
  long iCols = map->iCols();  
  rBufInp.Size(iCols, 1, 1);  // rasvec var: line
  rBufInpPrev.Size(iCols, 1, 1); // rasvec var: last_line
	
	int iCurrentIsoLine;
	for (iCurrentIsoLine = 0; iCurrentIsoLine < iNrIsoLevels; ++iCurrentIsoLine)
	{
		dbBufPrev[iCurrentIsoLine].Size(iCols, 1, 1); // rasvec var: prev
	  sbHoriz[iCurrentIsoLine].Size(iCols, 1, 1); // rasvec var: segline[RIGHT]
	  sbVert[iCurrentIsoLine].Size(iCols, 1, 1); // rasvec var: segline[DOWN]
	}
  
	dbBufCurr.Size(iCols, 1, 1); // rasvec var: cur
  dbBufNext.Size(iCols, 1, 1); // rasvec var: next
  for (j=-1; j <= iCols; ++j) {
    rBufInpPrev[j] = rUNDEF;
		for (iCurrentIsoLine = 0; iCurrentIsoLine < iNrIsoLevels; ++iCurrentIsoLine)
		{
			dbBufPrev[iCurrentIsoLine][j] = dbNONE;
	    sbHoriz[iCurrentIsoLine][j] = 0;
		  sbVert[iCurrentIsoLine][j] = 0;
		}
  }
  for (i=0; i < iLines + 1; ++i) {
    if (trq.fUpdate(i, iLines))
      return false;
    map->GetLineVal(i, rBufInp);
    rBufInp[-1] = rUNDEF;
    rBufInp[iCols] = rUNDEF;
		
		double rIsoVal = m_rIsoStartVal;
		for (iCurrentIsoLine = 0; iCurrentIsoLine < iNrIsoLevels; ++iCurrentIsoLine)
		{
			// calc dbBufCurr with horizontal directions by comparing vertical differences of pixel rows
			// if upper pixel > lower pixel then direction is RIGHT
			for (j=-1; j <= iCols; ++j) {
				if ( i == iLines + 1 )
					rBufInp[j] = rUNDEF;   //  fill the extra bottom line with Undefs
				switch (iComparePixels(rBufInpPrev[j], rBufInp[j], rIsoVal))
				{
				case -1:
					dbBufCurr[j] = dbLEFT;
					break;
				case 0:
					dbBufCurr[j] = dbNONE;
					break;
				case 1:
					dbBufCurr[j] = dbRIGHT;
					break;
				}
			} 
			// calc dbBufNext with vertical directions by comparing horizontal differences of pixel columns
			// if left pixel > right pixel then direction is UP
			dbBufNext[-1] = dbNONE;
			for (j=0; j <= iCols; ++j) {
				switch (iComparePixels(rBufInp[j-1], rBufInp[j], rIsoVal))
				{
				case -1:
					dbBufNext[j] = dbDOWN;
					break;
				case 0:
					dbBufNext[j] = dbNONE;
					break;
				case 1:
					dbBufNext[j] = dbUP;
					break;
				}
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
				if (dbBufPrev[iCurrentIsoLine][j] != dbNONE)
					b |= 2;
				if (dbBufCurr[j-1] != dbNONE)
					b |= 4;
				if (dbBufNext[j] != dbNONE)
					b |= 8;
				switch (b) {
					case 0: {
							sbHoriz[iCurrentIsoLine][j] = 0;
							sbVert[iCurrentIsoLine][j] = 0;
						}
						break;
					case 7: case 11: case 13: case 14 : {
							NewNode(i, j, b, iCurrentIsoLine);
						}  
						break;
					case 15 : {
							if (!fEightCon)
								NewNode(i, j, b, iCurrentIsoLine);
							else {
								bool f1 = (0 == iComparePixels(rBufInpPrev[j], rBufInp[j-1], rIsoVal));
								bool f2 = (0 == iComparePixels(rBufInpPrev[j-1], rBufInp[j], rIsoVal));
								if (f1 && f2) {
									if (1 == iComparePixels(rBufInp[j-1], rBufInp[j], rIsoVal))
										f2 = false;
									else
										f1 = false;
								}  
								if (f1) {
									AppendLeftUp(i, j, iCurrentIsoLine); // b = 6
									//b = 9
									sbVert[iCurrentIsoLine][j] = sbHoriz[iCurrentIsoLine][j] = sbNewInBetween(j);
                
								}
								else if (f2) {
									AppendUp(i, j, 1/*3*/, iCurrentIsoLine); 
									SegBound* h = sbHoriz[iCurrentIsoLine][j];
									AppendLeft(i, j, 0/*12*/, iCurrentIsoLine);
									sbHoriz[iCurrentIsoLine][j] = h;
								}
								else
									NewNode(i, j, b, iCurrentIsoLine);
							}
							break;
						}
					case 3: case 10 : { // append to upper segment 
							AppendUp(i,j,b, iCurrentIsoLine);
						}
						break;    
					case 5: case 12 : { // append to left segment 
							AppendLeft(i,j,b, iCurrentIsoLine);
						}
						break;    
					case 6 : {//  append to left and up (become same segment) 
							AppendLeftUp(i,j, iCurrentIsoLine);
						}
						break;
					case 9 : {
							sbVert[iCurrentIsoLine][j] = sbHoriz[iCurrentIsoLine][j] = sbNewInBetween(j);
						}
						break;
					case 1: case 2: case 4: case 8 : // does not occur 
						break;
				}    
			}
	    // swap buffers to be able to use them in the next line
	    Swap(dbBufNext, dbBufPrev[iCurrentIsoLine]);
			rIsoVal += m_rIsoStepVal;
		}
    // swap buffers to be able to use them in the next line
    Swap(rBufInp, rBufInpPrev);
  }

	// store the tmpSegmentMap: add the segments in order of iso-line
	trq.SetText("Store Segments");
	double rIsoVal = m_rIsoStartVal;
	for (iCurrentIsoLine = 0; iCurrentIsoLine < iNrIsoLevels; ++iCurrentIsoLine)
	{
    if (trq.fUpdate(iCurrentIsoLine, iNrIsoLevels))
      break;

		for (list<CoordBuf>::iterator it = tmpSegmentMap[iCurrentIsoLine].begin(); it != tmpSegmentMap[iCurrentIsoLine].end(); ++it)
		{
			long iSize = it->iSize();
			StoreSegm((*it), iSize, rIsoVal);
		}

		rIsoVal += m_rIsoStepVal;
	}
	
	// release allocated memory

	// cleanup SegBound objects
	for (list<SegBound*>::iterator it = cleanupList.begin(); it != cleanupList.end(); ++it)
		delete (*it);

	// delete the rest
	delete [] dbBufPrev;
	delete [] sbHoriz;
	delete [] sbVert;
	delete [] tmpSegmentMap;
	dbBufPrev = 0;
	sbHoriz = 0;
	sbVert = 0;
	tmpSegmentMap = 0;

  return true;
}
/*
void SegmentMapFromRasValueBnd::DetLink(DirBound db1, DirBound db2, DirBound db3)
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
void SegmentMapFromRasValueBnd::NewNode(long iLine, long iCol, byte b, int iCurrentIsoNr)
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
    sbHoriz[iCurrentIsoNr][iCol] = sbSeg[dbRIGHT];
  }
  else
    sbHoriz[iCurrentIsoNr][iCol] = 0;
  if (fSegExist[dbUP]) { // end of segment up 
    sbSeg[dbUP] = sbVert[iCurrentIsoNr][iCol];
    EndOfSegment(iLine, iCol, *sbSeg[dbUP], true, (bool &)fBeginSeg[dbUP], iCurrentIsoNr);
  }
  if (fSegExist[dbLEFT]) { //end of segment to the left 
    sbSeg[dbLEFT] = sbHoriz[iCurrentIsoNr][iCol - 1];
    EndOfSegment(iLine, iCol, *sbSeg[dbLEFT], false, (bool &)fBeginSeg[dbLEFT], iCurrentIsoNr);
  }
  if (fSegExist[dbDOWN]) { // new segment down }
    sbSeg[dbDOWN] = sbNewWithOneEnd(iLine, iCol, false, (bool &)fBeginSeg[dbDOWN]);
    sbVert[iCurrentIsoNr][iCol] = sbSeg[dbDOWN];
  }
  else
    sbVert[iCurrentIsoNr][iCol] = 0;

//  DetLink(RIGHT, DOWN, LEFT);
//  DetLink(UP, RIGHT, DOWN);
//  DetLink(LEFT, UP, RIGHT);
//  DetLink(DOWN, LEFT, UP);

  if (fSegExist[dbUP]) // end of segment up 
    StoreSegm(*sbSeg[dbUP], iCurrentIsoNr);
  if (fSegExist[dbLEFT]) // end of segment to the left 
    StoreSegm(*sbSeg[dbLEFT], iCurrentIsoNr);
}

void SegmentMapFromRasValueBnd::AppendLeftUp(long iLine, long iCol, int iCurrentIsoNr)
/* Appends codes of point (iLine, iCol) to segment to the left of point. 
   The segment above the point is chained to the left segment.   
   Is called if type of point = 6 (line from left to up or line  
   from up to left.                                              */
{
  enum WhatDel { DELNONE, DELLEFT, DELUP } del;
  SegBound* sbUp = sbVert[iCurrentIsoNr][iCol];
  SegBound* sbLeft = sbHoriz[iCurrentIsoNr][iCol - 1];
  if (sbLeft != sbUp) {
    del = DELNONE;
    if (dbBufCurr[iCol - 1]==dbLEFT) { //line comes from above, goes left }
      if (sbUp->fBeginSeg) { 
        sbUp->dlChain.append(sbLeft->dlChain);
        del = DELLEFT;
        sbUp->fEndSeg = sbLeft->fEndSeg;
        sbUp->crdTo = sbLeft->crdTo;
        StoreSegm(*sbUp, iCurrentIsoNr);
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
          StoreSegm(*sbUp, iCurrentIsoNr);
        }
        else {
          sbLeft->dlChain.append(sbUp->dlChain);
          del = DELUP;
        }
    }
    // adjust sbHoriz and sbVert by replacing the deleted SegBound with the other one
    if (del == DELLEFT) {
      for (long i = sbHoriz[iCurrentIsoNr].iLower(); i <= sbHoriz[iCurrentIsoNr].iUpper(); i++) {
        if (sbHoriz[iCurrentIsoNr][i] == sbLeft)
          sbHoriz[iCurrentIsoNr][i] =  sbUp;
        if (sbVert[iCurrentIsoNr][i] == sbLeft)
          sbVert[iCurrentIsoNr][i] = sbUp;
      }
    }
    else if (del == DELUP) {
      for (long i = sbHoriz[iCurrentIsoNr].iLower(); i <= sbHoriz[iCurrentIsoNr].iUpper(); i++) {
        if (sbHoriz[iCurrentIsoNr][i] == sbUp)
          sbHoriz[iCurrentIsoNr][i] =  sbLeft;
        if (sbVert[iCurrentIsoNr][i] == sbUp)
          sbVert[iCurrentIsoNr][i] = sbLeft;
      }
    }
    else {
      StoreSegm(*sbLeft, iCurrentIsoNr);
      StoreSegm(*sbUp, iCurrentIsoNr);
    }
  }
  else { // segment is contour of island 
    // create new node; start and end of segment 
    // modify segment 
    sbUp->fBeginSeg = true;
    sbUp->crdFrom.x = iCol; sbUp->crdFrom.y = iLine;
    sbUp->fEndSeg = true;
    sbUp->crdTo.x = iCol; sbUp->crdTo.y = iLine;
    StoreSegm(*sbUp, iCurrentIsoNr);
  }
  sbHoriz[iCurrentIsoNr][iCol] = 0;
  sbVert[iCurrentIsoNr][iCol] = 0;
}

void SegmentMapFromRasValueBnd::AppendUp(long iLine, long iCol, byte b, int iCurrentIsoNr)
// Append chain codes of (iLine,iRow) to segment above point
{
  SegBound* sb = sbVert[iCurrentIsoNr][iCol];
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
    sbHoriz[iCurrentIsoNr][iCol] = 0;
    sbVert[iCurrentIsoNr][iCol] = sb;
 }
 else {
    sbVert[iCurrentIsoNr][iCol] = 0;
    sbHoriz[iCurrentIsoNr][iCol] = sb;
 }
 StoreSegm(*sb, iCurrentIsoNr);
}

void SegmentMapFromRasValueBnd::AppendLeft(long iLine, long iCol, byte b, int iCurrentIsoNr)
// Append chain code from (iLine, iCol) to segment to the left.
{
  SegBound* sb = sbHoriz[iCurrentIsoNr][iCol - 1];
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
    sbVert[iCurrentIsoNr][iCol] = 0;
    sbHoriz[iCurrentIsoNr][iCol] = sb;
 }
 else {
    sbHoriz[iCurrentIsoNr][iCol] = 0;
    sbVert[iCurrentIsoNr][iCol] = sb;
 }
 StoreSegm(*sb, iCurrentIsoNr);
}

SegBound* SegmentMapFromRasValueBnd::sbNewInBetween(long iCol)
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
    sb->rLeftVal = rBufInp[iCol];
    sb->rRightVal = rBufInpPrev[iCol];
  }
  else { //up, right 
    crBeg.dbCode = dbUP;
    crEnd.dbCode = dbRIGHT;
//    Top.PolL := last_line^[x]; Top.PolR := line^[x];
    sb->rLeftVal = rBufInpPrev[iCol];
    sb->rRightVal = rBufInp[iCol];
  }
  sb->dlChain.insert(crBeg); // insert at begin
  sb->dlChain.append(crEnd); // append to end
  sb->fBeginSeg = sb->fEndSeg = false; 
  sb->crdFrom.x = sb->crdFrom.y = sb->crdTo.x = sb->crdTo.y = 0;
  return sb;
}

SegBound* SegmentMapFromRasValueBnd::sbNewWithOneEnd(long iLine, long iCol, bool fRight, bool& fBegin)
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
           sb->rLeftVal = rBufInpPrev[iCol];
           sb->rRightVal = rBufInp[iCol];
         }
         break;
    case dbUP : {
//              top.PolL := line^[x - 1]; top.PolR := line^[x];
           sb->rLeftVal = rBufInp[iCol-1];
           sb->rRightVal = rBufInp[iCol];
         }
         break;
    case dbLEFT : {
//              top.PolL := line^[x]; top.PolR := last_line^[x];
           sb->rLeftVal = rBufInp[iCol];
           sb->rRightVal = rBufInpPrev[iCol];
         }
         break;
    case dbDOWN : {
//                top.PolL := line^[x]; top.PolR := line^[x - 1];
           sb->rLeftVal = rBufInp[iCol];
           sb->rRightVal = rBufInp[iCol-1];
         }
         break;
    default : assert(0==1);
      break;
  }
  return sb;
}

void SegmentMapFromRasValueBnd::EndOfSegment(long iLine, long iCol, SegBound& sb, bool fUp, bool& fBegin, int iCurrentIsoNr)
{
  if ((fUp && (dbBufPrev[iCurrentIsoNr][iCol] == dbUP)) ||
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

void SegmentMapFromRasValueBnd::StoreSegm(const SegBound& sb)
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


void SegmentMapFromRasValueBnd::StoreSegm(const SegBound& sb, int& iCurrentIsoNr)
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
    for (unsigned int i=0; i < acr.iSize(); ++i) {
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

	tmpSegmentMap[iCurrentIsoNr].push_back(cBuf2);
}


void SegmentMapFromRasValueBnd::StoreSegm(CoordBuf& cBuf, long& iCrd, double& rVal)
{
  if (iCrd == 0)
    return;
  for ( long j=0; j < iCrd; ++j)
    map->gr()->RowCol2Coord(cBuf[j].y, cBuf[j].x, cBuf[j]);

  ILWIS::Segment *seg = CSEGMENT(pms->newFeature());
  seg->PutCoords(iCrd, cBuf);
	seg->PutVal(rVal);
  iCrd = 0;
}



