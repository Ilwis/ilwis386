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
/* instruc1.c
// Ilwis 2.0 instructions for calculator (1)
// sept. 1994, Jelle Wind
	Last change:  JEL  12 Jul 97    3:36 pm
*/
#include "Engine\Scripting\Instrucs.h"
#include "Engine\Scripting\CALCSTCK.H"
#include "Engine\Scripting\Calcvar.h"
#include "Engine\Domain\dmcoord.h"
#include "Engine\Domain\dmgroup.h"
#include "Engine\Domain\dmpict.h"
#include "Engine\Domain\Dmvalue.h"
#include "Headers\Hs\tbl.hs"


InstMapValue::InstMapValue(Instructions* instrucs, const BaseMap& m, const CoordSystem& csInp)
  : InstBase(instrucs)
{
  csIn = csInp;
  mp = m;
  csOut = mp->cs();
  fTransform = csIn != csOut;
  if (fTransform)
    fTransform = csOut->fConvertFrom(csIn);
  sot = StackObject::sotype(mp->dvrs());
  DomainPicture* pdp = mp->dm()->pdp();
  if (0 != pdp)
    rpr = pdp->rpr();
}

InstMapValue::~InstMapValue()
{
}

void InstMapValue::Exec()
{
  StackObject* soCrd = inst->stkCalc.pop();
//  CoordBuf bufCrd;
  soCrd->Resize(bufCrd);
  soCrd->GetVal(bufCrd);
  if (fTransform)
  for (int i=0; i<bufCrd.iSize(); i++)
    bufCrd[i] = csOut->cConv(csIn, bufCrd[i]);
  StackObject* soRes = new StackObject(sot, mp->dvrs(), inst->env.iBufSize);
  switch (sot)  {
    case sotLongRaw : {
       //ongBuf bufRes;
        bufResL.Size(soRes->iSize());
        for (int i=0; i<bufResL.iSize(); i++)
          bufResL[i] = mp->iRaw(bufCrd[i]);
        if (rpr.fValid())
          for (int i=0; i < bufResL.iSize(); i++)
            bufResL[i] = rpr->clrRaw(bufResL[i]);
        soRes->PutRaw(bufResL);
      }
      break;
    case sotLongVal : {
//        LongBuf bufRes;
        bufResL.Size(soRes->iSize());
        for (int i=0; i<bufResL.iSize(); i++)
          bufResL[i] = longConv(mp->rValue(bufCrd[i]));
        soRes->PutVal(bufResL);
      }
      break;
    case sotRealVal : {
//        RealBuf bufRes;
        bufResR.Size(soRes->iSize());
        for (int i=0; i<bufResR.iSize(); i++)
          bufResR[i] = mp->rValue(bufCrd[i]);
        soRes->PutVal(bufResR);
      }
      break;
    case sotStringVal : {
//        StringBuf bufRes;
        bufResS.Size(soRes->iSize());
        for (int i=0; i<bufResS.iSize(); i++)
          bufResS[i] = mp->sValue(bufCrd[i]);
        soRes->PutVal(bufResS);
      }
      break;
  }
  delete soCrd;
  inst->stkCalc.push(soRes);
}

void InstMapValue::AddDependentObjects(ObjectDependency& objdep)
{
  objdep.Add(mp);
}

InstRasValue::InstRasValue(Instructions* instrucs, const Map& m)
  : InstBase(instrucs)
{
  mp = m;
  sot = StackObject::sotype(mp->dvrs());
  DomainPicture* pdp = mp->dm()->pdp();
  if (0 != pdp)
    rpr = pdp->rpr();
}

InstRasValue::~InstRasValue()
{
}

