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
// $Log: /ILWIS 3.0/Calculator/Instruc1.cpp $
 * 
 * 20    20-03-02 18:40 Koolhoven
 * InstLoad::Exec() for columns when fDomainSortConvert allow highest
 * possible value, do not make it undef. (for Maps do not allow the
 * illegal 0)
 * 
 * 19    14-11-00 9:07a Martin
 * added pragma 4786 and removed the delete for the cvt for 2dimtables.
 * The delete will give an error because a second member of the class
 * already holds the pointer to the object. When this goes out of scope it
 * will clean up. The delete is not needed and will give an error
 * 
 * 18    28-09-00 2:56p Martin
 * instgoto use tranquilzier to prevent endless loops
 * 
 * 17    4-09-00 12:52p Martin
 * extended a bug solution to include reals 
 * 
 * 16    12-07-00 8:20a Martin
 * solved several bugs in the calculator
 * 
 * 15    14-02-00 16:13 Wind
 * added functions to retrieve dependent objects from instructions (bug
 * 404)
 * 
 * 14    10/02/00 12:08 Willem
 * Changed call to rprLoad() into rpr()
 * 
 * 13    17-01-00 15:57 Wind
 * cosmetic changes
 * 
 * 12    10-01-00 17:33 Wind
 * removed two static summy variables and changed interface of
 * CalcVariablePtr::col() and map()
 * 
 * 11    5-01-00 18:12 Wind
 * some cosmetic changes (renaming of some functions, removing of others)
 * 
 * 10    5-01-00 11:09 Wind
 * rename class ExternFuncDesc to UserDefFuncDesc
 * 
 * 9     4-01-00 10:28 Wind
 * partial redesign and simplification to solve some bugs related with
 * user defined functions
 * 
 * 8     22-12-99 17:50 Wind
 * removed GroupFunction and added functionality to DomainGroup
 * 
 * 7     10-12-99 12:47 Wind
 * made aiConvertRaw in InstLoad zero based index array
 * 
 * 6     25-11-99 13:16 Wind
 * bug 954
 * 
 * 5     10/18/99 9:35a Wind
 * bugs with params in user defined functions
 * 
 * 4     8/20/99 11:13a Wind
 * problems with comments and source save comment
 * 
 * 3     8/20/99 11:11a Wind
 * see  previous
 * 2     8/20/99 10:46a Wind
 * - removed bug from void InstLoad::GetMapUsed(Map& m) caused by
 * port to 3.0 (replaced mp=m; by m=mp;)
 * - removed code for fNbPosInstUsed in InstLoad:Exec() , because variable
 * was not initialized (need more study)
 */
// Revision 1.6  1998/09/16 17:30:36  Wim
// 22beta2
//
// Revision 1.5  1997/09/17 09:24:57  Wim
// Only allow using of raw values in InstConstString
// if they will not become undefined
//
// Revision 1.4  1997-09-08 20:22:43+02  Wim
// delete so in InstStore::Exec() after using it after the pop()
//
// Revision 1.3  1997-08-21 11:33:54+02  Wim
// In InstLoad::Exec() when calculating with bufR do also only use
// the size of bufR (errors with neighbour functions)
//
// Revision 1.2  1997-07-25 12:25:52+02  Wim
// Latest changes of Jelle
//
/* instruc1.c
// Ilwis 2.0 instructions for calculator (1)
// sept. 1994, Jelle Wind
	Last change:  WK    2 Jul 98    3:13 pm
*/
#define INSTRUC_C

#pragma warning( disable : 4786 )

#include "Engine\Scripting\Instrucs.h"
#include "Engine\Map\Raster\MAPSTORE.H"
#include "Engine\Table\COLSTORE.H"
#include "Engine\Scripting\CALCSTCK.H"
#include "Engine\Scripting\Calcvar.h"
#include "Engine\Domain\dmcoord.h"
#include "Engine\Domain\dmgroup.h"
#include "Engine\Domain\dmpict.h"
#include "Engine\Domain\groupfnc.h"

InstBase::InstBase(Instructions* instruc)
{ 
  inst = instruc; 
}

InstBase::~InstBase()
{
}


void InstBase::GetMapUsed(Map& mp)
{
  mp = Map();
}

void InstBase::ReplaceMapUsed(const Map&)
{
}

void InstBase::AddDependentObjects(ObjectDependency&)
{
}

InstConstInt::InstConstInt(Instructions* instrucs, long iValue)
  : InstBase(instrucs)
{
  cv.SetPointer(new CalcVarConstInt(iValue));
}

InstConstInt::~InstConstInt()
{
//  delete cv;
}

void InstConstInt::Exec() {
  StackObject* so = new StackObject(sotLongVal, cv->dvs, inst->env.iBufSize);
  long iVal = cv->iValue();
  for (long i=0; i < inst->env.iBufSize; i++)
    (*so->iBuf)[i] = iVal;
  inst->stkCalc.push(so);
}

InstConstReal::InstConstReal(Instructions* instrucs, double rValue, const String& sVal)
 : InstBase(instrucs)
{
  cv.SetPointer(new CalcVarConstReal(rValue, sVal));
}

InstConstReal::~InstConstReal()
{
//  delete cv;
}

void InstConstReal::Exec() {
  StackObject* so = new StackObject(sotRealVal, cv->dvs, inst->env.iBufSize);
  double rVal = cv->rValue();
  for (long i=0; i < inst->env.iBufSize; i++)
    (*so->rBuf)[i] = rVal;
  inst->stkCalc.push(so);
}

InstConstString::InstConstString(Instructions* instrucs, const String& sValue)
: InstBase(instrucs)
{
  cv.SetPointer(new CalcVarConstString(sValue, this));
}

InstConstString::~InstConstString()
{
//  delete cv;
}

