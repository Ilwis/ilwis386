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
/* PolygonMapStore, second part
   Copyright Ilwis System Development ITC
   april 1995, by Wim Koolhoven
   february 1997, split by Willem Nieuwenhuis
   Last change:  WK   10 Aug 98    4:20 pm
*/

#include "Engine\Map\Polygon\POLSTORE.H"
#include "Engine\Map\Polygon\POL14.H"
#include "Engine\Map\Polygon\POLAREA.H"
#include "Headers\Hs\polygon.hs"
//#include "Engine\Base\DataObjects\Tranq.h"

Topology PolygonMapStore::topNew()
{
	Updated();
	sm->segNew();
	return Topology(this,sm->iSeg());
}

Topology PolygonMapStore::topNew(long iTopNr)
{
	Updated();
	sm->segNew(iTopNr);
	return Topology(this, iTopNr);
}

ilwPolygon PolygonMapStore::polNew()
{
	++ptr._iPol;
	tblPolygon->iRecNew();
	
	// Set polygon not Deleted instead of 0 (== UNDEF in DomainBool!)
	tblPolygon->col(PolygonMapStore::ctDeleted)->PutVal(iPol(), (long)false);
	tblPolygon->col(PolygonMapStore::ctTopStart)->PutVal(iPol(), (long)0);
	
	Updated();
	return ilwPolygon(this,iPol());
}

ilwPolygon PolygonMapStore::polNewIsland(const String& sVal, long iNr, const CoordBuf& crdBuf)
{
	PolArea pa;
	for (int i = 1; i < iNr; ++i)
		pa.Line(crdBuf[i-1], crdBuf[i]);
	double rArea = pa.rArea();
	double rLen = pa.rLength();
	
	Topology tNew = topNew();
	if (rArea > 0)
		tNew.PutCoords(iNr,crdBuf);
	else
	{
		CoordBuf b(crdBuf.iSize());
		for (int i = 0; i < iNr; ++i)
			b[i] = crdBuf[iNr-1-i];
		rArea = -rArea;
		tNew.PutCoords(iNr,b);
	}
	// now tNew is always forward linked.
	
	return polNewIsland(tNew, sVal);
}

ilwPolygon PolygonMapStore::polNewIsland(Topology& tNew, const String& sVal)
{
	ilwPolygon polOuter = pol(tNew.crdBegin());  // inclusion ?
	
	long iNr;
	CoordBuf buf(1000);
	tNew.GetCoords(iNr, buf);
	
	PolArea pa;
	for (int i = 1; i < iNr; ++i)
		pa.Line(buf[i-1], buf[i]);
	double rArea = pa.rArea();
	double rLen = pa.rLength();
	
	ilwPolygon pNew = polNew();
	
	tNew.Fwl(tNew.iCurr());
	tNew.Bwl(-tNew.iCurr());
	tNew.LeftPol(pNew.id);
	tNew.RightPol(-1);
	pNew.TopStart(tNew.iCurr());
	pNew.PutName(sVal);
	pNew.Area(rArea);
	pNew.Len(rLen);
	pNew.SetBounds();
	
	if (polOuter.fValid())
	{         // island is in outer polygon.
		tNew.RightPol(polOuter.id);
		Topology ts = polOuter.topStart();
		Topology topCurr;
		Topology topNext = ts;
		do
		{
			topCurr = topNext;
			topNext = topCurr.topNext();
		} while (topNext != ts);
		if (topCurr.fForward())
			topCurr.Fwl(-tNew.iCurr());
		else
			topCurr.Bwl(-tNew.iCurr());
		if (topNext.fForward())
			tNew.Bwl(topNext.iCurr());
		else
			tNew.Bwl(-topNext.iCurr());
		double r = polOuter.rArea();
		r -= rArea;
		polOuter.Area(r);
	}
	
	CheckIslands(pNew, polOuter, 0);
	return pNew;
}

