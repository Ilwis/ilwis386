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
/* SegmentMapFromPoints
Copyright Ilwis System Development ITC
march 1995, by Wim Koolhoven
Last change:  JEL  13 May 97   11:53 am
*/
#include  "Headers\toolspch.h"
#include "Engine\Base\DataObjects\valrange.h"
#include "Engine\Map\Point\PNT.H"
#include "Engine\Domain\DomainUniqueID.h"
#include "SegmentApplications\SegmentMapFromPoints.H"
#include "Engine\Domain\dmsort.h"
#include "Headers\Err\Ilwisapp.err"
#include "Headers\Htp\Ilwisapp.htp"
#include "Headers\Hs\segment.hs"

IlwisObjectPtr * createSegmentMapFromPoints(const FileName& fn, IlwisObjectPtr& ptr, const String& sExpr, vector<void *> parms ) {
	if ( sExpr != "")
		return (IlwisObjectPtr *)SegmentMapFromPoints::create(fn, (SegmentMapPtr &)ptr, sExpr);
	else{
		return (IlwisObjectPtr *)new SegmentMapFromPoints(fn, (SegmentMapPtr &)ptr);
	}
}

const char* SegmentMapFromPoints::sSyntax() {
	return "SegmentMapFromPoints(pointmap[,ident[,order])\n";
}

SegmentMapFromPoints* SegmentMapFromPoints::create(const FileName& fn, SegmentMapPtr& p, const String& sExpr)
{
	Array<String> as;
	if (IlwisObjectPtr::iParseParm(sExpr, as) > 3 )
		ExpressionError(sExpr, sSyntax());
	PointMap smp(as[0], fn.sPath());
	String identC, orderC;
	if ( as.size() == 2)
		identC = as[1];
	if ( as.size() == 3) {
		orderC = as[2];
	}

	return new SegmentMapFromPoints(fn, p, smp, identC, orderC);
}

SegmentMapFromPoints::SegmentMapFromPoints(const FileName& fn, SegmentMapPtr& p)
: SegmentMapVirtual(fn, p)
{
	fNeedFreeze = true;
	String sColName;
	try {
		ReadElement("SegmentMapFromPoints", "PointMap", pmIn);
		if ( pmIn->fTblAtt()) {
			ReadElement("SegmentMapFromPoints", "IdentityColumn", sColName);
			if ( sColName != "")
				colIdent = pmIn->tblAtt()->col(sColName);
			sColName = "";
			ReadElement("SegmentMapFromPoints", "OrderColumn", sColName);
			if ( sColName != "") 
				colOrder  = pmIn->tblAtt()->col(sColName);
		}

	} catch (const ErrorObject& err) {  // catch to prevent invalid object
		err.Show();
		return;
	}
	Init();
	objdep.Add(pmIn.ptr());
	objdep.Add(colIdent.ptr());
}

SegmentMapFromPoints::SegmentMapFromPoints(const FileName& fn, SegmentMapPtr& p, const PointMap& pmpIn, 
										   const String& sIdentName, const String& sOrderName)
										   : SegmentMapVirtual(fn, p, pmpIn->cs(),pmpIn->cb(),pmpIn->dvrs()), pmIn(pmpIn)
{
	fNeedFreeze = true;
	if ( pmpIn->fTblAtt()) {
		if ( sIdentName != "")
			colIdent = pmpIn->tblAtt()->col(sIdentName);
		if ( sOrderName != "")
			colOrder = pmIn->tblAtt()->col(sOrderName);
	}
	Domain dom(p.fnObj, 0, dmtUNIQUEID, "seg");
	SetDomainValueRangeStruct(DomainValueRangeStruct(dom));
	Init();
	objdep.Add(pmIn.ptr());
	objdep.Add(colIdent.ptr());
	if (!fnObj.fValid()) // 'inline' object
		objtime = objdep.tmNewest();
}

