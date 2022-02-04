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
/* $Log: /ILWIS 3.0/Calculator/CALCSTCK.cpp $
 * 
 * 5     11/05/01 11:33a Martin
 * colors are now accept as "raw" values.
 * 
 * 4     14-07-00 15:02 Koolhoven
 * header comment was still wrong
 * 
 * 3     13-07-00 12:33p Martin
 * comment bug
 /* 
 * 2     13-07-00 10:14a Martin
 * sValueByRaw can be used because mask will now accpet codes
// Revision 1.3  1998/09/16 17:30:36  Wim
// 22beta2
//
// Revision 1.2  1997/09/26 17:00:04  Wim
// SatackObject::GetVal(stringbuf) from sotLongRaw
// using a DomainSort use sNameByRaw() instead sValueByRaw()
// to prevent getting "code:name"
//
/* calcstck.c
   Jelle Wind
   october 1995
	Last change:  WK   26 Sep 97    6:58 pm
*/
#define CALCSTCK_C
#include "Engine\Scripting\CALCSTCK.H"
#include "Engine\Scripting\Calcvar.h"
#include "Engine\Domain\Dmvalue.h"
#include "Engine\Domain\dmcoord.h"
#include "Engine\Domain\dmsort.h"
#include "Engine\Base\DataObjects\valrange.h"

StackObject::StackObject(StackObjectType sotp, const DomainValueRangeStruct& dvrs, int iBufSize)
{
  sot = sotp;
  dvs = dvrs;
  if (dvs.dm()->pdi() || dvs.dm()->pdbool())
    dvs.SetValueRange(ValueRange());
  switch (sot) {
    case sotStringVal:
      sBuf = new StringBuf(iBufSize);
      break;
    case sotRealVal:
      rBuf = new RealBuf(iBufSize);
      break;
    case sotLongVal:
      iBuf = new LongBuf(iBufSize);
      break;
    case sotLongRaw:
      iBuf = new LongBuf(iBufSize);
      break;
    case sotCoordVal:
      cBuf = new CoordBuf(iBufSize);
      break;
    default:
      break;
  }
  iSiz = iBufSize;
}

StackObject::StackObject(const Column& column)
{
  sot = sotColumn;
  col = column;
  dvs = column->dvrs();
  iSiz = 0;
}

StackObject::StackObject(const Map& mp)
{
  sot = sotMap;
  map = mp;
  dvs = map->dvrs();
  iSiz = 0;
}
/*
StackObject::StackObject(const StackObject& so)
{
  sot = so.sot;
  dvs = so.dvs;
  iSiz = so.iSiz;
  switch (sot) {
    case sotStringVal:
      sBuf = so.sBuf;
      const_cast<StackObject&>(so).sBuf = 0;
      const_cast<StackObject&>(so).iSiz = 0;
      break;
    case sotRealVal:
      rBuf = so.rBuf;
      const_cast<StackObject&>(so).rBuf = 0;
      const_cast<StackObject&>(so).iSiz = 0;
      break;
    case sotLongRaw:
    case sotLongVal:
      iBuf = so.iBuf;
      const_cast<StackObject&>(so).iBuf = 0;
      const_cast<StackObject&>(so).iSiz = 0;
      break;
    case sotCoordVal:
      cBuf = so.cBuf;
      const_cast<StackObject&>(so).cBuf = 0;
      const_cast<StackObject&>(so).iSiz = 0;
      break;
    case sotColumn:
      col = so.colGet();
      break;
    case sotMap:
      map = so.mapGet();
      break;
    default:
      break;
  }
}*/

StackObject::StackObject(const StackObject& so/*, bool fMoveBuf*/)
{
  sot = so.sot;
  dvs = so.dvs;
  iSiz = so.iSiz;
  switch (sot) {
    case sotStringVal:
/*      if (fMoveBuf) {
        sBuf = so.sBuf;
        so.sBuf = 0;
      }
      else {*/
        sBuf = new StringBuf(iSiz);
        *sBuf = *so.sBuf;
//      }
      break;
    case sotRealVal:
/*      if (fMoveBuf) {
        rBuf = so.rBuf;
        so.rBuf = 0;
      }
      else {*/
        rBuf = new RealBuf(iSiz);
        *rBuf = *so.rBuf;
//      }
      break;
    case sotLongRaw:
    case sotLongVal:
/*      if (fMoveBuf) {
        iBuf = so.iBuf;
        so.iBuf = 0;
      }
      else {*/
        iBuf = new LongBuf(iSiz);
        *iBuf = *so.iBuf;
//      }
      break;
    case sotCoordVal:
/*      if (fMoveBuf) {
        cBuf = so.cBuf;
        so.cBuf = 0;
      }
      else {*/
        cBuf = new CoordBuf(iSiz);
        *cBuf = *so.cBuf;
//      }
      break;
    case sotColumn:
      col = so.colGet();
      break;
    case sotMap:
      map = so.mapGet();
      break;
    default:
      break;
  }
}

