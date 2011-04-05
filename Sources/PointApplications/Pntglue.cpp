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
// $Log: /ILWIS 3.0/PointMap/Pntglue.cpp $
 * 
 * 17    12-03-01 16:46 Hendrikse
 * fAllAttrTablesEqual check is now effectively used and in both
 * constructors
 * 
 * 16    12/03/01 12:16 Willem
 * Added checks on validity of attribute tables
 * 
 * 15    20-02-01 19:46 Hendrikse
 * implemted use of fAllAttrTablesEqual; thus avoiding redundant new attr
 * table(s) and their glueing
 * 
 * 14    1-02-01 17:37 Hendrikse
 * removed redundant OutputDomain odFinal;
 * 
 * 13    26-01-01 18:16 Hendrikse
 * made sNewDomain impossible (empty string) for UniqIds (in constructor)
 * added check on fAllMapsHaveAttrTable before gluing attr tables
 * 
 * 12    26-01-01 13:39 Hendrikse
 * added spaces around = sign
 * added flag  afBoolToClass for later use in bool-class merge
 * 
 * 11    25-01-01 17:25 Hendrikse
 * added in fFreezing if (rpr.fValid())  for the sake of (Unique) Ids
 * 
 * 10    23/01/01 14:18 Willem
 * Added code to restore the representation in case of internal dom/rpr in
 * output map
 * 
 * 9     23-01-01 12:09 Hendrikse
 * cosmetic improvements
 * 
 * 8     16-01-01 14:02 Hendrikse
 * put the UNIQUEID case also in the  if (0 != pdsrt)   block in
 * fFreezing(),
 * so that the uniqueId domains are merged before the point maps are
 * 
 * 7     10-01-01 18:21 Hendrikse
 * implented new members use for  CheckAndFindOutputDomain(); function
 * 
 * 6     8-02-00 15:22 Wind
 * TableGlue (in PointMapGlue) was not aware of long file names
 * 
 * 5     10-12-99 13:10 Wind
 * made aiRecode zero based
 * 
 * 4     9/08/99 1:00p Wind
 * changed constructor calls FileName(fn, sExt, true) to FileName(fn,
 * sExt)
 * or changed FileName(fn, sExt, false) to FileName(fn.sFullNameQuoted(),
 * seExt, false)
 * to ensure that proper constructor is called
 * 
 * 3     9/08/99 11:59a Wind
 * comments
 * 
 * 2     9/08/99 10:21a Wind
 * adpated to use of quoted file names
*/
// Revision 1.8  1998/09/16 17:26:27  Wim
// 22beta2
//
// Revision 1.7  1997/09/15 08:13:46  Wim
// Use SetProximity() before check if point is already there to prevent
// rejection of valid points
//
// Revision 1.6  1997-08-25 19:48:46+02  Wim
// Start with an empty domain
//
// Revision 1.5  1997-08-21 13:07:58+02  martin
// The raw values are now correctly put in the internal domain
//
// Revision 1.4  1997/08/04 15:46:13  Wim
// Improved using of masks. Use now PointMap::fPntInMask()
//
// Revision 1.3  1997-07-30 15:57:27+02  Wim
// Corrected sExpression()
//
// Revision 1.2  1997-07-30 11:45:38+02  Wim
// Allow sNewDom instead of internal domain.
// Debugged GPF by replacing i by iMapNr
//
/* PointMapGlue
   Copyright Ilwis System Development ITC
   september 1996, by Jan Hendrikse
	Last change:  WK   29 Jun 98    3:51 pm
*/                                                                      
#define ILWPNTMAPGLUE_C
#include "PointApplications\PNTGLUE.H"
#include "Engine\Map\Point\PNTSTORE.H"
#include "Engine\Base\DataObjects\valrange.h"
#include "Engine\Base\mask.h"
//#include "Engine\Base\DataObjects\Hash.h"
#include "Engine\Domain\dmsort.h"
#include "Engine\Domain\DomainUniqueID.h"
#include "Engine\Representation\Rprclass.h"
#include "Applications\Table\TblGlue.h"
#include "Headers\Err\Ilwisapp.err"
#include "Headers\Htp\Ilwisapp.htp"
#include "Headers\Hs\point.hs"

