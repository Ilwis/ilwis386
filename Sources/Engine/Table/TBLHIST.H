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
/* $Log: /ILWIS 3.0/Table/TBLHIST.H $
 * 
 * 7     6/06/00 5:02p Wind
 * added function to acces table of histogram
 * 
 * 6     8-12-99 17:35 Koolhoven
 * Added HistogramWindow
 * which shows a histogram of a raster map
 * as a graph and as table at the same time.
 * 
 * 5     30-11-99 15:47 Koolhoven
 * Added TableHistogramInfo::colNPix()
 * 
 * 4     30-11-99 12:20 Wind
 * solved problems with TableHistogramInfo
 * 
 * 3     1-10-99 14:20 Koolhoven
 * Header comment
 * 
 * 2     10/01/99 1:37p Wind
 * support for histograms of map in maplist ODF 
// Revision 1.3  1998/09/16 17:25:32  Wim
// 22beta2
//
// Revision 1.2  1997/08/14 17:08:53  Wim
// Added alSort to calculate with raw values
//
/* TableHistogram
   Copyright Ilwis System Development ITC
   march 1995, by Wim Koolhoven
	Last change:  WK   14 Aug 97    6:54 pm
*/

#ifndef ILWTBLHIST_H
#define ILWTBLHIST_H
#include "Engine\Applications\TBLVIRT.H"
#include "Engine\Table\Col.h"
#include "Engine\Map\Raster\Map.h"

IlwisObjectPtr * createTableHistogram(const FileName& fn, IlwisObjectPtr& ptr, const String& sExpr, vector<void *> parms );

class TableHistogramInfo
{
public:
  _export TableHistogramInfo();
  _export TableHistogramInfo(const Map& mp, bool fIgnZero=false);
  _export void operator=(const TableHistogramInfo& inf);

  RangeInt _export riMinMax(double rPerc = 0) const;
  RangeReal _export rrMinMax(double rPerc = 0) const;
  Column _export colNPix() const;
  Column _export colNPixCum() const;
  Column _export colNPixCumPct() const;
  Column _export colValue() const;
  bool _export fValid() const;
	Table tbl() const { return his; }
private:
  Table his;
};

class TableHistogram: public TableVirtual
{
friend class MapPtr;
friend class DATEXPORT TableVirtual;
friend bool fLessRealVal(long a, long b, void* p);
public:
  TableHistogram(const FileName& fn, TablePtr&);
  TableHistogram(TablePtr&, const FileName& fnTbl, Map&, bool fIgnZero = false);
  virtual void Store();
  virtual void UnFreeze();
public:
	enum AggragationType{atSUM, atAVERAGE, atMAX, atMIN, atSTDEV, atMEDIAN, atPRED};
  const Column& colNPix() const { return _colNPix; }
  const Column& colNPixPct() const { return _colNPixPct; }
  const Column& colNPixCum() const { return _colNPixCum; }
  const Column& colNPixCumPct() const { return _colNPixCumPct; }
  const Column& colArea() const { return _colArea; }
  const Column& colValue() const { return _colValue; }
  RangeInt riMinMax(double rPerc = 0) const;
  RangeReal rrMinMax(double rPerc = 0) const;
  virtual String sExpression() const;
  virtual bool fFreezing();
  static const char* sSyntax();
//virtual String sAdditionalInfo() const;
//virtual bool fAdditionalInfo() const;
  String sSummary() const;
  virtual String sType() const;
	Map mp() const { return map; }
  static void _export Erase(const FileName& fnMap);
  double _export getAggregate(AggragationType type) const;
protected:
  Map map;
  long _iMin, _iMax;
  long iTotalPix;
  double rMean;
  double rStd;
  double rMedian;
  double rPred;
  long iPredCount;
private:
  void Init();
  bool fCount();
  void FillColumns();
  RangeInt riIndexMinMax(double rPerc = 0) const;
  Column _colValue, _colNPix, _colNPixPct, _colNPixCum,
         _colNPixCumPct, _colArea, _colNPixPctNotUndef;
public:
  RealArrayLarge arSort;
  LongArrayLarge alSort;
  bool _fIgnZero;
};

#endif // ILWTBLHIST_H