void InstRasValue::Exec()
{
  StackObject* soCol = inst->stkCalc.pop();
  StackObject* soRow = inst->stkCalc.pop();
//  LongBuf bufRow, bufCol;
  soRow->Resize(bufRow);
  soRow->GetVal(bufRow);
  soCol->Resize(bufCol);
  soCol->GetVal(bufCol);
  StackObject* soRes = new StackObject(sot, mp->dvrs(), inst->env.iBufSize);
  switch (sot)  {
    case sotLongRaw : {
//        LongBuf bufResL;
        bufResL.Size(soRes->iSize());
        for (int i=0; i<bufResL.iSize(); i++)
          bufResL[i] = mp->iRaw(RowCol(bufRow[i]-1, bufCol[i]-1));
        if (rpr.fValid())
          for (int i=0; i < bufResL.iSize(); i++)
            bufResL[i] = rpr->clrRaw(bufResL[i]);
        soRes->PutRaw(bufResL);
      }
      break;
    case sotLongVal : {
  //      LongBuf bufRes;
        bufResL.Size(soRes->iSize());
        for (int i=0; i<bufResL.iSize(); i++)
          bufResL[i] = mp->iValue(RowCol(bufRow[i]-1, bufCol[i]-1));
        soRes->PutVal(bufResL);
      }
      break;
    case sotRealVal : {
//        RealBuf bufRes;
        bufResR.Size(soRes->iSize());
        for (int i=0; i<bufResR.iSize(); i++)
          bufResR[i] = mp->rValue(RowCol(bufRow[i]-1, bufCol[i]-1));
        soRes->PutVal(bufResR);
      }
      break;
    case sotStringVal : {
//        StringBuf bufRes;
        bufResS.Size(soRes->iSize());
        for (int i=0; i<bufResS.iSize(); i++)
          bufResS[i] = mp->sValue(RowCol(bufRow[i]-1, bufCol[i]-1));
        soRes->PutVal(bufResS);
      }
      break;
  }
  delete soRow;
  delete soCol;
  inst->stkCalc.push(soRes);
}

void InstRasValue::GetMapUsed(Map& m)
{
  mp = m;
}

void InstRasValue::ReplaceMapUsed(const Map& m)
{
  if (mp.fValid())
    mp = m;
}

void InstRasValue::AddDependentObjects(ObjectDependency& objdep)
{
  objdep.Add(mp);
}

InstMapLineCol::InstMapLineCol(Instructions* instrucs, const Map& m, const CoordSystem& csInp, bool fCl)
  : InstBase(instrucs), fCol(fCl)
{
  csIn = csInp;
  csOut = m->cs();
  gr = m->gr();
  mp = m;
  fTransform = csIn != csOut;
  if (fTransform)
    fTransform = csOut->fConvertFrom(csIn);
  dvrs = DomainValueRangeStruct(1, fCol ? gr->rcSize().Col : gr->rcSize().Row);
}

InstMapLineCol::~InstMapLineCol()
{
}

void InstMapLineCol::Exec()
{
  StackObject* soCrd = inst->stkCalc.pop();
//  CoordBuf bufCrd;
  soCrd->Resize(bufCrd);
  soCrd->GetVal(bufCrd);
  if (fTransform)
  for (int i=0; i<bufCrd.iSize(); i++)
    bufCrd[i] = csOut->cConv(csIn, bufCrd[i]);
  StackObject* soRes = new StackObject(sotLongVal, dvrs, inst->env.iBufSize);
  //LongBuf bufRes;
  bufRes.Size(soRes->iSize());
  if (fCol)
    for (int i=0; i<bufRes.iSize(); i++)
      bufRes[i] = gr->rcConv(bufCrd[i]).Col+1;
  else
    for (int i=0; i<bufRes.iSize(); i++)
      bufRes[i] = gr->rcConv(bufCrd[i]).Row+1;
  soRes->PutVal(bufRes);
  delete soCrd;
  inst->stkCalc.push(soRes);
}

void InstMapLineCol::GetMapUsed(Map& m)
{
  mp = m;
}

void InstMapLineCol::ReplaceMapUsed(const Map& m)
{
  if (mp.fValid())
    mp = m;
}

void InstMapLineCol::AddDependentObjects(ObjectDependency& objdep)
{
  objdep.Add(mp);
}

InstMapCrd::InstMapCrd(Instructions* instrucs, const Map& m)
  : InstBase(instrucs)
{
  dvsCoord = DomainValueRangeStruct(Domain(FileName(), m->cs()));
  gr = m->gr();
  mp = m;
}

