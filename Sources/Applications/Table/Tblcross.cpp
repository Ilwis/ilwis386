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
/* TableCross
   Copyright Ilwis System Development ITC
   april 1995, by Jelle Wind
	Last change:  WK    8 Oct 98    5:23 pm
*/
#define TBLCROSS_C

#include "Applications\Table\Tblcross.h"
#include "Engine\Table\tblstore.h"
#include "Engine\Table\COLSTORE.H"
#include "Engine\Applications\COLVIRT.H"
#include "Engine\Applications\MAPVIRT.H"
#include "Applications\Raster\MAPCROSS.H"
#include "Engine\Domain\dmsort.h"
#include "Engine\Domain\Dmvalue.h"
#include "Engine\Base\DataObjects\valrange.h"
#include "Headers\Htp\Ilwisapp.htp"
#include "Headers\Err\Ilwisapp.err"
#include "Headers\Hs\tbl.hs"

const char* TableCross::sSyntax()
{
  return "TableCross(map1,map2[,IgnoreUndefs|IgnoreUndef1|IgnoreUndef2])\n"
  "TableCross(map1,map2,mapcross[,gnoreUndefs|IgnoreUndef1|IgnoreUndef2])\n";
}

IlwisObjectPtr * createTableCross(const FileName& fn, IlwisObjectPtr& ptr, const String& sExpr, vector<void *> parms ) {
	if ( sExpr != "")
		return (IlwisObjectPtr *)TableCross::create(fn, (TablePtr &)ptr, sExpr);
	else
		return (IlwisObjectPtr *)new TableCross(fn, (TablePtr &)ptr);
}

TableCross* TableCross::create(const FileName& fn, TablePtr& p, const String& sExpr)
{
  Array<String> as;
  int iParms = IlwisObjectPtr::iParseParm(sExpr, as);
  if ((iParms < 2) || (iParms > 4))
    throw ErrorExpression(sExpr, sSyntax());
  Map map1(as[0], fn.sPath());
  Map map2(as[1], fn.sPath());
  bool fIncompGeoRef = false;
  if (map1->gr()->fGeoRefNone() && map2->gr()->fGeoRefNone())
    fIncompGeoRef = map1->rcSize() != map2->rcSize();
  else
    fIncompGeoRef = map1->gr() != map2->gr();
  if (fIncompGeoRef)
    throw ErrorIncompatibleGeorefs(map1->gr()->sName(true, fn.sPath()),
                          map2->gr()->sName(true, fn.sPath()), fn, errMapCross+1);
  bool fIgnore1 = false;
  bool fIgnore2 = false;
  if (fCIStrEqual(as[iParms-1] , "IgnoreUndefs")) {
    fIgnore1 = true;
    fIgnore2 = true;
    iParms -= 1;
  }
  else if (fCIStrEqual(as[iParms-1] , "IgnoreUndef1")) {
    fIgnore1 = true;
    iParms -= 1;
  }
  else if (fCIStrEqual(as[iParms-1] , "IgnoreUndef2")) {
    fIgnore2 = true;
    iParms -= 1;
  }
  if (iParms == 3)
    return new TableCross(fn, p, map1, map2, as[2], fIgnore1, fIgnore2);
  return new TableCross(fn, p, map1, map2, String(), fIgnore1, fIgnore2);
}

TableCross::TableCross(const FileName& fn, TablePtr& p)
: TableVirtual(fn, p),
  fIgnoreUndef1(false), fIgnoreUndef2(false)
{
  FileName fnMap;
  ReadElement("TableCross", "Map1", map1);
  ReadElement("TableCross", "Map2", map2);
  ReadElement("TableCross", "CrossMap", sMapCross);
  ReadElement("TableCross", "IgnoreUndefs", fIgnoreUndef1);
  fIgnoreUndef2 = fIgnoreUndef1;
  ReadElement("TableCross", "IgnoreUndef1", fIgnoreUndef1);
  ReadElement("TableCross", "IgnoreUndef2", fIgnoreUndef2);
  // read ignore undefs
  objdep.Add(map1.ptr());
  objdep.Add(map2.ptr());
  fNeedFreeze = true;
  Init();
  sFreezeTitle = "TableCross";
  htpFreeze = "ilwisapp\\cross_algorithm.htm";
}

