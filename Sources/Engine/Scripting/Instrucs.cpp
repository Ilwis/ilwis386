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
// $Log: /ILWIS 3.0/Calculator/Instrucs.cpp $
 * 
 * 12    14-11-00 9:07a Martin
 * added pragma 4786
 * 
 * 11    28-09-00 2:57p Martin
 * instructions has now the tranquilizer as member to be able to prevent
 * endless lopps from functions. Access function included
 * 
 * 10    14-02-00 16:13 Wind
 * added functions to retrieve dependent objects from instructions (bug
 * 404)
 * 
 * 9     7-02-00 17:03 Wind
 * bug in GetMapVal
 * 
 * 8     18-01-00 9:50 Wind
 * instruction list with 'gotofalse' instructions are now executed pixel
 * by pixel, or record by record
 * 
 * 7     5-01-00 18:12 Wind
 * some cosmetic changes (renaming of some functions, removing of others)
 * 
 * 6     5-01-00 11:09 Wind
 * rename class ExternFuncDesc to UserDefFuncDesc
 * 
 * 5     4-01-00 10:28 Wind
 * partial redesign and simplification to solve some bugs related with
 * user defined functions
 * 
 * 4     22-12-99 10:30 Wind
 * change in enum nbDir
 * 
 * 3     10/19/99 9:39a Wind
 * comments
 * 
 * 2     10/18/99 9:37a Wind
 * trace instructions
*/
// Revision 1.4  1998/09/16 17:30:36  Wim
// 22beta2
//
// Revision 1.3  1997/09/03 18:23:14  Wim
// Default CalcEnv not only has 100 lines and cols but also 100 recs.
//
// Revision 1.2  1997-07-25 12:25:52+02  Wim
// Latest changes of Jelle
//
/* instruc.c
   Jelle Wind, april 1995
	Last change:  WK   14 Apr 98    4:41 pm
*/
#define INSTRUCS_C

#pragma warning( disable : 4786 )

#include "Engine\Scripting\Instrucs.h"
#include "Engine\Scripting\INSTRUC.H"
#include "Engine\Scripting\Parser.h"
#include "Engine\Scripting\CODEGEN.H"

UserDefFuncDesc::UserDefFuncDesc(const String& sNam, VarType vt)
{
  sName = sNam; vtRes = vt;
  if (vtRes == vtVALUE)
    dmDefault = Domain("value");
  else  
    dmDefault = Domain("string");
  iPrms = 0;
}    


//static int iNbPower[] = { 256, 128, 64, 32, 8, 4, 2, 1, 16};
static int iNbPower[] = { 16, 1, 2, 4 , 8, 32, 64, 128, 256 };
//static int iNbOrdDown[] = { 8, 7, 6, 5, 3, 2, 1, 0, 4 };  1.4 order
static int iNbOrdDown[] = { 4, 0, 1, 2, 3, 5, 6, 7, 8 };
//static int iNbOrdUp[] = { 2, 1, 0, 5, 3, 8, 7, 6, 4 }; 1.4 order
//static int iNbOrdUp[] = { 4, 6, 7, 8, 3, 5, 0, 1, 2 };
static int iNbOrdUp[] = { 4, 0, 1, 2, 3, 5, 6, 7, 8 };
//static int iNbOrdLeft[] = { 2, 5, 8, 1, 7, 0, 3, 6, 4 }; 1.4 order
static int iNbOrdLeft[] = { 4, 0, 3, 6, 1, 7, 2, 5, 8 };
//static int iNbOrdRight[] = { 8, 5, 2, 7, 1, 6, 3, 0, 4 }; 1.4 order
static int iNbOrdRight[] = { 4, 0, 3, 6, 1, 7, 2, 5, 8 };

CalcEnv::CalcEnv()
{ 
  iLines = 100; iCols = 100;
  iCurLine = 0; iCurCol = 0;
  iRecs = 100;
  iCurRec = 0;
  iBufSize = iCols;
  iCurrNb = shUNDEF;
  fNeighbAgg = false;
  dnb = dirNbDOWN;
}

