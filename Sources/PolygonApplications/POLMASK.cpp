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
// $Log: /ILWIS 3.0/PolygonMap/POLMASK.cpp $
 * 
 * 6     8-02-01 17:10 Hendrikse
 * implem errormessage SPOLErrNoAttColumnAllowed_S 
 * 
 * 5     17-01-00 8:17a Martin
 * changed rowcols to coords
 * 
 * 4     10-12-99 3:14p Martin
 * removed internal coordinates and used true coords
 * 
 * 3     9/08/99 11:57a Wind
 * comments
 * 
 * 2     9/08/99 10:22a Wind
 * adpated to use of quoted file names
*/
// Revision 1.3  1998/09/16 17:25:54  Wim
// 22beta2
//
// Revision 1.2  1997/08/04 15:42:36  Wim
// Improved use of masks
//
/* PolygonMapMask
   Copyright Ilwis System Development ITC
   oct. 1996, by Jelle Wind
	Last change:  WK    4 Aug 97    5:42 pm
*/

#include "PolygonApplications\POLMASK.H"
#include "Engine\Base\DataObjects\valrange.h"
#include "Headers\Err\Ilwisapp.err"
#include "Headers\Htp\Ilwisapp.htp"
#include "Headers\Hs\polygon.hs"

IlwisObjectPtr * createPolygonMapMask(const FileName& fn, IlwisObjectPtr& ptr, const String& sExpr, vector<void *> parms ) {
	if ( sExpr != "")
		return (IlwisObjectPtr *)PolygonMapMask::create(fn, (PolygonMapPtr &)ptr, sExpr);
	else
		return (IlwisObjectPtr *)new PolygonMapMask(fn, (PolygonMapPtr &)ptr);
}

const char* PolygonMapMask::sSyntax() {
  return "PolygonMapMask(polmap,\"mask\")";
}

PolygonMapMask* PolygonMapMask::create(const FileName& fn, PolygonMapPtr& p, const String& sExpr)
{
  Array<String> as(2);
  if (!IlwisObjectPtr::fParseParm(sExpr, as))
    throw ErrorExpression(sExpr, sSyntax());
	String sInputPolMapName = as[0];
	char *pCh = sInputPolMapName.strrchrQuoted('.');
  if ((pCh != 0) && (0 != _strcmpi(pCh, ".mpa")))  // attrib map
		throw ErrorObject(WhatError(String(SPOLErrNoAttColumnAllowed_S.scVal(), as[0]),
																 errPolygonMapMask), fn);
  PolygonMap pmp(as[0], fn.sPath());
  return new PolygonMapMask(fn, p, pmp, as[1]);
}

PolygonMapMask::PolygonMapMask(const FileName& fn, PolygonMapPtr& p)
: PolygonMapVirtual(fn, p)
{
  fNeedFreeze = true;
  String sColName;
  try {
    ReadElement("PolygonMapMask", "PolygonMap", pmp);
  }
  catch (const ErrorObject& err) {  // catch to prevent invalid object
    err.Show();
    return;
  }
  String sMask;
  ReadElement("PolygonMapMask", "Mask", sMask);
  mask.SetMask(sMask);
  Init();
  objdep.Add(pmp.ptr());
}

PolygonMapMask::PolygonMapMask(const FileName& fn, PolygonMapPtr& p, const PolygonMap& pm, 
                               const String& sMsk)
: PolygonMapVirtual(fn, p, pm->cs(),pm->cb(),pm->dvrs()), pmp(pm)
{
  mask.SetMask(sMsk);
  fNeedFreeze = true;
  Init();
  objdep.Add(pmp.ptr());
  if (!fnObj.fValid()) // 'inline' object
    objtime = objdep.tmNewest();
  if (pmp->fTblAttSelf())
    SetAttributeTable(pmp->tblAtt());
}

void PolygonMapMask::Store()
{
  PolygonMapVirtual::Store();
  WriteElement("PolygonMapVirtual", "Type", "PolygonMapMask");
  WriteElement("PolygonMapMask", "PolygonMap", pmp);
  WriteElement("PolygonMapMask", "Mask", mask.sMask());
}

PolygonMapMask::~PolygonMapMask()
{
}

String PolygonMapMask::sExpression() const
{
  return String("PolygonMapMask(%S,\"%S\")", pmp->sNameQuoted(false, fnObj.sPath()), mask.sMask());
}

bool PolygonMapMask::fDomainChangeable() const
{
  return false;
}

void PolygonMapMask::Init()
{
  htpFreeze = htpPolygonMapMaskT;
  sFreezeTitle = "PolygonMapMask";
}


bool PolygonMapMask::fFreezing()
{
	mask.SetDomain(dm());
	trq.SetText(SPOLTextCopyPolygons);
	for(int i=0; i < pmp->iFeatures(); ++i) {
		if (trq.fUpdate(i, pmp->iFeatures()))
			return false;
		ILWIS::Polygon *pol = (ILWIS::Polygon *)pmp->getFeature(i);
		if (!pol || !pol->fValid())
			continue;
		if (!pol->fInMask(dvrs(), mask))
			continue;
		ILWIS::Polygon *newPol = (ILWIS::Polygon *)pol->clone();
		pms->addPolygon(newPol);
	}
	return true;
}  




