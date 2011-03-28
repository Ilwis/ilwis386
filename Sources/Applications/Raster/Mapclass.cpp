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
// $Log: /ILWIS 3.0/RasterApplication/Mapclass.cpp $
 * 
 * 5     8/15/01 17:39 Hendrikse
 * checks now in create( )  on equality of domains of SampleSet anf table
 * in case of priorProbab classifier
 * 
 * 4     6-03-01 13:12 Hendrikse
 * In Init() I removed try..catch around clf->SetSampleSet to enable the
 * throw of an error object in case of too small samplesets
 * 
 * 3     9/08/99 11:45a Wind
 * comments
 * 
 * 2     9/08/99 8:53a Wind
 * changed sName() to sNameQuoted() in sExpression() tot suport quoted
 * file names
 */
// Revision 1.5  1998/10/08 10:52:50  Wim
// Check on all zero corrected in ComputeLineRaw()
//
// Revision 1.4  1998-10-07 17:09:34+01  Wim
// Now for all classifiers;
// byte maps when all input are 0 the result will be made 0 (undef)
// int and real maps when one input has ? the result will be amde 0  (undef).
//
// Revision 1.3  1998-09-16 18:24:31+01  Wim
// 22beta2
//
// Revision 1.2  1997/08/05 11:09:08  Wim
// sSyntax() corrected
//
/* MapClassify
   Copyright Ilwis System Development ITC
   april 1995, by Jelle Wind
	Last change:  WK    8 Oct 98   11:52 am
*/
#include "Applications\Raster\MAPCLASS.H"
#include "Headers\Htp\Ilwisapp.htp"
#include "Headers\Err\Ilwisapp.err"
#include "Headers\Hs\map.hs"
#include "Headers\Hs\tbl.hs"
#include "Engine\Function\PRIORPRB.H"

IlwisObjectPtr * createMapClassify(const FileName& fn, IlwisObjectPtr& ptr, const String& sExpr, vector<void *> parms ) {
	if ( sExpr != "")
		return (IlwisObjectPtr *)MapClassify::create(fn, (MapPtr &)ptr, sExpr);
	else
		return (IlwisObjectPtr *)new MapClassify(fn, (MapPtr &)ptr);
}

const char* MapClassify::sSyntax() {
  return "MapClassify(sampleset,classifier)";
}

MapClassify* MapClassify::create(const FileName& fn, MapPtr& p, const String& sExpr)
{
  Array<String> as(2);
  if (!IlwisObjectPtr::fParseParm(sExpr, as))
    ExpressionError(sExpr, sSyntax());
  Classifier clf(as[1]);
  SampleSet sms(as[0], fn.sPath());
	String sClfType = as[1].substr(0, as[1].find("("));
	if (fCIStrEqual(sClfType, "ClassifierPriorProb"))
	{
		Array<String> asCl;		
		int iParms = IlwisObjectPtr::iParseParm(as[1], asCl);
		Table tbl;
		if (iParms == 2)
			tbl = Table(asCl[0]);
		else if (iParms == 3)
			tbl = Table(asCl[1]);
		else
			ExpressionError(as[1], ClassifierPriorProb::sSyntax());
		Domain dmSms = sms->dm();
		Domain dmTbl = tbl->dm();
		if (dmSms != dmTbl)
			throw ErrorObject(WhatError(String(STBLErrIncompDomains_SS.scVal(), as[0],
			FileName(asCl[0]).sRelative()), errClassifier+3), fn);
	}
  return new MapClassify(fn, p, sms, clf);
}

MapClassify::MapClassify(const FileName& fn, MapPtr& p)
: MapFromMap(fn, p),
  bufListByteVal(0), bufListLongVal(0), bufListRealVal(0)
{
  ObjectInfo::ReadElement("MapClassify", "Classify", fn, clf);
  ObjectInfo::ReadElement("MapClassify", "SampleSet", fn, sms);
  objdep.Add(clf.ptr());
  objdep.Add(sms.ptr());
  fNeedFreeze = false;
  fInit = false;
}

MapClassify::MapClassify(const FileName& fn, MapPtr& p, const SampleSet& smpls, const Classifier& clfy)
: MapFromMap(fn, p, smpls->map()), clf(clfy), sms(smpls),
  bufListByteVal(0), bufListLongVal(0), bufListRealVal(0)
{
  objdep.Add(clf.ptr());
  objdep.Add(sms.ptr());
  fNeedFreeze = false;
  if (!fnObj.fValid())
    objtime = objdep.tmNewest(); 
  fInit = false;
}