void InstConstString::Exec()
{
  String sVal = cv->sValue();
  StackObjectType sot = sotLongRaw;
  if (cv->dvs.dm()->pds())
    sot = sotStringVal;
  else {
    long iRaw = cv->dvs.iRaw(sVal);
    if (iUNDEF == iRaw && sVal != "?") {
      Domain dm("String");
      cv->SetDomainValueRangeStruct(dm);
      sot = sotStringVal;
    }
  }
  StackObject* so = new StackObject(sot, cv->dvs, inst->env.iBufSize);
  if (sot == sotLongRaw) {
    long iRaw = cv->dvs.iRaw(sVal);
    if ((iRaw == iUNDEF) && (sVal != sUNDEF))
      iRaw = -1;
    for (long i=0; i < inst->env.iBufSize; i++)
      (*so->iBuf)[i] = iRaw;
  }
  else {
    for (long i=0; i < inst->env.iBufSize; i++)
      (*so->sBuf)[i] = sVal;
  }
  inst->stkCalc.push(so);
}

InstConstCoord::InstConstCoord(Instructions* instrucs, const Coord& cValue, const Domain& dm)
: InstBase(instrucs)
{
  cv.SetPointer(new CalcVarConstCoord(cValue, dm));
}

InstConstCoord::~InstConstCoord()
{
//  delete cv;
}

void InstConstCoord::Exec() {
  StackObject* so = new StackObject(sotCoordVal, cv->dvs, inst->env.iBufSize);
  Coord cVal = cv->cValue();
  for (long i=0; i < inst->env.iBufSize; i++)
    (*so->cBuf)[i] = cVal;
  inst->stkCalc.push(so);
}

InstColor::InstColor(Instructions* instrucs)
: InstBase(instrucs)
{
  dvsColor = DomainValueRangeStruct(Domain("color"));
}

InstColor::~InstColor()
{
}

void InstColor::Exec() {
  StackObject* so = new StackObject(sotLongRaw, dvsColor, inst->env.iBufSize);
//  LongBuf bufRed, bufGreen, bufBlue;
  inst->stkCalc[2]->Resize(bufRed);
  inst->stkCalc[2]->GetVal(bufRed);
  inst->stkCalc[1]->Resize(bufGreen);
  inst->stkCalc[1]->GetVal(bufGreen);
  inst->stkCalc[0]->Resize(bufBlue);
  inst->stkCalc[0]->GetVal(bufBlue);
  for (long i=0; i < inst->env.iBufSize; i++)
    (*so->iBuf)[i] = Color(byteConv(bufRed[i]), byteConv(bufGreen[i]), byteConv(bufBlue[i]));
  delete inst->stkCalc.pop();
  delete inst->stkCalc.pop();
  delete inst->stkCalc.pop();
  inst->stkCalc.push(so);
}

InstColorHSI::InstColorHSI(Instructions* instrucs)
: InstBase(instrucs)
{
  dvsColor = DomainValueRangeStruct(Domain("color"));
}

InstColorHSI::~InstColorHSI()
{
}

void InstColorHSI::Exec() {
  StackObject* so = new StackObject(sotLongRaw, dvsColor, inst->env.iBufSize);
//  LongBuf bufHue, bufSat, bufInt;
  inst->stkCalc[2]->Resize(bufHue);
  inst->stkCalc[2]->GetVal(bufHue);
  inst->stkCalc[1]->Resize(bufSat);
  inst->stkCalc[1]->GetVal(bufSat);
  inst->stkCalc[0]->Resize(bufInt);
  inst->stkCalc[0]->GetVal(bufInt);
  for (long i=0; i < inst->env.iBufSize; i++) {
    Color c;
    c.SetHSI(byteConv(bufHue[i]), byteConv(bufSat[i]), byteConv(bufInt[i]));
    (*so->iBuf)[i] = c;
  }
  delete inst->stkCalc.pop();
  delete inst->stkCalc.pop();
  delete inst->stkCalc.pop();
  inst->stkCalc.push(so);
}

InstCoord::InstCoord(Instructions* instrucs, const Domain& _dm)
: InstBase(instrucs)
{
  dvs = DomainValueRangeStruct(_dm);
}

InstCoord::~InstCoord()
{
}

void InstCoord::Exec() {
  StackObject* so = new StackObject(sotCoordVal, dvs, inst->env.iBufSize);
  //RealBuf bufX, bufY;
  inst->stkCalc[1]->Resize(bufX);
  inst->stkCalc[1]->GetVal(bufX);
  inst->stkCalc[0]->Resize(bufY);
  inst->stkCalc[0]->GetVal(bufY);
  for (long i=0; i < inst->env.iBufSize; i++)
    (*so->cBuf)[i] = Coord(bufX[i], bufY[i]);
  delete inst->stkCalc.pop();
  delete inst->stkCalc.pop();
  inst->stkCalc.push(so);
}

InstTransformCoord::InstTransformCoord(Instructions* instrucs, const CoordSystem& _csyOld, const CoordSystem& _csyNew)
: InstBase(instrucs)
{ 
  csyOld = _csyOld; csyNew = _csyNew;
  dm = Domain(FileName(), csyNew);
}

InstTransformCoord::~InstTransformCoord()
{
}

void InstTransformCoord::Exec() {
//  StackObject* soRes = new StackObject(sotCoordVal, dm, inst->env.iBufSize);
//  CoordBuf bufCrd;
  StackObject* so = inst->stkCalc.top();
  if (csyOld->fUnknown()) {
    DomainCoord* dmcrd = so->dvs.dm()->pdcrd();
    if (0 != dmcrd)
      csyOld = dmcrd->cs();
  }
  so->dvs = dm;
//  so->GetVal(bufCrd);
  for (long i=0; i < inst->env.iBufSize; i++)
    (*so->cBuf)[i] = csyNew->cConv(csyOld, (*so->cBuf)[i]);
//  delete so;
//  inst->stkCalc.push(soRes);
}