/*
Added february 11, 1997: add a single topology and defer building polygon
*/
Topology PolygonMapStore::AddNonTopolTopology(String& sVal, CoordBuf& crdBuf, Topology& tOld, ilwPolygon& pNew, int iNthTopol)
{
    Topology tNew = topNew();
    tNew.PutCoords(crdBuf.iSize(), crdBuf);
    if ( iNthTopol==0 && sVal!="")
    {
        pNew = polNew();
        pNew.PutVal(sVal);
        pNew.TopStart(tNew.iCurr());
    }
    if ( tOld.fValid() )
    {
        tNew.Bwl(-tOld.iCurr());
        tOld.Fwl(tNew.iCurr());
    }
    else
    {
        tNew.Fwl(tNew.iCurr());
        tNew.Bwl(-tNew.iCurr());
    }
	
    if ( sVal!="" )
    {
        tNew.LeftPol(pNew.id);
        tNew.RightPol(-1);
    }
    else
    {
        tNew.RightPol(pNew.id);
        tNew.LeftPol(-1);
    }
	
    Updated();
    return tNew;
}

ilwPolygon PolygonMapStore::polNewNonTopoPolygon(long iNr, const CoordBuf& crdBuf)
{
	if (iNr < 3)
		return ilwPolygon();
	Coord crdFirst = crdBuf[0];
	Coord crdLast = crdBuf[iNr-1];
	if (crdFirst != crdLast)
	{
		if (rDist2(crdFirst, crdLast) > 1e-3)
			return ilwPolygon(); // just skip them

		crdBuf[iNr-1] = crdFirst; // just make them equal
	}
	
	PolArea pa;
	for (int i = 1; i < iNr; ++i)
		pa.Line(crdBuf[i-1], crdBuf[i]);
	double rArea = pa.rArea();
	double rLen = pa.rLength();
	
	Topology tNew = topNew();
	if (rArea > 0)
		tNew.PutCoords(iNr,crdBuf);
	else
	{
		CoordBuf b(crdBuf.iSize());
		for (int i = 0; i < iNr; ++i)
			b[i] = crdBuf[iNr-1-i];
		rArea = -rArea;
		tNew.PutCoords(iNr,b);
	}
	// now tNew is always forward linked
	
	ilwPolygon pNew = polNew();
	tNew.Fwl(tNew.iCurr());
	tNew.Bwl(-tNew.iCurr());
	tNew.LeftPol(pNew.id);
	tNew.RightPol(-1);
	pNew.TopStart(tNew.iCurr());
	pNew.Area(rArea);
	pNew.Len(rLen);
	pNew.SetBounds();
	pNew.PutRaw(iUNDEF);
	return pNew;
}

void PolygonMapStore::NewNonTopolPolygon(const String& sVal, long iNr, const CoordBuf& crdBuf)
{
	ilwPolygon pNew = polNewNonTopoPolygon(iNr, crdBuf);
	if (!pNew.fValid())
		return;

	pNew.PutName(sVal);
}

void PolygonMapStore::RelinkIsland(Topology top, ilwPolygon pol, Tranquilizer* trq)
{
	Coord crdA, crdB;
	Topology t0, t1;
	t0 = top;
	bool fJump = false;
	long iTops = 0;
	for (;;)
	{
		if (++iTops > 5000)
			TooMuchTopsError();

		if (0 != trq)
			trq->fAborted();

		t1 = t0.topNext();
		crdA = t0.fForward() ? t0.crdEnd() : t0.crdBegin();
		crdB = t1.fForward() ? t1.crdBegin() : t1.crdEnd();
		if (crdA != crdB) {
			fJump = true;
			break;
		}
		if (t1 == top)
			break;
		t0 = t1;
	}
	if (fJump)    // if needed first unlink from outer
	{
		// island t0 jump t1 outer t2 jump t3 island
		Topology t2, t3;
		Coord crd2, crd3;
		t2 = t1;
		iTops = 0;
		for (;;)
		{
			if (++iTops > 5000)
				TooMuchTopsError();

			if (0 != trq)
				trq->fAborted();

			t3 = t2.topNext();
			crd2 = t2.fForward() ? t2.crdEnd() : t2.crdBegin();
			crd3 = t3.fForward() ? t3.crdBegin() : t3.crdEnd();
			if (crd3 == crdA)
				break;
			t2 = t3;
		}
		if (t0.fForward())
			t0.Fwl(t3.id);
		else
			t0.Bwl(t3.id);
		if (t2.fForward())
			t2.Fwl(t1.id);
		else
			t2.Bwl(t1.id);
	}
	// and link it to pol
	t1 = t0.topNext();
	Topology ts = pol.topStart();
	if (t0.fForward())
		t0.Fwl(ts.id);
	else
		t0.Bwl(ts.id);
	Topology t, tTmp;
	t = ts;
	iTops = 0;
	for(;;)
	{
		if (++iTops > 5000)
			TooMuchTopsError();

		if (0 != trq)
			trq->fAborted();

		tTmp = t.topNext();
		if (tTmp == ts)
			break;
		t = tTmp;
	}
	if (t.fForward())
		t.Fwl(t1.id);
	else
		t.Bwl(t1.id);
	// set correct left/rightpol of topologies
	t = top;
	iTops = 0;
	for (;;)
	{
		if (++iTops > 5000)
			TooMuchTopsError();

		if (t.fForward())
			t.LeftPol(pol.id);
		else
			t.RightPol(pol.id);
		t = t.topNext();
		if (t == top)
			break;
	}
}

