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
// TableMapListAttributeFrom.cpp: implementation of the TableMapListAttributeFrom class.
//
//////////////////////////////////////////////////////////////////////

#include "headers\toolspch.h"
#include "Engine\Applications\MapListVirtual.h"
#include "Engine\Base\DataObjects\WPSMetaData.h"
#include "Engine\Applications\TBLVIRT.H"
#include "Engine\Table\TBLHIST.H"
#include "TableAttributeFromMapList.h"
#include "Engine\Table\tblstore.h"
#include "Headers\Hs\maplist.hs"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

namespace {
	const char* sSyntax() 
	{
		return "TableMapListAttributeFrom(inpmaplist, source definition, method)\n ";
	}
}

IlwisObjectPtr * createTableMapListAttributeFrom(const FileName& fn, IlwisObjectPtr& ptr, const String& sExpr, vector<void *> parms ) {
	if ( sExpr != "")
		return (IlwisObjectPtr *) TableMapListAttributeFrom::create(fn, (TablePtr &)ptr, sExpr);
	else
		return (IlwisObjectPtr *)new TableMapListAttributeFrom(fn, (TablePtr &)ptr);
}

String wpsmetadataTableMapListAttributeFrom() {
	WPSMetaData metadata("TableMapListAttributeFrom");
	return metadata.toString();
}

ApplicationMetadata metadataTableMapListAttributeFrom(ApplicationQueryData *query) {
	ApplicationMetadata md;
	if ( query->queryType == "WPSMETADATA" || query->queryType == "") {
		md.wpsxml = wpsmetadataTableMapListAttributeFrom();
	}
	if ( query->queryType == "OUTPUTTYPE" || query->queryType == "")
		md.returnType = IlwisObject::iotTABLE;
	if ( query->queryType == "EXPERSSION" || query->queryType == "")
		md.skeletonExpression =  sSyntax();

	return md;
}

TableMapListAttributeFrom::TableMapListAttributeFrom(const FileName& fn, TablePtr& ptr)
: TableVirtual(fn, ptr)
{
	String dummy;
	ReadElement("TableMapListAttributeFrom", "TargetMapList", mpl1);
	ReadElement("TableMapListAttributeFrom", "SourceDefinition", sourceDefinition);
	ReadElement("TableMapListAttributeFrom", "Method", method);
	Init();
}

TableMapListAttributeFrom::TableMapListAttributeFrom(const FileName& fn, TablePtr& _ptr, const MapList& targetMpl, const String& _source, const String& _method):
TableVirtual(fn, _ptr,true),
mpl1(targetMpl),
sourceDefinition(_source),
method(_method)
{
	ptr.SetDomain(Domain("none"));
}

TableMapListAttributeFrom *TableMapListAttributeFrom::create(const FileName& fn, TablePtr& ptr, const String& sExpr)
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
	String methods("average,maximum,minimum,stdev,median,predominant");
	if ( methods.find(method) == string::npos) {
		throw ErrorObject("Unknow method used");
	}


	return new TableMapListAttributeFrom(fn ,ptr, mpl, source, method);
}

TableMapListAttributeFrom::~TableMapListAttributeFrom()
{
}

void TableMapListAttributeFrom::Store()
{
	TableVirtual::Store();
	WriteElement("TableMapListAttributeFrom", "TargetMapList", mpl1);
	WriteElement("TableMapListAttributeFrom", "SourceDefinition", sourceDefinition);
	WriteElement("TableMapListAttributeFrom", "Method", method);
}

void TableMapListAttributeFrom::Init()
{
	sFreezeTitle = "TableMapListAttributeFrom";
}

String TableMapListAttributeFrom::sExpression() const
{
	return String("TableMapListAttributeFrom(%S,%S,%S)",mpl1->fnObj.sRelative(), sourceDefinition, method);
}


bool TableMapListAttributeFrom::fFreezing()
{ 
	trq.SetText(TR("Calculating columns"));
	RangeReal rrMapList = mpl1->getRange();

	long tot=mpl1->iSize();
	ptr.iRecNew(tot);
	Table tbl;
	tbl.SetPointer(&ptr);

	Column colavg(tbl, method,DomainValueRangeStruct(Domain("value"),ValueRange(rrMapList.rLo(),rrMapList.rHi(),mpl1[0]->vr()->rStep())));

	for (int i=0; i<tot; ++i)
	{	
		Map mp = mpl1->map(i);
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
		colavg->PutVal(i+1, v);
	}

	return true;
}