void InstTransformCoord::AddDependentObjects(ObjectDependency& objdep)
{
  objdep.Add(csyOld);
  objdep.Add(csyNew);
}

InstCurrLine::InstCurrLine(Instructions* instrucs)
: InstBase(instrucs)
{
  dvs = DomainValueRangeStruct(1, LONG_MAX);
}

void InstCurrLine::Exec() {
  StackObject* so = new StackObject(sotLongVal, dvs, inst->env.iBufSize);
  for (long i=0; i < inst->env.iBufSize; i++)
    (*so->iBuf)[i] = inst->env.iCurLine+1;
  inst->stkCalc.push(so);
}

InstCurrColumn::InstCurrColumn(Instructions* instrucs)
: InstBase(instrucs)
{
  dvs = DomainValueRangeStruct(1, LONG_MAX);
}

void InstCurrColumn::Exec() {
  StackObject* so = new StackObject(sotLongVal, dvs, inst->env.iBufSize);
  for (long i=0; i < inst->env.iBufSize; i++)
    (*so->iBuf)[i] = inst->env.iCurCol+i+1;
  inst->stkCalc.push(so);
}

InstCurrRecord::InstCurrRecord(Instructions* instrucs, const Domain & dm)
: InstBase(instrucs)
{
  pdsrt = dm->pdsrt();
  dvs = DomainValueRangeStruct(0, LONG_MAX);
}

void InstCurrRecord::Exec() {
  StackObject* so = new StackObject(sotLongVal, dvs, inst->env.iBufSize);
  if (0 == pdsrt)
    for (long i=0; i < inst->env.iBufSize; i++)
      (*so->iBuf)[i] = inst->env.iCurRec+i;
  else
    for (long i=0; i < inst->env.iBufSize; i++)
      (*so->iBuf)[i] = pdsrt->iOrd(inst->env.iCurRec+i);
  inst->stkCalc.push(so);
}

InstCurrKey::InstCurrKey(Instructions* instrucs, const Domain & dm)
: InstBase(instrucs)
{
  dvs = DomainValueRangeStruct(dm);
}

void InstCurrKey::Exec() {
  StackObject* so;
  if (dvs.fValues()) {
    so = new StackObject(sotLongVal, dvs, inst->env.iBufSize);
    for (long i=0; i < inst->env.iBufSize; i++)
      (*so->iBuf)[i] = inst->env.iCurRec+i;
  }
  else {
    so = new StackObject(sotLongRaw, dvs, inst->env.iBufSize);
    for (long i=0; i < inst->env.iBufSize; i++)
      (*so->iBuf)[i] = inst->env.iCurRec+i;
  }
  inst->stkCalc.push(so);
}

InstCurrCoord::InstCurrCoord(Instructions* instrucs)
: InstBase(instrucs)
{
  dvs = DomainValueRangeStruct(Domain(FileName(), inst->gr()->cs()));
}

void InstCurrCoord::Exec() {
  StackObject* so = new StackObject(sotCoordVal, dvs, inst->env.iBufSize);
  for (long i=0; i < inst->env.iBufSize; i++)
    (*so->cBuf)[i] = inst->gr()->cConv(RowCol(inst->env.iCurLine,inst->env.iCurCol+i));
  inst->stkCalc.push(so);
}

InstCurrCoordX::InstCurrCoordX(Instructions* instrucs)
: InstBase(instrucs)
{
}

void InstCurrCoordX::Exec() {
  StackObject* so = new StackObject(sotRealVal, inst->dvrsValue, inst->env.iBufSize);
  for (long i=0; i < inst->env.iBufSize; i++)
    (*so->rBuf)[i] = inst->gr()->cConv(RowCol(inst->env.iCurLine,inst->env.iCurCol+i)).x;
  inst->stkCalc.push(so);
}

InstCurrCoordY::InstCurrCoordY(Instructions* instrucs)
: InstBase(instrucs)
{
}

void InstCurrCoordY::Exec() {
  StackObject* so = new StackObject(sotRealVal, inst->dvrsValue, inst->env.iBufSize);
  for (long i=0; i < inst->env.iBufSize; i++)
    (*so->rBuf)[i] = inst->gr()->cConv(RowCol(inst->env.iCurLine,inst->env.iCurCol+i)).y;
  inst->stkCalc.push(so);
}

InstLoad::InstLoad(Instructions* instrucs, const CalcVariable& cvar, bool fNeighb)
: InstBase(instrucs)
{
  cv = cvar;
  DomainPicture* pdp = cv->dvs.dm()->pdp();
  if (0 != pdp)
    rpr = pdp->rpr();
  fDomainSortConvert = false;
  fNb = fNeighb;
  mp = cv->map();
  col = cv->col();
}

InstLoad::~InstLoad()
{
}

void InstLoad::InitDomainConvert()
{
  DomainSort * pdsrtTo = cv->dvs.dm()->pdsrt();
  if (0 == pdsrtTo)
    return;
  DomainSort* pdsrtFrom=0;
  if (cv->vt == vtCOLUMN)
    pdsrtFrom = cv->col()->dm()->pdsrt();
  else if (cv->vt == vtMAP)
    pdsrtFrom = cv->map()->dm()->pdsrt();
  if (0 == pdsrtFrom)
    return;
  aiConvertRaw.Resize(pdsrtFrom->iSize()); // offset is 1
  for (long i = 1; i <= aiConvertRaw.iSize(); i++)
    aiConvertRaw[i-1] = pdsrtTo->iRaw(pdsrtFrom->sNameByRaw(i,0));
  fDomainSortConvert = true;
}

