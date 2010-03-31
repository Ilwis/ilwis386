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
   Semivariogram
   march 1998, by Wim Koolhoven
   (c) Ilwis System Development, ITC
	Last change:  WK   23 Apr 98    5:25 pm
*/
#include "Headers\toolspch.h"
#include "Engine\Base\Algorithm\semivar.h"
#include "Headers\Hs\Table.hs"
#include "Headers\Err\Ilwisapp.err"


static void ModelUnknown() {
  throw ErrorObject(WhatError(String(SDATErrModelUnknown), errSemiVariogram));
}
static void SemVarParamUndef() {
  throw ErrorObject(WhatError(String(SDATErrSemVarParamUndef), errSemiVariogram +1));
}
static void RangeNotPos() {
  throw ErrorObject(WhatError(String(SDATErrRangeNotPos), errSemiVariogram +2));
}
static void PowerNotPos() {
  throw ErrorObject(WhatError(String(SDATErrPowerNotPos), errSemiVariogram +3));
}
static void PowerAbove10() {
  throw ErrorObject(WhatError(String(SDATErrPowerAbove10), errSemiVariogram +4));
}


SemiVariogram::SemiVariogram()
: svm(svmSPHERICAL), rNugget(0), rSill(1), rRange(1000), rPower(1), rSlope(1)
{
}

SemiVariogram::SemiVariogram(const String& sExpr)
: svm(svmSPHERICAL), rNugget(0), rSill(1), rRange(1000), rPower(1), rSlope(1)
{
  String sModel = IlwisObjectPtr::sParseFunc(sExpr);
  Array<String> as;
  int iParms = IlwisObjectPtr::iParseParm(sExpr, as);
  if (fCIStrEqual("Spherical" , sModel))
    svm = svmSPHERICAL;
  else if (fCIStrEqual("Exponential" , sModel))
    svm = svmEXPONENTIAL;
  else if (fCIStrEqual("Gaussian" , sModel))
    svm = svmGAUSSIAN;
  else if (fCIStrEqual("Wave" , sModel))
    svm = svmWAVE;
  else if (fCIStrEqual("Circular" , sModel))
    svm = svmCIRCULAR;
  else if (fCIStrEqual("RatQuad" , sModel))
    svm = svmRATQUAD;
  else if (fCIStrEqual("Power" , sModel))
    svm = svmPOWER;
	else
    ModelUnknown();

  if (iParms < 1)
    return;
  
	rNugget = as[0].rVal();
  if (rUNDEF == rNugget)
    SemVarParamUndef();

	if (iParms < 2)
    return;
 if (svmPOWER == svm) 
 {
     rSlope = as[1].rVal();
     if (rUNDEF == rSlope)
       SemVarParamUndef();
  }     
  else {  
     rSill = as[1].rVal();
     if (rUNDEF == rSill)
       SemVarParamUndef();
  }    
  if (iParms < 3)
    return;
  if (svmPOWER == svm) 
	{
    rPower = as[2].rVal();
		if (rUNDEF == rPower)
      SemVarParamUndef();
		if (rPower <= 0)
      PowerNotPos();
		if (rPower > 10)
      PowerAbove10();  
  }    
  else 
	{
    rRange = as[2].rVal();
    if (rUNDEF == rRange)
      SemVarParamUndef();
    if (rRange <= 0)
      RangeNotPos();
  }    
}

SemiVariogram::SemiVariogram(const FileName& fn, const char* sSection)
: svm(svmSPHERICAL), rNugget(0), rSill(1), rRange(1000), rPower(1), rSlope(1)
{
  String sModel;
	ObjectInfo::ReadElement(sSection, "Model", fn, sModel);
  if (fCIStrEqual("Spherical" , sModel))
    svm = svmSPHERICAL;
  else if (fCIStrEqual("Exponential" , sModel))
    svm = svmEXPONENTIAL;
  else if (fCIStrEqual("Gaussian" , sModel))
    svm = svmGAUSSIAN;
  else if (fCIStrEqual("Wave" , sModel))
    svm = svmWAVE;
  else if (fCIStrEqual("Circular" , sModel))
    svm = svmCIRCULAR;  
  else if (fCIStrEqual("RatQuad" , sModel))
    svm = svmRATQUAD;
  else if (fCIStrEqual("Power" , sModel))
    svm = svmPOWER;
  ObjectInfo::ReadElement(sSection, "Nugget", fn, rNugget);
  if (svmPOWER == svm) {
    ObjectInfo::ReadElement(sSection, "Power", fn, rPower);
    ObjectInfo::ReadElement(sSection, "Slope", fn, rSlope);
  }
  else {
    ObjectInfo::ReadElement(sSection, "Sill", fn, rSill);
    ObjectInfo::ReadElement(sSection, "Range", fn, rRange);
  }
}

