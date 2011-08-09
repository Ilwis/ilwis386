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
// $Log: /ILWIS 3.0/Table/Tblhist.cpp $
 * 
 * 26    5-04-04 11:55 Retsios
 * [bug=6501] Last record in a table was forgotten in calculation of
 * several values (predominant, mean, standard deviation). This was
 * causing erroneous results at least since 1999, but it was only noticed
 * when small maps were used (3x3 pixels).
 * 
 * 25    19-09-01 17:55 Koolhoven
 * in sSummary() prevent to take a negative length for the number of
 * spaces (which causes a crash)
 * 
 * 24    3/15/01 18:46 Retsios
 * bug#4586: Don't store histograms every time they're opened.
 * 
 * 23    5-03-01 12:38 Koolhoven
 * in Erase() use PostMessage() instead of PostThreadMessage() to update
 * the catalog
 * 
 * 22    27-11-00 11:24 Koolhoven
 * readability measures: removed "== true" and replaced "== false" by "!"
 * 
 * 21    12-10-00 8:58a Martin
 * trq in fillcolumns will remove itself and not wait until the histogram
 * is gone
 * 
 * 20    5-10-00 10:42a Martin
 * the colnew of the init function may not cause a store of the table
 * 
 * 19    28-09-00 1:34p Martin
 * protected histogram from using non existing columns
 * 
 * 18    8/15/00 4:42p Wind
 * undid a change related to offsets that i made in february
 * 
 * 17    13-07-00 10:16a Martin
 * protected the Addintional Info against a possible undef
 * 
 * 16    7-03-00 13:52 Wind
 * reformatted summary
 * 
 * 15    28-02-00 12:25 Wind
 * adapted offset in call to PutBufVal
 * 
 * 14    9-02-00 10:54 Wind
 * replaced \n with \r\n in additional info
 * 
 * 13    8-02-00 17:58 Wind
 * no update of catalog in Erase() if file was already erased
 * 
 * 12    14-01-00 17:19 Koolhoven
 * Post reread Catalog as thread message
 * 
 * 11    17-12-99 10:05 Wind
 * adapted to template classes vector and HMap
 * 
 * 10    2-12-99 13:14 Wind
 * added object name to title of tranquilizer
 * 
 * 9     30-11-99 15:47 Koolhoven
 * Added TableHistogramInfo::colNPix()
 * 
 * 8     30-11-99 12:20 Wind
 * solved problems with TableHistogramInfo
 * 
 * 7     17-11-99 14:00 Wind
 * use of tranquilizer
 * buf in Hash function for 8 byte maps
 * 
 * 6     10/01/99 1:37p Wind
 * support for histograms of map in maplist ODF 
 * 
 * 5     9/08/99 10:28a Wind
 * adpated to use of quoted file names in sExpression()
 * 
 * 4     8/20/99 11:35a Wind
 * problems with comments placed by source safe
 * 
 * 3     8/20/99 11:33a Wind
 * IHash() returned negative values
 *
 * 
 * 2     8/18/99 2:00p Wind
 * Changed maximum for Tranquilizer in fCount() in section 'prepare for
 * sorting'
*/
// Revision 1.17  1998/09/16 17:25:32  Wim
// 22beta2
//
// Revision 1.15  1997/09/18 12:20:24  Wim
// Send ILW_READCATALOG message after deleting files in Erase()
//
// Revision 1.14  1997-09-01 15:15:51+02  Wim
// set help topic on in Init()
//
// Revision 1.13  1997-08-25 11:18:14+02  Wim
// Changed some tranquilizers
//
// Revision 1.12  1997-08-18 18:01:16+02  Wim
// Syntax error
//
// Revision 1.11  1997-08-18 17:57:54+02  Wim
// Only return false in fCount() when map is not usable
//
// Revision 1.10  1997-08-18 15:38:39+02  martin
// Removed the freeze from the constructor. Caused unwanted recalculation after an aborted calculation.
//
// Revision 1.9  1997/08/18 10:03:13  martin
// *** empty log message ***
//
// Revision 1.8  1997/08/15 10:42:58  Wim
// Remove colnpixnotundef when no undefs
//
// Revision 1.7  1997-08-14 19:15:34+02  Wim
// Improved readability of tranquilizer statements
//
// Revision 1.6  1997-08-14 19:09:15+02  Wim
// Calculate with raw values when possible
// Count when using real sorting
//
// Revision 1.5  1997-08-14 18:17:03+02  Wim
// If not properly calculated return false in fCount()
//
// Revision 1.4  1997-08-14 14:38:41+02  Wim
// Do not store in a filename which is empty
//
// Revision 1.3  1997-08-12 18:14:45+02  Wim
// Do not try to fill columns when no pts is available
//
/* TableHistogram
   Copyright Ilwis System Development ITC
   april 1995, by Jelle Wind
	Last change:  WK    1 Jul 98   10:02 am
*/