void InstLoad::Exec() {
  StackObject* so = new StackObject(cv->sot, cv->dvs, inst->env.iBufSize);
  switch (so->sot) {
    case sotLongVal:
      { //LongBuf buf;
        so->Resize(bufL);
        switch (cv->vt) {
          case vtVALUE:
            { long iVal = cv->iValue();
              for (int i=0; i < bufL.iSize(); i++)
                bufL[i] = iVal;
            }
            break;
          case vtCOLUMN:
            col->GetBufVal(bufL, inst->env.iCurRec, bufL.iSize());
            break;
          case vtMAP:
            { long iRow = inst->env.iCurLine;
              long iCol = inst->env.iCurCol;
              if (fNb && (inst->env.iCurrNb >= 0)) {
                iRow += (inst->env.iCurrNb / 3) - 1;
                iCol += (inst->env.iCurrNb % 3) - 1;
//                if (inst->fNbPosInstUsed) {
                  if ((iRow < 0) || (iRow >= mp->iLines()))
                    for (int i=0; i < bufL.iSize(); i++)
                      bufL[i] = iUNDEF;
/*                }
                else {
                  if (iRow < 0)
                    iRow =0;
                  else if (iRow >= mp->iLines())
                    iRow = mp->iLines()-1;
                }*/
              }
              if (bufL.iSize() == 1)
                bufL[0] = mp->iValue(RowCol(iRow, iCol));
              else {
                mp->GetLineVal(iRow, bufL, iCol, bufL.iSize());
                if (fNb) {
//                  if (inst->fNbPosInstUsed) {
                    if (iCol < 0)
                      bufL[0] = iUNDEF;
                    else if ((iCol + bufL.iSize()) > mp->iCols())
                      bufL[bufL.iSize()-1] = iUNDEF;
/*                  }
                  else {
                    if (iCol < 0)
                      bufL[0] = bufL[1];
                    else if ((iCol + bufL.iSize()) > mp->iCols())
                      bufL[bufL.iSize()-1] = bufL[bufL.iSize()-2];
                  }*/
                }
              }
            }
            break;
          default: assert (0 == 1);
        } // switch
        so->PutVal(bufL);
      }
      break;
    case sotRealVal:
      { //RealBuf buf;
        so->Resize(bufR);
        switch (cv->vt) {
          case vtVALUE:
            { double rVal = cv->rValue();
              for (int i=0; i < bufR.iSize(); i++)
                bufR[i] = rVal;
            }
            break;
          case vtCOLUMN:
            col->GetBufVal(bufR, inst->env.iCurRec, bufR.iSize());
            break;
          case vtMAP:
            { long iRow = inst->env.iCurLine;
              long iCol = inst->env.iCurCol;
              if (fNb && (inst->env.iCurrNb >= 0)) {
                iRow += (inst->env.iCurrNb / 3) - 1;
                iCol += (inst->env.iCurrNb % 3) - 1;
//                if (inst->fNbPosInstUsed) {
                  if ((iRow < 0) || (iRow >= mp->iLines()))
                    for (int i=0; i < bufR.iSize(); i++)
                      bufR[i] = rUNDEF;
/*                }
                else {
                  if (iRow < 0)
                    iRow =0;
                  else if (iRow >= mp->iLines())
                    iRow = mp->iLines()-1;
                }*/
              }
              if (bufR.iSize() == 1)
                bufR[0] = mp->rValue(RowCol(iRow, iCol));
              else {
                mp->GetLineVal(iRow, bufR, iCol, bufR.iSize());
                if (fNb) {
//                  if (inst->fNbPosInstUsed) {
                    if (iCol < 0)
                      bufR[0] = rUNDEF;
                    else if ((iCol + bufR.iSize()) > mp->iCols())
                      bufR[bufR.iSize()-1] = rUNDEF;
/*                  }
                  else {
                    if (iCol < 0)
                      bufR[0] = bufR[1];
                    else if ((iCol + bufR.iSize()) > mp->iCols())
                      bufR[bufR.iSize()-1] = bufR[bufR.iSize()-2];
                  }*/
                }
              }
            }
            break;
          default: assert (0 == 1);
        } // switch
        so->PutVal(bufR);
      }
      break;
    case sotLongRaw:
      { //LongBuf buf;
        so->Resize(bufL);
        switch (cv->vt) {
          case vtVALUE:
            { String sVal = cv->sValue();
              long iRaw = cv->dvs.iRaw(sVal);
              for (int i=0; i < bufL.iSize(); i++)
                bufL[i] = iRaw;
            }
            break;
          case vtCOLOR:
            { Color clrVal = cv->clrValue();
              for (int i=0; i < bufL.iSize(); i++)
                bufL[i] = clrVal;
            }
            break;
          case vtCOLUMN: {
              col->GetBufRaw(bufL, inst->env.iCurRec, bufL.iSize());
              if (fDomainSortConvert) {
                for (long i=0; i < bufL.iSize(); ++i) {
                  long j = bufL[i];
                  if ((j < 1) || (j > aiConvertRaw.iSize()))
                    bufL[i] = iUNDEF;
                  else
                    bufL[i] = aiConvertRaw[j-1];
                }
              }
            }
            break;
          case vtMAP:
            { long iRow = inst->env.iCurLine;
              long iCol = inst->env.iCurCol;
              if (fNb && (inst->env.iCurrNb >= 0)) {
                iRow += (inst->env.iCurrNb / 3) - 1;
                iCol += (inst->env.iCurrNb % 3) - 1;
//                if (inst->fNbPosInstUsed) {
                  if ((iRow < 0) || (iRow >= mp->iLines()))
                    for (int i=0; i < bufL.iSize(); i++)
                      bufL[i] = iUNDEF;
/*                }
                else {
                  if (iRow < 0)
                    iRow = 0;
                  else if (iRow >= mp->iLines())
                    iRow = mp->iLines()-1;
                }*/
              }
              if (bufL.iSize() == 1)
                bufL[0] = mp->iRaw(RowCol(iRow, iCol));
              else {
                mp->GetLineRaw(iRow, bufL, iCol, bufL.iSize());
                if (fNb) {
//                  if (inst->fNbPosInstUsed) {
                    if (iCol < 0)
                      bufL[0] = iUNDEF;
                    else if ((iCol + bufL.iSize()) > mp->iCols())
                      bufL[bufL.iSize()-1] = iUNDEF;
/*                  }
                  else {
                    if (iCol < 0)
                      bufL[0] = bufL[1];
                    else if ((iCol + bufL.iSize()) > mp->iCols())
                      bufL[bufL.iSize()-1] = bufL[bufL.iSize()-2];
                  }*/
                }
              }
              if (fDomainSortConvert) {
                for (long i=0; i < bufL.iSize(); ++i) {
                  long j = bufL[i];
                  if ((j < 1) || (j > aiConvertRaw.iSize()))
                    bufL[i] = iUNDEF;
                  else
                    bufL[i] = aiConvertRaw[j-1];
                }
              }
            }
            break;
          default: assert (0 == 1);
        } // switch
        if (rpr.fValid())
          for (int i=0; i < bufL.iSize(); i++)
            bufL[i] = rpr->clrRaw(bufL[i]);
        so->PutRaw(bufL);
      }
      break;
    case sotStringVal:
      { //StringBuf buf;
        so->Resize(bufS);
        switch (cv->vt) {
          case vtSTRING:
          case vtVALUE:
            { String sVal = cv->sValue();
              for (int i=0; i < bufS.iSize(); i++)
                bufS[i] = sVal;
            }
            break;
          case vtCOLUMN:
            col->GetBufVal(bufS, inst->env.iCurRec, bufS.iSize());
            break;
          case vtMAP:
            { for (int i=0; i < bufS.iSize(); i++)
                bufS[i] = sUNDEF;
            }
            break;
          default: assert (0 == 1);
        } // switch
        so->PutVal(bufS);
      }
      break;
    case sotCoordVal:
      { //CoordBuf buf;
        so->Resize(bufC);
        switch (cv->vt) {
          case vtCOORD:
          case vtVALUE:
            { Coord cVal = cv->cValue();
              for (int i=0; i < bufC.iSize(); i++)
                bufC[i] = cVal;
            }
            break;
          case vtCOLUMN:
            col->GetBufVal(bufC, inst->env.iCurRec, bufC.iSize());
            break;
          case vtMAP:
            { for (int i=0; i < bufC.iSize(); i++)
                bufC[0] = mp->gr()->cConv(RowCol((long)(inst->env.iCurLine), (long)(inst->env.iCurCol+i)));
            }
            break;
          default: assert (0 == 1);
        } // switch
        so->PutVal(bufC);
      }
      break;
    case sotColumn:
      so = new StackObject(col);
      break;
    case sotMap:
      so = new StackObject(mp);
      break;
    default :
      assert(0==1);
      break;
  }
  inst->stkCalc.push(so);
}

