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
// $Log: /ILWIS 3.0/SegmentMap/Segcheck.cpp $
 * 
 * 7     8/15/00 5:35p Wind
 * avoid endless loop when slef checking
 * 
 * 6     17-01-00 8:17a Martin
 * changed rowcols to coords
 * 
 * 5     10-12-99 13:07 Wind
 * made internal arrays zero based
 * 
 * 4     10-12-99 11:48a Martin
 * removed internal rowcols and replaced them by true coords
 * 
 * 3     9-09-99 10:27a Martin
 * added 2.22 stuff
 * 
 * 2     9/08/99 12:02p Wind
 * comments
*/
// Revision 1.3  1998/09/16 17:25:20  Wim
// 22beta2
//
// Revision 1.2  1997/08/04 15:36:21  Wim
// Improved use of mask. Use now Segment::fInMask(mask)
//
/* SegmentMapPtr Check functions
   Copyright Ilwis System Development ITC
   march 1995, by Wim Koolhoven
	Last change:  WK   21 Apr 98    4:39 pm
*/

//#include "Engine\Base\DataObjects\Tranq.h"
#include "Engine\Map\Segment\Seg.h"
#include "Engine\Base\mask.h"
#include "Headers\Hs\segment.hs"

static void FillMM(SegmentMapPtr* sm,
                   ArrayLarge<MinMax>& amm,
                   bool fMask, const Mask& mask);

static void FillCRDFL(const SegmentMapPtr* sm,
                   ArrayLarge<Coord>& crdF, ArrayLarge<Coord>& crdL,
                   bool fMask, const Mask& mask)
{
	int iSeg = sm->iFeatures();
	crdF.Resize(iSeg+1); // now zero based
	crdL.Resize(iSeg+1); // noe zero based
	for (int i = 0; i < iSeg; ++i) {
		ILWIS::Segment *s = (ILWIS::Segment *)sm->getFeature(i);
		if ( s == NULL || s->fValid() == false)
			continue;
		if (fMask && !s->fInMask(sm->dvrs(), mask))
			continue;
		crdF[i] = s->crdBegin();
		crdL[i] = s->crdEnd();
	}
}

static void FillCB(SegmentMapPtr* sm,
                   ArrayLarge<CoordBounds>& acb,
                   bool fMask, const Mask& mask)
{
	int iSeg = sm->iFeatures();
	acb.Resize(iSeg+1);
	for (int i = 0; i < iSeg; ++i) {
		ILWIS::Segment *s = (ILWIS::Segment *)sm->getFeature(i);
		if ( s == NULL || s->fValid() == false)
			continue;
		if (fMask && !s->fInMask(sm->dvrs(), mask))
			continue;
		acb[i] = s->crdBounds();
	}
}

