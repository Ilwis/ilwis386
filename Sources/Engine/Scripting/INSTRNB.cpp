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
/* $Log: /ILWIS 3.0/Calculator/INSTRNB.cpp $
 * 
 * 9     1/14/03 8:44a Martin
 * removed error in GetMapUsed routine. Wrong use of the mp member, cuased
 * undefine behaviour
 * 
 * 8     6/26/00 11:57a Wind
 * solved bugs in nbavg and nbflt
 * 
 * 7     6/20/00 11:19a Wind
 * nbsum ignores undefs
 * 
 * 6     14-02-00 16:13 Wind
 * added functions to retrieve dependent objects from instructions (bug
 * 404)
 * 
 * 5     5-01-00 18:11 Wind
 * some cosmetic changes (renaming of some functions, removing of others)
 * 
 * 4     9/15/99 11:22a Wind
 * added nbstd
 * 
 * 3     17-08-99 18:03 Koolhoven
 * Header comment
 * 
 * 2     8/10/99 12:40p Wind
// Revision 1.5  1998/09/16 17:30:36  Wim
// 22beta2
//
// Revision 1.4  1997/09/09 08:05:04  Wim
// removed superflous reset() in InstNbEnd::Exec()
//
// Revision 1.3  1997-09-08 20:21:58+02  Wim
// delete soNb in InstNbNum::Exec() after using it after the pop()
//
// Revision 1.2  1997-07-25 12:25:52+02  Wim
// Latest changes of Jelle
//
/* instruc1.c
// Ilwis 2.0 instructions for calculator (1)
// sept. 1994, Jelle Wind
	Last change:  WK    9 Sep 97   10:03 am
*/
#include "Engine\Scripting\Instrucs.h"
#include "Engine\Scripting\CALCSTCK.H"
#include "Engine\Scripting\Calcvar.h"
#include "Engine\Function\FLTLIN.H"

InstNbStart::InstNbStart(Instructions* instrucs, int iNbr)
  : InstBase(instrucs)
{
  iNbrs = iNbr;
}

InstNbStart::~InstNbStart()
{
}

void InstNbStart::Exec()
{
  inst->env.nb.Set(iNbrs, inst->env.dnb);
  inst->env.fNeighbAgg = true;
}

InstNbEnd::InstNbEnd(Instructions* instrucs)
  : InstBase(instrucs)
{
}

InstNbEnd::~InstNbEnd()
{
}

void InstNbEnd::Exec()
{
  inst->env.iCurrNb = shUNDEF;
  inst->env.fNeighbAgg = false;
}

InstNbNum::InstNbNum(Instructions* instrucs, const Map& m)
  : InstBase(instrucs)
{
  mp = m;
  sot = StackObject::sotype(mp->dvrs());
}

InstNbNum::~InstNbNum()
{
}
static int iNbOrdTrans[] = { 0, 3, 6, 1, 4, 7, 2, 5, 8 };