void Neighbours::Set(int iNbrs, dirNb dnb)
{
  Reset();
  int i;
  switch (dnb) {
    case dirNbDOWN :
      for (i=0; i<9; i++ )
        if (iNbPower[i] & iNbrs)
          (*this) &= iNbOrdDown[i];
      break;
    case dirNbUP :
      for (i=0; i<9; i++)
        if (iNbPower[i] & iNbrs)
          (*this) &= iNbOrdUp[i];
      break;
    case dirNbLEFT :
      for (i=0; i<9; i++)
        if (iNbPower[i] & iNbrs)
          (*this) &= iNbOrdLeft[i];
      break;
    case dirNbRIGHT :
      for (i=0; i<9; i++)
        if (iNbPower[i] & iNbrs)
          (*this) &= iNbOrdRight[i];
      break;
    default :
      break;
  }
}

Instructions::Instructions() :
	trq(NULL)
{
  dvrsValue = DomainValueRangeStruct(Domain("value"));
}

Instructions::~Instructions()
{
  unsigned int i;
  for (i=0; i < lstInst.iSize(); ++i)
    delete lstInst[i];
  while (!stkCalc.fEmpty())
    delete stkCalc.pop();
}

RowCol Instructions::rcSize() const
{
  if (!_gr->fGeoRefNone())
    _gr->rcSize();
  if (env.iLines >= 0)
    return RowCol(env.iLines, env.iCols);
  return RowCol(0L, 0L);
}

void Instructions::CalcExec()
{
/*
  for (int i=0; i < lstInst.iSize(); i++) {
    String s("%2li %2li %2li %2li, %2i %s\n", lstInst.iSize(),stkCalc.iSize(), lstLabels.iSize(), i, typeid(*lstInst[i]).name());
    TRACE(s.c_str());
  }
  TRACE("\n\n");
*/
  fStop = env.fNeighbAgg = false;
  iNextInst = -1;
  do {
    if (!env.fNeighbAgg) {
      ++iNextInst;
      if (iNextInst >= lstInst.iSize())
        fStop = true;
      else {
//String s("%2li %2li %2li %2li, %2i %s\n", lstInst.iSize(),stkCalc.iSize(), lstLabels.iSize(), iNextInst, typeid(*lstInst[iNextInst]).name());
//TRACE(s.c_str());
        lstInst[iNextInst]->Exec();
      }
    }
    else {
      int iNextInstSav = iNextInst;
      for (unsigned int j=0; j < env.nb.iSize(); ++j) {
        env.iCurrNb = env.nb[j];
        iNextInst = iNextInstSav;
        env.fNeighbAgg = true;
        do {
          ++iNextInst;
          if (iNextInst >= lstInst.iSize()) {
            fStop = true;
            break;
          }
          lstInst[iNextInst]->Exec();
        } while (env.fNeighbAgg);
        if (fStop)
          break;
      }
    }
  } while (!fStop);
}

double Instructions::rCalcVal()
{
  env.iBufSize = 1;
  env.iCurLine = iUNDEF;
  env.iCurCol = iUNDEF;
  CalcExec();
  RealBuf buf;
  if (stkCalc.fEmpty())
    return rUNDEF;
  StackObject* so = stkCalc.pop();
  so->GetVal(buf);
  delete so;
  return buf[0];
}

String Instructions::sCalcVal()
{
  env.iBufSize = 1;
  env.iCurLine = iUNDEF;
  env.iCurCol = iUNDEF;
  CalcExec();
  StringBuf buf;
  if (stkCalc.fEmpty())
    return sUNDEF;
  StackObject* so = stkCalc.pop();
  so->GetVal(buf);
  delete so;
  return buf[0];
}

Coord Instructions::cCalcVal()
{
  env.iBufSize = 1;
  env.iCurLine = iUNDEF;
  env.iCurCol = iUNDEF;
  CalcExec();
  CoordBuf buf;
  if (stkCalc.fEmpty())
    return crdUNDEF;
  StackObject* so = stkCalc.pop();
  so->GetVal(buf);
  delete so;
  return buf[0];
}

