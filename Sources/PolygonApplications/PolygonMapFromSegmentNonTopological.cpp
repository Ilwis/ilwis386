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
/* $Log:
  PolygonMapFromSegmentNonTopological.cpp: implementation of the PolygonMapFromSegmentNonTopological class.
*/

#include "Engine\Applications\POLVIRT.H"
#include "Engine\Map\Segment\Seg.h"
#include "Engine\Map\Point\PNT.H"
#include "Applications\Polygon\PolygonMapFromSegmentNonTopological.h"
#include "Engine\Map\Segment\SEGSTORE.H"
#include "Engine\Base\DataObjects\valrange.h"
#include "Engine\Domain\dmsort.h"
#include "Headers\Err\Ilwisapp.err"
#include "Headers\Htp\Ilwisapp.htp"
#include "Engine\Base\mask.h"
#include "Headers\Hs\polygon.hs"

IlwisObjectPtr * createPolygonMapFromSegmentNonTopological(const FileName& fn, IlwisObjectPtr& ptr, const String& sExpr, vector<void *> parms ) {
	if ( sExpr != "")
		return (IlwisObjectPtr *)PolygonMapFromSegmentNonTopological::create(fn, (PolygonMapPtr &)ptr, sExpr);
	else
		return (IlwisObjectPtr *)new PolygonMapFromSegmentNonTopological(fn, (PolygonMapPtr &)ptr);
}


const char* PolygonMapFromSegmentNonTopological::sSyntax() {
  return "PolygonMapFromSegmentNonTopo(segmap[,\"mask\"[,unique|segments|labelpntmap|domain]])";
}

PolygonMapFromSegmentNonTopological* PolygonMapFromSegmentNonTopological::create(const FileName& fn, PolygonMapPtr& p, const String& sExpr)
{
	Array<String> as;
	int iParms = IlwisObjectPtr::iParseParm(sExpr, as);
	if (iParms < 1)
		throw ErrorExpression(sExpr, sSyntax());
	
	// Disable use of attribute maps. A SegmentMapAttribute has no method to access the segments
	// via the MapVirtual (yet)
	String sInputMapName = as[0];
	char *pCh = sInputMapName.strrchrQuoted('.');
	if ((pCh != 0) && (0 != _strcmpi(pCh, ".mps")))  // attrib map
		throw ErrorObject(WhatError(String(SPOLErrNoAttColumnAllowed_S.scVal(), as[0]), errPolygonMapFromSegment), fn);
	
	SegmentMap smp(as[0], fn.sPath());
	String sMask;
	if (iParms > 1)
		sMask = as[1];
	if (iParms > 3)
		throw ErrorExpression(sExpr, sSyntax());
	
	bool fSegments = false;
	if (3 == iParms)
	{
		String s2 = as[2];
		s2.toLower();
		if ("unique" == s2)
		{
			fSegments = false;		
		}
		else if ("segments" == s2)
		{
			fSegments = true;		
		}
		else
		{
			FileName fn2(as[2]);
			if (".dom" == fn2.sExt)
			{
				Domain dm(as[2], fn.sPath());
				return new PolygonMapFromSegmentNonTopological(fn, p, smp, sMask, dm);
			}
			else
			{
				PointMap pm(as[2], fn.sPath());
				return new PolygonMapFromSegmentNonTopological(fn, p, smp, sMask, pm);
			}
		}
	}
	return new PolygonMapFromSegmentNonTopological(fn, p, smp, sMask, fSegments);
}

PolygonMapFromSegmentNonTopological::PolygonMapFromSegmentNonTopological(const FileName& fn, PolygonMapPtr& p)
: PolygonMapVirtual(fn, p)
{
	Init();
  String sColName;
  try {
    ReadElement("PolygonMapFromSegmentNonTopo", "SegmentMap", smap);
    sMask = "*";
    ReadElement("PolygonMapFromSegmentNonTopo", "Mask", sMask);
		String sDom;
    ReadElement("PolygonMapFromSegmentNonTopo", "Domain", sDom);
		sDom.toLower();
		if ("unique" == sDom) {
			eType = eUNIQUE;
		}
		else if ("segments" == sDom) {
			eType = eSEGMENTS;
		}
		else if ("labels" == sDom) {
			eType = eLABELS;
			try {
				ReadElement("PolygonMapFromSegmentNonTopo", "Labels", pmapLbl);
			}
			catch (const ErrorObject& ) {
				eType = eDOMAIN;
			}
		}
		else
			eType = eDOMAIN;
  }
  catch (const ErrorObject& err) {  // catch to prevent invalid object
    err.Show();
    return;
  }
  objdep.Add(smap.ptr());
  if (!fnObj.fValid())
    objtime = objdep.tmNewest();
}

PolygonMapFromSegmentNonTopological::PolygonMapFromSegmentNonTopological(
     const FileName& fn, PolygonMapPtr& p,
     const SegmentMap& sm, const String& sMsk, bool fSegm)
: PolygonMapVirtual(fn, p, sm->cs(),sm->cb(),
                    fSegm ? sm->dm() : Domain(fn, sm->iSeg(), dmtUNIQUEID, "Pol")),
  smap(sm), sMask(sMsk)
{
	if (fSegm)
		eType = eSEGMENTS;
	else
		eType = eUNIQUE;
	ptr.TopologicalMap(false);  
  Init();
  objdep.Add(smap.ptr());
  Store();
}

