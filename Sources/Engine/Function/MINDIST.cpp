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
/* ClassifierMinDist
   Copyright Ilwis System Development ITC
   June 1995, by Jelle Wind
	Last change:  JEL  18 Jun 97   11:37 am
*/

#include "Engine\Function\MINDIST.H"
#include "Engine\Base\DataObjects\ERR.H"

const char* ClassifierMinDist::sSyntax()
{
  return "ClassifierMinDist()\nClassifierMinDist(threshold)";
}

ClassifierMinDist::~ClassifierMinDist()
{
}

ClassifierMinDist* ClassifierMinDist::create(const FileName& fn, const String& sExpr)
{
  Array<String> as;
  int iParms = IlwisObjectPtr::iParseParm(sExpr, as);
  if (iParms > 1)
    ExpressionError(sExpr, sSyntax());
  double rThres = rUNDEF;
  if (iParms == 1) 
    rThres = as[0].rVal();
  return new ClassifierMinDist(fn, rThres);
}

ClassifierMinDist::ClassifierMinDist(const FileName& fn)
: ClassifierPtr(fn)
{
  rThreshold = rReadElement("ClassifierMinDist", "Threshold");
  if (rThreshold != rUNDEF)
    rThreshold2 = sqr(rThreshold);
  else
    rThreshold2 = DBL_MAX;
}

ClassifierMinDist::ClassifierMinDist(const FileName& fn, double rThresh)
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

void ClassifierMinDist::Store()
{
  ClassifierPtr::Store();
  WriteElement("Classifier", "Type", "ClassifierMinDist");
  if (rThreshold != rUNDEF)
    WriteElement("ClassifierMinDist", "Threshold", rThreshold);
  else
    WriteElement("ClassifierMinDist", "Threshold", (const char*)0);
}    

String ClassifierMinDist::sName(bool fExt, const String& sDirRelative) const
{
  String s = IlwisObjectPtr::sName(fExt, sDirRelative);
  if (s.length() != 0)
    return s;
  if (rThreshold != rUNDEF)
    return String("ClassifierMinDist(%f)", rThreshold);
  else 
    return String("ClassifierMinDist()");
}

void ClassifierMinDist::Classify(const ByteBuf* bufListVal, ByteBuf& bufRes)
{
  if (iClasses != sms->dc()->iNettoSize())
    SetSampleSet(sms);
  double rDist, rMinDist;
  byte bClass;
  for (int c=0; c < bufRes.iSize(); c++ ) {
    rMinDist = DBL_MAX;
    bClass = 0;
    for (unsigned int iOrd = 0; iOrd < bCl.iSize(); iOrd++) {
      byte cl = bCl[iOrd];
      rDist = 0;
      long d;
      for (int b=0; b < sms->iBands(); b++ ) {
        d = bufListVal[b][c] - (short)sms->smplS().bMean(cl, b);
        d *= d;
        rDist += d;
      }
      if ((rDist < rMinDist) && (rDist < rThreshold2)) {
        bClass = cl;
        rMinDist = rDist;
      }  
    }
    bufRes[c] = bClass;
  }
}                              

void ClassifierMinDist::Classify(const LongBuf* bufListVal, ByteBuf& bufRes)
{
  if (iClasses != sms->dc()->iNettoSize())
    SetSampleSet(sms);
  double rDist, rMinDist;
  byte bClass;
  for (int c=0; c < bufRes.iSize(); c++ ) {
    rMinDist = DBL_MAX;
    bClass = 0;
    for (unsigned int iOrd = 0; iOrd < bCl.iSize(); iOrd++) {
      byte cl = bCl[iOrd];
      rDist = 0;
      for (int b=0; b < sms->iBands(); b++ ) 
        rDist += sqr(bufListVal[b][c] - sms->smplS().rMean(cl, b));
      if ((rDist < rMinDist) && (rDist < rThreshold2)) {
        bClass = cl;
        rMinDist = rDist;
      }  
    }
    bufRes[c] = bClass;
  }
}                              

void ClassifierMinDist::Classify(const RealBuf* bufListVal, ByteBuf& bufRes)
{
  if (iClasses != sms->dc()->iNettoSize())
    SetSampleSet(sms);
  double rDist, rMinDist;
  byte bClass;
  for (int c=0; c < bufRes.iSize(); c++ ) {
    rMinDist = DBL_MAX;
    bClass = 0;
    for (unsigned int iOrd = 0; iOrd < bCl.iSize(); iOrd++) {
      byte cl = bCl[iOrd];
      rDist = 0;
      for (int b=0; b < sms->iBands(); b++ ) 
        rDist += sqr(bufListVal[b][c] - sms->smplS().rMean(cl, b));
      if ((rDist < rMinDist) && (rDist < rThreshold2)) {
        bClass = cl;
        rMinDist = rDist;
      }  
    }
    bufRes[c] = bClass;
  }
}