InstMapCrd::~InstMapCrd()
{
}

void InstMapCrd::Exec()
{
  StackObject* soCol = inst->stkCalc.pop();
  StackObject* soRow = inst->stkCalc.pop();
//  LongBuf bufRow, bufCol;
  soRow->Resize(bufRow);
  soRow->GetVal(bufRow);
  soCol->Resize(bufCol);
  soCol->GetVal(bufCol);
  StackObject* soRes = new StackObject(sotCoordVal, dvsCoord, inst->env.iBufSize);
//  CoordBuf bufRes;
  bufRes.Size(soRes->iSize());
  for (int i=0; i<bufRes.iSize(); i++)
    bufRes[i] = gr->cConv(RowCol(bufRow[i]-1, bufCol[i]-1));
  soRes->PutVal(bufRes);
  delete soRow;
  delete soCol;
  inst->stkCalc.push(soRes);
}

void InstMapCrd::GetMapUsed(Map& m)
{
  mp = m;
}

void InstMapCrd::ReplaceMapUsed(const Map& m)
{
  if (mp.fValid())
    mp = m;
}

void InstMapCrd::AddDependentObjects(ObjectDependency& objdep)
{
  objdep.Add(mp);
}

InstMapColor::InstMapColor(Instructions* instrucs, const Map& m)
  : InstBase(instrucs), mp(m), fColor(false)
{
  DomainColor* pdcol = mp->dm()->pdcol();
  if (0 != pdcol)
    fColor = true;
  else
    rpr = mp->dm()->rpr();
  dvsColor = DomainValueRangeStruct(Domain("color"));
}

InstMapColor::~InstMapColor()
{
}

void InstMapColor::Exec()
{
  StackObject* soCol = inst->stkCalc.pop();
  StackObject* soRow = inst->stkCalc.pop();
//  LongBuf bufRow, bufCol;
  soRow->Resize(bufRow);
  soRow->GetVal(bufRow);
  soCol->Resize(bufCol);
  soCol->GetVal(bufCol);
  StackObject* soRes = new StackObject(sotLongRaw, dvsColor, inst->env.iBufSize);
//  LongBuf bufRes;
  bufRes.Size(soRes->iSize());
  if (rpr.fValid() && mp->fValues()) 
  {
    RangeReal rrMinMax=mp->rrPerc1(true);
    if ( !rrMinMax.fValid())
    {
        rrMinMax=mp->dm()->pdv()->rrMinMax();
    }
//    RealBuf bufVal;
    bufVal.Size(bufRes.iSize());
    int i;
    for (i=0; i<bufRes.iSize(); i++)
      bufVal[i] = mp->rValue(RowCol(bufRow[i]-1, bufCol[i]-1));
    for (i=0; i < bufRes.iSize(); i++)
    {
        if (rpr->prv()) bufRes[i] = rpr->clr(bufVal[i]);
        else if (rpr->prg()) bufRes[i] = rpr->clr(bufVal[i], rrMinMax);
    }
  }  
  else {
    for (int i=0; i<bufRes.iSize(); i++)
      bufRes[i] = mp->iRaw(RowCol(bufRow[i]-1, bufCol[i]-1));
    if (rpr.fValid()) {
      for (int i=0; i < bufRes.iSize(); i++)
        bufRes[i] = rpr->clrRaw(bufRes[i]);
    }
    else if (!fColor) {
      for (int i=0; i < bufRes.iSize(); i++) {
        byte b = byteConv(bufRes[i]);
        bufRes[i] = Color(b, b, b);
      }
    }
  }
  soRes->PutRaw(bufRes);
  delete soRow;
  delete soCol;
  inst->stkCalc.push(soRes);
}

void InstMapColor::GetMapUsed(Map& m)
{
  mp = m;
}

void InstMapColor::ReplaceMapUsed(const Map& m)
{
  if (mp.fValid())
    mp = m;
}

void InstMapColor::AddDependentObjects(ObjectDependency& objdep)
{
  objdep.Add(mp);
  if (rpr.fValid())
    objdep.Add(rpr);
}

