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
/* Classifier, ClassifierPtr
   Copyright Ilwis System Development ITC
   june 1995, by Jelle Wind
	Last change:  JEL  18 Jun 97   11:31 am
*/

#ifndef ILWCLASSIFY_H
#define ILWCLASSIFY_H
#include "Engine\Function\FUNCTION.H"
#include "Engine\SampleSet\SAMPLSET.H"

class DATEXPORT SampleSet;

class DATEXPORT ClassifierPtr: public FunctionPtr
{
  friend class DATEXPORT Classifier;
  friend class DATEXPORT FunctionPtr;
  static ClassifierPtr* create(const FileName&);
  static ClassifierPtr* create(const FileName&, const String& sExpression);
public:
  virtual ~ClassifierPtr();
  virtual void Store();
  virtual String sType() const;
  virtual void Classify(const ByteBuf* bufListVal, ByteBuf& bufRes)=0;
  virtual void Classify(const LongBuf* bufListVal, ByteBuf& bufRes)=0;
  virtual void Classify(const RealBuf* bufListVal, ByteBuf& bufRes)=0;
  virtual void SetSampleSet(const SampleSet& sms); 
protected:
  ClassifierPtr(const FileName&);
  ClassifierPtr(const FileName&, const Domain& dmDef);
  SampleSet sms;
  bool fUseClass[65];
  int iClasses;
  ByteArray bCl;
};

class DATEXPORT Classifier: public Function
{
protected:
  Classifier(IlwisObjectPtr* ptr);
public:
  _export Classifier();
  _export Classifier(const FileName& filename);
  Classifier(const FileName& filename,const String& sExpression);
  _export Classifier(const Classifier& clf);
  _export Classifier(const String& sExpression);
  _export Classifier(const String& sExpression, const String& sPath);
  void operator = (const Classifier& clf) { SetPointer(clf.pointer()); }
  ClassifierPtr* ptr() const { return static_cast<ClassifierPtr*>(pointer()); }
  ClassifierPtr* operator -> () const { return ptr(); }
  static void ErrorThreshold(double rThres, const String& sObjName);
};

#endif