StackObject::~StackObject()
{
  switch (sot) {
    case sotStringVal:
      if (sBuf)
        delete sBuf;
      break;
    case sotRealVal:
      if (rBuf) 
        delete rBuf;
      break;
    case sotLongRaw:
    case sotLongVal:
      if (iBuf) 
        delete iBuf;
      break;
    case sotCoordVal:
      if (cBuf) 
        delete cBuf;
      break;
    default:
      break;
  }
}

StackObjectType StackObject::sotype(const DomainValueRangeStruct& dvrs)
{
  CalcVariable cv(dvrs, vtVALUE);
  StackObjectType sot;
  switch (cv->vt) {
    case vtVALUE:
      if (dvrs.fRealValues())
        sot = sotRealVal;
      else  
        sot = sotLongVal;
      break;
    case vtSTRING:
      if (0 != dvrs.dm()->pdsrt())
        sot = sotLongRaw;
      else
        sot = sotStringVal;
      break;
    case vtCOORD:
      sot = sotCoordVal;
      break;
    case vtCOLOR:
      sot = sotLongRaw;
      break;
    case vtMAP:
      if (dvrs.fValues()) {
        if (dvrs.fRealValues())
          sot = sotRealVal;
        else  
          sot = sotLongVal;
      }
      else  
        sot = sotLongRaw;
      break;
    case vtTABLE2:
    case vtCOLUMN:
      if (dvrs.fValues()) {
        if (dvrs.fRealValues())
          sot = sotRealVal;
        else  
          sot = sotLongVal;
      }
      else if (dvrs.dm()->pds())
        sot = sotStringVal;
      else if (dvrs.dm()->pdcrd())
        sot = sotCoordVal;
      else
        sot = sotLongRaw;
      break;
    default :
      sot = sotRealVal;
      break;
  }
  return sot;
}

void StackObject::PutVal(const StringBuf& bf)
{
  switch (sot) {
    case sotStringVal:
      { for (int i = 0; i < sBuf->iSize(); i++)
          (*sBuf)[i] = bf[i];
      }
      break;
    case sotRealVal:
      { for (int i = 0; i < rBuf->iSize(); i++)
          (*rBuf)[i] = bf[i].rVal();
      }
      break;
    case sotLongVal:
      { for (int i = 0; i < iBuf->iSize(); i++)
          (*iBuf)[i] = bf[i].rVal();
      }
      break;
    case sotLongRaw:
      {
        if (dvs.fRawIsValue())
          for (int i = 0; i < iBuf->iSize(); i++)
            (*iBuf)[i] = bf[i].iVal();
        else if (dvs.fValues())
          for (int i = 0; i < iBuf->iSize(); i++)
            (*iBuf)[i] = dvs.iRaw(bf[i].iVal());
        else
          for (int i = 0; i < iBuf->iSize(); i++)
            (*iBuf)[i] = dvs.dm()->iRaw(bf[i]);
      }
      break;
    case sotCoordVal:
      { DomainCoord* dmcrd = dvs.dm()->pdcrd();
        if (0 != dmcrd)
          for (int i = 0; i < cBuf->iSize(); i++)
            (*cBuf)[i] = dmcrd->cValue(bf[i]);
        else
          for (int i = 0; i < cBuf->iSize(); i++)
            (*cBuf)[i] = crdUNDEF;
      }
      break;
    default:
      assert(0==1);
      break;
  }
}

void StackObject::PutVal(const RealBuf& bf)
{
  switch (sot) {
    case sotStringVal:
      { for (int i = 0; i < sBuf->iSize(); i++)
          (*sBuf)[i] = String("%g", bf[i]);
      }
      break;
    case sotRealVal:
      *rBuf = bf;
      break;
    case sotLongVal:
      { for (int i = 0; i < iBuf->iSize(); i++)
          (*iBuf)[i] = longConv(bf[i]);
      }
      break;
    case sotLongRaw:
      {
        if (dvs.fRawIsValue())  
          for (int i = 0; i < iBuf->iSize(); i++)
            (*iBuf)[i] = longConv(bf[i]);
        else if (dvs.fValues())
          for (int i = 0; i < iBuf->iSize(); i++)
            (*iBuf)[i] = dvs.iRaw(bf[i]);
        else
          for (int i = 0; i < iBuf->iSize(); i++)
            (*iBuf)[i] = iUNDEF;
      }
      break;
    default:
      assert(0==1);
      break;
  }
}

