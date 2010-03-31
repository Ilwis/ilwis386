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
/* ColumnVirtual
   Copyright Ilwis System Development ITC
   may 1995, by Jelle Wind
	Last change:  WK    1 Jul 98   10:00 am
*/
#define COLVIRT_C
#include "Engine\Table\COLSTORE.H"
#include "Engine\Applications\COLVIRT.H"
#include "Engine\Domain\Dmvalue.h"
#include "Engine\Base\DataObjects\valrange.h"
#include "Engine\Applications\ModuleMap.h"
#include "Engine\Base\System\Engine.h"
#include "Engine\Base\Tokbase.h"
#include "Headers\Hs\tbl.hs"

ColumnVirtual* ColumnVirtual::create(const Table& tbl, const String& sColName, ColumnPtr& p)
{
  String sExpr;
  ObjectInfo::ReadElement(String("%SCol:%S", tbl->sSectionPrefix, sColName).scVal(), "Expression", tbl->fnObj, sExpr);
  String sFunc = IlwisObjectPtr::sParseFunc(sExpr);

  ApplicationInfo * info = Engine::modules.getAppInfo(sFunc);
  vector<void *> extraParms = vector<void *>();
  extraParms.push_back((void *)&sColName);
  if ( info != NULL ) {
	return (ColumnVirtual *)(info->createFunction)(tbl->fnObj, p, "", extraParms);
  }
  throw ErrorInvalidType(tbl->fnObj, "ColumnVirtual", sFunc);





  /*if (fCIStrEqual(sFunc.sLeft(3),"agg")  || fCIStrEqual(sFunc.sLeft(15),"ColumnAggregate") ||
      fCIStrEqual(sFunc.sLeft(4),"join") || fCIStrEqual(sFunc.sLeft(10),"ColumnJoin")) {
    if (ColumnAggregate::fValidFunc(sFunc))
      return new ColumnAggregate(tbl, sColName, p);
  }  
  if (fCIStrEqual(sFunc.sLeft(3),"cum") || fCIStrEqual(sFunc, "ColumnCumulative"))
    return new ColumnCumulative(tbl, sColName, p);
  if (fCIStrEqual(sFunc,"ColumnLeastSquaresFit"))
    return new ColumnLeastSquaresFit(tbl, sColName, p);
  if (fCIStrEqual(sFunc,"ColumnSemiVarioGram"))
    return new ColumnSemiVariogram(tbl, sColName, p);
  return new ColumnCalculate(tbl, sColName, p);*/

	return 0;
}

ColumnVirtual* ColumnVirtual::create(const Table& tbl, const String& sColName, ColumnPtr& p,
                                     const String& sExpression, const DomainValueRangeStruct& dvs)
{
  String sFunc = IlwisObjectPtr::sParseFunc(sExpression);
  ApplicationInfo * info = Engine::modules.getAppInfo(sFunc);
  vector<void *> extraParms = vector<void *>();
  extraParms.push_back((void *)&sColName);
  extraParms.push_back((void *)&dvs);
  extraParms.push_back((void *)&sFunc);
  if ( info != NULL ) {
	return (ColumnVirtual *)(info->createFunction)(tbl->fnObj, p, sExpression, extraParms);
  }
  info = Engine::modules.getAppInfo("ColumnCalculate");
  if ( info)
	return (ColumnVirtual *)(info->createFunction)(tbl->fnObj, p, sExpression, extraParms);

  throw ErrorInvalidType(tbl->fnObj, "ColumnVirtual", sFunc);
  //String sFunc = IlwisObjectPtr::sParseFunc(sExpression);
  //if (fCIStrEqual("ColumnCalculate",sFunc)) {
  //  String sStrippedExpr = sExpression.sSub(16, sExpression.length()-17);
  //  return ColumnCalculate::create(tbl, sColName, p, sStrippedExpr, dvs);
  //}
  //if (fCIStrEqual(sFunc.sLeft(3), "agg")  || fCIStrEqual(sFunc.sLeft(15), "ColumnAggregate") ||
  //  if (fCIStrEqual(sFunc.sLeft(4), "join") || fCIStrEqual(sFunc.sLeft(10), "ColumnJoin")) {
  //    AggregateFunction* agf = AggregateFunction::create(sFunc);
  //    if (0 == agf) {
  //      Array<String> as;
  //      int iParms = IlwisObjectPtr::iParseParm(sExpression, as);
  //      String sExpr;
  //      if (iParms == 2)
  //        sExpr = String("%S.%S", as[0], as[1]);
  //      else if (iParms == 3)
  //        sExpr = String("%S.%S[%S]", as[0], as[1], as[2]);
  //      else
  //        return ColumnCalculate::create(tbl, sColName, p, sExpression, dvs);
  //      return ColumnCalculate::create(tbl, sColName, p, sExpr, dvs);
  //    }
  //  }
  //  if (ColumnAggregate::fValidFunc(sFunc))
  //    return ColumnAggregate::create(tbl, sColName, p, sExpression, dvs);
  //}  
  //if (fCIStrEqual(sFunc.sLeft(3), "cum") || fCIStrEqual(sFunc, "ColumnCumulative"))
  //  return ColumnCumulative::create(tbl, sColName, p, sExpression, dvs);
  //if (fCIStrEqual(sFunc, "ColumnLeastSquaresFit"))
  //  return ColumnLeastSquaresFit::create(tbl, sColName, p, sExpression, dvs);
  //if (fCIStrEqual(sFunc, "ColumnSemiVarioGram"))
  //  return ColumnSemiVariogram::create(tbl, sColName, p, sExpression, dvs);
  //return ColumnCalculate::create(tbl, sColName, p, sExpression, dvs);

	return 0;
}

