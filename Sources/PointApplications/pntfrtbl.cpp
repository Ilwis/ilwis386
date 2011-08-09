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
// $Log: /ILWIS 3.0/PointMap/pntfrtbl.cpp $
 * 
 * 6     8-12-00 12:37 Koolhoven
 * sExpression() with coordcolumn was wrong
 * 
 * 5     27-11-00 10:32 Koolhoven
 * allow also X,Y columns as input. Check on colCoord now checks on domain
 * type to prevent crashes
 * 
 * 4     1-09-00 12:11p Martin
 * Extended syntax to include coordinate columns
 * 
 * 3     9/08/99 11:59a Wind
 * comments
 * 
 * 2     9/08/99 10:21a Wind
 * adpated to use of quoted file names
*/
// Revision 1.1  1998/09/16 17:26:27  Wim
// 22beta2
//
/* PointMapFromTable
   Copyright Ilwis System Development ITC
   march 1998, by Wim Koolhoven
	Last change:  WK    7 May 98   12:40 pm
*/

#include "PointApplications\pntfrtbl.h"
#include "Engine\Map\Point\PNTSTORE.H"
#include "Engine\Domain\Dmvalue.h"
#include "Engine\Domain\dmsort.h"
#include "Engine\Domain\dmcoord.h"
#include "Engine\Base\DataObjects\valrange.h"
#include "Headers\Htp\Ilwisapp.htp"
#include "Headers\Err\Ilwisapp.err"
#include "Headers\Hs\point.hs"

IlwisObjectPtr * createPointMapFromTable(const FileName& fn, IlwisObjectPtr& ptr, const String& sExpr, vector<void *> parms ) {
	if ( sExpr != "")
		return (IlwisObjectPtr *)PointMapFromTable::create(fn, (PointMapPtr &)ptr, sExpr);
	else
		return (IlwisObjectPtr *)new PointMapFromTable(fn, (PointMapPtr &)ptr);
}

const char* PointMapFromTable::sSyntax() {
  return "PointMapFromTable(table,[colX,colY,]coordsys)\n"
         "PointMapFromTable(table,[colX,colY,]coordsys,attribcol)\n"
         "PointMapFromTable(table,[colX,colY,]coordsys,prefix)\n"
         "PointMapFromTable(table,colCoord,prefix)\n";
         "PointMapFromTable(table,colCoord,attribcol)";
}

void TableNeedsIdentifierDomainError(const FileName& fnTbl)
{
  throw ErrorObject(
    WhatError(String(TR("Table does not have identifier domain")),
                errPointMapFromTable), fnTbl);
}

