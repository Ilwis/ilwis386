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
// $Log: /ILWIS 3.0/RasterApplication/Mpsegdns.cpp $
 * 
 * 5     21/02/01 14:06 Willem
 * CalcLineLength() now uses an improved calculation to get a new pixel to
 * examine when a line part that is on or very close to a corner of a
 * pixel is encountered.
 * 
 * 4     10-01-00 4:09p Martin
 * 
 * 3     9/08/99 11:52a Wind
 * comment problem
 * 
 * 2     9/08/99 8:58a Wind
 * changed sName() to sNameQuoted() in sExpression() to support long file
 * names
*/
// Revision 1.5  1998/09/16 17:24:40  Wim
// 22beta2
//
// Revision 1.4  1997/09/10 15:47:42  Wim
// intersection now takes absolutes before deciding which routine to use.
//
// Revision 1.3  1997-08-26 17:39:36+02  Wim
// Use always a RealPatch in the calculation
//
// Revision 1.2  1997-08-04 17:50:27+02  Wim
// Improved use of masks
//
/* MapSegmentDensity
   Copyright Ilwis System Development ITC
   august 1996, by Jelle Wind, Dick Visser
	Last change:  WK   10 Sep 97    5:44 pm
*/

#include "Engine\Domain\dm.h"
#include "Engine\Domain\Dmvalue.h"
#include "Engine\Base\DataObjects\valrange.h"
#include "Applications\Raster\MPSEGDNS.H"
#include "Headers\Hs\map.hs"

#include "Headers\Htp\Ilwisapp.htp"
#include "Headers\Err\Ilwisapp.err"

IlwisObjectPtr * createMapSegmentDensity(const FileName& fn, IlwisObjectPtr& ptr, const String& sExpr, vector<void *> parms ) {
	if ( sExpr != "")
		return (IlwisObjectPtr *)MapSegmentDensity::create(fn, (MapPtr &)ptr, sExpr);
	else
		return (IlwisObjectPtr *)new MapSegmentDensity(fn, (MapPtr &)ptr);
}


const char* MapSegmentDensity::sSyntax() {
  return "MapSegmentDensity(segmap,georef)/nMapSegmentDensity(segmap,mask,georef)";
}

MapSegmentDensity* MapSegmentDensity::create(const FileName& fn, MapPtr& p, const String& sExpr)
{
  Array<String> as;
  int iParms = IlwisObjectPtr::iParseParm(sExpr, as);
  if ((iParms < 1) || (iParms > 3))
    ExpressionError(sExpr, sSyntax());
  SegmentMap sm(as[0], fn.sPath());
  String sMask("*");
  if (iParms == 3)
    sMask = as[1];
  GeoRef gr(as[as.iSize()-1], fn.sPath());
  return new MapSegmentDensity(fn, p, sm, sMask, gr);
}


MapSegmentDensity::MapSegmentDensity(const FileName& fn, MapPtr& p)
: MapFromSegmentMap(fn, p)
{
  fNeedFreeze = true;
  sFreezeTitle = "MapSegmentDensity";
  htpFreeze = htpMapSegmentDensityT;
  String sMask;
  ReadElement("SegmentMapDensity", "Mask", sMask);
  mask.SetMask(sMask);
  objdep.Add(gr().ptr());
}

MapSegmentDensity::MapSegmentDensity(const FileName& fn, MapPtr& p,
           const SegmentMap& sm, const String& sMsk, const GeoRef& gr)
: MapFromSegmentMap(fn, p, sm, gr, mfLine)
{
  SetDomainValueRangeStruct(DomainValueRangeStruct(Domain("value")));
  if (gr->fGeoRefNone())
    throw ErrorGeoRefNone(gr->fnObj, errMapSegmentDensity);
  if (!cs()->fConvertFrom(sm->cs()))
    IncompatibleCoordSystemsError(cs()->sName(true, fnObj.sPath()), sm->cs()->sName(true, fnObj.sPath()), sTypeName(), errMapSegmentDensity+1);
  fNeedFreeze = true;
  sFreezeTitle = "MapSegmentDensity";
  htpFreeze = htpMapSegmentDensityT;
  mask.SetMask(sMsk);
  objdep.Add(gr.ptr());
  if (!fnObj.fValid())
    objtime = objdep.tmNewest();
}