void InstNbNum::Exec()
{
  StackObject* soNb = inst->stkCalc.pop();
  soNb->Resize(bufNb);
  soNb->GetVal(bufNb);
  delete soNb;
  for (int i=0; i < bufNb.iSize(); ++i)
    bufNb[i]--; // starts at 1
  if ((inst->env.dnb == dirNbLEFT)||(inst->env.dnb == dirNbRIGHT)) {
    for (int i=0; i < bufNb.iSize(); ++i)
      if ((bufNb[i] >= 0) && (bufNb[i] <= 8))
        bufNb[i] = iNbOrdTrans[bufNb[i]];
  }

  StackObject* so = new StackObject(sot, mp->dvrs(), inst->env.iBufSize);
  long iLine = inst->env.iCurLine-1;
  long iCol = inst->env.iCurCol-1;
  int iRowNb, iColNb;
  if (sot == sotRealVal) {
    so->Resize(bufR);
    for (int i=0; i < bufNb.iSize(); ++i, iCol++) {
      if ((bufNb[i] < 0) || (bufNb[i] > 8))
        bufR[i] = rUNDEF;
      else {
        iRowNb = bufNb[i] / 3;
        iColNb = bufNb[i] % 3;
        long iLine1 = iLine + iRowNb;
        long iCol1 = iCol + iColNb;
        if (iLine1 < 0)
          iLine1 = 0;
        else if (iLine1 >= mp->iLines())
          iLine1 = mp->iLines()-1;
        if (iCol1 < 0)
          iCol1 = 0;
        else if (iCol1 >= mp->iCols())
          iCol1 = mp->iCols()-1;
        bufR[i] = mp->rValue(RowCol(iLine1, iCol1));
      }
    }
    so->PutVal(bufR);
  }
  else {
    so->Resize(bufL);
    for (int i=0; i < bufNb.iSize(); ++i, iCol++) {
      if ((bufNb[i] < 0) || (bufNb[i] > 8))
        bufL[i] = iUNDEF;
      else {
        iRowNb = bufNb[i] / 3;
        iColNb = bufNb[i] % 3;
        long iLine1 = iLine + iRowNb;
        long iCol1 = iCol + iColNb;
        if (iLine1 < 0)
          iLine1 = 0;
        else if (iLine1 >= mp->iLines())
          iLine1 = mp->iLines()-1;
        if (iCol1 < 0)
          iCol1 = 0;
        else if (iCol1 >= mp->iCols())
          iCol1 = mp->iCols()-1;
        bufL[i] = mp->iRaw(RowCol(iLine1, iCol1));
      }
    }
    so->PutRaw(bufL);
  }
  inst->stkCalc.push(so);
}

void InstNbNum::GetMapUsed(Map& m)
{
  m = mp;
}

void InstNbNum::ReplaceMapUsed(const Map& m)
{
  if (mp.fValid())
    mp = m;
}

void InstNbNum::AddDependentObjects(ObjectDependency& objdep)
{
  objdep.Add(mp);
}

InstNbMinMax::InstNbMinMax(Instructions* instrucs, bool fMn, bool fPos, bool fCondit)
  : InstBase(instrucs)
{
  fCond = fCondit;
  fPosit = fPos;
  fMin = fMn;
  if (fPosit)
    dvsNbPos = DomainValueRangeStruct(1, 9);
}

InstNbMinMax::~InstNbMinMax()
{
}

