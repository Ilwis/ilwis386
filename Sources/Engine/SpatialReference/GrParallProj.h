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
/* $Log: /ILWIS 3.0/GeoReference/GrParallProj.h $
 * 
 * 2     28-09-04 14:16 Hendrikse
 * Extended functionality to support sub-pixel precision in the tiepoints
 * and fiducials
 * 
 * 1     8-12-00 17:32 Hendrikse
  Copyright Ilwis System Development ITC
   december 2000, by Jan Hendrikse
	Last change:  JH    3 Dec 2000   12:59 pm
*/

#ifndef ILWGRPARALLPROJ_H
#define ILWGRPARALLPROJ_H
#include "Engine\SpatialReference\GRCTP.H"
#include "Engine\SpatialReference\SolveParallelProjective.h"
#include "Engine\Map\Raster\Map.h"
#include "Engine\Table\tblstore.h"
#include "Engine\Table\COLSTORE.H"

class DATEXPORT GeoRefParallelProjective: public GeoRefCTP, private SolveParallelProjective
{
  friend class GeoRefPtr;
  friend class _export GeoRefEditor;
  GeoRefParallelProjective(const FileName&);
public:
  _export GeoRefParallelProjective(const FileName&, const Map& mp, RowCol, bool fSubPixelPrecise);
  ~GeoRefParallelProjective();
  virtual String sType() const;
  virtual String sFormula() const;
  virtual double rPixSize() const;
  virtual void Coord2RowCol(const Coord& c, double& rR, double& rC) const;
  virtual void RowCol2Coord(double rR, double rC, Coord& c) const;
  virtual bool fEqual(const IlwisObjectPtr&) const;
  virtual void Store();
  virtual void GetDataFiles(Array<FileName>& afnDat, Array<String>* asSection=0, Array<String>* asEntry=0) const;
  double rHeightAccuracy;
  int iMaxOrientCount;
  int iMaxHeightCount; 
  virtual int Compute();
	virtual int iMinNr() const; 
  virtual void Rotate(bool fSwapRows, bool fSwapCols, bool fRotate);
	void GetObjectStructure(ObjectStructure& os)	;
  Map mapDTM;
	Column colZdiff;
	
private:
  CoordBounds CalcCoordBounds(MinMax& mm);
  double rEstimTerrainHeight; // 1st TerrainHeightGuess
  double rPrevTerrainHeight;
  void init();
};

#endif // ILWGRPARALLPROJ_H




