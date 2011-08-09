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

#include "Applications\Raster\MAPCLCMP.H"
#include "Engine\Representation\Rprclass.h"
#include "Engine\Table\TBLHIST.H"
#include "Engine\Base\DataObjects\WPSMetaData.h"
#include "Headers\Htp\Ilwisapp.htp"
#include "Headers\Err\Ilwisapp.err"

IlwisObjectPtr * createMapColorComp(const FileName& fn, IlwisObjectPtr& ptr, const String& sExpr, vector<void *> parms ) {
	if ( sExpr != "")
		return (IlwisObjectPtr *)MapColorComp::create(fn, (MapPtr &)ptr, sExpr);
	else
		return (IlwisObjectPtr *)new MapColorComp(fn, (MapPtr &)ptr);
}

String wpsmetadataMapColorComp() {
	WPSMetaData metadata("MapColorComp");


	return metadata.toString();
}

ApplicationMetadata metadataMapColorComp(ApplicationQueryData *query) {
	ApplicationMetadata md;
	if ( query->queryType == "WPSMETADATA" || query->queryType == "") {
		md.wpsxml = wpsmetadataMapColorComp();
	}
	if ( query->queryType == "OUTPUTTYPE" || query->queryType == "")
		md.returnType = IlwisObject::iotRASMAP;
	if ( query->queryType == "EXPERSSION" || query->queryType == "")
		md.skeletonExpression =  MapColorComp::sSyntax();

	return md;
}
const char* MapColorComp::sSyntax() 
{ return "MapColorCompLinear(maplist,range1,range2,range3)\nMapColorCompHistEq(maplist,range1,range2,range3)\n"
"MapColorComp24Linear(maplist,range1,range2,range3)\nMapColorComp24HistEq(maplist,range1,range2,range3)"
"MapColorComp24(maplist)\nMapColorComp24HSI(maplist)"; }

MapColorComp* MapColorComp::create(const FileName& fn, MapPtr& p, const String& sExpr)
{
  String sFunc = IlwisObjectPtr::sParseFunc(sExpr);
  Array<String> as;
  int  iParms = IlwisObjectPtr::fParseParm(sExpr, as);
  bool f24Bit = false;
  bool fLinear = false;
  bool fHSI = false;
  if ( fCIStrEqual(sFunc, "MapColorComp") && iParms == 5) {
	  f24Bit = strstr(as[1].c_str(), "24") != 0;
	  fLinear = strstr(as[1].c_str(), "Linear") != 0;
	  fHSI = strstr(as[1].c_str(), "HSI") != 0;

  } else {
	  if (fCIStrEqual(sFunc, "MapColorCompLinear")   || fCIStrEqual(sFunc, "MapColorComp") ||
		  fCIStrEqual(sFunc, "MapColorComp24Linear") || fCIStrEqual(sFunc, "MapColorComp24"))
		fLinear = true;
	  else if (fCIStrEqual(sFunc,"MapColorCompHistEq") || fCIStrEqual(sFunc, "MapColorComp24HistEq"))
		fLinear = false;
	  else if (fCIStrEqual(sFunc, "MapColorComp24HSI"))
		fHSI = true;
	  else
		AppNameError(fn, sFunc);
	  bool f24Bit = strstr(sFunc.sVal(), "24") != 0;
  }
  if ((iParms != 1) && (iParms != 4))
    ExpressionError(sExpr, sSyntax());
  MapList mpl(as[0], fn.sPath());
  if (iParms == 1) 
    if (f24Bit || fHSI)
      return new MapColorComp(fn, p, mpl, rUNDEF, RangeReal(), rUNDEF, RangeReal(), rUNDEF, RangeReal(), true, f24Bit, fHSI);
    //else if (fHSI)
    //  return new MapColorComp(fn, p, mpl, rUNDEF, RangeReal(), rUNDEF, RangeReal(), rUNDEF, RangeReal(), true, false);
    else
      ExpressionError(sExpr, sSyntax());
  double rPerc1 = as[1].rVal();
  RangeReal rr1(as[1]);
  double rPerc2 = as[2].rVal();
  RangeReal rr2(as[2]);
  double rPerc3 = as[3].rVal();
  RangeReal rr3(as[3]);
  return new MapColorComp(fn, p, mpl, rPerc1, rr1, rPerc2, rr2, rPerc3, rr3, fLinear, f24Bit);
}