void InstNbMinMax::Exec()
{
  // bufL1 or bufR1 will contain the min or max
  // bufPos will contain the position of min or max
  // for conditional min or max LONG_MAX/DBL_MAX (and .._MIN) are used to determine
  // if there was a min or maximum
  StackObject* soCond;
  if (fCond) {
    soCond = inst->stkCalc.pop();
    soCond->Resize(bufCond);
    soCond->GetVal(bufCond);
    delete soCond;
  }
  StackObject* so1 = inst->stkCalc.pop();
  StackObject* so2;
  if (fPosit) {
    so1->Resize(bufPos);
    for (int j=0; j<bufPos.iSize(); ++j)
      bufPos[j] = 0;
  }
  if (so1->sot == sotRealVal) {
    so1->Resize(bufR1);
    so1->GetVal(bufR1);
    for (int j=0; j<bufR1.iSize(); ++j)
      if (bufR1[j] == rUNDEF)
        bufR1[j] = fMin ? DBL_MAX : -DBL_MAX;
    if (fCond) {
      for (int j=0; j<bufCond.iSize(); ++j)
        if ((bufCond[j] == 0) || (bufCond[j] == iUNDEF))
          bufR1[j] = fMin ? DBL_MAX : -DBL_MAX;
    }
    so1->Resize(bufR2);
    for (unsigned int i=1; i < inst->env.nb.iSize(); ++i) {
      if (fCond) {
        soCond = inst->stkCalc.pop();
        soCond->GetVal(bufCond);
        delete soCond;
      }
      so2 = inst->stkCalc.pop();
      so2->GetVal(bufR2);
      for (int j=0; j<bufR2.iSize(); ++j)
        if (bufR2[j] == rUNDEF)
          bufR2[j] = fMin ? DBL_MAX : -DBL_MAX;
      delete so2;
      if (fCond) {
        for (int j=0; j<bufCond.iSize(); ++j)
          if ((bufCond[j] == 0) || (bufCond[j] == iUNDEF))
            bufR2[j] = fMin ? DBL_MAX : -DBL_MAX;
      }
      for (int j=0; j<bufR1.iSize(); ++j)
        if ((fMin && (bufR2[j] <= bufR1[j])) ||
            (!fMin && (bufR2[j] >= bufR1[j]))) {
          bufR1[j] = bufR2[j];
          if (fPosit)
            bufPos[j] = i;
        }
    }
    if (fMin) {
      for (int j=0; j<bufCond.iSize(); ++j)
        if (bufR1[j] == DBL_MAX) {
          if (fPosit)
            bufPos[j] = iUNDEF;
          else
            bufR1[j] = rUNDEF;
        }
    }
    else {
      for (int j=0; j<bufR1.iSize(); ++j)
        if (bufR1[j] == -DBL_MAX)
          if (fPosit)
            bufPos[j] = iUNDEF;
          else
            bufR1[j] = rUNDEF;
    }
    if (!fPosit)
      so1->PutVal(bufR1);
  }
  else {
    so1->Resize(bufL1);
    so1->GetVal(bufL1);
    for (int j=0; j<bufL1.iSize(); ++j)
      if (bufL1[j] == iUNDEF)
        bufL1[j] = fMin ? LONG_MAX : -LONG_MAX+1;
    if (fCond) {
      for (int j=0; j<bufCond.iSize(); ++j)
        if ((bufCond[j] == 0) || (bufCond[j] == iUNDEF))
          bufL1[j] = fMin ? LONG_MAX : -LONG_MAX+1;
    }
    so1->Resize(bufL2);
    for (unsigned int i=1; i < inst->env.nb.iSize(); ++i) {
      if (fCond) {
        soCond = inst->stkCalc.pop();
        soCond->GetVal(bufCond);
        delete soCond;
      }
      so2 = inst->stkCalc.pop();
      so2->GetVal(bufL2);
      for (int j=0; j<bufL2.iSize(); ++j)
        if (bufL2[j] == iUNDEF)
          bufL2[j] = fMin ? LONG_MAX : -LONG_MAX+1;
      delete so2;
      if (fCond) {
        for (int j=0; j<bufCond.iSize(); ++j)
          if ((bufCond[j] == 0) || (bufCond[j] == iUNDEF))
            bufL2[j] = fMin ? LONG_MAX : -LONG_MAX+1;
      }
      for (int j=0; j<bufL1.iSize(); ++j) {
        if ((fMin && (bufL2[j] <= bufL1[j])) ||
                 (!fMin && (bufL2[j] >= bufL1[j]))) {
          bufL1[j] = bufL2[j];
          if (fPosit)
            bufPos[j] = i;
        }
      }
    }
    if (fMin) {
      for (int j=0; j<bufL1.iSize(); ++j)
        if (bufL1[j] == LONG_MAX) {
          if (fPosit)
            bufPos[j] = iUNDEF;
          else
            bufL1[j] = iUNDEF;
        }
    }
    else {
      for (int j=0; j<bufL1.iSize(); ++j)
        if (bufL1[j] == -LONG_MAX+1)
          if (fPosit)
            bufPos[j] = iUNDEF;
          else
            bufL1[j] = iUNDEF;
    }
    if (!fPosit)
      so1->PutVal(bufL1);
  }
  if (!fPosit)
    inst->stkCalc.push(so1);
  else {
    delete so1;
    StackObject* so = new StackObject(sotLongVal, dvsNbPos, inst->env.iBufSize);
    for (int j=0; j<bufPos.iSize(); ++j)
      if (bufPos[j] != iUNDEF)
        bufPos[j] = inst->env.nb[inst->env.nb.iSize()-bufPos[j]-1]+1;
    so->PutVal(bufPos);
    inst->stkCalc.push(so);
  }
}