void InstLoad::GetMapUsed(Map& m)
{
  m = mp;
}

void InstLoad::ReplaceMapUsed(const Map& m)
{
  if (mp.fValid())
    mp = m;
}

void InstLoad::AddDependentObjects(ObjectDependency& objdep)
{
  if (mp.fValid())
    objdep.Add(mp);
  if (col.fValid())
    objdep.Add(col);
}

InstStore::InstStore(Instructions* instrucs, const CalcVariable& cvar)
: InstBase(instrucs)
{
  cv = cvar;
}

InstStore::~InstStore()
{
//  delete cv;
}

void InstStore::Exec() {
  StackObject* so = inst->stkCalc.pop();
  switch (so->sot) {
    case sotLongVal:
      { //LongBuf buf;
        so->Resize(bufL);
        so->GetVal(bufL);
        switch (cv->vt) {
          case vtVALUE: 
            { CalcVarSimple* cvs = cv->pcvSimple();
              if (0 != cvs)
                cvs->PutVal(bufL[0]);
              else {
                CalcVarParm* cvp= cv->pcvParm();
                if (0 != cvp)
                  cvp->PutVal(bufL[0]);
              }
            }
            break;
          case vtCOLUMN:
            col->PutBufVal(bufL, inst->env.iCurRec, bufL.iSize());
            break;
          case vtMAP:
            { if (bufL.iSize() == 1)
                cv->map()->PutVal(RowCol(inst->env.iCurLine, inst->env.iCurCol), bufL[0]);
              else
                cv->map()->PutLineVal(inst->env.iCurLine, bufL,
                                             inst->env.iCurCol, bufL.iSize());
            }
            break;
          default: assert (0 == 1);
        } // switch
      }
      break;
    case sotRealVal:
      { //RealBuf buf;
        so->Resize(bufR);
        so->GetVal(bufR);
        switch (cv->vt) {
          case vtVALUE:
            { CalcVarSimple* cvs = cv->pcvSimple();
              if (0 != cvs)
                cvs->PutVal(bufR[0]);
              else {
                CalcVarParm* cvp= cv->pcvParm();
                if (0 != cvp)
                  cvp->PutVal(bufR[0]);
              }
            }
						break;
          case vtCOLUMN:
            col->PutBufVal(bufR, inst->env.iCurRec, bufR.iSize());
            break;
          case vtMAP:
            { if (bufR.iSize() == 1)
                cv->map()->PutVal(RowCol(inst->env.iCurLine, inst->env.iCurCol), bufR[0]);
              else
                cv->map()->PutLineVal(inst->env.iCurLine, bufR,
                                             inst->env.iCurCol, bufR.iSize());
            }
            break;
          default: assert (0 == 1);
        } // switch
      }
      break;
    case sotLongRaw:
      { //LongBuf buf;
        so->Resize(bufL);
        so->GetRaw(bufL);
        switch (cv->vt) {
          case vtVALUE:
            cv->pcvSimple()->PutVal(cv->dvs.sValueByRaw(bufL[0]));
            break;
          case vtCOLOR:
            cv->pcvSimple()->PutVal(Color(bufL[0]));
            break;
          case vtCOLUMN:
            col->PutBufRaw(bufL, inst->env.iCurRec, bufL.iSize());
            break;
          case vtMAP:
            { if (bufL.iSize() == 1)
                cv->map()->PutRaw(RowCol(inst->env.iCurLine, inst->env.iCurCol), bufL[0]);
              else
                cv->map()->PutLineRaw(inst->env.iCurLine, bufL,
                                             inst->env.iCurCol, bufL.iSize());
            }
            break;
          default: assert (0 == 1);
        } // switch
      }
      break;
    case sotStringVal:
      { //StringBuf buf;
        so->Resize(bufS);
        so->GetVal(bufS);
        switch (cv->vt) {
          case vtSTRING:
          case vtVALUE:
            cv->pcvSimple()->PutVal(bufS[0]);
            break;
          case vtCOLUMN:
            col->PutBufVal(bufS, inst->env.iCurRec, bufS.iSize());
            break;
          case vtMAP:
            { for (int i=0; i < bufS.iSize(); i++)
                bufS[i] = sUNDEF;
            }
            break;
          default: assert (0 == 1);
        } // switch
      }
      break;
    case sotCoordVal:
      { //CoordBuf buf;
        so->Resize(bufC);
        so->GetVal(bufC);
        switch (cv->vt) {
          case vtCOORD:
          case vtVALUE:
            cv->pcvSimple()->PutVal(bufC[0]);
            break;
          case vtCOLUMN:
            col->PutBufVal(bufC, inst->env.iCurRec, bufC.iSize());
            break;
          case vtMAP:
            break;
          default: assert (0 == 1);
        } // switch
      }
      break;
    default :
      assert(0==1);
      break;
  }
  delete so;
}
/*
InstLoadX::InstLoadX(Instructions* instrucs, const CalcVariable& cvar)
: InstBase(instrucs)
{
  cv = cvar;
}

InstLoadX::~InstLoadX()
{
//  delete cv;
}

void InstLoadX::Exec() {
  StackObject* so = new StackObject(sotRealVal, inst->dvrsValue, inst->env.iBufSize);
//  RealBuf buf;
  so->Resize(buf);
  switch (cv->vt) {
    case vtCOORD:
      { double rVal = cv->cValue().x;
        for (int i=0; i < buf.iSize(); i++)
          buf[i] = rVal;
      }
      break;
    case vtCOLUMN:
      cv->col()->GetBufVal(buf, inst->env.iCurRec, buf.iSize());
      break;
    case vtMAP:
      {
        for (int i=0; i < buf.iSize(); i++)
          buf[i] = cv->map()->gr()->cConv(RowCol(inst->env.iCurLine, inst->env.iCurCol+i)).x;
        break;
      }
    default: assert (0 == 1);
  } // switch
  so->PutVal(buf);
  inst->stkCalc.push(so);
}

InstLoadY::InstLoadY(Instructions* instrucs, const CalcVariable& cvar)
: InstBase(instrucs)
{
  cv = cvar;
}

InstLoadY::~InstLoadY()
{
//  delete cv;
}

void InstLoadY::Exec() {
  StackObject* so = new StackObject(sotRealVal, inst->dvrsValue, inst->env.iBufSize);
//  RealBuf buf;
  so->Resize(buf);
  switch (cv->vt) {
    case vtCOORD:
      { double rVal = cv->cValue().y;
        for (int i=0; i < buf.iSize(); i++)
          buf[i] = rVal;
      }
      break;
    case vtCOLUMN:
      cv->col()->GetBufVal(buf, inst->env.iCurRec, buf.iSize());
      break;
    case vtMAP:
      {
        for (int i=0; i < buf.iSize(); i++)
          buf[i] = cv->map()->gr()->cConv(RowCol(inst->env.iCurLine, inst->env.iCurCol+i)).y;
        break;
      }
    default: assert (0 == 1);
  } // switch
  so->PutVal(buf);
  inst->stkCalc.push(so);
}
*/
InstLoadParm::InstLoadParm(Instructions* instruc, int iPrm)
: InstBase(instruc)
{
  iParm = iPrm;
}