MapColorComp::MapColorComp(const FileName& fn, MapPtr& p)
: MapFromMapList(fn, p)
{
  String s;
  ReadElement("MapFromMapList", "Type", s);
  fLinear = fCIStrEqual(s, "MapColorCompLinear");
  ReadElement("MapColorComp", "RangeRed", rr1);
  rPerc1 = rUNDEF;
  if (!rr1.fValid())
   ReadElement("MapColorComp", "PercRed", rPerc1);
  ReadElement("MapColorComp", "RangeGreen", rr2);
  rPerc2 = rUNDEF;
  if (!rr2.fValid())
    ReadElement("MapColorComp", "PercGreen", rPerc2);
  ReadElement("MapColorComp", "RangeBlue", rr3);
  rPerc3 = rUNDEF;
  if (!rr3.fValid())
    ReadElement("MapColorComp", "PercBlue", rPerc3);
  if (0 == ReadElement("MapColorComp", "24Bit", f24Bit))
    f24Bit = false;
  if (0 == ReadElement("MapColorComp", "HSI", fHSI))
    fHSI = false;
  fNeedFreeze = !f24Bit;
  sFreezeTitle = "MapColorComp";
  htpFreeze = "ilwisapp\\color_composite_algorithm.htm";
}

MapColorComp::MapColorComp(const FileName& fn, MapPtr& p, const MapList& mpl, double _rPerc1, const RangeReal& _rr1,
 double _rPerc2, const RangeReal& _rr2, double _rPerc3, const RangeReal& _rr3, bool fLin, bool f24Bt, bool _fHSI)
: MapFromMapList(fn, p, mpl, Domain("colorcmp")), 
  rr1(_rr1), rPerc1(_rPerc1), rr2(_rr2), rPerc2(_rPerc2), rr3(_rr3), rPerc3(_rPerc3), fLinear(fLin), f24Bit(f24Bt)
{
  sFreezeTitle = "MapColorComp";
  htpFreeze = "ilwisapp\\color_composite_algorithm.htm";
  if (!fnObj.fValid())
    objtime = objdep.tmNewest();
  fHSI = _fHSI && ((rPerc1 == rUNDEF) && !rr1.fValid());
  if (f24Bit || fHSI)
    SetDomainValueRangeStruct(Domain("color"));
  fNeedFreeze = !f24Bit;
  if (rPerc1 != rUNDEF)
    if ((rPerc1 < 0) || (rPerc1 > 50))
      PercError(rPerc1, fnObj, errMapColorComp+1, 50);
  if (rPerc2 != rUNDEF)
    if ((rPerc2 < 0) || (rPerc2 > 50))
      PercError(rPerc2, fnObj, errMapColorComp+1, 50);
  if (rPerc2 != rUNDEF)
    if ((rPerc2 < 0) || (rPerc2 > 50))
      PercError(rPerc1, fnObj, errMapColorComp+1, 50);
  // check on image domain
  Map mp = mpl->map(mpl->iLower());
  if (0 == mp->dm()->pdi())
    throw ErrorImageDomain(mp->dm()->sName(true, fnObj.sPath()), mp->fnObj, errMapColorComp);
  mp = mpl->map(mpl->iLower()+1);
  if (0 == mp->dm()->pdi())
    throw ErrorImageDomain(mp->dm()->sName(true, fnObj.sPath()), mp->fnObj, errMapColorComp);
  mp = mpl->map(mpl->iLower()+2);
  if (0 == mp->dm()->pdi())
    throw ErrorImageDomain(mp->dm()->sName(true, fnObj.sPath()), mp->fnObj, errMapColorComp);
}

