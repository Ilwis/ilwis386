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
#include "Engine\Base\DataObjects\valrange.h"
#include "Engine\Base\DataObjects\WPSMetaData.h"
#include "Applications\Raster\MAPSUB.H"
#include "Engine\SpatialReference\Grsub.h"
#include "Headers\Htp\Ilwisapp.htp"
#include "Headers\Err\Ilwisapp.err"
#include "Headers\Hs\map.hs"
#include "Headers\Hs\Georef.hs"

IlwisObjectPtr * createMapSubMap(const FileName& fn, IlwisObjectPtr& ptr, const String& sExpr, vector<void *> parms ) {
	if ( sExpr != "")
		return (IlwisObjectPtr *)MapSubMap::create(fn, (MapPtr &)ptr, sExpr);
	else
		return (IlwisObjectPtr *)new MapSubMap(fn, (MapPtr &)ptr);
}

const char* MapSubMap::sSyntax() {
  return "MapSubMap(map,1st line,1st col,nrlines,nrcols)\n"
  "MapSubMap(map,1st line,1st col)\n"
  "MapSubMapCorners(map,1st line,1st col,last line, last col)\n"
  "MapSubMapCoords(map,cornerX, cornerY, oppositecornerX, oppositecornerY)\n";
}

static const char* sSyntax2() {
  return "MapSubMap(map,1st line,1st col,nrlines,nrcols,grf)\n"
  "MapSubMap(map,1st line,1st col,grf)\n"
  "MapSubMapCorners(map,1st line,1st col,last line, last col,grf)\n"
  "MapSubMapCoords(map,cornerX, cornerY, oppositecornerX, oppositecornerY,grf)\n";
}

String wpsmetadataMapSubMap() {
	WPSMetaData metadata("MapSubMap");
	return metadata.toString();
}

ApplicationMetadata metadataMapSubMap(ApplicationQueryData *query) {
	ApplicationMetadata md;
	if ( query->queryType == "WPSMETADATA" || query->queryType == "") {
		md.wpsxml = wpsmetadataMapSubMap();
	}
	if ( query->queryType == "OUTPUTTYPE" || query->queryType == "")
		md.returnType = IlwisObject::iotRASMAP;
	if ( query->queryType == "EXPERSSION" || query->queryType == "")
		md.skeletonExpression =  String("%s %s", MapSubMap::sSyntax(),sSyntax2()).scVal();

	return md;
}

MapSubMap* MapSubMap::create(const FileName& fn, MapPtr& p, const String& sExpr)
{
  Array<String> as;
  String sFunc = IlwisObjectPtr::sParseFunc(sExpr);
  int iParms = IlwisObjectPtr::iParseParm(sExpr, as);
  if ((iParms < 3) || (iParms > 7)) 
    ExpressionError(sExpr, sSyntax());

  bool useCorners = as[1].toLower() == "corners" || fCIStrEqual(sFunc, "MapSubMapCorners");
  bool useCoords = as[1].toLower() == "coords" || fCIStrEqual(sFunc, "MapSubMapCoords");

  Map mp(as[0], fn.sPath());
  if (fCIStrEqual(sFunc, "MapSubMap")) {
    RowCol rcOffset, rcSize;
    rcOffset.Row = as[1].iVal()-1L;
    rcOffset.Col = as[2].iVal()-1L;
    if ((iParms == 3) || (iParms == 4)) {
      rcSize.Row = mp->rcSize().Row - rcOffset.Row;
      rcSize.Col = mp->rcSize().Col - rcOffset.Col;
    }
    else {
      rcSize.Row = as[3].iVal();
      rcSize.Col = as[4].iVal();
    }
	String sGrf = FileName(fn,".grf").sFullPath();
    if ((iParms == 4) || (iParms == 6))
      sGrf = as[iParms-1];
    if (sGrf.length() == 0)
      ExpressionError(sExpr, sSyntax2());
    return new MapSubMap(fn, p, mp, rcSize, rcOffset,  crdUNDEF, crdUNDEF, sGrf, false);
  }
  else if (useCorners) {
	 int offset = as[1] == "corners" ? 1 : 0;
    RowCol rcOffset, rcSize;
    rcOffset.Row = as[1 + offset].iVal()-1L;
    rcOffset.Col = as[2 + offset].iVal()-1L;
    rcSize.Row = as[3 + offset].iVal() - as[1].iVal() + 1;
    rcSize.Col = as[4 + offset].iVal() - as[2].iVal() + 1;
    String sGrf = fn.sFile;
    if (iParms == 6 + offset)
      sGrf = as[iParms-1  + offset];
    if (sGrf.length() == 0)
      ExpressionError(sExpr, sSyntax2());
    return new MapSubMap(fn, p, mp, rcSize, rcOffset, crdUNDEF, crdUNDEF, sGrf, true);
  }
  else if (useCoords) {
    Coord crd1, crd2;
	int offset = as[1] == "coords" ? 1 : 0;
    crd1.x = as[1  + offset].rVal();
    crd1.y = as[2 + offset].rVal();
    crd2.x = as[3 + offset].rVal();
    crd2.y = as[4 + offset].rVal();
    String sGrf = fn.sFile;
    if (iParms == 6 + offset)
      sGrf = as[iParms-1 + offset];
    if (sGrf.length() == 0)
      ExpressionError(sExpr, sSyntax2());
    return new MapSubMap(fn, p, mp, rcUNDEF, rcUNDEF, crd1, crd2, sGrf, true);
  }
  return 0;
}