//////// only properly implementef for byte maps

#include "Engine\Table\TBLHIST.H"
#include "Engine\Table\tblstore.h"
#include "Engine\Table\COLSTORE.H"
#include "Engine\Domain\Dmvalue.h"
#include "Engine\Applications\MAPVIRT.H"
#include "Engine\Table\COLLONG.H"
#include "Engine\Table\COLREAL.H"
#include "Engine\Base\DataObjects\valrange.h"
#include "Engine\Base\System\mutex.h"
#include "Headers\Htp\Ilwisapp.htp"
#include "Headers\Err\Ilwisapp.err"
#include "Headers\constant.h"
#include "Headers\Hs\tbl.hs"
#include "Engine\Base\DataObjects\hmap.h"

const char* TableHistogram::sSyntax() { return "TableHistogram(map)"; }

void ThrowHistogramError(const String& sMes, int iNr, const FileName& fnObj)
{
    throw ErrorObject(WhatError(sMes, iNr), fnObj);
}

TableHistogramInfo::TableHistogramInfo()
{
}

TableHistogramInfo::TableHistogramInfo(const Map& mp, bool fIgnZero)
{
  bool fValidMapName = mp->fnObj.fValid();
  FileName filnam;
  if (fValidMapName) {
    filnam = FileName(mp->fnObj, ".his", true);
    if (mp->fnObj.sSectionPostFix.length() > 0) 
      filnam.sFile &= String("__%s", &mp->fnObj.sSectionPostFix[1]);
  }
  else
    filnam = FileName::fnUnique(FileName("$tmphis", ".his", true));
  MutexFileName mut(filnam);
  if (File::fExist(filnam)) {
    // histogram file exists already
    his = Table(filnam);
    if (his->objtime < mp->objtime)
      his->Calc();
  }
  else {
    if (fIgnZero)
      his = Table(filnam, String("TableHistogram(%S,IgnoreZero)", mp->sNameQuoted()));
    else
      his = Table(filnam, String("TableHistogram(%S)", mp->sNameQuoted()));
    if (!his->fCalculated())
      his->Calc();
    if (!fValidMapName)
      his->fErase = true;
  }
  his->OpenTableVirtual();
  if (!his->fCalculated())
    his->Calc();
}

void TableHistogramInfo::operator=(const TableHistogramInfo& inf)
{
  his = inf.his;
}

bool TableHistogramInfo::fValid() const
{
  if (!his.fValid())
    return false;
  return 0 != his->ptv;
}

RangeInt TableHistogramInfo::riMinMax(double rPerc) const
{
  if (!fValid())
    return RangeInt();
  TableHistogram* hist = static_cast<TableHistogram*>(his->ptv);
  return hist->riMinMax(rPerc);
}

RangeReal TableHistogramInfo::rrMinMax(double rPerc) const
{
  if (!fValid())
    return RangeReal();
  TableHistogram* hist = static_cast<TableHistogram*>(his->ptv);
  return hist->rrMinMax(rPerc);
}

Column TableHistogramInfo::colNPix() const
{
  if (!fValid())
    return Column();
  TableHistogram* hist = static_cast<TableHistogram*>(his->ptv);
  return hist->colNPix();
}

Column TableHistogramInfo::colNPixCum() const
{
  if (!fValid())
    return Column();
  TableHistogram* hist = static_cast<TableHistogram*>(his->ptv);
  return hist->colNPixCum();
}

Column TableHistogramInfo::colNPixCumPct() const
{
  if (!fValid())
    return Column();
  TableHistogram* hist = static_cast<TableHistogram*>(his->ptv);
  return hist->colNPixCumPct();
}

Column TableHistogramInfo::colValue() const
{
  if (!fValid())
    return Column();
  TableHistogram* hist = static_cast<TableHistogram*>(his->ptv);
  return hist->colValue();
}