PointMapFromTable* PointMapFromTable::create(const FileName& fn, PointMapPtr& p, const String& sExpr)
{
  Array<String> as;
  int iParms = IlwisObjectPtr::iParseParm(sExpr, as);
  if ((iParms < 2) || (iParms > 5))
    throw ErrorExpression(sExpr, sSyntax());
  FileName fnTbl = as[0];
  fnTbl.sExt = ".tbt";
  Table tbl(fnTbl);
  if (!tbl.fValid())
    NotFoundError(fnTbl);
  FileName fnCsy;
  String sColX, sColY, sColCoord;
	CoordSystem cs;
	Column colX, colY, colCoord;
	sColCoord = as[1];
	colCoord = tbl[sColCoord];
	if (colCoord.fValid() && !colCoord->dm()->pdcrd())
		colCoord = Column();
	if (!colCoord.fValid()) 
	{
		if (iParms < 4) 
		{
			sColX = "X";
			sColY = "Y";
			fnCsy = as[1];
		}
		else 
		{
			sColX = as[1];
			sColY = as[2];
			fnCsy = as[3];
		}
	  colX = tbl[sColX];
		if (!colX.fValid())
			ColumnNotFoundError(fnTbl, sColX);
		colY = tbl[sColY];
		if (!colY.fValid())
			ColumnNotFoundError(fnTbl, sColY);
		cs = CoordSystem(fnCsy);
		fnCsy.sExt = ".csy";
		if (!cs.fValid())
			NotFoundError(fnCsy);
	}
	if (!colCoord.fValid() && !colX.fValid() && !colY.fValid())
		ColumnNotFoundError(fnTbl, sColCoord);		

  String sColAttr, sPrefix;
  Column colAttrib;
  DomainValueRangeStruct dvrs;
  if (2 == iParms || (4 == iParms && !colCoord.fValid())) {
    if (0 != tbl->dm()->pdnone())
      sPrefix = "pnt";
    else if (0 != tbl->dm()->pdid())
      dvrs = tbl->dm();
    else
      TableNeedsIdentifierDomainError(fnTbl);
  }
  else {
    sColAttr = as[iParms-1];
    colAttrib = tbl[sColAttr];
    if (!colAttrib.fValid()) {
      if (0 != tbl->dm()->pdnone())
        sPrefix = as[iParms-1];
      else
        ColumnNotFoundError(fnTbl, sColAttr);
    }
    else
      dvrs = colAttrib->dvrs();
  }
  if (sPrefix[0]) {
    FileName fnDom = fn;
    fnDom.sExt = ".dom";
    Domain dm(FileName::fnUnique(fnDom), tbl->iRecs(), dmtID, sPrefix);
    tbl->SetDomain(dm);
    tbl->Updated();
    dvrs = dm;
  }
	if (!colCoord.fValid())
		return new PointMapFromTable(fn, p, tbl, colX, colY, cs, dvrs, colAttrib);
	else
	{
		CoordSystem cs = colCoord->dm()->pdcrd()->csyCoordSys();
		return new PointMapFromTable(fn, p, tbl, colCoord, cs, dvrs, colAttrib);
	}		
}

PointMapFromTable::PointMapFromTable(const FileName& fn, PointMapPtr& p)
: PointMapVirtual(fn, p)
{
  try {
    ReadElement("PointMapFromTable", "Table", tbl);
    String sColX, sColY, sColCoord, sColAttrib;
    int iR = ReadElement("PointMapFromTable", "X-Column", sColX);
		if (iR != 0)
		{
	    colX = tbl[sColX];
		  if (!colX.fValid())
			  ColumnNotFoundError(tbl->fnObj, sColX);
			ReadElement("PointMapFromTable", "Y-Column", sColY);
			colY = tbl[sColY];
			if (!colY.fValid())
				ColumnNotFoundError(tbl->fnObj, sColY);
		}
		else
		{
			ReadElement("PointMapFromTable", "Coord-Column", sColCoord);			
			colCoord = tbl[sColCoord];
		  if (!colCoord.fValid())
			  ColumnNotFoundError(tbl->fnObj, sColCoord);			
		}			
		
    ReadElement("PointMapFromTable", "AttribColumn", sColAttrib);
    colAttrib = tbl[sColAttrib];
  }
  catch (const ErrorObject& err) {  // catch to prevent invalid object
    err.Show();
    return;
  }
  _iPoints = tbl->iRecs();
  objdep.Add(tbl);
  objdep.Add(colX);
  objdep.Add(colY);
  if (colAttrib.fValid())
    objdep.Add(colAttrib);
  else
    SetAttributeTable(tbl);
}

PointMapFromTable::PointMapFromTable(
    const FileName& fn, PointMapPtr& p, const Table& table,
    const Column& colx, const Column& coly,
    const CoordSystem& cs, const DomainValueRangeStruct& dvrs,
    const Column& colAttr)
: PointMapVirtual(fn, p, cs, cs->cb, dvrs),
  tbl(table), colX(colx), colY(coly), colAttrib(colAttr)
{
  _iPoints = tbl->iRecs();
  objdep.Add(tbl);
  objdep.Add(colX);
  objdep.Add(colY);
  if (colAttrib.fValid())
    objdep.Add(colAttrib);
  else
    SetAttributeTable(tbl);
  Init();
}

PointMapFromTable::PointMapFromTable(
    const FileName& fn, PointMapPtr& p, const Table& table,
    const Column& _colCoord, const CoordSystem& cs, 
		const DomainValueRangeStruct& dvrs, const Column& colAttr)
