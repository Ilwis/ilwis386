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
/*// $Log: /ILWIS 3.0/BasicDataStructures/dmgroup.cpp $
 * 
 * 10    13-03-03 17:31 Willem
 * - Changed: Sorting a domain group is required, also when it is
 * ReadOnly. This is done in memory, so the fUpdateRequired parameter of
 * CalcIndex must be set to false
 * 
 * 9     9-10-00 8:46a Martin
 * setupperbounds will set the updated flag
 * 
 * 8     21/08/00 11:59 Willem
 * - Sort() function now only sorts active items
 * - In Merge() function the ReSort() function has been replaced by
 * Init(). The ReSort() function is for DomainClass only
 * 
 * 7     5/17/00 10:22a Wind
 * added fEqual functions
 * 
 * 6     31/03/00 14:08 Willem
 * - DomainGroup now has its own virtual merge() function that also that
 * into account the upper boundaries
 * - DomainGroup now has its own virtual Delete() function that resets the
 * upper boundary of the item to delete to rUNDEF
 * 
 * 5     24/03/00 10:53 Willem
 * DomainGroup now handles deleted items correctly when sorting
 * 
 * 4     24-12-99 11:56 Wind
 * made sorting thread save
 * 
 * 3     23-12-99 9:55 Wind
 * comments
 * 
 * 2     22-12-99 17:52 Wind
 * removed GroupFunction and added functionality to DomainGroup
*/
// Revision 1.3  1998/09/16 17:25:17  Wim
// 22beta2
//
// Revision 1.2  1997/09/04 13:13:51  Wim
// ValueRange of Columns "Bounds" is now specifically set
//
/* DomainGroup
   Copyright Ilwis System Development ITC
   march 1995, by Wim Koolhoven
	Last change:  WK    4 Sep 97    3:11 pm
*/
#include "Engine\Domain\dmgroup.h"
#include "Engine\Table\tblstore.h"
#include "Engine\Representation\Rprclass.h"
#include <algorithm>

class CompareGroup
{
public:
	CompareGroup(DomainGroup* ds)
	{
		_ds = ds;
	}
	bool operator()(const long& a, const long& b)
	{
		if (a == iUNDEF)
			return false;
		else if (b == iUNDEF)
			return true;
		double rA = _ds->rUpper(a);
		double rB = _ds->rUpper(b);
		return rA < rB;
	}
	DomainGroup* _ds;
};


DomainGroup::DomainGroup(const FileName& fn)
: DomainClass(fn), buf(0)
{
  dsType = dsAUTO;
  if (tbl.fValid())
    colBnd = tbl->col("Bounds");
  ObjectTime tim = objtime, timTbl;
  if (tbl.fValid()) {
    timTbl = tbl->objtime;
    Sort();
  }
  fChanged = false;
  objtime = tim;
  if (tbl.fValid()) {
    tbl->fChanged = false;
    tbl->objtime = timTbl;
  } 
  Init();
}

DomainGroup::DomainGroup(const FileName& fn, long iNr)
: DomainClass(fn, iNr), buf(0)
{
  dsType = dsAUTO;
  if (!tbl.fValid())
    CreateColumns();
  Domain dmType = Domain("value");
  ValueRange vr(-1e100,1e100,0);
  if (tbl.fValid())
    colBnd = tbl->colNew("Bounds", dmType, vr);
  fChanged = true;
  Init();
  Store();
}

DomainGroup::~DomainGroup()
{
  if (0 != buf)
    delete [] buf;
}

void DomainGroup::Store()
{
  DomainClass::Store();
  WriteElement("Domain", "Type", "DomainGroup");
}

String DomainGroup::sType() const
{
  return "Domain Group";
}

long DomainGroup::iClassify(double rVal) const
{
  if (rVal == rUNDEF)
    return iUNDEF;
  long i1 = iSize()-1;
  if (rVal > buf[i1].bnd)
    return iUNDEF;
  long i0 = 0;
  while (i0 < i1) {
    long iRes = i0/2 + i1/2;
    if (rVal <= buf[iRes].bnd)
      i1 = iRes;
    else 
      i0 = iRes + 1;
  }
  return buf[i0].cls;
}

void DomainGroup::SetUpperBound(long iRaw, double rVal)
{
  if (!colBnd.fValid())
    return;
  colBnd->PutVal(iRaw, rVal);
  Sort();
  Init();
	Updated();
}

double DomainGroup::rLower(long iRaw) const
{
  if (!colBnd.fValid())
    return rUNDEF;
  double rTmp = colBnd->rValue(iRaw);
  rTmp -= 1e-6;
  long iUnder = iClassify(rTmp);
  return colBnd->rValue(iUnder);
}

double DomainGroup::rUpper(long iRaw) const
{
  if (!colBnd.fValid())
    return iUNDEF;
  return colBnd->rValue(iRaw);
}

RangeReal DomainGroup::rr(long iRaw) const
{
  return RangeReal(rLower(iRaw), rUpper(iRaw));
}

static int sortClsBnd(const void* a, const void* b)
{
  const DomainGroup::clsbnd* cbA = (const DomainGroup::clsbnd*)a;
  const DomainGroup::clsbnd* cbB = (const DomainGroup::clsbnd*)b;
  if (cbA->bnd < cbB->bnd)
    return -1;
  else if (cbA->bnd > cbB->bnd)
    return 1;
  else
    return 0;    
}

void DomainGroup::Init()
{
	int iNr = iSize();
	if (0 != buf)
		delete buf;
	buf = new clsbnd[iNr];
	for (long i = 0; i < iNr; ++i)
	{
		buf[i].cls = i + 1;
		double rUppBnd = rUpper(i+1);
		buf[i].bnd = rUppBnd;
	}  
	qsort(buf, iNr, sizeof(clsbnd), sortClsBnd);
}