TableHistogram::TableHistogram(const FileName& fn, TablePtr& p)
: TableVirtual(fn, p)
{
  FileName fnMap;
  ReadElement("TableHistogram", "Map", fnMap);
  try {
    map = Map(fnMap);
  }
  catch (const ErrorObject& err) {
    map = Map();
    throw err;
  }
  objdep.Add(map.ptr());
  _fIgnZero = false;
  ReadElement("TableHistogram", "IgnoreZero", _fIgnZero);
  fNeedFreeze = true;
  if (0 == pts)
    return;
  Init();
  FillColumns();
}

IlwisObjectPtr * createTableHistogram(const FileName& fn, IlwisObjectPtr& ptr, const String& sExpr, vector<void *> parms ) {
	if ( sExpr == "")
		return (IlwisObjectPtr *)new TableHistogram(fn, (TablePtr &)ptr);

	Array<String> as(1);
    if (!IlwisObjectPtr::fParseParm(sExpr, as))
      throw ErrorExpression(fn.sFullName(), sExpr);
    FileName filnam(as[0], ".his", true);
    if (File::fExist(filnam))
      // histogram file exists already
      return (IlwisObjectPtr *)new TableHistogram(filnam, (TablePtr&)ptr);
    else // create new histogram from map
      return (IlwisObjectPtr *)new TableHistogram((TablePtr&)ptr, fn, Map(String(as[0])) );
}

TableHistogram::TableHistogram(TablePtr& p, const FileName& fnTbl, Map& mp, bool fIgnZero)
: TableVirtual(fnTbl, p, true)
{
  ptr.SetAdditionalInfoFlag(true);
  _fIgnZero = fIgnZero;
  if (0 != mp->dm()->pdcol())
    ThrowHistogramError(String(TR("No color domain allowed %S").c_str(), mp->sName(true, fnObj.sPath())), errTableHistogram+1, fnObj);
  if (mp->dm()->pdi() || mp->dm()->pdsrt() || mp->dm()->pdbit() || mp->dm()->pdp()) {
    ptr.SetDomain(mp->dm());
  }  
  else
    ptr.SetDomain("none");
  map = mp;
  objdep.Add(map.ptr());
  if (!fnObj.fValid())
    objtime = objdep.tmNewest();
  fNeedFreeze = true;
}

void TableHistogram::Store()
{
  if (fnObj.sFile.length() == 0)  // empty file name
    return;
  TableVirtual::Store();
  WriteElement("TableVirtual", "Type", "TableHistogram");
  WriteElement("TableHistogram", "Map", map);
  WriteElement("TableHistogram", "IgnoreZero", _fIgnZero);
}

bool TableHistogram::fFreezing()
{
  if (map->dm()->pdi() || map->dm()->pdsrt() || map->dm()->pdbit() || map->dm()->pdp()) {
    ptr.SetDomain(map->dm());
  }  
  else
    ptr.SetDomain("none");
  Init();
  if (!fCount())
    return false;
  FillColumns();
  ptr.SetAdditionalInfo(sSummary());
  return true;
}

#define IntOffset (-1L << 15)

// Hash table for raw values (first) and frequency (second)
typedef HashFun<long> hf;
typedef HMap<long,long,hf> htab;
typedef htab::value_type hitem; // first: raw value; second: frequency
typedef Array<hitem> ahitem;

// Hash table for real values (first) and frequency (second)
typedef HashFun<double> hfr;
typedef HMap<double,long,hfr> htabr;
typedef htabr::value_type hitemr; // first: real value; second: frequency
typedef Array<hitemr> ahitemr;

