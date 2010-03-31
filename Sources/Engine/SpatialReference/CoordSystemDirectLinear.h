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
/* CoordSystemDirectLinear.h
   Copyright Ilwis System Development ITC
   september 1999, by Jan Hendrikse
*/

#ifndef ILWCSDIRECTLINEAR_H
#define ILWCSDIRECTLINEAR_H

#include "Engine\SpatialReference\csctp.h"
#include "Engine\SpatialReference\SolveDirectLinear.h"
#include "Engine\Map\Raster\Map.h"

class _export CoordSystemDirectLinear: public CoordSystemCTP,private SolveDirectLinear
{
  friend class CoordSystemPtr;
  friend class _export CoordSystemEditor;
  CoordSystemDirectLinear(const FileName&);
public:  
  CoordSystemDirectLinear(const FileName&, const Map& mapDTM, const FileName& fnRefMap);
  ~CoordSystemDirectLinear();
  virtual String sType() const;
  virtual double rPixSize() const;
  virtual Coord cConvFromOther(const Coord&) const;
  virtual Coord cConvToOther(const Coord&) const;
  virtual void Store();
  virtual int Compute();
	void GetObjectStructure(ObjectStructure& os)	;	
  virtual int iMinNr() const; 		
  double rHeightAccuracy;
  int iMaxHeightCount; 
  Map mapDTM;
protected:
  CoordBounds CalcCoordBounds(MinMax& mm);
  CoordBounds cbDTM;
private:
  String sFormula() const;
  double rAvgScale; 
	void init();
  double rEstimTerrainHeight;  // Inilial terrain height guess
  double rPrevTerrainHeight;   // needed when segment digitizing in monoplot
};

#endif // ILWCSDIRECTLINEAR_H
