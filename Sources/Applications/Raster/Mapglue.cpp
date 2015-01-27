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

#include "Applications\Raster\Mapglue.h"
#include "Engine\SpatialReference\Grsub.h"
#include "Engine\Base\DataObjects\valrange.h"
#include "Engine\Domain\dmsort.h"
#include "Engine\Domain\Dmvalue.h"
#include "Engine\Domain\DomainUniqueID.h"
#include "Engine\Base\DataObjects\WPSMetaData.h"
#include "Engine\Domain\dmpict.h"
#include "Headers\Htp\Ilwisapp.htp"
#include "Headers\Err\Ilwisapp.err"
#include "Engine\Map\Raster\Map.h"
#include "Headers\Hs\map.hs"
#include "Headers\constant.h"

IlwisObjectPtr * createMapGlue(const FileName& fn, IlwisObjectPtr& ptr, const String& sExpr, vector<void *> parms ) {
	if ( sExpr != "")
		return (IlwisObjectPtr *)MapGlue::create(fn, (MapPtr &)ptr, sExpr);
	else
		return (IlwisObjectPtr *)new MapGlue(fn, (MapPtr &)ptr);
}

String wpsmetadataMapGlue() {
	WPSMetaData metadata("MapGlue");

	return metadata.toString();
}

ApplicationMetadata metadataMapGlue(ApplicationQueryData *query) {
	ApplicationMetadata md;
	if ( query->queryType == "WPSMETADATA" || query->queryType == "") {
		md.wpsxml = wpsmetadataMapGlue();
	}
	if ( query->queryType == "OUTPUTTYPE" || query->queryType == "")
		md.returnType = IlwisObject::iotRASMAP;
	if ( query->queryType == "EXPERSSION" || query->queryType == "")
		md.skeletonExpression =  MapGlue::sSyntax();

	return md;
}


const char* MapGlue::sSyntax() {
  return "MapGlue(map1,map2, ...)\n"
"MapGlue(map1,map2,...,replace)\n"
"MapGlue(map1,map2,...,domname)\n"
"MapGlue(map1,map2,...,domname,replace)"
"MapGlue(georef,map1,map2,...)\n"
"MapGlue(georef,map1,map2,...,domname)\n";
}

static void GeoRefNoneError(const FileName& fn, IlwisError err)
{
  throw ErrorGeoRefNone(fn, err);
}

void ThrowIncompatibleGeorefError(const String& sMessage, const FileName& fn)
{
    throw ErrorObject(WhatError(sMessage, errMapGlue+1), fn);
}

void ThrowInvalidMapError(const String& sMessage, const FileName& fn)
{
    throw ErrorObject(WhatError(sMessage, errMapGlue+4), fn);
}

void ThrowInvalidDVRSError(const String& sMessage, const FileName& fn)
{
    throw ErrorObject(WhatError(sMessage, errMapGlue+5), fn);
}

MapGlue* MapGlue::create(const FileName& fn, MapPtr& p, const String& sExpr)
{
  Array<String> as;
  bool fReplaceAlways = false;
  String sNewDom;
  int iParms = IlwisObjectPtr::iParseParm(sExpr, as);
  if (iParms < 2)
    ExpressionError(sExpr, sSyntax());

  int iCurrParam = 0;
  bool fCreateGeoRef = true;
  GeoRef grf;
  FileName fnGrf(as[iCurrParam]);
  if (".grf" == fnGrf.sExt) 
  {
    fCreateGeoRef = false;
    grf = GeoRef(fnGrf);
    ++iCurrParam;
  }

  Array<Map> mapList;
  int iDomainParm = iParms - 1;
  mapList &= Map(as[iCurrParam++]);
  mapList &= Map(as[iCurrParam++]);
  if (iParms > iCurrParam)
  {
    fReplaceAlways = fCIStrEqual(as[iParms-1] , "replace");
    if (fReplaceAlways) iDomainParm -=1;
    FileName fnMaybeRaster(as[iDomainParm], String(".mpr"));
    if (!fnMaybeRaster.fExist()) // does not exists so the file is a domain
      sNewDom=as[iDomainParm--];
    for (int i=iCurrParam; i <= iDomainParm; ++i)
      mapList &= Map(as[i]);
  }
  if (fCreateGeoRef)
    return new MapGlue(fn, p, mapList, fReplaceAlways, sNewDom);
  else
    return new MapGlue(fn, p, grf, mapList, fReplaceAlways, sNewDom);
}