bool TableHistogram::fCount()
{
  iPredCount = iUNDEF;
  map->MakeUsable();
  if (map->fDependent() && !map->fCalculated() && !map->fDefOnlyPossible())
    return false;
  long i, l, c;
  long iMapLines = map->iLines();
  long iMapCols = map->iCols();
  map->KeepOpen(true);
  if (0 == dm()->pdnone() && iRecs()<16000) {
    long iMin = iOffset();
    long iMax = iMin + iRecs() - 1;
    LongBuf buf(iRecs() + iMin);
    LongBuf line(iMapCols);
    for (i = iMin; i <= iMax; i++)
      buf[i] = 0;
    trq.SetText(TR("Calculating histogram"));
    trq.HelpEnable(false);
    for (l = 0; l < iMapLines; l++) {
      if (trq.fUpdate(l, iMapLines))
        return false;
  // GetLine can set error flag and is thus not a constant function
      map->GetLineRaw(l, line);
      for (c = 0; c < iMapCols; c++) {
        long b = line[c];
        if (_fIgnZero && b == 0)
          continue;
        if ((b >= iMin) && (b <= iMax))
          buf[b]++;
      }
    }
//    colNPix()->PutBufVal(buf, iOffset());
    colNPix()->PutBufVal(buf);
    trq.HelpEnable(true);
  }
  else { // use hashtable

    if (map->st() <= stLONG) {  // use raw values
      ahitem ahitSort;
      htab htl(hf(16000));
      LongBuf line(iMapCols);
      trq.SetText(TR("Calculating histogram"));
      trq.HelpEnable(false);
      long iItem = 0;
      for (l = 0; l < iMapLines; l++) {
        if (trq.fUpdate(l, iMapLines))
          return false;
    // GetLine can set error flag and is thus not a constant function
        map->GetLineRaw(l, line);
        for (c = 0; c < iMapCols; c++) {
          long raw = line[c];
          if (raw != iUNDEF) {
            htab::iterator iter = htl.Find(raw);
            if (iter == htl.end()) { // not yet in hash table,
              htl.Insert(hitem(raw, 1)); //insert with frequency 1
              iItem++;
            }
            else 
              (*iter).second++;  // add one to frequency 
          }
        }
      }
      trq.fUpdate(iMapLines, iMapLines);
      trq.SetText(TR("Prepare for sorting"));

      ptr.DeleteRec(iOffset(),iRecs());
      pts->iRecNew(iItem); 

      // prepare for sorting
      ahitSort.Resize(iItem);
      long n = 0;
      htab::iterator iter = htl.begin();

      for (int k = 0; iter != htl.end(); iter++, ++k)  {
        if (0 == k % 100)
          if (trq.fUpdate(k, htl.iSize())) 
            return false;
        ahitSort[n++] = *iter;

      }

      trq.SetText(TR("Sort values"));
      sort(ahitSort.begin(), ahitSort.end()); 

      trq.SetText(TR("Fill table"));
      DomainValueRangeStruct dvs = map->dvrs();
      DomainSort *pdsrt = dm()->pdsrt();    
      for (long m=1; m <= iItem; ++m) {
        if (0 == m % 100)
          if (trq.fUpdate(m, iItem)) {
            ahitSort.Resize(0);
            return false;
          }
        hitem hit = ahitSort[m-1];
        long iVal=pdsrt ? hit.first : m;
        if (_colValue.fValid())   
            _colValue->PutVal(iVal, dvs.rValue(hit.first));
        _colNPix->PutVal(iVal, hit.second);
      }
      trq.fUpdate(iItem, iItem);
      ahitSort.Resize(0);
    }
    else { // use real values (slow)
      ahitemr ahitrSort;
      htabr htr(hfr(16000));
      RealBuf line(iMapCols);
      trq.SetText(TR("Calculating histogram"));
      trq.HelpEnable(false);
      long iItem = 0;
      for (l = 0; l < iMapLines; l++) {
        if (trq.fUpdate(l, iMapLines))
          return false;
    // GetLine can set error flag and is thus not a constant function
        map->GetLineVal(l, line);
        for (c = 0; c < iMapCols; c++) {
          double r = line[c];
          if (r != rUNDEF) {
            htabr::iterator iter = htr.Find(r);
            if (iter == htr.end()) { // not yet in hash table,
              htr.Insert(hitemr(r, 1)); //insert with frequency 1
              iItem++;
            }
            else 
              (*iter).second++;  // add one to frequency 
          }
        }
      }
      trq.fUpdate(iMapLines, iMapLines);
      trq.SetText(TR("Prepare for sorting"));

      ptr.DeleteRec(iOffset(),iRecs());
      pts->iRecNew(iItem);

      // prepare for sorting
      ahitrSort.Resize(iItem);
      long n = 0;
      htabr::iterator iter = htr.begin();
      for (int k = 0; iter != htr.end(); iter++, ++k)  {
        if (0 == k % 100)
          if (trq.fUpdate(k, htr.iSize())) 
            return false;
        ahitrSort[n++] = *iter;
      }

      trq.SetText(TR("Sort values"));
      sort(ahitrSort.begin(), ahitrSort.end()); 

      trq.SetText(TR("Fill table"));
      for (long m=1; m <= iItem; ++m) {
        if (0 == m % 100)
          if (trq.fUpdate(m, iItem)) {
            ahitrSort.Resize(0);
            return false;
          }
        hitemr hitr = ahitrSort[m-1];
        _colValue->PutVal(m, hitr.first);
        _colNPix->PutVal(m, hitr.second);
      }
      trq.fUpdate(iItem, iItem);
      ahitrSort.Resize(0);
    }
  }
  map->KeepOpen(false);
  return true;
}

