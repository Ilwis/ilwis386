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
/* 1    14/02/01 16:14 Lichun
 * Made sure that  that the undefs values in the output map correct
 *
/* MapDensify
   Copyright Ilwis System Development ITC
   august 1995, by Jelle Wind
	Last change:  MS   17 Nov 97   10:10 am
*/
#include "Applications\Raster\MAPDENSF.H"
#include "Engine\Base\DataObjects\valrange.h"
#include "Engine\Domain\Dmvalue.h"
#include "Engine\SpatialReference\Grfactor.h"
#include "Engine\Base\DataObjects\WPSMetaData.h"
#include "Engine\Base\objdepen.h"
#include "Headers\Htp\Ilwisapp.htp"
#include "Headers\Err\Ilwisapp.err"
#include "Headers\Hs\map.hs"

IlwisObjectPtr * createMapDensify(const FileName& fn, IlwisObjectPtr& ptr, const String& sExpr, vector<void *> parms ) {
	if ( sExpr != "")
		return (IlwisObjectPtr *)MapDensify::create(fn, (MapPtr &)ptr, sExpr);
	else
		return (IlwisObjectPtr *)new MapDensify(fn, (MapPtr &)ptr);
}

String wpsmetadataMapDensify() {
	WPSMetaData metadata("MapDensify");
	metadata.AddTitle("MapDensify");
	metadata.AddAbstract("allows you to reduce the pixel size of a raster map, i.e. the number of rows and columns is increased in the output map");
	metadata.AddKeyword("spatial");
	metadata.AddKeyword("raster");
	metadata.AddKeyword("georeference");
	WPSParameter *parm1 = new WPSParameter("1","Input Map",WPSParameter::pmtRASMAP);
	parm1->AddAbstract("Input raster map with associated attribute table");
	WPSParameter *parm2 = new WPSParameter("2","Enlargement Factor", WPSParameter::pmtREAL);
	parm2->AddAbstract("enlargement factor (real value > 1)");
	WPSParameter *parm3= new WPSParameter("2","Interpolation method", WPSParameter::pmtENUM);
	parm3->AddAbstract("Method of how pixel values are calculated");
	metadata.AddParameter(parm1);
	metadata.AddParameter(parm2);
	metadata.AddParameter(parm3);
	WPSParameter *parmout = new WPSParameter("Result","Output Map", WPSParameter::pmtRASMAP, false);
	parmout->AddAbstract("reference Outputmap and supporting data objects");
	metadata.AddParameter(parmout);
	

	return metadata.toString();
}

ApplicationMetadata metadataMapDensify(ApplicationQueryData *query) {
	ApplicationMetadata md;
	if ( query->queryType == "WPSMETADATA" || query->queryType == "") {
		md.wpsxml = wpsmetadataMapDensify();
	}
	if ( query->queryType == "OUTPUTTYPE" || query->queryType == "")
		md.returnType = IlwisObject::iotRASMAP;
	if ( query->queryType == "EXPERSSION" || query->queryType == "")
		md.skeletonExpression =  MapDensify::sSyntax();

	return md;
}

#define rHalf 0.4999999999

const char* MapDensify::sSyntax() {
  return "MapDensify(map,factor,method)\nmethod = nearestneighbour | bilinear | bicubic ";
}

static const char * sMethods[] = { "BiLinear", "BiCubic", "NearestNeighbour",  0 };

enum method { BiLin = 0, BiCubic, NearestNeighbour };

static int iFind(const String& s, const char* sArr[])
{
  int i = 0; 
  while (sArr[i]) {
    if (_strnicmp(sArr[i], s.scVal(), s.length()) == 0)
      return i;
    i++;
  }
  return shUNDEF;
}

class DATEXPORT ErrorEnlarge: public ErrorObject
{
public:
  ErrorEnlarge(double rEnlFac, const WhereError& where)
  : ErrorObject(WhatError(SMAPErrTooSmallFactor, errMapDensify+1), where) {}
};