InstNbPrd::InstNbPrd(Instructions* instrucs, bool fPos, bool fCondit, bool fRl)
  : InstBase(instrucs)
{
  fCond = fCondit;
  fReal = fRl;
  fPosit = fPos;
  if (fPosit)
    dvsNbPos = DomainValueRangeStruct(1, 9);
//  agf = AggregateFunction::create("aggprd");
}

InstNbPrd::~InstNbPrd()
{
//  delete agf;
}

void InstNbPrd::Exec()
{
  StackObject* so, *soCond;
  StackObject* soRes;
  if (fPosit) {
    bufPos.Size(inst->env.iBufSize);
    for (int j=0; j<bufPos.iSize(); ++j)
      bufPos[j] = iUNDEF;
  }
  if (fReal) {
    bufR.Size(inst->env.iBufSize);
    for (unsigned int i=0; i < inst->env.nb.iSize(); ++i) {
      if (fCond) {
        soCond = inst->stkCalc.pop();
        abufCond[i].Size(inst->env.iBufSize);
        soCond->GetVal(abufCond[i]);
        delete soCond;
      }
      so = inst->stkCalc.pop();
      abufR[i].Size(inst->env.iBufSize);
      so->GetVal(abufR[i]);
      if (fPosit || i!=0)
        delete so;
      else  // use it for the result on the stack
        soRes = so;
    }
    for (int j=0; j<bufR.iSize(); ++j) {
      int ii, iCnt = 0;
      for (unsigned int i=0; i < inst->env.nb.iSize(); ++i) {
        if (fCond) {
          long iCond = abufCond[i][j];
          if ((iCond == 0) || (iCond == iUNDEF))
            continue;
        }
        double rVal = abufR[i][j];
        if (rVal == rUNDEF)
          continue;
        for (ii=0; ii < iCnt; ++ii) {
          if (rVal == arVal[ii]) {
            aiCnt[ii]++;
            aiPos[ii] = i;
            break;
          }
        }
        if (ii==iCnt) {
          aiCnt[iCnt] = 1;
          arVal[iCnt] = rVal;
          aiPos[iCnt] = i;
          iCnt++;
        }
      }
      double rMaxVal = rUNDEF;
      int iMaxCnt = 0;
      int iMaxPos = 0;
      for (ii=iCnt-1; ii>=0; --ii)
        if (aiCnt[ii] > iMaxCnt) {
          rMaxVal = arVal[ii];
          iMaxCnt = aiCnt[ii];
          iMaxPos = aiPos[ii];
        }
      if (fPosit)
        bufPos[j] = iMaxPos;
      else
        bufR[j] = rMaxVal;
    }
    if (!fPosit)
      soRes->PutVal(bufR);
  }
  else {
    bufL.Size(inst->env.iBufSize);
    for (unsigned int i=0; i < inst->env.nb.iSize(); ++i) {
      if (fCond) {
        soCond = inst->stkCalc.pop();
        abufCond[i].Size(inst->env.iBufSize);
        soCond->GetVal(abufCond[i]);
        delete soCond;
      }
      so = inst->stkCalc.pop();
      abufL[i].Size(inst->env.iBufSize);
      so->GetRaw(abufL[i]);
      if (fPosit || i!=0)
        delete so;
      else  // use it for the result on the stack
        soRes = so;
    }
    for (int j=0; j<bufL.iSize(); ++j) {
      int ii, iCnt = 0;
      for (unsigned int i=0; i < inst->env.nb.iSize(); ++i) {
        if (fCond) {
          long iCond = abufCond[i][j];
          if ((iCond == 0) || (iCond == iUNDEF))
            continue;
        }
        long iVal = abufL[i][j];
        if (iVal == iUNDEF)
          continue;
        for (ii=0; ii < iCnt; ++ii) {
          if (iVal == aiVal[ii]) {
            aiCnt[ii]++;
            aiPos[ii] = i;
            break;
          }
        }
        if (ii==iCnt) {
          aiCnt[iCnt] = 1;
          aiVal[iCnt] = iVal;
          aiPos[iCnt] = i;
          iCnt++;
        }
      }
      long iMaxVal = iUNDEF;
      int iMaxCnt = 0;
      int iMaxPos = shUNDEF;
      for (ii=iCnt-1; ii>=0; --ii)
        if (aiCnt[ii] > iMaxCnt) {
          iMaxVal = aiVal[ii];
          iMaxCnt = aiCnt[ii];
          iMaxPos = aiPos[ii];
        }
      if (fPosit)
        bufPos[j] = iMaxPos;
      else
        bufL[j] = iMaxVal;
    }
    if (!fPosit)
      soRes->PutRaw(bufL);
  }
  if (!fPosit)
    inst->stkCalc.push(soRes);
  else {
    so = new StackObject(sotLongVal, dvsNbPos, inst->env.iBufSize);
    for (int j=0; j<bufPos.iSize(); ++j)
      if (bufPos[j] != iUNDEF)
        bufPos[j] = inst->env.nb[inst->env.nb.iSize()-bufPos[j]-1]+1;
    so->PutVal(bufPos);
    inst->stkCalc.push(so);
  }
}

