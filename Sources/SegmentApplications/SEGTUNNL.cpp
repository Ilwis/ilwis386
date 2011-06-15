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
/* SegmentMapTunneling
   Copyright Ilwis System Development ITC
   jan 1996, Jan Hendrikse
    Last change:  JEL   6 May 97    6:03 pm
*/

#include "SegmentApplications\SEGTUNNL.H"
#include "Engine\Base\DataObjects\valrange.h"
#include "Engine\Domain\dmsort.h"
#include "Engine\Base\Algorithm\Tunnel.h"
#include "Headers\Err\Ilwisapp.err"
#include "Headers\Htp\Ilwisapp.htp"
#include "Headers\Hs\segment.hs"

IlwisObjectPtr * createSegmentMapTunneling(const FileName& fn, IlwisObjectPtr& ptr, const String& sExpr, vector<void *> parms ) {
	if ( sExpr != "")
		return (IlwisObjectPtr *)SegmentMapTunneling::create(fn, (SegmentMapPtr &)ptr, sExpr);
	else
		return (IlwisObjectPtr *)new SegmentMapTunneling(fn, (SegmentMapPtr &)ptr);
}

const char* SegmentMapTunneling::sSyntax() {
  return "SegmentMapTunneling(segmap,tunnelwidth,removenodes)";
}

SegmentMapTunneling* SegmentMapTunneling::create(const FileName& fn, SegmentMapPtr& p,
                                                 const String& sExpr)
{
  Array<String> as;
  int iParms = IlwisObjectPtr::iParseParm(sExpr, as);
  if ((iParms<1) || (iParms > 3))
    ExpressionError(sExpr, sSyntax());
	String sInputSegMapName = as[0];
	char *pCh = sInputSegMapName.strrchrQuoted('.');
  if ((pCh != 0) && (0 != _strcmpi(pCh, ".mps")))  // attrib map
		throw ErrorObject(WhatError(String(SSEGErrNoAttColumnAllowed_S.scVal(), as[0]),
																 errSegmentMapTunneling), fn);
  SegmentMap smp(as[0], fn.sPath());
	double rTunnelWidth = as[1].rVal();
  if (rTunnelWidth < 0)
    throw ErrorObject(WhatError(String(SSEGErrTunnelWidthNotPositive_S.scVal(), as[1]),
                                 errSegmentMapTunneling+1), fn);
  bool fRemoveNodes = false;
  if (iParms == 3)
    fRemoveNodes = as[2].fVal();
  return new SegmentMapTunneling(fn, p, smp, rTunnelWidth, fRemoveNodes);
}

SegmentMapTunneling::SegmentMapTunneling(const FileName& fn, SegmentMapPtr& p)
: SegmentMapVirtual(fn, p)
{
  fNeedFreeze = false;
  String sColName;
  ReadElement("SegmentMapTunneling", "SegmentMap", smp);
  ReadElement("SegmentMapTunneling", "RemoveNodes", fRemoveNodes);
  ReadElement("SegmentMapTunneling", "TunnelWidth", rTunnelWidth);
  Init();
  objdep.Add(smp.ptr());
}

SegmentMapTunneling::SegmentMapTunneling(const FileName& fn, SegmentMapPtr& p, const SegmentMap& sm,
                                         double rTunnelW, bool fRemNodes)
: SegmentMapVirtual(fn, p, sm->cs(),sm->cb(),sm->dvrs()), smp(sm),
  rTunnelWidth(rTunnelW), fRemoveNodes(fRemNodes)
{
  fNeedFreeze = true;
  Init();
  objdep.Add(smp.ptr());
  if (!fnObj.fValid())
    objtime = objdep.tmNewest();
  if (smp->fTblAttSelf())
    SetAttributeTable(smp->tblAtt());
}

void SegmentMapTunneling::Store()
{
  SegmentMapVirtual::Store();
  WriteElement("SegmentMapVirtual", "Type", "SegmentMapTunneling");
  WriteElement("SegmentMapTunneling", "SegmentMap", smp);
  WriteElement("SegmentMapTunneling", "TunnelWidth", rTunnelWidth);
  WriteElement("SegmentMapTunneling", "RemoveNodes", fRemoveNodes);
}