String SemiVariogram::sExpression() const
{
  String sModel;
  switch (svm) {
    case svmSPHERICAL:
      sModel = "Spherical"; break;
    case svmEXPONENTIAL:
      sModel = "Exponential"; break;
    case svmGAUSSIAN:
      sModel = "Gaussian"; break;
    case svmWAVE:
      sModel = "Wave"; break;
    case svmCIRCULAR:
      sModel = "Circular"; break;
    case svmRATQUAD:
      sModel = "RatQuad"; break;
    case svmPOWER:
      sModel = "Power"; break;
  }
	String sRes;
	if (svmPOWER == svm)
		sRes = String ("%S(%.3f,%.6f,%.3f)",
             sModel, rNugget, rSlope, rPower);
	else
		sRes = String("%S(%.3f,%.3f,%.3f)",
             sModel, rNugget, rSill, rRange);
  return sRes;
}

void SemiVariogram::Store(const FileName& fn, const char* sSection) const
{
  ObjectInfo::WriteElement(sSection, "Nugget", fn, rNugget);
  ObjectInfo::WriteElement(sSection, "Sill", fn, rSill);
  ObjectInfo::WriteElement(sSection, "Range", fn, rRange);
  ObjectInfo::WriteElement(sSection, "Power", fn, (char*)0);
  ObjectInfo::WriteElement(sSection, "Slope", fn, (char*)0);
  switch (svm) {
    case svmSPHERICAL:
      ObjectInfo::WriteElement(sSection, "Model", fn, "Spherical");
      break;
    case svmEXPONENTIAL:
      ObjectInfo::WriteElement(sSection, "Model", fn, "Exponential");
      break;
    case svmGAUSSIAN:
      ObjectInfo::WriteElement(sSection, "Model", fn, "Gaussian");
      break;
    case svmWAVE:
      ObjectInfo::WriteElement(sSection, "Model", fn, "Wave");
      break;
    case svmCIRCULAR:
      ObjectInfo::WriteElement(sSection, "Model", fn, "Circular");
      break;
    case svmRATQUAD:
      ObjectInfo::WriteElement(sSection, "Model", fn, "RatQuad");
      break;
    case svmPOWER:
      ObjectInfo::WriteElement(sSection, "Model", fn, "Power");
      ObjectInfo::WriteElement(sSection, "Slope", fn, rSlope);
      ObjectInfo::WriteElement(sSection, "Power", fn, rPower);
      ObjectInfo::WriteElement(sSection, "Sill", fn, (char*)0);
      ObjectInfo::WriteElement(sSection, "Range", fn, (char*)0);
      break;
  }
}

double SemiVariogram::rCalc(double rDist) const
{
  if (rUNDEF == rDist)
    return rUNDEF;
  if (rDist < 0)
    return rUNDEF;
  if (rDist == 0)
    return 0;
  if (rRange < 1e-12)
    return rSill;
  double rRet = 0;
  switch (svm) {
    case svmSPHERICAL:
      rDist /= rRange;
      if (rDist > 1)
        rRet = 1;
      else
        rRet = 1.5 * rDist - 0.5 * pow(rDist,3);
      break;
    case svmEXPONENTIAL:
      rDist /= rRange;
      rRet = 1 - exp(-rDist);
      break;
    case svmGAUSSIAN:
      rDist /= rRange;
      rRet = 1 - exp(-rDist*rDist);
      break;
    case svmWAVE:
      if (rDist > 0) {
        rDist /= rRange;
        rRet = 1 - sin(rDist) / rDist;
      }
      break;
    case svmCIRCULAR:
      rDist /= rRange;
      if (rDist > 1)
        rRet = 1;
      else
        rRet = 1 + 2 / M_PI * (- acos(rDist) + rDist * sqrt(1 - rDist*rDist));
      break;
    case svmPOWER:
      rRet = rSlope * pow(rDist, rPower);
      rRet += rNugget;
      return rRet;
    case svmRATQUAD:
      rDist /= rRange;
      rRet = rDist * rDist / (1 + rDist * rDist);
      break;
    default:
      return rUNDEF;
  }
  rRet *= rSill - rNugget;
  rRet += rNugget;
  return rRet;
}

String SemiVariogram::sModel() const
{
  switch (svm) {
    case svmSPHERICAL:
      return STBSvmSpherical;
    case svmEXPONENTIAL:
      return STBSvmExponential;
    case svmGAUSSIAN:
      return STBSvmGaussian;
    case svmWAVE:
      return STBSvmWave;
    case svmCIRCULAR:
      return STBSvmCircular;
    case svmPOWER:
      return STBSvmPower;
    case svmRATQUAD:
      return STBSvmRatQuad;
    default:
      return "Undefined";
  }
}





