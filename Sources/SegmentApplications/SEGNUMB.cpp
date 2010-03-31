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
// $Log: /ILWIS 3.0/SegmentMap/SEGNUMB.cpp $
 * 
 * 10    23/03/01 15:51 Willem
 * - The Column in the new attribute table with the name of the original
 * map is now not TableOwned anymore
 * - The existance of the attribute table is checked before use
 * 
 * 9     9-11-00 22:06 Hendrikse
 * implemented an extra length column for the attrib table
 * 
 * 8     16-06-00 12:21 Koolhoven
 * now create a DomainUinqueID. specifying a domain name has no influence
 * anymore
 * 
 * 7     18-01-00 15:32 Wind
 * domain and attribute table created at definition time
 * 
 * 6     17-01-00 8:17a Martin
 * changed rowcols to coords
 * 
 * 5     10-12-99 11:48a Martin
 * removed internal rowcols and replaced them by true coords
 * 
 * 4     4/11/99 16:57 Willem
 * Extended the syntax to accept a domain prefix
 * 
 * 3     9/08/99 12:02p Wind
 * comments
 * 
 * 2     9/08/99 10:26a Wind
 * adpated to use of quoted file names in sExpression()
*/
// Revision 1.6  1998/09/16 17:25:20  Wim
// 22beta2
//
// Revision 1.5  1997/08/26 08:50:24  Wim
// Typing error
//
// Revision 1.4  1997-08-26 10:45:27+02  Wim
// Added optional new domain name
//
// Revision 1.3  1997-08-13 13:56:50+02  Wim
// Column in Attribute Table is now table owned and read only
//
// Revision 1.2  1997-07-29 16:37:50+02  Wim
// sSyntax() corrected
//
/* SegmentMapNumbering
   Copyright Ilwis System Development ITC
   feb 1997, by Jelle Wind
	Last change:  WK   26 Aug 97   10:47 am
*/                                                                      

#include "SegmentApplications\SEGNUMB.H"
#include "Engine\Base\DataObjects\valrange.h"
#include "Engine\Domain\dmsort.h"
#include "Engine\Table\COLSTORE.H"
#include "Headers\Err\Ilwisapp.err"
#include "Headers\Htp\Ilwisapp.htp"
#include "Headers\Hs\segment.hs"

IlwisObjectPtr * createSegmentMapNumbering(const FileName& fn, IlwisObjectPtr& ptr, const String& sExpr, vector<void *> parms ) {
	if ( sExpr != "")
		return (IlwisObjectPtr *)SegmentMapNumbering::create(fn, (SegmentMapPtr &)ptr, sExpr);
	else
		return (IlwisObjectPtr *)new SegmentMapNumbering(fn, (SegmentMapPtr &)ptr);
}

const char* SegmentMapNumbering::sSyntax() {
  return "SegmentMapNumbering(segmap[,,prefix])";
}

SegmentMapNumbering* SegmentMapNumbering::create(const FileName& fn, SegmentMapPtr& p, const String& sExpr)
{
  Array<String> as;
  int iParms = IlwisObjectPtr::iParseParm(sExpr, as);
  if ((iParms < 1) || (iParms > 3))
    throw ErrorExpression(sExpr, sSyntax());
  SegmentMap smp(as[0], fn.sPath());
  String sNewPrefix;

  if (iParms >= 3)
	  sNewPrefix = as[2];

  if (sNewPrefix.length() == 0)
	  sNewPrefix = "Seg";

	return new SegmentMapNumbering(fn, p, smp, sNewPrefix);
}

SegmentMapNumbering::SegmentMapNumbering(const FileName& fn, SegmentMapPtr& p)
: SegmentMapVirtual(fn, p)
{
  fNeedFreeze = true;
  String sColName;
  try {
    ReadElement("SegmentMapNumbering", "SegmentMap", smp);
  }
  catch (const ErrorObject& err) {  // catch to prevent invalid object
    err.Show();
    return;
  }
  Init();
  objdep.Add(smp.ptr());
}