MapGlue::MapGlue(const FileName& fn, MapPtr& p)
: MapFromMap(fn, p)
, fReplaceAlways(false)
, fAllUniqueID(false)
{
  maps &= mp;
  Map map2;
  ReadGeoRefFromFileAndSet();
  fCreateGeoRef = true;
  ReadElement("MapGlue", "Create GeoRef", fCreateGeoRef);
  int iRet = ReadElement("MapGlue", "Map2", map2);
  if (iRet > 0)
    maps &= map2;
  else
  {
    long iNMaps = 0;
    ReadElement("MapGlue", "NumberOfMaps", iNMaps);
    for (int i=1; i < iNMaps; ++i)
    {
      String sEntry("RasterMap%d", i);
      ReadElement("MapGlue", sEntry.c_str(), map2);
      maps &= map2;
      objdep.Add(map2.ptr());
    }
  }      
  ReadElement("MapGlue", "ReplaceAlways", fReplaceAlways);
  ReadElement("MapGlue", "NewDomain", sNewDom);
	LegalMapCollection();
	DomainValueRangeStruct ndvr;
	fAllUniqueID = true;
	for (unsigned int i=0; i < maps.size(); ++i)
		DetermineOutputDomain(maps[i], ndvr, fAllUniqueID, false);
	if (fAllUniqueID )
		ndvr.SetDomain( Domain(fnObj, 0, dmtUNIQUEID, maps[0]->dm()->pdUniqueID()->sGetPrefix()));		
		
	SetDomainValueRangeStruct(ndvr);  

  fNeedFreeze = true;
  sFreezeTitle = "MapGlue";
  htpFreeze = "ilwisapp\\glue_raster_maps_algorithm.htm";
}

MapGlue::MapGlue(const FileName& fn, MapPtr& p, const Array<Map>& _maps, bool fRepAlw, const String& sNewDomain)
: MapFromMap(fn, p, _maps[0])
, fReplaceAlways(fRepAlw), sNewDom(sNewDomain), fAllUniqueID(false)
, fCreateGeoRef(true)
{
  for(unsigned int i=0; i<_maps.iSize(); ++i)
	{
    maps&=_maps[i];
	}		

  LegalMapCollection();
  
  fNeedFreeze = true;

  DomainValueRangeStruct ndvr;
	fAllUniqueID = true;
	for (unsigned int i=0; i < maps.size(); ++i)
    DetermineOutputDomain(maps[i], ndvr, fAllUniqueID, false);

	if (fAllUniqueID)
		ndvr.SetDomain( Domain(fnObj, 0, dmtUNIQUEID, maps[0]->dm()->pdUniqueID()->sGetPrefix()));		
		
  SetDomainValueRangeStruct(ndvr);

  sFreezeTitle = "MapGlue";
  htpFreeze = "ilwisapp\\glue_raster_maps_algorithm.htm";
  for ( unsigned int i=1; i<maps.iSize(); ++i)
    objdep.Add(maps[i].ptr());
}

MapGlue::MapGlue(const FileName& fn, MapPtr& p, const GeoRef& gr, const Array<Map>& _maps, bool fRepAlw, const String& sNewDomain)
: MapFromMap(fn, p, _maps[0])
, fReplaceAlways(fRepAlw), sNewDom(sNewDomain), fAllUniqueID(false)
, fCreateGeoRef(false)
{
  ptr.SetSize(gr->rcSize());
  ptr.SetGeoRef(gr);
  objdep.Add(gr);

  for(unsigned int i=0; i<_maps.iSize(); ++i)
	{
    maps &= _maps[i];
	}		

  LegalMapCollection();
  
  fNeedFreeze = true;

  DomainValueRangeStruct ndvr;
	fAllUniqueID = true;
	for (unsigned int i=0; i < maps.size(); ++i)
    DetermineOutputDomain(maps[i], ndvr, fAllUniqueID, false);

	if (fAllUniqueID)
		ndvr.SetDomain( Domain(fnObj, 0, dmtUNIQUEID, maps[0]->dm()->pdUniqueID()->sGetPrefix()));		
		
  SetDomainValueRangeStruct(ndvr);

  sFreezeTitle = "MapGlue";
  htpFreeze = "ilwisapp\\glue_raster_maps_algorithm.htm";
  for (unsigned int i=1; i < maps.iSize(); ++i)
    objdep.Add(maps[i].ptr());
}

