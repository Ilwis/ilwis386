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
/* $Log: /ILWIS 3.0/Function/BOX.cpp $
 * 
 * 6     15-03-01 18:46 Hendrikse
 * improved error message by giving Class name in st of number
 * 
 * 5     6-03-01 17:50 Hendrikse
 * added error message in case of box of zero volume (at least one std dev
 * == 0)
 * 
 * 4     27-11-00 11:23 Koolhoven
 * readability measures: removed "== true" and replaced "== false" by "!"
 * 
 * 3     23-05-00 18:35 Koolhoven
 * header comment
 * 
 * 2     5/23/00 2:50p Wind
 * array out of bounds (already in 2.2, but that didn't cause a crash)
// Revision 1.3  1998/09/16 17:30:34  Wim
// 22beta2
//
// Revision 1.2  1997/09/02 16:40:01  Wim
// Removed mbBoxMax and mbBoxMin
//
/* ClassifierBox
   Copyright Ilwis System Development ITC
   June 1995, by Jelle Wind
	Last change:  WK    2 Sep 97    6:39 pm
*/

#include "Engine\Function\BOX.H"
#include "Engine\SampleSet\SMPLHS.H"
#include "Engine\Base\DataObjects\ERR.H"
#include "Headers\Err\ILWISDAT.ERR"

const char* ClassifierBox::sSyntax()
{
  return "ClassifierBox(factor)";
}

ClassifierBox::~ClassifierBox()
{
}

ClassifierBox* ClassifierBox::create(const FileName& fn, const String& sExpr)
{
  Array<String> as(1);
  if (!IlwisObjectPtr::fParseParm(sExpr, as))
    ExpressionError(sExpr, sSyntax());
  double rThres = as[0].rVal();
  return new ClassifierBox(fn, rThres);
}


ClassifierBox::ClassifierBox(const FileName& fn)
: ClassifierPtr(fn)
{
  rFact = rReadElement("ClassifierBox", "Factor");
}

ClassifierBox::ClassifierBox(const FileName& fn, double rFactor)
: ClassifierPtr(fn, Domain())
{
  rFact = rFactor;
  if (rFact <= 0)
    throw ErrorObject(WhatError(String("Positive factor needed: %g", rFact), errClassifier+1), sTypeName());
}

void ClassifierBox::Store()
{
  ClassifierPtr::Store();
  WriteElement("Classifier", "Type", "ClassifierBox");
  WriteElement("ClassifierBox", "Factor", rFact);
}    

String ClassifierBox::sName(bool fExt, const String& sDirRelative) const
{
  String s = IlwisObjectPtr::sName(fExt, sDirRelative);
  if (s.length() != 0)
    return s;
  return String("ClassifierBox(%f)", rFact);
}


void ClassifierBox::Classify(const ByteBuf* bufListVal, ByteBuf& bufRes)
{
  bool fClass;
  byte bClass, cl;
  long iOrd;
  int b;

  if (iClasses != sms->dc()->iSize())
    SetSampleSet(sms);
  for (int c=0; c < bufRes.iSize(); c++ ) {
    for (iOrd = 1; iOrd <= sms->dc()->iSize(); iOrd++) {
      cl = abClassSeq[iOrd];
      fClass = true;
      bClass = 0;
      for (b=0; b < sms->iBands(); b++ ) {
        fClass = fClass && 
          (bufListVal[b][c] < mBoxMax(cl, b) &&
           bufListVal[b][c] > mBoxMin(cl, b));
// this is the 1.4 method with integer box-boundaries
//          (bufListVal[b][c] <= mBoxMax(cl, b) &&
//           bufListVal[b][c] >= mBoxMin(cl, b));
        if (!fClass) 
					break;
      }
      if (fClass) {
        bClass = cl;
        break;
      }  
    }
    bufRes[c] = bClass;
  }
}                              

void ClassifierBox::Classify(const LongBuf* bufListVal, ByteBuf& bufRes)
{
  bool fClass;
  byte bClass, cl;
  long iOrd;
  int b;

  if (iClasses != sms->dc()->iSize())
    SetSampleSet(sms);
  for (int c=0; c < bufRes.iSize(); c++ ) {
    for (iOrd = 1; iOrd <= sms->dc()->iSize(); iOrd++) {
      cl = abClassSeq[iOrd];
      fClass = true;
      bClass = 0;
      for (b=0; b < sms->iBands(); b++ ) {
        fClass = fClass && 
          (bufListVal[b][c] < mBoxMax(cl, b) &&
           bufListVal[b][c] > mBoxMin(cl, b));
// this is the 1.4 method with integer box-boundaries
//          (bufListVal[b][c] <= mBoxMax(cl, b) &&
//           bufListVal[b][c] >= mBoxMin(cl, b));
        if (!fClass) 
					break;
      }
      if (fClass) {
        bClass = cl;
        break;
      }  
    }
    bufRes[c] = bClass;
  }
}                              