TableCross::TableCross(const FileName& fn, TablePtr& p, const Map& mp1, const Map& mp2,
                       const String& sMapCr, bool fIgnore1, bool fIgnore2)
: TableVirtual(fn, p, true), //FileName(fn, ".tb#", true), Domain("none")),
  map1(mp1), map2(mp2), sMapCross(sMapCr),//, sTblAggr(sTblAgg)
  fIgnoreUndef1(fIgnore1), fIgnoreUndef2(fIgnore2)
{
  objdep.Add(map1.ptr());
  objdep.Add(map2.ptr());
  if (!fnObj.fValid())
    objtime = objdep.tmNewest();
  fNeedFreeze = true;
  sFreezeTitle = "TableCross";
  htpFreeze = "ilwisapp\\cross_algorithm.htm";
  FileName fnDom = FileName::fnUnique(FileName(fnObj, ".dom"));
  ptr.SetDomain(Domain(fnDom, 0, dmtID, "CrossNr")); 
  if (sMapCross.length() > 0) {
    FileName fnCrMap(sMapCross, fnObj);
    fnCrMap.sExt = ".mpr";
    bool fAlreadyExists = false;
    if (fnCrMap.fExist()) {
      String sType;
      ObjectInfo::ReadElement("MapFromMap", "Type", fnCrMap, sType);
      fAlreadyExists = sType == "MapCross";
    }
    if (!fAlreadyExists) {
      ptr.Store(); // map cross wants to open tablecross
      Store();
      Map mpCross(fnCrMap, String("MapCross(%S,%S,%S)",
                map1->sNameQuoted(true, fnObj.sPath()), map2->sNameQuoted(true, fnObj.sPath()),
                ptr.sNameQuoted(true, fnObj.sPath())));
    }
  }
}

TableCross::~TableCross()
{
}

void TableCross::Store()
{
  TableVirtual::Store();
  WriteElement("TableVirtual", "Type", "TableCross");
  WriteElement("TableCross", "Map1", map1);
  WriteElement("TableCross", "Map2", map2);
  WriteElement("TableCross", "CrossMap", sMapCross);
  WriteElement("TableCross", "IgnoreUndef1", fIgnoreUndef1);
  WriteElement("TableCross", "IgnoreUndef2", fIgnoreUndef2);
}


static int iHash(const CrossCombLongLong& ccll)
{
  return abs(abs(ccll.i1) * 59781 + abs(ccll.i2) * 80091 ) % 160000;
}

static int iHash(const CrossCombLongReal& cclr)
{
  return abs(longConv(abs(cclr.i1) * 59781 + fabs(cclr.r2) * 80091)) % 160000;
}

static  int iHash(const CrossCombRealLong& ccrl)
{
  return abs(longConv(fabs(ccrl.r1) * 59781 + abs(ccrl.i2) * 80091)) % 160000;
}

static int iHash(const CrossCombRealReal& ccrr)
{
  return abs(longConv(fabs(ccrr.r1) * 59781 + fabs(ccrr.r2) * 80091)) % 160000;
}

class CompareLongLong 
{
public:
  CompareLongLong(TableCross* tc) { _tc = tc; }
  bool operator()(const long& i1, const long& i2)
  {
    long iRaw1 = _tc->colMap1->iRaw(i1);
    if (iRaw1 == iUNDEF)
      return 0;
    long iRaw2 = _tc->colMap1->iRaw(i2);
    if (iRaw2 == iUNDEF)
      return 1;
    if (iRaw1 < iRaw2)
      return 1;
    if (iRaw1 > iRaw2) 
      return 0;
    iRaw1 = _tc->colMap2->iRaw(i1);
    if (iRaw1 == iUNDEF)
      return 0;
    iRaw2 = _tc->colMap2->iRaw(i2);
    if (iRaw2 == iUNDEF)
      return 1;
    if (iRaw1 < iRaw2)
      return 1;
    return 0;
  }
  TableCross* _tc;
};