void EnlargeError(double rEnlFac, const FileName& fn)
{
  throw ErrorEnlarge(rEnlFac, fn);
}

class DATEXPORT ErrorMethod: public ErrorObject
{
public:
  ErrorMethod(const String& sMethod, const WhereError& where)
  : ErrorObject(WhatError(String(SMAPErrInvalidDensifyMethod_S.scVal(), sMethod), errMapDensify+2), where) {}
};

void MethodError(const String& sMethod, const FileName& fn)
{
  throw ErrorMethod(sMethod, fn);
}

MapDensify* MapDensify::create(const FileName& fn, MapPtr& p, const String& sExpr)
{
  Array<String> as(3);
  if (!IlwisObjectPtr::fParseParm(sExpr, as))
    ExpressionError(sExpr, sSyntax());
  Map mp(as[0], fn.sPath());
  double rEnlFac = as[1].rVal();
  if (rEnlFac <= 1)
    EnlargeError(rEnlFac, fn);
  int iMethod = iFind(as[2], sMethods);
  if (iMethod == shUNDEF)
    MethodError(as[2], fn);
  return new MapDensify(fn, p, mp, rEnlFac, iMethod);
}

MapDensify::MapDensify(const FileName& fn, MapPtr& p)
: MapFromMap(fn, p), bufListValReal(0)
{
  String sMeth;
  ReadElement("MapDensify", "Method", sMeth);
  iMethod = iFind(sMeth, sMethods);
  if (iMethod == shUNDEF)
    MethodError(sMeth, fnObj);
  if (!dvrs().fValues() && iMethod!=NearestNeighbour)
    ValueDomainError(dm()->sName(true, fnObj.sPath()), fnObj, errMapDensify);    
  ReadElement("MapDensify", "EnlargeFactor", rEnlFac);
  if (rEnlFac <= 1)
    EnlargeError(rEnlFac, fnObj);
  ReadGeoRefFromFileAndSet();
  Init();
}

MapDensify::MapDensify(const FileName& fn, MapPtr& p,
           const Map& mp, double rEnlarge, int iMeth)
: MapFromMap(fn, p, mp), 
  rEnlFac(rEnlarge), 
  iMethod(iMeth), 
  bufListValReal(0)
{
  if (iMethod != NearestNeighbour && !dvrs().fValues())
    ValueDomainError(dm()->sName(true, fnObj.sPath()), sTypeName(), errMapDensify);
  if (mp->gr()->fGeoRefNone())
    ptr.SetSize(RowCol(mp->iLines()*rEnlFac, mp->iCols()*rEnlFac));
  else {
    GeoRef grNew;
    grNew.SetPointer(new GeoRefFactor(FileName(fn, ".grf"), mp->gr(), rEnlarge));
    ptr.SetGeoRef(grNew);
    ptr.SetSize(grNew->rcSize());
  }
  Init();
	ptr.gr()->SetDescription(String(SMAPMsgGeoRefCreatedFrom_S.scVal(), sExpression()));
	ptr.gr()->Store();
	objdep.Add(ptr.gr());

  if (mp->fTblAtt()) 
    SetAttributeTable(mp->tblAtt());
  if (!fnObj.fValid())
    objtime = objdep.tmNewest();
}

void MapDensify::Store()
{
  MapFromMap::Store();
  WriteElement("MapFromMap", "Type", "MapDensify");
  WriteElement("MapDensify", "Method", sMethods[iMethod]);
  WriteElement("MapDensify", "EnlargeFactor", rEnlFac);
}

MapDensify::~MapDensify()
{
  if (0 != bufListValReal)
    delete [] bufListValReal;
}

