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
/* GeoRefEpipolar
   made for Stereoscopy
   Copyright Ilwis System Development ITC
   september 2001, by Jan Hendrikse
*/

#ifndef GRSCALEROTATE_H
#define GRSCALEROTATE_H

class DATEXPORT GeoRefScaleRotate: public GeoRefPtr
{
  friend class GeoRef;
  friend class GeoRefPtr;
//  static GeoRefScaleRotate* create(const FileName& fn, const String& sExpression);
public:
	_export GeoRefScaleRotate();
  _export GeoRefScaleRotate(const FileName& fn, const GeoRef& grf,  
												const double rPivotRow, const double rPivotCol, 
												const double rAngle, const double rScale,  
												const double rPivotRowTarget, const double rPivotColTarget);
protected:
  GeoRefScaleRotate(const FileName& fn);
  GeoRefScaleRotate(const FileName& fn, const CoordSystem& cs, RowCol rc)
    : GeoRefPtr(fn, cs, rc) {}
  static const char* sSyntax();
public:
  virtual void _export Store();
  virtual String sType() const;
  virtual String sTypeName() const;
	_export RowCol rGetPivotRowCol();
	_export double rGetRotAngle();
	_export double rGetScaleFactor();
	_export void SetPivotRowCol(const RowCol rcPiv);
	_export void SetRotAngle(const double rAng);
	_export void SetScaleFactor(const double rScale);
	void _export SetRowCol(RowCol rcSize);
	virtual void _export Coord2RowCol(const Coord& c, double& rRow, double& rCol) const;
  virtual void _export RowCol2Coord(double rRow, double rCol, Coord& c) const;
	virtual bool fGeoRefNone() const;
protected:
	
	void _export SetMatrixCoefficients(
										const double rPivotRow_in, const double rPivotCol_in,
										const double rAngle_in, const double rScale_in, 
										const bool fLeft_in);

	RowCol rcPivot;  // initial pivot position
	RowCol rcOffSet; // pivot position in target georef
	double rAngle, rScale;	
private:  
  GeoRef grSourceMap;
	//RowCol rcOffSet;
	virtual double rPixSize() const;
  virtual bool _export fEqual(const IlwisObjectPtr&) const;
  void SetConfTransfCoeffs(double rAngl, double rScal, RowCol rcPiv, RowCol rcOffS)
    { rAngle = rAngl; rScale = rScal; rcPivot = rcPiv;fChanged = true; }
  void GetConfTransfCoeffs(double& rAngl, double& rScal, RowCol& rcPiv, RowCol& rcOffS) const
    { rAngl = rAngle; rScal = rScale; rcPiv= rcPivot;}
	
//protected:
  virtual bool _export fDependent() const;
  virtual bool _export fLinear() const;
  virtual bool _export fNorthOriented() const;
};

#endif // GRSCALEROTATE_H