double Instructions::rMapCalcVal(RowCol rc)
{
  env.iBufSize = 1;
  env.iCurLine = rc.Row;
  env.iCurCol = rc.Col;
  CalcExec();
  if (stkCalc.fEmpty())
    return rUNDEF;
  RealBuf buf;
  StackObject* so = stkCalc.pop();
  so->GetVal(buf);
  delete so;
  return buf[0];
}

long Instructions::iMapCalcVal(RowCol rc)
{
  env.iBufSize = 1;
  env.iCurLine = rc.Row;
  env.iCurCol = rc.Col;
  CalcExec();
  if (stkCalc.fEmpty())
    return iUNDEF;
  LongBuf buf;
  StackObject* so = stkCalc.pop();
  so->GetVal(buf);
  delete so;
  return buf[0];
}

long Instructions::iMapCalcRaw(RowCol rc)
{
  env.iBufSize = 1;
  env.iCurLine = rc.Row;
  env.iCurCol = rc.Col;
  CalcExec();
  if (stkCalc.fEmpty())
    return iUNDEF;
  LongBuf buf;
  StackObject* so = stkCalc.pop();
  so->GetRaw(buf);
  delete so;
  return buf[0];
}

void Instructions::MapCalcVal(long iLine, RealBuf& buf, long iFrom, long iNum)
{
  env.iCurLine = iLine;
  env.iCurCol = iFrom;
  if (fGotoFalseInstUsed()) { 
    // can only use buffers of size 1, so do the calculation for every pixel seperately
    RealBuf rb(1);
    env.iBufSize = 1;
    for (int i=0; i < iNum; i++) {
      CalcExec();
      if (stkCalc.fEmpty())
        return;
      StackObject* so = stkCalc.pop();
      so->GetVal(rb);
      delete so;
      buf[i] = rb[0];
      env.iCurCol++;
    }
  }
  else {
    env.iBufSize = iNum;
    CalcExec();
    if (stkCalc.fEmpty())
      return;
    StackObject* so = stkCalc.pop();
    so->GetVal(buf);
    delete so;
  }
}

void Instructions::MapCalcVal(long iLine, LongBuf& buf, long iFrom, long iNum)
{
  env.iCurLine = iLine;
  env.iCurCol = iFrom;
  if (fGotoFalseInstUsed()) { 
    // can only use buffers of size 1, so do the calculation for every pixel seperately
    LongBuf lb(1);
    env.iBufSize = 1;
    for (int i=0; i < iNum; i++) {
      CalcExec();
      if (stkCalc.fEmpty())
        return;
      StackObject* so = stkCalc.pop();
      so->GetVal(lb);
      delete so;
      buf[i] = lb[0];
      env.iCurCol++;
    }
  }
  else {
    env.iBufSize = iNum;
    CalcExec();
    if (stkCalc.fEmpty())
      return;
    StackObject* so = stkCalc.pop();
    so->GetVal(buf);
    delete so;
  }
}

void Instructions::MapCalcRaw(long iLine, LongBuf& buf, long iFrom, long iNum)
{
  env.iCurLine = iLine;
  env.iCurCol = iFrom;
  if (fGotoFalseInstUsed()) { 
    // can only use buffers of size 1, so do the calculation for every pixel seperately
    LongBuf lb(1);
    env.iBufSize = 1;
    for (int i=0; i < iNum; i++) {
      CalcExec();
      if (stkCalc.fEmpty())
        return;
      StackObject* so = stkCalc.pop();
      so->GetRaw(lb);
      delete so;
      buf[i] = lb[0];
      env.iCurCol++;
    }
  }
  else {
    env.iBufSize = iNum;
    CalcExec();
    if (stkCalc.fEmpty())
      return;
    StackObject* so = stkCalc.pop();
    so->GetRaw(buf);
    delete so;
  }
}

double Instructions::rColumnCalcVal(long iKey)
{
  env.iBufSize = 1;
  env.iCurRec = iKey;
  CalcExec();
  if (stkCalc.fEmpty())
    return rUNDEF;
  RealBuf buf;
  StackObject* so = stkCalc.pop();
  so->GetVal(buf);
  delete so;
  return buf[0];
}