void MapDensify::Init()
{
  sFreezeTitle = "MapDensify";
  htpFreeze = "ilwisapp\\densify_algorithm.htm";
  long i;
  long iInpMapCols = mp->iCols();
  iLastInpLine = iUNDEF;
  iLastInpFrom = iUNDEF;
  iLastInpNum = iUNDEF;
  switch (iMethod) {
    case NearestNeighbour: 
      iLinesStored = 1;
      break;
    case BiLin:
      iLinesStored = 2;
      break;
    case BiCubic:
      iLinesStored = 4;
      break;
  }

  rDL.Resize(iLines());
  rDC.Resize(iCols());
  iInpLine.Resize(iLines());  // array with start input lines for each output line
  iInpFirstCol.Resize(iCols());    // array with start input columns for each output column
  iInpLastCol.Resize(iCols());    // array with last input columns for each output column
  bufListValReal = new RealBuf[iLinesStored];
  for (i=0; i < iLinesStored; i++)
     bufListValReal[i].Size(iCols());

  if (iMethod == NearestNeighbour) {
    for (i=0; i < iLines(); i++)
      iInpLine[i] = (long)floor(i/rEnlFac+0.000001);
    for (i=0; i < iCols(); i++) {
      iInpFirstCol[i] = (long)floor(i/rEnlFac+0.000001);
      iInpLastCol[i] = iInpFirstCol[i];
    }
    return;
  }
  double r;
  for (i=0; i < iLines(); i++) {
    r = (i + rHalf) / rEnlFac - rHalf;
    long l = (long)floor(r);
    iInpLine[i] = l;
    if (iMethod == BiCubic)
      iInpLine[i]--;
    r -= l;
    rDL[i] = r;
  }
  for (i=0; i < iCols(); i++) {
    r = (i + rHalf) / rEnlFac - rHalf;
    long l = (long)floor(r);
    iInpFirstCol[i] = l;
    if (iMethod == BiCubic)
      iInpFirstCol[i]--;
    iInpLastCol[i] = l;
    if (iInpLastCol[i] < 0)
      iInpLastCol[i] = 0;
    else if (iInpLastCol[i] > iInpMapCols-1)
      iInpLastCol[i] = iInpMapCols-1;
    r -= l;
    rDC[i] = r;
  }
  ValueRange vr = dvrs().vr();
  if (!vr.fValid())
    rrMinMax = dm()->pdv()->rrMinMax();
  else
    rrMinMax = vr->rrMinMax();
}

long MapDensify::iComputePixelRaw(RowCol rc) const
{
  RealBuf buf(1);
  ComputeLineVal(rc.Row, buf, rc.Col, 1);
//if (dvrs().fRawIsValue())
  if (!dvrs().fValues())
    return longConv(buf[0]);
  return dvrs().iRaw(buf[0]);
}

double MapDensify::rComputePixelVal(RowCol rc) const
{
  RealBuf buf(1);
  ComputeLineVal(rc.Row, buf, rc.Col, 1);
  return buf[0];
}

void MapDensify::ComputeLineRaw(long Line, ByteBuf& buf, long iFrom, long iNum) const
{
  RealBuf rb(iNum);
  ComputeLineVal(Line, rb, iFrom, iNum);
  if (!dvrs().fValues())
    for (long i = 0; i < iNum; ++i)
      buf[i] = byteConv(rb[i]);
  else 
    for (long i = 0; i < iNum; ++i)
	    buf[i] = byteConv(dvrs().iRaw(rb[i]));
}

void MapDensify::ComputeLineRaw(long Line, IntBuf& buf, long iFrom, long iNum) const
{
  RealBuf rb(iNum);
  ComputeLineVal(Line, rb, iFrom, iNum);
  if (!dvrs().fValues())
    for (long i = 0; i < iNum; ++i)
      buf[i] = shortConv(rb[i]);
  else
    for (long i = 0; i < iNum; ++i)
			buf[i] = shortConv(dvrs().iRaw(rb[i]));
}