void MapSegmentDensity::Store()
{
  MapFromSegmentMap::Store();
  WriteElement("MapFromSegmentMap", "Type", "MapSegmentDensity");
  WriteElement("SegmentMapDensity", "Mask", mask.sMask());
}

MapSegmentDensity::~MapSegmentDensity()
{
}

String MapSegmentDensity::sExpression() const
{
  return String("MapSegmentDensity(%S,\"%S\",%S)", sm->sNameQuoted(true, fnObj.sPath()), mask.sMask(), gr()->sNameQuoted(true, fnObj.sPath()));
}

bool MapSegmentDensity::fFreezing()
{
  mask.SetDomain(sm->dm());
// temporary fix to create a patch map with a temp name
// needed to be able to unpatch to the real name
  mapTmp = Map(FileName::fnUnique(fnObj), gr(), rcSize(), dvrs(), mfPatch);
  mapTmp->fErase = true;
// end fix code
  bool fTransformCoords = cs() != sm->cs();

  if (!fInitFill(mapTmp)) return false;
//msTmp = mapTmp->pms();


  CoordinateSequence *cb;
  long iNumC, iX, iY;
  long iNrLines = iLines();
  long iNrCols  = iCols();
  Coord cFrom, cTo;
  RowCol rcTopLeft, rcFrom, rcTo;
  bool fPatchChanged;
  trq.SetText(SMAPTextCalculating);
// 26/8/97 Wim: I do not see any reason for more than one calculation type,
// always work with real patches.
  RealPatch ptCur;
  long iNrPatch = 0;
  long iTotPatch = ((iNrLines + PATCH_SIDE) / PATCH_SIDE) * ((iNrCols + PATCH_SIDE) / PATCH_SIDE);
  for (iY=0; iY < iNrLines; iY+=PATCH_SIDE) {
    for (iX=0; iX < iNrCols; iX+=PATCH_SIDE) {
      mapTmp->GetPatchVal(RowCol(iY+1, iX+1), ptCur);
      fPatchChanged = false;
      CalcPatchBounds(iX, iY);
      if (trq.fUpdate(iNrPatch, iTotPatch))
        return false;
	  for (int k=0; k < sm->iFeatures(); ++k) {
		  ILWIS::Segment *segCur = CSEGMENT( sm->getFeature(k));
		  if ( !segCur || segCur->fValid()==false)
			  continue;
        if (trq.fAborted())
          return false;
        if (!segCur->fInMask(sm->dvrs(),mask))
          continue;
        if (fInPatch(segCur)) {
          cb = segCur->getCoordinates();
		  iNumC = (long)cb->size();
          rcTopLeft = mm.rcMin; // topleft of patch
          if (iNumC>0) {
            cFrom = cb->getAt(0);               // to world coordinate
            if (fTransformCoords)
              cFrom = cs()->cConv(sm->cs(), cFrom);
            rcFrom = mapTmp->gr()->rcConv(cFrom);
            for (long i=1; i<iNumC; i++) {
				cTo = cb->getAt(i);               // to world coordinate
              if (fTransformCoords)
                cTo = cs()->cConv(sm->cs(), cTo);
              rcTo = mapTmp->gr()->rcConv(cTo);
              CalcLineLength(ptCur, cFrom, cTo, rcFrom, rcTo, rcTopLeft);
              cFrom = cTo;
              rcFrom = rcTo;
              fPatchChanged = true;
            }
          }
		  delete cb;
        }
      }
      if (fPatchChanged)
        mapTmp->PutPatchVal(RowCol(iY+1, iX+1), ptCur);
      iNrPatch++;
    }
  }

// start fix to unpatch mp

  bool fStop;
  switch (st()) {
    case stREAL : {
      fStop = pms->fUnPatchReal(mapTmp.ptr(), sFreezeTitle, trq);
      break;
    }
    case stLONG : {
      fStop = pms->fUnPatchLong(mapTmp.ptr(), sFreezeTitle, trq);
      break;
    }
    case stINT : {
      fStop = pms->fUnPatchInt(mapTmp.ptr(), sFreezeTitle, trq);
      break;
    }
    case stBYTE :
    case stDUET :
    case stNIBBLE :
    case stBIT : {
      fStop = pms->fUnPatchByte(mapTmp.ptr(), sFreezeTitle, trq);
      break;
    }
  } // switch()
  mapTmp = Map();
  return fStop;
}