void MapGlue::Store()
{
  long iNMaps=maps.iSize(); // first map is already stored
  MapFromMap::Store();
  WriteElement("MapFromMap", "Type", "MapGlue");
  WriteElement("MapGlue", "Create GeoRef", fCreateGeoRef);
  WriteElement("MapGlue", "NumberOfMaps", iNMaps);
  for(int i=1; i<iNMaps; ++i)
  {
    String sEntry("RasterMap%d", i);
    Map map_j=maps[i];
    WriteElement("MapGlue", sEntry.c_str(), map_j);
  }
  WriteElement("MapGlue", "ReplaceAlways", fReplaceAlways);
  if (sNewDom.length())
    WriteElement("MapGlue", "NewDomain", sNewDom);
  else
    WriteElement("MapGlue", "NewDomain", (char*)0);
}

MapGlue::~MapGlue()
{
}

MapGlue::DomainCombinations MapGlue::dcDomainCombination(const Domain& dm1, const Domain& dm2)
{
  if (dm1->pdi() && dm2->pdi()) return dcIMAGEIMAGE;
  if (dm1==dm2) return dcEQUAL;
  if (dm1->pdsrt() && dm2->pdsrt()) return dcSORTSORT;
  if (dm1->pdv() && dm2->pdv()) return dcVALUEVALUE;
  if (dm1->pdp() && dm2->pdp()) return dcPICTUREPICTURE;
  if ((dm1->pdcol() && dm2->pdi()) || (dm1->pdi() && dm2->pdcol())) return dcCOLORIMAGE;
  if ((dm1->pdcol() && dm2->pdp()) || (dm1->pdp() && dm2->pdcol())) return dcCOLORPICTURE;
  if ((dm1->pdc() && dm2->pdv()) || (dm1->pdv() && dm2->pdc())) return dcCLASSVALUE; 
  if ((dm1->pdc() && dm2->pdcol()) || (dm1->pdcol() && dm2->pdc())) return dcCOLORCLASS; 
  if ((dm1->pdcol() && dm2->pdv()) || (dm2->pdcol() && dm1->pdv())) return dcCOLORVALUE; 

  return dcILLEGAL;
}        

void MapGlue::LegalMapCollection()
{
  int iNMaps = maps.iSize();
  for (int i=0; i < iNMaps; ++i) // no georef nones present
  {
    if (!maps[i].fValid())    
      ThrowInvalidMapError(TR("Invalid map"), mp->fnObj);
    if (maps[i]->gr()->fGeoRefNone())
      GeoRefNoneError(mp->fnObj, errMapGlue);
  }          
  for (int i=0; i<iNMaps; ++i) // coordinate systems compatible?
  {
    const Map& map_i=maps[i];
    for(int j=i+1; j < iNMaps; ++j)
    {
      const Map& map_j=maps[j];
      if (!map_i->gr()->cs()->fConvertFrom(map_j->gr()->cs()))
        IncompatibleCoordSystemsError(map_i->gr()->cs()->sName(true, fnObj.sPath()), 
          map_j->gr()->cs()->sName(true, fnObj.sPath()), sTypeName(), errMapGlue+2);
      if (dcDomainCombination(map_i->dm(), map_j->dm())==dcILLEGAL)
        IncompatibleDomainsError(map_i->dm()->sName(true, fnObj.sPath()),
          map_j->dm()->sName(true, fnObj.sPath()), sTypeName(), errMapGlue+3);                                  
    }            
  }
}

void MapGlue::MakeNewRowColLimitsAndCrdBounds(const GeoRef& gr1, const GeoRef& gr2,
                                  MinMax& mmMapLimits, CoordBounds& cb)
{
  cb = CoordBounds();
  bool fTransformCoords = (gr1->cs() != gr2->cs());
  MinMax mmLimits;
  // doorloop de hele rand van grf2 RowCols
  // zet ze stuk voor stuk om naar grf1 Rowcols
  // via gr2->RowCol2Coord en gr1->Coord2RowCol
  // pas bij elke stap de mmLimits aan
  Coord crd; // Coord of point in 2nd and 1st CSY respectively
  long iRow, iCol;  // integer RowCol counters
  iCol = 0;
  for (iRow = 0; iRow < gr2->rcSize().Row; iRow++)
  {
    RowCol rc(iRow, iCol);
    crd = gr2->cConv(rc);
    if (fTransformCoords)
      crd = gr1->cs()->cConv(gr2->cs(), crd);
    rc = gr1->rcConv(crd);
    mmLimits += rc;
    cb += crd;
  }
  iRow = gr2->rcSize().Row - 1;
  for (iCol = 0; iCol < gr2->rcSize().Col; iCol++) {
    RowCol rc(iRow, iCol);
    crd = gr2->cConv(rc);
    if (fTransformCoords)
      crd = gr1->cs()->cConv(gr2->cs(), crd);
    rc = gr1->rcConv(crd);
    mmLimits += rc;
    cb += crd;
  }
  iCol = gr2->rcSize().Col - 1;
  for (iRow = gr2->rcSize().Row - 1; iRow >= 0; iRow--) {
    RowCol rc(iRow, iCol);
    crd = gr2->cConv(rc);
    if (fTransformCoords)
      crd = gr1->cs()->cConv(gr2->cs(), crd);
    rc = gr1->rcConv(crd);
    mmLimits += rc;
    cb += crd;
  }
  iRow = 0;
  for (iCol = gr2->rcSize().Col - 1; iCol >= 0; iCol--) {
    RowCol rc(iRow, iCol);
    crd = gr2->cConv(rc);
    if (fTransformCoords)
      crd = gr1->cs()->cConv(gr2->cs(), crd);
    rc = gr1->rcConv(crd);
    mmLimits += rc;
    cb += crd;
  }
  mmMapLimits += mmLimits;
}

