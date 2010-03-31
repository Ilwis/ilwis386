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
// $Log: /ILWIS 3.0/RasterApplication/MAPSUB.cpp $
 * 
 * 9     27/03/00 9:50 Willem
 * The description of the dependent georef is now set in the map
 * constructor instead of in the Store()
 * 
 * 8     24/03/00 17:36 Willem
 * Dependent GeoRef's: The description is now set to:
 * "Created from " + the create expression
 * 
 * 7     24/01/00 17:36 Willem
 * MapSubMap now initializes a default description for the GeoRefSubMap
 * 
 * 6     20-12-99 12:42 Wind
 * bug in MapSubmapCorners
 * 
 * 5     23-11-99 12:58 Wind
 * added mapsubmapcorners and mapsubmapcoords
 * 
 * 4     22-11-99 13:24 Wind
 * working on extension of submap
 * 
 * 3     9/08/99 11:51a Wind
 * comment problem
 * 
 * 2     9/08/99 8:57a Wind
 * changed sName() to sNameQuoted() in sExpression() to support long file
 * names
*/
// Revision 1.5  1998/09/16 17:24:31  Wim
// 22beta2
//
// Revision 1.4  1997/09/29 12:55:07  Wim
// Corrected the check on size in constructor
//
// Revision 1.3  1997-09-11 09:51:40+02  martin
// No submaps possiblewhich do not contain at least a part of the map.
//
// Revision 1.2  1997/08/14 18:04:22  Wim
// Improved sSyntax() line
//
/* MapSubMap
   Copyright Ilwis System Development ITC
   august 1995, by Jelle Wind
	Last change:  WK   29 Sep 97    2:54 pm
*/
#include "Applications\Raster\MAPSUB.H"
#include "Engine\Base\DataObjects\valrange.h"
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

MapSubMap* MapSubMap::create(const FileName& fn, MapPtr& p, const String& sExpr)
{
  Array<String> as;
  String sFunc = IlwisObjectPtr::sParseFunc(sExpr);
  int iParms = IlwisObjectPtr::iParseParm(sExpr, as);
  if ((iParms < 3) || (iParms > 6)) 
    ExpressionError(sExpr, sSyntax());
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
  else if (fCIStrEqual(sFunc, "MapSubMapCorners")) {
    RowCol rcOffset, rcSize;
    rcOffset.Row = as[1].iVal()-1L;
    rcOffset.Col = as[2].iVal()-1L;
    rcSize.Row = as[3].iVal() - as[1].iVal() + 1;
    rcSize.Col = as[4].iVal() - as[2].iVal() + 1;
    String sGrf = fn.sFile;
    if (iParms == 6)
      sGrf = as[iParms-1];
    if (sGrf.length() == 0)
      ExpressionError(sExpr, sSyntax2());
    return new MapSubMap(fn, p, mp, rcSize, rcOffset, crdUNDEF, crdUNDEF, sGrf, true);
  }
  else if (fCIStrEqual(sFunc, "MapSubMapCoords")) {
    Coord crd1, crd2;
    crd1.x = as[1].rVal();
    crd1.y = as[2].rVal();
    crd2.x = as[3].rVal();
    crd2.y = as[4].rVal();
    String sGrf = fn.sFile;
    if (iParms == 6)
      sGrf = as[iParms-1];
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
  htpFreeze = htpMapSubMapT;
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