ColumnVirtual::ColumnVirtual(const Table& tbl, const String& sColName, ColumnPtr& _ptr)
: IlwisObjectVirtual(tbl->fnObj, _ptr, _ptr.objdep, false), ptr(_ptr), pcs(_ptr.pcs)
{
//  ReadElement(sSection(), "Expression", _sExpression);
//  if (_sExpression.length() == 0)
//    throw ErrorExpression(fnObj, _sExpression);
  ptr.ReadElement(sSection().scVal(), "SearchTbl", fnTblSearch);
/*  ReadElement(sSection(), "Stored", fFrozen);
  ptr = 0;
  if (fFrozen)
    ptr = new ColumnStore(tbl, sNam, dvs);
  if (ptr == 0)
    fFrozen = false;
  if (ptr != 0)
    fFrozen = objtime <= ptr->objtime;
  else
    fFrozen = false;
*/
}

ColumnVirtual::ColumnVirtual(const Table& tbl, const String& sColName, ColumnPtr& _ptr,
                             const DomainValueRangeStruct& dvs, const Table& tblSearch)
: IlwisObjectVirtual(tbl->fnObj, _ptr, _ptr.objdep, true), ptr(_ptr), pcs(_ptr.pcs)
{
  ptr.dvs = dvs;
//  _fDataReadOnly = true;
//  ptr = 0;
//  fFrozen = false;
  fNeedFreeze = true;
  if (tblSearch.fValid())
    fnTblSearch = tblSearch->fnObj;
 }

bool ColumnVirtual::fInit()
{
  return true;
}

void ColumnVirtual::Store()
{
  // note: doesn't call IlwisObjectVirtual::Store(), because this func writes in a wrong section
  if (ptr.sNam[0] == '#') // temp column
    return;
  if (ptr.fErase) {
    WriteElement(sSection().scVal(), NULL, NULL);
    return;
  }  
  WriteEntry("Type", "ColumnVirtual");
  // for downward compatibility to 2.02:
  WriteEntry("Expression", sExpression());
  if (fnTblSearch.fValid())
    WriteEntry("SearchTbl", fnTblSearch.sFullNameQuoted());
  objdep.Store(&optr, sSection());
}

ColumnVirtual::~ColumnVirtual()
{
  if (ptr.fErase)
    WriteEntry((const char*)0, (const char*)0);
}

bool ColumnVirtual::fDomainChangeable() const
{
  return dm()->pdv() != 0;
}

bool ColumnVirtual::fValueRangeChangeable() const
{
  return dm()->pdv() != 0;
}