InstRprColor::InstRprColor(Instructions* instrucs, const Representation& rp)
  : InstBase(instrucs), rpr(rp)
{
  dvsColor = DomainValueRangeStruct(Domain("color"));
  fRprClass = 0 != rpr->prc();
}

InstRprColor::~InstRprColor()
{
}

void InstRprColor::Exec()
{
  StackObject* soVal = inst->stkCalc.pop();
  StackObject* soRes = new StackObject(sotLongRaw, dvsColor, inst->env.iBufSize);
//  LongBuf bufRes;
  bufRes.Size(soRes->iSize());
  int i;
  switch (soVal->sot) {
    case sotStringVal: {
  //      StringBuf bufVal;
        bufValS.Size(bufRes.iSize());
        soVal->GetVal(bufValS);
        for (i=0; i < bufRes.iSize(); i++)
          bufRes[i] = rpr->clrRaw(rpr->dm()->iRaw(bufValS[i]));
      }
      break;
    case sotRealVal: case sotLongVal: {
        if (fRprClass) {
//          LongBuf bufVal;
          bufValL.Size(bufRes.iSize());
          soVal->GetVal(bufValL);
          for (i=0; i < bufRes.iSize(); i++)
            bufRes[i] = rpr->clrRaw(bufValL[i]);
        }
        else {
//          RealBuf bufVal;
          bufValR.Size(bufRes.iSize());
          soVal->GetVal(bufValR);
          for (i=0; i < bufRes.iSize(); i++)
            bufRes[i] = rpr->clr(bufValR[i]);
        }
      }
      break;
    case sotLongRaw: {
//        LongBuf bufRaw;
        bufValL.Size(bufRes.iSize());
        soVal->GetRaw(bufValL);
        for (i=0; i < bufRes.iSize(); i++)
          bufRes[i] = rpr->clrRaw(bufValL[i]);
      }
      break;
    default: {
        for (i=0; i < bufRes.iSize(); i++)
          bufRes[i] = 0;
      }
      break;
  }
  soRes->PutRaw(bufRes);
  delete soVal;
  inst->stkCalc.push(soRes);
}

void InstRprColor::AddDependentObjects(ObjectDependency& objdep)
{
  objdep.Add(rpr);
}

InstPntCrd::InstPntCrd(Instructions* instrucs, const PointMap& m)
  : InstBase(instrucs)
{
  dvsCoord = DomainValueRangeStruct(Domain(FileName(), m->cs()));
  pmp = m;
}

InstPntCrd::~InstPntCrd()
{
}

void InstPntCrd::Exec()
{
  StackObject* soIndex = inst->stkCalc.pop();
//LongBuf bufIndex;
  soIndex->Resize(bufIndex);
  if ((soIndex->sot == sotStringVal) || (soIndex->sot == sotLongRaw)) {
    StringBuf bufStrIndex;
    soIndex->Resize(bufStrIndex);
    soIndex->GetVal(bufStrIndex);
    for (int i=0; i<bufIndex.iSize(); i++)
      bufIndex[i] = pmp->iRec(bufStrIndex[i]);
  }
  else 
	{
    soIndex->GetVal(bufIndex);
//    for (int i=0; i<bufIndex.iSize(); i++) // correct between raw and rec
//      bufIndex[i] -= 1;
  }
  StackObject* soRes = new StackObject(sotCoordVal, dvsCoord, inst->env.iBufSize);
//  CoordBuf bufRes;
  bufRes.Size(soRes->iSize());
  for (int i=0; i<bufRes.iSize(); i++)
    bufRes[i] = pmp->cValue(bufIndex[i]);
  soRes->PutVal(bufRes);
  delete soIndex;
  inst->stkCalc.push(soRes);
}

void InstPntCrd::AddDependentObjects(ObjectDependency& objdep)
{
  objdep.Add(pmp);
}

InstPntVal::InstPntVal(Instructions* instrucs, const PointMap& m)
  : InstBase(instrucs)
{
  pmp = m;
  sot = StackObject::sotype(pmp->dvrs());
}