void InstLoadParm::Exec() {
  StackObject* so = new StackObject(*inst->stkParms.top()[iParm]);
  inst->stkCalc.push(so);
}

InstFuncEnd::InstFuncEnd(Instructions* instrucs, UserDefFuncDesc* funcDesc)
: InstBase(instrucs)
{
  fds = funcDesc;
}

void InstFuncEnd::Exec()
{ 
  StackObject* soRes = inst->stkCalc.pop();
  inst->stkCalc.push(soRes);
}


InstEnterExternFunc::InstEnterExternFunc(Instructions* instrucs, int iP)
  : InstBase(instrucs)
{
  iParms = iP;
}

InstEnterExternFunc::~InstEnterExternFunc()
{
}

void InstEnterExternFunc::Exec()
{ 
  inst->stkParms.push(vector<StackObject*>(iParms));
  for (int i=0; i < iParms; i++) {
    inst->stkParms.top()[iParms - i - 1] = inst->stkCalc.top();
    inst->stkCalc.pop();
  }
}

InstLeaveExternFunc::InstLeaveExternFunc(Instructions* instrucs, int iP)
  : InstBase(instrucs)
{
  iParms = iP;
}

InstLeaveExternFunc::~InstLeaveExternFunc()
{
}

void InstLeaveExternFunc::Exec()
{ 
  for (int i=0; i < iParms; i++)
    delete inst->stkParms.top()[i];
  inst->stkParms.pop();
}



InstInternFunc::InstInternFunc(Instructions* instrucs, InternFuncDesc* funcDesc,
                               const DomainValueRangeStruct& dvsr, const StackObjectType& sotp)
  : InstBase(instrucs)
{
  fds = funcDesc->fdsCopy();
  dvs = dvsr;
  sot = sotp;
}

InstInternFunc::~InstInternFunc()
{
  delete fds;
}

