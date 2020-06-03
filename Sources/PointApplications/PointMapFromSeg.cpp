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
/* PointMapFromSeg
   Copyright Ilwis System Development ITC
   nov 1995, by Martin Schouwenburg
	Last change:  JEL   6 May 97    5:54 pm
*/


#include "PointApplications\PointMapFromSeg.H"
#include "Engine\Map\Point\PNTSTORE.H"
#include "Engine\Base\DataObjects\valrange.h"
#include "Headers\Htp\Ilwisapp.htp"
#include "Headers\Err\Ilwisapp.err"
#include "Headers\Hs\point.hs"


IlwisObjectPtr * createPointMapFromSeg(const FileName& fn, IlwisObjectPtr& ptr, const String& sExpr, vector<void *> parms ) {
	if ( sExpr != "")
		return (IlwisObjectPtr *)PointMapFromSeg::create(fn, (PointMapPtr &)ptr, sExpr);
	else
		return (IlwisObjectPtr *)new PointMapFromSeg(fn, (PointMapPtr &)ptr);
}

const char* PointMapFromSeg::sSyntax() {
  return "PointMapFromSeg(map, minimumDistance)";
}

PointMapFromSeg* PointMapFromSeg::create(const FileName& fn, PointMapPtr& p, const String& sExpr)
{
  Array<String> as(2);
  if (!IlwisObjectPtr::fParseParm(sExpr, as))
    throw ErrorExpression(sExpr, sSyntax());
  SegmentMap segmap(as[0], fn.sPath());
  double minimumDist = as[1].rVal();
  if ( minimumDist == rUNDEF)
	throw ErrorExpression(sExpr, sSyntax());
  return new PointMapFromSeg(fn, p, segmap, minimumDist);
}

PointMapFromSeg::PointMapFromSeg(const FileName& fn, PointMapPtr& p)
: PointMapVirtual(fn, p)
{
  ReadElement("PointMapFromSeg", "SegmentMap", smap);
  ReadElement("PointMapFromSeg", "MinimumDistance", minDist);
  fNeedFreeze = true;
  Init();
  objdep.Add(smap.ptr());
}

PointMapFromSeg::~PointMapFromSeg()
{
}

PointMapFromSeg::PointMapFromSeg(const FileName& fn, PointMapPtr& p, const SegmentMap& smp, double minimumDist)
: PointMapVirtual(fn, p, smp->cs(),smp->cb(), smp->dvrs()), smap(smp), minDist(minimumDist)
{
  fNeedFreeze = true;
  Init();
  objdep.Add(smap.ptr());
  if (!fnObj.fValid())
    objtime = objdep.tmNewest();
  if (smap->fTblAttSelf())
    SetAttributeTable(smap->tblAtt());
}

void PointMapFromSeg::Store()
{
  PointMapVirtual::Store();
  WriteElement("PointMapVirtual", "Type", "PointMapFromSeg");
  WriteElement("PointMapFromSeg", "SegmentMap", smap);
  WriteElement("PointMapFromSeg", "MinimumDistance", minDist);
}

String PointMapFromSeg::sExpression() const
{
  return String("PointMapFromSeg(%S)", smap->sNameQuoted(true, fnObj.sPath()));
}

bool PointMapFromSeg::fDomainChangeable() const
{
  return false;
}

void PointMapFromSeg::Init()
{
  sFreezeTitle = "PointMapFromSeg";
}

bool PointMapFromSeg::fFreezing()
{
  trq.SetText(String(TR("Calculating number of points. '%S'").c_str(), sName(true, fnObj.sPath())));
  
  for (long i=1; i <= smap->iFeatures(); ++i ) {
    if (trq.fUpdate(i, smap->iFeatures()))
      return false;
	ILWIS::Segment *seg = CSEGMENT(smap->getFeature(i));
	if ( seg && seg->fValid()) {
		const CoordinateSequence *seq = seg->getCoordinatesRO();
		Coord crdOld;
		for(int j = 0; j < seq->size(); ++j) {
			Coord crd = seq->getAt(j);
			if ( !crdOld.fUndef() && minDist > 0) {
				double delta = rDist(crdOld, crd);
				if ( delta < minDist) {
					continue;
				}
			}
			if (fUseReals())
				pms->iAddVal(crd, seg->rValue());
			else
				pms->iAddRaw(crd, seg->iValue());
			crdOld = crd;
		}
	}

  }
  return true;
}