InstPntVal::~InstPntVal()
{
}

void InstPntVal::Exec()
{
  StackObject* soIndex = inst->stkCalc.pop();
//  LongBuf bufIndex;
  soIndex->Resize(bufIndex);
  if ((soIndex->sot == sotStringVal) || (soIndex->sot == sotLongRaw)) {
    StringBuf bufStrIndex;
    soIndex->Resize(bufStrIndex);
    soIndex->GetVal(bufStrIndex);
    for (int i=0; i<bufIndex.iSize(); i++)
      bufIndex[i] = pmp->iRec(bufStrIndex[i]);
  }
  else
    soIndex->GetVal(bufIndex);
  StackObject* soRes = new StackObject(sot, pmp->dvrs(), inst->env.iBufSize);
  switch (sot)  {
    case sotLongRaw : {
  //      LongBuf bufRes;
        bufResL.Size(soRes->iSize());
        for (int i=0; i<bufResL.iSize(); i++)
          bufResL[i] = pmp->iRaw(bufIndex[i]);
        soRes->PutRaw(bufResL);
      }
      break;
    case sotLongVal : {
//        LongBuf bufRes;
        bufResL.Size(soRes->iSize());
        for (int i=0; i<bufResL.iSize(); i++)
          bufResL[i] = pmp->iValue(bufIndex[i]);
        soRes->PutVal(bufResL);
      }
      break;
    case sotRealVal : {
//        RealBuf bufRes;
        bufResR.Size(soRes->iSize());
        for (int i=0; i<bufResR.iSize(); i++)
          bufResR[i] = pmp->rValue(bufIndex[i]);
        soRes->PutVal(bufResR);
      }
      break;
    case sotStringVal : {
//        StringBuf bufRes;
        bufResS.Size(soRes->iSize());
        for (int i=0; i<bufResS.iSize(); i++)
          bufResS[i] = pmp->sValue(bufIndex[i]);
        soRes->PutVal(bufResS);
      }
      break;
    case sotCoordVal : {
//        CoordBuf bufRes;
        bufResC.Size(soRes->iSize());
        for (int i=0; i<bufResC.iSize(); i++)
          bufResC[i] = pmp->cValue(bufIndex[i]);
        soRes->PutVal(bufResC);
      }
      break;
    default : {
//        LongBuf bufRes;
        bufResL.Size(soRes->iSize());
        for (int i=0; i<bufResL.iSize(); i++)
          bufResL[i] = iUNDEF;
        soRes->PutRaw(bufResL);
      }
      break;
  }
  delete soIndex;
  inst->stkCalc.push(soRes);
}

void InstPntVal::AddDependentObjects(ObjectDependency& objdep)
{
  objdep.Add(pmp);
}

InstPntNr::InstPntNr(Instructions* instrucs, const PointMap& m)
  : InstBase(instrucs)
{
  pmp = m;
  dvrs = DomainValueRangeStruct(1, pmp->iFeatures());
}

InstPntNr::~InstPntNr()
{
}

void InstPntNr::Exec()
{
  StackObject* soIndex = inst->stkCalc.pop();
//LongBuf bufRes;
  soIndex->Resize(bufRes);
  if ((soIndex->sot == sotStringVal) || (soIndex->sot == sotLongRaw)) {
//    StringBuf bufStrIndex;
    soIndex->Resize(bufStrIndex);
    soIndex->GetVal(bufStrIndex);
    for (int i=0; i<bufRes.iSize(); i++)
      bufRes[i] = pmp->iRec(bufStrIndex[i]);
  }
  else
    soIndex->GetVal(bufRes);
  StackObject* soRes = new StackObject(sotLongVal, dvrs, inst->env.iBufSize);
  soRes->PutVal(bufRes);
  delete soIndex;
  inst->stkCalc.push(soRes);
}

void InstPntNr::AddDependentObjects(ObjectDependency& objdep)
{
  objdep.Add(pmp);
}

InstInMask::InstInMask(Instructions* instrucs)
  : InstBase(instrucs)
{
  dvsBool = DomainValueRangeStruct(Domain("bool"));
}