void MapColorComp::Store()
{
  MapFromMapList::Store();
  if (fLinear)
    WriteElement("MapFromMapList", "Type", "MapColorCompLinear");
  else
    WriteElement("MapFromMapList", "Type", "MapColorCompHistEq");
  if (rPerc1 == rUNDEF)
    WriteElement("MapColorComp", "RangeRed", rr1);
  else
    WriteElement("MapColorComp", "PercRed", rPerc1);
  if (rPerc2 == rUNDEF)
    WriteElement("MapColorComp", "RangeGreen", rr2);
  else
    WriteElement("MapColorComp", "PercGreen", rPerc2);
  if (rPerc3 == rUNDEF)
    WriteElement("MapColorComp", "RangeBlue", rr3);
  else
    WriteElement("MapColorComp", "PercBlue", rPerc3);
  WriteElement("MapColorComp", "24Bit", f24Bit);
  WriteElement("MapColorComp", "HSI", fHSI);
}

MapColorComp::~MapColorComp()
{
}

void MapColorComp::InitCalc(const Map& mp, double rPerc, RangeReal& rr, Array<long>& iBnd, int iInf)
{
//  TableHistogramInfo histinf;
  if ((rPerc != rUNDEF) || !fLinear) {
    try {
      histinf[iInf] = TableHistogramInfo(mp);
    }
    catch (const ErrorObject&) {
      if (rPerc == 1)
        rr = mp->rrPerc1(true);
      else
        rr = mp->rrMinMax(BaseMapPtr::mmmCALCULATE);
    }
    if (histinf[iInf].fValid() && (rPerc != rUNDEF))
      rr = histinf[iInf].rrMinMax(rPerc);
    trq.SetTitle(sFreezeTitle);
  }
  trq.SetText(sFreezeTitle);
  if (histinf[iInf].fValid()) {
    if (rPerc != rUNDEF) 
      rr = histinf[iInf].rrMinMax(rPerc);
  }
  iBnd.Resize(256);
  int iMax = f24Bit ? 255 : 5;
  long iRawFromLo = mp->dvrs().iRaw(rr.rLo());
  long iRawFromHi = mp->dvrs().iRaw(rr.rHi());
  for (long i=0; i < iRawFromLo; i++)
    iBnd[i] = 0;
  if (fLinear) {
    int j1=iRawFromLo, j2;
    for (int k=0; k <= iMax; ++k) {
      j2 = iRawFromLo + round((iRawFromHi - iRawFromLo) * (k+1) / (double)(iMax+1));
      for (int j=j1; j <= j2; ++j)
        iBnd[j] = k;
      j1 = j2+1;
    }  
  }  
  else {
    if (histinf[iInf].fValid()) {
      double rPercStep = 100.0 / (iMax+1);
      Column colCumPct = histinf[iInf].colNPixCumPct();
      for (long i=iRawFromLo; i <= iRawFromHi; i++)
        iBnd[i] = longConv(floor(colCumPct->rValue(i) / rPercStep));
    }  
    else
      for (long i=iRawFromLo; i <= iRawFromHi; i++)
        iBnd[i] = 0;
  }    
  for (long i=iRawFromHi+1; i <= 255; i++)
    iBnd[i] = iMax;
  
  trq.SetTitle(sFreezeTitle);
}

void MapColorComp::InitCalc()
{
  InitCalc(mpl->map(mpl->iLower()), rPerc1, rr1, iBnd1, 0);
  InitCalc(mpl->map(mpl->iLower()+1), rPerc2, rr2, iBnd2, 1);
  InitCalc(mpl->map(mpl->iLower()+2), rPerc3, rr3, iBnd3, 2);
}

long MapColorComp::iComputePixelRaw(RowCol rc) const
{
  LongBuf buf(1);
  ComputeLineRaw(rc.Row, buf, rc.Col, 1);
  return buf[0];
}