bool MapGlue::fFreezing()
{
	int i;
	GeoRef grf0 = maps[0]->gr();
	if (!fCreateGeoRef)
		grf0 = gr();  // use the user supplied georef for checking
	
	MinMax mmMapLimits;
	mmMapLimits.rcMin = RowCol(0L, 0L);
	mmMapLimits.rcMax = grf0->rcSize();
	mmMapLimits.MaxRow() -= 1;
	mmMapLimits.MaxCol() -= 1;
	
	int iNMaps=maps.iSize();
	int iFirstMap = fCreateGeoRef ? 0 : 1;
	CoordBounds cbNew = cb();
	for (i = iFirstMap; i < iNMaps; ++i)
		MakeNewRowColLimitsAndCrdBounds(grf0, maps[i]->gr(), mmMapLimits, cbNew);
	
	//DomainValueRangeStruct ndvr;	
	DomainCombinations dcomb;
	if (!fAllUniqueID)
		for (i = 0; i < iNMaps; ++i) {
			dcomb = dcDomainCombination(dm(), maps[i]->dm());
			if (dcomb == dcSORTSORT) 
				MergeDomainSorts(maps[i]);
		}  
		//SetDomainValueRangeStruct(ndvr);	
	
	bool fResampleNeeded = false;
	for (i = iFirstMap; i < iNMaps; ++i)
		fResampleNeeded |= ( grf0 != maps[i]->gr() );
	
	if (fCreateGeoRef && fResampleNeeded) 
	{
		ptr.SetCoordBounds(cbNew);
		GeoRef grNew;
		FileName fnGrf = FileName(fnObj, ".grf");
		RowCol rcMin = mmMapLimits.rcMin;
		RowCol rcSize = mmMapLimits.rcSize();
		rcSize.Row += 1;
		rcSize.Col += 1;
		grNew.SetPointer(new GeoRefSubMap(fnGrf, grf0, rcMin, rcSize));
		grNew->Store();
		ptr.SetGeoRef(grNew);
		ptr.SetSize(grNew->rcSize());
	}
	
	// First set domain none, to force removal of current domain (possibly with the wrong storetype)
	Domain dom = dm();
	DomainValueRangeStruct dvrs("none");
	SetDomainValueRangeStruct(dvrs);
	// Then set the domain again, but now with the correct storetype
	dvrs = DomainValueRangeStruct(dom);
	SetDomainValueRangeStruct(dvrs);

	CreateMapStore();
	Store();
	ptr.FillWithUndef();
	int iEndLastID = 0;
	for(i = 0; i < iNMaps; ++i)
	{
		bool fResampleOfMap_iNeeded = ptr.gr() != maps[i]->gr();
		if (!fGlueRasterMaps(maps[i], fResampleOfMap_iNeeded, mmMapLimits, iEndLastID))
			return false;
	}
	bool fAllMapsHaveAttrTable = true;
	for (int iMapNr=0; iMapNr < iNMaps; ++iMapNr)  // do for each input map
	{
		if ((!maps[iMapNr].fValid()) || (!maps[iMapNr]->fTblAtt())) {
			fAllMapsHaveAttrTable = false;
			break;
		}
	}
	if (fAllMapsHaveAttrTable)
		GlueAttributeTables();
	AfxGetApp()->PostThreadMessage(ILW_READCATALOG, 0, 0);
	
	return true;
}