InstNbSum::InstNbSum(Instructions* instrucs, const DomainValueRangeStruct& dvrs)
  : InstBase(instrucs)
{
  dvs = dvrs;
  sot = StackObject::sotype(dvs);
}

InstNbSum::~InstNbSum()
{
}

void InstNbSum::Exec()
{
  StackObject* so1 = inst->stkCalc.pop();
  StackObject* so2;
  StackObject* soRes= new StackObject(sot, dvs, inst->env.iBufSize);
  if (so1->sot == sotRealVal) {
    so1->Resize(bufR1);
    so1->GetVal(bufR1);
    for (int j=0; j<bufR1.iSize(); ++j)
			if (bufR1[j] == rUNDEF)
				bufR1[j] = 0;
    so1->Resize(bufR2);
    for (unsigned int i=1; i < inst->env.nb.iSize(); ++i) {
      so2 = inst->stkCalc.pop();
      so2->GetVal(bufR2);
      for (int j=0; j<bufR1.iSize(); ++j)
        if (bufR2[j] == rUNDEF)
          continue;
        else
          bufR1[j] += bufR2[j];
      delete so2;
    }
    soRes->PutVal(bufR1);
  }
  else {
    so1->Resize(bufL1);
    so1->GetVal(bufL1);
    for (int j=0; j<bufL1.iSize(); ++j)
			if (bufL1[j] == iUNDEF)
				bufL1[j] = 0;
    so1->Resize(bufL2);
    for (unsigned int i=1; i < inst->env.nb.iSize(); ++i) {
      so2 = inst->stkCalc.pop();
      so2->GetVal(bufL2);
      for (int j=0; j<bufL1.iSize(); ++j)
        if (bufL2[j] == iUNDEF)
          continue;
        else
          bufL1[j] += bufL2[j];
      delete so2;
    }
    soRes->PutVal(bufL1);
  }
  delete so1;
  inst->stkCalc.push(soRes);
}

InstNbAvg::InstNbAvg(Instructions* instrucs, const DomainValueRangeStruct& dvrs)
  : InstBase(instrucs)
{
  dvs = dvrs;
  sot = StackObject::sotype(dvs);
}

InstNbAvg::~InstNbAvg()
{
}

void InstNbAvg::Exec()
{
  StackObject* so1 = inst->stkCalc.pop();
  StackObject* so2;
  StackObject* soRes= new StackObject(sot, dvs, inst->env.iBufSize);
  
  bufCnt.Size(inst->env.iBufSize); // contains nr. of neighbours that are not undefined
  
  so1->Resize(bufR1);
  so1->GetVal(bufR1);
  so1->Resize(bufR2);
  for (int k=0; k < inst->env.iBufSize; ++k) 
    if (bufR1[k] == rUNDEF) {
      bufCnt[k] = 0;
      bufR1[k] = 0;
    }
    else
      bufCnt[k] = 1;
  for (unsigned int i=1; i < inst->env.nb.iSize(); ++i) {
    so2 = inst->stkCalc.pop();
    so2->GetVal(bufR2);
    for (int j=0; j<bufR1.iSize(); ++j)
      if (bufR2[j] != rUNDEF) {
        bufR1[j] += bufR2[j];
		    bufCnt[j]++;
      } 
    delete so2;
  }
  for (int j=0; j<bufR1.iSize(); ++j) 
    if (bufCnt[j] != 0)
	    bufR1[j] /= bufCnt[j];
	  else
      bufR1[j] = rUNDEF;
  soRes->PutVal(bufR1);
  delete so1;
  inst->stkCalc.push(soRes);
}

