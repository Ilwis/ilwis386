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
// $Log: /ILWIS 3.0/PointMap/PNTMASK.cpp $
 * 
 * 3     9/08/99 11:59a Wind
 * comments
 * 
 * 2     9/08/99 10:21a Wind
 * adpated to use of quoted file names
*/
// Revision 1.3  1998/09/16 17:26:27  Wim
// 22beta2
//
// Revision 1.2  1997/08/04 15:46:13  Wim
// Improved using of masks. Use now PointMap::fPntInMask()
//
/* PointMapMask
   Copyright Ilwis System Development ITC
   oct. 1996, by Jelle Wind
	Last change:  WK    4 Aug 97    5:44 pm
*/

#include "PointApplications\PNTMASK.H"
#include "Engine\Map\Point\PNTSTORE.H"
#include "Engine\Base\DataObjects\valrange.h"
#include "Headers\Err\Ilwisapp.err"
#include "Headers\Htp\Ilwisapp.htp"
#include "Headers\Hs\point.hs"

IlwisObjectPtr * createPointMapMask(const FileName& fn, IlwisObjectPtr& ptr, const String& sExpr, vector<void *> parms ) {
	if ( sExpr != "")
		return (IlwisObjectPtr *)PointMapMask::create(fn, (PointMapPtr &)ptr, sExpr);
	else
		return (IlwisObjectPtr *)new PointMapMask(fn, (PointMapPtr &)ptr);
}

const char* PointMapMask::sSyntax() {
  return "PointMapMask(pntmap,\"mask\")";
}

PointMapMask* PointMapMask::create(const FileName& fn, PointMapPtr& p, const String& sExpr)
{
  Array<String> as(2);
  if (!IlwisObjectPtr::fParseParm(sExpr, as))
    throw ErrorExpression(sExpr, sSyntax());
  PointMap pmp(as[0], fn.sPath());
  return new PointMapMask(fn, p, pmp, as[1]);
}

PointMapMask::PointMapMask(const FileName& fn, PointMapPtr& p)
: PointMapVirtual(fn, p)
{
  fNeedFreeze = true;
  String sColName;
  try {
    ReadElement("PointMapMask", "PointMap", pmp);
  }
  catch (const ErrorObject& err) {  // catch to prevent invalid object
    err.Show();
    return;
  }
  String sMask;
  ReadElement("PointMapMask", "Mask", sMask);
  mask.SetMask(sMask);
  Init();
  objdep.Add(pmp.ptr());
}

PointMapMask::PointMapMask(const FileName& fn, PointMapPtr& p, const PointMap& pm, 
                               const String& sMsk)
: PointMapVirtual(fn, p, pm->cs(),pm->cb(),pm->dvrs()), pmp(pm)
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

void PointMapMask::Store()
{
  PointMapVirtual::Store();
  WriteElement("PointMapVirtual", "Type", "PointMapMask");
  WriteElement("PointMapMask", "PointMap", pmp);
  WriteElement("PointMapMask", "Mask",  mask.sMask());
}

PointMapMask::~PointMapMask()
{
}

String PointMapMask::sExpression() const
{
  return String("PointMapMask(%S,\"%S\")", pmp->sNameQuoted(false, fnObj.sPath()), mask.sMask());
}

bool PointMapMask::fDomainChangeable() const
{
  return false;
}

void PointMapMask::Init()
{
  htpFreeze = "ilwisapp\\mask_points_algorithm.htm";
  sFreezeTitle = "PointMapMask";
}


bool PointMapMask::fFreezing()
{
  mask.SetDomain(dm());
  trq.SetText(String(SPNTTextCopyWithMask_S.scVal(), mask.sMask()));
  long iPoints = pmp->iFeatures();
  for (long i=0; i < iPoints; ++i) {
    if (trq.fUpdate(i, iPoints))
      return false;
    if (!pmp->fPntInMask(i, mask))
      continue;
    String sValue = pmp->sValue(i,0);
    pms->iAddVal(pmp->cValue(i), sValue);
  }
  trq.fUpdate(iPoints, iPoints);
  _iPoints = pms->iPnt();
  return true;
}  