IlwisObjectPtr * createPointMapGlue(const FileName& fn, IlwisObjectPtr& ptr, const String& sExpr, vector<void *> parms ) {
	if ( sExpr != "")
		return (IlwisObjectPtr *)PointMapGlue::create(fn, (PointMapPtr &)ptr, sExpr);
	else
		return (IlwisObjectPtr *)new PointMapGlue(fn, (PointMapPtr &)ptr);
}

const char* PointMapGlue::sSyntax() {
  return "PointMapGlue(pntmap1,\"mask1\",pntmap2,\"mask2\",...)\n"
"PointMapGlue(pntmap1,\"mask1\",pntmap2,\"mask2\",...,newdom)\n"
"PointMapGlue(MinX,MinY,MaxX,MaxY,pntmap1,\"mask1\",pntmap2,\"mask2\",...)\n"
"PointMapGlue(MinX,MinY,MaxX,MaxY,pntmap1,\"mask1\",pntmap2,\"mask2\",...,newdom)";
}

#define MAX_HASH 16000

class CoordHash
{
    public:
        CoordHash();
        CoordHash(Coord crd) : cCoord(crd)
        {}
        bool operator==(const CoordHash crd) { return crd.cCoord==cCoord; }

        Coord cCoord;
        long iCount;
};

static int iHash(const CoordHash& hcrd)
{
    double dummy;
    double rSum=abs(hcrd.cCoord.x * hcrd.cCoord.y) + 1.0;
    double rFrac=log10(rSum);
    rFrac = modf(rFrac, &dummy);
    return (int)(rFrac*MAX_HASH);
}    

PointMapGlue* PointMapGlue::create(const FileName& fn, PointMapPtr& p, const String& sExpr)
{
	Array<String> as;
	int iParms = IlwisObjectPtr::iParseParm(sExpr, as);
	String sNewDom;
	if (iParms % 2 == 1) {
		sNewDom = as[iParms-1];
		iParms -= 1;
	}
	CoordBounds cb;
	cb.MinX() = (double)as[0].rVal();
	int iFirstPntMapParm = 0;
	if (cb.MinX() != rUNDEF) {
		cb.MinY() = as[1].rVal();
		cb.MaxX() = as[2].rVal();
		cb.MaxY() = as[3].rVal();
		if ( (cb.MinY()==rUNDEF)||(cb.MinY()==rUNDEF)
			||(cb.MinY()==rUNDEF)||(cb.MinY()==rUNDEF) )
			ExpressionError(sExpr, sSyntax());
		iFirstPntMapParm = 4;
	}
	Array<PointMap> apmap;
	Array<String> asMask;
	apmap &= PointMap(as[iFirstPntMapParm], fn.sPath());
	asMask &= as[iFirstPntMapParm+1];
	
	for (int i = iFirstPntMapParm+2; i < iParms; ++i)
	{
		PointMap pmp = PointMap(as[i], fn.sPath());
		apmap &= pmp;
		asMask &= as[++i];
	}
	if ( apmap.iSize() == 0 )
		ExpressionError(sExpr, sSyntax());
	return new PointMapGlue(fn, p, cb, apmap, asMask, sNewDom);
}

PointMapGlue::PointMapGlue(const FileName& fn, PointMapPtr& p)
: PointMapVirtual(fn, p)
{
	Init();
	ReadElement("PointMapGlue", "NrPointMap", iPntMap);
	ReadElement("PointMapGlue", "NewDomain", sNewDom);
	ReadElement("PointMapGlue", "UsesClipping", fUsesClipping); 
	apmp.Resize(iPntMap);
	asMask.Resize(iPntMap);
	for (int i = 0;  i < iPntMap; ++i)
	{
		try
		{
			ReadElement("PointMapGlue", String("PointMap%i", i).scVal(), apmp[i]);
		}
		catch (const ErrorObject& err) {  // catch to prevent invalid object
			err.Show();
			return;
		}
		ReadElement("PointMapGlue", String("PointMapMask%i", i).scVal(), asMask[i]);
	}
	Table tblAtt0 = apmp[0]->tblAtt();
	fAllAttrTablesEqual = tblAtt0.fValid();
	// loop needs to be traversed completely because of objdep.Add()
	for (int i = 1;  i < iPntMap; ++i)
	{
		objdep.Add(apmp[i].ptr());
		Table& tbl = apmp[i]->tblAtt();
		if (fAllAttrTablesEqual)
			fAllAttrTablesEqual &= (tbl.fValid() && tblAtt0 == tbl);
	}
}

