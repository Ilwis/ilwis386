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
/* PolygonMapLabels
   Copyright Ilwis System Development ITC
   may 1996, by Jelle Wind
	Last change:  JEL   6 May 97    5:58 pm
*/

#include "PolygonApplications\POLLABEL.H"
#include "Engine\Base\DataObjects\valrange.h"
#include "Engine\Domain\dmsort.h"
#include "Headers\Err\Ilwisapp.err"
#include "Headers\Htp\Ilwisapp.htp"
#include "Headers\Hs\polygon.hs"

IlwisObjectPtr * createPolygonMapLabels(const FileName& fn, IlwisObjectPtr& ptr, const String& sExpr, vector<void *> parms ) {
	if ( sExpr != "")
		return (IlwisObjectPtr *)PolygonMapLabels::create(fn, (PolygonMapPtr &)ptr, sExpr);
	else
		return (IlwisObjectPtr *)new PolygonMapLabels(fn, (PolygonMapPtr &)ptr);
}

const char* PolygonMapLabels::sSyntax() {
  return "PolygonMapLabels(polmap,pntmap)";
}

PolygonMapLabels* PolygonMapLabels::create(const FileName& fn, PolygonMapPtr& p, const String& sExpr)
{
  Array<String> as(2);
  if (!IlwisObjectPtr::fParseParm(sExpr, as))
    throw ErrorExpression(sExpr, sSyntax());
  PolygonMap polmp(as[0], fn.sPath());
  PointMap pntmp(as[1], fn.sPath());
  return new PolygonMapLabels(fn, p, polmp, pntmp);
}

PolygonMapLabels::PolygonMapLabels(const FileName& fn, PolygonMapPtr& p)
: PolygonMapVirtual(fn, p)
{
  fNeedFreeze = true;
  String sColName;
  try {
    ReadElement("PolygonMapLabels", "PolygonMap", polmap);
  }
  catch (const ErrorObject& err) {  // catch to prevent invalid object
    err.Show();
    return;
  }
  try {
    ReadElement("PolygonMapLabels", "PointMap", pntmap);
  }
  catch (const ErrorObject& err) {  // catch to prevent invalid object
    err.Show();
    return;
  }
  Init();
  objdep.Add(polmap.ptr());
  objdep.Add(pntmap.ptr());
  if (!fnObj.fValid())
    objtime = objdep.tmNewest();
}

PolygonMapLabels::PolygonMapLabels(const FileName& fn, PolygonMapPtr& p, const PolygonMap& polm, const PointMap& pntm)
: PolygonMapVirtual(fn, p, polm->cs(),polm->cb(),pntm->dvrs()), polmap(polm), pntmap(pntm)
{
  fNeedFreeze = true;
  Init();
  objdep.Add(polmap.ptr());
  objdep.Add(pntmap.ptr());
  if (!cs()->fConvertFrom(pntmap->cs()))
    IncompatibleCoordSystemsError(cs()->sName(true, fnObj.sPath()), pntmap->cs()->sName(true, fnObj.sPath()), sTypeName(), errPolygonMapLabels);
  if (pntm->fTblAttSelf())
    SetAttributeTable(pntm->tblAtt());
}

void PolygonMapLabels::Store()
{
  PolygonMapVirtual::Store();
  WriteElement("PolygonMapVirtual", "Type", "PolygonMapLabels");
  WriteElement("PolygonMapLabels", "PolygonMap", polmap);
  WriteElement("PolygonMapLabels", "PointMap", pntmap);
}

PolygonMapLabels::~PolygonMapLabels()
{
}

String PolygonMapLabels::sExpression() const
{
  return String("PolygonMapLabels(%S,%S)", polmap->sNameQuoted(false, fnObj.sPath()), pntmap->sNameQuoted(false, fnObj.sPath()));
}

void PolygonMapLabels::Init()
{
  htpFreeze = "ilwisapp\assign_labels_to_polygons_algoritm.htm";
  sFreezeTitle = "PolygonMapLabels";
}

bool PolygonMapLabels::fFreezing()
{
	trq.SetText(TR("Copying polygons"));
	bool fUseReals = dvrs().fUseReals();
	CoordSystem csOld = polmap->cs();
	bool fTransformCoords = cs() != csOld;
	long iPnt = pntmap->iFeatures();
	for (long i = 0; i < iPnt; ++i) {
		if (trq.fUpdate(i, iPnt))
			return false;
		Coord crdPnt = pntmap->cValue(i);
		if (fTransformCoords)
			crdPnt = cs()->cConv(csOld, crdPnt);
		vector<Geometry *> pols = polmap->getFeatures(crdPnt); // point in polygon
		for(int j = 0; j < pols.size(); ++j) {
			ILWIS::Polygon *pol = (ILWIS::Polygon *)pols[j];
			ILWIS::Polygon *newPol = (ILWIS::Polygon *)pol->clone();
			if (fUseReals)
				newPol->PutVal(pntmap->rValue(i));
			else
				newPol->PutVal(pntmap->iRaw(i));
			pms->addPolygon(newPol);
		}
  }

  return true;
} 




