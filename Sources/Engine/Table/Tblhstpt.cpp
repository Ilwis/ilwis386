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
// $Log: /ILWIS 3.0/Table/Tblhstpt.cpp $
 * 
 * 7     5-03-01 12:38 Koolhoven
 * in Erase() use PostMessage() instead of PostThreadMessage() to update
 * the catalog
 * 
 * 6     4-08-00 14:54 Koolhoven
 * protected against nil valid points
 * 
 * 5     8-02-00 17:58 Wind
 * no update of catalog in Erase() if file was already erased
 * 
 * 4     14-01-00 17:19 Koolhoven
 * Post reread Catalog as thread message
 * 
 * 3     9/08/99 11:55a Wind
 * comments
 * 
 * 2     9/08/99 10:28a Wind
 * adpated to use of quoted file names in sExpression()
 */
// Revision 1.7  1998/09/16 17:25:32  Wim
// 22beta2
//
// Revision 1.6  1997/09/18 12:20:24  Wim
// Send ILW_READCATALOG message after deleting files in Erase()
//
// Revision 1.5  1997-09-01 15:15:51+02  Wim
// set help topic on in Init()
//
// Revision 1.4  1997-08-27 16:33:25+02  martin
// The domain was not set in the freezing function
//
// Revision 1.3  1997/08/15 14:20:33  Wim
// Optimized riIndexMinMax(rPerc)
//
// Revision 1.2  1997-08-12 18:14:45+02  Wim
// Do not try to fill columns when no pts is available
//
/* TableHistogramPnt
   Copyright Ilwis System Development ITC
   april 1995, by Jelle Wind
	Last change:  WK    1 Jul 98   10:03 am
*/

//////// only properly implementef for byte maps

#include "Engine\Table\TBLHSTPT.H"
#include "Engine\Table\tblstore.h"
#include "Engine\Table\COLSTORE.H"
#include "Engine\Domain\Dmvalue.h"
#include "Engine\Applications\PNTVIRT.H"
#include "Engine\Table\COLLONG.H"
#include "Engine\Table\COLREAL.H"
#include "Engine\Base\DataObjects\valrange.h"
#include "Engine\Base\Algorithm\Qsort.h"
#include "Headers\Htp\Ilwisapp.htp"
#include "Headers\Err\Ilwisapp.err"
#include "Headers\constant.h"
#include "Headers\Hs\tbl.hs"

IlwisObjectPtr * createTableHistogramPnt(const FileName& fn, IlwisObjectPtr& ptr, const String& sExpr, vector<void *> parms ) {
	if ( sExpr == "")
		return (IlwisObjectPtr *)new TableHistogramPnt(fn, (TablePtr &)ptr);

	Array<String> as(1);
    if (!IlwisObjectPtr::fParseParm(sExpr, as))
      throw ErrorExpression(fn.sFullName(), sExpr);
    FileName filnam(as[0], ".hss", true);
    if (File::fExist(filnam))
      // histogram file exists already
      return (IlwisObjectPtr *)new TableHistogramPnt(filnam, (TablePtr&)ptr);
    else // create new histogram from map
      return (IlwisObjectPtr *)new TableHistogramPnt(PointMap(String(as[0])), (TablePtr&)ptr);
}

const char* TableHistogramPnt::sSyntax() { return "TableHistogramPnt(map)"; }

TableHistogramPnt::TableHistogramPnt(const FileName& fn, TablePtr& p)
: TableVirtual(fn, p)
{
  FileName fnMap;
  ReadElement("TableHistogramPnt", "Map", fnMap);
  try {
    map = PointMap(fnMap);
  }
  catch (const ErrorObject& err) {
    map = PointMap();
    throw err;
  }
  objdep.Add(map.ptr());
  fNeedFreeze = true;
  if (0 == pts)
    return;
  Init();
  FillColumns();
}

TableHistogramPnt::TableHistogramPnt(PointMap& mp, TablePtr& p)
: TableVirtual(FileName(mp->fnObj, ".hsp", true), p, true)
//               FileName(mp->fnObj, ".hp#"), Domain("none")) //mp->dm())
{
  if (mp->dm()->pdi() || mp->dm()->pdsrt() || mp->dm()->pdbit() || mp->dm()->pdp()) {
    ptr.SetDomain(mp->dm());
    if (iRecs() > 16000)
      throw ErrorObject(WhatError(String(TR("Too large domain").c_str(), mp->sName(true, fnObj.sPath())), errTableHistogramPnt), fnObj);
  }  
  else
    ptr.SetDomain("none");
  map = mp;
  objdep.Add(map.ptr());
  if (!fnObj.fValid())
    objtime = objdep.tmNewest();
  fNeedFreeze = true;
  Freeze();
}