void PolygonMapStore::LinkIsland(Topology tsNewIsland, ilwPolygon polOuter, double rArea, Tranquilizer* trq)
{
	// this function links island of which tsNewIsland is the topStart into polygon polOuter  
	Topology tsOuter = polOuter.topStart();
	Topology topLastOuter;
	Topology topNextOuter = tsOuter;
	long iTops = 0;
	do
	{
		if (++iTops > 5000)
			PolygonMapStore::TooMuchTopsError();

		if (0 != trq)
			trq->fAborted();

		topLastOuter = topNextOuter;
		topNextOuter = topLastOuter.topNext();
	} while (topNextOuter != tsOuter);

	Topology topLastNewIsland;
	Topology topNextNewIsland = tsNewIsland;
	do
	{
		topLastNewIsland = topNextNewIsland;
		topNextNewIsland = topLastNewIsland.topNext();
	} while (topNextNewIsland != tsNewIsland);

	if (topLastOuter.fForward())
	{
		if (tsNewIsland.fForward())
			topLastOuter.Fwl(tsNewIsland.iCurr());
		else
			topLastOuter.Fwl(-tsNewIsland.iCurr());
	}
	else
	{
		if (tsNewIsland.fForward())
			topLastOuter.Bwl(tsNewIsland.iCurr());
		else
			topLastOuter.Bwl(-tsNewIsland.iCurr());
	}
	if (topLastNewIsland.fForward())
	{
		if (tsOuter.fForward())
			topLastNewIsland.Fwl(tsOuter.iCurr());
		else
			topLastNewIsland.Fwl(-tsOuter.iCurr());
	}
	else
	{
		if (tsOuter.fForward())
			topLastNewIsland.Bwl(tsOuter.iCurr());
		else
			topLastNewIsland.Bwl(-tsOuter.iCurr());
	}
	double r = polOuter.rArea();
	r -= rArea;
	polOuter.Area(r);
}