bool SegmentMapPtr::fCheckForPolygonize(const String& sMask, bool fAutoCorrect, Tranquilizer& trq)
{
	bool fFoundError;
	ArrayLarge<Coord> crdF, crdL;
	ArrayLarge<CoordBounds> acb;
	Mask mask(dm(), sMask);
	bool fMask = sMask.length() > 0;

	trq.SetTitle(SSEGTitleCheckSegments);
	do {
		fFoundError = false;

		// Check Individual Self
		trq.SetText(SSEGTextSelfOverlapCheck);
		for (int i = 0; i < iFeatures(); ++i) {
			ILWIS::Segment *sg = (ILWIS::Segment *) (ILWIS::Segment *)getFeature(i);
			if ( sg == NULL || sg->fValid() == false)
				continue;
			if (trq.fUpdate(i, iFeatures()))
				return false;
			if (fMask && !sg->fInMask(dvrs(), mask))
				continue;
			long iFirst, iSecond;
			Coord acbt;
			while (!sg->fSelfCheck(iFirst, iSecond, acbt)) {
				if (!fAutoCorrect)
					return false;
				fFoundError = true;
				if (-1 == iFirst) { // self overlap -> delete segment
					sg->Delete();
					break; // added 15/8/00 to avoid endless loop
				}
				else { // self cross -> remove false polygon
					sg->segSplit(iSecond, acbt);
					ILWIS::Segment *s  = CSEGMENT(newFeature());
					sg->segSplit(iFirst, acbt,&s);
					delete s;
				}
			}
		}
		if (trq.fUpdate(iFeatures(), iFeatures()))
			return false;
		if (fFoundError)
			continue;

		// Check Connected
		trq.SetText(SSEGTextCheckDeadEnds);
		if (trq.fUpdate(0, iFeatures()))
			return false;
		FillCRDFL(this,crdF,crdL,fMask,mask);
		for (int i = 0; i < iFeatures(); ++i) {
			ILWIS::Segment *sg = (ILWIS::Segment *) getFeature(i);
			if ( sg == NULL || sg->fValid() == false)
				continue;
			if (trq.fUpdate(i, iFeatures()))
				return false;
			if (fMask && !sg->fInMask(dvrs(), mask))
				continue;
			bool fBegin = false;
			bool fEnd = false;
			Coord crdBegin = sg->crdBegin();
			Coord crdEnd = sg->crdEnd();
			double rAzimBegin = sg->rAzim(false);
			double rAzimEnd = sg->rAzim(true);
			if (crdBegin == crdEnd) {
				fBegin = fEnd = true;
				if (abs(rAzimBegin - rAzimEnd) < 1e-6) {
					// segment overlays it self -> delete
					if (!fAutoCorrect)
						return false;
					fFoundError = true;
					sg->Delete();
					//          FillRCFL(this,crdF,crdL,fMask,mask);
					crdF[i] = Coord();
					crdL[i] = Coord();
					continue;
				}
			}
			ILWIS::Segment *s;
			bool fError = false;
			for (int j = 0; j < iFeatures(); ++j) {
				if (trq.fAborted())
					return false;
				if (i == j)
					continue;
				if (crdF[j] == crdBegin) {
					fBegin = true;
					s = (ILWIS::Segment *)getFeature(j);
					if (abs(s->rAzim(false) - rAzimBegin) < 1e-6) {
						fError = true;
					}
				}
				if (crdL[j] == crdBegin) {
					fBegin = true;
					s = (ILWIS::Segment *)getFeature(j);
					if (abs(s->rAzim(true) - rAzimBegin) < 1e-6) {
						fError = true;
					}
				}
				if (!fError && crdBegin == crdEnd)
					continue;
				if (crdF[j] == crdEnd) {
					fEnd = true;
					s = (ILWIS::Segment *)getFeature(j);
					if (abs(s->rAzim(false) - rAzimEnd) < 1e-6) {
						fError = true;
					}
				}
				if (crdL[j] == crdEnd) {
					fEnd = true;
					s = (ILWIS::Segment *)getFeature(j);
					if (abs(s->rAzim(true) - rAzimEnd) < 1e-6) {
						fError = true;
					}
				}
				if (fError)
					break;
			}
			if (fError || !fBegin || !fEnd) {
				if (!fAutoCorrect)
					return false;
				fFoundError = true;
				sg->Delete();
				//        FillRCFL(this,crdF,crdL,fMask,mask);
				crdF[i] = Coord();
				crdL[i] = Coord();
			}
		}
		if (trq.fUpdate(iFeatures(), iFeatures()))
			return false;
		if (fFoundError)
			continue;

		// Check Intersections
		trq.SetText(SSEGTextCheckIntersections);
		if (trq.fUpdate(0, iFeatures()))
			return false;
		FillCB(this,acb,fMask,mask);
		for (int k = 0; k < iFeatures(); ++k) {
			ILWIS::Segment *sg = (ILWIS::Segment *) (ILWIS::Segment *) getFeature(k);
			if ( sg == NULL || sg->fValid() == false)
				continue;
			if (trq.fUpdate(k, iFeatures()))
				return false;
			if (fMask && !sg->fInMask(dvrs(),mask))
				continue;

			int iCurr = k;
			CoordBounds cbCurr = acb[iCurr];
			for (int j = 0; j < iFeatures(); ++j) {
				if (trq.fAborted())
					return false;
				if (j == k)
					continue;
				if (!cbCurr.fContains(acb[j]))
					continue;
				bool fOverlay;
				long iAft, iAft2;
				Coord acbt;
				ILWIS::Segment *s2 = (ILWIS::Segment *)getFeature(j);
				if (sg->fIntersects(fOverlay, iAft, acbt, s2, iAft2, trq)) {
					if (!fAutoCorrect)
						return false;
					fFoundError = true;
					if (fOverlay)
						sg->Delete();
					else {
						sg->segSplit(iAft, acbt);
						s2->segSplit(iAft2, acbt);
					}
					FillCB(this,acb,fMask,mask);
				}
			}
		}
		if (trq.fUpdate(iFeatures(), iFeatures()))
			return false;

	} while (fFoundError);
	return true;
}