void InstInternFunc::Exec() {
  int iParms = fds->iParms();
  StackObject* soRes = new StackObject(sot, dvs, inst->env.iBufSize);
  switch (iParms) {
    case 0:
      (*fds->fp)(soRes); break;
    case 1:
      (*fds->fp)(soRes, inst->stkCalc[0]); break;
    case 2:
      (*fds->fp)(soRes, inst->stkCalc[1], inst->stkCalc[0]); break;
    case 3:
      (*fds->fp)(soRes, inst->stkCalc[2], inst->stkCalc[1], inst->stkCalc[0]); break;
    case 4:
      (*fds->fp)(soRes, inst->stkCalc[3], inst->stkCalc[2], inst->stkCalc[1], inst->stkCalc[0]); break;
  }
  for (int i=iParms-1; i >= 0; i--)
    delete inst->stkCalc.pop();
  inst->stkCalc.push(soRes);
}

InstIndexLoad::InstIndexLoad(Instructions* instrucs, const CalcVariable& cvar, const Table& table)
    : InstBase(instrucs)
{
  cv = cvar; tbl = table;
  col = cv->col();
  if (tbl.fValid())
    pdsrt = tbl->dm()->pdsrt();
  else
    if (col.fValid())
      pdsrt = col->dmKey()->pdsrt();
}

InstIndexLoad::~InstIndexLoad()
{
//  delete cv;
}

void InstIndexLoad::Exec()
{
  StackObject* soIndex = inst->stkCalc.pop();
//  LongBuf bufInd;
  soIndex->Resize(bufInd);
//  if (tbl.fValid() && (0 != tbl->dm()->pdv()))
  if (soIndex->dvs.fValues()) {
    soIndex->GetVal(bufInd);
    if (0 != pdsrt)
      for (long i=0; i < inst->env.iBufSize; i++)
        bufInd[i] = pdsrt->iKey(bufInd[i]);
  }
  else if (0 != soIndex->dvs.dm()->pds()) {
//    StringBuf sBufInd;
    soIndex->GetVal(sBufInd);
    if (0 != pdsrt)
      for (long i=0; i < inst->env.iBufSize; i++)
        bufInd[i] = pdsrt->iKey(pdsrt->iOrd(sBufInd[i]));
  }
  else
    soIndex->GetRaw(bufInd);
  StackObject* soRes = new StackObject(cv->sot, cv->dvs, inst->env.iBufSize);
  switch (soRes->sot) {
    case sotLongVal:
      {// LongBuf buf;
        soRes->Resize(bufL);
        for (long i=0; i < inst->env.iBufSize; i++)
          bufL[i] = col->iValue(bufInd[i]);
        soRes->PutVal(bufL);
      }
      break;
    case sotRealVal:
      { //RealBuf buf;
        soRes->Resize(bufR);
        for (long i=0; i < inst->env.iBufSize; i++)
          bufR[i] = col->rValue(bufInd[i]);
        soRes->PutVal(bufR);
      }
      break;
    case sotLongRaw:
      { //LongBuf buf;
        soRes->Resize(bufL);
        for (long i=0; i < inst->env.iBufSize; i++)
          bufL[i] = col->iRaw(bufInd[i]);
        soRes->PutRaw(bufL);
      }
      break;
    case sotStringVal:
      { //StringBuf buf;
        soRes->Resize(bufS);
        for (long i=0; i < inst->env.iBufSize; i++)
          bufS[i] = col->sValue(bufInd[i]);
        soRes->PutVal(bufS);
      }
    case sotCoordVal:
      { //CoordBuf buf;
        soRes->Resize(bufC);
        for (long i=0; i < inst->env.iBufSize; i++)
          bufC[i] = col->cValue(bufInd[i]);
        soRes->PutVal(bufC);
      }
      break;
  }
  delete soIndex;
  inst->stkCalc.push(soRes);
}

void InstIndexLoad::AddDependentObjects(ObjectDependency& objdep)
{
  objdep.Add(col);
}

InstIndex2Load::InstIndex2Load(Instructions* instrucs, CalcVarTable2* cvar, const Table& table)
 : InstBase(instrucs)
{
   cvt2 = cvar; 
   tbl = table; 
   cv.SetPointer(cvt2);
}

InstIndex2Load::~InstIndex2Load()
{
  //delete cvt2;
}

void InstIndex2Load::Exec()
{
  StackObject* soIndex2 = inst->stkCalc.pop();
  StackObject* soIndex1 = inst->stkCalc.pop();
//  LongBuf bufInd1, bufInd2;
  soIndex2->Resize(bufInd2);
  soIndex1->Resize(bufInd1);
//  if (tbl.fValid() && (0 != tbl->dm()->pdv()))
//if (soIndex2->dvs.fValues())
//  soIndex2->GetVal(bufInd2);
//else
    soIndex2->GetRaw(bufInd2);
//if (soIndex1->dvs.fValues())
//  soIndex1->GetVal(bufInd1);
 //lse
    soIndex1->GetRaw(bufInd1);
  StackObject* soRes = new StackObject(cvt2->sot, cvt2->dvs, inst->env.iBufSize);
  switch (soRes->sot) {
    case sotLongVal:
      { //LongBuf buf;
        soRes->Resize(bufL);
        for (long i=0; i < inst->env.iBufSize; i++)
          bufL[i] = cvt2->tb2()->iValue(bufInd1[i], bufInd2[i]);
        soRes->PutVal(bufL);
      }
      break;
    case sotRealVal:
      { //RealBuf buf;
        soRes->Resize(bufR);
        for (long i=0; i < inst->env.iBufSize; i++)
          bufR[i] = cvt2->tb2()->rValue(bufInd1[i], bufInd2[i]);
        soRes->PutVal(bufR);
      }
      break;
    case sotLongRaw:
      { //LongBuf buf;
        soRes->Resize(bufL);
        for (long i=0; i < inst->env.iBufSize; i++)
          bufL[i] = cvt2->tb2()->iRaw(bufInd1[i], bufInd2[i]);
        soRes->PutRaw(bufL);
      }
      break;
    case sotStringVal:
      { //StringBuf buf;
        soRes->Resize(bufS);
        for (long i=0; i < inst->env.iBufSize; i++)
          bufS[i] = cvt2->tb2()->sValue(bufInd1[i], bufInd2[i]);
        soRes->PutVal(bufS);
      }
      break;
  }
  delete soIndex1;
  delete soIndex2;
  inst->stkCalc.push(soRes);
}

