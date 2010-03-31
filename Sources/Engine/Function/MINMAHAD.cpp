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
/* ClassifierMinMahaDist
   Copyright Ilwis System Development ITC
   June 1995, by Jelle Wind
	Last change:  JEL  18 Jun 97   11:36 am
*/

#include "Engine\Function\MINMAHAD.H"
#include "Engine\Base\DataObjects\ERR.H"
#include "Headers\Err\ILWISDAT.ERR"

const char* ClassifierMinMahaDist::sSyntax()
{
  return "ClassifierMinMahaDist()\nClassifierMinMahaDist(threshold)";
}

ClassifierMinMahaDist::~ClassifierMinMahaDist()
{
}

ClassifierMinMahaDist* ClassifierMinMahaDist::create(const FileName& fn, const String& sExpr)
{
  Array<String> as;
  int iParms = IlwisObjectPtr::iParseParm(sExpr, as);
  if (iParms > 1)
    ExpressionError(sExpr, sSyntax());
  double rThres = rUNDEF;
  if (iParms == 1)
    rThres = as[0].rVal();
 return new ClassifierMinMahaDist(fn, rThres);
}

ClassifierMinMahaDist::ClassifierMinMahaDist(const FileName& fn)
: ClassifierPtr(fn)
{
  rThreshold = rReadElement("ClassifierMinMahaDist", "Threshold");
  if (rThreshold != rUNDEF)
    rThreshold2 = sqr(rThreshold);
  else
    rThreshold2 = DBL_MAX;
}

ClassifierMinMahaDist::ClassifierMinMahaDist(const FileName& fn, double rThresh)
: ClassifierPtr(fn, Domain())
{
  rThreshold = rThresh;
  if ((rThreshold <= 0) && (rThreshold != rUNDEF))
    Classifier::ErrorThreshold(rThreshold, sTypeName());
  if (rThreshold != rUNDEF)
    rThreshold2 = sqr(rThreshold);
  else
    rThreshold2 = DBL_MAX;
}

void ClassifierMinMahaDist::Store()
{
  ClassifierPtr::Store();
  WriteElement("Classifier", "Type", "ClassifierMinMahaDist");
  if (rThreshold != rUNDEF)
    WriteElement("ClassifierMinMahaDist", "Threshold", rThreshold);
  else
    WriteElement("ClassifierMinMahaDist", "Threshold", (const char*)0);
}

String ClassifierMinMahaDist::sName(bool fExt, const String& sDirRelative) const
{
  String s = IlwisObjectPtr::sName(fExt, sDirRelative);
  if (s.length() != 0)
    return s;
  if (rThreshold != rUNDEF)
    return String("ClassifierMinMahaDist(%f)", rThreshold);
  else
    return String("ClassifierMinMahaDist()");
}

double ClassifierMinMahaDist::rAdd(int /*iClass*/)
{
  return 0;
}

void ClassifierMinMahaDist::Classify(const ByteBuf* bufListVal, ByteBuf& bufRes)
{
  if (iClasses != sms->dc()->iNettoSize())
    SetSampleSet(sms);
  double rDist, rDistX, rMinDist, r;
  byte bClass;
  for (int c=0; c < bufRes.iSize(); c++ ) {
    rMinDist = DBL_MAX;
    bClass = 0;
    try {
      for (unsigned int iOrd = 0; iOrd < bCl.iSize(); iOrd++) {
        byte cl = bCl[iOrd];
        int b;
        rDist = 0;
        long d, d1;
        // calc x-transposed * varcovinv * x :
        for (b=0; b < sms->iBands(); b++ ) {
          d = bufListVal[b][c] - (short)sms->smplS().bMean(cl, b);
          for (int b1=0; b1 < sms->iBands(); b1++ ) {
            d1 = bufListVal[b1][c] - (short)sms->smplS().bMean(cl, b1);
            r = varcovinv[cl](b, b1);
            r *= d*d1;
            rDist += r;
          }
        }
        rDistX = rDist + rAdd(cl);
        if ((rDistX < rMinDist) && (rDist < rThreshold2)) {
          bClass = cl;
          rMinDist = rDistX;
        }
      }
    }
    catch (ErrorObject&) 
    { // ignore matrix errors etc.
    }  
    bufRes[c] = bClass;
  }
}