void StackObject::PutVal(const LongBuf& bf)
{
  switch (sot) {
    case sotStringVal:
      { for (int i = 0; i < sBuf->iSize(); i++)
          (*sBuf)[i] = String("%li", bf[i]);
      }
      break;
    case sotRealVal:
      { for (int i = 0; i < rBuf->iSize(); i++)
          (*rBuf)[i] = doubleConv(bf[i]);
      }
      break;
    case sotLongVal:
      *iBuf = bf;
      break;
    case sotLongRaw:
      {
        if (dvs.fRawIsValue())
          for (int i = 0; i < iBuf->iSize(); i++)
            (*iBuf)[i] = bf[i];
        else if (dvs.fValues())
          for (int i = 0; i < iBuf->iSize(); i++)
            (*iBuf)[i] = dvs.iRaw(bf[i]);
        else
          for (int i = 0; i < iBuf->iSize(); i++)
            (*iBuf)[i] = iUNDEF;
      }
      break;
    default:
      assert(0==1);
      break;
  }
}

void StackObject::PutRaw(const LongBuf& bf)
{
  switch (sot) {
    case sotStringVal:
      {
        for (int i = 0; i < sBuf->iSize(); i++)
          (*sBuf)[i] = dvs.dm()->sValueByRaw(bf[i]);
      }
      break;
    case sotRealVal:
      {
        if (dvs.fRawIsValue())  
          for (int i = 0; i < rBuf->iSize(); i++)
            (*rBuf)[i] = doubleConv(bf[i]);
        else if (dvs.fValues())
          for (int i = 0; i < rBuf->iSize(); i++)
            (*rBuf)[i] = dvs.rValue(bf[i]);
        else
          for (int i = 0; i < rBuf->iSize(); i++)
            (*rBuf)[i] = rUNDEF;
      }
      break;
    case sotLongVal:
      {
        if (dvs.fRawIsValue())  
          for (int i = 0; i < rBuf->iSize(); i++)
            (*iBuf)[i] = bf[i];
        else if (dvs.fValues())
          for (int i = 0; i < rBuf->iSize(); i++)
            (*iBuf)[i] = dvs.iValue(bf[i]);
        else
          for (int i = 0; i < rBuf->iSize(); i++)
            (*iBuf)[i] = iUNDEF;
      }
      break;
    case sotLongRaw:
      *iBuf = bf;
      break;
    default:
      assert(0==1);
      break;
  }
}

void StackObject::PutVal(const CoordBuf& bf)
{
  switch (sot) {
    case sotCoordVal:
      { const_cast<CoordBuf&>(bf).Size(cBuf->iSize());
        for (int i = 0; i < cBuf->iSize(); i++)
          (*cBuf)[i] = bf[i];
      }
      break;
    default :
      break;
  }
}

void StackObject::GetVal(CoordBuf& bf) const
{
  switch (sot) {
    case sotCoordVal:
      { bf.Size(cBuf->iSize());
        for (int i = 0; i < cBuf->iSize(); i++)
          bf[i] = (*cBuf)[i];
      }
      break;
    default :
      break;
  }
}

void StackObject::GetVal(StringBuf& bf, int iWid, int iDec) const
{
  switch (sot) {
    case sotStringVal:
      { bf.Size(sBuf->iSize());
        for (int i = 0; i < sBuf->iSize(); i++)
          bf[i] = (*sBuf)[i];
      }
      break;
    case sotRealVal:
      { bf.Size(rBuf->iSize());
        for (int i = 0; i < rBuf->iSize(); i++)
          bf[i] = dvs.sValue((*rBuf)[i], iWid, iDec);
      }
      break;
    case sotLongVal:
      { bf.Size(iBuf->iSize());
        for (int i = 0; i < iBuf->iSize(); i++)
          bf[i] = dvs.sValue((*iBuf)[i], iWid);
      }
      break;
    case sotLongRaw:
      { bf.Size(iBuf->iSize());
        if (dvs.fRawIsValue()) {
          long j;
          for (int i = 0; i < iBuf->iSize(); i++) {
            j = (*iBuf)[i];
            if (j == iUNDEF)
              bf[i] = sUNDEF;
            else
              bf[i] = String("%li", j);
          }
        }
        else {
//          DomainSort* ds = dvs.dm()->pdsrt();
//          if (ds)
//            for (int i = 0; i < sBuf->iSize(); i++)
//              bf[i] = ds->sNameByRaw((*iBuf)[i], iWid);
//          else
            for (int i = 0; i < iBuf->iSize(); i++)
              bf[i] = dvs.sValueByRaw((*iBuf)[i], iWid);
        }
      }
      break;
    case sotCoordVal:
      { bf.Size(cBuf->iSize());
        DomainCoord* dmcrd = dvs.dm()->pdcrd();
        for (int i = 0; i < cBuf->iSize(); i++)
          bf[i] = dmcrd->sValue((*cBuf)[i], iWid, iDec);
      }
      break;
    default :
      assert(0==1);
/*      { bf.Size(1);
        bf[0] = sUNDEF;
      }*/
  }
}