class CompareLongReal 
{
public:
  CompareLongReal(TableCross* tc) { _tc = tc; }
  bool operator()(const long& i1, const long& i2)
  {
    long iRaw1 = _tc->colMap1->iRaw(i1);
    if (iRaw1 == iUNDEF)
      return 0;
    long iRaw2 = _tc->colMap1->iRaw(i2);
    if (iRaw2 == iUNDEF)
      return 1;
    if (iRaw1 < iRaw2)
      return 1;
    if (iRaw1 > iRaw2) 
      return 0;
    double r1 = _tc->colMap2->rValue(i1);
    if (r1 == rUNDEF)
      return 0;
    double r2 = _tc->colMap2->rValue(i2);
    if (r2 == rUNDEF)
      return 1;
    if (r1 < r2)
      return 1;
    return 0;
  }
  TableCross* _tc;
};

class CompareRealLong 
{
public:
  CompareRealLong(TableCross* tc) { _tc = tc; }
  bool operator()(const long& i1, const long& i2)
  {
    double rVal1 = _tc->colMap1->rValue(i1);
    if (rVal1 == rUNDEF)
      return 0;
    double rVal2 = _tc->colMap1->rValue(i2);
    if (rVal2 == rUNDEF)
      return 1;
    if (rVal1 < rVal2)
      return 1;
    if (rVal1 > rVal2) 
      return 0;
    long iRaw1 = _tc->colMap2->iRaw(i1);
    if (iRaw1 == iUNDEF)
      return 0;
    long iRaw2 = _tc->colMap2->iRaw(i2);
    if (iRaw2 == iUNDEF)
      return 1;
    if (iRaw1 < iRaw2)
      return 1;
    return 0;
  }
  TableCross* _tc;
};

class CompareRealReal 
{
public:
  CompareRealReal(TableCross* tc) { _tc = tc; }
  bool operator()(const long& i1, const long& i2)
  {
    double rVal1 = _tc->colMap1->rValue(i1);
    if (rVal1 == rUNDEF)
      return 0;
    double rVal2 = _tc->colMap1->rValue(i2);
    if (rVal2 == rUNDEF)
      return 1;
    if (rVal1 < rVal2)
      return 1;
    if (rVal1 > rVal2) 
      return 0;
    double r1 = _tc->colMap2->rValue(i1);
    if (r1 == rUNDEF)
      return 0;
    double r2 = _tc->colMap2->rValue(i2);
    if (r2 == rUNDEF)
      return 1;
    if (r1 < r2)
      return 1;
    return 0;
  }
  TableCross* _tc;
}; 