void TableHistogram::FillColumns()
{
  long i;
  trq.fText(TR("Fill other columns"));
  // reset undefined values
  long iMin = iOffset();
  long iMax = iMin + iRecs() - 1;
  iTotalPix = 0;
  for (i = iMin; i <= iMax; i++) {
    long l = _colNPix->iValue(i);
    if (l < 0)
      _colNPix->PutVal(i, 0L);
    else
      iTotalPix += l;
  } 
  _colNPix->CalcMinMax();
  if (colNPixCum().fValid()) {
    long iCum = 0;
    for (i = iMin; i <= iMax; i++) {
      iCum += _colNPix->iValue(i);
      colNPixCum()->PutVal(i, iCum);
    }
    colNPixCum()->CalcMinMax();
  }  
  long iMapSize = map->rcSize().Row * map->rcSize().Col;
  if (colNPixCumPct().fValid() && iTotalPix > 0) {
    double r = iMapSize / 100.0;
    for (i = iMin; i <= iMax; i++) {
      double rVal = (double)colNPixCum()->iValue(i)/r;
      if (rVal > 100)
        rVal = 100;
      colNPixCumPct()->PutVal(i, rVal);
    } 
    colNPixCumPct()->CalcMinMax();
  }  
  if (colNPixPct().fValid() && iTotalPix > 0) {
    double r = iMapSize / 100.0;
    for (i = iMin; i <= iMax; i++) {
      double rr = colNPix()->iValue(i)/r;
      colNPixPct()->PutVal(i, rr);
    } 
    colNPixPct()->CalcMinMax();
  }  
  if (iMapSize != iTotalPix) { // there ar undefs
    if (_colNPixPctNotUndef.fValid()) {
      double r = iTotalPix / 100.0;
      for (i = iMin; i <= iMax; i++) {
        double rr = colNPix()->iValue(i)/r;
        _colNPixPctNotUndef->PutVal(i, rr);
      }  
      _colNPixPctNotUndef->CalcMinMax();
    }
  }
  else if (0 != dm()->pdi()) { // make column with ignore zeroes
    if (_colNPixPctNotUndef.fValid()) {
      double r = (iTotalPix - colNPix()->iValue(iMin)) / 100.0;
      _colNPixPctNotUndef->PutVal(iMin, 0L);
      for (i = iMin+1; i <= iMax; i++) {
        double rr = colNPix()->iValue(i)/r;
        _colNPixPctNotUndef->PutVal(i, rr);
      }  
      _colNPixPctNotUndef->CalcMinMax();
    }
  }
  else if (_colNPixPctNotUndef.fValid()) {
    ptr.RemoveCol(_colNPixPctNotUndef);
    _colNPixPctNotUndef = Column();
  }
  if (_colArea.fValid()) {
    double rPixSize = map->gr()->rPixSize();
    if (rPixSize > 0) {
      rPixSize *= rPixSize;
      for (long i=iMin; i <= iMax; ++i)
        _colArea->PutVal(i, rPixSize*colNPix()->iValue(i));
    }
  }  
  if (map->dvrs().fValues()) {
    map->SetMinMax(riMinMax(0));
    map->SetMinMax(rrMinMax(0));
    map->SetPerc1(riMinMax(1));
    map->SetPerc1(rrMinMax(1));
  }
  ptr.fChanged = false;
  // Store(); // Stored histograms everytime they were opened (not necessary)
  
  trq.fText(TR("Calculate mean and standard deviation"));
  rMean = rUNDEF;
  rStd = rUNDEF;
  rPred = rUNDEF;
  iPredCount = iUNDEF;
  rMedian = rrMinMax(50).rLo();
  long iPix;
  if (map->fValues() && iTotalPix > 1) {
    double r = 0;
    if (_colValue.fValid())
      for (long i = iOffset(); i < iOffset()+iRecs(); i++) {
        iPix = colNPix()->iValue(i);
        r += _colValue->rValue(i) * iPix;
        if (iPix > iPredCount) {
          iPredCount = iPix;
          rPred = _colValue->rValue(i);
        }
      }  
    else
      for (long i = iOffset(); i < iOffset()+iRecs(); i++) {
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
      for (long i = iOffset(); i < iOffset()+iRecs(); i++) {
		double rV = rMean - _colValue->rValue(i);
		rV = rV * rV * colNPix()->iValue(i);
        r += rV;
      }  
    else  
      for (long i = iOffset(); i < iOffset()+iRecs(); i++) {
		  double rV = rMean - map->dvrs().iValue(i) ;
		  rV = rV * rV * colNPix()->iValue(i);
        r += rV;
      }  
    rStd = sqrt(r / (iTotalPix - 1));
  }
  else {
    for (long i = iOffset(); i < iOffset()+iRecs(); i++) {
      iPix = colNPix()->iValue(i);
      if (iPix > iPredCount) {
        iPredCount = iPix;
        rPred = i;
      }
    }  
  }
	trq.Stop(); // end trq else it remains visible, even when showing the histogram
}

void TableHistogram::Init()
{
	ptr.DoNotStore(true);
	
  String sTitle("%S - %S", TR("TableHistogram"), sName(true));
  trq.SetTitle(sTitle);
  trq.setHelpItem("ilwisapp\\histogram_algorithm.htm");
  if (0 == pts)
    return;
  if (0 != dm()->pdnone()) {
    _colValue = pts->col("value");
    if (!_colValue.ptr()) {
      _colValue = pts->colNew("value", map->dvrs());
      _colValue->sDescription = TR("Pixel value");
    }
    _colValue->SetOwnedByTable(true);
    _colValue->SetReadOnly(true);
  }
  _colNPix = pts->col("npix");
  long iMaxCount = map->iLines() * map->iCols();
  if (!_colNPix.ptr()) {
    _colNPix = pts->colNew("npix", Domain("count"), ValueRange(0, iMaxCount));
    _colNPix->sDescription = TR("Number of pixels");
    for (long i=iOffset(); i <= iOffset()+iRecs()-1; ++i)
       _colNPix->PutVal(i, 0L);
  }
  _colNPix->SetOwnedByTable(true);
  _colNPix->SetReadOnly(true);
  _colNPixPct = pts->col("npixpct");
  if (!_colNPixPct.ptr()) {
    _colNPixPct = pts->colNew("npixpct", Domain("perc"));
    _colNPixPct->sDescription = TR("Percentage of pixels");
  }
  _colNPixPct->SetOwnedByTable(true);
  _colNPixPct->SetReadOnly(true);
  String sNotUndef = "pctnotund";
  String s = TR("not undef");
  if (_fIgnZero || (0 != dm()->pdi())) {
    sNotUndef = "pctnotzero";
    s = TR("not zero");
  }
  _colNPixPctNotUndef = pts->col(sNotUndef);
  if (!_colNPixPctNotUndef.ptr()) {
    _colNPixPctNotUndef = pts->colNew(sNotUndef, Domain("perc"));
    _colNPixPctNotUndef->sDescription = String(TR("Percentage of %S pixels").c_str(), s);
  }
  _colNPixPctNotUndef->SetOwnedByTable(true);
  _colNPixPctNotUndef->SetReadOnly(true);
  if (map->fValues()) {
    _colNPixCum = pts->col("npixcum");
    if (!_colNPixCum.ptr()) {
      _colNPixCum = pts->colNew("npixcum", Domain("count"), ValueRange(0, iMaxCount));
      _colNPixCum->sDescription = TR("Cumulative number of pixels");
    }
    _colNPixCum->SetOwnedByTable(true);
    _colNPixCum->SetReadOnly(true);
    _colNPixCumPct = pts->col("npcumpct");
    if (!_colNPixCumPct.ptr()) {
      _colNPixCumPct = pts->colNew("npcumpct", Domain("perc"));
      _colNPixCumPct->sDescription = TR("Cumulative percentage of pixels");
    }
    _colNPixCumPct->SetOwnedByTable(true);
    _colNPixCumPct->SetReadOnly(true);
  }  
  double rPixSize = map->gr()->rPixSize();
  if (rPixSize > 0) {
    _colArea = pts->col("Area");
    bool fOldFormat = false;
    if (_colArea.fValid()) {
      // check if area is dependent column as in 2.01 and previous versions
      if (_colArea->fDependent()) {
        pts->RemoveCol(_colArea);
        _colArea = Column();
        fOldFormat = true;
      }
    }
    if (!_colArea.fValid()) {
      rPixSize *= rPixSize;
      double rStep = 1;
      if (fmod(rPixSize,1) > 1e-6)
        rStep = 0.1;
      ValueRange vrr(0, iMaxCount*rPixSize, rStep);
      vrr->AdjustRangeToStep();
      _colArea = pts->colNew("Area", Domain("value"), vrr);
      _colArea->sDescription = TR("Area for pixel value");
      if (fOldFormat) {
        try {
          for (long i=iOffset(); i <= iOffset()+iRecs()-1; ++i)
            _colArea->PutVal(i, rPixSize*colNPix()->iValue(i));
          Store();
        }
        catch (...) {
        }  
      }    
    }  
    _colArea->SetOwnedByTable(true);
    _colArea->SetReadOnly(true);
  }
	ptr.DoNotStore(false);
}


RangeInt TableHistogram::riIndexMinMax(double rPerc) const
{
  RangeInt ri;
	if (!_colNPix.fValid() || !_colNPixCum.fValid())
		return RangeInt(iUNDEF, iUNDEF);
	
  ri.iLo() = iOffset();
  ri.iHi() = iRecs() + iOffset() - 1;
  if (rPerc == 0) {
    for (long i = iOffset(); i < iRecs()+iOffset(); i++)
      if (colNPix()->iValue(i) > 0) {
        ri.iLo() = i;
        break;
      }  
    for (long i = iOffset()+iRecs()-1; i >= iOffset(); i--)
      if (colNPix()->iValue(i) > 0) {
        ri.iHi() = i;
        break;
      }
    return ri;
  }
  Column cp = colNPixCum();
  bool fIgnZero = 0 != dm()->pdi();
  long iZero = 0;
  long iOff = iOffset();
  if (fIgnZero) {
    iZero = colNPix()->iValue(0);
    iOff = 1;
  }
  long iPerc = rPerc * (iTotalPix - iZero) / 100;
  for (long i = iOff; i < iRecs()+iOffset(); i++)
    if ((cp->iValue(i) - iZero) > iPerc) {
      ri.iLo() = i;
      break;
    }
  if (ri.iLo() > iOff) {
    long iPrevDif = cp->iValue(ri.iLo()-1) - iZero - iPerc;
    long iDiv = cp->iValue(ri.iLo()) - iZero - iPerc;
    if (iPrevDif != 0)
      if (abs(iPrevDif) < abs(cp->iValue(ri.iLo()) - iZero - iPerc))
        ri.iLo()--;
  }
  iPerc = (100 - rPerc) * (iTotalPix - iZero) / 100;
  for (long i = iRecs()+iOffset()-1; i >= iOff; i--)
    if (cp->iValue(i) - iZero <= iPerc) {
      ri.iHi() = i;
      break;
    }
  if (ri.iHi() < iOffset()+iRecs()-1) {
    long iPrevDif = cp->iValue(ri.iHi()+1) - iZero - iPerc;
    if (iPrevDif != 0)
      if (abs(iPrevDif) < abs(cp->iValue(ri.iHi()) - iZero - iPerc))
        ri.iHi()++;
  }    
  return ri;
}

RangeInt TableHistogram::riMinMax(double rPerc) const
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
}