void TableHistogramPnt::Store()
{
  TableVirtual::Store();
  WriteElement("TableVirtual", "Type", "TableHistogramPnt");
  WriteElement("TableHistogramPnt", "Map", map);
}

bool TableHistogramPnt::fFreezing()
{
  if (map->dm()->pdi() || map->dm()->pdsrt() || map->dm()->pdbit())
        ptr.SetDomain(map->dm());
  else
    ptr.SetDomain("none");

  Init();
  if (!fCount())
    return false;
  FillColumns();
  return true;
}

static bool fLessRealVal(long a, long b, void* p)
{
  TableHistogramPnt *his = static_cast<TableHistogramPnt*>(p);
  if (his->trq.fAborted())
    throw ErrorObject();
  return his->arSort[a] < his->arSort[b];
}

static void SwapRealVal(long a, long b, void* p)
{
  TableHistogramPnt *his = static_cast<TableHistogramPnt*>(p);
  double r = his->arSort[a];
  his->arSort[a] = his->arSort[b];
  his->arSort[b] = r;
}  

class HistItemPnt {
public:
  HistItemPnt()
    { r = rUNDEF; iCount = 0; }
  HistItemPnt(double _r)
    {  r = _r; iCount = 0;  }
  bool operator ==(const HistItemPnt& cc)
  { return  cc.r == r; }
  double r;
  long iCount;
};

static  int iHash(const HistItemPnt& hir)
{
  word *pw = static_cast<word*>((void*)&hir.r);
  word w = *pw;
  ++pw;
  for (int i = 1; i <= 3; ++i, ++pw)
    w ^= *pw;
  return w % 16000;
}

bool TableHistogramPnt::fCount()
{
  if (!map->fCalculated())
    map->Calc();
  long iMin = iOffset();
  long iMax = iMin + iRecs() - 1;
  for (long i = iMin; i <= iMax; i++)
    _colFreq->PutVal(i, 0L);
  trq.fText(TR("Determine frequencies"));
  if (0 == dm()->pdnone())
    for (long i=0; i < map->iFeatures(); ++i ) {
      if (trq.fAborted())
        return false;
      long iRaw = map->iRaw(i);
      if (iRaw == iUNDEF)
        continue;
      long iFreq = _colFreq->iValue(iRaw);
      _colFreq->PutVal(iRaw, (long)(iFreq+1));
    }
  else { // use hashtable

    HashTable<HistItemPnt> htp(16000);
    trq.SetText(TR("Calculating histogram"));
    trq.HelpEnable(false);
    long iItem = 0;
    for (long i=0; i < map->iFeatures(); ++i ) {
      if (trq.fAborted())
        return false;
      double rVal = map->rValue(i);
      if (rVal == rUNDEF)
        continue;
	  if ( map->dvrs().fRawAvailable())
 		rVal = map->dvrs().rValue(rVal);
	  HistItemPnt hip(rVal);
      HistItemPnt& hipHash = htp.get(hip);
      if (hipHash.iCount == 0) { // not found
        ++iItem;
        hip.iCount++;
        htp.add(hip);
      }
      else {
        hipHash.iCount++;
      }
    }
    pts->iRecNew(iItem);
//    _iRecs = iItem;

    // prepare for sorting
    arSort.Resize(iItem);
    trq.fText(TR("Prepare for sorting"));
    long n = 0;
    for (int k = 0; k < htp.iTabSize; ++k) {
      if (!(n % 100))
        if (trq.fUpdate(n, iItem))
          return false;
      if (0 != htp.table[k]) 
        for (SLIter<HistItemPnt> iter(htp.table[k]); iter.fValid(); ++iter)
          arSort[n++] = iter().r;
    }     
    trq.fUpdate(iItem, iItem);
    trq.fText(TR("Sort values"));
    try {
			if (iItem > 1)
				::QuickSort(0L, iItem-1, fLessRealVal, SwapRealVal, this);
    }  
    catch (const ErrorObject&) {
      arSort.Resize(0);
      return false;
    }  
    trq.fText(TR("Fill table"));
    for (long m=1; m <= iItem; ++m) {
      if (!(m % 100))
        if (trq.fUpdate(m, iItem)) {
          arSort.Resize(0);
          return false;
        }  
      HistItemPnt hip(arSort[m-1]);
      HistItemPnt& hipHash = htp.get(hip);
      _colValue->PutVal(m, hipHash.r);
      _colFreq->PutVal(m, hipHash.iCount);
    }
    trq.fUpdate(iItem, iItem);
    arSort.Resize(0);

  }
  return true;
}