InstNbStd::InstNbStd(Instructions* instrucs, const DomainValueRangeStruct& dvrs)
  : InstBase(instrucs)
{
  dvs = dvrs;
  sot = StackObject::sotype(dvs);
}

InstNbStd::~InstNbStd()
{
}

void InstNbStd::Exec()
{
  StackObject* so1 = inst->stkCalc.pop();
  StackObject* so2;
  StackObject* soRes= new StackObject(sot, dvs, inst->env.iBufSize);
  
  bufCnt.Size(inst->env.iBufSize); // contains nr. of neighbours that are not undefined
  
  so1->Resize(bufR1);
  so1->GetVal(bufR1);
  so1->Resize(bufR2);
  so1->Resize(bufRR);
  for (int k=0; k < inst->env.iBufSize; ++k) 
    if (bufR1[k] == rUNDEF) {
      bufCnt[k] = 0;
      bufR1[k] = bufRR[k] = 0;
    }
    else {
      bufCnt[k] = 1;
      bufRR[k] = bufR1[k] * bufR1[k];
    }
  for (unsigned int i=1; i < inst->env.nb.iSize(); ++i) {
    so2 = inst->stkCalc.pop();
    so2->GetVal(bufR2);
    for (int j=0; j<bufR1.iSize(); ++j)
      if (bufR2[j] != rUNDEF) {
        bufR1[j] += bufR2[j];
        bufRR[j] += bufR2[j]*bufR2[j];
        bufCnt[j]++;
      } 
    delete so2;
  }
  for (int j=0; j<bufR1.iSize(); ++j) 
    if (bufCnt[j] > 1) 
     bufR1[j] = sqrt((bufCnt[j] * bufRR[j] - sqr(bufR1[j])) / bufCnt[j] / (bufCnt[j]-1));
   else
      bufR1[j] = rUNDEF;
  soRes->PutVal(bufR1);
  delete so1;
  inst->stkCalc.push(soRes);
}

InstNbPos::InstNbPos(Instructions* instrucs)
  : InstBase(instrucs)
{
  dvsNbPos = DomainValueRangeStruct(1, 9);
}

InstNbPos::~InstNbPos()
{
}

void InstNbPos::Exec()
{
  int iNbPos;
  if (inst->env.fNeighbAgg) {
    iNbPos = inst->env.iCurrNb;
    if ((inst->env.dnb == dirNbLEFT)||(inst->env.dnb == dirNbRIGHT))
      iNbPos = iNbOrdTrans[iNbPos];
    iNbPos += 1;
  }
  else
    iNbPos = shUNDEF;
  StackObject* so = new StackObject(sotLongVal, dvsNbPos, inst->env.iBufSize);
  so->Resize(bufPos);
  for (int i=0; i < bufPos.iSize(); ++i)
    bufPos[i] = iNbPos;
  so->PutVal(bufPos);
  inst->stkCalc.push(so);
}

InstNbDis::InstNbDis(Instructions* instrucs)
  : InstBase(instrucs)
{
  dvsNbDis = DomainValueRangeStruct(0, 1.4, 0.1);
}

InstNbDis::~InstNbDis()
{
}

void InstNbDis::Exec()
{
  double rDis;
  if (!inst->env.fNeighbAgg)
    rDis = rUNDEF;
  else
    if (inst->env.iCurrNb == 4)
      rDis = 0;
    else if (inst->env.iCurrNb % 2)
      rDis = 1;
    else
      rDis = 1.4;
  StackObject* so = new StackObject(sotRealVal, dvsNbDis, inst->env.iBufSize);
  so->Resize(bufDis);
  for (int i=0; i < bufDis.iSize(); ++i)
    bufDis[i] = rDis;
  so->PutVal(bufDis);
  inst->stkCalc.push(so);
}