MapSubMap::MapSubMap(const FileName& fn, MapPtr& p)
: MapFromMap(fn, p)
{
  ptr.ReadElement("MapSubMap", "Offset", rcOffset);
  if (0 == ptr.ReadElement("MapSubMap", "Corners", fCorners))
    fCorners = false;
  if (0 == ptr.ReadElement("MapSubMap", "Coord1", crd1))
    crd1 = crdUNDEF;
  if (0 == ptr.ReadElement("MapSubMap", "Coord2", crd2))
    crd2 = crdUNDEF;
  ReadGeoRefFromFileAndSet();
  Init();
}

MapSubMap::MapSubMap(const FileName& fn, MapPtr& p,
           const Map& mp, RowCol rcSiz, RowCol rcOffs, const Coord& _crd1, const Coord& _crd2, const String& sGrf, bool fCorn)
: MapFromMap(fn, p, mp), rcOffset(rcOffs), crd1(_crd1), crd2(_crd2), fCorners(fCorn)
{
  if (st() == stBIT) {
    if (rcOffset.Row+rcSiz.Row > mp->rcSize().Row)
      rcSiz.Row = mp->rcSize().Row - rcOffset.Row;
    if (rcOffset.Col+rcSiz.Col > mp->rcSize().Col)
      rcSiz.Col = mp->rcSize().Col - rcOffset.Col;
  }  
  if (gr()->fGeoRefNone()) {
    if (!crd1.fUndef())
      throw ErrorGeoRefNone(mp->fnObj, errMapSubMap+4);
    ptr.SetGeoRef(GeoRef(rcSiz));
    ptr.SetSize(rcSiz);
  }  
  else {
    GeoRef grNew;
    GeoRefSubMap* grfsub;
    if (!crd1.fUndef())
      grfsub = new GeoRefSubMapCoords(FileName(sGrf, ".grf", true), mp->gr(), crd1, crd2);
    else if (fCorners) 
      grfsub = new GeoRefSubMapCorners(FileName(sGrf, ".grf", true), mp->gr(), rcOffset, RowCol(rcSiz.Row+rcOffset.Row-1, rcSiz.Col+rcOffset.Col-1));
    else
      grfsub = new GeoRefSubMap(FileName(sGrf, ".grf", true), mp->gr(), rcOffset, rcSiz);
    grNew.SetPointer(grfsub);
    ptr.SetGeoRef(grNew);
    ptr.SetSize(grNew->rcSize());
    rcSiz = grNew->rcSize();
    rcOffset = grfsub->rcTopLeft();
		if (grNew->sGetDescription() == "")
		{
			String sD = grNew->sTypeName();
			sD &= String(SGRInfLinkedTo_S.scVal(), sTypeName());
			grNew->SetDescription(sD);
		}
  }  
  if ((rcOffset.Row + rcSiz.Row < 0) || (rcOffset.Row >= mp->rcSize().Row))
    throw ErrorObject(WhatError(String(SMAPErrInvalidStartRow_i.scVal(), rcOffset.Row+1), errMapSubMap), fnObj);
  if ((rcOffset.Col + rcSiz.Col < 0) || (rcOffset.Col >= mp->rcSize().Col))
    throw ErrorObject(WhatError(String(SMAPErrInvalidStartCol_i.scVal(), rcOffset.Col+1), errMapSubMap+1), fnObj);
  if (st() == stBIT) {
    if ((rcSiz.Row<=0) || (rcSiz.Row > mp->rcSize().Row))
      throw ErrorObject(WhatError(String(SMAPErrInvalidHeight_i.scVal(), rcSiz.Row), errMapSubMap+2), fnObj);
    if ((rcSiz.Col<=0) || (rcSiz.Col > mp->rcSize().Col))
      throw ErrorObject(WhatError(String(SMAPErrInvalidWidth_i.scVal(), rcSiz.Col), errMapSubMap+3), fnObj);
  }  

  Init();
	ptr.gr()->SetDescription(String(SMAPMsgGeoRefCreatedFrom_S.scVal(), sExpression()));
	ptr.gr()->Store();
  objdep.Add(gr().ptr());
  if (mp->fTblAtt()) 
    SetAttributeTable(mp->tblAtt());
  if (!fnObj.fValid())
    objtime = objdep.tmNewest();
  fNeedFreeze = false;
}