void PolygonMapStore::AddBoundary(Topology& tNew, const String& sVal, Tranquilizer* trq)
{
	long iNr;
	CoordBuf buf(1000);
	tNew.GetCoords(iNr, buf);
	Coord crdBegin = buf[0];
	Coord crdEnd = buf[iNr - 1];
	Coord crd;
	crd.y = crdBegin.y / 2 + buf[1].y / 2;
	crd.x = crdBegin.x / 2 + buf[1].x / 2;
	ilwPolygon pOld = pol(crd);
	Topology tBegin, tEnd;
	bool fConnectedToOldPolygon = false;
	// tBegin and tEnd are the topologies connected 
	// to the beginning and end of the new topology
	if (pOld.fValid())
	{
		Topology ts = pOld.topStart();
		Topology t = ts;
		long iTops = 0;
		do
		{
			if (0 != trq)
				trq->fAborted();

			if (++iTops > 5000)
			{
				TooMuchTopsError(pOld);
				return;
			}  
			if (t.fForward())
				crd = t.crdEnd();
			else
				crd = t.crdBegin();
			if (crd == crdBegin) 
				tBegin = t;
			if (crd == crdEnd)   
				tEnd = t;
			t = t.topNext();
		} while (t.id != ts.id);
		fConnectedToOldPolygon = tBegin.fValid() /*||*/&& tEnd.fValid(); // Jelle: 12-feb-97, to prevent invalid tBegin or tEnd
	}
	if (!fConnectedToOldPolygon)
	{
		// check other segments for connectivity
		double rAzimBegin = tNew.rAzim(false);
		double rAzimEnd = tNew.rAzim(true);
		double r1, r3;
		r1 = 100;
		r3 = 100;
		Segment s;
		tBegin = tEnd = tNew;
		for (s = sm->segFirst(); s.fValid(); ++s)
		{
			if (s.iCurr() == tNew.iCurr())
				continue;

			if (0 != trq)
				trq->fAborted();

			if (s.crdBegin() == crdBegin)
			{
				double r = s.rAzim(false);
				r -= rAzimBegin;
				if (r == 0)       // ???????????????????????????
					return;
				if (r < 0) 
					r += 2 * M_PI;
				if (r < r1)
				{
					tBegin.id = -s.iCurr();
					r1 = r;
				}
			}
			if (s.crdEnd() == crdBegin)
			{
				double r = s.rAzim(true);
				r -= rAzimBegin;
				if (r == 0)    // ???????????????????????????
					return;
				if (r < 0) r += 2 * M_PI;
				if (r < r1)
				{
					tBegin.id = s.iCurr();
					r1 = r;
				}
			}
			if (s.crdBegin() == crdEnd)
			{
				double r = s.rAzim(false);
				r -= rAzimEnd;
				if (r == 0)    // ???????????????????????????
					return;
				if (r < 0) r += 2 * M_PI;
				if (r < r3)
				{
					tEnd.id = -s.iCurr();
					r3 = r;
				}
			}
			if (s.crdEnd() == crdEnd)
			{
				double r = s.rAzim(true);
				r -= rAzimEnd;
				if (r == 0)    // ???????????????????????????
					return;
				if (r < 0) r += 2 * M_PI;
				if (r < r3)
				{
					tEnd.id = s.iCurr();
					r3 = r;
				}
			}
		}
	}
	if (tBegin == tNew && tEnd == tNew) // it's a segment without connection to others
	{
		tNew.Fwl(-tNew.id);
		tNew.Bwl(tNew.id);
		//    return; // 25/2/97 Wim: return seems most unwise to me.
	}
	Topology tNextOfBegin = tBegin.topNext();
	if (tBegin == tNew)
	{
		tNew.Bwl(tNew.id);
	}
	else
	{
		tNew.Bwl(tNextOfBegin.id);
		if (tBegin.fForward())
			tBegin.Fwl(tNew.id);
		else
			tBegin.Bwl(tNew.id);
	}
	if (tEnd == tNew)
		tNew.Fwl(-tNew.id);
	else if (tBegin == tEnd)  // 27/2/97 Wim
	{
		// find special case where new polygon surrounds totally old one:
		double rAzimBegin = tNew.rAzim(false);
		double rAzimEnd = tNew.rAzim(true);
		double rAzimOld = tBegin.rAzim(false); 
		double rStart = rAzimOld - rAzimBegin;
		if (rStart < 0) 
			rStart += 2 * M_PI;
		double rEnd = rAzimOld - rAzimEnd;
		if (rEnd < 0) 
			rEnd += 2 * M_PI;
		if (rEnd > rStart)  // twitched case
		{
			tNew.Fwl(tNextOfBegin.id);
			tNew.Bwl(-tNew.id);
		}  
		else  // hanging polygon outside of old one
		{
			tNew.Fwl(tNew.id);
			if (tBegin.fForward())
				tBegin.Fwl(-tNew.id);
			else
				tBegin.Bwl(-tNew.id);
		}
	} 
	else
	{
		Topology tNextOfEnd = tEnd.topNext();
		tNew.Fwl(tNextOfEnd.id);
		if (tEnd.fForward())
			tEnd.Fwl(-tNew.id);
		else
			tEnd.Bwl(-tNew.id);
	}  
	
	tNew.LeftPol(pOld.id);
	tNew.RightPol(pOld.id);
	SList<Topology> JumpList;
	try
	{
		UnlinkIslands(tNew, JumpList, trq);
		UnlinkIslands(-tNew, JumpList, trq);
		CheckJumpList(JumpList, trq);
	}
	catch (ErrorObject& err)
	{
		err.Show();
		DeleteTopology(tNew);
		return;
	}  
	
	// check if polygon hasn't been changed
	// this is true if following the topology starting from tNew it comes back to tNew
	bool fNewPolygonCreated = true;
	// Checking is the only garantee ! Wim 25/2/97 
	//  if (!fConnectedToOldPolygon) {  
    Topology ts = -tNew;
    Topology t = ts;
    long iTops = 0;
    do
	{
		if (++iTops > 5000)
			TooMuchTopsError();
		t = t.topNext();
		if (t == tNew) {
			fNewPolygonCreated = false;
			break;
		}
    } while (t.id != ts.id);
	//  }
	//  else 
	//    fNewPolygonCreated = tBegin.fValid() && tEnd.fValid();
	
	if (!fNewPolygonCreated)
	{
		if (pOld.fValid())  // reset Area and perimeter
		{
			PolArea paOld;
			paOld.Pol(pOld);
			pOld.Area(paOld.rArea());
			pOld.Len(paOld.rLength());
		}
	}
	else 
	{
		ilwPolygon pNew = polNew();
		if (sVal.length())
			pNew.PutName(sVal);
		else if (pOld.fValid())
			pNew.PutName(pOld.sValue());
		else
			pNew.PutName(String());
		pNew.TopStart(-tNew.id);
		PolArea paBwd;
		paBwd.Pol(pNew);
		bool fBwd = paBwd.rArea() > 0;
		pNew.TopStart(tNew.id);
		PolArea paFrw;
		paFrw.Pol(pNew);
		bool fFrw = paFrw.rArea() > 0;
		if (fFrw && fBwd) 
		{
			pNew.TopStart(tNew.id);
			pNew.SetBounds();
			pNew.Area(paFrw.rArea());
			pNew.Len(paFrw.rLength());
			tNew.LeftPol(pNew.id);
			if (pOld.fValid())
			{
				pOld.TopStart(-tNew.id);
				pOld.SetBounds();
				PolArea paOld;
				paOld.Pol(pOld);
				pOld.Area(paOld.rArea());
				pOld.Len(paOld.rLength());
			}
			long iTops = 0;
			for (Topology t = tNew.topNext(); t.id != tNew.id; t = t.topNext())
			{
				if (++iTops > 5000)
					TooMuchTopsError();

				if (t.fForward())
				{
					t.LeftPol(pNew.id);
					if (!fConnectedToOldPolygon && pOld.fValid())
						if (t.RightPol() < 0)
							t.RightPol(pOld.id);
				}
				else
				{
					t.RightPol(pNew.id);
					if (!fConnectedToOldPolygon && pOld.fValid())
						if (t.LeftPol() < 0)
							t.LeftPol(pOld.id);
				}
			}
			if (!fConnectedToOldPolygon && pOld.fValid())
				LinkIsland(tNew, pOld, paFrw.rArea(), trq);
		}
		else if (fFrw)
		{
			pNew.TopStart(tNew.id);
			pNew.SetBounds();
			pNew.Area(paFrw.rArea());
			pNew.Len(paFrw.rLength());
			tNew.LeftPol(pNew.id);
			Topology t = tNew;
			long iTops = 0;
			for (t = t.topNext(); t.id != tNew.id; t = t.topNext())
			{
				if (++iTops > 5000)
					TooMuchTopsError();

				if (t.fForward())
				{
					t.LeftPol(pNew.id);
					if (!fConnectedToOldPolygon && pOld.fValid())
						if (t.RightPol() < 0)
							t.RightPol(pOld.id);
				}
				else
				{
					t.RightPol(pNew.id);
					if (!fConnectedToOldPolygon && pOld.fValid())
						if (t.LeftPol() < 0)
							t.LeftPol(pOld.id);
				}
			}
			CheckIslands(pNew, pOld, trq);
			if (!fConnectedToOldPolygon && pOld.fValid())
				LinkIsland(tNew, pOld, paFrw.rArea(), trq);
		}
		else if (fBwd)
		{
			pNew.TopStart(-tNew.id);
			pNew.SetBounds();
			pNew.Area(paBwd.rArea());
			pNew.Len(paBwd.rLength());
			tNew.RightPol(pNew.id);
			Topology t = -tNew;
			long iTops = 0;
			for (t = t.topNext(); t.id != -tNew.id; t = t.topNext())
			{
				if (++iTops > 5000)
					TooMuchTopsError();

				if (t.fForward())
				{
					t.LeftPol(pNew.id);
					if (!fConnectedToOldPolygon && pOld.fValid())
						if (t.RightPol() < 0)
							t.RightPol(pOld.id);
				}
				else
				{
					t.RightPol(pNew.id);
					if (!fConnectedToOldPolygon && pOld.fValid())
						if (t.LeftPol() < 0)
							t.LeftPol(pOld.id);
				}
			}
			CheckIslands(pNew, pOld, trq);
			if (!fConnectedToOldPolygon && pOld.fValid())
				LinkIsland(tNew, pOld, paBwd.rArea(), trq);
		}
		else
		{
			--ptr._iPol;
			pNew.id = -1;
		}
		AddLastPol(pNew.iCurr());
	}
	AddLastPol(pOld.iCurr());
	
	for (SLIter<Topology> iter(&JumpList); iter.fValid(); ++iter)
	{
		if (0 != trq)
			trq->fAborted();
		
		Topology top = iter();
		Coord crd = top.crdBegin();
		ilwPolygon p = pol(crd);
		if (p.fValid())
			RelinkIsland(top, p, trq);
	}
	
	// check if both forward and backward topology have positive polygon areas
	// otherwise it is part of an inverse island and check for outer island 
	// should be made
	crd = tNew.crdBegin();
	//  ilwPolygon p = pol(rc);
	if (pOld.fValid())
	{
		if (!fPositiveArea(tNew)) 
			RelinkIsland(tNew, pOld, trq);
		if (!fPositiveArea(-tNew)) 
			RelinkIsland(-tNew, pOld, trq);
	}  
}