void StackObject::GetVal(RealBuf& bf) const
{
  switch (sot) {
    case sotStringVal:
      { bf.Size(sBuf->iSize());
        for (int i = 0; i < sBuf->iSize(); i++)
          bf[i] = (*sBuf)[i].rVal();
      }
      break;
    case sotRealVal:
      bf = *rBuf;
      break;
    case sotLongVal:
      { bf.Size(iBuf->iSize());
        for (int i = 0; i < iBuf->iSize(); i++)
          bf[i] = doubleConv((*iBuf)[i]);
      }
      break;
    case sotLongRaw:
      { bf.Size(iBuf->iSize());
        if (dvs.fRawIsValue())
          for (int i = 0; i < iBuf->iSize(); i++)
            bf[i] = doubleConv((*iBuf)[i]);
        else if (dvs.fValues())  
          for (int i = 0; i < iBuf->iSize(); i++)
            bf[i] = dvs.rValue((*iBuf)[i]);
        else  
          for (int i = 0; i < iBuf->iSize(); i++)
            bf[i] = rUNDEF;
      }
      break;
    case sotCoordVal:
      { bf.Size(iBuf->iSize());
        for (int i = 0; i < iBuf->iSize(); i++)
          bf[i] = rUNDEF;
      }
      break;
    default :
      assert(0==1);
/*      { bf.Size(1);
        bf[0] = rUNDEF;
      }*/
  }
}

void StackObject::GetVal(LongBuf& bf) const
{
  switch (sot) {
    case sotStringVal:
      { bf.Size(sBuf->iSize());
        for (int i = 0; i < sBuf->iSize(); i++)
          bf[i] = longConv((*sBuf)[i].iVal());
      }
      break;
    case sotRealVal:
      { bf.Size(rBuf->iSize());
        for (int i = 0; i < rBuf->iSize(); i++)
          bf[i] = longConv((*rBuf)[i]);
      }
      break;
    case sotLongVal:
      bf = *iBuf;
      break;
    case sotLongRaw:
      { bf.Size(iBuf->iSize());
        if (dvs.fRawIsValue() || dvs.dm()->dmt() == dmtCOLOR )
          for (int i = 0; i < iBuf->iSize(); i++)
            bf[i] = (*iBuf)[i];
        else if (dvs.fValues())
          for (int i = 0; i < iBuf->iSize(); i++)
            bf[i] = dvs.iValue((*iBuf)[i]);
				else					
          for (int i = 0; i < iBuf->iSize(); i++)
            bf[i] = iUNDEF;
      }
      break;
    case sotCoordVal:
      { bf.Size(iBuf->iSize());
        for (int i = 0; i < iBuf->iSize(); i++)
          bf[i] = iUNDEF;
      }
    default :
      assert(0==1);
/*      { bf.Size(1);
        bf[0] = iUNDEF;
      }*/
  }
}

void StackObject::GetRaw(LongBuf& bf) const
{
  switch (sot) {
    case sotStringVal:
      { bf.Size(sBuf->iSize());
        for (int i = 0; i < sBuf->iSize(); i++)
          bf[i] = dvs.dm()->iRaw((*sBuf)[i]);
      }
      break;
    case sotRealVal:
      { bf.Size(rBuf->iSize());
        if (dvs.fRawIsValue())
          for (int i = 0; i < rBuf->iSize(); i++)
            bf[i] = longConv((*rBuf)[i]);
        else if (dvs.fValues())
          for (int i = 0; i < rBuf->iSize(); i++)
            bf[i] = dvs.iRaw((*rBuf)[i]);
        else
          for (int i = 0; i < rBuf->iSize(); i++)
            bf[i] = iUNDEF;
      }
      break;
    case sotLongVal:
      { bf.Size(iBuf->iSize());
        if (dvs.fRawIsValue())
          for (int i = 0; i < iBuf->iSize(); i++)
            bf[i] = (*iBuf)[i];
        else if (dvs.fValues())
          for (int i = 0; i < iBuf->iSize(); i++)
            bf[i] = dvs.iRaw((*iBuf)[i]);
        else  
          for (int i = 0; i < iBuf->iSize(); i++)
            bf[i] = iUNDEF;
      }
      break;
    case sotLongRaw:
      { bf = *iBuf;
      }
      break;
    case sotCoordVal:
      { bf.Size(iBuf->iSize());
        for (int i = 0; i < iBuf->iSize(); i++)
          bf[i] = iUNDEF;
      }
    default :
      assert(0==1);
/*      { bf.Size(1);
        bf[0] = iUNDEF;
      }*/
  }
}