void SegmentMapPtr::RemoveRedundantNodes(Tranquilizer& trq)
{
  trq.SetText(SSEGTextRemoveRedundantNodes);
  ArrayLarge<Coord> crdF, crdL;
  CoordinateSequence *buf1, *buf2;
  CoordBuf buf;
  Mask mask(dm(), "*");
  FillCRDFL(this,crdF,crdL,false,mask);
  for (int k = 0; k < iFeatures(); ++k) {
	  ILWIS::Segment *sg = (ILWIS::Segment *) getFeature(k);
	  if ( sg == NULL || sg->fValid() == false)
		  continue;
	  if (trq.fUpdate(k, iFeatures()))
		  return;
	  int iCurr =k;
	  bool fBeginNeeded = false;
	  bool fEndNeeded = false;
	  while (!fBeginNeeded || !fEndNeeded) {
		  Coord crdBegin = sg->crdBegin();
		  Coord crdEnd = sg->crdEnd();
		  crdF[iCurr] = crdBegin;
		  crdL[iCurr] = crdEnd;
		  if (crdBegin == crdEnd)
			  break;
		  int iFoundBegin = 0;
		  int iFoundEnd = 0;
		  for (int i = 0; i < iFeatures(); ++i) {
			  if (trq.fAborted())
				  return;
			  if (i == iCurr)
				  continue;
			  if (fBeginNeeded && fEndNeeded)
				  break;
			  if (!fBeginNeeded) {
				  if (crdF[i] == crdBegin) {
					  if (0 == iFoundBegin)
						  iFoundBegin = -i;
					  else {
						  fBeginNeeded = true;
					  }
				  }
				  if (crdL[i] == crdBegin) {
					  if (0 == iFoundBegin)
						  iFoundBegin = i;
					  else {
						  fBeginNeeded = true;
					  }
				  }
			  }
			  if (!fEndNeeded) {
				  if (crdF[i] == crdEnd) {
					  if (0 == iFoundEnd)
						  iFoundEnd = i;
					  else {
						  fEndNeeded = true;
					  }
				  }
				  if (crdL[i] == crdEnd) {
					  if (0 == iFoundEnd)
						  iFoundEnd = -i;
					  else {
						  fEndNeeded = true;
					  }
				  }
			  }
		  }
		  if (0 == iFoundBegin)
			  fBeginNeeded = true;
		  if (0 == iFoundEnd)
			  fEndNeeded = true;
		  if (!fBeginNeeded && 0 != iFoundBegin) {
			  long iNr, iNr1, iNr2;
			  buf1 = sg->getCoordinates();
			  iNr1 = buf1->size();
			  ILWIS::Segment *seg2 = (ILWIS::Segment *)getFeature(iFoundBegin);
			  buf2 = seg2->getCoordinates();
			  iNr2 = buf2->size();
			  iNr = iNr1 + iNr2 - 1;
			  if (iNr > 1000)
				  fBeginNeeded = true;
			  else {
				  int j, k = 0;
				  for (j = 0; j < iNr2; ++j)
					  buf[k++] = buf2->getAt(j);
				  for (j = 1; j < iNr1; ++j)
					  buf[k++] = buf1->getAt(j);
				  sg->PutCoords(iNr, buf);
				  seg2->Delete();
				  crdF[abs(iFoundEnd)] = Coord();
				  crdL[abs(iFoundEnd)] = Coord();
			  }
		  }
		  if (!fEndNeeded && 0 != iFoundEnd) {
			  long iNr, iNr1, iNr2;
			  buf1 = sg->getCoordinates();
			  iNr1 = buf1->size();
			  ILWIS::Segment *seg2 = (ILWIS::Segment *)getFeature(iFoundEnd);
			  buf2 = seg2->getCoordinates();
			  iNr2 = buf2->size();
			  iNr = iNr1 + iNr2 - 1;
			  if (iNr > 1000)
				  fEndNeeded = true;
			  else {
				  int j, k = 0;
				  for (j = 0; j < iNr1; ++j)
					  buf[k++] = buf1->getAt(j);
				  for (j = 1; j < iNr2; ++j)
					  buf[k++] = buf2->getAt(j);
				  sg->PutCoords(iNr, buf);
				  seg2->Delete();
				  crdF[abs(iFoundEnd)] = Coord();
				  crdL[abs(iFoundEnd)] = Coord();
			  }
		  }
	  }
  }
}





