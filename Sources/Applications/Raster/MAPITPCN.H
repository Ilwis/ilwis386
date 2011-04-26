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
/* MapInterpolContour
   Copyright Ilwis System Development ITC
   august 1995, by Jelle Wind
   Last change:  JEL   7 May 97    9:25 am
*/

#ifndef ILWMAPITPCNT_H
#define ILWMAPITPCNT_H
#include "Engine\Map\Segment\Seg.h"
#include "Engine\Applications\MAPFMAP.H"

struct tcand {
  int iDistToNearestContour;
  double rNearestContour;
  tcand()
    { iDistToNearestContour = shUNDEF; rNearestContour = rUNDEF; }
};

inline tcand undef(tcand)
{
  return tcand();
}

IlwisObjectPtr * createMapInterpolContour(const FileName& fn, IlwisObjectPtr& ptr, const String& sExpr, vector<void *> parms=vector<void*>() );
ApplicationMetadata metadataMapInterpolContour(ApplicationQueryData *query);

class DATEXPORT MapInterpolContour: public MapFromMap
{
  friend class DATEXPORT MapVirtual;  
  friend class DATEXPORT MapFromMap;  
public:
  static const char* sSyntax();
	static ValueRange _export vrDefault(const SegmentMap& segmap);
  virtual void Store();
  static MapInterpolContour* create(const FileName&, MapPtr& p, const String& sExpression);
  MapInterpolContour(const FileName&, MapPtr& p);
protected:
  MapInterpolContour(const FileName&, MapPtr& p, const Map& map);
  MapInterpolContour(const FileName&, MapPtr& p, const Map& map, const SegmentMap& sm, 
                const GeoRef& gr);
  ~MapInterpolContour();
  virtual String sExpression() const;
  virtual bool fFreezing();
  virtual bool fDomainChangeable() const;
  virtual bool fValueRangeChangeable() const;
private:
  void Init();
  bool fForwardDistances(long& iChanges, bool fFirstPass);
  bool fBackwardDistances(long& iChanges);
  bool fInterpolate();
  bool fSkip(long iCurLine) const;  
  Map mpSegRas;
  File *filTemp;
  IntBuf iCurrDistToNearestContour1;
  RealBuf rCurrNearestContour1;
  IntBuf iCurrDistToNearestContour2;
  RealBuf rCurrNearestContour2;
  IntBuf iPrevDistToNearestContour1;
  RealBuf rPrevNearestContour1;
  IntBuf iPrevDistToNearestContour2;
  RealBuf rPrevNearestContour2;
  Array<tcand> cand; //temporary storage of candidates distances and
                     // candidates values of the nearest contour
  ArrayLarge<bool> fLineChanged;
  IlwisAppContext* ilwapp;
  SegmentMap segmap;
  GeoRef grf;
};

#endif




