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
// $Log: /ILWIS 3.0/PointMap/PNTNUMB.cpp $
 * 
 * 7     23/03/01 15:51 Willem
 * - The Column in the new attribute table with the name of the original
 * map is now not TableOwned anymore
 * - The existance of the attribute table is checked before use
 * 
 * 6     16-06-00 12:07 Koolhoven
 * Now uses DomainUniqueID, no sNewDom can be specified
 * 
 * 5     10-01-00 17:35 Wind
 * adapted to creation of table and domain when creating object
 * 
 * 4     4/11/99 14:10 Willem
 * Extended to accept a domain prefix. Changed the syntax and the form
 * 
 * 3     9/08/99 11:59a Wind
 * comments
 * 
 * 2     9/08/99 10:21a Wind
 * adpated to use of quoted file names
*/
// Revision 1.5  1998/09/16 17:26:27  Wim
// 22beta2
//
// Revision 1.4  1997/08/26 14:11:03  martin
// Changed PutVal to a AddRaw. Tbl is empty so putval has little effect.
//
// Revision 1.3  1997/08/26 08:45:14  Wim
// Added optional new domain name
//
// Revision 1.2  1997-08-13 13:56:50+02  Wim
// Column in Attribute Table is now table owned and read only
//
/* PointMapNumbering
   Copyright Ilwis System Development ITC
   feb 1997, by Jelle Wind
	Last change:  MS   26 Aug 97    3:09 pm
*/                                                                      

#include "PointApplications\PNTNUMB.H"
#include "Engine\Map\Point\PNTSTORE.H"
#include "Engine\Table\COLSTORE.H"
#include "Engine\Base\DataObjects\valrange.h"
#include "Engine\Domain\dmsort.h"
#include "Engine\Base\DataObjects\ERR.H"
#include "Headers\Err\Ilwisapp.err"
#include "Headers\Htp\Ilwisapp.htp"
#include "Headers\Hs\point.hs"

IlwisObjectPtr * createPointMapNumbering(const FileName& fn, IlwisObjectPtr& ptr, const String& sExpr, vector<void *> parms ) {
	if ( sExpr != "")
		return (IlwisObjectPtr *)PointMapNumbering::create(fn, (PointMapPtr &)ptr, sExpr);
	else
		return (IlwisObjectPtr *)new PointMapNumbering(fn, (PointMapPtr &)ptr);
}

const char* PointMapNumbering::sSyntax() {
  return "PointMapNumbering(pntmap,,prefix)";
}

PointMapNumbering* PointMapNumbering::create(const FileName& fn, PointMapPtr& p, const String& sExpr)
{
  Array<String> as;
  int iParms = IlwisObjectPtr::iParseParm(sExpr, as);
  if ((iParms < 1) || (iParms > 3))
    throw ErrorExpression(sExpr, sSyntax());
  PointMap pmp(as[0], fn.sPath());

  String sNewPrefix;

  if (iParms >= 3)
	  sNewPrefix = as[2];

  if (sNewPrefix.length() == 0)
	  sNewPrefix = "Pnt";

  return new PointMapNumbering(fn, p, pmp, sNewPrefix);
}

PointMapNumbering::PointMapNumbering(const FileName& fn, PointMapPtr& p)
: PointMapVirtual(fn, p)
{
  fNeedFreeze = true;
  try {
    ReadElement("PointMapNumbering", "PointMap", pmp);
  }
  catch (const ErrorObject& err) {  // catch to prevent invalid object
    err.Show();
    return;
  }
  Init();
  objdep.Add(pmp.ptr());
}

PointMapNumbering::PointMapNumbering(const FileName& fn, PointMapPtr& p,
  const PointMap& pm, const String& sNewPrefix)
: PointMapVirtual(fn,p,pm->cs(),pm->cb(),pm->dvrs()),
  pmp(pm)
{
	Domain dom(fnObj, 0, dmtUNIQUEID, sNewPrefix);
	SetDomainValueRangeStruct(dom);
	fNeedFreeze = true;
  FileName fnAtt(fnObj, ".tbt", true);
	Table	tblAtt = Table(fnAtt, dm());
	Column col = Column(tblAtt, pmp->fnObj.sFile, pmp->dvrs());
	SetAttributeTable(tblAtt);
	Init();
	objdep.Add(pmp.ptr());
	if (!fnObj.fValid()) // 'inline' object
		objtime = objdep.tmNewest();
  ptr.Store(); // store domain and att table info
  ptr.fChanged = true; // to ensure that other info will be written
}

void PointMapNumbering::Store()
{
  PointMapVirtual::Store();
  WriteElement("PointMapVirtual", "Type", "PointMapNumbering");
  WriteElement("PointMapNumbering", "PointMap", pmp);
}

PointMapNumbering::~PointMapNumbering()
{
}

String PointMapNumbering::sExpression() const
{
  String s("PointMapNumbering(%S)", pmp->sNameQuoted(false, fnObj.sPath()));
  return s;
}


void PointMapNumbering::Init()
{
  htpFreeze = "ilwisapp\\unique_id_algorithm.htm";
  sFreezeTitle = "PointMapNumbering";
}

bool PointMapNumbering::fFreezing()
{
	trq.SetTitle(SPNTTitlePointMapNumbering);
	trq.SetText(SPNTTextCheckPoints);
	long iNewPnt = 0;
	Coord crd;
	for (long i=1; i <= pmp->iFeatures(); ++i ) {
		if (trq.fUpdate(i, pmp->iFeatures()))
			return false; 
		crd = pmp->cValue(i);
		if (crd.fUndef())
			continue;
		iNewPnt++;
	}  
	FileName fnDom = dm()->fnObj;
	
	DomainSort *pds = dm()->pdsrt();
	pds->Resize(iNewPnt);
	
	CreatePointMapStore();
	
	Table tbl;
	try
	{
		tbl = tblAtt();
	}
	catch (ErrorNotFound&)
	{
		// prevent not found error: if not exist the table is created anyway
	}
	
	if (!tbl.fValid()) {
		FileName fnAtt(fnObj, ".tbt", true);
		tbl = Table(fnAtt, dm());
		SetAttributeTable(tbl);
	}
	else
		tbl->CheckNrRecs();
	Column col = tbl->col(pmp->fnObj.sFile);
	if (!col.fValid()) {
		col = Column(tbl, pmp->fnObj.sFile, pmp->dvrs());
		col->fErase = true;
	}
	
	trq.SetText(String(SPNTTextNumbering_S.scVal(), sName(true, fnObj.sPath())));
	iNewPnt = 0;
	double rVal;
	long iRaw;
	bool fUseReals = pmp->dvrs().fUseReals();
	for (long i=1; i <= pmp->iFeatures(); ++i ) {
		if (trq.fUpdate(i, pmp->iFeatures()))
			return false; 
		crd = pmp->cValue(i);
		if (crd.fUndef())
			continue;
		if (fUseReals) {
			rVal = pmp->rValue(i);
			if (rVal == rUNDEF)
				continue;
		}
		else {
			iRaw = pmp->iRaw(i);
			if (iRaw == iUNDEF)
				continue;
		}
		iNewPnt++;
		pms->iAddRaw(crd, iNewPnt);
		if (fUseReals)
			col->PutVal(iNewPnt, rVal);
		else
			col->PutRaw(iNewPnt, iRaw);
	}
	col->SetReadOnly(true);
	col->fErase = false;
	tbl->Updated();
	trq.fUpdate(pmp->iFeatures(), pmp->iFeatures());
	_iPoints = iNewPnt;
	return true;
}




