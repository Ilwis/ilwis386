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
/* MapKrigingFromRaster
   Copyright Ilwis System Development ITC
   october 1999, Jan Hendrikse
	Last change:  JH   22 Oct 99   11:47 am
*/

#ifndef MAPKRIGFROMRASTER_H
#define MAPKRIGFROMRASTER_H
#include "Engine\Map\Raster\Map.h"
#include "Engine\Applications\MAPFMAP.H"
#include "Engine\Base\Algorithm\semivar.h"
#include "Engine\Base\Algorithm\Octantmatrix.h"

IlwisObjectPtr * createMapKrigingFromRaster(const FileName& fn, IlwisObjectPtr& ptr, const String& sExpr, vector<void *> parms=vector<void*>() );

class DATEXPORT MapKrigingFromRaster: public MapFromMap
{
  friend DATEXPORT MapVirtual;
  friend DATEXPORT MapFromMap;
public:
	enum RadiusUnits{ ruPIXELS = 0, ruMETERS };
	static double _export rDefaultRadius(const Map& mp);
	static ValueRange _export vrDefault(const Map& mp);
    static MapKrigingFromRaster* create(const FileName& fn, MapPtr& p,
                                     const String& sExpression);
	MapKrigingFromRaster(const FileName& fn, MapPtr&);
protected:
  virtual void Store();
  MapKrigingFromRaster(const FileName& fn, MapPtr&,
             const Map& mp,
             const SemiVariogram& sv,
             const double rRad,
						 RadiusUnits ru,
						 const FileName& fnErrMap,
             const int iMin,
             const int iMax);
  ~MapKrigingFromRaster();
  static const char* sSyntax();

  virtual String sExpression() const;
  virtual bool fFreezing();
  virtual bool fValueRangeChangeable() const;
  virtual bool fDomainChangeable() const;
  virtual bool fGeoRefChangeable() const;
private:
  void Init();
  SemiVariogram svm;
	//FileName fnErrorMap;
  double rRadius;
	RadiusUnits ruUnits;
  int iMinNrSamples;
  int iMaxNrSamples;
  //bool fKrigingFromRaster(const DoubleOctantMatrix & omDistances, 
	//												const DoubleOctantMatrix & omVarioSurf);

};

#endif
