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
/* TableNeighbourPol
   Copyright Ilwis System Development ITC
   april 1995, by Jelle Wind
	Last change:  WK    1 Jul 98   10:01 am
*/
#define ILWMNEIGHPOL_C

#include "Applications\Table\Neighpol.h"
#include "Engine\Table\tblstore.h"
#include "Engine\Table\COLSTORE.H"
#include "Engine\Map\Polygon\POL.H"
#include "Engine\Base\Algorithm\Qsort.h"
#include "Headers\Htp\Ilwisapp.htp"
#include "Headers\Err\Ilwisapp.err"
#include "Headers\Hs\tbl.hs"

const char* TableNeighbourPol::sSyntax()
{
  return "TableNeighbourPol(polmap)";
}

IlwisObjectPtr * createTableNeighbourPol(const FileName& fn, IlwisObjectPtr& ptr, const String& sExpr, vector<void *> parms ) {
	if ( sExpr != "")
		return (IlwisObjectPtr *)TableNeighbourPol::create(fn, (TablePtr &)ptr, sExpr);
	else
		return (IlwisObjectPtr *)new TableNeighbourPol(fn, (TablePtr &)ptr);
}

TableNeighbourPol* TableNeighbourPol::create(const FileName& fn, TablePtr& p, const String& sExpr)
{
  Array<String> as(1);
  if (!IlwisObjectPtr::fParseParm(sExpr, as))
    throw ErrorExpression(sExpr, sSyntax());
  PolygonMap pmp(as[0], fn.sPath());

	if (!pmp->fTopologicalMap())
		throw ErrorObject(WhatError(STBLErrNonTopoMap, errTableNeighbourPol+1), fn);

  return new TableNeighbourPol(fn, p, pmp);
}

TableNeighbourPol::TableNeighbourPol(const FileName& fn, TablePtr& p)
: TableVirtual(fn, p)
{
  fNeedFreeze = true;
  ReadElement("TableNeighbourPol", "PolygonMap", pmp);
  Init();
  objdep.Add(pmp.ptr());
}

TableNeighbourPol::TableNeighbourPol(const FileName& fn, TablePtr& p, const PolygonMap& pmap)
: TableVirtual(fn, p, true), //FileName(fn, ".TB#", true), Domain("none")),
  pmp(pmap)
{
  fNeedFreeze = true;
  if (!pmp->dm()->pdsrt())
    throw ErrorSortDomain(pmp->dm()->sName(true, fnObj.sPath()), pmp->sTypeName(), 
                          errTableNeighbourPol);
  objdep.Add(pmp.ptr());
  if (!fnObj.fValid())
    objtime = objdep.tmNewest();
  ptr.SetDomain(Domain("none"));
}

TableNeighbourPol::~TableNeighbourPol()
{
}

void TableNeighbourPol::Store()
{
  TableVirtual::Store();
  WriteElement("TableVirtual", "Type", "TableNeighbourPol");
  WriteElement("TableNeighbourPol", "PolygonMap", pmp);
}


struct recordInfo {
	long name1;
	long name2;
	long segName;
	double length;
	
};
bool TableNeighbourPol::fFreezing()
{
  Init();
  trq.SetTitle(sFreezeTitle);
  trq.SetText(STBLTextGetNeigbours);
  long iPol = pmp->iFeatures();
  long iPol2 = pmp->iFeatures() * pmp->iFeatures();
  vector<recordInfo> output;
  vector<vector<bool>> combos;
  combos.resize(iPol);
  for(int i = 0; i < combos.size(); ++i) {
	  combos[i].resize(iPol);
  }

  for(int i =0; i < iPol; ++i) {
	   for(int j =0; j < iPol; ++j) {
		   combos[i][j] = false;
	   }
  }

  for(int i =0; i < iPol; ++i) {
	  ILWIS::Polygon *pol1 = CPOLYGON(pmp->getFeature(i));
	  if ( !pol1 || !pol1->fValid())  continue;

	  long big = iPol*i;
	  for(int j =0; j < iPol; ++j) {
		  trq.fUpdate(i * j, iPol2);
		  if ( combos[i][j] || combos[j][i] || i==j)
			  continue;
		
		  ILWIS::Polygon *pol2 = CPOLYGON(pmp->getFeature(j));
		  if ( !pol2 || !pol2->fValid()) continue;

		  if ( pol1->touches(pol2)) {
			  Geometry *g = pol1->intersection(pol2);
			  double l = g->getLength();
			  recordInfo rec;
			  rec.length = l;
			  rec.name1 = pol1->iValue();
			  rec.name2 = pol2->iValue();
			  rec.segName = i + j *i;
			  output.push_back(rec);
		  }
		  combos[i][j] = true;
	  }
  }
  ptr.iRecNew((long)output.size());
  for(int i=0; i<output.size(); ++i) {
	  colPolName1->PutRaw(i,output[i].name1);
	  colPolName2->PutRaw(i,output[i].name2);
	  colLength->PutVal(i,output[i].length);
  }
  return true;
}

void TableNeighbourPol::Init()
{
  sFreezeTitle = "TableNeighbourPol";
  htpFreeze = "ilwisapp\\neighbour_polygons_algorithm.htm";
  if (pts==0)
    return;  

  colPolName1 = pts->col("PolName1");
  if (!colPolName1.fValid()) {
    colPolName1 = pts->colNew("PolName1", pmp->dm());

  }
  colPolName1->SetOwnedByTable(true);
  colPolName1->SetReadOnly(true);
  colPolName2 = pts->col("PolName2");
  if (!colPolName2.fValid()) {
    colPolName2 = pts->colNew("PolName2", pmp->dm());

  }
  colPolName2->SetOwnedByTable(true);
  colPolName2->SetReadOnly(true);
  colLength= pts->col("Length");
  if (!colLength.fValid()) {
    colLength = pts->colNew("Length", Domain("value"));

  }
  colLength->SetOwnedByTable(true);
  colLength->SetReadOnly(true);
}

String TableNeighbourPol::sExpression() const
{
   return String("TableNeighbourPol(%S)", pmp->sNameQuoted(true, fnObj.sPath()));
}