void TableHistogramPnt::FillColumns()
{
  long i;
  trq.fText(TR("Fill other columns"));
  // reset undefined values
  bool fChanged = false;
  long iMin = iOffset();
  long iMax = iMin + iRecs() - 1;
  iTotalFreq = 0;
  for (i = iMin; i <= iMax; i++) {
    long l = _colFreq->iValue(i);
    if (l < 0)
      _colFreq->PutVal(i, 0L);
    else
      iTotalFreq += l;
  } 
  _colFreq->CalcMinMax();
  if (colFreqCum().fValid()) {
    long iCum = 0;
    for (i = iMin; i <= iMax; i++) {
      iCum += _colFreq->iValue(i);
      colFreqCum()->PutVal(i, iCum);
//      trq.fAborted();
    }
    colFreqCum()->CalcMinMax();
  }  
  if (map->dvrs().fValues()) {
    map->SetMinMax(riMinMax(0));
    map->SetMinMax(rrMinMax(0));
    map->SetPerc1(riMinMax(1));
    map->SetPerc1(rrMinMax(1));
  }
/*  trq.fText("Calc mean and standard deviation");
  rMean = rUNDEF;
  rStd = rUNDEF;
  rPred = rUNDEF;
  iPredCount = iUNDEF;
  rMedian = rrMinMax(50).rLo();
  long iPix;
  if (map->fValues() && iTotalPix > 1) {
    double r = 0;
    if (_colValue.fValid())
      for (long i = iOffset(); i < iOffset()+iRecs()-1; i++) {
        trq.fAborted();
        iPix = colNPix()->iValue(i);
        r += _colValue->rValue(i) * iPix;
        if (iPix > iPredCount) {
          iPredCount = iPix;
          rPred = _colValue->rValue(i);
        }
      }  
    else
      for (long i = iOffset(); i < iOffset()+iRecs()-1; i++) {
        trq.fAborted();
        iPix = colNPix()->iValue(i);
        r += iPix * map->dvrs().iValue(i);
        if (iPix > iPredCount) {
          iPredCount = iPix;
          rPred = map->dvrs().iValue(i);
        }
      }  
    rMean = r / iTotalPix;
    r = 0;
    if (_colValue.fValid())
      for (long i = iOffset(); i < iOffset()+iRecs()-1; i++) {
        r += sqr(rMean - _colValue->rValue(i)) * colNPix()->iValue(i);
        trq.fAborted();
      }  
    else  
      for (long i = iOffset(); i < iOffset()+iRecs()-1; i++) {
        r += sqr(rMean - map->dvrs().iValue(i)) * colNPix()->iValue(i);
        trq.fAborted();
      }  
    rStd = sqrt(r / (iTotalPix - 1));
  }
  else {
    for (long i = iOffset(); i < iOffset()+iRecs()-1; i++) {
      trq.fAborted();
      iPix = colNPix()->iValue(i);
      if (iPix > iPredCount) {
        iPredCount = iPix;
        rPred = i;
      }
    }  
  }*/
}

void TableHistogramPnt::Init()
{
  trq.SetTitle("TableHistogramPnt");
  trq.setHelpItem("ilwisapp\\histogram_algorithm.htm");
  if (pts==0)
    return;
//  Time tim = pts->objtime;
  if (0 != dm()->pdnone()) {
    _colValue = pts->col("Value");
    if (!_colValue.ptr()) {
      _colValue = pts->colNew("Value", map->dvrs());
      _colValue->sDescription = TR("Point value");
    }
    _colValue->SetOwnedByTable(true);
    _colValue->SetReadOnly(true);
  }
  _colFreq = pts->col("NrPnt");
  if (!_colFreq.ptr()) {
    _colFreq = pts->colNew("NrPnt", Domain("count"));
    _colFreq->sDescription = TR("Number of points");
  }
  _colFreq->SetOwnedByTable(true);
  _colFreq->SetReadOnly(true);
  if (map->fValues()) {
    _colFreqCum = pts->col("NrPntCum");
    if (!_colFreqCum.ptr()) {
      _colFreqCum = pts->colNew("NrPntCum", Domain("count"));
    _colFreqCum->sDescription = TR("Cumulative number of points");
    }
    _colFreqCum->SetOwnedByTable(true);
    _colFreqCum->SetReadOnly(true);
  }  
}