SegmentMapTunneling::~SegmentMapTunneling()
{
}

String SegmentMapTunneling::sExpression() const
{
  return String("SegmentMapTunneling(%S,%g,%s)", smp->sNameQuoted(true, fnObj.sPath()),
                rTunnelWidth, pcYesNo(fRemoveNodes));
}

bool SegmentMapTunneling::fDomainChangeable() const
{
  return false;
}

bool SegmentMapTunneling::fValueRangeChangeable() const
{
  return false;
}

void SegmentMapTunneling::Init()
{
  htpFreeze = "ilwisapp\tunnel_segments_functionality_algorithm.htm";
  sFreezeTitle = "SegmentMapTunneling";
}

void SegmentMapTunneling::NodeCheck(long s, long t, bool& fNodeF,
                           bool& fNodeL, long& iNodSegF, long& iNodSegL) {

  // *** To be considered for future implementation:
  // *** Collect only segments with nodes of degree 2 and concatenate them. Easier ???

  if ( fYesF)  {          // current segment s First point in acrdFirst[s],still candidate
                          // and has not yet been treated(i.e. its NodeF set to rcUNDEF)
    if (acrdFirst[s] == acrdFirst[t]) { // if segm s and segm t have 1st crd in common
      if (! fNodeF) {                // if need of this node not yet sure
        if (iNodSegF != 0 ) {        // if at least one nonzero segnumber treated ?
          fNodeF = true;
          if (iNodSegF > 0)
            acrdFirst[iNodSegF] = crdUNDEF;
          else
            acrdLast[-iNodSegF] = crdUNDEF;
        }
        else {
          iNodSegF = t;
        }
      }
      if (fNodeF)
        acrdFirst[t] = crdUNDEF;
    }

    if (acrdFirst[s] == acrdLast[t]) {
      if (! fNodeF ) {
        if ( iNodSegF != 0 ) {
          fNodeF = true;
          if ( iNodSegF > 0 )
            acrdFirst[iNodSegF] = crdUNDEF;
          else
            acrdLast[-iNodSegF] = crdUNDEF;
        }
        else {
          iNodSegF = -t;
        }
      }
      if (fNodeF)
        acrdLast[t] = crdUNDEF;
    }
  }

  if ( fYesL ) {
    if (acrdLast[s] == acrdFirst[t]) {
      if ( ! fNodeL ) {
        if ( iNodSegL != 0 ) {
          fNodeL = true;
          if ( iNodSegL > 0 )
            acrdFirst[iNodSegL] = crdUNDEF;
          else
            acrdLast[-iNodSegL] = crdUNDEF;
        }
        else {
          iNodSegL = t;
        }
      }
      if ( fNodeL )
          acrdFirst[t] = crdUNDEF;
    }

    if (acrdLast[s] == acrdLast[t]) {
      if ( ! fNodeL ) {
        if ( iNodSegL != 0 ) {
          fNodeL = true;
          if ( iNodSegL > 0 )
            acrdFirst[iNodSegL] = crdUNDEF;
          else
            acrdLast[-iNodSegL] = crdUNDEF;
        }
        else {
          iNodSegL = -t;
        }
      }
      if ( fNodeL )
        acrdLast[t] = crdUNDEF;
    }
  }
} //  NodeCheck