bool TableCross::fFreezing()
{
  long iCols = map1->iCols();
  long iLines = map1->iLines();
  bool fCreateMap = sMapCross.length() != 0;
  LongBuf bufRes(iCols);
  LongBuf buf1l;
  RealBuf buf1r;
  LongBuf buf2l;
  RealBuf buf2r;
  bool fUseReal1 = map1->fUseReals();
  bool fUseReal2 = map2->fUseReals();
  if (fUseReal1)
    buf1r.Size(iCols);
  else
    buf1l.Size(iCols);
  if (fUseReal2) {
    if (fUseReal1)
      htrr.Resize(160000);
    else
      htlr.Resize(160000);
    buf2r.Size(iCols);
  }
  else {
    if (fUseReal1)
      htrl.Resize(160000);
    else
      htll.Resize(160000);
    buf2l.Size(iCols);
  }
  long iComb=0;
  FileName fnTemp = fnObj;
  fnTemp.sFile = "filtemp";
  fnTemp.sExt = ".tmp";
  File filTemp(FileName::fnUnique(fnTemp), facCRT);
  filTemp.SetErase(true);
  trq.SetText(TR("Fill hash table"));
  long iMaxCount = 0;
  if (fUseReal1) {
    for (long i = 0; i < iLines; ++i) {
      if (trq.fUpdate(i, iLines)) 
        return false; 
      map1->GetLineVal(i, buf1r);
      if (fUseReal2) 
      {
        map2->GetLineVal(i, buf2r);
        for (long j = 0; j < iCols; ++j)
          if ((fIgnoreUndef1 && (buf1r[j] == rUNDEF))
           || (fIgnoreUndef2 && (buf2r[j] == rUNDEF))
           || ((buf1r[j] == rUNDEF) && (buf2r[j] == rUNDEF)))
          {
            bufRes[j] = iUNDEF;
          }
          else 
          {
            CrossCombRealReal ccr(buf1r[j], buf2r[j]);
            CrossCombRealReal& ccrHash = htrr.get(ccr);
            if (ccrHash.iIndex == iUNDEF) { // not found 
              ccr.iIndex = ++iComb;
              htrr.add(ccr);
              bufRes[j] = ccr.iIndex;
            }
            else 
            {
              ccrHash.iCount++;
              if (ccrHash.iCount > iMaxCount)
                iMaxCount = ccrHash.iCount;
              bufRes[j] = ccrHash.iIndex;
            }
          }  
      }
      else 
      {
        map2->GetLineRaw(i, buf2l);
        for (long j = 0; j < iCols; ++j) 
        {
          if ((fIgnoreUndef1 && (buf1r[j] == rUNDEF))
           || (fIgnoreUndef2 && (buf2l[j] == iUNDEF))
           || ((buf1r[j] == rUNDEF) && (buf2l[j] == iUNDEF)))
          {
            bufRes[j] = iUNDEF;
          }
          else
          {
            CrossCombRealLong ccl(buf1r[j], buf2l[j]);
            CrossCombRealLong& cclHash = htrl.get(ccl);
            if (cclHash.iIndex == iUNDEF)  // not found
            {
              ccl.iIndex = ++iComb;
              htrl.add(ccl);
              bufRes[j] = ccl.iIndex;
            }
            else
            {
              cclHash.iCount++;
              if (cclHash.iCount > iMaxCount)
                iMaxCount = cclHash.iCount;
              bufRes[j] = cclHash.iIndex;
            }
          }
         }
      }  
      if (fCreateMap)
        filTemp.Write(iCols*sizeof(long), bufRes.buf());
    }
  }
  else {
    for (long i = 0; i < iLines; ++i) {
      if (trq.fUpdate(i, iLines)) 
        return false; 
      map1->GetLineRaw(i, buf1l);
      if (fUseReal2) 
      {
        map2->GetLineVal(i, buf2r);
        for (long j = 0; j < iCols; ++j)
        {
          if ((fIgnoreUndef1 && (buf1l[j] == iUNDEF))
           || (fIgnoreUndef2 && (buf2r[j] == rUNDEF))
           || ((buf1l[j] == iUNDEF) && (buf2r[j] == rUNDEF)))
          {
            bufRes[j] = iUNDEF;
          }
          else
          { 
			CrossCombLongReal ccr(buf1l[j], buf2r[j]);
			CrossCombLongReal& ccrHash = htlr.get(ccr);
            if (ccrHash.iIndex == iUNDEF)  // not found
            {
              ccr.iIndex = ++iComb;
              htlr.add(ccr);
              bufRes[j] = ccr.iIndex;
            }
            else
            {
              ccrHash.iCount++;
              if (ccrHash.iCount > iMaxCount)
                iMaxCount = ccrHash.iCount;
              bufRes[j] = ccrHash.iIndex;
            }
          }
         }
      }
      else {
        map2->GetLineRaw(i, buf2l);
        for (long j = 0; j < iCols; ++j) 
        {
          if ((fIgnoreUndef1 && (buf1l[j] == iUNDEF))
           || (fIgnoreUndef2 && (buf2l[j] == iUNDEF))
           || ((buf1l[j] == iUNDEF) && (buf2l[j] == iUNDEF)))
          {
            bufRes[j] = iUNDEF;
          }
          else
          {
            CrossCombLongLong ccl(buf1l[j], buf2l[j]);
            CrossCombLongLong& cclHash = htll.get(ccl);
            if (cclHash.iIndex == iUNDEF)  // not found
            {
              ccl.iIndex = ++iComb;
              htll.add(ccl);
              bufRes[j] = ccl.iIndex;
            }
            else
            {
              cclHash.iCount++;
              if (cclHash.iCount > iMaxCount)
                iMaxCount = cclHash.iCount;
              bufRes[j] = cclHash.iIndex;
            }
          }
          }
      }  
      if (fCreateMap)
        filTemp.Write(iCols*sizeof(long), bufRes.buf());
    }
  }  
  if (trq.fUpdate(iLines, iLines)) 
    return false; 

  DomainSort* pdsrt = dm()->pdsrt();
  pdsrt->Resize(iComb);
  ptr.CheckNrRecs();

  Init();

  trq.SetText(TR("Fill cross table"));
  long iIndex;
  long iChangName = 0;
  pdsrt->dsType = DomainSort::dsMANUAL;
  if (!fUseReal1) {
    if (!fUseReal2) {
      for (int k = 0; k < htll.iTabSize; ++k) {
        if ((k % 100) == 0)
          if (trq.fUpdate(k, htll.iTabSize))
            return false;
        if (htll.table[k]) {
          if (trq.fUpdate(k, htll.iTabSize))
            return false;
          for (SLIter<CrossCombLongLong> iter(htll.table[k]); iter.fValid(); ++iter)
          {
            String sCombVal = sCrossValue(map1, iter().i1);
            if (map1 != map2) {
              sCombVal &= " * ";
              sCombVal &= sCrossValue(map2, iter().i2);
            }
            iIndex = iter().iIndex;
            try {
			  pdsrt->SetVal(iIndex, sCombVal);
            }
            catch (ErrorObject&) {
              // if one of the input maps contains deleted domain items you could get double names
            }
            colMap1->PutRaw(iIndex, iter().i1);
            if (colMap1 != colMap2)
              colMap2->PutRaw(iIndex, iter().i2);
            colNPix->PutVal(iIndex, iter().iCount);
          }
        }
      }
    }
    else {
      DomainValue * dv2 = map2->dm()->pdv();
      for (int k = 0; k < htlr.iTabSize; ++k) {
        if ((k % 100) == 0)
          if (trq.fUpdate(k, htlr.iTabSize))
            return false;
        if (htlr.table[k]) {
          if (trq.fUpdate(k, htlr.iTabSize))
            return false;
          for (SLIter<CrossCombLongReal> iter(htlr.table[k]); iter.fValid(); ++iter)
          {
                String sCombVal = sCrossValue(map1, iter().i1);
                if (map1 != map2)
                {
                    sCombVal &= " * ";
                    sCombVal &= dv2->sValue(iter().r2,0);   
                }
            iIndex = iter().iIndex;
			try {
				if ( pdsrt->iOrd(sCombVal) != iUNDEF ){
					iChangName++;
					sCombVal &= String(" #%li", iChangName);
				}
              pdsrt->SetVal(iIndex, sCombVal);
            }
            catch (ErrorObject&) {
              //if one of the input maps contains deleted domain items you could get double names
            }
            colMap1->PutRaw(iIndex, iter().i1);
            colMap2->PutVal(iIndex, iter().r2);
            colNPix->PutVal(iIndex, iter().iCount);
          }
        }
      }
    }
  }
  else {
    DomainValue * dv1 = map1->dm()->pdv();
    if (!fUseReal2) {
      for (int k = 0; k < htrl.iTabSize; ++k) {
        if ((k % 100) == 0)
          if (trq.fUpdate(k, htrl.iTabSize))
            return false;
        if (htrl.table[k]) {
          if (trq.fUpdate(k, htrl.iTabSize))
            return false;
          for (SLIter<CrossCombRealLong> iter(htrl.table[k]); iter.fValid(); ++iter) {
            String sCombVal = dv1->sValue(iter().r1,0);
            if (map1 != map2) {
              sCombVal &= " * ";
              sCombVal &= map2->dvrs().sValueByRaw(iter().i2,0);
            }
            iIndex = iter().iIndex;
            try {
			  if ( pdsrt->iOrd(sCombVal) != iUNDEF ){
				iChangName++;
				sCombVal &= String(" #%li", iChangName);
			  }
              pdsrt->SetVal(iIndex, sCombVal);
            }
            catch (ErrorObject&) {
              // if one of the input maps contains deleted domain items you could get double names
            }
            colMap1->PutVal(iIndex, iter().r1);
            colMap2->PutRaw(iIndex, iter().i2);
            colNPix->PutVal(iIndex, iter().iCount);
          }
        }
      }
    }
    else {
      DomainValue * dv2 = map2->dm()->pdv();
      for (int k = 0; k < htrr.iTabSize; ++k) {
        if ((k % 100) == 0)
          if (trq.fUpdate(k, htrr.iTabSize))
            return false;
        if (htrr.table[k]) {
          if (trq.fUpdate(k, htrr.iTabSize))
            return false;
          for (SLIter<CrossCombRealReal> iter(htrr.table[k]); iter.fValid(); ++iter) {
            String sCombVal = dv1->sValue(iter().r1,0);
            if (map1 != map2) {
              sCombVal &= " * ";
              sCombVal &= dv2->sValue(iter().r2,0); 
            }
            iIndex = iter().iIndex;
            try {
			  if ( pdsrt->iOrd(sCombVal) != iUNDEF ){
				iChangName++;
				sCombVal &= String(" #%li", iChangName);
			  }
              pdsrt->SetVal(iIndex, sCombVal);
            }
            catch (ErrorObject&) {
              // if one of the input maps contains deleted domain items you could get double names
            }
            colMap1->PutVal(iIndex, iter().r1);
            if (colMap1 != colMap2)
              colMap2->PutVal(iIndex, iter().r2);
            colNPix->PutVal(iIndex, iter().iCount);
          }
        }
      }
    }
  }
  Map mpCross;
  if (fCreateMap) {
    FileName fnCrMap(sMapCross, fnObj);
    fnCrMap.sExt = ".mpr";
    try {
      String sType;
      ObjectInfo::ReadElement("MapFromMap", "Type", fnCrMap, sType);
      if (sType == "MapCross")
        mpCross = Map(fnCrMap);
    }
    catch (const ErrorObject&) {
      try {
        mpCross = Map(fnCrMap, String("MapCross(%S,%S,%S)",
                  map1->sNameQuoted(true, fnObj.sPath()), map2->sNameQuoted(true, fnObj.sPath()),
                  ptr.sNameQuoted(true, fnObj.sPath())));
      }
      catch (const ErrorObject& err) {
        err.Show();
      }
    }
    fCreateMap = mpCross.fValid();
  }

//  if ( iComb > 16000) 
//	{
//    long iMax = iComb + 1;
//    for (long i = 1; i < iMax; ++i) {
//      pdsrt->SetOrdWithoutIndex(i, i);
//    }
//  }
//  else 
	{
		trq.fUpdate(iComb, iComb); 
		trq.SetText(TR("Sorting"));
		vector<long> index(iComb);
		for (long i = 0; i < iComb; ++i)
			index[i] = 1+i;
		if (fUseReal1) {
      if (fUseReal2) {
        CompareRealReal cmp(this);
				sort(index.begin(), index.end(), cmp);
      }
      else {
        CompareRealLong cmp(this);
				sort(index.begin(), index.end(), cmp);
      }
		}  
		else {
      if (fUseReal2) {
        CompareLongReal cmp(this);
				sort(index.begin(), index.end(), cmp);
      }
      else {
        CompareLongLong cmp(this);
				sort(index.begin(), index.end(), cmp);
      }
		}  
		for (long i = 0; i < iComb; ++i) {
			if (i % 10 == 0)
				if (trq.fUpdate(i, iComb)) 
					return false; 
			pdsrt->SetOrdWithoutIndex(index[i], i+1);
		}  
		trq.fUpdate(iComb, iComb);
		index.resize(0);
	}
  pdsrt->CalcIndex();
  if (fCreateMap)
  {
	mpCross->OpenMapVirtual();
    mpCross->DeleteCalc();

	// First set domain none, to force removal of current domain (possibly with the wrong storetype)
	DomainValueRangeStruct dvs("none");
	mpCross->SetDomainValueRangeStruct(dvs);
	// Then set the domain again, but now with the correct storetype
	dvs = DomainValueRangeStruct(dm());
	mpCross->SetDomainValueRangeStruct(dvs);
    mpCross->CreateMapStore();
    trq.SetText(TR("Writing cross map"));
    filTemp.Seek(0);
    for (long i=0; i< mpCross->iLines(); ++i) {
      if (trq.fUpdate(i, mpCross->iLines()))
        return false;
      filTemp.Read(iCols*sizeof(long), bufRes.buf());
      mpCross->PutLineRaw(i, bufRes);
    }
    mpCross->Store();
  }

  if (colArea.fValid()) {
    double rPixSize = map1->gr()->rPixSize();
    if (rPixSize > 0) {
      rPixSize *= rPixSize;
      for (long i=1; i <= iRecs(); ++i)
        colArea->PutVal(i, rPixSize*colNPix->rValue(i));
    }
  }  

  return true;
}