InstNbFlt::InstNbFlt(Instructions* instrucs, FilterLinear* fltLin)
  : InstBase(instrucs)
{
  arFlt.Resize(9);
  int i;
  if (fltLin->fUseRealMat)
    for (i=0; i<9; ++i)
      arFlt[i] = fltLin->rMat(i / 3, i % 3) * fltLin->rGain;
  else
    for (i=0; i<9; ++i)
      arFlt[i] = fltLin->iMat(i / 3, i % 3) * fltLin->rGain;
  double rMin=DBL_MAX, rMax=-DBL_MAX;
  int iDec=0;
  for (i=0; i<9; ++i) {
    double r = arFlt[i];
    if (r < rMin)
      rMin = r;
    if (r > rMax)
      rMax = r;
    int iDc = 0;
    while (r - floor(r) > 1e-10) {
      r *= 10;
      iDc++;
      if (iDc > 5)
        break;
    }
    iDec = max(iDec, iDc);
  }
  double rStep = 1;
  for (i=0; i < iDec; ++i)
    rStep /= 10;
  dvsNbFlt = DomainValueRangeStruct(rMin, rMax, rStep);
}

InstNbFlt::~InstNbFlt()
{
}

void InstNbFlt::Exec()
{
  double rFlt;
  if (!inst->env.fNeighbAgg)
    rFlt = rUNDEF;
  else
    rFlt = arFlt[inst->env.iCurrNb];
  StackObject* so = new StackObject(sotRealVal, dvsNbFlt, inst->env.iBufSize);
  so->Resize(bufFlt);
  for (int i=0; i < bufFlt.iSize(); ++i)
    bufFlt[i] = rFlt;
  so->PutVal(bufFlt);
  inst->stkCalc.push(so);
}

InstNbCnt::InstNbCnt(Instructions* instrucs)
  : InstBase(instrucs)
{
  dvsCnt = DomainValueRangeStruct(0,9);
}

InstNbCnt::~InstNbCnt()
{
}

void InstNbCnt::Exec()
{
  StackObject* so;
  StackObject* soRes= new StackObject(sotLongVal, dvsCnt, inst->env.iBufSize);
  soRes->Resize(bufCond);
  soRes->Resize(bufCnt);
  for (int j=0; j<bufCnt.iSize(); ++j)
    bufCnt[j] = 0;
  for (unsigned int i=0; i < inst->env.nb.iSize(); ++i) {
    so = inst->stkCalc.pop();
    so->GetVal(bufCond);
    for (int j=0; j<bufCond.iSize(); ++j)
      if ((bufCond[j] != iUNDEF) && (bufCond[j] != 0))
        bufCnt[j]++;
    delete so;
  }
  soRes->PutVal(bufCnt);
  inst->stkCalc.push(soRes);
}

InstNbCondPos::InstNbCondPos(Instructions* instrucs)
  : InstBase(instrucs)
{
  dvsNbPos = DomainValueRangeStruct(1,9);
}

InstNbCondPos::~InstNbCondPos()
{
}

void InstNbCondPos::Exec()
{
  StackObject* so;
  StackObject* soRes= new StackObject(sotLongVal, dvsNbPos, inst->env.iBufSize);
  soRes->Resize(bufCond);
  soRes->Resize(bufPos);
  for (int j=0; j<bufPos.iSize(); ++j)
    bufPos[j] = iUNDEF;
  for (unsigned int i=0; i < inst->env.nb.iSize(); ++i) {
    so = inst->stkCalc.pop();
    so->GetVal(bufCond);
    for (int j=0; j<bufCond.iSize(); ++j)
      if ((bufCond[j] != iUNDEF) && (bufCond[j] != 0))
        bufPos[j] = inst->env.nb[inst->env.nb.iSize()-i-1]+1;
    delete so;
  }
  soRes->PutVal(bufPos);
  inst->stkCalc.push(soRes);
}