void MapDensify::ComputeLineRaw(long Line, LongBuf& buf, long iFrom, long iNum) const
{
  RealBuf rb(iNum);
  ComputeLineVal(Line, rb, iFrom, iNum);
  if (!dvrs().fValues())
    for (long i = 0; i < iNum; ++i)
      buf[i] = longConv(rb[i]);
  else
    for (long i = 0; i < iNum; ++i)
      buf[i] = dvrs().iRaw(rb[i]);
}

	
void MapDensify::ComputeLineVal(long Line, LongBuf& buf, long iFrom, long iNum) const
{
  RealBuf rb(iNum);
  ComputeLineVal(Line, rb, iFrom, iNum);
  for (int i=0; i < iNum; i++)
    buf[i] = longConv(rb[i]);
}

void MapDensify::ComputeLineVal(long Line, RealBuf& buf, long iFrom, long iNum) const
{
   const_cast<MapDensify *>(this)->ReadBufLines(Line, iFrom, iNum);
   switch (iMethod) {
    case NearestNeighbour: 
      const_cast<MapDensify *>(this)->ComputeNearest(Line, buf, iFrom, iNum);
      break;
    case BiLin:
      const_cast<MapDensify *>(this)->ComputeBiLin(Line, buf, iFrom, iNum);
      break;
    case BiCubic:
      const_cast<MapDensify *>(this)->ComputeCubic(Line, buf, iFrom, iNum);
      break;
  }
}

void MapDensify::ReadLineVal(long iLine, RealBuf& buf, long iFrom, long iNum)
{
  if (iLine < 0)
    iLine = 0;
  else if (iLine >= mp->iLines())
    iLine = mp->iLines()-1;
//  if (dvrs().fRawAvailable())
  if (!dvrs().fValues())
  {
    LongBuf longBuf(buf.iSize());
    mp->GetLineRaw(iLine, longBuf, iFrom, iNum);
    for(long i=0; i<buf.iSize(); ++i)
        buf[i]=doubleConv(longBuf[i]);
  }           
  else  
    mp->GetLineVal(iLine, buf, iFrom, iNum);
}

void MapDensify::ReadBufLines(long iLine, long iFrom, long iNum)
{
  long iInputLine = iInpLine[iLine];
  long iInputFrom = iInpFirstCol[iFrom];
  if (iInputFrom < 0)
    iInputFrom = 0;
  long iInputNum = iInpLastCol[iFrom+iNum-1] - iInputFrom + 1;
  if ((iInputFrom == iLastInpFrom) && (iInputNum == iLastInpNum)) {
    if (iInputLine == iLastInpLine) // already read in
      return;
    if (iLastInpLine == iInputLine - 1) { // roulate buffers and only read last line
      for (int i=0; i < iLinesStored-1; i++)
        Swap(bufListValReal[i], bufListValReal[i+1]);
        
      ReadLineVal(iInputLine+iLinesStored-1, bufListValReal[iLinesStored-1], iInputFrom, iInputNum);
      iLastInpLine = iInputLine;
      return;
    }
  }
  // read all lines
  for (int i=0; i < iLinesStored; i++)
    ReadLineVal(iInputLine+i, bufListValReal[i], iInputFrom, iInputNum);
    
  iLastInpLine = iInputLine;
  iLastInpFrom = iInputFrom;
  iLastInpNum = iInputNum;
}

String MapDensify::sExpression() const
{
  return String("MapDensify(%S,%g,%s)", mp->sNameQuoted(true, fnObj.sPath()), rEnlFac, sMethods[iMethod]);
}

void MapDensify::ComputeNearest(long Line, RealBuf& buf, long iFrom, long iNum)
{
  long iCol;
  for (long i=iFrom; i < iFrom + iNum; i++) 
  {
    iCol = iInpFirstCol[i];
    buf[i] = bufListValReal[0][iCol];
  }
}