void ColumnVirtual::SetDomainValueRangeStruct(const DomainValueRangeStruct& dvrs)
{
  /* nothing */
}
/*
void ColumnVirtual::SetValueRange(const ValueRange& vr)
{
  ColumnPtr::SetValueRange(vr);
  UnFreeze();
}

void ColumnVirtual::DependencyNames(Array<String>& asNames) const
{ 
  objdep.Names(fnTbl.sPath(), asNames);
}

void ColumnVirtual::GetObjectDependencies(Array<FileName>& afnObjDep)
{
  objdep.Names(afnObjDep);
}
*/
#define BufSize 4096

#define compute(BufRawVal, Type)                     \
{ Type buf(BufSize);                              \
 for (long i = 0; i < iBufs; ++i) {               \
   Compute##BufRawVal(buf, iOffset()+i*BufSize, BufSize); \
   pcs->Put##BufRawVal(buf, iOffset()+i*BufSize, BufSize);\
 }                                                \
 if (iLeft > 0) {                                 \
   buf.Size(iLeft);                               \
   Compute##BufRawVal(buf, iOffset()+iBufs*BufSize, iLeft);  \
   pcs->Put##BufRawVal(buf, iOffset()+iBufs*BufSize, iLeft); \
 }\
}

void ColumnVirtual::Freeze()
{
	if (!objdep.fMakeAllUsable())
		return;
	
	try
	{
		CreateColumnStore();
		assert(pcs != 0);
		InitFreeze();

		bool fFrozen;
		try
		{
			fFrozen = fFreezing();
		}
		catch (ErrorObject& err)
		{
			fFrozen = false;
			err.Show();
		}
		if (fFrozen)
		{
			ptr.Updated();
			Table tbl(ptr.fnTbl);
			tbl->Updated();
		}
		else
		{
			delete pcs;
			pcs = 0;
		}
		ptr.Store();
	}
	catch (CException *err)
	{
    MessageBeep(MB_ICONHAND);
		err->ReportError(MB_OK|MB_ICONHAND|MB_TOPMOST);
		err->Delete();
	}
}

bool ColumnVirtual::fFreezing()
{
  long iBufs = iRecs() / BufSize;
  long iLeft = iRecs() - iBufs * BufSize;

  switch (st()) {
    case stBIT:
    case stDUET:
    case stNIBBLE:
    case stBYTE:
      compute(BufRaw, ByteBuf);
      break;
    case stINT:
      compute(BufRaw, IntBuf);
      break;
    case stLONG:
      compute(BufRaw, LongBuf);
      break;
    case stREAL:
      compute(BufVal, RealBuf);
      break;
    case stSTRING:
      compute(BufVal, StringBuf);
      break;
    case stCRD:
      compute(BufVal, CoordBuf);
      break;
  }
  return true;
}

void ColumnVirtual::UnFreeze()
{
  if (0 == pcs)
    return;
  delete pcs;
  pcs = 0;
//fFrozen = false;
  ptr.SetMinMax(RangeInt());
  ptr.SetMinMax(RangeReal());
//TablePtr::Changed(fnTbl);
  ptr.Store();
}

long ColumnVirtual::iComputeRaw(long iKey) const
{
  return dvrs().iRaw(rComputeVal(iKey));
}

double ColumnVirtual::rComputeVal(long) const
{
  return rUNDEF;
} 

Coord ColumnVirtual::cComputeVal(long) const
{
  return crdUNDEF;
} 

long ColumnVirtual::iComputeVal(long iKey) const
{
  if (!dvrs().fValues())
    return iUNDEF;
  if (dvrs().fRawAvailable()) {
    long iRaw = iComputeRaw(iKey);
    return dvrs().iValue(iRaw);
  }
  return longConv(rComputeVal(iKey));
}

String ColumnVirtual::sComputeVal(long iKey, short iWidth, short iDec) const
{
  if (dvrs().fUseReals())
    return dvrs().sValue(rComputeVal(iKey), iWidth, iDec);
  else
    return dvrs().sValueByRaw(iComputeRaw(iKey), iWidth, iDec);
}