bool SegmentMapTunneling::fRemovePseudoNodes(double rTol2)  {

	// Loading nodes of non_deleted segments :
	bool fUseRaw = !fUseReals();
	long i;
	long iNodSegF, iNodSegL;
	bool fNodeF, fNodeL;
	CoordinateSequence *crdBuf, *crdBuf2;// buffers to combine points of two consecutive segmts
	//  acrdFirst.Resize(smp->iFeatures(),1); acrdLast.Resize(smp->iFeatures(),1);// make available 2 arrays
	acrdFirst.Resize(smp->iFeatures()+1); acrdLast.Resize(smp->iFeatures()+1); // is zero based // now available 2 arrays
	// of RowCols having iSeg places (nr of segmts in smp
	fPossible.Resize(smp->iFeatures()+1); // is zero based
	long iSegNr,iNrCrd, iNrCrd2;
	trq.SetText(SSEGTextRemovingPseudoNodes);
	ILWIS::Segment *segNodeLess;
	for (iSegNr=0; iSegNr < smp->iFeatures(); ++iSegNr) {
		ILWIS::Segment *seg = (ILWIS::Segment *)smp->getFeature(iSegNr);
		if ( !seg || ! seg->fValid())
			continue;
		fPossible[iSegNr] = true;
		acrdFirst[iSegNr] = seg->crdBegin();
		acrdLast[iSegNr] = seg->crdEnd();
	}
	// Consider every segment in the map :
	for (int iSegNr=0; iSegNr < smp->iFeatures(); ++iSegNr) {
		ILWIS::Segment *seg = (ILWIS::Segment *)smp->getFeature(iSegNr);
		if ( !seg || ! seg->fValid())
			continue;
		if ( (trq.fUpdate(iSegNr, smp->iFeatures())) ) {
			//acrdFirst.Resize(0);acrdLast.Resize(0); fPossible.Resize(0);
			return false;
		}
		// Get_Segment_Code(InMap, iSegNr, Code);
		if  (! fPossible[iSegNr])  continue;
		crdBuf = seg->getCoordinates();
		iNrCrd = (long)crdBuf->size();

		fNodeF = false; fNodeL = false;

		while (!fNodeF || !fNodeL) {
			fNodeF = false;
			fNodeL = false;
			iNodSegF = 0;
			iNodSegL = 0;
			fYesF = (acrdFirst[iSegNr] != crdUNDEF); // true if NodeF of iSegNr not yet treated
			fYesL = (acrdLast[iSegNr] != crdUNDEF);  // true if NodeL of iSegNr not yet treated
			if  (acrdFirst[iSegNr] == acrdLast[iSegNr]) {
				fNodeF = true;
				fNodeL = true;
			}
			int iSeg2Nr = iSegNr;
			for (ILWIS::Segment *seg2=pms->seg(iSeg2Nr); seg2 && seg2->fValid(); ++iSeg2Nr) {

				if ( ! fPossible[iSeg2Nr] ) continue;
				NodeCheck(iSegNr, iSeg2Nr, fNodeF, fNodeL, iNodSegF, iNodSegL);
			}

			if ( ! fNodeF) {
				if ( iNodSegF > 0 ) {
					ILWIS::Segment *seg2 = (ILWIS::Segment *)smp->getFeature(iNodSegF);
					crdBuf2 = seg2->getCoordinates();
					iNrCrd2 = (long)crdBuf2->size();

					//WrWriteLn('F - F ', iNodSegF:3, iNrCrd2:5, '  "', Code1, '"');
					if (fUseRaw) {
						if ( seg2->iValue() != seg->iValue() )
							fNodeF = true;
					}
					else if ( seg2->rValue() != seg->rValue() )
						fNodeF = true;
					if (!fNodeF && (iNrCrd + iNrCrd2 > 1000 ) )
						fNodeF = true;             // need of this Nodef is sure now

					if (!fNodeF) {               // i.e if this node is redundant
						for ( i = iNrCrd-1 ; i >= 0 ; i--)  {
							crdBuf->setAt(crdBuf->getAt(i), iNrCrd2+i-1);   // points of seg are shifted iNrCrd2-1
						}                                  // positions further in rcBuf
						for ( i = 0 ; i < iNrCrd2-1 ; i++) { // points of seg2, except the last
							crdBuf->setAt(crdBuf2->getAt(iNrCrd2-i-1),i);  //are inserted at begin of rcBuf
						}                                  // in reversed order
						iNrCrd += iNrCrd2-1;               // amount of points in seg is updated
						acrdFirst[iSegNr] = acrdLast[iNodSegF]; // seg firstpoint is renewed
						fPossible[iNodSegF] = false;
					}
					delete crdBuf2;
				}
				else if ( iNodSegF < 0 ) {
					ILWIS::Segment *seg2 = (ILWIS::Segment *)smp->getFeature(-iNodSegF);
					crdBuf2 = seg2->getCoordinates();
					iNrCrd2 = (long)crdBuf2->size();

					//WrWriteLn('F - L ', -iNodSegF:3, iNrCrd2:5, '  "', Code1, '"');
					if (fUseRaw) {
						if ( seg2->iValue() != seg->iValue() )
							fNodeF = true;
					}
					else if ( seg2->rValue() != seg->rValue() )
						fNodeF = true;
					if (!fNodeF && (iNrCrd+ iNrCrd2 > 1000 ) )
						fNodeF = true;             // need of this Nodef is sure now

					if (!fNodeF) {                // i.e if this node is redundant
						for ( i = iNrCrd-1; i >=0; i-- ) {  // points of seg are shifted
							crdBuf->setAt(crdBuf->getAt(i),iNrCrd2+i-1);    // iNrCrd2-1 positions
						}
						for (i = 0 ;i < iNrCrd2-1; i++ ) {  // points of seg2 (except one)
							crdBuf->setAt(crdBuf2->getAt(i),i);             // are inserted at begin of rcBuf
						}
						iNrCrd += iNrCrd2-1;
						acrdFirst[iSegNr] = acrdFirst[-iNodSegF];
						fPossible[-iNodSegF] = false;
					}
					delete crdBuf2;
				}
				else // iNodSegF = 0 (Dead end )
					fNodeF = true;
			}
			if ( ! fNodeL) {
				if ( iNodSegL > 0 ) {
					ILWIS::Segment *seg2 = (ILWIS::Segment *)smp->getFeature(iNodSegL);
					crdBuf2 = seg2->getCoordinates();
					iNrCrd2 = (long)crdBuf2->size();

					//WrWriteLn('L - F ', iNodSegL:3, iNrCrd2:5, '  "', Code1, '"');
					if (fUseRaw) {
						if ( seg2->iValue() != seg->iValue() )
							fNodeL = true;
					}
					else if ( seg2->rValue() != seg->rValue() )
						fNodeL = true;
					if (!fNodeL && ( iNrCrd + iNrCrd2 > 1000 ) )
						fNodeL = true;                   // need of this NodeL is sure now

					if (!fNodeL) {                      // i.e if this node is redundant
						if ( iNodSegL == abs(iNodSegF) )  continue;
						for ( i = 1 ; i < iNrCrd2; i++ ) { // points of seg2 except one are
							crdBuf->setAt(crdBuf2->getAt(i),iNrCrd+i-1);   // appended at end of rcBuf
						}
						iNrCrd = iNrCrd + iNrCrd2-1;
						acrdLast[iSegNr] = acrdLast[iNodSegL];
						fPossible[iNodSegL] = false;
					}
					delete crdBuf2;
				}
				else if ( iNodSegL < 0 ) {
					ILWIS::Segment *seg2 = (ILWIS::Segment *)smp->getFeature(-iNodSegL);
					crdBuf2 = seg2->getCoordinates();
					iNrCrd2 = (long)crdBuf2->size();

					//WrWriteLn('L - L ', -iNodSegL:3, iNrCrd2:5, '  "', Code1, '"');
					if (fUseRaw) {
						if ( seg2->iValue() != seg->iValue() )
							fNodeL = true;
					}
					else if ( seg2->rValue() != seg->rValue() )
						fNodeL = true;
					if (!fNodeL && ( iNrCrd + iNrCrd2 > 1000 ) )
						fNodeL = true;                   // need of this NodeL is sure now

					if (!fNodeL) {                      // i.e if this node is redundant
						if ( iNodSegL == -abs(iNodSegF) )  continue;
						for (i = 1 ; i < iNrCrd2 ; i++) {
							crdBuf->setAt(crdBuf2->getAt(iNrCrd2-i-1),iNrCrd+i-1);// points of seg2 are appended
						}                                       // in reversed order
						iNrCrd = iNrCrd + iNrCrd2-1;
						acrdLast[iSegNr] = acrdFirst[-iNodSegL];
						fPossible[-iNodSegL] = false;
					}
					delete crdBuf2;
				}
				else // iNodSegL = 0 (Dead end})
					fNodeL = true;
			}
		}
		CoordBuf buf(crdBuf);
		if (rTol2 > 0) {
			Tunnel(iNrCrd, buf, rTol2);
		}
		delete crdBuf;

		segNodeLess = CSEGMENT(pms->newFeature());
		segNodeLess->PutCoords(iNrCrd,buf);
		if (fUseRaw)
			segNodeLess->PutVal(seg->iValue());        // take over the raw val of orig segment
		else
			segNodeLess->PutVal(seg->rValue());      // take over the (numeric) val of orig segment
	} // end for seg.fValid()
	//acrdFirst.Resize(0);
	//acrdLast.Resize(0);
	//fPossible.Resize(0);
	return true;
}