String TableCross::sCrossValue(const Map& mp, long iRaw)
{
    String sVal;
    DomainSort* ds = mp->dm()->pdsrt();
    bool fOk = false;
    if (ds && ds->fCodesAvailable()) {
      sVal = ds->sCodeByRaw(iRaw, 0);
      fOk = sVal.length() > 0;
    }
    if (!fOk)
      sVal = mp->dvrs().sValueByRaw(iRaw,0);
    return sVal;
}

Table TableCross::tblAgg(const FileName& fn) const
{
  if (!File::fExist(fn))
    return Table(fn, colMap1->dm());
  Table tbl(fn);
  if (tbl->dm() == colMap1->dm())
    return tbl;
  throw ErrorNotCreated(fn); // should give better message

  return Table();
}

Column TableCross::colAgg(const Table& tbl, const String& sColName, const String& sAgg) const
{
  assert(tbl->dm() == colMap1->dm());
  String sColGrp = String("%S.%S", ptr.sNameQuoted(false, fnObj.sPath()), colMap1->sNameQuoted());
  String sColVal = String("%S.%S", ptr.sNameQuoted(false, fnObj.sPath()), colMap2->sNameQuoted());
  String sColWgt = String("%S.NPix", ptr.sNameQuoted(false, fnObj.sPath()));
  bool fWeight = fCIStrEqual(sAgg, "avg") || fCIStrEqual(sAgg, "std") || fCIStrEqual(sAgg, "prd") || fCIStrEqual(sAgg, "med");
  Column col;
  if (fWeight)
    col = Column(tbl, sColName, String("agg%S(%S,%S,%S)", sAgg, sColVal, sColGrp, sColWgt));
  else if (fCIStrEqual(sAgg , "sum"))
    col = Column(tbl, sColName, String("agg%S(%S,%S)", sAgg, sColWgt, sColGrp));
  else
    col = Column(tbl, sColName, String("agg%S(%S,%S)", sAgg, sColVal, sColGrp));
//  if (0 != tbl->pts())  // already done by constructor
//    tbl->pts()->AddCol(col);
  return col;
}
/*
bool TableCross::fFrozenValid() const
{
  return TableVirtual::fFrozenValid();
}
*/
void TableCross::Init()
{
  if (pts==0)
    return;
	if (!map1.fValid() || !map2.fValid())
		throw ErrorDummy();
//  Time tim = pts->objtime;
  String sColName = "map1";
  if (map1->fnObj.sFile.length() != 0)
    sColName = map1->fnObj.sFile;
  colMap1 = pts->col(sColName);
  if (!colMap1.fValid()) {
    colMap1 = pts->colNew(sColName, map1->dvrs());
//    _iCols += 1;
  }
  colMap1->SetOwnedByTable(true);
  colMap1->SetReadOnly(true);
  if (map1 != map2) {
//    colMap2 = pts->colNew(sColName, map2->dvrs());
    sColName = "map2";
    if (map2->fnObj.sFile.length() != 0)
      sColName = map2->fnObj.sFile;
    colMap2 = pts->col(sColName);
    if (!colMap2.fValid()) {
      colMap2 = pts->colNew(sColName, map2->dvrs());
//      _iCols += 1;
    }
    colMap2->SetOwnedByTable(true);
    colMap2->SetReadOnly(true);
  }
  else
    colMap2 = colMap1;
  colNPix = pts->col("NPix");
  long iMaxCount = map1->iLines() * map1->iCols();
  if (!colNPix.fValid()) {
    colNPix = pts->colNew("NPix", Domain("count"), ValueRange(0, iMaxCount));
    for (long i=iOffset(); i <= iOffset()+iRecs()-1; ++i)
       colNPix->PutVal(i, 0L);
//    _iCols += 1;
    colNPix->SetOwnedByTable(true);
    colNPix->SetReadOnly(true);
  }
  double rPixSize = map1->gr()->rPixSize();
  if (rPixSize > 0) {
    colArea = pts->col("Area");
    bool fOldFormat = false;
    if (colArea.fValid()) {
      // check if area is dependent column as in 2.01 and previous versions
//      ColumnVirtual* pcv = colArea->pcv();
//      if (0 != pcv) {
      if (colArea->fDependent()) {
        pts->RemoveCol(colArea);
        colArea = Column();
        fOldFormat = true;
  //      _iCols -= 1;
      }  
    }
    if (!colArea.fValid()) {
      rPixSize *= rPixSize;
      double rStep = 1;
      if (fmod(rPixSize,1) > 1e-6)
        rStep = 0.1;
      ValueRange vrr(0, iMaxCount*rPixSize, rStep);
      vrr->AdjustRangeToStep();
      colArea = pts->colNew("Area", Domain("value"), vrr);
//      _iCols += 1;
      if (fOldFormat) {
        for (long i=1; i <= iRecs(); ++i)
          colArea->PutVal(i, rPixSize*colNPix->rValue(i));
        Store();
      }    
    }  
    colArea->SetOwnedByTable(true);
    colArea->SetReadOnly(true);
  }
//  pts->fChanged = false;
//  pts->objtime = tim;
}

void TableCross::UnFreeze()
{
  if (colMap1.fValid()) {
    pts->RemoveCol(colMap1);
    colMap1 = Column();
  }  
  if (colMap2.fValid()) {
    pts->RemoveCol(colMap2);
    colMap2 = Column();
  }  
  if (colNPix.fValid()) {
    pts->RemoveCol(colNPix);
    colNPix = Column();
  }  
  if (colArea.fValid()) {
    pts->RemoveCol(colArea);
    colArea = Column();
  }
  TableVirtual::UnFreeze();
}

String TableCross::sExpression() const
{
  String sMapCr, sIgnore;
  if (sMapCross.length() > 0)
    sMapCr = String(",%S", sMapCross);
  if (fIgnoreUndef1)
    if (fIgnoreUndef2)
      sIgnore = ",IgnoreUndefs";
    else
      sIgnore = ",IgnoreUndef1";
  else
    if (fIgnoreUndef2)
      sIgnore = ",IgnoreUndef2";
  String sMap1 = map1->sNameQuoted(true, fnObj.sPath());
  String sMap2 = map2->sNameQuoted(true, fnObj.sPath());
  return String("TableCross(%S,%S%S%S)",
                sMap1, sMap2, sMapCr, sIgnore);
}
