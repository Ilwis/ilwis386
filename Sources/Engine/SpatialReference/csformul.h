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
/* CoordSystemFormula
   Copyright Ilwis System Development ITC
   april 1998, by Wim Koolhoven
	Last change:  WK   17 Apr 98   12:09 pm
*/

#ifndef ILWCSFORMULA_H
#define ILWCSFORMULA_H
#include "Engine\SpatialReference\csdirect.h"
#include "Engine\SpatialReference\prj.h"
#include "Engine\Scripting\Instrucs.h"

class CoordSystemFormula: public CoordSystemDirect
{
  friend class _export CoordSysFormulaView;
public:
  CoordSystemFormula(const FileName&);
  _export CoordSystemFormula(const FileName&, const CoordSystem& csRef);
  ~CoordSystemFormula();
  virtual void Store();
  virtual String sType() const;
protected:
  virtual bool fInverse() const;
  virtual Coord cConvFromOther(const Coord&) const;
  virtual Coord cConvToOther(const Coord&) const;
private:
  enum csfTransf{ eConformal, eDiffScaling, eSkewX, eSkewY, eAffine, eUserDef } eTransf;
  double a11, a12, a21, a22;
  double rDeterm() const;
// Conformal:
  double rScale, rRot;
// DiffScaling:
  double rScaleX, rScaleY;
// Shear Deformation:
  double rSkewX, rSkewY;
// General:
  Coord c0, cOther0;
  void _export init(); // load (again) from disk
  void _export Calc(); // set a11, a12, a21, a22, b1, b2.
// UserDefFormula:
  String sExprX, sExprY, sExprInvX, sExprInvY;
  Instructions *insX, *insY, *insInvX, *insInvY;
};


#endif // ILWCSFORMULA_H