RangeInt TableHistogramPnt::riIndexMinMax(double rPerc) const
{
  long iLo = iOffset();
  long iHi = iRecs() + iOffset() - 1;
  RangeInt ri(iLo, iHi);
  if (rPerc == 0) {
    for (long i = iLo; i <= iHi; i++)
      if (colFreq()->iValue(i) > 0) {
        ri.iLo() = i;
        break;
      }  
    for (long i = iHi; i >= iLo; i--)
      if (colFreq()->iValue(i) > 0) {
        ri.iHi() = i;
        break;
      }
    return ri;
  }
  Column cp = colFreqCum();
  double rPrc = rPerc * iTotalFreq / 100;
  for (long i = iLo+1; i <= iHi; i++)
    if (cp->rValue(i) > rPrc) {
      ri.iLo() = i-1;
      break;
    }
  rPrc = (100 - rPerc) * iTotalFreq / 100;
  for (long i = iHi-1; i >= iLo; i++)
    if (cp->rValue(i) < rPrc) {
      ri.iHi() = i+1;
      break;
    }
/*
  try {
    long iPrevDif = cp->rValue(ri.iLo()-1) - rPrc;
    if (ri.iLo() > iOffset()) {
      if (iPrevDif != 0)
        if (abs(iPrevDif) < abs(cp->rValue(ri.iLo()) - rPrc))
          ri.iLo()--;
    }
    if (iPrevDif != 0)
      ri.iLo()++;
    rPrc = (100 - rPerc) * iTotalFreq / 100;
    for (i = iRecs()+iOffset()-1; i >= iOffset(); i--)
      if (cp->rValue(i) <= rPrc) {
        ri.iHi() = i;
        break;
      }
    if (ri.iHi() < iOffset()+iRecs()-1) {
      long iPrevDif = cp->rValue(ri.iHi()+1) - rPrc;
      if (iPrevDif != 0)
        if (abs(iPrevDif) < abs(cp->rValue(ri.iHi()) - rPrc))
          ri.iHi()++;
    }
  }
  catch (const ErrorFloatingPoint&) {
    // for some strange reason floating point errors ocuur in this section
  }
*/
  return ri;
}

RangeInt TableHistogramPnt::riMinMax(double rPerc) const
{
  if (!map->dvrs().fValues())
    return RangeInt();
  if (map->dvrs().fUseReals()) {
    RangeReal rr = rrMinMax(rPerc);
    return RangeInt(longConv(rr.rLo()), longConv(rr.rHi()));
  }  
  RangeInt ri = riIndexMinMax(rPerc);
  if (_colValue.fValid())
    return RangeInt(_colValue->iValue(ri.iLo()),
                    _colValue->iValue(ri.iHi()));
  return RangeInt(map->dvrs().iValue(ri.iLo()),
                  map->dvrs().iValue(ri.iHi()));
//    return ri;
}

RangeReal TableHistogramPnt::rrMinMax(double rPerc) const
{
  if (!map->dvrs().fValues())
    return RangeReal();
  RangeInt ri = riIndexMinMax(rPerc);
  if (_colValue.fValid())
    return RangeReal(_colValue->rValue(ri.iLo()),
                     _colValue->rValue(ri.iHi()));
  return RangeReal(map->dvrs().rValue(ri.iLo()),
                   map->dvrs().rValue(ri.iHi()));
}

String TableHistogramPnt::sExpression() const
{
  return String("TableHistogramPnt(%S)", map->sNameQuoted(true, fnObj.sPath()));
}

String TableHistogramPnt::sType() const
{
  return "HistogramPnt";
}

void TableHistogramPnt::UnFreeze()
{
  if (_colValue.fValid()) {
    pts->RemoveCol(_colValue);
    _colValue = Column();
  }  
  if (_colFreq.fValid()) {
    pts->RemoveCol(_colFreq);
    _colFreq = Column();
  }  
  if (_colFreqCum.fValid()) {
    pts->RemoveCol(_colFreqCum);
    _colFreqCum = Column();
  }  
  TableVirtual::UnFreeze();
}

void TableHistogramPnt::Erase(const FileName& fnMap)
{
  FileName fnHis = fnMap;
  fnHis.sExt = ".hp#"; 
  _unlink(fnHis.sFullName(true).c_str()); // delete data file
  fnHis.sExt = ".hsp";
  // delete ODF of histogram
  if (0 == _unlink(fnHis.sFullName(true).c_str())) 
		AfxGetApp()->GetMainWnd()->PostMessage(ILW_READCATALOG); 
}