void ClassifierMinMahaDist::Classify(const LongBuf* bufListVal, ByteBuf& bufRes)
{
  if (iClasses != sms->dc()->iNettoSize())
    SetSampleSet(sms);
  double rDist, rDistX, rMinDist;
  byte bClass;
  for (int c=0; c < bufRes.iSize(); c++ ) {
    rMinDist = DBL_MAX;
    bClass = 0;
    try {
      for (unsigned int iOrd = 0; iOrd < bCl.iSize(); iOrd++) {
        int cl = bCl[iOrd];
        for (int b=0; b < sms->iBands(); b++ )
          x[b] = bufListVal[b][c] - sms->smplS().rMean(cl, b);
        rDist = 0;
        // calc x-transposed * varcovinv * x :
        for (int b=0; b < sms->iBands(); b++ )
          for (int b1=0; b1 < sms->iBands(); b1++ )
            rDist += x[b] * x[b1] * varcovinv[cl](b, b1);
        rDistX = rDist + rAdd(cl);
        if ((rDistX < rMinDist) && (rDist < rThreshold2)) {
          bClass = cl;
          rMinDist = rDistX;
        }
      }
    }
    catch (ErrorObject&) 
    { // ignore matrix errors etc.
    }  
    bufRes[c] = bClass;
  }
}

void ClassifierMinMahaDist::Classify(const RealBuf* bufListVal, ByteBuf& bufRes)
{
  if (iClasses != sms->dc()->iNettoSize())
    SetSampleSet(sms);
  double rDist, rMinDist;
  byte bClass;
  for (int c=0; c < bufRes.iSize(); c++ ) {
    rMinDist = DBL_MAX;
    bClass = 0;
    for (unsigned int iOrd = 0; iOrd < bCl.iSize(); iOrd++) {
      int cl = bCl[iOrd];
      for (int b=0; b < sms->iBands(); b++ )
        x[b] = bufListVal[b][c] - sms->smplS().rMean(cl, b);
      rDist = 0;
      // calc x-transposed * varcovinv * x :
      for (int b=0; b < sms->iBands(); b++ )
        for (int b1=0; b1 < sms->iBands(); b1++ )
          rDist += x[b] * x[b1] * varcovinv[cl](b, b1);
      rDist += rAdd(cl);
      if ((rDist < rMinDist) && (rDist < rThreshold2)) {
        bClass = cl;
        rMinDist = rDist;
      }
    }
    bufRes[c] = bClass;
  }
}

void ClassifierMinMahaDist::SetSampleSet(const SampleSet& sms)
{
  ClassifierPtr::SetSampleSet(sms);
  for (unsigned int iOrd = 0; iOrd < bCl.iSize(); iOrd++) {
    int cl = bCl[iOrd];
    varcovinv[cl] = RealMatrix(sms->iBands());
    for (int i=0; i < sms->iBands(); i++) {
      for (int j=0; j < sms->iBands(); j++)
        if (j >= i) {
          double rTot = sms->smplSum().rPixInClass(cl);
          if (rTot != 0)
            varcovinv[cl](i, j) = sms->smplSumXY()(cl, i, j) / rTot -
                                 (sms->smplSum()(cl, i) / rTot) * (sms->smplSum()(cl, j) / rTot);
          else
            varcovinv[cl](i, j) = 0;
        }
        else
          varcovinv[cl](i, j) = varcovinv[cl](j, i);
    }
		if (varcovinv[cl].fSingular())
		{
			String sMessage = String("Singular Covariance Matrix found.\r\n");	
			sMessage &= String("Definite positive variances needed.\r\n");
			sMessage &= String("Class ");
			sMessage &= sms->dc()->sNameByRaw(cl,2);
			sMessage &= String(" needs more samples.\r\n");
			sMessage &= String(" Or bands are linearly dependent.");
			throw ErrorObject(WhatError(sMessage, errClassifier+2), sTypeName());
    }
		else
		 varcovinv[cl].Invert();
  }
  x.Resize(sms->iBands());
  xi.Resize(sms->iBands());
}


