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
// $Log: /ILWIS 3.0/PolygonMap/POLNUMB.cpp $
 * 
 * 11    23/03/01 15:51 Willem
 * - The Column in the new attribute table with the name of the original
 * map is now not TableOwned anymore
 * - The existance of the attribute table is checked before use
 * 
 * 10    10-11-00 11:51 Hendrikse
 * corrected check if ( !polNew.fDeleted() ) in fFreezing()
 * 
 * 9     25-07-00 10:35a Martin
 * area column added
 * 
 * 8     16-06-00 12:29 Koolhoven
 * creates now a DomainUniqueID, specifying a domain name has no influence
 * anymore
 * 
 * 7     17-01-00 8:17a Martin
 * changed rowcols to coords
 * 
 * 6     5-01-00 18:10 Wind
 * now domain and attribute table are properly created at defintion time
 * 
 * 5     10-12-99 3:14p Martin
 * removed internal coordinates and used true coords
 * 
 * 4     4/11/99 16:43 Willem
 * Extended the syntax to accept a domain prefix
 * 
 * 3     9/08/99 11:57a Wind
 * comments
 * 
 * 2     9/08/99 10:23a Wind
 * adpated to use of quoted file names
*/
// Revision 1.5  1998/09/16 17:25:54  Wim
// 22beta2
//
// Revision 1.4  1997/08/26 08:45:37  Wim
// Added optional new domain name
//
// Revision 1.3  1997-08-13 13:56:50+02  Wim
// Column in Attribute Table is now table owned and read only
//
// Revision 1.2  1997-07-29 16:38:34+02  Wim
// sSyntax() corrected
//
/* PolygonMapNumbering
   Copyright Ilwis System Development ITC
   feb 1997, by Jelle Wind
	Last change:  WK   26 Aug 97   10:44 am
*/                                                                      

#include "PolygonApplications\POLNUMB.H"
#include "Engine\Base\DataObjects\valrange.h"
#include "Engine\Domain\dmsort.h"
#include "Engine\Table\COLSTORE.H"
#include "Headers\Err\Ilwisapp.err"
#include "Headers\Htp\Ilwisapp.htp"
#include "Headers\Hs\polygon.hs"

IlwisObjectPtr * createPolygonMapNumbering(const FileName& fn, IlwisObjectPtr& ptr, const String& sExpr, vector<void *> parms ) {
	if ( sExpr != "")
		return (IlwisObjectPtr *)PolygonMapNumbering::create(fn, (PolygonMapPtr &)ptr, sExpr);
	else
		return (IlwisObjectPtr *)new PolygonMapNumbering(fn, (PolygonMapPtr &)ptr);
}

const char* PolygonMapNumbering::sSyntax() {
  return "PolygonMapNumbering(polmap[,,prefix])";
}

PolygonMapNumbering* PolygonMapNumbering::create(const FileName& fn, PolygonMapPtr& p, const String& sExpr)
{
  Array<String> as;
  int iParms = IlwisObjectPtr::iParseParm(sExpr, as);
  if ((iParms < 1) || (iParms > 3))
    throw ErrorExpression(sExpr, sSyntax());
  PolygonMap pmp(as[0], fn.sPath());
  String sNewPrefix;

  if (iParms >= 3)
	  sNewPrefix = as[2];

  if (sNewPrefix.length() == 0)
	  sNewPrefix = "Pol";

  return new PolygonMapNumbering(fn, p, pmp, sNewPrefix);
}

PolygonMapNumbering::PolygonMapNumbering(const FileName& fn, PolygonMapPtr& p)
: PolygonMapVirtual(fn, p)
{
  fNeedFreeze = true;
  String sColName;
  try {
    ReadElement("PolygonMapNumbering", "PolygonMap", pmp);
  }
  catch (const ErrorObject& err) {  // catch to prevent invalid object
    err.Show();
    return;
  }
  Init();
  objdep.Add(pmp.ptr());
}

PolygonMapNumbering::PolygonMapNumbering(const FileName& fn, PolygonMapPtr& p,
  const PolygonMap& pm, const String& sNewPrefix)
: PolygonMapVirtual(fn, p, pm->cs(),pm->cb(),pm->dvrs()),
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
  Store();
}

void PolygonMapNumbering::Store()
{
  PolygonMapVirtual::Store();
  WriteElement("PolygonMapVirtual", "Type", "PolygonMapNumbering");
  WriteElement("PolygonMapNumbering", "PolygonMap", pmp);
}

PolygonMapNumbering::~PolygonMapNumbering()
{
}

String PolygonMapNumbering::sExpression() const
{
  String s("PolygonMapNumbering(%S)", pmp->sNameQuoted(false, fnObj.sPath()));
  return s;
}


void PolygonMapNumbering::Init()
{
  htpFreeze = "ilwisapp\unique_id_algorithm.htm";
  sFreezeTitle = "PolygonMapNumbering";
}

bool PolygonMapNumbering::fFreezing()
{
	long iPol = pmp->iFeatures();
	long iNrNewPol = 0;
	trq.SetText(TR("Determine number of polygons"));
	for (int i=0; i < iPol; ++i) {
		if (trq.fUpdate(i, iPol))
			return false;
		ILWIS::Polygon *pol = (ILWIS::Polygon *)pmp->getFeature(i);
		if (pmp->fRealValues()) {
			if (pol->rValue() == rUNDEF)
				continue;
		}
		else {
			if (pol->iValue() == iUNDEF)
				continue;
		}  
		iNrNewPol++;
	}  
	FileName fnDom = dm()->fnObj;
	
	DomainSort* pdsrt = dm()->pdsrt();
	pdsrt->Resize(iNrNewPol);
	
	CreatePolygonMapStore();

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
	if (!col.fValid())
		col = Column(tbl, pmp->fnObj.sFile, pmp->dvrs());
	col->fErase = true;
	
	Column colArea = tbl->col("Area");
	if ( !colArea.fValid())
		colArea = tbl->colNew("Area",DomainValueRangeStruct(-1e307, 1e307, 0.001));	
	
	trq.SetText(String(TR("Numbering '%S'").c_str(), sName(true, fnObj.sPath())));
	iNrNewPol = 0;
	CoordBuf crdBuf(1000);
	double rVal;
	long iRaw;
	bool fUseReals = pmp->dvrs().fUseReals();
	
	for (int i=0; i < iPol; ++i) {
		if (trq.fUpdate(i, iPol))
			return false;
		ILWIS::Polygon *pol = (ILWIS::Polygon *)pmp->getFeature(i);
		if (!pol  && !pol->fValid())
			continue;

		ILWIS::Polygon *polNew = (ILWIS::Polygon *)pol->clone();
		if (fUseReals) 
		{
			rVal = pol->rValue();
			if (rVal == rUNDEF)
				delete polNew;
			else 
			{
				iNrNewPol++;
				polNew->PutVal(iNrNewPol);
				col->PutVal(iNrNewPol, rVal);
			}	
		}
		else 
		{
			iRaw = pol->iValue();
			if (iRaw == iUNDEF)
				delete polNew;
			else 
			{
				iNrNewPol++;
				polNew->PutVal(iNrNewPol);
				col->PutRaw(iNrNewPol, iRaw);
			}
		}
		if ( !polNew->fDeleted() )
			colArea->PutVal(iNrNewPol, pol->rArea());
		pms->addPolygon(polNew);

	}
	col->SetReadOnly(true);
	col->fErase = false;
	tbl->Updated();
	return true;
}