SegmentMapNumbering::SegmentMapNumbering(const FileName& fn, SegmentMapPtr& p,
  const SegmentMap& sm, const String& sNewPrefix)
: SegmentMapVirtual(fn, p, sm->cs(),sm->cb(),sm->dvrs()),
  smp(sm)
{
	Domain dom(fnObj, 0, dmtUNIQUEID, sNewPrefix);
  SetDomainValueRangeStruct(dom);

  fNeedFreeze = true;
  FileName fnAtt(fnObj, ".tbt", true);
	Table	tblAtt = Table(fnAtt, dm());
	Column col = Column(tblAtt, smp->fnObj.sFile, smp->dvrs());
	SetAttributeTable(tblAtt);
  Init();
  objdep.Add(smp.ptr());
  if (!fnObj.fValid()) // 'inline' object
    objtime = objdep.tmNewest();
}

void SegmentMapNumbering::Store()
{
  SegmentMapVirtual::Store();
  WriteElement("SegmentMapVirtual", "Type", "SegmentMapNumbering");
  WriteElement("SegmentMapNumbering", "SegmentMap", smp);
}

SegmentMapNumbering::~SegmentMapNumbering()
{
}

String SegmentMapNumbering::sExpression() const
{
  String s("SegmentMapNumbering(%S)", smp->sNameQuoted(false, fnObj.sPath()));
  return s;
}


void SegmentMapNumbering::Init()
{
  htpFreeze = htpSegmentMapNumberingT;
  sFreezeTitle = "SegmentMapNumbering";
}

bool SegmentMapNumbering::fFreezing()
{
	long iSeg = smp->iFeatures();
	long iNrNewSeg = 0;
	trq.SetText(SSEGTextCalcNumberSegments);
	for (int i=0; i < smp->iFeatures(); ++i )  {
	  ILWIS::Segment *seg = (ILWIS::Segment *)smp->getFeature(i);
	  if ( seg == NULL || !seg->fValid())
		  continue;
		if (trq.fUpdate(i, iSeg))
			return false;
		iNrNewSeg++;
	}  
	
	DomainSort* pdsrt = dm()->pdsrt();
	pdsrt->Resize(iNrNewSeg);
	
	CreateSegmentMapStore();

	Table tbl;
	try
	{
		tbl = tblAtt();
	}
	catch (ErrorNotFound&)
	{
		// prevent not found error: if not exist the table is created anyway
	}
	if (!tbl.fValid())
	{
		FileName fnAtt(fnObj, ".tbt", true);
		tbl = Table(fnAtt, dm());
		SetAttributeTable(tbl);
	}
	else
		tbl->CheckNrRecs();

	Column col = tbl->col(smp->fnObj.sFile);
	if (!col.fValid())
		col = Column(tbl, smp->fnObj.sFile, smp->dvrs());
	
	Column colLength = tbl->col("Length");
	if ( !colLength.fValid())
		colLength = tbl->colNew("Length",DomainValueRangeStruct(-1e307, 1e307, 0.001));	
	
	trq.SetText(String(SSEGTextNumbering_S.scVal(), sName(true, fnObj.sPath())));
	iNrNewSeg = 0;
	double rVal;

	bool fUseReals = smp->dvrs().fUseReals();
	for (int i=0; i < smp->iFeatures(); ++i )  {
	  ILWIS::Segment *seg = (ILWIS::Segment *)smp->getFeature(i);
	  if ( seg == NULL || !seg->fValid())
		  continue;
		if (trq.fUpdate(i, iSeg))
			return false;
		rVal = seg->rValue();
		if (rVal == rUNDEF)
				continue;
		CoordinateSequence *seq = seg->getCoordinates();
		
		ILWIS::Segment *segNew = CSEGMENT(pms->newFeature());
		segNew->PutCoords(seq);
		iNrNewSeg++;
		segNew->PutVal(iNrNewSeg);
		col->PutVal(iNrNewSeg, rVal);
		colLength->PutVal(iNrNewSeg, seg->rLength());
	}
	col->SetReadOnly(true);
	tbl->Updated();
	trq.fUpdate(iSeg, iSeg);
	
	return true;
}