void MapClassify::Init()
{
  if (fInit)
    return;
  if (!sms.fValid() || !clf.fValid())
    return;
  fInit = true;
  sFreezeTitle = "MapClassify";
  htpFreeze = htpMapClassifyT;
  mpl = sms->mpl();
  sms->fInitStat();
  clf->SetSampleSet(sms);
  fImages = true;
  fIntValues = false;
  int i;
  for (i=mpl->iLower(); i <= mpl->iUpper(); i++)
    if (0 == mpl[i]->dm()->pdi()) {
      fImages = false;
      break;
    }
//fImages = false;
  if (fImages) {
    bufListByteVal = new ByteBuf[mpl->iSize()];
    for (i=0; i < mpl->iSize(); i++)
      bufListByteVal[i].Size(mp->iCols());
    bufListLongVal = 0;
    bufListRealVal = 0;
    bufByteVal = ByteBuf(mpl->iSize());
  }
  else {
    fIntValues = true;
    for (i=mpl->iLower(); i <= mpl->iUpper(); i++)
      if (mpl[i]->dvrs().fRealValues()) {
        fIntValues = false;
        break;
      }
    if (fIntValues) {
      bufListLongVal = new LongBuf[mpl->iSize()];
      for (i=0; i < mpl->iSize(); i++)
        bufListLongVal[i].Size(mp->iCols());
      bufListByteVal = 0;
      bufListRealVal = 0;
      bufLongVal = LongBuf(mpl->iSize());
    }
    else {
      bufListRealVal = new RealBuf[mpl->iSize()];
      for (i=0; i < mpl->iSize(); i++)
        bufListRealVal[i].Size(mp->iCols());
      bufListByteVal = 0;
      bufListLongVal = 0;
      bufRealVal = RealBuf(mpl->iSize());
    }
  }
  trq.SetTitle(sFreezeTitle);
  trq.setHelpItem(htpFreeze);
}

void MapClassify::Store()
{
  MapFromMap::Store();
  WriteElement("MapFromMap", "Type", "MapClassify");
  if (clf.fValid())
    WriteElement("MapClassify", "Classify", clf);
  if (sms.fValid())
    WriteElement("MapClassify", "SampleSet", sms);
}

MapClassify::~MapClassify()
{
  if (bufListLongVal)
    delete [] bufListLongVal;
  if (bufListRealVal)
    delete [] bufListRealVal;
}

long MapClassify::iComputePixelRaw(RowCol rc) const
{
  ByteBuf buf(1);
  ComputeLineRaw(rc.Row, buf, rc.Col, 1);
  if (buf[0] == 0)
    return iUNDEF;
  return buf[0];
}

double MapClassify::rComputePixelVal(RowCol) const
{
  return rUNDEF;
}

void MapClassify::ComputeLineRaw(long iLine, ByteBuf& buf, long iFrom, long iNum) const
{
  int b;
  if (!fInit)
    const_cast<MapClassify *>(this)->Init();
  if (!fInit) {
    for (int c=0; c < buf.iSize(); c++) 
      buf[c] = 0; //iUNDEF;
    return;  
  }  
  if (fImages) {
    for (b = 0; b < mpl->iSize(); b++)
      mpl[b+mpl->iLower()]->GetLineRaw(iLine, bufListByteVal[b], iFrom, iNum);
    clf->Classify(bufListByteVal, buf);
    for (int c=0; c < buf.iSize(); ++c) {
      bool fAllZero = true;
      for (b = 0; b < mpl->iSize(); b++)
        if (0 != bufListByteVal[b][c]) {
          fAllZero = false;
          break;
        }
      if (fAllZero)
        buf[c] = 0;
    }
  }
  else if (fIntValues) {
    for (b = 0; b < mpl->iSize(); b++)
      mpl[b+mpl->iLower()]->GetLineVal(iLine, bufListLongVal[b], iFrom, iNum);
    clf->Classify(bufListLongVal, buf);
    for (int c=0; c < buf.iSize(); ++c) {
      for (b = 0; b < mpl->iSize(); b++)
        if (iUNDEF == bufListLongVal[b][c]) {
          buf[c] = 0;
          break;
        }
    }
  }
  else {
    for (b = 0; b < mpl->iSize(); b++)
      mpl[b+mpl->iLower()]->GetLineVal(iLine, bufListRealVal[b], iFrom, iNum);
    clf->Classify(bufListRealVal, buf);
    for (int c=0; c < buf.iSize(); ++c) {
      for (b = 0; b < mpl->iSize(); b++)
        if (rUNDEF == bufListRealVal[b][c]) {
          buf[c] = 0;
          break;
        }
    }
  }
}

void MapClassify::ComputeLineRaw(long iLine, LongBuf& buf, long iFrom, long iNum) const
{
  ByteBuf bb(buf.iSize());
  ComputeLineRaw(iLine, bb, iFrom, iNum);
  for (int c=0; c < buf.iSize(); c++)
    if (bb[c] == 0)
      buf[c] = iUNDEF;
    else
      buf[c] = bb[c];
}


String MapClassify::sExpression() const
{
  String sClf;
  if (clf.fValid())
    sClf = clf->sName(true, fnObj.sPath());
  return String("MapClassify(%S,%S)", mp->sNameQuoted(true, fnObj.sPath()), sClf);
}