bool SegmentMapTunneling::fFreezing()
{
	Init();
	bool fUseRaw = !fUseReals();
	double rTol2 = rTunnelWidth*rTunnelWidth/(0.25);  // speed up tunneling computation
	ILWIS::Segment *segOut;
	long iOutCrd;
	if (fRemoveNodes) {
		if ( !fRemovePseudoNodes(rTol2) ) 
			return false;
	}  // Pseudonodes are removed inclusive tunneling if rTol2 > 0
	else if (rTunnelWidth > 0) {
		trq.SetText(SSEGTextTunnelingSegments_);
		for (int iSegNr=0; iSegNr < smp->iFeatures(); ++iSegNr) {
			ILWIS::Segment *seg = (ILWIS::Segment *)smp->getFeature(iSegNr);
			if ( !seg || ! seg->fValid())
				continue;
			if ( (trq.fUpdate(iSegNr, smp->iFeatures())) )
				return false;
			CoordinateSequence *seq = seg->getCoordinates();
			CoordBuf crdBuf(seq);
			delete seq;
			iOutCrd = (long)crdBuf.size();
			Tunnel(iOutCrd, crdBuf, rTol2); // *** waarom is Tunnel een member v seg ???
			segOut = CSEGMENT(pms->newFeature());          // make a new segm in new seg map
			segOut->PutCoords(iOutCrd, crdBuf); // memberfunction of Segment
			segOut->PutVal(seg->rValue());   // take over the (numeric) val of orig segment
		}
	}
	else if (rTunnelWidth == 0) { // copy each seg to segOut without Tunnel
		trq.SetText(SSEGTextJustCopyingSegments);
		for (int iSegNr=0; iSegNr < smp->iFeatures(); ++iSegNr) {
			ILWIS::Segment *seg = (ILWIS::Segment *)smp->getFeature(iSegNr);
			if ( !seg || ! seg->fValid())
				continue;
			if ( (trq.fUpdate(iSegNr, smp->iFeatures())) )
				return false;
			segOut = CSEGMENT(pms->newFeature(seg));          // make a new segm in new seg map
			segOut->PutVal(seg->iValue());       // take over the raw val of orig segment
		}
	}
	return true;
}

void SegmentMapTunneling::Tunnel(long& iNrCoords, CoordBuf& crdBuf, double rTunnelTolerance)
{
  while (iNrCoords > 2) {
    int iLst = iNrCoords - 1;
    iNrCoords = 1;
	int b = 0;
    for ( ;b + 2 <=iLst;) {
      if (fInTunnel(rTunnelTolerance,
		    crdBuf[b], crdBuf[b+1], crdBuf[b+2]))
        b += 2;
      else
       	b += 1;
      crdBuf[iNrCoords++] = crdBuf[b];
    }
    if (b != iLst)
      crdBuf[iNrCoords++] = crdBuf[iLst];
    if (iLst == iNrCoords - 1)
      break;  
  }
}