RangeReal TableHistogram::rrMinMax(double rPerc) const
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

String TableHistogram::sExpression() const
{
  if (_fIgnZero)
    return String("TableHistogram(%S,IgnoreZero)", map->sNameQuoted(true, fnObj.sPath()));
  return String("TableHistogram(%S)", map->sNameQuoted(true, fnObj.sPath()));
}

String TableHistogram::sSummary() const
{
  long iMapSize = map->rcSize().Row * map->rcSize().Col;
  String s;
  if (iTotalPix == 0) {
      s &= String(TR("Number of Undef pixels = %li (100%%)").c_str(),iMapSize);
    return s;
  }
  if (!map->fValues()) {
    s = String("%S=%S (%li)\r\n", TR("Pred"), map->dvrs().sValueByRaw(longConv(rPred)), iPredCount);
    if (iMapSize != iTotalPix)
      s &= String(TR("Number of Undef pixels = %li (%.2f%%)").c_str(),iMapSize - iTotalPix, (iMapSize - iTotalPix) * 100.0 / iMapSize);
    return s;
  }  

	String sSpaces;
	if ( rMean != rUNDEF)
	{
		String sMean("%S=%.2lf", TR("Mean"), rMean); 
    int iLen = 20 - sMean.length();
    if (iLen < 0)
      iLen = 0;
		sSpaces = String(' ', iLen);
		s = String("%S %S %S=%.2f\r\n", sMean, sSpaces, TR("Std.Dev"), rStd);
	}
  String sMedian("%S=%S", TR("Median"),map->dvrs().sValue(rMedian,0));
  int iLen = 20 - sMedian.length();
  if (iLen < 0)
    iLen = 0;
  sSpaces = String(' ', iLen);
  s &= String("%S %S %S=%S (%li)\r\n", sMedian, sSpaces, TR("Pred"), map->dvrs().sValue(rPred,0), max(0L, iPredCount));
  if (iMapSize != iTotalPix) 
    s &= String("%S=%li (%.2f%%)\r\n", TR("Undef count"), iMapSize - iTotalPix, (iMapSize - iTotalPix) * 100.0 / iMapSize);
  RangeReal rr = rrMinMax(0);
  s &= String("0.0%% int= %S:%S   ", map->dvrs().sValue(rr.rLo()), map->dvrs().sValue(rr.rHi()));
  rr = rrMinMax(0.5);
  s &= String(" 0.5%% int= %S:%S\r\n", map->dvrs().sValue(rr.rLo()), map->dvrs().sValue(rr.rHi()));
  rr = rrMinMax(1);
  s &= String("1.0%% int= %S:%S   ", map->dvrs().sValue(rr.rLo()), map->dvrs().sValue(rr.rHi()));
  rr = rrMinMax(2);
  s &= String(" 2.0%% int= %S:%S\r\n", map->dvrs().sValue(rr.rLo()), map->dvrs().sValue(rr.rHi()));
  rr = rrMinMax(5.0);
  s &= String("5.0%% int= %S:%S   ", map->dvrs().sValue(rr.rLo()), map->dvrs().sValue(rr.rHi()));
  rr = rrMinMax(10.0);
  s &= String("10.0%% int= %S:%S", map->dvrs().sValue(rr.rLo()), map->dvrs().sValue(rr.rHi()));
  return s;
}

