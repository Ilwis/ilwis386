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
/* MapResample
   Copyright Ilwis System Development ITC
   Last change:  WK   15 Oct 98    7:32 pm
*/

#ifndef ILWMAPRESAMPLE_H
#define ILWMAPRESAMPLE_H
#include "Engine\Applications\MAPFMAP.H"
#include "Engine\Map\Polygon\POL.H"
#define MAX_SIZE_PATCH_BUFFER 10

IlwisObjectPtr * createMapResample(const FileName& fn, IlwisObjectPtr& ptr, const String& sExpr, vector<void *> parms=vector<void*>() );
String metadataResample();
                        
enum ResampleMethod { rmNEARNEIGHB, rmBILINEAR, rmBICUBIC };                         


class DATEXPORT GeoRef3D;

class DATEXPORT MapResample: public MapFromMap
{
  friend MapFromMap;
public:  
  static const char* sSyntax();  
  virtual bool fFreezing();
  virtual String sExpression() const;
  static MapResample* create(const FileName& fn, MapPtr& p, const String& sExpr);
  MapResample(const FileName& fn, MapPtr& p);
protected:
  virtual void Store();
  MapResample(const FileName& fn, MapPtr& p, const Map& map, const GeoRef& gr,
              ResampleMethod rsm, bool fPatch, bool fGeoRefNoneAllowed);
  ~MapResample();
  static ResampleMethod rmType(const String& sRm);
  static String sResampleMethod(ResampleMethod rm);
  ResampleMethod rm;
  bool fValueRangeChangeable() const;
private:
  double rBiLinear(const Coord& cOutCoord);
  long iBiLinearColor(const Coord& cOutCoord);
  double rBiCubic(const Coord& cOutCoord);
  double rBiCubicResult(long iRow, long iCol, const double& rDCol);
  double rBiCubicPolynom(double rPar[], const double& rDeltaRow);
  double rBiCubicPolynom(const double& a, const double& b, const double& c, const double&d, const double& rDeltaRow);
  bool fResolveRealUndefs(double& a, double& b, double& c, double& d);
  bool fResolveRealUndefs(double rPar[]);
  void Init();
  double rRealFromPatch(RowCol& rc);
  long iRawFromPatch(RowCol& rc);
  bool fTransformCoords;
  CoordSystem csIn, csOut;
  long iRow[4], iCol[4];
  double rWeight[4], rValRow[4], rValCol[4];
  long iInpLines, iInpCols;
  bool fPatch;
  Map mpInpMap;
  LongPatch     **iPatchBuffer;
  RealPatch     **rPatchBuffer;
  long          iPatchPresent[MAX_SIZE_PATCH_BUFFER];

  long iNrPatchCols;
  long iNrPatchRows;
  long iNrPatches;
};

#endif