Coord Instructions::cColumnCalcVal(long iKey)
{
  env.iBufSize = 1;
  env.iCurRec = iKey;
  CalcExec();
  if (stkCalc.fEmpty())
    return Coord();
  CoordBuf buf;
  StackObject* so = stkCalc.pop();
  so->GetVal(buf);
  delete so;
  return buf[0];
}

long Instructions::iColumnCalcVal(long iKey)
{
  env.iBufSize = 1;
  env.iCurRec = iKey;
  CalcExec();
  if (stkCalc.fEmpty())
    return iUNDEF;
  LongBuf buf;
  StackObject* so = stkCalc.pop();
  so->GetVal(buf);
  delete so;
  return buf[0];
}

long Instructions::iColumnCalcRaw(long iKey)
{
  env.iBufSize = 1;
  env.iCurRec = iKey;
  CalcExec();
  if (stkCalc.fEmpty())
    return iUNDEF;
  LongBuf buf;
  StackObject* so = stkCalc.pop();
  so->GetRaw(buf);
  delete so;
  return buf[0];
}

String Instructions::sColumnCalcVal(long iKey, short iWid, short iDec)
{
  env.iBufSize = 1;
  env.iCurRec = iKey;
  CalcExec();
  if (stkCalc.fEmpty())
    return sUNDEF;
  StringBuf buf;
  StackObject* so = stkCalc.pop();
  so->GetVal(buf, iWid, iDec);
  delete so;
  return buf[0];
}

void Instructions::ColumnCalcVal(RealBuf& buf, long iKey, long iNum)
{
  env.iCurRec = iKey;
  if (fGotoFalseInstUsed()) { 
    // can only use buffers of size 1, so do the calculation for every record seperately
    RealBuf rb(1);
    env.iBufSize = 1;
    for (int i=0; i < iNum; i++) {
      CalcExec();
      if (stkCalc.fEmpty())
        return;
      StackObject* so = stkCalc.pop();
      so->GetVal(rb);
      delete so;
      buf[i] = rb[0];
      env.iCurRec++;
    }
  }
  else {
    env.iBufSize = iNum;
    CalcExec();
    if (stkCalc.fEmpty())
      return;
    StackObject* so = stkCalc.pop();
    so->GetVal(buf);
    delete so;
  }
}

void Instructions::ColumnCalcVal(CoordBuf& buf, long iKey, long iNum)
{
  env.iCurRec = iKey;
  if (fGotoFalseInstUsed()) { 
    // can only use buffers of size 1, so do the calculation for every record seperately
    CoordBuf crdb(1);
    env.iBufSize = 1;
    for (int i=0; i < iNum; i++) {
      CalcExec();
      if (stkCalc.fEmpty())
        return;
      StackObject* so = stkCalc.pop();
      so->GetVal(crdb);
      delete so;
      buf[i] = crdb[0];
      env.iCurRec++;
    }
  }
  else {
    env.iBufSize = iNum;
    CalcExec();
    if (stkCalc.fEmpty())
      return;
    StackObject* so = stkCalc.pop();
    so->GetVal(buf);
    delete so;
  }
}

void Instructions::ColumnCalcVal(LongBuf& buf, long iKey, long iNum)
{
  env.iCurRec = iKey;
  if (fGotoFalseInstUsed()) { 
    // can only use buffers of size 1, so do the calculation for every record seperately
    LongBuf lb(1);
    env.iBufSize = 1;
    for (int i=0; i < iNum; i++) {
      CalcExec();
      if (stkCalc.fEmpty())
        return;
      StackObject* so = stkCalc.pop();
      so->GetVal(lb);
      delete so;
      buf[i] = lb[0];
      env.iCurRec++;
    }
  }
  else {
    env.iBufSize = iNum;
    CalcExec();
    if (stkCalc.fEmpty())
      return;
    StackObject* so = stkCalc.pop();
    so->GetVal(buf);
    delete so;
  }
}