void MapDensify::ComputeBiLin(long Line, RealBuf& buf, long iFrom, long iNum)
{
  long iCol, iColNext;
  double rDifCol;
  for (long i=iFrom; i < iFrom + iNum; i++) {
    iCol = iInpFirstCol[i];
    iColNext = iCol+1;
    if (iCol < 0)
      iCol=0;
    if (iColNext < 0)
      iColNext=0;
    else if (iColNext >= mp->iCols())
      iColNext = mp->iCols()-1;

    rDifCol = rDC[i];
    buf[i] = rBiLin(rBiLin(bufListValReal[0][iCol], bufListValReal[0][iColNext], rDifCol),
                    rBiLin(bufListValReal[1][iCol], bufListValReal[1][iColNext], rDifCol),
                    rDL[Line]);
  }
}

void MapDensify::ComputeCubic(long Line, RealBuf& buf, long iFrom, long iNum)
{
  double rDifCol;
  long iCol, iColNext, iColNext2, iColNext3;
  for (long i=iFrom; i < iFrom + iNum; i++) {
    iCol = iInpFirstCol[i];
    iColNext = iCol+1;
    iColNext2 = iCol+2;
    iColNext3 = iCol+3;
    if (iCol < 0)
      iCol = 0;
    if (iColNext < 0)
      iColNext = 0;
    if (iColNext >= mp->iCols())
      iColNext = mp->iCols()-1;
    if (iColNext2 >= mp->iCols())
      iColNext2 = mp->iCols()-1;
    if (iColNext3 >= mp->iCols())
      iColNext3 = mp->iCols()-1;
    rDifCol = rDC[i];
//    buf[i] = rCubic(bufListValReal[0][iCol], bufListValReal[1][iCol],
//                           bufListValReal[2][iCol], bufListValReal[3][iCol], rDL[Line]);
//    buf[i] = rCubic(bufListValReal[0][iCol], bufListValReal[0][iColNext],
//                           bufListValReal[0][iColNext2], bufListValReal[0][iColNext3], rDifCol);
     
    buf[i] = rCubic(rCubic(bufListValReal[0][iCol], bufListValReal[0][iColNext],
                           bufListValReal[0][iColNext2], bufListValReal[0][iColNext3], rDifCol),
                    rCubic(bufListValReal[1][iCol], bufListValReal[1][iColNext],
                           bufListValReal[1][iColNext2], bufListValReal[1][iColNext3], rDifCol),
                    rCubic(bufListValReal[2][iCol], bufListValReal[2][iColNext],
                           bufListValReal[2][iColNext2], bufListValReal[2][iColNext3], rDifCol),
                    rCubic(bufListValReal[3][iCol], bufListValReal[3][iColNext],
                           bufListValReal[3][iColNext2], bufListValReal[3][iColNext3], rDifCol),
                    rDL[Line]);
    if (buf[i] > rrMinMax.rHi())
      buf[i] = rrMinMax.rHi();
    else if ((buf[i] < rrMinMax.rLo()) && (buf[i] != rUNDEF))  //Made sure the undefs values correct
      buf[i] = rrMinMax.rLo();
  }
}

double MapDensify::rBiLin(const double& a, const double& b, const double& dx)
{
  if ((a==rUNDEF) || (b == rUNDEF))
    return rUNDEF;
  return  a + dx * (b-a);
}

double MapDensify::rCubic(const double& a, const double& b, const double& c, const double& d, const double& dx)
{
  if ((a==rUNDEF) || (b == rUNDEF) || (c==rUNDEF) || (d == rUNDEF))
    return rUNDEF;
  return b + dx * ((c - b/2 - a/3 - d/6) +
             dx * ((c + a)/2 - b + dx * ((b - c)/2 + (d - a)/6)));
}

bool MapDensify::fDomainChangeable() const
{
  return true;
}

bool MapDensify::fValueRangeChangeable() const
{
  return true;
}
