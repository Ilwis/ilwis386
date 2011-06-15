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
/* MapKriging
   Copyright Ilwis System Development ITC
   april 1998, Jan Hendrikse
   Last change:  H     2 Jan 99   10:29 pm
*/

#ifndef MAPKRIGING_H
#define MAPKRIGING_H
#include "Engine\Map\Raster\Map.h"
#include "Engine\Applications\MAPFPNT.H"
#include "Engine\Base\Algorithm\semivar.h"

IlwisObjectPtr * createMapKriging(const FileName& fn, IlwisObjectPtr& ptr, const String& sExpr, vector<void *> parms=vector<void*>() );
ApplicationMetadata metadataMapKrigingSimple(ApplicationQueryData *query) ;
ApplicationMetadata metadataMapKrigingOrdinary(ApplicationQueryData *query);


class DATEXPORT MapKriging: public MapFromPointMap
{
  friend DATEXPORT MapVirtual;
  friend DATEXPORT MapFromPointMap;
  friend class MapKrigingCreate;
  enum KrigingMethod { kmSIMPLE = 0, kmORDINARY, kmANISOTROPIC, kmUNIVERSAL };
  enum DuplicRemoveMethod { drmNO = 0, drmAVERAGE, drmFIRSTVAL };
public:
	static ValueRange  _export vrDefault(const PointMap& pmap);
	static MapKriging* create(const FileName& fn, MapPtr& p, const String& sExpression);
	MapKriging(const FileName& fn, MapPtr&);
  static const char* sSyntax();
protected:
  virtual void Store();
  MapKriging(const FileName& fn, MapPtr&, const PointMap& pm, const GeoRef& gr,
             const SemiVariogram& smv,
             double rLimDist, long iDegr,
             double rAzim, double rRati,
             const FileName& fnErrMap, long iMin, long iMax,
             DuplicRemoveMethod drm, double rToler,
             KrigingMethod km,
             const bool fSpheric);
  ~MapKriging();
	virtual String sExpression() const;
  virtual bool fFreezing();
  virtual bool fValueRangeChangeable() const;
  virtual bool fDomainChangeable() const;
private:
  void Init();
  SemiVariogram svm;
  long iNrPoints, iMRow, iMCol;
  double rLimDist;
  long iDegree;
  double rAzimuth;
  double rRatio;   // if rRatio != 0 geometric  else zonal anisotropy
  //FileName fnErrorMap; defined now at basemapptr level
	//MapKrigingError mpKrigingError;
  long iMinNrPoints;
  long iMaxNrPoints;
  KrigingMethod krigMeth;
  DuplicRemoveMethod duplicRemoval;
  double rTolerance;
  bool fKrigingSimple(Array<Array<double> > & aaH2, Map & mpError);
  bool fKrigingUniversal(Array<Array<double> > & aaH2, Map & mpError);
  //Coord crdEllipTransform(const Coord& crd, const double rAngle,
  //                                       const double rRatio);
  double rDist2Ellip(const Coord& crd1, const Coord& crd2,
                     const double rAzimuth, const double rRatio);
  long iNrValidPnts;
  ArrayLarge<Coord> cPoints;
  ArrayLarge<double> rAttrib;
};

#endif