void Instructions::ColumnCalcRaw(LongBuf& buf, long iKey, long iNum)
{
  env.iCurRec = iKey;
  if (fGotoFalseInstUsed()) { 
    // can only use buffers of size 1, so do the calculation for every record seperately
    LongBuf lb(1);
    env.iBufSize = 1;
    for (int i=0; i < iNum; i++) {
      CalcExec();
      if (stkCalc.fEmpty())
        return;
      StackObject* so = stkCalc.pop();
      so->GetRaw(lb);
      delete so;
      buf[i] = lb[0];
      env.iCurRec++;
    }
  }
  else {
    env.iBufSize = iNum;
    CalcExec();
    if (stkCalc.fEmpty())
      return;
    StackObject* so = stkCalc.pop();
    so->GetRaw(buf);
    delete so;
  }
}

void Instructions::ColumnCalcVal(StringBuf& buf, long iKey, long iNum, short iWid, short iDec)
{
  env.iCurRec = iKey;
  if (fGotoFalseInstUsed()) { 
    // can only use buffers of size 1, so do the calculation for every record seperately
    StringBuf sb(1);
    env.iBufSize = 1;
    for (int i=0; i < iNum; i++) {
      CalcExec();
      if (stkCalc.fEmpty())
        return;
      StackObject* so = stkCalc.pop();
      so->GetVal(sb, iWid, iDec);
      delete so;
      buf[i] = sb[0];
      env.iCurRec++;
    }
  }
  else {
    env.iBufSize = iNum;
    CalcExec();
    if (stkCalc.fEmpty())
      return;
    StackObject* so = stkCalc.pop();
    so->GetVal(buf, iWid, iDec);
    delete so;
  }
}

void Instructions::ChangeConstStringsToRaw(const Domain& dm)
{
  if (0 == dm->pdsrt())
    return;
  for (int j=0; j < lstInst.iSize(); j++) {
    InstConstString* ics = dynamic_cast<InstConstString*>(lstInst[j]);
    if (0 != ics) {
      if (0 != ics->cv->dvs.dm()->pds())
        ics->cv->SetDomainValueRangeStruct(dm);
    }
    else {
      InstInternFunc* iif = dynamic_cast<InstInternFunc*>(lstInst[j]);
      if (0 != iif) {
        if ((0 != iif->dvs.dm()->pds()) && (0 == _strcmpi(iif->fds->sFunc,"iff"))) {
          iif->dvs = dm;
          iif->sot = sotLongRaw;
        }
      }
    }
  }
}

Instructions* Instructions::instSimple(const String& sCalc)
{
  CodeGenerator cg(ctSIMPLE, new Instructions());
  cg.sExpr = sCalc;
  ProgramParser parser(&TokenizerBase(&TextInput(sCalc)), &cg);
  parser.GetNextToken();
  parser.Program();
  if (!parser.fEnd())
    ExpressionError(sCalc, String());
  cg.DetermineDomain();
  return cg.inst;
}

void Instructions::GetVars(Array<CalcVariable>& acv)
{
  for (int j=0; j < lstInst.iSize(); j++) {
    InstLoad* ild = dynamic_cast<InstLoad*>(lstInst[j]);
    if (0 != ild) {
      // check if already is in list
    	int k=0;
      for (; k < acv.iSize(); k++)
        if (acv[k] == ild->cv)
          break;
      if (k == acv.iSize())  // not found
        acv &= ild->cv;
    }
    else {
      InstStore* ist = dynamic_cast<InstStore*>(lstInst[j]);
      if (0 != ist) {
        // check if already is in list
		  int k=0;
        for (; k < acv.iSize(); k++)
          if (acv[k] == ist->cv)
            break;
        if (k == acv.iSize())  // not found
          acv &= ist->cv;
      }
    }
  }
}


void Instructions::Add(InstBase* inst)
{ 
   lstInst &= inst;
}

double Instructions::rCalcVal(double r)
{
  env.iBufSize = 1;
  RealBuf buf(1);
  buf[0] = r;
  StackObject* so = new StackObject(sotRealVal, dvrsValue, env.iBufSize);
  so->PutVal(buf);
  stkCalc.push(so);
  CalcExec();
  // get value from stack
  so = stkCalc.pop();
  so->GetVal(buf);
  r = buf[0];
  delete so;
  return r;
}