bool MapGlue::fGlueRasterMaps(const Map& map_i, bool fResampleNeeded, MinMax& mmMapLimits, int& iEndLastID)
{
  Map mpSubMap, mpResample;
  if (fResampleNeeded) 
  {
    mpResample = Map(FileName::fnUnique(FileName(map_i->fnObj,".mpr",true)),
                 String("MapResample(%S,%S,NearestNeighbour)", map_i->sNameQuoted(), gr()->sNameQuoted()));
    mpResample->fErase = true;
    mpResample->Store();
    mpResample->Calc();
    if (!mpResample->fCalculated())
      return false;
  }
  else 
  {
    mpResample = map_i;
  }
  bool fSameDom = map_i->dvrs() == dvrs();
  DomainCombinations dcomb = dcDomainCombination(dm(), map_i->dm());
  
  trq.SetTitle(sFreezeTitle);
  trq.SetText(TR("Calculating"));
    // merge maps: first one takes precedence over second
  if (fValues()) 
  {
    if (dcomb == dcIMAGEIMAGE) 
    {
      return fMergeByteValues(mpResample);
    }
    else if ((0 != dm()->pdi()) || (0 != dm()->pdbit())) 
    {
      return fMergeBits(mpResample);
    }
    else 
      return fMergeValues(mpResample);
  }
  else if (fSameDom)               // picture, colour domains (and others)
    return fMergeEqualDomains(mpResample, map_i->dm()->pdp()!=0);
  else 
    switch (dcomb) 
    {
      case dcSORTSORT:
      case dcPICTUREPICTURE:
        return fMergeSortSortOrPicturePicture(mpResample, iEndLastID);
      case dcCOLORIMAGE:
        return fMergeColorImage(mpResample);
      case dcCOLORPICTURE:
        return fMergeColorPicture(mpResample);
      case dcCOLORVALUE:
        return fMergeValueColor(mpResample);
      case dcCOLORCLASS:
        return fMergeClassColor(mpResample);         
    }
  return false;
}

bool MapGlue::fMergeBits(Map& mpResample)
{
    RealBuf rBuf1(iCols()), rBuf2(iCols());
    ByteBuf bBuf1(iCols());
    for (long i=0; i <  iLines(); ++i) 
    {
        if (trq.fUpdate(i, iLines()))
          return false;
        pms->GetLineRaw(i, bBuf1);
        mpResample->GetLineVal(i, rBuf2);
        for (long j=0; j < iCols(); j++)
          if ((fReplaceAlways && rBuf2[j] != rUNDEF) || bBuf1[j] == 0) // replace with second one
            rBuf1[j] = rBuf2[j];
          else
            rBuf1[j] = bBuf1[j];
        pms->PutLineVal(i, rBuf1);
    }
    return true;
}

bool MapGlue::fMergeValues(Map& mpResample)
{
    RealBuf rBuf1(iCols()), rBuf2(iCols());
    double rIgnore1=mpResample->dm()->pdi() ? 0 : rUNDEF;
    double rIgnore2=dm()->pdi() ? 0 : rUNDEF;
    for (long i=0; i <  iLines(); ++i) 
    {
        if (trq.fUpdate(i, iLines()))
          return false;
        pms->GetLineVal(i, rBuf1);
        mpResample->GetLineVal(i, rBuf2);
        for (long j=0; j < iCols(); j++)
          if ((fReplaceAlways && rBuf2[j] != rIgnore1) || rBuf1[j] == rIgnore2) // replace with second one
            rBuf1[j] = rBuf2[j];
        pms->PutLineVal(i, rBuf1);
    }
    return true;
}    

bool MapGlue::fMergeByteValues(Map& mpResample)
{
    ByteBuf bBuf1(iCols()), bBuf2(iCols());
    for (long i=0; i <  iLines(); ++i) 
    {
        if (trq.fUpdate(i, iLines()))
            return false;
        pms->GetLineRaw(i, bBuf1);
        mpResample->GetLineRaw(i, bBuf2);
        for (long j=0; j < iCols(); j++)
          if ((fReplaceAlways && bBuf2[j] != 0) || bBuf1[j] == 0) // replace with second one
            bBuf1[j] = bBuf2[j];
        pms->PutLineRaw(i, bBuf1);
     }
     return true;
}

