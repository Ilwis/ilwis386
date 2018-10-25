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
                                                              

//#include <geos/algorithm/Union.h>
#include "Engine\Map\Point\PNT.H"
#include "Engine\Map\Point\PNTSTORE.H"
#include "Engine\Map\Polygon\POL.H"
#include "Engine\Map\Raster\MapList\maplist.h"
#include "PointApplications\PointMapCross.H"
#include "Engine\Map\basemap.h"
#include "Engine\Base\System\LOGGER.H"
#include "Engine\Base\System\Engine.h"
#include "Engine\Base\DataObjects\valrange.h"
#include "Headers\Err\Ilwisapp.err"
#include "Headers\Htp\Ilwisapp.htp"
#include "Headers\Hs\Point.hs"

IlwisObjectPtr * createPointMapCross(const FileName& fn, IlwisObjectPtr& ptr, const String& sExpr, vector<void *> parms ) {
	if ( sExpr != "")
		return (IlwisObjectPtr *)PointMapCross::create(fn, (PointMapPtr &)ptr, sExpr);
	else
		return (IlwisObjectPtr *)new PointMapCross(fn, (PointMapPtr &)ptr);
}

const char* PointMapCross::sSyntax() {
  return "PointMapCross(pointmap, inputmaplist)";
}

PointMapCross* PointMapCross::create(const FileName& fn, PointMapPtr& p, const String& sExpr)
{
	Array<String> as(2);
	if (!IlwisObjectPtr::fParseParm(sExpr, as))
		ExpressionError(sExpr, sSyntax());
	String sInputMapList = as[1];
	FileName fnInput(sInputMapList);
	MapList mlist(fnInput);
	String sInputPointMap(as[0]);
	FileName fnPmap(sInputPointMap);
	PointMap pmap(fnPmap);
    return new PointMapCross(fn, p, pmap, mlist);
}

PointMapCross::PointMapCross(const FileName& fn, PointMapPtr& p)
: PointMapVirtual(fn, p)
{
  fNeedFreeze = true;
  String sColName;
  try {
	FileName fn;
    ReadElement("PointMapCross", "InputMapList", fn);
	maps = MapList(fn);
	ReadElement("PointMapCross", "PointMap", fn);
	pointmap = PointMap(fn);
  }
  catch (const ErrorObject& err) {  // catch to prevent invalid object
    err.Show();
    return;
  }
  Init();
  objdep.Add(maps.ptr());
  objdep.Add(pointmap.ptr());
}

PointMapCross::PointMapCross(const FileName& fn, PointMapPtr& p, const PointMap& pmap, const MapList& mlist)
: PointMapVirtual(fn, p, pmap->cs(), pmap->cb(), pmap->dm()), 
	maps(mlist),
	pointmap(pmap)
{
  fNeedFreeze = true;
  Init();
  objdep.Add(maps);
  if (!fnObj.fValid()) // 'inline' object
    objtime = objdep.tmNewest();
  FileName fnTable(ptr.fnObj,".tbt");
   Table tbl;
  if ( pmap->fTblAtt()) {
	Table tblatt = pmap->tblAtt();
	String cmd("copy %S %S;", tblatt->fnObj.sRelativeQuoted(), fnTable.sRelativeQuoted());
	getEngine()->Execute(cmd);
	tbl = Table(fnTable);
  } else {
    tbl = Table(fnTable, ptr.dm());
  }
  
  for(int i=0; i < mlist->iSize(); ++i) {
	  Map mp = mlist->map(i);
	  Column col(tbl,mp->fnObj.sFile,mp->dvrs());
	  tbl->AddCol(col);
  }
  SetAttributeTable(tbl);
 
}


void PointMapCross::Store()
{
  PointMapVirtual::Store();
  WriteElement("PointMapVirtual", "Type", "PointMapCross");
  WriteElement("PointMapCross", "InputMapList", maps->fnObj);
  WriteElement("PointMapCross", "PointMap", pointmap->fnObj);
}

PointMapCross::~PointMapCross()
{
}

String PointMapCross::sExpression() const
{
	return String("PointMapCross(%S,%S)", pointmap->fnObj.sRelativeQuoted(false), maps->fnObj.sRelativeQuoted(false));
}

bool PointMapCross::fDomainChangeable() const
{
  return false;
}

void PointMapCross::Init()
{
  //htpFreeze = htpPointMapCrossT;
  sFreezeTitle = "PointMapCross";
}

bool PointMapCross::fFreezing()
{
	trq.SetText("Crossing of Pointmap with a maplist" );
	Table tblAtt = ptr.tblAtt();
	if ( maps->iSize() == 0 || pointmap->iFeatures() == 0)
		return true;

	bool fCoordConvert = maps->map(0)->cs() != pointmap->cs();
	if ( tblAtt->iRecs() < pointmap->iFeatures())
		tblAtt->iRecNew(pointmap->iFeatures() - tblAtt->iRecs());

	for(int f = 0; f < pointmap->iFeatures(); ++f) {
		ILWIS::Point *p = CPOINT(pointmap->getFeature(f));
		if ( !p  || !p->fValid())
			continue;
		ILWIS::Point *newPoint = CPOINT(ptr.newFeature(p));
		newPoint->PutVal(p->rValue());
		Coord c(*(newPoint->getCoordinate()));
		if (fCoordConvert)
			c = maps->map(0)->cs()->cConv(pointmap->cs(), c);
		for(int i=0; i < maps->iSize(); ++i) {
			tblAtt->Updated();
			Map mp = maps->map(i);
			Column col = tblAtt->col(mp->fnObj.sFile);
			double rVal = mp->rValue(c);
			col->PutVal(f + 1,rVal);
		}
	}
	if ( tblAtt->fChanged)
		tblAtt->Store();
	return true;
}