double Instructions::rCalcVal(double x, double y)
{
  env.iBufSize = 1;
  RealBuf buf(1), bufX(1), bufY(1);
  bufX[0] = x;
  bufY[0] = y;
  StackObject* so = new StackObject(sotRealVal, dvrsValue, env.iBufSize);
  so->PutVal(bufY);
  stkCalc.push(so);
  so = new StackObject(sotRealVal, dvrsValue, env.iBufSize);
  so->PutVal(bufX);
  stkCalc.push(so);
  CalcExec();
  // get value from stack
  so = stkCalc.pop();
  so->GetVal(buf);
  double r = buf[0];
  delete so;
  return r;
}

void Instructions::CalcCoords(const CoordBuf& cBufIn, CoordBuf& cBufOut)
{
  env.iBufSize = cBufIn.iSize();
  // split in X and Y buffer
  RealBuf bufX(env.iBufSize);
  RealBuf bufY(env.iBufSize);
  for (int i=0; i<env.iBufSize; ++i) {
    bufX[i] = cBufIn[i].x;
    bufY[i] = cBufIn[i].y;
  }
  // load buffers on stack
  StackObject* so = new StackObject(sotRealVal, dvrsValue, env.iBufSize);
  so->PutVal(bufX);
  stkCalc.push(so);
  so = new StackObject(sotRealVal, dvrsValue, env.iBufSize);
  so->PutVal(bufY);
  stkCalc.push(so);
  CalcExec();
  // get coords from stack
  so = stkCalc.pop();
  so->GetVal(cBufOut);
  delete so;
}

void Instructions::CalcCoords(const Coord& cIn, Coord& cOut)
{
  env.iBufSize = 1;
  RealBuf bufX(1);
  RealBuf bufY(1);
  bufX[0] = cIn.x;
  bufY[0] = cIn.y;
  // load buffers on stack
  StackObject* so = new StackObject(sotRealVal, dvrsValue, env.iBufSize);
  so->PutVal(bufX);
  stkCalc.push(so);
  so = new StackObject(sotRealVal, dvrsValue, env.iBufSize);
  so->PutVal(bufY);
  stkCalc.push(so);
  CalcExec();
  // get coords from stack
  so = stkCalc.pop();
  CoordBuf cBufOut(1);
  so->GetVal(cBufOut);
  cOut = cBufOut[0];
  delete so;
}

void Instructions::SetGeoRef(const GeoRef& grf)
{
  _gr = grf;
}

void Instructions::GetMapsUsed(Array<Map>& amp)
{
  for (int i=0; i < lstInst.iSize(); ++i) {
    InstBase* inst = lstInst[i];
    Map mp;
    inst->GetMapUsed(mp);
    if (mp.fValid()) {
      for (int j=0; j < amp.iSize(); ++j)
        if (amp[j] == mp) {
          mp = Map();
          break;
        }
    }
    if (mp.fValid())
      amp &= mp;
  }
}

void Instructions::ReplaceMapUsed(const Map& map1, const Map& map2)
{
  for (int i=0; i < lstInst.iSize(); ++i) {
    InstBase* inst = lstInst[i];
    Map mp;
    inst->GetMapUsed(mp);
    if (mp.fValid() && (mp == map1))
      inst->ReplaceMapUsed(map2);
  }
}

void Instructions::SetNbPosInstUsed()
{
  for (int i=0; i < lstInst.iSize(); ++i) {
    InstBase* inst = lstInst[i];
    if (0 != dynamic_cast<InstNbPos*>(inst))
      fNbPosInstUsed = true;
  }
  fNbPosInstUsed = false;
}


int Instructions::iNewLabel()
{
  lstLabels &= 0; // extend array
  return lstLabels.size()-1;
}

bool Instructions::fGotoFalseInstUsed() const
{
  for (int i=0; i < lstInst.iSize(); i++)
    if (0 != dynamic_cast<InstGotoFalse*>(lstInst[i]))
      return true;
  return false;
}

void Instructions::Get(ObjectDependency& objdep)
{
  for (int i=0; i < lstInst.iSize(); i++)
    lstInst[i]->AddDependentObjects(objdep);
}

void Instructions::SetTranquilizer(Tranquilizer *_trq)
{
	trq = _trq;
}

Tranquilizer *Instructions::trqTranquilizer()
{
	return trq;
}