bool MapGlue::fMergeColorPicture(Map& mpResample)
{
    LongBuf colBuf(iCols());
    LongBuf iBuf(iCols());
    LongBuf colBufOut(iCols());
    Representation rpr=mpResample->dm()->rpr();
    for (long i=0; i <  iLines(); ++i) 
    {
        if (trq.fUpdate(i, iLines()))
          return false;
        pms->GetLineRaw(i, colBuf);
        mpResample->GetLineRaw(i, iBuf);
        for (long j=0; j < iCols(); j++)  
        {
            long iColVal=iBuf[j];
            colBufOut[j]=((fReplaceAlways && iColVal != iUNDEF) || colBuf[j] != iUNDEF) ? colBuf[j] :
                                                                                      (long)rpr->clrRaw(iColVal);
        }       
        pms->PutLineRaw(i, colBufOut);
    }
    return true;
}    

bool MapGlue::fMergeColorImage(Map& mpResample)
{
    LongBuf colBuf(iCols());
    ByteBuf bBuf(iCols());
    LongBuf colBufOut(iCols());
    for (long i=0; i <  iLines(); ++i) 
    {
        if (trq.fUpdate(i, iLines()))
            return false;
        pms->GetLineRaw(i, colBuf);
        mpResample->GetLineRaw(i, bBuf);
     
        for (long j=0; j < iCols(); j++)  
        {
            byte bColVal=bBuf[j];
            colBufOut[j]=((fReplaceAlways && bColVal != 0) || colBuf[j] == 0) ? (long)Color(bColVal,bColVal,bColVal) :
                                                                                 colBuf[j];
        }
        pms->PutLineRaw(i, colBufOut);
    }
    return true;
}    

// iEndLastID is used for domainUniqueID. As UniqueID is only used when all maps use uniqueID
// the recode tables can simply use an offset to determine the raw in the new map. This offset is
// the iEndLastID.
bool MapGlue::fMergeSortSortOrPicturePicture(Map& mpResample, int  & iEndLastID ) // and picture, 
{
    LongBuf iBuf1(iCols()), iBuf2(iCols());
    trq.SetText(TR("Calculating"));
    ArrayLarge<long> aiRecode;
    MakeRecodeTable(mpResample, aiRecode, iEndLastID);
    for (long i=0; i <  iLines(); ++i) 
    {
      if (trq.fUpdate(i, iLines()))
        return false;
      pms->GetLineRaw(i, iBuf1);
      mpResample->GetLineRaw(i, iBuf2);
      long j;
         
      for( j = 0; j < iCols(); j++ ) 
        iBuf2[j]=(iBuf2[j] > 0 &&  iBuf2[j] <= aiRecode.iSize()) ? aiRecode[iBuf2[j]-1] : iUNDEF;
        
      for ( j = 0; j < iCols(); j++ )
        if ((fReplaceAlways && iBuf2[j] != iUNDEF) || iBuf1[j] == iUNDEF) // replace with second one
          iBuf1[j] = iBuf2[j];
      pms->PutLineRaw(i, iBuf1);
    }
    return true;
}

bool MapGlue::fMergeEqualDomains(Map& mpResample, bool fFirstIsPicture)
{
    LongBuf iBuf1(iCols()), iBuf2(iCols());
    for (long i=0; i <iLines(); ++i) 
    {
      if (trq.fUpdate(i, iLines()))
        return false;
      pms->GetLineRaw(i, iBuf1);
      mpResample->GetLineRaw(i, iBuf2);
      for (long j=0; j < iCols(); j++)
      {
        if ( fFirstIsPicture )
        {
            if ((fReplaceAlways && iBuf2[j] !=0) || iBuf1[j] == 0 ) // replace with second one
                iBuf1[j] = iBuf2[j];
        }
        else if ((fReplaceAlways && iBuf2[j] != iUNDEF) || iBuf1[j] == iUNDEF) // replace with second one
                 iBuf1[j] = iBuf2[j];
      }
      pms->PutLineRaw(i, iBuf1);
    }      
    return true;
}

bool MapGlue::fMergeClassColor(Map& mpResample)
{
    LongBuf colBuf(iCols()), iBuf1(iCols()), iBuf2(iCols());
    Representation rpr2=mpResample->dm()->rpr();
    RangeReal rrMinMax;
       
    for (long i=0; i <  iLines(); ++i) 
    {
        if (trq.fUpdate(i, iLines()))
            return false;
        mpResample->GetLineRaw(i, iBuf2);
        pms->GetLineRaw(i, iBuf1);
        long iCols=this->iCols(); // huh ??
        for ( long j=0; j< iCols; j++) 
        {
            long iRaw=iBuf2[j];
            Color valColor=rpr2->clrRaw(iRaw);
            colBuf[j]=((fReplaceAlways && iRaw != iUNDEF) || iBuf1[j] == iUNDEF) ? valColor : (Color)iBuf1[j];  
        }
        pms->PutLineRaw(i, colBuf);
    }
    return true;
}