void ClassifierBox::Classify(const RealBuf* bufListVal, ByteBuf& bufRes)
{
  bool fClass;
  long iOrd;
  byte bClass, cl;
  int b;
  if (iClasses != sms->dc()->iSize())
    SetSampleSet(sms);

  for (int c=0; c < bufRes.iSize(); c++ ) {
    for (iOrd = 1; iOrd <= sms->dc()->iSize(); iOrd++) {
      cl = abClassSeq[iOrd];
      fClass = true;
      bClass = 0;
      for (b=0; b < sms->iBands(); b++ ) {
        fClass = fClass && 
          (bufListVal[b][c] <= mBoxMax(cl, b) &&
           bufListVal[b][c] >= mBoxMin(cl, b));
// this is the 1.4 method with integer box-boundaries
//          (bufListVal[b][c] <= mBoxMax(cl, b) &&
//           bufListVal[b][c] >= mBoxMin(cl, b));
        if (!fClass) 
					break;
      }
      if (fClass) {
        bClass = cl;
        break;
      }  
    }
    bufRes[c] = bClass;
  }
}                                            

void ClassifierBox::SetSampleSet(const SampleSet& sms) 
{
  ClassifierPtr::SetSampleSet(sms);
  mBoxMax  = RealMatrix(sms->dc()->iSize()+1, sms->iBands());
  mBoxMin  = RealMatrix(sms->dc()->iSize()+1, sms->iBands());
  abClassSeq    = Array<byte>(sms->dc()->iSize()+1);
  Array<double> arSTDProducts(sms->dc()->iSize()+1);
  byte bSmallestClass;
  long iOrd;
  int b, cl, c1, c2;
  for (cl = 0; cl <= sms->dc()->iSize(); cl++) {
    arSTDProducts[cl] = 1;
  }
	cl = 0;
	double rM = rUNDEF;
  for (iOrd = 1; iOrd <= sms->dc()->iSize(); iOrd++) {
    cl = sms->dc()->iKey(iOrd);
    for (b=0; b < sms->iBands(); b++ ) { 
      arSTDProducts[cl] *= sms->smplS().rStd(cl, b);
			double rM = sms->smplS().rMean(cl, b);
			if (rM > 0 && arSTDProducts[cl]< 1.e-10) 
			{
				String sMessage = String("Positive standard deviation needed.\r\n");
				sMessage &= String("Class ");
				sMessage &= sms->dc()->sNameByRaw(cl,2);
				sMessage &= String(" needs more samples for ");
				sMessage &= String(" band %i .",b + 1);
				throw ErrorObject(WhatError(sMessage, errClassifier+2), sTypeName());
			}
		}
  }
  arSTDProducts[0] = DBL_MAX;
  for (c1 = 1; c1 <= sms->dc()->iSize(); c1++)
  {
    bSmallestClass = 0;
    for (c2 = 1; c2 <= sms->dc()->iSize(); c2++)
      if (arSTDProducts[c2] < arSTDProducts[bSmallestClass])
        bSmallestClass = c2;
    abClassSeq[c1] = bSmallestClass;
    arSTDProducts[bSmallestClass] = DBL_MAX;
  }
  if (rFact == rUNDEF) 
    rFact = DBL_MAX;
  for (iOrd = 1; iOrd <= sms->dc()->iSize(); iOrd++) {
    cl = sms->dc()->iKey(iOrd);
    for (b=0; b < sms->iBands(); b++ ) { 
// this is the 1.4 method with integer box-boundaries:
//      mBoxMax(cl, b) = round(ms->smplS().rMean(cl, b) + 
//                (rFact * ms->smplS().rStd(cl, b)));
//      mBoxMin(cl, b) = floor(ms->smplS().rMean(cl, b) - 
//                (rFact * ms->smplS().rStd(cl, b)));
      mBoxMax(cl, b) = sms->smplS().rMean(cl, b) + 
                (rFact * sms->smplS().rStd(cl, b));
      mBoxMin(cl, b) = sms->smplS().rMean(cl, b) - 
                (rFact * sms->smplS().rStd(cl, b));
//      mbBoxMax(cl, b) = max(0, min(byteConv(mBoxMax(cl, b)), 255));
//      mbBoxMin(cl, b) = max(0, min(byteConv(floor(mBoxMin(cl, b))), 255));
    }
  }
}


