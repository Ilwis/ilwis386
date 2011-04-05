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
// $Log: /ILWIS 3.0/SegmentMap/Segglue.cpp $
 * 
 * 18    15-11-01 12:07 Retsios
 * Now also return clipping region in sExpression (bug#4931), analog to
 * polglue.
 * Removed unused variable.
 * 
 * 17    20-03-01 17:16 Hendrikse
 * Corrected wrong counter (index in
 * else if (0 != asmp[iMapNr]->dm()->pdbool()) 
 * in fFreezing()
 * 
 * 16    12-03-01 16:50 Hendrikse
 * fAllAttrTablesEqual check is implemented and in both constructors.
 * Merged output domain in case of dom sort is now properly set in
 * fFreezing before starting to put segments in the output map usinf
 * aiRecode table.
 * Improved index name: iMapNr for counting input maps 
 * 
 * 15    22-02-01 21:36 Hendrikse
 * ErrorObject in create() now prevents use of attrib columns in input
 * segmaps
 * 
 * 14    20-02-01 19:45 Hendrikse
 * implemted use of fAllAttrTablesEqual; thus avoiding redundant new attr
 * table(s) and their glueing
 * 
 * 13    19-02-01 18:50 Hendrikse
 * added fAllAttrTablesEqual to avoid new attrib tables if not needed
 * 
 * 12    30-01-01 17:46 Hendrikse
 * added and used bool fAllMapsHaveAttrTable = true;  in fFreezing()
 * 
 * 11    30-01-01 17:06 Hendrikse
 * implemented 	enum OutputDomain and its use similar to pointmapglue
 * 
 * 10    2-10-00 2:42p Martin
 * if needed calc minmax is forced
 * 
 * 9     4/03/00 5:47p Hendrikse
 * corrected in fFreezing:   segNew.PutCoords(iCrdOut, acrdBuf[j]);
 * and removed some obsolete buf definitions
 * 
 * 8     8-02-00 15:20 Wind
 * TableGlue (used in segmentmap glue) was not aware of long file names
 * 
 * 7     17-01-00 8:17a Martin
 * changed rowcols to coords
 * 
 * 6     10-12-99 13:08 Wind
 * made aiRecode zero based
 * 
 * 5     10-12-99 11:48a Martin
 * removed internal rowcols and replaced them by true coords
 * 
 * 4     9/08/99 12:55p Wind
 * changed constructor calls FileName(fn, sExt, true) to FileName(fn,
 * sExt)
 * or changed FileName(fn, sExt, false) to FileName(fn.sFullNameQuoted(),
 * seExt, false)
 * to ensure that proper constructor is called
 * 
 * 3     9/08/99 12:02p Wind
 * comments
 * 
 * 2     9/08/99 10:26a Wind
 * adpated to use of quoted file names in sExpression()
*/
// Revision 1.5  1998/09/16 17:25:20  Wim
// 22beta2
//
// Revision 1.4  1997/08/25 16:06:12  janh
// In fFreezing iRaw counts now until airecode.iSize included
//
// Revision 1.3  1997/08/04 15:36:21  Wim
// Improved use of mask. Use now Segment::fInMask(mask)
//
// Revision 1.2  1997-07-30 15:59:46+02  Wim
// Added sNewDom option to create a new userdefined domain instead of an internal domain
//
/* SegmentMapGlue
   Copyright Ilwis System Development ITC
   september 1996, by Jan Hendrikse
	Last change:  WK   29 Jun 98    3:53 pm
*/                                                                      
#define ILWSEGMAPGLUE_C
#include "SegmentApplications\SEGGLUE.H"
#include "Engine\Base\DataObjects\valrange.h"
#include "Engine\Base\mask.h"
#include "Engine\Domain\dmsort.h"
#include "Engine\Domain\DomainUniqueID.h"
#include "Engine\Representation\Rprclass.h"
#include "Headers\Err\Ilwisapp.err"
#include "Headers\Htp\Ilwisapp.htp"
#include "Headers\Hs\segment.hs"

IlwisObjectPtr * createSegmentMapGlue(const FileName& fn, IlwisObjectPtr& ptr, const String& sExpr, vector<void *> parms ) {
	if ( sExpr != "")
		return (IlwisObjectPtr *)SegmentMapGlue::create(fn, (SegmentMapPtr &)ptr, sExpr);
	else
		return (IlwisObjectPtr *)new SegmentMapGlue(fn, (SegmentMapPtr &)ptr);
}

const char* SegmentMapGlue::sSyntax() {
  return "SegmentMapGlue(segmap1,\"mask1\",segmap2,\"mask2\",...)\n"
"SegmentMapGlue(segmap1,\"mask1\",segmap2,\"mask2\",...,newdom)\n"
"SegmentMapGlue(MinX,MinY,MaxX,MaxY,segmap1,\"mask1\",segmap2,\"mask2\",...)\n"
"SegmentMapGlue(MinX,MinY,MaxX,MaxY,segmap1,\"mask1\",segmap2,\"mask2\",...,newdom)";
}

SegmentMapGlue* SegmentMapGlue::create(const FileName& fn, SegmentMapPtr& p, const String& sExpr)
{
  Array<String> as;
  int iParms = IlwisObjectPtr::iParseParm(sExpr, as);
  String sNewDom;
  if (iParms % 2 == 1) {
    sNewDom = as[iParms-1];
    iParms -= 1;
  }
  CoordBounds cb;
  cb.MinX() = as[0].rVal();
  int iFirstSegMapParm = 0;
  if (cb.MinX() != rUNDEF) {
    cb.MinY() = as[1].rVal();
    cb.MaxX() = as[2].rVal();
    cb.MaxY() = as[3].rVal();
    if ( (cb.MinY()==rUNDEF)||(cb.MinY()==rUNDEF)
          ||(cb.MinY()==rUNDEF)||(cb.MinY()==rUNDEF) )
      ExpressionError(sExpr, sSyntax());
    iFirstSegMapParm = 4;
  }
  Array<SegmentMap> asmp;
  Array<String> asMask;
  asmp &= SegmentMap(as[iFirstSegMapParm], fn.sPath());
  asMask &= as[iFirstSegMapParm+1];
//  CoordSystem cs0 = asmp[0]->cs();
//  Domain dm0 = asmp[0]->dm();
  /// int j = 0;   // number of segm maps glued to asmp[0]
  for (int i=iFirstSegMapParm+2; i < iParms; ++i) 
  {
		String sInputSegMapName = as[i];
		char *pCh = sInputSegMapName.strrchrQuoted('.');
		if ((pCh != 0) && (0 != _strcmpi(pCh, ".mps")))  // attrib map
			throw ErrorObject(WhatError(String(SSEGErrNoAttColumnAllowed_S.scVal(), as[i]),
																	 errSegmentMapTransform), fn);
    SegmentMap smp = SegmentMap(as[i], fn.sPath());
    asmp &= smp;
    asMask &= as[++i];
  }
  if ( asmp.iSize() == 0 )
    ExpressionError(sExpr, sSyntax());
  return new SegmentMapGlue(fn, p, cb, asmp, asMask, sNewDom);
}

SegmentMapGlue::SegmentMapGlue(const FileName& fn, SegmentMapPtr& p)
: SegmentMapVirtual(fn, p)
{
	Init();
  ReadElement("SegmentMapGlue", "NrSegmentMap", iSegMap);
  ReadElement("SegmentMapGlue", "NewDomain", sNewDom);
	ReadElement("SegmentMapGlue", "UsesClipping", fUsesClipping); 
  asmp.Resize(iSegMap);
  asMask.Resize(iSegMap);
  for (int i=0;  i < iSegMap; ++i) {
    try {
      ReadElement("SegmentMapGlue", String("SegmentMap%i", i).scVal(), asmp[i]);
    }
    catch (const ErrorObject& err) {  // catch to prevent invalid object
      err.Show();
      return;
    }
    ReadElement("SegmentMapGlue", String("SegmentMapMask%i", i).scVal(), asMask[i]);
  }  
	Table tblAtt0 = asmp[0]->tblAtt();
	fAllAttrTablesEqual = tblAtt0.fValid();
  for (int i = 1;  i < iSegMap; ++i) {
    objdep.Add(asmp[i].ptr());
		Table tbl = asmp[i]->tblAtt();
		if (fAllAttrTablesEqual)
			fAllAttrTablesEqual &= (tbl.fValid() && tblAtt0 == tbl);
	}
}

SegmentMapGlue::SegmentMapGlue(const FileName& fn, SegmentMapPtr& p, const CoordBounds& _cb,
                               const Array<SegmentMap>& asmap, 
                               const Array<String>& asMsk,
                               const String& sNewDomain)
: SegmentMapVirtual(fn, p, asmap[0]->cs(),_cb, asmap[0]->dvrs()),
  sNewDom(sNewDomain),
  fUsesClipping(false)
{
  Init();
  iSegMap = asmap.iSize();
	Table tblAtt0 = asmap[0]->tblAtt();
	fAllAttrTablesEqual = tblAtt0.fValid();
  for (int i = 0; i < iSegMap; i++) {
    asmp &= asmap[i];
    asMask &= asMsk[i];
		Table tbl = asmp[i]->tblAtt();
		if (fAllAttrTablesEqual)
			fAllAttrTablesEqual &= (tbl.fValid() && tblAtt0 == tbl);
  }  
  for (int i = 1; i < iSegMap; i++) {
    if (!cs()->fConvertFrom(asmp[i]->cs()))
      IncompatibleCoordSystemsError(cs()->sName(true, fnObj.sPath()), asmp[i]->cs()->sName(true, fnObj.sPath()), sTypeName(), errSegmentMapGlue);
  } 
	//OutputDomain odFinal;
	CheckAndFindOutputDomain(iSegMap, asmp, sTypeName(), odFinal);
/*
  DomainValueRangeStruct ndvr;
	Domain dom = ndvr.dm();
	if (odFinal == odUNIQUEID)
		ndvr.SetDomain( Domain(fnObj, 0, dmtUNIQUEID, asmp[0]->dm()->pdUniqueID()->sGetPrefix()));		
	else if ( sNewDom.length()) {
		if (odFinal == odID)
			dom = Domain(sNewDom, 0, dmtID);
		else if (odFinal == odCLASS)
			dom = Domain(sNewDom, 0, dmtCLASS);
	}
	else {
		if (odFinal == odID)
			dom = Domain(fnObj, 0, dmtID);
		else if (odFinal == odCLASS)
			dom = Domain(fnObj, 0, dmtCLASS);
	}
	
	if (odFinal == odBOOL)	
		 ndvr.SetDomain( Domain("bool"));
	else if (odFinal == odVALUE)	
		 ndvr.SetDomain( Domain("value")); 
  
  SetDomainValueRangeStruct(ndvr);
*/
	if (cb().fUndef())
	{
    CoordBounds cbUnion = asmp[0]->cb();
    for (int i = 1; i < iSegMap; i++)
		{
      CoordBounds cb = asmp[i]->cb();
      if (cs() != asmp[i]->cs())
        cb = cs()->cbConv(asmp[i]->cs(), cb);
      cbUnion += cb;
    }  
    SetCoordBounds(cbUnion);
  }
  else
		fUsesClipping=true;
	for (int i=0;  i < iSegMap; ++i)
    objdep.Add(asmp[i].ptr());
  if (!fnObj.fValid()) // 'inline' object
    objtime = objdep.tmNewest();
  //if (asmp[0]->fTblAttSelf())
  //  SetAttributeTable(asmp[0]->tblAtt());
}

void SegmentMapGlue::Store()
{
  SegmentMapVirtual::Store();
  WriteElement("SegmentMapVirtual", "Type", "SegmentMapGlue");
  WriteElement("SegmentMapGlue", "NrSegmentMap", (long)iSegMap);
  for (int i=0;  i < iSegMap; ++i) {
    WriteElement("SegmentMapGlue", String("SegmentMap%i", i).scVal(), asmp[i]);
    WriteElement("SegmentMapGlue", String("SegmentMapMask%i",i).scVal(), asMask[i]);
  }  
  if (sNewDom.length())
    WriteElement("SegmentMapGlue", "NewDomain", sNewDom);
  else
    WriteElement("SegmentMapGlue", "NewDomain", (char*)0);
  WriteElement("SegmentMapGlue", "UsesClipping", fUsesClipping);
}

SegmentMapGlue::~SegmentMapGlue()
{
}

String SegmentMapGlue::sExpression() const
{
  String s = "SegmentMapGlue(";
	if (fUsesClipping)
	{
		s &= String("%f, %f, %f, %f, ", cb().MinX(), cb().MinY(), cb().MaxX(), cb().MaxY());
	}
  for (int i=0;  i < iSegMap; ++i) {
    s &= asmp[i]->sNameQuoted(false, fnObj.sPath());
    s &= String(", \"%S\"", asMask[i]);
    if (i < iSegMap - 1)
      s &= ", ";
  }
  if (sNewDom.length())
    s &= String(", %S)", sNewDom);
  else
    s &= ')';
  return s;
}

bool SegmentMapGlue::fDomainChangeable() const
{
  return false;
}

void SegmentMapGlue::Init()
{
  htpFreeze = htpSegmentMapGlueT;
  sFreezeTitle = "SegmentMapGlue";
	fNeedFreeze = true;
}

bool SegmentMapGlue::fFreezing()
{
	CheckAndFindOutputDomain(iSegMap, asmp, sTypeName(), odFinal);
	DomainSort* pdsrt = asmp[0]->dm()->pdsrt();
	Domain dom;
	ValueRange vr;
	bool fSameDom = true;
	for (int i=1; i < iSegMap; ++i)
		if (asmp[0]->dvrs() != asmp[i]->dvrs()) {
			fSameDom = false;
			break;
		}
		if (!fSameDom) {
			if (0 != pdsrt) {
				trq.SetText(SSEGTextCreatingNewDomain);
				if (sNewDom.length()) {
					if (odFinal == odUNIQUEID) 
						dom = Domain(fnObj, 0, dmtUNIQUEID, asmp[0]->dm()->pdUniqueID()->sGetPrefix());
					else if (asmp[0]->dm()->pdid())
						dom = Domain(sNewDom, 0, dmtID);
					else
						dom = Domain(sNewDom, 0, dmtCLASS);
				}
				else {
					if (odFinal == odUNIQUEID) 
						dom = Domain(fnObj, 0, dmtUNIQUEID, asmp[0]->dm()->pdUniqueID()->sGetPrefix());
					else if (asmp[0]->dm()->pdid())
						dom = Domain(fnObj, 0, dmtID);
					else
						dom = Domain(fnObj, 0, dmtCLASS);
				}
				// merge codes of all 'sort' - domains
				DomainSort* pdsrt1 = dom->pdsrt();
				pdsrt1->Resize(0); // start empty !
				pdsrt1->dsType = DomainSort::dsMANUAL;
				for (int i=0; i < iSegMap; ++i) {
					DomainSort* pdsrt2 = asmp[i]->dm()->pdsrt();
					if (trq.fAborted())
						return false;
					pdsrt1->Merge(pdsrt2);
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
					for (int i=0; i < iSegMap; ++i) 
					{
						if (!asmp[i]->dvrs().fValues())
							continue;
						RangeReal rr = asmp[i]->rrMinMax(BaseMapPtr::mmmNOCALCULATE);
						if ( !rr.fValid())
							rr = asmp[i]->rrMinMax(BaseMapPtr::mmmCALCULATE);
						if (rr.rLo() < rMin)
							rMin = rr.rLo();
						if (rr.rHi() > rMax)
							rMax = rr.rHi();
						if (asmp[i]->dvrs().rStep() < rStep)
							rStep = asmp[i]->dvrs().rStep();
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
			if (rpr.fValid()) {
				rpr->Store();
				dom->SetRepresentation(rpr);
			}
			CreateSegmentMapStore();
			Store();
		}

		Array<CoordBuf> acrdBuf;
		long iCrdOut;           // nr of points on newly created, possibly clipped, segment
		int iLastUNIQUEID = 0; 
		if (dom.fValid())
			pdsrt = dom->pdsrt();
		bool fBoolInput = false;
		//double rValueStep = max(vr->rStep(), 0.0000001);
		for (int iMapNr = 0; iMapNr < iSegMap; ++iMapNr)
		{
			// build recode table if needed
			//    ArrayLarge<long> aiRecode(0,1); // 'lookup table' of new codes for input domain
			ArrayLarge<long> aiRecode; // zero based now // 'lookup table' of new codes for input domain
			if (0 != pdsrt) {
				DomainSort* pdsrt_i = asmp[iMapNr]->dm()->pdsrt();
				//      aiRecode.Resize(pdsrt1->iSize(),1); // in these arrays we find the mapping from old Raw values in
				aiRecode.Resize(pdsrt_i->iSize()); // zero based // in these arrays we find the mapping from old Raw values in
				for (unsigned long i = 1; i <= aiRecode.iSize(); i++) {
					if (odFinal == odUNIQUEID) 
						aiRecode[i-1] = iLastUNIQUEID + i;
					else
						aiRecode[i-1] = pdsrt->iRaw(pdsrt_i->sNameByRaw(i,0));
				}
				if (!fAllAttrTablesEqual)
					iLastUNIQUEID += pdsrt_i->iSize();
			}
			else if (0 != asmp[iMapNr]->dm()->pdbool()) 
				fBoolInput = true;
			else
				fBoolInput = false;
			// transform coord bounds if needed
			CoordSystem csOld = asmp[iMapNr]->cs();
			bool fTransformCoords = cs() != csOld;
			CoordBounds cbs = asmp[iMapNr]->cb();
			if (fTransformCoords)
				cbs = cs()->cbConv(csOld, cbs);
			if (cb().fContains(cbs))    // asmp[iMapNr] will be (partly) glued
			{
				SegmentMap smp = asmp[iMapNr];
				String sMask = asMask[iMapNr];
				trq.SetText(String(SSEGTextCopyWithMask_SS.scVal(), smp->sName(true, fnObj.sPath()), sMask));
				Mask mask(smp->dm(), sMask);
				long iSeg = smp->iFeatures();

				if (cb().fContains(smp->cb().cMin) && cb().fContains(smp->cb().cMax))
				{                            // no clipping needed, smp completely inside clipbox
					for (int i=0; i < smp->iFeatures(); ++i )  {
						ILWIS::Segment *seg = (ILWIS::Segment *)smp->getFeature(i);
						if ( seg == NULL || !seg->fValid())
							continue;
						if (trq.fUpdate(i, iSeg))
							return false;
						if (!seg->fInMask(pms->dvrs(),mask))
							continue;
						CoordinateSequence *seq = seg->getCoordinates();
						CoordBuf crdBuf;
						crdBuf.Size((long)seq->size());
						//seg.InternalCoors2WorldCoors(rcBuf, crdBuf, iCrdOut);
						for (int i=0; i < seq->size(); ++i ) 
							crdBuf[i] = fTransformCoords ? cs()->cConv(csOld, seq->getAt(i)) : seq->getAt(i);
						ILWIS::Segment *segNew = CSEGMENT(pms->newFeature());
						//segNew.WorldCoors2InternalCoors(crdBuf, rcBuf, iCrdOut);
						segNew->PutCoords(crdBuf.clone());
						long iRaw = seg->iValue();
						if (0 != pdsrt) {
							if ((iRaw > 0) && (iRaw <= (long)aiRecode.iSize()))
								iRaw = aiRecode[iRaw-1];
							else
								iRaw = iUNDEF;
							segNew->PutVal(iRaw);
						}
						else  {
							if (odFinal == odVALUE && !fBoolInput)
								segNew->PutVal(pms->dvrs(), seg->sValue(pms->dvrs()));
							else
								segNew->PutVal((iRaw == 1)? 0L : 1L);
						}
						delete seq;
					}  // end copying without clipping for seg.fValid
				}
				else
				{      // clipping at cb() limits needed
					for (int i=0; i < smp->iFeatures(); ++i )  {
						ILWIS::Segment *seg = CSEGMENT(smp->getFeature(i));
						if ( seg == NULL || !seg->fValid())
							continue;
						if (trq.fUpdate(i, iSeg))
							return false;
						if (!seg->fInMask(pms->dvrs(),mask))
							continue;
						seg->Clip(cb(),acrdBuf);
						for ( unsigned long j = 0; j < acrdBuf.iSize(); j++ ) // treat all buffers in array
						{
							iCrdOut = acrdBuf[j].iSize();
							ILWIS::Segment *segNew = CSEGMENT(pms->newFeature());
							if (fTransformCoords)
								for (int i=0; i < iCrdOut; ++i ) 
									acrdBuf[j][i] = cs()->cConv(csOld, acrdBuf[j][i]);
							//segNew.WorldCoors2InternalCoors(acrdBuf[j], rcBuf, iCrdOut);
							segNew->PutCoords(iCrdOut, acrdBuf[j]);
							long iRaw = seg->iValue();
							if (0 != pdsrt) {
								if ((iRaw > 0) && (iRaw <= (long)aiRecode.iSize()))
									iRaw = aiRecode[iRaw-1];
								else
									iRaw = iUNDEF;
								segNew->PutVal(iRaw);
							}
							else  {
								if (odFinal == odVALUE && !fBoolInput)
									segNew->PutVal(pms->dvrs(), seg->sValue(pms->dvrs()));
								else
									segNew->PutVal((iRaw == 1)? 0L : 1L);//raw values {1,2} mapped onto 'bools' {0,1}
							}
						} // end for j , all pieces of clipped segments
					}// end for seg.fValid
				}
			} // end if smp = asmp[i] completely or partly  inside cb()
			/// trq.fUpdate(iSeg, iSeg);
		}// end for i <  iSegMap
		bool fAllMapsHaveAttrTable = true;
		for (int i = 0; i < iSegMap; ++i)  // do for each input map
		{
			if ((!asmp[i].fValid()) || (!asmp[i]->fTblAtt())) {
				fAllMapsHaveAttrTable = false;
				break;
			}
		}
		if (fAllMapsHaveAttrTable)
			GlueAttributeTables();
		return true;
}

SegmentMapGlue::OutputDomain SegmentMapGlue::FindOutputDomain
																							(OutputDomain odOut , const Domain dm_i) 
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
  if ((odOut == odCLASS && dm_i->pdbool()) || (odOut == odBOOL && dm_i->pdc()))	
																												return odCLASS;
  if ((odOut == odVALUE && dm_i->pdbool()) || (odOut == odBOOL && dm_i->pdv()))	
																											  return odVALUE;
	return odINVALID;
}

void SegmentMapGlue::CheckAndFindOutputDomain(const long iSegMap, const Array<SegmentMap> asmp,
																						String sTypeN, OutputDomain& odOut)
{
	OutputDomain odResult = odNOTYETKNOWN;
	bool fSameDom = true;
  for (int i=1; i < iSegMap; ++i)
    if (asmp[0]->dvrs() != asmp[i]->dvrs()) {
      fSameDom = false;
      break;
    }
	if (fSameDom) 
		odResult = FindOutputDomain(odResult, asmp[0]->dm()); // just one single domain
  else {
		for (int i=0; i < iSegMap; ++i) {
			odResult = FindOutputDomain(odResult, asmp[i]->dm());
			if (odResult == odINVALID)
				IncompatibleDomainsError(asmp[0]->dm()->sName(),
													asmp[i]->dm()->sName(), sTypeN, errSegmentMapGlue+1);
		}
		for (int i=0; i < iSegMap; ++i) {
			odResult = FindOutputDomain(odResult, asmp[i]->dm());
			if (odResult == odINVALID)
				 IncompatibleDomainsError(asmp[0]->dm()->sName(),
													asmp[i]->dm()->sName(), sTypeN, errSegmentMapGlue+1);
		}
	}
	odOut = odResult;  // domains are combinable
}
/*
void SegmentMapGlue::MergeDomainSorts(const SegmentMap& smp)
{
	if (smp->dm()->pdsrt() && dm()->pdsrt())
		dm()->pdsrt()->Merge(smp->dm()->pdsrt(), &trq);		
}*/

void SegmentMapGlue::GlueAttributeTables()
{
  String sExpr("TableGlue(%S", dm()->fnObj.sFullNameQuoted(true));
  int iCount=0;
  for (unsigned int iMapNr = 0; iMapNr < asmp.iSize(); ++iMapNr)  // do for each input map
  {
    if (!asmp[iMapNr].fValid()) continue;
    if (asmp[iMapNr]->fTblAtt())
    {
      sExpr &= ",";
      sExpr &= asmp[iMapNr]->tblAtt()->fnObj.sFullNameQuoted(true);
      iCount++;
    }        
  }
	
  if (iCount>0) { // length original string
		if (fAllAttrTablesEqual)
			SetAttributeTable(asmp[0]->tblAtt());
		else
		{
			sExpr &= ")";
			FileName fn=FileName::fnUnique(FileName(fnObj,".tbt"));
			Table tbl(fn, sExpr);
			tbl->fErase = true;
			tbl->Calc();
			SetAttributeTable(tbl);
			tbl->fErase = false;
		}  
	}
}