void SegmentMapFromPoints::Store()
{
	SegmentMapVirtual::Store();
	WriteElement("SegmentMapVirtual", "Type", "SegmentMapFromPoints");
	WriteElement("SegmentMapFromPoints", "PointMap", pmIn);
	if ( colIdent.fValid())
		WriteElement("SegmentMapFromPoints", "IdentityColumn", colIdent);
	if ( colOrder.fValid())
		WriteElement("SegmentMapFromPoints", "OrderColumn", colOrder);
}

SegmentMapFromPoints::~SegmentMapFromPoints()
{
}

String SegmentMapFromPoints::sExpression() const
{
	if ( colIdent.fValid() && !colOrder.fValid())
		return String("SegmentMapFromPoints(%S,%S)", pmIn->sNameQuoted(false, fnObj.sPath()), colIdent->sNameQuoted());
	if ( !colIdent.fValid() && !colOrder.fValid())
		return String("SegmentMapFromPoints(%S)", pmIn->sNameQuoted(false, fnObj.sPath()));
	if ( !colIdent.fValid() && colOrder.fValid())
		return String("SegmentMapFromPoints(%S,,%S)", pmIn->sNameQuoted(false, fnObj.sPath()), colOrder->sNameQuoted());
	if ( colIdent.fValid() && colOrder.fValid())
		return String("SegmentMapFromPoints(%S,$S,%S)", pmIn->sNameQuoted(false, fnObj.sPath()), colIdent->sNameQuoted(), colOrder->sNameQuoted());
	return sUNDEF;
}

bool SegmentMapFromPoints::fDomainChangeable() const
{
	return false;
}
/*
long SegmentMapFromPoints::iFeatures() const
{
return smp->iFeatures();
}
*/
void SegmentMapFromPoints::Init()
{

}

// offset of rec == 1 !!
double SegmentMapFromPoints::getValue(int rec) const{
	if ( colIdent.fValid()) {
		if ( colIdent->fUseReals()) {
			return colIdent->rValue(rec);
		} else
			return colIdent->iRaw(rec);
	} else
		return pmIn->rValue(rec - 1);

	return rUNDEF;
}

void  SegmentMapFromPoints::makeOrder(vector<long>& order) const{
	if ( colOrder.fValid()) {
		map<long, long> orderMap;
		for(long i = 1; i <= colOrder->iRecs(); ++i) {
			long ord = colOrder->iValue(i);
			if ( ord != iUNDEF)
				orderMap[ord] = i;

		}
		for(map<long,long>::const_iterator iter = orderMap.begin(); iter != orderMap.end(); ++iter) {
			order.push_back((*iter).second);
		}
	} else {
		for(long i = 1; i <= pmIn->iFeatures(); ++i)
			order.push_back(i);
	}
}

void SegmentMapFromPoints::addSegment(CoordinateSequence* seq) {
	ILWIS::Segment *seg =  CSEGMENT(ptr.newFeature());
	seg->PutCoords(seq);
	seg->PutVal(ptr.iFeatures());
}

bool SegmentMapFromPoints::fFreezing()
{
	trq.SetText(String(TR("Calculating '%S'").c_str(), sName(true, fnObj.sPath())));
	ILWIS::Segment *seg = 0;
	CoordinateSequence *seq = 0;
	double vOld = rUNDEF;
	vector<long> order;
	makeOrder(order);
	for(int rec= 0; rec < order.size(); ++rec) {
		trq.fUpdate(rec, order.size());
		double v = getValue(order[rec]);
		if ( v == iUNDEF || v == rUNDEF)
			continue;
		if ( v != vOld || vOld == rUNDEF) {
			dm()->pdsrt()->pdUniqueID()->iAdd();
			if ( seq) {
				if ( seq->size() > 1) {
					addSegment(seq);
				} else
					delete seq;
			}
			seq = new CoordinateArraySequence();
			vOld = v;
		}
		Coord crd = pmIn->cValue(order[rec] - 1);
		seq->add(crd,false);
	}
	if ( seq) {
		addSegment(seq);
	}

	trq.fUpdate(0, 0);
	return true;
}