PolygonMapFromSegmentNonTopological::PolygonMapFromSegmentNonTopological(     
		const FileName& fn, PolygonMapPtr& p,
    const SegmentMap& sm, const String& sMsk, const Domain& dm)
: PolygonMapVirtual(fn, p, sm->cs(),sm->cb(), dm),
  smap(sm), sMask(sMsk), eType(eDOMAIN)
{
	ptr.TopologicalMap(false);  
  Init();
  objdep.Add(smap.ptr());
  Store();
}

PolygonMapFromSegmentNonTopological::PolygonMapFromSegmentNonTopological(     
		const FileName& fn, PolygonMapPtr& p,
    const SegmentMap& sm, const String& sMsk, const PointMap& pmLbl)
: PolygonMapVirtual(fn, p, sm->cs(),sm->cb(), pmLbl->dm()),
  smap(sm), sMask(sMsk), pmapLbl(pmLbl), eType(eLABELS)
{
	ptr.TopologicalMap(false);  
  Init();
  objdep.Add(smap.ptr());
  objdep.Add(pmapLbl.ptr());
  Store();
}


PolygonMapFromSegmentNonTopological::~PolygonMapFromSegmentNonTopological()
{
}

void PolygonMapFromSegmentNonTopological::Store()
{
  PolygonMapVirtual::Store();
  WriteElement("PolygonMapVirtual", "Type", "PolygonMapFromSegmentNonTopo");
  WriteElement("PolygonMapFromSegmentNonTopo", "SegmentMap", smap);
  WriteElement("PolygonMapFromSegmentNonTopo", "Mask", sMask);
	WriteElement("PolygonMapFromSegmentNonTopo", "Labels", (char*)0);
	switch (eType)
	{
		case eLABELS:
	    WriteElement("PolygonMapFromSegmentNonTopo", "Domain", "labels");
		  WriteElement("PolygonMapFromSegmentNonTopo", "Labels", pmapLbl);
			break;
		case eSEGMENTS:
		  WriteElement("PolygonMapFromSegmentNonTopo", "Domain", "segments");
			break;
		case eUNIQUE:
		  WriteElement("PolygonMapFromSegmentNonTopo", "Domain", "unique");
			break;
		case eDOMAIN:
		  WriteElement("PolygonMapFromSegmentNonTopo", "Domain", "domain");
			break;
	}
}

String PolygonMapFromSegmentNonTopological::sExpression() const
{
  String s;
	switch (eType)
	{
		case eLABELS:
	    s = pmapLbl->sNameQuoted(true, fnObj.sPath());
			break;
		case eSEGMENTS:
			s = "segments";
			break;
		case eUNIQUE:
			s = "unique";
			break;
		case eDOMAIN:
			s = ptr.dm()->sNameQuoted(true, fnObj.sPath());
			break;
	}
  return String("PolygonMapFromSegmentNonTopo(%S,\"%S\",%S)",
              smap->sNameQuoted(false, fnObj.sPath()), sMask, s);
}

bool PolygonMapFromSegmentNonTopological::fDomainChangeable() const
{
	switch (eType)
	{
		case eLABELS:
		case eSEGMENTS:
		case eUNIQUE:
			return false;
		case eDOMAIN:
			return true;
		default: // should not happen
			return false;
	}
}

void PolygonMapFromSegmentNonTopological::Init()
{
  fNeedFreeze = true;
  htpFreeze = htpPolygonMapFromSegmentT;
  sFreezeTitle = "PolygonMapFromSegmentNonTopo";
}

bool PolygonMapFromSegmentNonTopological::fFreezing()
{
  Mask mask(smap->dm(), sMask);
  bool fMask = sMask.length() > 0;

  CoordBuf crdBuf(1000);
  long iNr;
  long iSeg = smap->iSeg();
  trq.SetText(SPOLTextCopySegments);
	bool fUseReals = dvrs().fUseReals();
  Segment seg;

  for (seg = smap->segFirst(); seg.fValid(); ++seg) 
	{
    if (trq.fUpdate(seg.iCurr(), iSeg))
      return false;
    if (fMask && !seg.fInMask(mask))
      continue;
    seg.GetCoords(iNr, crdBuf);	 
		Polygon pol = pms->polNewNonTopoPolygon(iNr, crdBuf);
		if (!pol.fValid())
			continue;
	  if (!pmapLbl.fValid()) 
			if (eSEGMENTS == eType)
        if (fUseReals)
          pol.PutVal(seg.rValue());
        else
          pol.PutRaw(seg.iRaw());
			else if (eUNIQUE == eType)
				pol.PutRaw(pol.iCurr());
  }
  if (eLABELS == eType) 
	{
    trq.SetText(SPOLTextLabelPolygons);
    CoordSystem csLbl = pmapLbl->cs();
    CoordSystem csPol = cs();
    bool fTransformCoords = csPol != csLbl;
    long iNrPnt = pmapLbl->iPnt();
    for (int i = 1; i <= iNrPnt; ++i) {
      if (trq.fUpdate(i, iNrPnt))
        return false;
      Coord crdPnt = pmapLbl->cValue(i);
      if (fTransformCoords)
         crdPnt = csPol->cConv(csLbl, crdPnt);
      Polygon pol = pms->pol(crdPnt); // point in polygon
      if (pol.fValid())
        if (fUseReals)
          pol.PutVal(pmapLbl->rValue(i));
        else
          pol.PutRaw(pmapLbl->iRaw(i));
    }
  }
	return true;
}


