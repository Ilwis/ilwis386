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
/*
// $Log: /ILWIS 3.0/GeoReference/Grctppla.h $
 * 
 * 9     6-12-04 16:29 Hendrikse
 * Added computed sigma to add info
 * 
 * 8     28-09-04 14:16 Hendrikse
 * Extended functionality to support sub-pixel precision in the tiepoints
 * and fiducials
 * 
 * 7     6/12/02 8:30a Martin
 * Merged from the ASTER branch
 * 
 * 8     13-03-02 13:08 Hendrikse
 * exclude the use of an extra Jacobian matrix rmJR2C for the inversion
 * (to prevent the assignement that causes a crash)
 * 
 * 6     24-09-99 11:32 Hendrikse
 * added #include "Engine\SpatialReference\SolveOblique.h" from *.c file
 * 
 * 5     24-09-99 11:14 Hendrikse
 * comment
 * 
 * 2     23-09-99 18:09 Hendrikse
 * Now GeoRefCTPplanar: public GeoRefCTP, private SolveOblique
 * (multiple inherit)
*/
// Revision 1.4  1998/09/16 17:24:43  Wim
// 22beta2
//
// Revision 1.3  1997/09/29 11:13:25  Wim
// Removed fnReferenceMap which is already available in its parent,
// so was creating problems here.
/*
// Revision 1.2  1997-09-24 19:47:33+02  Wim
// Added grConvertToSimple()
//

   GeoRefCTPplanar
   Copyright Ilwis System Development ITC
   march 1995, by Wim Koolhoven
	Last change:  WK   11 Jun 98    2:07 pm
*/

#ifndef ILWGRPLA_H
#define ILWGRPLA_H
#include "Engine\SpatialReference\GRCTP.H"
#include "Engine\SpatialReference\SolveOblique.h"
#include "Engine\Base\Algorithm\Fpolynom.h"

class DATEXPORT GeoRefCTPplanar: public GeoRefCTP, private SolveOblique
{
  friend class GeoRefPtr;
  friend class _export GeoRefEditor;
  GeoRefCTPplanar(const FileName&);
public:  
  _export GeoRefCTPplanar(const FileName&, const CoordSystem&, RowCol, bool fSubPixelPrecise = false);
  ~GeoRefCTPplanar();
  virtual String sType() const;
  virtual double rPixSize() const;
  virtual void Coord2RowCol(const Coord& c, double& rR, double& rC) const;
  virtual void RowCol2Coord(double rR, double rC, Coord& c) const;
  virtual bool fEqual(const IlwisObjectPtr&) const;
  virtual void Store();
  virtual void GetDataFiles(Array<FileName>& afnDat, Array<String>* asSection=0, Array<String>* asEntry=0) const;
  
  virtual int Compute();
 
  virtual int iMinNr() const;
  virtual void Rotate(bool fSwapRows, bool fSwapCols, bool fRotate);
  virtual bool fLinear() const;
  virtual bool fNorthOriented() const;
  GeoRef _export grConvertToSimple() const;
	void _export SetSigma(double s);
private:
  String sFormula() const;
  double rAvgX, rAvgY, rAvgRow, rAvgCol;
  double rCoeffX[10];
  double rCoeffY[10];
  double rCoeffRow[10];
  double rCoeffCol[10];
  Coord crdInverseOfAffine(const double &rCol, const double &rRow);
  Coord crdInverseOfSecondOrderBilinear(const double &rCol, const double &rRow);
  Coord crdInverseOfHigherOrder(const double &rCol, const double &rRow);
  Coord crdInverseOfProjective(const double &rCol, const double &rRow);
  RealMatrix rmJC2R;// rmJR2C no extra matrix needed (11/3/02)
  void MakeJacMatrix(const Coord &crdIn , RealMatrix &rmJ);
  ///void MakeJacMatrix3(const Coord &crdIn , RealMatrix &rmJ);	
	double m_rSigma;
};

#endif // ILWGRPLA_H