void ColumnVirtual::ComputeBufRaw(ByteBuf& buf, long iKey, long iNr) const
{
  LongBuf b(iNr);
  ComputeBufRaw(b, iKey, iNr);
  for (long i = 0; i < iNr; ++i)
    buf[i] = byteConv(b[i]);
}

void ColumnVirtual::ComputeBufRaw(IntBuf& buf, long iKey, long iNr) const
{
  LongBuf b(iNr);
  ComputeBufRaw(b, iKey, iNr);
  for (long i = 0; i < iNr; ++i)
    buf[i] = shortConv(b[i]);
}

void ColumnVirtual::ComputeBufRaw(LongBuf& buf, long iKey, long iNr) const
{
  long key = iKey;
  for (long i = 0; i < iNr; ++i, ++key)
    buf[i] = iComputeRaw(key);
}

void ColumnVirtual::ComputeBufVal(LongBuf& buf, long iKey, long iNr) const
{
  long key = iKey;
  for (long i = 0; i < iNr; ++i, ++key)
    buf[i] = iComputeVal(key);
}

void ColumnVirtual::ComputeBufVal(RealBuf& buf, long iKey, long iNr) const
{
  long key = iKey;
  for (long i = 0; i < iNr; ++i, ++key)
    buf[i] = rComputeVal(key);
}

void ColumnVirtual::ComputeBufVal(StringBuf& buf, long iKey, long iNr, short iWid, short iDec) const
{
  long key = iKey;
  for (long i = 0; i < iNr; ++i, ++key)
    buf[i] = sComputeVal(key, iWid, iDec);
}

void ColumnVirtual::ComputeBufVal(CoordBuf& buf, long iKey, long iNr) const
{
  long key = iKey;
  for (long i = 0; i < iNr; ++i, ++key)
    buf[i] = cComputeVal(key);
}

String ColumnVirtual::sTblSearch(String& sExpression) 
{
  String sExpr = sExpression;
  char* p = sExpr.strchrQuoted('}');
  if (0 == p)
    return String();
  p++;
  while (p && (*p == ' '))
    p++;
  sExpression = p;
  String sTbl;
  TokenizerBase tkn(&TextInput(sExpr));
  Token tok = tkn.tokGet(); 
  while ((tok.sVal() != "}") && (tok.sVal().length() != 0)) {
    if (tok.sVal() == ";") {
      tok = tkn.tokGet();
      continue;
    }
    if (fCIStrEqual(tok.sVal(),"search")) {
      tok = tkn.tokGet();
      if (tok.sVal() != "=")
        return String();
      tok = tkn.tokGet();
      sTbl = tok.sVal();
    }
    tok = tkn.tokGet(); 
  } 
  if (tok.sVal() != "}") {
    ErrorObject(STBLErrCurlyExpected, 0).Show();  // '}'
    return String();
  }
  return sTbl; 
}

String ColumnVirtual::sExpression() const
{
/*  if (!fnTblSearch.fValid())
    return _sExpression;
  else
    return String("{search=%S} %S", fnTblSearch.sRelative(true, fnObj.sPath()), _sExpression);*/
  return String();
}

String ColumnVirtual::sType() const
{
  return "Dependent Column";
}

void ColumnVirtual::CreateColumnStore()
{
  if (0 != pcs) {
    delete pcs;
    pcs = 0;
  }  
  Table tbl(ptr.fnTbl);
	// function is called from freeze functionality that stores the tabel at the end 
	// if data is not loaded and there are multiple calculated columns in the table it will not be stored properly unless
	// the data is loaded. Else it tries to load the data at a later moment from disk and overwrites calculated columns with undefs
	// on disk no calculation has been stored (yet)
	tbl->LoadData();
  pcs = new ColumnStore(tbl, ptr.sName(), ptr, ptr.dvrs());
//  ptr.SetDataReadOnly(); // previous statement sets it on false
}

void ColumnVirtual::Replace(const String& sExpr)
{
}

DomainValueRangeStruct ColumnVirtual::dvrsDefault(const String& sExpression) const
{
  return DomainValueRangeStruct();
}

bool ColumnVirtual::fMergeDomainForCalc(Domain& /*dm*/, const String& /*sExpr*/)
{
  return true;
}




