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
/* ClassifierPtr
   Copyright Ilwis System Development ITC
   June 1995, by Jelle Wind
	Last change:  J Hendrikse    21 Oct 02   10:03 am
*/

#include "Engine\Function\CLASSIFR.H"
#include "Engine\Function\BOX.H"
#include "Engine\Function\MINDIST.H"
#include "Engine\Function\MINMAHAD.H"
#include "Engine\Function\MXLIKELI.H"
#include "Engine\Function\SpectralAngle.h"
#include "Engine\Function\PRIORPRB.H"
#include "Engine\Base\DataObjects\ERR.H"
#include "Engine\Base\System\mutex.h"
#include "Engine\Applications\objvirt.h"
#include "Engine\Applications\ModuleMap.h"
#include "Engine\Base\System\Engine.h"
#include "Headers\Err\ILWISDAT.ERR"
#include "Headers\Hs\DAT.hs"


Classifier::Classifier()
: Function()
{
}

Classifier::Classifier(const Classifier& flt)
: Function(flt.pointer())
{
}

Classifier::Classifier(const FileName& filename)
: Function(filename)
{
  if (!pointer())
    SetPointer(ClassifierPtr::create(filename));
}

Classifier::Classifier(IlwisObjectPtr* ptr)
: Function(ptr)
{
}

Classifier::Classifier(const String& sExpression)
: Function(ClassifierPtr::create(FileName(), sExpression))
{}

Classifier::Classifier(const String& sExpression, const String& sPath)
: Function(ClassifierPtr::create(FileName::fnPathOnly(sPath), sExpression))
{}

Classifier::Classifier(const FileName& filename, const String& sExpression)
: Function(ClassifierPtr::create(filename, sExpression))
{}

ClassifierPtr::~ClassifierPtr()
{
}

String ClassifierPtr::sType() const
{
  return "Classifier";
}

ClassifierPtr* ClassifierPtr::create(const FileName& fn)
{
  FileName filnam = fn;
  if (!File::fExist(filnam)) { // check std dir
    filnam.Dir(getEngine()->getContext()->sStdDir());
    if (!File::fExist(filnam)) 
      NotFoundError(fn);
  }
  MutexFileName mut(fn);
  ClassifierPtr* p = static_cast<ClassifierPtr*>(Function::pGet(fn));
  if (p) // if already open return it
    return p;
  String sType;
  ObjectInfo::ReadElement("Classifier", "Type", fn, sType);
  if ("ClassifierBox" == sType)
    return new ClassifierBox(fn);
  if ("ClassifierMinDist" == sType)
    return new ClassifierMinDist(fn);
  if ("ClassifierMinMahaDist" == sType)
    return new ClassifierMinMahaDist(fn);
  if ("ClassifierMaxLikelihood" == sType)
    return new ClassifierMaxLikelihood(fn);
	if ("ClassifierSpectralAngle" == sType)
    return new ClassifierSpectralAngle(fn);
  if ("ClassifierPriorProb" == sType)
    return new ClassifierPriorProb(fn);
  InvalidTypeError(fn, "Classifier", sType);
  return 0;
}

ClassifierPtr* ClassifierPtr::create(const FileName& fn, const String& sExpression)
{
  String sFunc = IlwisObjectPtr::sParseFunc(sExpression);
  if (sFunc == sUNDEF && fn.sFile.length() == 0)
    return ClassifierPtr::create(FileName(sExpression,".clf",true));
  if (fCIStrEqual(sFunc, "ClassifierBox"))
    return ClassifierBox::create(fn, sExpression);
  if (fCIStrEqual(sFunc, "ClassifierMinDist"))
    return ClassifierMinDist::create(fn, sExpression);
  if (fCIStrEqual(sFunc, "ClassifierMinMahaDist"))
    return ClassifierMinMahaDist::create(fn, sExpression);
  if (fCIStrEqual(sFunc, "ClassifierMaxLikelihood"))
    return ClassifierMaxLikelihood::create(fn, sExpression);
	if (fCIStrEqual(sFunc, "ClassifierSpectralAngle"))
    return ClassifierSpectralAngle::create(fn, sExpression);
  if (fCIStrEqual(sFunc, "ClassifierPriorProb"))
    return ClassifierPriorProb::create(fn, sExpression);
  NotFoundError(String("Classifier: %S", sExpression));
  return 0;
}

ClassifierPtr::ClassifierPtr(const FileName& fn)
: FunctionPtr(fn)
{
  iClasses = 0;
}

ClassifierPtr::ClassifierPtr(const FileName& fn, const Domain& dmDef)
: FunctionPtr(fn, dmDef)
{
  iClasses = 0;
}

void ClassifierPtr::Store()
{
  FunctionPtr::Store();
  WriteElement("Function", "Type", "Classifier");
}    

void ClassifierPtr::SetSampleSet(const SampleSet& smpls) 
{
  bCl.Resize(0);
  sms = smpls;
  iClasses = sms->dc()->iNettoSize();
  for (int iOrd = 1; iOrd <= iClasses; iOrd++) {
    int cl = sms->dc()->iKey(iOrd);
    double rTot = sms->smplSum().rPixInClass(cl);
    fUseClass[cl] = rTot != 0;
    if (fUseClass[cl])
      bCl &= cl;
  }  
}

void Classifier::ErrorThreshold(double rThres, const String& sObjName)
{
  throw ErrorObject(WhatError(String("%S %g", TR("Positive threshold needed:"), rThres), errClassifier), sObjName);
}
