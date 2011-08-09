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
// $Log: /ILWIS 3.0/PointMap/SEGMASK.cpp $
 * 
 * 6     8-02-01 17:13 Hendrikse
 * implem errormessage TR("Use of attribute maps is not possible: '%S'")
 * 
 * 5     17-01-00 8:17a Martin
 * changed rowcols to coords
 * 
 * 4     10-12-99 11:48a Martin
 * removed internal rowcols and replaced them by true coords
 * 
 * 3     9/08/99 12:02p Wind
 * comments
 * 
 * 2     9/08/99 10:26a Wind
 * adpated to use of quoted file names in sExpression()
*/
// Revision 1.3  1998/09/16 17:25:20  Wim
// 22beta2
//
// Revision 1.2  1997/08/04 15:36:21  Wim
// Improved use of mask. Use now VoronoiPoint::fInMask(mask)
//
/* PointMapRelate
   Copyright Ilwis System Development ITC
   march 1995, by Wim Koolhoven
	Last change:  WK    4 Aug 97    5:19 pm
*/                                                                      

#include "Engine\Map\Point\pnt.H"
#include "PointApplications\pntRelate.H"
#include "Engine\Map\basemap.h"
#include "Engine\Base\DataObjects\valrange.h"
#include "Headers\Err\Ilwisapp.err"
#include "Headers\Htp\Ilwisapp.htp"
#include "Headers\Hs\Point.hs"

IlwisObjectPtr * createPointMapRelate(const FileName& fn, IlwisObjectPtr& ptr, const String& sExpr, vector<void *> parms ) {
	if ( sExpr != "")
		return (IlwisObjectPtr *)PointMapRelate::create(fn, (PointMapPtr &)ptr, sExpr);
	else
		return (IlwisObjectPtr *)new PointMapRelate(fn, (PointMapPtr &)ptr);
}

const char* PointMapRelate::sSyntax() {
  return "PointMapRelate(inputmap, Touches|Intersect|Overlaps|Contains|Covers|CoveredBy|Equals|Within|Crosses|Disjoint)";
}

PointMapRelate* PointMapRelate::create(const FileName& fn, PointMapPtr& p, const String& sExpr)
{
	Array<String> as(4);
	if (!IlwisObjectPtr::fParseParm(sExpr, as))
		ExpressionError(sExpr, sSyntax());
	String sInputMapName1 = as[0];
	String sInputMapName2 = as[1];
	String type = as[2];
	String negation = as[3].toLower();
	if ( negation != "true" && negation !="false")
	   ExpressionError(sExpr, sSyntax());

	if ( type.substr(0,6) != "DE-9IM") {
		String types("touches@intersects@overlaps@contains@covers@coveredby@equals@within@crosses@disjoint");
		if ( types.find(type.toLower()) == string::npos) {
			ExpressionError(sExpr, sSyntax());
		}
	} else {
		String pattern=type.sTail("[");
		if ( pattern.size() !=10)
		   ExpressionError(sExpr, sSyntax());
	}
	
	FileName fnInput1(sInputMapName1);
	FileName fnInput2(sInputMapName2);


	return new PointMapRelate(fn, p, PointMap(fnInput1), BaseMap(fnInput2), type.toLower(),negation == "true");
}

PointMapRelate::PointMapRelate(const FileName& fn, PointMapPtr& p)
: PointMapVirtual(fn, p)
{
  fNeedFreeze = true;
  String sColName;
  try {
	  FileName fn;
    ReadElement("PointMapRelate", "InputMap1", fn);
	polmap = PointMap(fn);
    ReadElement("PointMapRelate", "InputMap2", fn);
	bmp = BaseMap(fn);

  }
  catch (const ErrorObject& err) {  // catch to prevent invalid object
    err.Show();
    return;
  }
     ReadElement("PointMapRelate", "Relation", rt);
     ReadElement("PointMapRelate", "Negation", fNegation);
  Init();
  objdep.Add(bmp.ptr());
}

PointMapRelate::PointMapRelate(const FileName& fn, PointMapPtr& p, const PointMap& pm, const BaseMap& bm, String _rt, bool n)
: PointMapVirtual(fn, p, pm->cs(),pm->cb(), pm->dm()), polmap(pm),bmp(bm), rt(_rt), fNegation(n)
{
  fNeedFreeze = true;
  Init();
  objdep.Add(bmp.ptr());
  if (!fnObj.fValid()) // 'inline' object
    objtime = objdep.tmNewest();
 
}


void PointMapRelate::Store()
{
  PointMapVirtual::Store();
  WriteElement("PointMapVirtual", "Type", "PointMapRelate");
  WriteElement("PointMapRelate", "InputMap1", polmap);
  WriteElement("PointMapRelate", "InputMap2", bmp);
  WriteElement("PointMapRelate", "Relation", rt);
  WriteElement("PointMapRelate", "Negation", fNegation);
}

PointMapRelate::~PointMapRelate()
{
}

String PointMapRelate::sExpression() const
{
	String n = fNegation ? "true" : "false";
	return String("PointMapRelate(%S,%S,%S,%S)", polmap->sNameQuoted(true, fnObj.sPath()), bmp->sNameQuoted(true, fnObj.sPath()), rt,n);
}

bool PointMapRelate::fDomainChangeable() const
{
  return false;
}

void PointMapRelate::Init()
{
  //htpFreeze = htpPointMapRelateT;
  sFreezeTitle = "PointMapRelate";
}

bool PointMapRelate::fFreezing()
{
	trq.SetText("adding features" );

	for (int i=0; i <  polmap->iFeatures(); ++i ) {
		ILWIS::Point *pnt = CPOINT(polmap->getFeature(i));
		if ( pnt == NULL)
			continue;
		for(int j = 0; j < bmp->iFeatures(); ++j) {
			Geometry *g = bmp->getFeature(j);
			if ( !g || g->isEmpty())
				continue;
			if ( fNegation) {
			   if( !Relate(pnt,g,rt))
				 addPoint(pnt);
			} else if( Relate(pnt,g,rt))
				 addPoint(pnt);
		}
	}
		
	trq.fUpdate( polmap->iFeatures(),  polmap->iFeatures());
	return true;
}

bool PointMapRelate::Relate(Geometry *g1, Geometry* g2, const String& rt) {
	if ( rt == "touches" && g1->touches(g2)) {
		return true;
	} else if ( rt == "intersects" && g1->intersects(g2)) {
		return true;
	} else if ( rt == "overlaps" && g1->overlaps(g2)) {
		return true;
	} else if ( rt == "contains" && g1->contains(g2)) {
		return true;
	} else if ( rt == "covers" && g1->covers(g2)) {
		return true;
	} else if ( rt == "equals" && g1->equals(g2)) {
		return true;
	} else if ( rt == "within" && g1->within(g2)) {
		return true;
	} else if ( rt == "coveredby" && g1->coveredBy(g2)) {
		return true;
	}else if ( rt == "crosses" && g1->crosses(g2)) {
		return true;
	} else if ( rt == "disjoint" && g1->disjoint(g2)) {
		return true;
	} else if  (rt.substr(0,6) == "de-9im") {
		String pattern = rt.sTail("[").sHead("]");
		if (g1->relate(g2,pattern))
			return true;
	}
	return false;

}

void PointMapRelate::addPoint(ILWIS::Point *p) {
	ILWIS::Point *pNew = CPOINT(ptr.newFeature(p));
	pNew->PutVal(p->rValue());
}