InstInMask::~InstInMask()
{
}

void InstInMask::Exec()
{
  StackObject* soMask = inst->stkCalc.pop();
  StackObject* soString = inst->stkCalc.pop();
//  StringBuf bufMask, bufStr;
  soMask->Resize(bufMask);
  soString->Resize(bufStr);
  soMask->GetVal(bufMask);
  soString->GetVal(bufStr);
  StackObject* soRes = new StackObject(sotLongVal, dvsBool, inst->env.iBufSize);
//  LongBuf bufRes;
  soRes->Resize(bufRes);
  for (int i=0; i<bufRes.iSize(); i++) {
    if (bufStr[i] == sUNDEF) {
      bufRes[i] = iUNDEF;
      continue;
    }
    if (msk.sMask() != bufMask[i])
      msk.SetMask(bufMask[i]);
    bufRes[i] = msk.fInMask(bufStr[i]);
  }
  soRes->PutVal(bufRes);
  delete soMask;
  delete soString;
  inst->stkCalc.push(soRes);
}

InstTblValue::InstTblValue(Instructions* instrucs, const Table& _tbl) :
	InstBase(instrucs),
	tbl(_tbl)
{
}

void InstTblValue::AddDependentObjects(ObjectDependency& objdep)
{
  objdep.Add(tbl);
}

void InstTblValue::Exec()
{
  StackObject* soRow = inst->stkCalc.pop();	
  StackObject* soCol = inst->stkCalc.pop();
//  LongBuf bufRow, bufCol;
	DomainSort *dmsrt = tbl->dm()->pdsrt();
	soRow->Resize(sbufRow);
	soRow->GetVal(sbufRow);
  soRow->Resize(bufRow);
  soRow->GetVal(bufRow);
	
  soCol->Resize(bufCol);
  soCol->GetVal(bufCol);
	Column col = tbl->col(bufCol[0]);	
	if ( !col.fValid())
		throw ErrorObject(String(TR("Invalid column %S").c_str(), bufCol[0]));

	long iKey = iUNDEF;
	if ( dmsrt)
	{
  	if ( bufRow[0] == iUNDEF )
			iKey = dmsrt->iKey(dmsrt->iOrd(sbufRow[0]));
		else
			iKey = dmsrt->iKey(bufRow[0]);

	}		
	else
		iKey = bufRow[0];

	sot = StackObject::sotype(col->dm());	
  StackObject* soRes = new StackObject(sot, col->dvrs(), inst->env.iBufSize);
  switch (sot)  {
    case sotLongRaw : {
//        LongBuf bufResL;
			  bufResL.Size(soRes->iSize());
				if ( bufResL.iSize() == 0 )
					bufResL.Size(1);
				for ( int v = 0; v < bufResL.iSize(); ++v)
					bufResL[v] = col->iRaw(iKey);
        soRes->PutRaw(bufResL);
      }
      break;
    case sotLongVal : {
  //      LongBuf bufRes;
			  bufResL.Size(soRes->iSize());			
 				if ( bufResL.iSize() == 0 )
					bufResL.Size(1);
				for ( int v = 0; v < bufResL.iSize(); ++v)
					bufResL[v] = col->iValue(iKey);
        soRes->PutVal(bufResL);
      }
      break;
    case sotRealVal : {
//        RealBuf bufRes;
			  bufResR.Size(soRes->iSize());			
				if ( bufResR.iSize() == 0 )
					bufResR.Size(1);
				for ( int v = 0; v < bufResR.iSize(); ++v)
					bufResR[v] = col->rValue(iKey);		
        soRes->PutVal(bufResR);
      }
      break;
    case sotStringVal : 
			{
//        StringBuf bufRes;
			  bufResS.Size(soRes->iSize());				
				if ( bufResS.iSize() == 0 )
					bufResS.Size(1);
				for ( int v = 0; v < bufResS.iSize(); ++v)
					bufResS[v] = col->sValue(iKey);
        soRes->PutVal(bufResS);
      }
      break;
  }
  delete soRow;
  delete soCol;
  inst->stkCalc.push(soRes);
}