void MapSegmentDensity::CalcPatchBounds(long iX, long iY)
{
  mm = MinMax(RowCol(iY, iX),
              RowCol((iY+PATCH_SIDE-1) < iLines() ? iY+PATCH_SIDE-1 : iLines(),
                     (iX+PATCH_SIDE-1)  < iCols()  ? iX+PATCH_SIDE-1  : iCols()) );
  pms->gr()->RowCol2Coord(iY, iX, cTopLeft);
  pms->gr()->RowCol2Coord(1.0 + mm.rcMax.Row, 1.0 + mm.rcMax.Col, cBotRight);
  pms->gr()->RowCol2Coord(iY, 1.0 + mm.rcMax.Col, cTopRight);
  pms->gr()->RowCol2Coord(1.0 + mm.rcMax.Row, iX, cBotLeft);
}

bool MapSegmentDensity::fInPatch(const ILWIS::Segment* seg) {
  CoordBounds cb = seg->crdBounds();
  Coord cLow = cb.cMin, cHigh = cb.cMax;

  if (min(cLow.x, cHigh.x) > max(cTopLeft.x, cBotRight.x))
    return false;
  if (max(cLow.x, cHigh.x) < min(cTopLeft.x, cBotRight.x))
    return false;
  if (min(cLow.y, cHigh.y) > max(cTopLeft.y, cBotRight.y))
    return false;
  if (max(cLow.y, cHigh.y) < min(cTopLeft.y, cBotRight.y))
    return false;
  return true;
}

bool MapSegmentDensity::fLineInPatch(const Coord& cFrom, const Coord& cTo)
{

  if (min(cFrom.x, cTo.x) > max(cTopLeft.x, cBotRight.x))
    return false;
  if (max(cFrom.x, cTo.x) < min(cTopLeft.x, cBotRight.x))
    return false;
  if (min(cFrom.y, cTo.y) > max(cTopLeft.y, cBotRight.y))
    return false;
  if (max(cFrom.y, cTo.y) < min(cTopLeft.y, cBotRight.y))
    return false;
  return true;
}

bool MapSegmentDensity::fLineInPatch(const RowCol& rcF, const RowCol& rcT)
{
  if (rcF.Row < 0 && rcT.Row < 0)
    return false;
  if (rcF.Row >= PATCH_SIDE && rcT.Row >= PATCH_SIDE)
    return false;
  if (rcF.Col < 0 && rcT.Col < 0)
    return false;
  if (rcF.Col >= PATCH_SIDE  && rcT.Col >= PATCH_SIDE)
    return false;
  return true;
}

bool MapSegmentDensity::fInPatch(const RowCol& rc)
{
  if (rc.Row < 0 || rc.Row >= PATCH_SIDE)
    return false;
  if (rc.Col < 0 || rc.Col >= PATCH_SIDE)
    return false;
  return true;
}


