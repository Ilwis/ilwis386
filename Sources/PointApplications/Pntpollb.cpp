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
/* PointMapPolLabels
   Copyright Ilwis System Development ITC
   jan 1996  by Jan Hendrikse
	Last change:  WK    5 Jun 98   11:04 am
*/
#include "PointApplications\Pntpollb.h"
#include "Engine\Map\Point\PNTSTORE.H"
#include "Engine\Base\DataObjects\valrange.h"
#include "Engine\Domain\Dmvalue.h"
#include "Headers\Htp\Ilwisapp.htp"
#include "Headers\Err\Ilwisapp.err"
#include "Headers\Hs\point.hs"


IlwisObjectPtr * createPointMapPolLabels(const FileName& fn, IlwisObjectPtr& ptr, const String& sExpr, vector<void *> parms ) {
	if ( sExpr != "")
		return (IlwisObjectPtr *)PointMapPolLabels::create(fn, (PointMapPtr &)ptr, sExpr);
	else
		return (IlwisObjectPtr *)new PointMapPolLabels(fn, (PointMapPtr &)ptr);
}

const char* PointMapPolLabels::sSyntax() {
  return "PointMapPolLabels(pntmap)\n"
         "PointMapPolLabels(pntmap,alsoundefs)\n";
}

PointMapPolLabels* PointMapPolLabels::create(const FileName& fn, PointMapPtr& p, const String& sExpr)
{
  Array<String> as;
  int iParms = IlwisObjectPtr::iParseParm(sExpr, as);
  bool fOk = false;
  bool fIncludeUndefs = false;
  if (1 == iParms)
    fOk = true;
  else if (2 == iParms && fCIStrEqual("alsoundefs", as[1])) {
    fIncludeUndefs = true;
    fOk = true;
  }
  if (!fOk)
    ExpressionError(sExpr, sSyntax());
  PolygonMap pmp(as[0], fn.sPath());
  return new PointMapPolLabels(fn, p, pmp, fIncludeUndefs);
}

PointMapPolLabels::PointMapPolLabels(const FileName& fn, PointMapPtr& p)
: PointMapVirtual(fn, p)
{
  String sType;
  ReadElement("PointMapPolLabels", "PolygonMap", polmp);
  fAlsoUndefs = false;
  ReadElement("PointMapPolLabels", "AlsoUndefs", fAlsoUndefs);
  fNeedFreeze = true;
  Init();
  objdep.Add(polmp.ptr());
}

PointMapPolLabels::~PointMapPolLabels()
{
}

PointMapPolLabels::PointMapPolLabels(const FileName& fn, PointMapPtr& p,
		   const PolygonMap& pmap, bool fIncludeUndefs)
: PointMapVirtual(fn, p, pmap->cs(), pmap->cb(), pmap->dvrs()),
  polmp(pmap), fAlsoUndefs(fIncludeUndefs)
{
  fNeedFreeze = true;
  Init();
  objdep.Add(polmp.ptr());
  if (!fnObj.fValid())
    objtime = objdep.tmNewest();
  if (polmp->fTblAttSelf())
    SetAttributeTable(polmp->tblAtt());
}

void PointMapPolLabels::Store()
{
  PointMapVirtual::Store();
  WriteElement("PointMapVirtual", "Type", "PointMapPolLabels");
  WriteElement("PointMapPolLabels", "PolygonMap", polmp);
  WriteElement("PointMapPolLabels", "AlsoUndefs", fAlsoUndefs);
}

String PointMapPolLabels::sExpression() const
{
  String sPntName = polmp->sNameQuoted(true, fnObj.sPath());
  return String("PointMapPolLabels(%S%s)",
                sPntName,
                fAlsoUndefs ? ",alsoundefs" : "");
}

void PointMapPolLabels::Init()
{
  htpFreeze = "ilwisapp\\polygons_to_points_algorithm.htm";
  sFreezeTitle = "PointMapPolLabels";
}

bool PointMapPolLabels::fFreezing()
{
  Init();
  trq.SetText(SPNTTextCalculating);
  for (int i = 0; i < polmp->iFeatures(); ++i) {
	ILWIS::Polygon *pol = (ILWIS::Polygon *)polmp->getFeature(i);
	if ( pol == NULL || !pol->fValid())
		continue;
    if ( (trq.fUpdate(i, polmp->iFeatures()) ))
      return false;
      if (fUseReals()) {
        double r = pol->rValue();
	if (r != rUNDEF || (fAlsoUndefs && pol->rArea() > 0))
 //         pms->iAddVal(polmp->smp()->cConv(pol.rcFindPointInPol()),r);
          pms->iAddVal(pol->crdFindPointInPol(),r);
    }  
    else {
      long iRaw = pol->iValue();
      if (iRaw != iUNDEF || (fAlsoUndefs && pol->rArea() > 0))
 //       pms->iAddRaw(polmp->smp()->cConv(pol.rcFindPointInPol()), iRaw);
			pms->iAddRaw(pol->crdFindPointInPol(), iRaw);
    }  
  }
  _iPoints = pms->iPnt();
  return true;
}