bool MapGlue::fMergeValueColor(Map& mpResample)
{
    LongBuf colBuf(iCols()), iBuf1(iCols());
    RealBuf rBuf2(iCols());
    Representation rpr2=mpResample->dm()->rpr();
    RangeReal rrMinMax;
    rrMinMax=rrDetermineValueRange(mpResample);
        
    for (long i=0; i <  iLines(); ++i) 
    {
        if (trq.fUpdate(i, iLines()))
            return false;
        mpResample->GetLineVal(i, rBuf2);
        pms->GetLineRaw(i, iBuf1);
        long iCols=this->iCols(); // huh ??
        for ( long j=0; j< iCols; j++) 
        {
            double rVal=rBuf2[j];
            Color valColor;
            if (!rpr2.fValid())
            {
                long ValCol=(long)(255*((rrMinMax.rHi() - rVal)/rrMinMax.rWidth()));
                valColor=Color((byte)ValCol, (byte)ValCol, (byte)ValCol);
            }
            else
                valColor=!rpr2->prg() ? rpr2->clr(rVal, rrMinMax) : rpr2->clr(rVal);

            colBuf[j]=((fReplaceAlways && rVal != rUNDEF) || iBuf1[j] == iUNDEF) ? valColor : (Color)iBuf1[j];  
        }
        pms->PutLineRaw(i, colBuf);
    }
    return true;
}

RangeReal MapGlue::rrDetermineValueRange(Map& ValMap)
{
    RangeReal rrMinMax=ValMap->rrPerc1(true);
    if ( !rrMinMax.fValid())
    {
        return ValMap->dm()->pdv()->rrMinMax();
    }
    return rrMinMax;
}

void MapGlue::DetermineOutputDomain(const Map& mp, DomainValueRangeStruct& ndvr, bool& fAllUniqueID, bool fDoNotCalc)
{   
		bool fInitial=!ndvr.dm().fValid();
		ValueRange vr;
		Domain dom = ndvr.dm();
		if ((0 !=mp->dm()->pdc() && 0!=ndvr.dm()->pdv()) ||
			 (0 !=ndvr.dm()->pdc() && 0!=mp->dm()->pdv()))
		{
				dom=Domain("color"); 
				fAllUniqueID = false;				
		}				
		else if ((mp->dm()->pdsrt() && ndvr.dm()->pdsrt()) || 
				(fInitial && mp->dm()->pdsrt()))
		{
			// do not create a domain for unique id during initial phase. As unique id will only be used if
			// all maps use unique id it can not be determined on a map by map basis. In the final calculation
			// phase the domain must be filled. As the map already has the correct domaintype, unique id may also
			// use the code below
			if (mp->dm()->pdUniqueID() == 0 || fDoNotCalc) 
			{
				if (!fDoNotCalc)
					fAllUniqueID = false;

				// create (new) domain if the first map comes along
				//if (fInitial) 
				bool fId = true; // 2 dmSorts produce dmId except dmUId*dmUId and dmClass*dmClass
				{
					// no domain needed for uniqueid. can only be created after all maps have been checked
					if (!fAllUniqueID)									
					{
						bool fClassAndClass = (mp->dm()->pdc()!=0 && ndvr.dm()->pdc()!=0 );
						if (fClassAndClass) 
							fId = false; // Class and Unique Id yields now Id domain, january 2001
						if ( sNewDom.length())
								dom = Domain(sNewDom, 0, fId ? dmtID : dmtCLASS);
						else
								dom = Domain(fnObj, 0, fId ? dmtID : dmtCLASS);
					}
					else
						dom = dm();
				}
			}
		}
		else if ((mp->dm()->pdp() && ndvr.dm()->pdp()) ||
							(fInitial && mp->dm()->pdp()))
		{
			if (fInitial)
				dom = Domain(fnObj, 1, dmtPICTURE);
			DomainPicture* pdpic = dom->pdp(); // new domain
//				if (!fInitial) pdpic->Merge(ndvr.dm()->pdp());
			pdpic->Merge(mp->dm()->pdp());
			fAllUniqueID = false;			
		}
		else if ( (0 != mp->dm()->pdcol()) || (0 != ndvr.dm()->pdcol()) )  
		{
				dom = Domain("color");
				fAllUniqueID = false;				
		}
		else
		{
				dom = Domain("value");
				RangeReal mmMap=mp->rrMinMax();
				RangeReal mmGlue=ndvr.rrMinMax();
				double rMin = mmGlue.fValid() ? min (mmMap.rLo(), mmGlue.rLo()) : mmMap.rLo();
				double rMax = mmGlue.fValid() ? max (mmMap.rHi(), mmGlue.rHi()) : mmMap.rHi();
				double rStep= ndvr.fValues() ? min (mp->dvrs().rStep(), ndvr.rStep()) : mp->dvrs().rStep();
				if ( rStep==1.0 && rMin>=0 && rMax<=255.0 && mp->dm()->pdi())
						dom=Domain("image");
				vr = ValueRange(rMin, rMax, rStep);
				fAllUniqueID = false;
		}
		
		ndvr=DomainValueRangeStruct(dom, vr);
} 	 