PointMapGlue::PointMapGlue(const FileName& fn, PointMapPtr& p, const CoordBounds& cbnd,
                               const Array<PointMap>& apmap,
                               const Array<String>& asMsk,
                               const String& sNewDomain)
: PointMapVirtual(fn, p, apmap[0]->cs(),cbnd, apmap[0]->dvrs()),
  sNewDom(sNewDomain),
  fUsesClipping(false)
{
	Init();
	iPntMap = apmap.iSize();
	Table tblAtt0 = apmap[0]->tblAtt();
	fAllAttrTablesEqual = tblAtt0.fValid();
	for (int i = 0; i < iPntMap; i++)
	{
		apmp &= apmap[i];
		asMask &= asMsk[i];
		Table& tbl = apmp[i]->tblAtt();
		if (fAllAttrTablesEqual)
			fAllAttrTablesEqual &= (tbl.fValid() && tblAtt0 == tbl);
	}
	for (int i = 1; i < iPntMap; i++) {
		if (!cs()->fConvertFrom(apmp[i]->cs()))
			IncompatibleCoordSystemsError(cs()->sName(true, fnObj.sPath()), apmp[i]->cs()->sName(true, fnObj.sPath()), sTypeName(), errPointMapGlue);
	}

	//OutputDomain odFinal;
	CheckAndFindOutputDomain(iPntMap, apmp, sTypeName(), odFinal);
	if (odFinal == odUNIQUEID)
		sNewDom = String("");

	DomainValueRangeStruct ndvr;
	Domain dom = ndvr.dm();
	if ( sNewDom.length())
	{
		if (odFinal == odID)
			dom = Domain(sNewDom, 0, dmtID);
		else if (odFinal == odCLASS)
			dom = Domain(sNewDom, 0, dmtCLASS);
		ndvr.SetDomain(dom);
	}
	else
	{
		if (odFinal == odID)
			dom = Domain(fnObj, 0, dmtID);
		else if (odFinal == odCLASS)
			dom = Domain(fnObj, 0, dmtCLASS);
		ndvr.SetDomain(dom);
	}
	
	if (odFinal == odBOOL)	
		ndvr.SetDomain( Domain("bool"));
	else if (odFinal == odVALUE)	
		ndvr.SetDomain( Domain("value"));
	
	if (cb().fUndef()) 
	{
		CoordBounds cbUnion = apmp[0]->cb();
		for (int i = 1; i < iPntMap; i++) 
		{
			CoordBounds cb = apmp[i]->cb();
			if (cs() != apmp[i]->cs())
				cb = cs()->cbConv(apmp[i]->cs(), cb);
			cbUnion += cb;
		}  
		SetCoordBounds(cbUnion);
	}
	else
		fUsesClipping=true;
	for (int i=0;  i < iPntMap; ++i)
		objdep.Add(apmp[i].ptr());
	if (!fnObj.fValid()) // 'inline' object
		objtime = objdep.tmNewest();
}

void PointMapGlue::Store()
{
  PointMapVirtual::Store();
  WriteElement("PointMapVirtual", "Type", "PointMapGlue");
  WriteElement("PointMapGlue", "NrPointMap", (long)iPntMap);
  for (int i=0;  i < iPntMap; ++i) {
    WriteElement("PointMapGlue", String("PointMap%i", i).scVal(), apmp[i]);
    WriteElement("PointMapGlue", String("PointMapMask%i",i).scVal(), asMask[i]);
  }
  if (sNewDom.length())
    WriteElement("PointMapGlue", "NewDomain", sNewDom);
  else
    WriteElement("PointMapGlue", "NewDomain", (char*)0);
  WriteElement("PointMapGlue", "UsesClipping", fUsesClipping); 
}

