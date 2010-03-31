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
/* ClassifierBox
   Copyright Ilwis System Development ITC
   june 1995, by Jelle Wind
	Last change:  WK    2 Sep 97    6:39 pm
*/

#ifndef ILWCLASSBOX_H
#define ILWCLASSBOX_H
#include "Engine\Function\CLASSIFR.H"
#include "Engine\Base\Algorithm\Basemat.h"

class DATEXPORT ClassifierBox: public ClassifierPtr
{
  friend class DATEXPORT ClassifierPtr;
  static ClassifierBox* create(const FileName&, const String& sExpr);
public:
  virtual ~ClassifierBox();
  static const char* sSyntax();
  virtual void Store();
  virtual void Classify(const ByteBuf* bufListVal, ByteBuf& bufRes);
  virtual void Classify(const LongBuf* bufListVal, ByteBuf& bufRes);
  virtual void Classify(const RealBuf* bufListVal, ByteBuf& bufRes);
  virtual String sName(bool fExt, const String& sDirRelative) const;
  virtual void SetSampleSet(const SampleSet& sms); 
protected:
  ClassifierBox(const FileName&);
  ClassifierBox(const FileName&, double rFact);
  double rFact;
  // contains per class per band the min and max of the box:
  RealMatrix mBoxMax;
  RealMatrix mBoxMin;
//  BaseMatrix<byte> mbBoxMax;
//  BaseMatrix<byte> mbBoxMin;
  // contains the sequence of classes in ascending standard
  // deviation product order:
  Array<byte>  abClassSeq;
};

#endif


