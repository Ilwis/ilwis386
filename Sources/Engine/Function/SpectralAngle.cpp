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
/* ClassifierSpectralAngle
   Copyright Ilwis System Development ITC
   June 1995, by Jan Hendrikse
	Last change:  JAN  17 Jun 01   11:37 am
*/

#include "Engine\Function\SpectralAngle.h"
#include "Engine\Base\DataObjects\ERR.H"
#define ONE_TOL	 1.00000000000001

const char* ClassifierSpectralAngle::sSyntax()
{
  return "ClassifierSpectralAngle()\nClassifierSpectralAngle(threshold)";
}

ClassifierSpectralAngle::~ClassifierSpectralAngle()
{
}

ClassifierSpectralAngle* ClassifierSpectralAngle::create(const FileName& fn, const String& sExpr)
{
  Array<String> as;
  int iParms = IlwisObjectPtr::iParseParm(sExpr, as);
  if (iParms > 1)
    ExpressionError(sExpr, sSyntax());
  double rThres = rUNDEF;
  if (iParms == 1) 
    rThres = as[0].rVal();
  return new ClassifierSpectralAngle(fn, rThres);
}

ClassifierSpectralAngle::ClassifierSpectralAngle(const FileName& fn)
: ClassifierPtr(fn)
{
  rThreshold = rReadElement("ClassifierSpectralAngle", "Threshold");
  if (rThreshold != rUNDEF)
    rThreshold2 = sqr(rThreshold);
  else
    rThreshold2 = DBL_MAX;
}

ClassifierSpectralAngle::ClassifierSpectralAngle(const FileName& fn, double rThresh)
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

void ClassifierSpectralAngle::Store()
{
  ClassifierPtr::Store();
  WriteElement("Classifier", "Type", "ClassifierSpectralAngle");
  if (rThreshold != rUNDEF)
    WriteElement("ClassifierSpectralAngle", "Threshold", rThreshold);
  else
    WriteElement("ClassifierSpectralAngle", "Threshold", (const char*)0);
}    

String ClassifierSpectralAngle::sName(bool fExt, const String& sDirRelative) const
{
  String s = IlwisObjectPtr::sName(fExt, sDirRelative);
  if (s.length() != 0)
    return s;
  if (rThreshold != rUNDEF)
    return String("ClassifierSpectralAngle(%f)", rThreshold);
  else 
    return String("ClassifierSpectralAngle()");
}

double ClassifierSpectralAngle::aacos(double v) 
{
	double av;

	if ((av = abs(v)) >= 1.) {
		if (av > ONE_TOL)
			return rUNDEF;
		return (v < 0. ? M_PI : 0.);
	}
	return acos(v);
}

void ClassifierSpectralAngle::Classify(const ByteBuf* bufListVal, ByteBuf& bufRes)
{
  if (iClasses != sms->dc()->iSize())
    SetSampleSet(sms);
  double rAngle, rMinAngle;
  double rT, rM; 
  byte bClass;
  for (int c=0; c < bufRes.iSize(); c++ ) {
    rMinAngle = DBL_MAX;
    bClass = 0;
    for (int iOrd = 0; iOrd < bCl.iSize(); iOrd++) {
      byte cl = bCl[iOrd];
      rAngle = 0;
      //long d;
			double rSumT2 = 0; // norm terms for vector T
			double rSumM2 = 0; // norm terms for M
			double rSumTM = 0; // inproduct terms
      for (int b=0; b < sms->iBands(); b++ ) {
				rT = bufListVal[b][c];
				rM = sms->smplS().rMean(cl, b);
				rSumT2 += rT * rT;
				rSumM2 += rM * rM;
				rSumTM += rT * rM;
			}
      rAngle = aacos(rSumTM / sqrt(rSumT2 * rSumM2)); 
      if ((rAngle < rMinAngle) && (rAngle < rThreshold)) {
        bClass = cl;
        rMinAngle = rAngle;
      }  
    }
    bufRes[c] = bClass;
  }
}                              

void ClassifierSpectralAngle::Classify(const LongBuf* bufListVal, ByteBuf& bufRes)
{
  if (iClasses != sms->dc()->iSize())
    SetSampleSet(sms);
  double rAngle, rMinAngle;
  double rT, rM;
  byte bClass;
  for (int c=0; c < bufRes.iSize(); c++ ) {
    rMinAngle = DBL_MAX;
    bClass = 0;
    for (int iOrd = 0; iOrd < bCl.iSize(); iOrd++) {
      byte cl = bCl[iOrd];
      //rDist = 0;
			double rSumT2 = 0; // norm terms for vector T
			double rSumM2 = 0; // norm terms for M
			double rSumTM = 0; // inproduct terms
      for (int b=0; b < sms->iBands(); b++ ){
				rT = bufListVal[b][c];
				rM = sms->smplS().rMean(cl, b);
				rSumT2 += rT * rT;
				rSumM2 += rM * rM;
				rSumTM += rT * rM;
			}
      rAngle = aacos(rSumTM / sqrt(rSumT2 * rSumM2)); 
      if ((rAngle < rMinAngle) && (rAngle < rThreshold)) {
        bClass = cl;
        rMinAngle = rAngle;
      }  
    }
    bufRes[c] = bClass;
  }
}                              

void ClassifierSpectralAngle::Classify(const RealBuf* bufListVal, ByteBuf& bufRes)
{
  if (iClasses != sms->dc()->iSize())
    SetSampleSet(sms);
  double rAngle, rMinAngle;
  double rT, rM;
  byte bClass;
  for (int c=0; c < bufRes.iSize(); c++ ) {
    rMinAngle = DBL_MAX;
    bClass = 0;
    for (int iOrd = 0; iOrd < bCl.iSize(); iOrd++) {
      byte cl = bCl[iOrd];
      rAngle = 0;
			double rSumT2 = 0; // norm terms for vector T
			double rSumM2 = 0; // norm terms for M
			double rSumTM = 0; // inproduct terms
      for (int b=0; b < sms->iBands(); b++ ) {
				rT = bufListVal[b][c];
				rM = sms->smplS().rMean(cl, b);
				rSumT2 += rT * rT;
				rSumM2 += rM * rM;
				rSumTM += rT * rM;
			}
      rAngle = aacos(rSumTM / sqrt(rSumT2 * rSumM2)); 
			//angle between vectors T and class mean M in iBands()-dimensional space
      if ((rAngle < rMinAngle) && (rAngle < rThreshold)) {
        bClass = cl;
        rMinAngle = rAngle;
      }  
    }
    bufRes[c] = bClass;
  }
}