PointMapGlue::~PointMapGlue()
{
}

String PointMapGlue::sExpression() const
{
  String s = "PointMapGlue(";
  if (fUsesClipping)
  {
    s&=String("%f, %f, %f, %f,", cb().MinX(), cb().MinY(), cb().MaxX(), cb().MaxY());
  }      
  for (int i=0;  i < iPntMap; ++i) {
    s &= apmp[i]->sNameQuoted(false, fnObj.sPath());
    s &= String(", \"%S\"", asMask[i]);
    if (i < iPntMap - 1)
      s &= ", ";
  }
  if (sNewDom.length())
    s &= String(", %S)", sNewDom);
  else
    s &= ')';
  return s;
}

bool PointMapGlue::fDomainChangeable() const
{
	return true;
}

void PointMapGlue::Init()
{
	htpFreeze = htpPointMapGlueT;
	sFreezeTitle = "PointMapGlue";
	fNeedFreeze = true;
}

bool PointMapGlue::fFreezing()
{
	CheckAndFindOutputDomain(iPntMap, apmp, sTypeName(), odFinal);
	DomainSort* pdsrt = apmp[0]->dm()->pdsrt();
	Domain dom;
	ValueRange vr;
	bool fSameDom = true;
	for (int i=1; i < iPntMap; ++i)
		if (apmp[0]->dvrs() != apmp[i]->dvrs()) {
			fSameDom = false;
			break;
		}

	Array<bool> afBoolToClass;// flag needed to glue bool with class
	afBoolToClass.Resize(iPntMap);// still to be implemented (26 jan 2001)
	if (odFinal == odCLASS)
		for (int i = 1; i < iPntMap; ++i) {
			if (apmp[i]->dm()->pdbool()) 
				afBoolToClass[i] = true;
			else
				afBoolToClass[i] = false;
		}

	if (!fSameDom) {
		if (0 != pdsrt) {
			trq.SetText(SPNTTextCreatingNewDomain);
			if (sNewDom.length()) {
				if (odFinal == odUNIQUEID)
					dom = Domain(fnObj, 0, dmtUNIQUEID, apmp[0]->dm()->pdUniqueID()->sGetPrefix());
				else if (odFinal == odID) 
					dom = Domain(sNewDom, 0, dmtID);
				else
					dom = Domain(sNewDom, 0, dmtCLASS);
			}
			else {
				if (odFinal == odUNIQUEID) 
					dom = Domain(fnObj, 0, dmtUNIQUEID, apmp[0]->dm()->pdUniqueID()->sGetPrefix());
				else if (odFinal == odID)
					dom = Domain(fnObj, 0, dmtID);
				else
					dom = Domain(fnObj, 0, dmtCLASS);
			}
			// merge codes of all 'sort' - domains
			DomainSort* pdsrt1 = dom->pdsrt();
			pdsrt1->Resize(0); // start empty !
			pdsrt1->dsType = DomainSort::dsMANUAL;
			for (int i = 0; i < iPntMap; ++i) {
				DomainSort* pdsrt2 = apmp[i]->dm()->pdsrt();
				if (trq.fAborted())
					return false;
				pdsrt1->Merge(pdsrt2, &trq);
			}
		}
		else {
			if (odFinal == odBOOL) {
				dom = Domain("bool");
				vr = ValueRange(0,1,1);
			}
			else {
				dom = Domain("value");
				// determine value range
				double rMin = DBL_MAX;
				double rMax = -DBL_MAX;
				double rStep = 1;
				for (int i = 0; i < iPntMap; ++i) {
					if (!apmp[i]->dvrs().fValues())
						continue;
					RangeReal rr = apmp[i]->rrMinMax(BaseMapPtr::mmmCALCULATE);
					if (rr.rLo() < rMin)
						rMin = rr.rLo();
					if (rr.rHi() > rMax)
						rMax = rr.rHi();
					if (apmp[i]->dvrs().rStep() < rStep)
						rStep = apmp[i]->dvrs().rStep();
				}
				if (rMin <= rMax)
					vr = ValueRange(rMin, rMax, rStep);
			}
		}
		// Keep copy of representation: In case of internal domain
		// and representation the RPR info is lost because the coming
		// unfreeze will clear the ODF and the RPR info is only stored there.
		Representation rpr = dom->rpr();
		SetDomainValueRangeStruct(DomainValueRangeStruct(dom, vr));

		// Administration needed to account for the fact that UnFreeze() has been
		// called. (the next four(!) statements)
		if (rpr.fValid()) {
			rpr->Store();
			dom->SetRepresentation(rpr);
		}
		CreatePointMapStore();
		Store();
	}

	HashTable<CoordHash> crdHashTab(MAX_HASH);
	int iLastUNIQUEID = 0;
	if (dom.fValid())
		pdsrt = dom->pdsrt();
	for (int iMapNr=0; iMapNr < iPntMap; ++iMapNr)  // do for each input map
	{
		ArrayLarge<long> aiRecode; // now zero based  // 'lookup table' of new codes for input domain
		if (0 != pdsrt) {
			DomainSort* pdsrt1 = apmp[iMapNr]->dm()->pdsrt();
			aiRecode.Resize(pdsrt1->iSize()); // now zero based // in these arrays we find the mapping from old Raw values in
			for (unsigned long i = 1; i <= aiRecode.iSize(); i++) {
				if (odFinal == odUNIQUEID) 
					aiRecode[i-1] = iLastUNIQUEID + i;
				else
					aiRecode[i-1] = pdsrt->iRaw(pdsrt1->sNameByRaw(i,0));
			}
			if (!fAllAttrTablesEqual)
				iLastUNIQUEID += pdsrt1->iSize();
		}
		CoordSystem csOld = apmp[iMapNr]->cs();
		bool fTransformCoords = cs() != csOld;
		CoordBounds cbnd = apmp[iMapNr]->cb();
		if (fTransformCoords)
			cbnd = cs()->cbConv(csOld, cbnd);
		if (cb().fContains(cbnd)) {  // apmp[i] will be (partly) glued
			PointMap pmp = apmp[iMapNr];
			String sMask = asMask[iMapNr];
			trq.SetText(String(SPNTTextCopyWithMask_SS.scVal(), pmp->sName(true, fnObj.sPath()), sMask));
			Mask mask(pmp->dm(), sMask);
			long iPnt = pmp->iFeatures();
			for (long i = 0; i < pmp->iFeatures(); ++i)
			{
				if (trq.fUpdate(i, iPnt))
					return false;

				if (!pmp->fPntInMask(i, mask))  // if stringvalue doesn't satisfy the mask
					continue;                         // then continue with next point recordnr i
				Coord crd = pmp->cValue(i);
				if (fTransformCoords)
					crd = cs()->cConv(csOld, crd);
				ptr.SetProximity(0);

				CoordHash hcrd(crd);
				if ( crdHashTab.fPresent(hcrd) ) continue;
				else crdHashTab.add(hcrd);
      
				if (cb().fContains(crd)) {
					if (dvrs().fValues())
						pms->iAddVal(crd, pmp->rValue(i));
					else {
						long iRaw = pmp->iRaw(i);
						if ((iRaw > 0) && (iRaw <= (int)aiRecode.iSize()))
							iRaw = aiRecode[iRaw-1];
						else
							iRaw = iUNDEF;
						pms->iAddRaw(crd, iRaw);
					}
				}  
			} // end for pnt.fValid
			trq.fUpdate(iPnt,iPnt);               // fill the tranqil bar totally
		} // end if apmp[i] partly in cbOuter

	}// end for i <  iPntMap
	_iPoints = pms->iPnt();  // pointmapvirt gets total nr of points from pointmapstore

	bool fAllMapsHaveAttrTable = true;
	for (int iMapNr=0; iMapNr < iPntMap; ++iMapNr)  // do for each input map
	{
		if ((!apmp[iMapNr].fValid()) || (!apmp[iMapNr]->fTblAtt())) {
			fAllMapsHaveAttrTable = false;
			break;
		}
	}
	if (fAllMapsHaveAttrTable)
		GlueAttributeTables();
       
	return true;
}