Topology PolygonMapStore::topNewBoundary(const String& sVal, long iNr, const CoordBuf& crdBuf, Tranquilizer* trq)
{
	if (iNr <= 1)
		return Topology();
	Coord crdBegin = crdBuf[0];
	Coord crdEnd = crdBuf[iNr - 1];
	if (crdBegin == crdEnd)
	{
		if (iNr <= 3) // Jelle, 12-feb-97: island is one point; Wim 26/2/97: island should contain something!
			return Topology();
		// only create island, if it is a true island.
		bool fIsland = true;
		for (Segment seg = sm->segFirst(); seg.fValid(); ++seg)
		{
			if (0 != trq)
				trq->fAborted();

			if (crdBegin == seg.crdBegin() || crdBegin == seg.crdEnd())
			{
				fIsland = false;
				break;
			}  
		}
		if (fIsland)
		{
			ilwPolygon pol = polNewIsland(sVal, iNr, crdBuf);
			return pol.topStart();
		}
		else
		{ 
			// invert clockwise closing
			PolArea pa;
			for (int i = 1; i < iNr; ++i)
				pa.Line(crdBuf[i-1], crdBuf[i]);
			double rArea = pa.rArea();
			if (rArea < 0)
			{
				CoordBuf b(crdBuf.iSize());
				for (int i = 0; i < iNr; ++i)
					b[i] = crdBuf[iNr-1-i];
				const_cast<CoordBuf&>(crdBuf) = b;  
			}
		}
	}
	Topology tNew = topNew();
	tNew.PutCoords(iNr,crdBuf);
	AddBoundary(tNew,sVal, trq);
	if ((tNew.Fwl() == 0) || (tNew.Bwl() == 0))
		tNew.Delete();
	return tNew;
}