/****************************************************************************/
/* Compute intersection of 2 line sections (line pieces !!)                 */
/* inputs are 2 pair of Coord's                                             */
/* no check is done whether the two pairs coincide                          */
/* function returns true if there is an intersection and                    */
/* in that case the coord of intersection                                   */
/* if an intersection is found the coord of a point somewhat further        */
/*   on the line is given too.                                              */
/****************************************************************************/
bool MapSegmentDensity::intersection(Coord co0, Coord co1, Coord cob, Coord coe,
                                     Coord& cr, double& rBeta, double& rMu,
                                     Coord& crPlusD)
{
	double rBeta_t , rBeta_n , rMu_t , rMu_n;
	if (abs(coe.x-cob.x) > abs(coe.y-cob.y))
	{
		rBeta_t = (-co0.y + cob.y) * (coe.x - cob.x) + (co0.x - cob.x) * (coe.y - cob.y);
		rBeta_n = (co1.y - co0.y) * (coe.x - cob.x)  - (co1.x - co0.x) * (coe.y - cob.y);
		if (fabs(rBeta_n * 10.0) <= fabs(rBeta_t))
			return false; // avoid dividing by zero
		
		rBeta = rBeta_t / rBeta_n;
		if (rBeta < (-0.0000001)) return false;
		if (rBeta > ( 1.0000001)) return false;
		
		rMu_t = co0.x + rBeta * (co1.x - co0.x) - cob.x;
		rMu_n = coe.x - cob.x;
		if (fabs(rMu_n * 10.0) <= fabs(rMu_t))
			return false; // avoid dividing by zero

		rMu = rMu_t / rMu_n;
		if (rMu < (-0.0000001)) return false;
		if (rMu > ( 1.0000001)) return false;
	}
	else
	{
		rBeta_t = (-co0.x + cob.x) * (coe.y - cob.y) + (co0.y - cob.y) * (coe.x - cob.x);
		rBeta_n = (co1.x - co0.x) * (coe.y - cob.y)  - (co1.y - co0.y) * (coe.x - cob.x);
		if (fabs(rBeta_n * 10.0) <= fabs(rBeta_t))
			return false; // avoid dividing by zero

		rBeta = rBeta_t / rBeta_n;
		if (rBeta < (-0.0000001)) return false;
		if (rBeta > ( 1.0000001)) return false;
		
		rMu_t = co0.y + rBeta * (co1.y - co0.y) - cob.y;
		rMu_n = coe.y - cob.y;
		if (fabs(rMu_n * 10.0) <= fabs(rMu_t))
			return false; // avoid dividing by zero

		rMu = rMu_t / rMu_n;
		if (rMu < (-0.0000001)) return false;
		if (rMu > ( 1.0000001)) return false;
	}
	cr.x = co0.x + rBeta * (co1.x - co0.x);
	cr.y = co0.y + rBeta * (co1.y - co0.y);

	return true;
}


enum BoundCode { bcLEFT=1, bcTOP, bcRIGHT, bcBOTTOM };