PointMapGlue::OutputDomain PointMapGlue::FindOutputDomain
										(PointMapGlue::OutputDomain odOut, const Domain dm_i)
{ 
	//at first call OutputDomain is odNOTYETKNOWN;
  if ( odOut == odNOTYETKNOWN) {	
		if (dm_i->pdUniqueID()) 				return odUNIQUEID;
		if (dm_i->pdc())								return odCLASS;
		if (dm_i->pdid())								return odID;
		if (dm_i->pdbool())							return odBOOL;
		if (dm_i->pdv()) 								return odVALUE;
	}
		// combination of equal domain types
  if ( odOut == odUNIQUEID	&& dm_i->pdUniqueID()) 			return odUNIQUEID;
  if ( odOut == odID				&& dm_i->pdid())						return odID;
	if ( odOut == odCLASS			&& dm_i->pdc())							return odCLASS;
	if ( odOut == odVALUE			&& dm_i->pdv())							return odVALUE;
	if ( odOut == odBOOL			&& dm_i->pdbool())					return odBOOL;
	
	 // combination of different domain types:
	if ((odOut == odUNIQUEID && dm_i->pdid()) || (odOut == odID && dm_i->pdUniqueID()))	
																												return odID;
	if ((odOut == odCLASS && dm_i->pdUniqueID()) || (odOut == odUNIQUEID && dm_i->pdc()))	
																												return odID;
	if ((odOut == odCLASS && dm_i->pdid()) || (odOut == odID && dm_i->pdc()))	
																												return odID;
  //if ((odOut == odCLASS && dm_i->pdbool()) || (odOut == odBOOL && dm_i->pdc()))	
	//return odCLASS;  /// refuse this combin untill it's implemented (25-1-01)
  if ((odOut == odVALUE && dm_i->pdbool()) || (odOut == odBOOL && dm_i->pdv()))	
																												return odVALUE;
  
	return odINVALID;
}

