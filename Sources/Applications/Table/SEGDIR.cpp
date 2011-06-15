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
// $Log: /ILWIS 3.0/Table/SEGDIR.cpp $
 * 
 * 5     23-05-05 12:43 Retsios
 * [bug=6575] Do not get goordinates of an invalid segment, as it will
 * cause a crash.
 * 
 * 4     10-01-00 4:09p Martin
 * removed internal rowcols and changed to true coords
 * 
 * 3     9/08/99 11:55a Wind
 * comments
 * 
 * 2     9/08/99 10:27a Wind
 * adpated to use of quoted file names in sExpression()
*/
// Revision 1.4  1998/09/16 17:25:32  Wim
// 22beta2
//
// Revision 1.3  1997/09/10 16:03:29  Wim
// Use geographical definition and not mathematical one
//
// Revision 1.2  1997-08-14 19:47:05+02  Wim
// Renamed column "NrSeg" to "NrLines"
//
/* TableSegDir
   Copyright Ilwis System Development ITC
   april 1995, by Jelle Wind
	Last change:  WK   10 Sep 97    6:00 pm
*/

#include "Headers\Htp\Ilwisapp.htp"
#include "Headers\Err\Ilwisapp.err"
#include "Engine\Map\Segment\Seg.h"
#include "Engine\Table\COLSTORE.H"
#include "Applications\Table\SEGDIR.H"
#include "Engine\Table\tblstore.h"
#include "Engine\Base\DataObjects\valrange.h"
#include "Headers\Hs\segment.hs"


const char* TableSegDir::sSyntax()
{
  return "TableSegDir(segmap)";
}

IlwisObjectPtr * createTableSegDir(const FileName& fn, IlwisObjectPtr& ptr, const String& sExpr, vector<void *> parms ) {
	if ( sExpr != "")
		return (IlwisObjectPtr *)TableSegDir::create(fn, (TablePtr &)ptr, sExpr);
	else
		return (IlwisObjectPtr *)new TableSegDir(fn, (TablePtr &)ptr);
}

TableSegDir* TableSegDir::create(const FileName& fn, TablePtr& p, const String& sExpr)
{
  Array<String> as(1);
  if (!IlwisObjectPtr::fParseParm(sExpr, as))
    throw ErrorExpression(sExpr, sSyntax());
  SegmentMap smp(as[0], fn.sPath());
  return new TableSegDir(fn, p, smp);
}

TableSegDir::TableSegDir(const FileName& fn, TablePtr& p)
: TableVirtual(fn, p)
{
  fNeedFreeze = true;
  ReadElement("TableSegDir", "SegmentMap", smp);
  objdep.Add(smp.ptr());
  if (!fnObj.fValid())
    objtime = objdep.tmNewest();
  Init();
}

TableSegDir::TableSegDir(const FileName& fn, TablePtr& p, const SegmentMap& smap)
: TableVirtual(fn, p, true), //FileName(fn, ".TB#", true), Domain("none")),
  smp(smap)
{
  ptr.SetDomain(Domain("none"));
  ptr.iRecNew(180);
  fNeedFreeze = true;
  objdep.Add(smp.ptr());
}

TableSegDir::~TableSegDir()
{
}

void TableSegDir::Store()
{
  TableVirtual::Store();
  WriteElement("TableVirtual", "Type", "TableSegDir");
  WriteElement("TableSegDir", "SegmentMap", smp);
}

bool TableSegDir::fFreezing()
{
	CoordBuf crdBuf(1000);
	long iNmbrLegs;
	RealBuf rSumL(180);
	LongBuf iFreq(180);
	Init();
	for (int idegree = 0; idegree < 180; idegree++) {
		rSumL[idegree] = 0.0;
		iFreq[idegree] = 0;
	} 
	const double rRadtoDegree = 180.0 / M_PI;

	if (iRecs() == 0)
		ptr.iRecNew(180);

	for (long iRec = 1; iRec <= 180; iRec++)
		colDirection->PutVal(iRec, (long)(iRec-1));

	trq.SetText(SSEGTextCalculating);

	for (long iSeg = 0; iSeg < smp->iFeatures(); iSeg++) {
		ILWIS::Segment *seg = CSEGMENT(smp->getFeature(iSeg));
		if (!seg || !seg->fValid())
			continue;

		if (trq.fUpdate(iSeg, smp->iFeatures()))
			return false;
		// calculate direction between points in segment
		CoordinateSequence *seq = seg->getCoordinates();
		iNmbrLegs = (long)seq->size();

		double rYStart = seq->getAt(0).y;
		double rXStart = seq->getAt(0).x;

		for (long iLegc = 1; iLegc < iNmbrLegs; iLegc++)  {
			double rYEnd = seq->getAt(iLegc).y;
			double rXEnd = seq->getAt(iLegc).x;
			double rYDiff = rYEnd - rYStart;
			double rXDiff = rXEnd - rXStart;
			if (rYDiff == 0 && rXDiff == 0)
				continue;
			// do not use mathematical definition
			// but geographical definition:
			double rAngle = atan2(rXDiff, rYDiff);
			double rDegree = rRadtoDegree * rAngle;
			double rLength = sqrt(rYDiff * rYDiff + rXDiff * rXDiff);
			if (rDegree < 0.0) rDegree += 180.0;
			int iDegree = (int)round (rDegree);
			if (iDegree == 180) iDegree = 0;
			rSumL[iDegree] += rLength;
			iFreq[iDegree]++;
			rYStart = rYEnd;
			rXStart = rXEnd;
		}
		delete seq;
	}


	// correct length values for conversion RowCol to GeoRef
	//for (idegree = 0; idegree < 180; idegree++)
	//  rSumL[idegree] *= ralfa;

	colLength->PutBufVal(rSumL, 1);
	colFreq->PutBufVal(iFreq, 1);

	return true;
}

void TableSegDir::Init()
{
  sFreezeTitle = "TableSegDir";
  htpFreeze = "ilwisapp\\segment_direction_histogram_algorithm.htm";
  if (pts==0)
    return;
  colDirection = pts->col("Direction");
  if (!colDirection.fValid()) {
    colDirection = pts->colNew("Direction", Domain("value"), ValueRange(0,179));
    colDirection->sDescription = SSEGMsgDirectionDegree;
  }
  colDirection->SetOwnedByTable(true);
  colDirection->SetReadOnly(true);
  colLength= pts->col("Length");
  if (!colLength.fValid()) {
    colLength = pts->colNew("Length", Domain("distance"));
    colLength->sDescription = SSEGMsgTotalLengthDirection;
  }
  colLength->SetOwnedByTable(true);
  colLength->SetReadOnly(true);
  colFreq = pts->col("NrLines");
  if (!colFreq.fValid()) {
    colFreq = pts->colNew("NrLines", Domain("count"));
    colFreq->sDescription = SSEGMsgLinesPerDirection;
  }
  colFreq->SetOwnedByTable(true);
  colFreq->SetReadOnly(true);
}

void TableSegDir::UnFreeze()
{
  if (colDirection.fValid()) {
    pts->RemoveCol(colDirection);
    colDirection = Column();
  }  
  if (colLength.fValid()) {
    pts->RemoveCol(colLength);
    colLength = Column();
  }  
  if (colFreq.fValid()) {
    pts->RemoveCol(colFreq);
    colFreq = Column();
  }  
  TableVirtual::UnFreeze();
}


String TableSegDir::sExpression() const
{
   return String("TableSegDir(%S)", smp->sNameQuoted(true, fnObj.sPath()));
}




