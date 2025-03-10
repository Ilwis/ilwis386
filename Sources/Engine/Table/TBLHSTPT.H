/***************************************************************
 ILWIS integrates image, vector and thematic data in one unique 
 and powerful package on the desktop. ILWIS delivers a wide 
 range of feautures including import/export, digitizing, editing, 
 analysis and display of data as well as production of 
 quality mapsinformation about the sensor mounting platform
 
 Exclusive rights of use by 52�North Initiative for Geospatial 
 Open Source Software GmbH 2007, Germany

 Copyright (C) 2007 by 52�North Initiative for Geospatial
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
/* TableHistogramPnt
   Copyright Ilwis System Development ITC
   oct. 1996, by Jelle Wind
	Last change:  JEL  15 May 97    1:56 pm
*/

#ifndef ILWTBLHISTPNT_H
#define ILWTBLHISTPNT_H
#include "Engine\Applications\TBLVIRT.H"
#include "Engine\Table\Col.h"
#include "Engine\Map\Point\PNT.H"

IlwisObjectPtr * createTableHistogramPnt(const FileName& fn, IlwisObjectPtr& ptr, const String& sExpr, vector<void *> parms );

class TableHistogramPnt: public TableVirtual
{
public:
friend class PointMapPtr;
friend class DATEXPORT TableVirtual;
  TableHistogramPnt(const FileName& fn, TablePtr&);
  TableHistogramPnt(PointMap&, TablePtr&);
  virtual void Store();
  virtual void UnFreeze();
public:
  const Column& colFreq() const { return _colFreq; }
  const Column& colFreqCum() const { return _colFreqCum; }
  const Column& colValue() const { return _colValue; }
  RangeInt riMinMax(double rPerc = 0) const;
  RangeReal rrMinMax(double rPerc = 0) const;
  virtual String sExpression() const;
  virtual bool fFreezing();
  static const char* sSyntax();
//virtual String sAdditionalInfo() const;
//virtual bool fAdditionalInfo() const;
  virtual String sType() const;
  static void _export Erase(const FileName& fnMap);
protected:
  PointMap map;
  long _iMin, _iMax;
  long iTotalFreq;
//double rMean;
//double rStd;
//double rMedian;
//double rPred;
//long iPredCount;
private:
  void Init();
  bool fCount();
  void FillColumns();
  RangeInt riIndexMinMax(double rPerc = 0) const;
  Column _colValue;
  Column _colFreq;
  Column _colFreqCum;
public:
  RealArrayLarge arSort;
};

#endif




