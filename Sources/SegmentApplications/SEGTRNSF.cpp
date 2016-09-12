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
/* SegmentMapTransform
   Copyright Ilwis System Development ITC
   may 1996, by Jelle Wind
	Last change:  JEL   6 May 97    6:03 pm
*/                                                                      

#include "SegmentApplications\SEGTRNSF.H"
#include "Engine\Base\DataObjects\valrange.h"
#include "Engine\Base\DataObjects\ERR.H"
#include "Headers\Err\Ilwisapp.err"
#include "Headers\Htp\Ilwisapp.htp"
#include "Headers\Hs\segment.hs"
#include "Engine\Base\Round.h"

IlwisObjectPtr * createSegmentMapTransform(const FileName& fn, IlwisObjectPtr& ptr, const String& sExpr, vector<void *> parms ) {
	if ( sExpr != "")
		return (IlwisObjectPtr *)SegmentMapTransform::create(fn, (SegmentMapPtr &)ptr, sExpr);
	else
		return (IlwisObjectPtr *)new SegmentMapTransform(fn, (SegmentMapPtr &)ptr);
}

#define EPS10 1.0e-10

const char* SegmentMapTransform::sSyntax() {
  return "SegmentMapTransform(segmap,coordsys[,dist])";
}

double SegmentMapTransform::rDefaultDensifyDistance(const SegmentMap& segmap)
{
  double w = segmap->cb().width();
  double h = segmap->cb().height();
  double m = min(w,h);
  long iNrSeg = segmap->iFeatures();
  if (iNrSeg == 0 || !segmap->cs().fValid())
    return rUNDEF;
  else
	if (segmap->cs()->pcsLatLon()) 
		return rRoundDMS(m / 1000); // default distance in DMS
	else								// between denser coords
		return rRound (m / 1000); //default distance in meters
}

SegmentMapTransform* SegmentMapTransform::create(const FileName& fn, SegmentMapPtr& p, const String& sExpr)
{
  Array<String> as;
  //if (!IlwisObjectPtr::fParseParm(sExpr, as))
  //  ExpressionError(sExpr, sSyntax());
	short iParms = IlwisObjectPtr::iParseParm(sExpr, as);
  if (iParms > 3 || iParms < 2)
        ExpressionError(sExpr, sSyntax());  
	String sInputSegMapName = as[0];
	char *pCh = sInputSegMapName.strrchrQuoted('.');
  if ((pCh != 0) && (0 != _strcmpi(pCh, ".mps")))  // attrib map
		throw ErrorObject(WhatError(String(TR("Use of attribute maps is not possible: '%S'").c_str(), as[0]),
																 errSegmentMapTransform), fn);
  SegmentMap smp(as[0], fn.sPath());
  CoordSystem csy(as[1], fn.sPath());
	double rD = 0; // default no densifying
  if (iParms > 2)
    rD = max(as[2].rVal(), 0);  // default distance over-ruled by user
  return new SegmentMapTransform(fn, p, smp, csy, rD);
}

SegmentMapTransform::SegmentMapTransform(const FileName& fn, SegmentMapPtr& p)
: SegmentMapVirtual(fn, p)
{
  fNeedFreeze = true;
  String sColName;
  try {
    ReadElement("SegmentMapTransform", "SegmentMap", smp);
    ReadElement("SegmentMapTransform", "CoordSys", csy);
		ReadElement("SegmentMapTransform", "Distance", rDistance);
  }
  catch (const ErrorObject& err) {  // catch to prevent invalid object
    err.Show();
    return;
  }
  Init();
  objdep.Add(smp.ptr());
  objdep.Add(csy.ptr());
}

SegmentMapTransform::SegmentMapTransform(const FileName& fn, SegmentMapPtr& p, const SegmentMap& sm, 
																				 const CoordSystem& cs, const double rD)
: SegmentMapVirtual(fn, p, cs,cs->cb,sm->dvrs()), smp(sm), csy(cs), rDistance(rD)
{
  if (!cs->fConvertFrom(sm->cs()))
    IncompatibleCoordSystemsError(cs->sName(true, fnObj.sPath()), sm->cs()->sName(true, fnObj.sPath()), sTypeName(), errSegmentMapTransform+1);
  fNeedFreeze = true;

  SetCB(csy->cbConv(smp->cs(), smp->cb()));
  ptr.SetAlfa(cb());
  Init();
  objdep.Add(smp.ptr());
  objdep.Add(csy.ptr());
  if (!fnObj.fValid()) // 'inline' object
    objtime = objdep.tmNewest();
  if (smp->fTblAttSelf())
    SetAttributeTable(smp->tblAtt());
}

void SegmentMapTransform::Store()
{
  SegmentMapVirtual::Store();
  WriteElement("SegmentMapVirtual", "Type", "SegmentMapTransform");
  WriteElement("SegmentMapTransform", "SegmentMap", smp);
  WriteElement("SegmentMapTransform", "CoordSys", csy);
	WriteElement("SegmentMapTransform", "Distance", rDistance);
}

SegmentMapTransform::~SegmentMapTransform()
{
}

String SegmentMapTransform::sExpression() const
{
  return String("SegmentMapTransform(%S,%S,%.4f)", smp->sNameQuoted(false, fnObj.sPath()), 
                                              csy->sNameQuoted(false, fnObj.sPath()),
																							rDistance);

}

void SegmentMapTransform::Init()
{
  htpFreeze = "ilwisapp\\transform_segment_map_functionality_algorithm.htm";
  sFreezeTitle = "SegmentMapTransform";
}

bool SegmentMapTransform::fFreezing()
{
	trq.SetText(String(TR("Transforming '%S'").c_str(), sName(true, fnObj.sPath())));

	CoordinateSequence *seq;

	bool fDensify = false;
	if (rDistance > EPS10)
		fDensify = true;
	CoordSystem csOld = smp->cs();
	long iSeg = smp->iFeatures();
	for (int i = 0; i < iSeg; ++i) {
		CoordBuf crdBufOut,crdBuf;
		ILWIS::Segment *seg = (ILWIS::Segment *)smp->getFeature(i);
		if ( !seg || !seg->fValid())
			continue;
		if (trq.fUpdate(i, iSeg))
			return false;
		if (fDensify)	{
			seg->Densify(rDistance,crdBuf);
			seq = crdBuf.clone();
		}
		else {
			seq = seg->getCoordinates();	
		}
		for (long i = 0; i < seq->size(); ++i)
		{
			Coord crd = csy->cConv(csOld, seq->getAt(i));
			if ( !crd.fUndef()) 
				crdBufOut.add(crd) ; // transformation of coors to new csy
		} 
		if (seq->size() > 0) {
			ILWIS::Segment *segNew = CSEGMENT(pms->newFeature());
			segNew->PutCoords(crdBufOut.clone());
			segNew->PutVal(seg->rValue());
		}
		delete seq;
	}
	trq.fUpdate(iSeg, iSeg);
	return true;
}




