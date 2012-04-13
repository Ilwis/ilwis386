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
// ColumnAttributeFromMapList.cpp: implementation of the ColumnAttributeFromMapList class.
//
//////////////////////////////////////////////////////////////////////

#include "headers\toolspch.h"
#include "Engine\Applications\MapListVirtual.h"
#include "Engine\Base\DataObjects\WPSMetaData.h"
#include "Engine\Applications\colVIRT.H"
#include "Engine\Table\TBLHIST.H"
#include "ColumnAttributeFromMapList.h"
#include "Engine\Table\tblstore.h"
#include "Headers\Hs\maplist.hs"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

namespace {
	const char* sSyntax() 
	{
		return "ColumnAttributeFromMapList(inpmaplist, source definition, method)\n ";
	}
}
IlwisObjectPtr * createColumnAttributeFromMapList(const FileName& fn, IlwisObjectPtr& ptr, const String& sExpr, vector<void *> parms ) {
	if ( sExpr != "") {
		String *sCol = (String *)parms[1];
		DomainValueRangeStruct *dvs = (DomainValueRangeStruct *)parms[1];
		return (IlwisObjectPtr *)ColumnAttributeFromMapList::create(Table(fn), *sCol, (ColumnPtr &)ptr, sExpr, *dvs);
	}
	else {
		String *sCol = (String *)parms[0];
		return (IlwisObjectPtr *)new ColumnAttributeFromMapList(Table(fn), *sCol, (ColumnPtr &)ptr);
	}
}

String wpsmetadataColumnAttributeFromMapList() {
	WPSMetaData metadata("ColumnAttributeFromMapList");
	return metadata.toString();
}

ApplicationMetadata metadataColumnAttributeFromMapList(ApplicationQueryData *query) {
	ApplicationMetadata md;
	if ( query->queryType == "WPSMETADATA" || query->queryType == "") {
		md.wpsxml = wpsmetadataColumnAttributeFromMapList();
	}
	if ( query->queryType == "OUTPUTTYPE" || query->queryType == "")
		md.returnType = IlwisObject::iotTABLE;
	if ( query->queryType == "EXPERSSION" || query->queryType == "")
		md.skeletonExpression =  sSyntax();

	return md;
}

ColumnAttributeFromMapList::ColumnAttributeFromMapList(const Table& tbl, const String& sColName, ColumnPtr& ptr)
: ColumnVirtual(tbl, sColName, ptr)
{
	String dummy;
	ReadElement("ColumnAttributeFromMapList", "TargetMapList", mpl1);
	ReadElement("ColumnAttributeFromMapList", "SourceDefinition", sourceDefinition);
	ReadElement("ColumnAttributeFromMapList", "Method", method);
	Init();
}

ColumnAttributeFromMapList::ColumnAttributeFromMapList(const Table& tbl, const String& sColName, ColumnPtr& ptr, const MapList& targetMpl, const String& _source, const String& _method):
ColumnVirtual(tbl, sColName, ptr, DomainValueRangeStruct(), Table()),
mpl1(targetMpl),
sourceDefinition(_source),
method(_method)
{
	RangeReal rrMapList = mpl1->getRange();
	ValueRangeReal vr(rrMapList.rLo(),rrMapList.rHi(), mpl1[0]->vr()->rStep());
	ptr.SetDomainValueRangeStruct(DomainValueRangeStruct(Domain("value"),vr));

}

ColumnAttributeFromMapList *ColumnAttributeFromMapList::create(const Table& tbl, const String& sColName, ColumnPtr& ptr,
									const String& sExpr, const DomainValueRangeStruct& dvs)
{
	Array<String> as;
	short iParms = IlwisObjectPtr::iParseParm(sExpr, as);
	MapList mpl(FileName(as[0],".mpl"));
	String source = as[1];
	source = source.toLower();
	if ( source != "histogram")
		throw ErrorObject("Wrong attribute source type");

	String method = as[2];
	method = method.toLower();
	//atSUM, atAVERAGE, atMAX, atMIN, atSTDEV, atMEDIAN, atPRED
	String methods("average,maximum,minimum,stdev,median,predominant,sum");
	if ( methods.find(method) == string::npos) {
		throw ErrorObject("Unknow method used");
	}


	return new ColumnAttributeFromMapList(Table() , sColName, ptr, mpl, source, method);
}

ColumnAttributeFromMapList::~ColumnAttributeFromMapList()
{
}

void ColumnAttributeFromMapList::Store()
{
	ColumnVirtual::Store();
	WriteElement("ColumnAttributeFromMapList", "TargetMapList", mpl1);
	WriteElement("ColumnAttributeFromMapList", "SourceDefinition", sourceDefinition);
	WriteElement("ColumnAttributeFromMapList", "Method", method);
}

void ColumnAttributeFromMapList::Init()
{
	sFreezeTitle = "ColumnAttributeFromMapList";
}

String ColumnAttributeFromMapList::sExpression() const
{
	return String("ColumnAttributeFromMapList(%S,%S,%S)",mpl1->fnObj.sRelative(), sourceDefinition, method);
}


double ColumnAttributeFromMapList::rComputeVal(long iKey) const {

	Map mp = mpl1->map(iKey -1);
	TableHistogramInfo tblHist(mp); 
	double v = rUNDEF;
	if ( method == "average")
		v = tblHist.getAggregate(TableHistogramInfo::atAVERAGE);
	else if ( method == "median")
		v = tblHist.getAggregate(TableHistogramInfo::atMEDIAN);
	else if ( method == "maximum") {
		v = tblHist.getAggregate(TableHistogramInfo::atMAX);
	} 	else if ( method == "minimum") {
		v = tblHist.getAggregate(TableHistogramInfo::atMIN);
	} else if ( method == "predominant") {
		v = tblHist.getAggregate(TableHistogramInfo::atPRED);
	} 	else if ( method == "stdev") {
		v = tblHist.getAggregate(TableHistogramInfo::atSTDEV);
	} 	else if ( method == "sum") {
		v = tblHist.getAggregate(TableHistogramInfo::atSUM);
	} 	


	return v;
}

bool ColumnAttributeFromMapList::fDomainChangeable() const
{
  return false;
}

bool ColumnAttributeFromMapList::fValueRangeChangeable() const
{
  return true;
}