: PointMapVirtual(fn, p, cs, cs->cb, dvrs),
  tbl(table), colCoord(_colCoord), colAttrib(colAttr)
{
  _iPoints = tbl->iRecs();
  objdep.Add(tbl);
  objdep.Add(colCoord);
  if (colAttrib.fValid())
    objdep.Add(colAttrib);
  else
    SetAttributeTable(tbl);
  Init();
}

PointMapFromTable::~PointMapFromTable()
{
}

String PointMapFromTable::sExpression() const
{
  String sTbl = tbl->sNameQuoted(false, fnObj.sPath());
  String sCsy = cs()->sNameQuoted(false, fnObj.sPath());
  String sXY;
	String str	;
	if (!colCoord.fValid())
	{
	  if (colX->sName() != "X" || colY->sName() != "Y")
		  sXY = String(",%S,%S", colX->sNameQuoted(), colY->sNameQuoted());
		String sLast;
		if (colAttrib.fValid())
			sLast = String(",%S", colAttrib->sNameQuoted());
		str = String("PointMapFromTable(%S%S,%S%S)", sTbl, sXY, sCsy, sLast);
		return str;
	}
	else
	{
		String sLast;
		if (colAttrib.fValid())
			sLast = String(",%S", colAttrib->sNameQuoted());
		str = String("PointMapFromTable(%S,%S%S)", sTbl, colCoord->sName(), sLast);		
	}	

	return str;
}

void PointMapFromTable::Store()
{
  PointMapVirtual::Store();
  WriteElement("PointMapVirtual", "Type", "PointMapFromTable");
  WriteElement("PointMapFromTable", "Table", tbl);
	if (!colCoord.fValid())
	{
	  WriteElement("PointMapFromTable", "X-Column", colX);
		WriteElement("PointMapFromTable", "Y-Column", colY);
	}
	else
		WriteElement("PointMapFromTable", "Coord-Column", colCoord);
	
	if (colAttrib.fValid())
		WriteElement("PointMapFromTable", "AttribColumn", colAttrib);
	else
		WriteElement("PointMapFromTable", "AttribColumn", (char*)0);
}

void PointMapFromTable::Init()
{
  _iPoints = tbl->iRecs();
  CoordBounds cb;
  for (long i = 0; i < _iPoints; ++i)
    cb += cValue(i);
  double rExtraWidth = 1 + cb.width() / 20;
  cb.MinX() -= rExtraWidth;
  cb.MaxX() += rExtraWidth;
  double rExtraHeight = 1 + cb.height() / 20;
  cb.MinY() -= rExtraHeight;
  cb.MaxY() += rExtraHeight;
  SetCoordBounds(cb);
}

void PointMapFromTable::InitFreeze()
{
  Init();
  htpFreeze = "ilwisapp\\table_to_point_map_dialog_box.htm";
  sFreezeTitle = "PointMapFromTable";
}

long PointMapFromTable::iRaw(long iRec) const
{
  if (colAttrib.fValid())
    return colAttrib->iRaw(iRec + 1) + 1;
  else
    return iRec + 1;
}

long PointMapFromTable::iValue(long iRec) const
{
  if (colAttrib.fValid())
    return colAttrib->iValue(iRec + 1);
  else
    return dvrs().iValue(iRec + 1);
}

double PointMapFromTable::rValue(long iRec) const
{
  if (colAttrib.fValid())
    return colAttrib->rValue(iRec + 1);
  else
    return dvrs().rValue(iRec + 1);
}

String PointMapFromTable::sValue(long iRec, short iWidth, short iDec) const
{
  if (colAttrib.fValid())
    return colAttrib->sValue(iRec + 1, iWidth, iDec);
  else
    return dvrs().sValue(iRec + 1, iWidth, iDec);
}

Coord PointMapFromTable::cValue(long iRec) const
{
  Coord crd;
	if (!colCoord.fValid())
	{
	  crd.x = colX->rValue(iRec + 1);
		crd.y = colY->rValue(iRec + 1);
	}	
	else
		crd = colCoord->cValue(iRec+1);
		
  return crd;
}

bool PointMapFromTable::fDomainChangeable() const
{
  return false;
}