void MapSegmentDensity::CalcLineLength(RealPatch& ptCur, const Coord& cFrom,
                                       const Coord& cTo, const RowCol& rcFrom,
                                       const RowCol& rcTo, const RowCol& rcTopLeft)
{
	if (!fLineInPatch(cFrom, cTo))
		return;
	
	RowCol rcFromInPatch;
	rcFromInPatch.Row = rcFrom.Row - rcTopLeft.Row;
	rcFromInPatch.Col = rcFrom.Col - rcTopLeft.Col;
	RowCol rcToInPatch;
	rcToInPatch.Row = rcTo.Row - rcTopLeft.Row;
	rcToInPatch.Col = rcTo.Col - rcTopLeft.Col;
	RowCol rcActInPatch;
	rcActInPatch = rcFromInPatch;
	// first look if From and To are in same RowCol THEN they are in the patch !
	if ( rcFrom == rcTo)
	{
		if (fInPatch(rcActInPatch))
			ptCur(rcActInPatch.Row, rcActInPatch.Col) += rDist(cFrom,cTo);
		return;
	}
	RowCol rcCurr = rcFrom;
	Coord cStart = cFrom;
	bool fRcCurrInPatch = fInPatch(rcActInPatch);
	
	// improvement needed: if starting point lies outside patch still all 
	// kind of intersections are calculated.
	for(;;)
	{
		if ( rcCurr == rcTo )
		{
			if (fInPatch(rcActInPatch))
				ptCur(rcActInPatch.Row, rcActInPatch.Col) += rDist(cStart,cTo);
			return;
		}
		
		// get coord of fCorners of Start rc
		Coord cTL, cBL, cTR, cBR;
		pms->gr()->RowCol2Coord(rcCurr.Row  , rcCurr.Col  , cTL );
		pms->gr()->RowCol2Coord(rcCurr.Row+1, rcCurr.Col  , cBL );
		pms->gr()->RowCol2Coord(rcCurr.Row  , rcCurr.Col+1, cTR );
		pms->gr()->RowCol2Coord(rcCurr.Row+1, rcCurr.Col+1, cBR );
		// determine cross points with border of this pixel
		Coord cIntersect[2], cPlus[2];
		bool fHitLeft, fHitTop, fHitRight, fHitBottom;
		fHitLeft = fHitTop = fHitRight = fHitBottom = true;
		double arBeta[2], arMu[2];
		BoundCode bcHit[2];
		bool fCorner = false;
		int iCC = 0;
		int iLess = 1,iMax = 0;
		Coord co0;
		fHitLeft = intersection(cTL, cBL, cStart, cTo, cIntersect[iCC],
			arBeta[iCC], arMu[iCC], cPlus[iCC] );
		if (fHitLeft)
		{
			co0 = cTL;
			if(arBeta[iCC] > 0.99999)
			{
				fHitBottom = false;
				fCorner = true;
			}
			if(arBeta[iCC] < 0.00001)
			{
				fHitTop = false;
				fCorner = true;
			}
			bcHit[iCC] = bcLEFT;
			iCC++;
		}
		
		if (fHitTop)
			fHitTop = intersection(cTL, cTR, cStart, cTo, cIntersect[iCC],
			arBeta[iCC], arMu[iCC], cPlus[iCC]);
		if (fHitTop)
		{
			co0 = cTL;
			if(arBeta[iCC] > 0.99999)
			{
				fHitRight = false;
				fCorner = true;
			}
			bcHit[iCC] = bcTOP;
			iCC++;
			if (iCC == 2)
				fHitRight = fHitBottom = false;
		}
		if (fHitRight)
			fHitRight = intersection(cBR, cTR, cStart, cTo, cIntersect[iCC],
			arBeta[iCC], arMu[iCC], cPlus[iCC]);
		if (fHitRight)
		{
			co0 = cBR;
			if(arBeta[iCC] < 0.00001)
			{
				fHitBottom = false;
				fCorner = true;
			}
			bcHit[iCC] = bcRIGHT;
			iCC++;
			if (iCC == 2)
				fHitBottom = false;
			
		}
		if (fHitBottom)
		{
			fHitBottom = intersection(cBR, cBL, cStart, cTo, cIntersect[iCC],
				arBeta[iCC], arMu[iCC], cPlus[iCC]);
			if (fHitBottom)
			{
				co0 = cBR;
				bcHit[iCC] = bcBOTTOM;
				iCC++;
			}
		}

		// check if one point on the border
		if(iCC == 0 )
			return;
		
		if (iCC == 1)
		{
			if (arMu[0] > 0.00001)
				if (fInPatch(rcActInPatch))
					ptCur(rcActInPatch.Row, rcActInPatch.Col) += rDist(cStart,cIntersect[0]);
				cStart = cIntersect[0];
		}
		else
		{
			if (arMu[0] < arMu[1])
			{
				iLess = 0;
				iMax  = 1;
			}
			if (fInPatch(rcActInPatch))
				ptCur(rcActInPatch.Row, rcActInPatch.Col) += rDist(cIntersect[0],cIntersect[1]);
			cStart = cIntersect[iMax];
		}
		
		// determine end of loop
		if (arMu[iMax] > 0.99999)
			return;
		
		if (!fCorner)
		{
			if (bcHit[iMax]==bcLEFT) rcCurr.Col--;
			if (bcHit[iMax]==bcTOP) rcCurr.Row--;
			if (bcHit[iMax]==bcRIGHT) rcCurr.Col++;
			if (bcHit[iMax]==bcBOTTOM) rcCurr.Row++;
		}
		else
		{
			// We are (close) at a corner of a pixel. Calculate point somewhat further on 
			// the segment line part. Make sure the point does end up in another pixel
			// than the current!
			// co0 is the coordinate of the corner. rMu has been calculated in the intersection()
			// function. rDelta is a choosen value to slightly increase/decrease the X,Y values
			rcCurr = mapTmp->gr()->rcConv(co0);
			RowCol rcTry;
			Coord crPlusD;
			double rMu = arMu[iMax];
			double rDelta = 0.00001;
			do
			{
				crPlusD.x = cStart.x + (rMu + rDelta * (1.0 + rMu)) * (cTo.x - cStart.x);
				crPlusD.y = cTo.y + (rMu + rDelta * (1.0 + rMu)) * (cTo.y - cStart.y);
				rcTry = mapTmp->gr()->rcConv(crPlusD);
				rDelta += 0.00001;
			}
			while (rcCurr == rcTry);
			rcCurr = rcTry;
		}
		
		rcActInPatch.Row = rcCurr.Row - rcTopLeft.Row;
		rcActInPatch.Col = rcCurr.Col - rcTopLeft.Col;
		if (fRcCurrInPatch)
			if (!(fInPatch(rcActInPatch)))
				return;
  }
}