void MapColorComp::ComputeLineRaw(long Line, LongBuf& buf, long iFrom, long iNum) const
{
  if (f24Bit && !fHSI) {
    bool fStretch = true; //rPerc1 != rUNDEF;
    long i;
    if (iBnd1.iSize() == 0) {
      const_cast<MapColorComp *>(this)->InitCalc();
    }
    ByteBuf bf(iNum);
    mpl->map(mpl->iLower())->GetLineRaw(Line, bf, iFrom, iNum);
    if (fStretch)
      for (i = 0; i < iNum; ++i)
        buf[i] = iBnd1[bf[i]];
    else   
      for (i = 0; i < iNum; ++i)
        buf[i] = bf[i];
    mpl->map(mpl->iLower()+1)->GetLineRaw(Line, bf, iFrom, iNum);
    if (fStretch)
      for (i = 0; i < iNum; ++i)
        bf[i] = (byte)iBnd2[bf[i]];
    for (i = 0; i < iNum; ++i)
      buf[i] += (word)bf[i] << 8;
    mpl->map(mpl->iLower()+2)->GetLineRaw(Line, bf, iFrom, iNum);
    if (fStretch)
      for (i = 0; i < iNum; ++i)
        bf[i] = (byte)iBnd3[bf[i]];
    for (i = 0; i < iNum; ++i)
      buf[i] += (long)bf[i] << 16;
    return;
  }
  else if (fHSI) {
    long i;
    ByteBuf bfHue(iNum);
    ByteBuf bfSat(iNum);
    ByteBuf bfInt(iNum);
    mpl->map(mpl->iLower())->GetLineRaw(Line, bfHue, iFrom, iNum);
    mpl->map(mpl->iLower()+1)->GetLineRaw(Line, bfSat, iFrom, iNum);
    mpl->map(mpl->iLower()+2)->GetLineRaw(Line, bfInt, iFrom, iNum);
    Color col;
    for (i = 0; i < iNum; ++i) {
      col.SetHSI(bfHue[i], bfSat[i], bfInt[i]);
      buf[i] = (long) col;
    }    
    return;
  }
  if (iBnd1.iSize() == 0)
    const_cast<MapColorComp *>(this)->InitCalc();
  LongBuf bf(iNum);
  mpl->map(mpl->iLower())->GetLineVal(Line, bf, iFrom, iNum);
  for (long i = 0; i < iNum; ++i)
    buf[i] = 36*iBnd1[bf[i]];
  mpl->map(mpl->iLower()+1)->GetLineVal(Line, bf, iFrom, iNum);
  for (long i = 0; i < iNum; ++i)
    buf[i] += 6*iBnd2[bf[i]];
  mpl->map(mpl->iLower()+2)->GetLineVal(Line, bf, iFrom, iNum);
  for (long i = 0; i < iNum; ++i)
    buf[i] += iBnd3[bf[i]];
}

String MapColorComp::sExpression() const
{
  if (fHSI && (rPerc1 == rUNDEF) && !rr1.fValid())
    return String("MapColorComp24HSI(%S)", mpl->sNameQuoted(true, fnObj.sPath()));
  if (f24Bit && (rPerc1 == rUNDEF) && !rr1.fValid())
    return String("MapColorComp24(%S)", mpl->sNameQuoted(true, fnObj.sPath()));
  String s1, s2, s3;
  if (rPerc1 == rUNDEF)
    s1 = rr1.s();
  else
    s1 = String("%g", rPerc1);
  if (rPerc2 == rUNDEF)
    s2 = rr2.s();
  else
    s2 = String("%g", rPerc2);
  if (rPerc3 == rUNDEF)
    s3 = rr3.s();
  else
    s3 = String("%g", rPerc3);
  String s("MapColorComp");
  if (f24Bit)
    s &= "24";
  if (fLinear)
    s &= "Linear";
  else
    s &= "HistEq";
  
  return String("%S(%S,%S,%S,%S)", s, mpl->sNameQuoted(true, fnObj.sPath()), s1, s2, s3);
}




