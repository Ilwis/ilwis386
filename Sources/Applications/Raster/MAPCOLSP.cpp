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
/* MapColorSep
   Copyright Ilwis System Development ITC
   sept. 1996, by Jelle Wind
	Last change:  JEL  20 Jun 97    5:37 pm
*/

#include "Applications\Raster\MAPCOLSP.H"
#include "Engine\Base\DataObjects\Color.h"
#include "Engine\Base\DataObjects\valrange.h"
#include "Headers\Htp\Ilwisapp.htp"
#include "Headers\Err\Ilwisapp.err"
#include "Headers\Hs\map.hs"

IlwisObjectPtr * createMapColorSep(const FileName& fn, IlwisObjectPtr& ptr, const String& sExpr, vector<void *> parms ) {
	if ( sExpr != "")
		return (IlwisObjectPtr *)MapColorSep::create(fn, (MapPtr &)ptr, sExpr);
	else
		return (IlwisObjectPtr *)new MapColorSep(fn, (MapPtr &)ptr);
}

#define MAXAREAS 1L

const char* MapColorSep::sSyntax() {
  return "MapColorSep(map,colsep)\ncolsep: red|green|blue|yellow|magenta|cyan|hue|sat|intens|grey";
}

MapColorSep* MapColorSep::create(const FileName& fn, MapPtr& p, const String& sExpr)
{
  Array<String> as;
  int iParms = IlwisObjectPtr::iParseParm(sExpr, as);
  if (iParms != 2)
    ExpressionError(sExpr, sSyntax());
  Map mp(as[0], fn.sPath());
  String sColor;
  if (iParms == 1) {
    String sFunc = IlwisObjectPtr::sParseFunc(sExpr);
    sColor = sFunc.sRight(sFunc.length()-11);
  }
  else
    sColor = as[1];  
  int iColor = iColorType(sColor);
  if (iColor == shUNDEF)
    ExpressionError(sExpr, sSyntax());
  return new MapColorSep(fn, p, mp, sColor);
}

MapColorSep::MapColorSep(const FileName& fn, MapPtr& p)
: MapFromMap(fn, p)
{
  String sColor;
  ReadElement("MapColorSep", "ColorSep", sColor);
  iColor = iColorType(sColor);
  fPicture = 0 != mp->dm()->pdp();
  if (fPicture)
    rpr = mp->dm()->rpr();
  fNeedFreeze = false;
  sFreezeTitle = "MapColorSep";
  htpFreeze = htpMapColorSepT;
}

MapColorSep::MapColorSep(const FileName& fn, MapPtr& p,const Map& mp, const String& sColor)
: MapFromMap(fn, p, mp)
{
  iColor = iColorType(sColor);
  if (iColor == shUNDEF)
    throw ErrorObject(WhatError(String(SMAPErrInvalidColor_S.scVal(), sColor), errMapColorSep), fnObj);

  SetDomainValueRangeStruct(Domain("image"));
assert(dm()->pdi());  
  // check domain of input map (should be picture or color)
  fPicture = 0 != mp->dm()->pdp();
  if (fPicture)
    rpr = mp->dm()->rpr();
  else if (0 == mp->dm()->pdcol())
    throw ErrorObject(
      WhatError(String(SMAPErrPictOrColorRequired_S.scVal(), dm()->sName(true, fnObj.sPath())), errMapColorSep+1), 
      fnObj);
  fNeedFreeze = false;
  if (!fnObj.fValid())
    objtime = objdep.tmNewest();
  sFreezeTitle = "MapColorSep";
  htpFreeze = htpMapColorSepT;
}

void MapColorSep::Store()
{
  MapFromMap::Store();
  WriteElement("MapFromMap", "Type", "MapColorSep");
  WriteElement("MapColorSep", "ColorSep", sColorType(iColor));
}

MapColorSep::~MapColorSep()
{
}

String MapColorSep::sExpression() const
{
//  return String("MapColorSep(%S,%S)", mp->sName(true, fnObj.sPath()), sColorType(iColor));
  return String("%S.%S", mp->sNameQuoted(false, fnObj.sPath()), sColorType(iColor));
}

String MapColorSep::sColorType(int iColor)
{
  switch (iColor) {
    case 0: 
      return "red";
    case 1: 
      return "green";
    case 2: 
      return "blue";
    case 3: 
      return "yellow";
    case 4: 
      return "magenta";
    case 5: 
      return "cyan";
    case 6: 
      return "hue";
    case 7: 
      return "saturation";
    case 8: 
      return "intensity";
    case 9: 
      return "gray";
    default :
      return sUNDEF;
  }
}

int MapColorSep::iColorType(const String& sColor)
{
  if (fCIStrEqual(sColor , "red"))
    return 0; 
  if (fCIStrEqual(sColor , "green"))
    return 1; 
  if (fCIStrEqual(sColor , "blue"))
    return 2; 
  if (fCIStrEqual(sColor , "yellow"))
    return 3; 
  if (fCIStrEqual(sColor , "magenta"))
    return 4; 
  if (fCIStrEqual(sColor , "cyan"))
    return 5; 
  if (fCIStrEqual(sColor , "hue"))
    return 6; 
  if ((fCIStrEqual(sColor , "sat")) || (fCIStrEqual(sColor , "saturation")))
    return 7; 
  if ((fCIStrEqual(sColor , "intens")) || (fCIStrEqual(sColor , "intensity")))
    return 8; 
  if ((fCIStrEqual(sColor , "grey")) || (fCIStrEqual(sColor , "gray")))
    return 9; 
  return shUNDEF;
}