bool MapSegmentDensity::fInitFill(Map& mp)
{
//MapStore* ms = mp->pms();
  long iCol  = iCols();
  long iLine = iLines();

  long iNrPatch = 0;
  long iTotPatch = ((iLine + PATCH_SIDE) / PATCH_SIDE) * ((iCol + PATCH_SIDE) / PATCH_SIDE);
  trq.SetText(SMAPTextInitializeMap);

  switch (st()) {
    case stBIT :
    case stDUET :
    case stNIBBLE :
    case stBYTE: {
      BytePatch ptCur;
      for (int l =0 ; l < PATCH_SIDE; l++)
       for (int c = 0; c < PATCH_SIDE; c++)
          ptCur(c,l) = 0;
      for (long iY=0; iY < iLine; iY+=PATCH_SIDE)
        for (long iX=0; iX < iCol; iX+=PATCH_SIDE) {
          if (trq.fUpdate(iNrPatch, iTotPatch))
            return false;
          mp->PutPatchRaw(RowCol(iY, iX), ptCur);
          iNrPatch++;
        }
      break;
    }
    case stINT : {
      IntPatch ptCur;
      for (int l =0 ; l < PATCH_SIDE; l++)
       for (int c = 0; c < PATCH_SIDE; c++)
          ptCur(c,l) = 0;
      for (long iY=0; iY < iLine; iY+=PATCH_SIDE)
        for (long iX=0; iX < iCol; iX+=PATCH_SIDE) {
          if (trq.fUpdate(iNrPatch, iTotPatch))
            return false;
          mp->PutPatchRaw(RowCol(iY, iX), ptCur);
          iNrPatch++;
        }
      break;
    }
    case stLONG : {
      LongPatch ptCur;
      for (int l =0 ; l < PATCH_SIDE; l++)
       for (int c = 0; c < PATCH_SIDE; c++)
          ptCur(c,l) = 0;
      for (long iY=0; iY < iLine; iY+=PATCH_SIDE)
        for (long iX=0; iX < iCol; iX+=PATCH_SIDE) {
          if (trq.fUpdate(iNrPatch, iTotPatch))
            return false;
          mp->PutPatchRaw(RowCol(iY, iX), ptCur);
          iNrPatch++;
        }
      break;
    }
    case stREAL : {
      RealPatch ptCur;
      for (int l =0 ; l < PATCH_SIDE; l++)
        for (int c = 0; c < PATCH_SIDE; c++)
          ptCur(c,l) = 0;
      for (long iY=0; iY < iLine; iY+=PATCH_SIDE)
        for (long iX=0; iX < iCol; iX+=PATCH_SIDE) {
          if (trq.fUpdate(iNrPatch, iTotPatch))
            return false;
          mp->PutPatchVal(RowCol(iY, iX), ptCur);
          iNrPatch++;
        }
      break;
    }
  }  // switch()
  trq.fUpdate(iTotPatch, iTotPatch);
  return true;
}