void MapGlue::MakeRecodeTable(Map& map_i, ArrayLarge<long>& aiRecode, int  & iEndLastID)
{
    Domain dom;
    ValueRange vr;
    DomainSort* pdsrt1 = dm()->pdsrt();
    DomainSort* pdsrt2 = map_i->dm()->pdsrt();
    DomainPicture* pdpic1 = dm()->pdp();
    DomainPicture* pdpic2 = map_i->dm()->pdp();
    if (pdsrt1!=0 && pdsrt2!=0)         // if map has domain sort (id or class)
    {
//      aiRecode.Resize(pdsrt2->iSize(),1); // in these arrays we find the mapping from old Raw values in
      aiRecode.Resize(pdsrt2->iSize()); // now zero based // in these arrays we find the mapping from old Raw values in
      
      for ( unsigned int i = 1; i <= aiRecode.iSize(); i++)
			{
				if (!fAllUniqueID)
					aiRecode[i-1] = pdsrt1->iRaw(pdsrt2->sNameByRaw(i,0));
				else
					aiRecode[i-1] = iEndLastID + i;
			}		
			iEndLastID += pdsrt2->iSize();
    }
    else if (0 != pdpic1 && 0 != pdpic2) // both are pictures to be merged
    {
      Representation rpr = map_i->dm()->rpr();
//      aiRecode.Resize(pdpic2->iColors()-1,1); // pdpic1 and pdpic2 resp. to the new values of common domain pdsrt
      aiRecode.Resize(pdpic2->iColors()-1); // now zero based // pdpic1 and pdpic2 resp. to the new values of common domain pdsrt

      for ( unsigned int i = 1; i <= aiRecode.iSize(); i++)
        aiRecode[i-1] = pdpic1->iRaw(rpr->clrRaw(i));
    }
}

void MapGlue::GlueAttributeTables()
{
  if ( !dm()->pdsrt()) return;
    
	String sExpr("TableGlue(%S", dm()->fnObj.sFullNameQuoted(true));
	int iCount=0;
	for (unsigned int iMapNr=0; iMapNr < maps.iSize(); ++iMapNr)  // do for each input map
	{
			if (!maps[iMapNr].fValid()) continue;
			if (maps[iMapNr]->fTblAtt())
			{
					sExpr&=",";
					sExpr&=maps[iMapNr]->tblAtt()->fnObj.sFullNameQuoted(true);
					iCount++;
			}        
	}
	if (iCount>0) // length original string
	{
			sExpr&=")";
			FileName fn=FileName::fnUnique(FileName(fnObj,".tbt"));
			Table tbl(fn, sExpr);
			tbl->fErase=true;
			tbl->Calc();
			SetAttributeTable(tbl);
			tbl->fErase=false;
	}    
}

void MapGlue::MergeDomainSorts(const Map& mp)
{
	if (mp->dm()->pdsrt() && dm()->pdsrt())
		dm()->pdsrt()->Merge(mp->dm()->pdsrt(), &trq);		
}

String MapGlue::sExpression() const
{
  String s = "MapGlue(";
  if (!fCreateGeoRef)
    s &= String("%S,", gr()->sNameQuoted(true, fnObj.sPath()));
  for (unsigned int i=0;  i < maps.iSize(); ++i) 
  {
    if (0 !=  i) 
      s &= ",";
    s &= maps[i]->sNameQuoted(false, fnObj.sPath());
  }
  if (sNewDom.length())
    s &= String(",%S", sNewDom);
  if (fReplaceAlways)
    s &= ",replace)";
  else
    s &= ')';
  return s;
  
}

bool MapGlue::fDomainChangeable() const
{
  return false;
}

bool MapGlue::fValueRangeChangeable() const
{
  return false;
}