void MapSubMap::Store()
{
  MapFromMap::Store();
  WriteElement("MapFromMap", "Type", "MapSubMap");
  WriteElement("MapSubMap", "Offset", rcOffset);
  WriteElement("MapSubMap", "Corners", fCorners);
  if (!crd1.fUndef()) {
    WriteElement("MapSubMap", "Coord1", crd1);
    WriteElement("MapSubMap", "Coord2", crd2);
  }
}

MapSubMap::~MapSubMap()
{
}

void MapSubMap::Init()
{
  fNeedFreeze = false;
  sFreezeTitle = "MapSubMap";
  htpFreeze = "ilwisapp\\submap_of_raster_map_functionality_algorithm.htm";
}

long MapSubMap::iComputePixelRaw(RowCol rc) const
{
  return mp->iRaw(RowCol(rc.Row+rcOffset.Row, rc.Col+rcOffset.Col)); 
}

double MapSubMap::rComputePixelVal(RowCol rc) const
{
  return mp->rValue(RowCol(rc.Row+rcOffset.Row, rc.Col+rcOffset.Col)); 
}

void MapSubMap::ComputeLineRaw(long Line, ByteBuf& buf, long iFrom, long iNum) const
{
  if (iNum == 0)
    iNum = iCols();
  mp->GetLineRaw(Line+rcOffset.Row, buf, iFrom+rcOffset.Col, iNum); 
}

void MapSubMap::ComputeLineRaw(long Line, IntBuf& buf, long iFrom, long iNum) const
{
  if (iNum == 0)
    iNum = iCols();
  mp->GetLineRaw(Line+rcOffset.Row, buf, iFrom+rcOffset.Col, iNum); 
}

void MapSubMap::ComputeLineRaw(long Line, LongBuf& buf, long iFrom, long iNum) const
{
  if (iNum == 0)
    iNum = iCols();
  mp->GetLineRaw(Line+rcOffset.Row, buf, iFrom+rcOffset.Col, iNum); 
}

void MapSubMap::ComputeLineVal(long Line, LongBuf& buf, long iFrom, long iNum) const
{
  if (iNum == 0)
    iNum = iCols();
  mp->GetLineVal(Line+rcOffset.Row, buf, iFrom+rcOffset.Col, iNum); 
}

void MapSubMap::ComputeLineVal(long Line, RealBuf& buf, long iFrom, long iNum) const
{
  if (iNum == 0)
    iNum = iCols();
  mp->GetLineVal(Line+rcOffset.Row, buf, iFrom+rcOffset.Col, iNum); 
}

String MapSubMap::sExpression() const
{
  if (!crd1.fUndef())
    return String("MapSubMapCoords(%S, %lg,%lg,%lg,%lg)", mp->sNameQuoted(true, fnObj.sPath()), 
                  crd1.x, crd1.y, crd2.x, crd2.y);
  if (fCorners)
    return String("MapSubMapCorners(%S,%li,%li,%li,%li)", mp->sNameQuoted(true, fnObj.sPath()), 
                  rcOffset.Row+1, rcOffset.Col+1, rcOffset.Row+rcSize().Row, rcOffset.Col+rcSize().Col);
  return String("MapSubMap(%S,%li,%li,%li,%li)", mp->sNameQuoted(true, fnObj.sPath()), 
                rcOffset.Row+1, rcOffset.Col+1, rcSize().Row, rcSize().Col);
}