String TableHistogram::sType() const
{
  return "Histogram";
}

void TableHistogram::UnFreeze()
{
  if (_colValue.fValid()) {
    pts->RemoveCol(_colValue);
    _colValue = Column();
  }  
  if (_colNPix.fValid()) {
    pts->RemoveCol(_colNPix);
    _colNPix = Column();
  }  
  if (_colNPixPct.fValid()) {
    pts->RemoveCol(_colNPixPct);
    _colNPixPct = Column();
  }  
  if (_colNPixPctNotUndef.fValid()) {
    pts->RemoveCol(_colNPixPctNotUndef);
    _colNPixPctNotUndef = Column();
  }  
  if (_colNPixCum.fValid()) {
    pts->RemoveCol(_colNPixCum);
    _colNPixCum = Column();
  }  
  if (_colNPixCumPct.fValid()) {
    pts->RemoveCol(_colNPixCumPct);
    _colNPixCumPct = Column();
  }  
  if (_colArea.fValid()) {
    pts->RemoveCol(_colArea);
    _colArea = Column();
  }
  TableVirtual::UnFreeze();
}

void TableHistogram::Erase(const FileName& fnMap)
{
  FileName fnHis = fnMap;
  fnHis.sExt = ".hi#"; 
  _unlink(fnHis.sFullName(true).c_str()); // delete data file
  fnHis.sExt = ".his";
   // delete object def file
  if (0 == _unlink(fnHis.sFullName(true).c_str()))
		AfxGetApp()->GetMainWnd()->PostMessage(ILW_READCATALOG); 
}




