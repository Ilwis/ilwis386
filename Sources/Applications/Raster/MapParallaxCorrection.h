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

#ifndef ILWMAPParallaxCorrection_H
#define ILWMAPParallaxCorrection_H
//#include "Engine\Applications\MAPFMAP.H"
#include "Mapresmp.h"

IlwisObjectPtr * createMapParallaxCorrection(const FileName& fn, IlwisObjectPtr& ptr, const String& sExpr, vector<void *> parms=vector<void*>() );

class DATEXPORT MapParallaxCorrection: public MapResample
{
  //friend MapFromMap;
public:  
  static const char* sSyntax();  
  virtual bool fFreezing();
  virtual String sExpression() const;
  static MapParallaxCorrection* create(const FileName& fn, MapPtr& p, const String& sExpr);
  MapParallaxCorrection(const FileName& fn, MapPtr& p);
protected:
  virtual void Store();
  MapParallaxCorrection(const FileName& fn, MapPtr& p, const Map& map, const Map & _dem, ResampleMethod rsm, double _rLatSat, double _rLonSat, bool fFill);
  ~MapParallaxCorrection();
private:
  void ComputeLocation(const double hcloud, const double fcloud, const double lcloud, double & fcloudcorr, double & lcloudcorr);
  void fillUndef(long iRow1, long iCol1, long iRow2, long iCol2, Coord * matrix, long iMatrixXSize);
  void setUndef(long iRow1, long iCol1, long iRow2, long iCol2, Coord * matrix, long iMatrixXSize);
  void fillWithNearest(long i, long j, RowCol & fillPixel, Coord * matrix, long iMatrixXSize);
  Map dem;
  bool fFillUndef;
  double rLatSatellite;
  double rLonSatellite;
};

#endif // ILWMAPParallaxCorrection_H