void InstIndex2Load::AddDependentObjects(ObjectDependency& objdep)
{
  objdep.Add(cvt2->tb2());
}

InstIndexStore::InstIndexStore(Instructions* instrucs, const CalcVariable& cvar, const Table& table)
: InstBase(instrucs)
{
  cv = cvar; tbl = table;
  pdsrt = tbl->dm()->pdsrt();
  col = cv->col();
}

InstIndexStore::~InstIndexStore()
{
//  delete cv;
}

void InstIndexStore::Exec()
{
  StackObject* soVal = inst->stkCalc.pop();
  StackObject* soIndex = inst->stkCalc.pop();
//  LongBuf bufInd;
  soIndex->Resize(bufInd);
//  if (tbl.fValid() && (0 != tbl->dm()->pdv()))
//    soIndex->GetVal(bufInd);
//  else
  if (soIndex->dvs.fValues()) {
    soIndex->GetVal(bufInd);
    if (0 != pdsrt)
      for (long i=0; i < inst->env.iBufSize; i++)
        bufInd[i] = pdsrt->iKey(bufInd[i]);
  }
  else if (0 != soIndex->dvs.dm()->pds()) {
//    StringBuf sBufInd;
    soIndex->GetVal(sBufInd);
    if (0 != pdsrt)
      for (long i=0; i < inst->env.iBufSize; i++)
        bufInd[i] = pdsrt->iKey(pdsrt->iOrd(sBufInd[i]));
  }
  else
    soIndex->GetRaw(bufInd);
  if (col->fDependent())
    return;
  switch (soVal->sot) {
    case sotLongVal:
      { //LongBuf buf;
        soVal->Resize(bufL);
        soVal->GetVal(bufL);
        for (long i=0; i < inst->env.iBufSize; i++)
          col->PutVal(bufInd[i], bufL[i]);
      }
      break;
    case sotRealVal:
      { //RealBuf buf;
        soVal->Resize(bufR);
        soVal->GetVal(bufR);
        for (long i=0; i < inst->env.iBufSize; i++)
          col->PutVal(bufInd[i], bufR[i]);
      }
      break;
    case sotLongRaw:
      { //LongBuf buf;
        soVal->Resize(bufL);
        soVal->GetRaw(bufL);
        for (long i=0; i < inst->env.iBufSize; i++)
          col->PutRaw(bufInd[i], bufL[i]);
      }
      break;
    case sotStringVal:
      { //StringBuf buf;
        soVal->Resize(bufS);
        soVal->GetVal(bufS);
        for (long i=0; i < inst->env.iBufSize; i++)
          col->PutVal(bufInd[i], bufS[i]);
      }
      break;
    case sotCoordVal:
      { //CoordBuf buf;
        soVal->Resize(bufC);
        soVal->GetVal(bufC);
        for (long i=0; i < inst->env.iBufSize; i++)
          col->PutVal(bufInd[i], bufC[i]);
      }
      break;
  }
  col->Updated();
  Table(col->fnTbl)->Updated();
  delete soIndex;
  delete soVal;
}

InstGoto::InstGoto(Instructions* instrucs, int iLab)
: InstBase(instrucs)
{
  iLabel = iLab;
}

void InstGoto::Exec()
{
  inst->iNextInst = inst->lstLabels[iLabel]-1;
	Tranquilizer *trq = inst->trqTranquilizer();
	if ( trq && trq->fUpdate(iUNDEF))
		inst->Stop();
}

InstGotoFalse::InstGotoFalse(Instructions* instrucs, int iLab)
: InstBase(instrucs)
{
  iLabel = iLab;
}

void InstGotoFalse::Exec()
{
  StackObject* so = inst->stkCalc.pop();
//  LongBuf buf;
  so->Resize(buf);
  so->GetVal(buf);
  if (buf[0] == 0)
    inst->iNextInst = inst->lstLabels[iLabel]-1;
  delete so;
}

InstStop::InstStop(Instructions* instrucs)
: InstBase(instrucs)
{
}

InstSimpleStart::InstSimpleStart(Instructions* instrucs)
: InstBase(instrucs)
{
}

InstSimpleEnd::InstSimpleEnd(Instructions* instrucs)
: InstBase(instrucs)
{
}

void InstStop::Exec()
{ 
  inst->Stop(); 
}

InstClfy::InstClfy(Instructions* instrucs, const Domain& dmGroup, const DomainValueRangeStruct& dvs)
  : InstBase(instrucs)
{
  dom = dmGroup;
  dmgrp = dom->pdgrp();
  grpfnc = 0;
  if (!dvs.fUseReals())
   grpfnc = new GroupFunction(dvs, dmgrp);
//instrucs->aGroupDom &= dmgrp;
}

void InstClfy::Exec()
{
  StackObject* soRes = new StackObject(sotLongRaw, dom, inst->env.iBufSize);
  bufRes.Size(soRes->iSize());
  RealBuf buf;
  inst->stkCalc[0]->GetVal(buf);
  for (int i=0; i<buf.iSize(); i++)
    bufRes[i] = dmgrp->iClassify(buf[i]);
  soRes->PutRaw(bufRes);
  delete inst->stkCalc.pop();
  inst->stkCalc.push(soRes);
}

void InstClfy::AddDependentObjects(ObjectDependency& objdep)
{
  objdep.Add(dom);
}