void PointMapGlue::CheckAndFindOutputDomain(const long iPntMap, const Array<PointMap> apmp,
																						String sTypeN, OutputDomain& odOut)
{
	OutputDomain odResult = odNOTYETKNOWN;
	bool fSameDom = true;
	for (int i = 0; i < iPntMap; ++i){
		if (apmp[0]->dvrs() != apmp[i]->dvrs()) {
			fSameDom = false;
			break;
		}
	}
	if (fSameDom) 
		odResult = FindOutputDomain(odResult, apmp[0]->dm()); // just one single domain
	else {
		for (int i = 0; i < iPntMap; ++i) {
			odResult = FindOutputDomain(odResult, apmp[i]->dm());
			if (odResult == odINVALID)
				IncompatibleDomainsError(apmp[0]->dm()->sName(), apmp[i]->dm()->sName(),
				sTypeN, errPointMapGlue+1);
		}
		for (int i = 0; i < iPntMap; ++i) {  // check twice to remove non-associativity risk
			odResult = FindOutputDomain(odResult, apmp[i]->dm());
			if (odResult == odINVALID)
				IncompatibleDomainsError(apmp[0]->dm()->sName(), apmp[i]->dm()->sName(),
				sTypeN, errPointMapGlue+1);
		}
	}
	odOut = odResult;  // domains are combinable
}

void PointMapGlue::GlueAttributeTables()
{
  String sExpr("TableGlue(%S", dm()->fnObj.sFullNameQuoted(true));
  int iCount = 0;
  for (int iMapNr = 0; iMapNr < iPntMap; ++iMapNr)  // do for each input map
  {
    if (!apmp[iMapNr].fValid()) continue;
    if (apmp[iMapNr]->fTblAtt())
    {
        sExpr &= ",";
        sExpr &= apmp[iMapNr]->tblAtt()->fnObj.sFullNameQuoted(true);
        iCount++;
    }        
  }
  if (iCount>0) {// length original string
		if (fAllAttrTablesEqual)
				SetAttributeTable(apmp[0]->tblAtt());
		else
		{
			sExpr&=")";
			FileName fn = FileName::fnUnique(FileName(fnObj,".tbt"));
			Table tbl(fn, sExpr);
			tbl->fErase = true;
			tbl->Calc();
			SetAttributeTable(tbl);
			tbl->fErase=false;
		}  
	}
}




