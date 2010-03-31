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
/* GeoRefSmpl
   Copyright Ilwis System Development ITC
   march 1995, by Wim Koolhoven
	Last change:  WK   11 Jun 98    2:05 pm
*/


#ifndef ILWGRSMPL_H
#define ILWGRSMPL_H
#include "Engine\SpatialReference\Gr.h"

class GeoRefSmpl: public GeoRefPtr
{
  friend class GeoRef;
  friend class GeoRefPtr;
  static GeoRefSmpl* create(const FileName& fn, const String& sExpression);
public:  
  _export GeoRefSmpl(const FileName& fn, const CoordSystem& cs, RowCol rc,
             double a11, double a12, double a21, double a22,
             double b1, double b2);
  GeoRefSmpl(const FileName& fn, RowCol rc,
             double a11, double a12, double a21, double a22,
             double b1, double b2);
protected:
  GeoRefSmpl(const FileName& fn);
  GeoRefSmpl(const FileName& fn, const CoordSystem& cs, RowCol rc)
    : GeoRefPtr(fn, cs, rc) {}
  static const char* sSyntax();
public:
  virtual void _export Store();
  virtual String _export sName(bool fExt = false, const String& sDirRelative = "") const;
  virtual double _export rPixSize() const;
  virtual void _export Coord2RowCol(const Coord& c, double& rRow, double& rCol) const;
  virtual void _export RowCol2Coord(double rRow, double rCol, Coord& c) const;
  virtual bool _export fEqual(const IlwisObjectPtr&) const;
  void SetSmpl(double a_11, double a_12, double a_21, double a_22,
               double b_1, double b_2)
    { a11 = a_11; a12 = a_12; a21 = a_21; a22 = a_22;
      b1 = b_1; b2 = b_2;
      fChanged = true;
    }
  void GetSmpl(double& a_11, double& a_12, double& a_21, double& a_22,
               double& b_1, double& b_2) const
    { a_11 = a11; a_12 = a12; a_21 = a21; a_22 = a22;
      b_1 = b1; b_2 = b2;
    }
  void GetSmpl(float& a_11, float& a_12, float& a_21, float& a_22,
               float& b_1, float& b_2) const
    { a_11 = (float)a11; a_12 = (float)a12; a_21 = (float)a21; a_22 = (float)a22;
      b_1 = (float)b1; b_2 = (float)b2;
    }
//  virtual void Enlarge(double rEnlfac);
  virtual void _export Rotate(bool fSwapRows, bool fSwapCols, bool fRotate);
//protected:
  virtual bool _export fDependent() const;
  virtual bool _export fLinear() const;
  virtual bool _export fNorthOriented() const;
  double rDeterm() const;
  double a11, a12, a21, a22, b1, b2;
};

#endif