void DomainGroup::Sort()
{
	int iNr = iNettoSize();
	vector<long> index(iNr);
	int i;
	for (i = 0; i < iNr; ++i)
		index[i] = iKey(1+i);
	CompareGroup cds(this);
	sort(index.begin(), index.end(), cds);
	for (i = 0; i < iNr; ++i)
		colOrd->PutRaw(index[i], 1+i);
	index.resize(0); 
	CalcIndex(false);
}

void DomainGroup::Merge(const DomainSort* pdsrt, Tranquilizer* trq)
{
	if (fReadOnly())
		throw ErrorNotCreatedOpen(fnObj);
	
	Representation rpr1 = const_cast<DomainGroup *>(this)->rpr();
	Representation rpr2 = const_cast<DomainSort *>(pdsrt)->rpr();
	RepresentationClass* rprc1 = 0;
	if (rpr1.fValid())
		rprc1 = rpr1->prc();
	RepresentationClass* rprc2 = 0;
	if (rpr2.fValid())
		rprc2 = rpr2->prc();
	long iNew = 0;
	if ( trq)
	{
		trq->SetTitle(SDATTextMergingDomains);
		trq->SetText(SDATTextSizeNewDomain);
	}
	DomainGroup* pdg2 = dynamic_cast<DomainGroup*>(const_cast<DomainSort*>(pdsrt));
	for (long i = 1; i <= pdsrt->iSize(); ++i) 
	{
		if ( trq && trq->fUpdate(i, pdsrt->iSize())) 
			return;
		if (pdg2)
		{
			double rNewBnd = pdg2->rUpper(i);
			double rBnd = rUpper(iClassify(rNewBnd));
			
			// rBnd <> iUNDEF: found upper boundary >= new boundary
			if (rBnd != iUNDEF && abs(rBnd - rNewBnd) < 1.0e-6)
				continue;
		}
		if (pdsrt->iOrd(i) != iUNDEF) 
		{
			String sClass = pdsrt->sNameByRaw(i, 0);
			long iRaw = DomainSort::iRaw(sClass);
			long iRawNameCode = iFindCodeRaw(sClass);  // 2nd name exists as code
			if (iRaw == iUNDEF || iRawNameCode != iUNDEF)
				iNew++;
		}
	}
	if (iNew == 0)
		return;
	long iCurrNr = iSize();
	
	DomainSortingType dst = dsType;
	dsType = dsMANUAL;
	if (!tbl.fValid())
		CreateColumns();
	tbl->iRecNew(iNew);
	Updated();
	Table::EnlargeTables(this);
	long iStartInd=iNettoSize();
	if (trq) trq->SetText(SDATTextAddToDomain);
	for (int i = 1; i <= pdsrt->iSize(); ++i) 
	{
		if ( trq && trq->fUpdate(i, pdsrt->iSize())) 
			return ;
		
		double rNewBnd;
		if (pdg2)
		{
			rNewBnd = pdg2->rUpper(i);
			double rBnd = rUpper(iClassify(rNewBnd));
			
			// rBnd <> iUNDEF: found upper boundary >= new boundary
			if (rBnd != iUNDEF && abs(rBnd - rNewBnd) < 1.0e-6)
				continue;
		}
		if (pdsrt->iOrd(i) != iUNDEF) 
		{
			String sClass = pdsrt->sNameByRaw(i, 0);
			String sCode = pdsrt->sCodeByRaw(i, 0);
			long iRaw = iFindNameRaw(sClass);
			
			if (iRaw == iUNDEF)
			{
				++iNr;
				++iCurrNr;
				colOrd->PutRaw(iCurrNr, ++iStartInd);
				colInd->PutRaw(iStartInd, iCurrNr);
				
				long iRawCode = iOrd(sCode);   // 2nd code exists as name or code in current domain
				long iRawNameCode = iFindCodeRaw(sClass);  // 2nd name exists as code
				
				if (iOrd(iRawNameCode) != iUNDEF)
					SetCode(iRawNameCode, "");  // remove the code that will be merged as a name from second domain
				
				SetVal(iCurrNr, sClass);
				
				if (pdsrt->fCodesAvailable() && iRawCode == iUNDEF) // Add code only if the code is not in the current domain
					SetCode(iCurrNr, sCode);
				else if (fCodesAvailable())
					SetCode(iCurrNr, "");
				
				if (pdsrt->fDescriptionsAvailable())
					SetDescription(iCurrNr, pdsrt->sDescriptionByRaw(i));
				else if (fDescriptionsAvailable())
					SetDescription(iCurrNr, "");
				
				if (0 != rprc1 && 0 != rprc2)
					rprc1->PutColor(iCurrNr,rprc2->clrRaw(i));
				
				SetUpperBound(iCurrNr, pdg2 ? rNewBnd : rUNDEF);
			}
		}
	}
	dsType = dst;
	Init();
	
	Updated();
}

void DomainGroup::Delete(long raw)
{
	if (fReadOnly())
		throw ErrorNotCreatedOpen(fnObj);

	DomainSort::Delete(raw);

	SetUpperBound(raw, rUNDEF);
}

bool DomainGroup::fEqual(const IlwisObjectPtr& ptr) const
{
  const DomainGroup* pdg = dynamic_cast<const DomainGroup*>(&ptr);
  if (pdg == 0)
    return false;
  if (!DomainSort::fEqual(ptr))
		return false;
	for (int i = 1; i <= iSize(); ++i) 
		if (rr(i) != pdg->rr(i))
			return false;
  return true;
}