long MapColorSep::iComputePixelRaw(RowCol rc) const
{
  LongBuf buf(1);
  ComputeLineRaw(rc.Row, buf, rc.Col, 1);
  return buf[0];
}

double MapColorSep::rComputePixelVal(RowCol rc) const
{
  RealBuf buf(1);
  ComputeLineVal(rc.Row, buf, rc.Col, 1);
  return buf[0];
}

void MapColorSep::ComputeLineRaw(long Line, ByteBuf& buf, long iFrom, long iNum) const
{
  int i;
  if (fPicture) {
    mp->GetLineRaw(Line, buf, iFrom, iNum);
    switch (iColor) {
      case 0: // red
        for (i = 0; i < buf.iSize(); i++)
          buf[i] = rpr->clrRaw(buf[i]).red();
        break;
      case 1: // green
        for (i = 0; i < buf.iSize(); i++)
          buf[i] = rpr->clrRaw(buf[i]).green();
        break;
      case 2: // blue
        for (i = 0; i < buf.iSize(); i++)
          buf[i] = rpr->clrRaw(buf[i]).blue();
        break;
      case 3: // yellow
        for (i = 0; i < buf.iSize(); i++)
          buf[i] = rpr->clrRaw(buf[i]).yellow();
        break;
      case 4: // magenta
        for (i = 0; i < buf.iSize(); i++)
          buf[i] = rpr->clrRaw(buf[i]).magenta();
        break;
      case 5: // cyan
        for (i = 0; i < buf.iSize(); i++)
          buf[i] = rpr->clrRaw(buf[i]).cyan();
        break;
      case 6: // hue
        for (i = 0; i < buf.iSize(); i++)
          buf[i] = rpr->clrRaw(buf[i]).hue();
        break;
      case 7: // sat
        for (i = 0; i < buf.iSize(); i++)
          buf[i] = rpr->clrRaw(buf[i]).sat();
        break;
      case 8: // int
        for (i = 0; i < buf.iSize(); i++)
          buf[i] = rpr->clrRaw(buf[i]).intens();
        break;
      case 9: // gray
        for (i = 0; i < buf.iSize(); i++)
          buf[i] = rpr->clrRaw(buf[i]).grey();
        break;
      default :
        for (i = 0; i < buf.iSize(); i++)
          buf[i] = 0;
        break;
    }
  }
  else { 
    LongBuf lb(buf.iSize());
    mp->GetLineRaw(Line, lb, iFrom, iNum);
    switch (iColor) {
      case 0: // red
        for (i = 0; i < buf.iSize(); i++)
          buf[i] = Color(lb[i]).red();
        break;
      case 1: // green
        for (i = 0; i < buf.iSize(); i++)
          buf[i] = Color(lb[i]).green();
        break;
      case 2: // blue
        for (i = 0; i < buf.iSize(); i++)
          buf[i] = Color(lb[i]).blue();
        break;
      case 3: // yellow
        for (i = 0; i < buf.iSize(); i++)
          buf[i] = Color(lb[i]).yellow();
        break;
      case 4: // magenta
        for (i = 0; i < buf.iSize(); i++)
          buf[i] = Color(lb[i]).magenta();
        break;
      case 5: // cyan
        for (i = 0; i < buf.iSize(); i++)
          buf[i] = Color(lb[i]).cyan();
        break;
      case 6: // hue
        for (i = 0; i < buf.iSize(); i++)
          buf[i] = Color(lb[i]).hue();
        break;
      case 7: // sat
        for (i = 0; i < buf.iSize(); i++)
          buf[i] = Color(lb[i]).sat();
        break;
      case 8: // int
        for (i = 0; i < buf.iSize(); i++)
          buf[i] = Color(lb[i]).intens();
        break;
      case 9: // grey
        for (i = 0; i < buf.iSize(); i++)
          buf[i] = Color(lb[i]).grey();
        break;
      default :
        for (i = 0; i < buf.iSize(); i++)
          buf[i] = 0;
        break;
    }
  }  
}

void MapColorSep::ComputeLineRaw(long Line, IntBuf& buf, long iFrom, long iNum) const
{
  ByteBuf bb(buf.iSize());
  ComputeLineRaw(Line, bb, iFrom, iNum);
  for (int i = 0; i < buf.iSize(); i++)
    buf[i] = bb[i];
}

void MapColorSep::ComputeLineRaw(long Line, LongBuf& buf, long iFrom, long iNum) const
{
  ByteBuf bb(buf.iSize());
  ComputeLineRaw(Line, bb, iFrom, iNum);
  for (int i = 0; i < buf.iSize(); i++)
    buf[i] = bb[i];
}

void MapColorSep::ComputeLineVal(long Line, LongBuf& buf, long iFrom, long iNum) const
{
  ByteBuf bb(buf.iSize());
  ComputeLineRaw(Line, bb, iFrom, iNum);
  for (int i = 0; i < buf.iSize(); i++)
    buf[i] = bb[i];
}

void MapColorSep::ComputeLineVal(long Line, RealBuf& buf, long iFrom, long iNum) const
{
  ByteBuf bb(buf.iSize());
  ComputeLineRaw(Line, bb, iFrom, iNum);
  for (int i = 0; i < buf.iSize(); i++)
    buf[i] = bb[i];
}




