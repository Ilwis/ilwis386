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
/* MapMovingAverage
   Copyright Ilwis System Development ITC
   september 1995, by Jelle Wind
   Last change:  JEL   8 May 97   11:37 am
*/

#ifndef ILWMAPMOVAVG_H
#define ILWMAPMOVAVG_H
#include "Engine\Map\Raster\Map.h"
#include "Engine\Applications\MAPFPNT.H"

#ifndef ILW_WEIGTHTP
#define ILW_WEIGTHTP
enum WeightFuncType {wfEXACT = 0, wfNOTEXACT };
#endif

IlwisObjectPtr * createMapMovingAverage(const FileName& fn, IlwisObjectPtr& ptr, const String& sExpr, vector<void *> parms=vector<void*>() );
ApplicationMetadata metadataMovingAverage(ApplicationQueryData *query);

class DATEXPORT MapMovingAverage: public MapFromPointMap
{
  friend DATEXPORT MapVirtual;
  friend DATEXPORT MapFromPointMap;
public:
  MapMovingAverage(const FileName& fn, MapPtr&);
  MapMovingAverage(const FileName& fn, MapPtr&, const PointMap& pmp, const GeoRef& gr,
                   WeightFuncType wft, double rWeightExp, double rLimDist, const bool fSpheric);
  ~MapMovingAverage();
  static MapMovingAverage* create(const FileName& fn, MapPtr&, const String& sExpression);
  static const char* sSyntax();
  static double _export rLimDistDefault(const PointMap& pmap);
  virtual String sExpression() const;
  virtual void Store();
  virtual bool fFreezing();
  virtual bool fDomainChangeable() const;
  virtual bool fValueRangeChangeable() const;
protected:
private:
  void Init();
  WeightFuncType wft;
  double rInvDist(double rDis);
  double rLinDecr(double rDis);
  double rWeightExp;
  double rLimDist;
  double rLimDist2;
private:  
  long iNrPoints;
  long iNrValidPnts;
  Array<Coord> cPoints; 
  Array<double> rAttrib;
};

#endif




