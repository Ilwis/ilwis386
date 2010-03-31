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
/* CoordSystemTiePoints
   Copyright Ilwis System Development ITC
   april 1998, by Wim Koolhoven
	Last change:  JH   17 Sep 99    8:14 am
*/

#ifndef ILWCSTIEPOINTS_H
#define ILWCSTIEPOINTS_H
#include "Engine\SpatialReference\csctp.h"
#include "Engine\SpatialReference\SolveOblique.h"
#include "Engine\Base\Algorithm\Fpolynom.h"

class _export CoordSystemTiePoints: public CoordSystemCTP, private SolveOblique
{
  friend class CoordSystemPtr;
  friend class CoordSystemEditor;
public:
  CoordSystemTiePoints(const FileName&);
  CoordSystemTiePoints(const FileName&, const CoordSystem& csRef, const FileName& fnRefMap);
  ~CoordSystemTiePoints();
  virtual void Store();
  virtual String sType() const;
protected:
  virtual Coord cConvFromOther(const Coord&) const;
  virtual Coord cConvToOther(const Coord&) const;
private:
  String sFormula() const;
  virtual int Compute();
  enum Transf { CONFORM, AFFINE,                 
                SECONDORDER, FULLSECONDORDER,    
				THIRDORDER, PROJECTIVE } transf; 
  virtual int iMinNr() const;
  double rAvgX, rAvgY, rAvgRefX, rAvgRefY;
  double rCoeffX[10];
  double rCoeffY[10];
  double rCoeffRefX[10];
  double rCoeffRefY[10];
  Coord crdInverseOfAffine(const Coord& crd) const;
  Coord crdInverseOfSecondOrderBilinear(const Coord& crd) const;
  Coord crdInverseOfHigherOrder(const Coord& crd) const;
  Coord crdInverseOfProjective(const Coord& crd) const;
  mutable RealMatrix rmJR2C, rmJC2R;
  void MakeJacMatrix(const Coord &crdIn, RealMatrix &rmJ) const;
};

#endif // ILWCSTIEPOINTS_H
