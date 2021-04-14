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
/*$Log: /ILWIS 3.0/GeoReference/Grctppla.cpp $
 * 
 * 15    6-12-04 16:29 Hendrikse
 * debugged additionalInfo: replacing rAvgRow by rRowOffSet and rAvgCol by
 * rColOffSet
 * Added computed sigma to add info
 * 
 * 14    15-10-04 18:40 Hendrikse
 * Changed AdditionInfo text (in sFormula()) for the centroid of the
 * tiepoints in RowCol system from 0 to 3 decimals after the point in the
 * inverse formula's to be in agreement with forward formula's (rowcol to
 * XY) and to allow checking of transformation using subpixel-precision
 * positions of tiepoints 
 * 
 * 13    28-09-04 14:16 Hendrikse
 * Extended functionality to support sub-pixel precision in the tiepoints
 * and fiducials
 * 
 * 12    6/12/02 8:30a Martin
 * merged from the ASTER branch
 * 
 * 14    13-03-02 13:09 Hendrikse
 * exclude the use of an extra Jacobian matrix rmJR2C for the inversion
 * (to prevent the assignement that causes a crash).
 * rmJC2R seem to be enough to do the job for crdInverseHigherOrder
 * RowCol2Order
 * 
 * 13    6-03-02 14:28 Hendrikse
 * sFormula gives now more Addit.Info on angular distortion (Grid
 * intersection angle) and on the computed (estimated) pixelSize
 * 
 * 11    11/23/01 17:49 Hendrikse
 * made higher precision in Additional info text
 * 
 * 10    13-11-00 10:52 Hendrikse
 * made rAzimuth 1 digit more precise for use in epipolar rotations
 * 
 * 9     12-10-00 15:02 Hendrikse
 * improved lay-out of addit info text
 * 
 * 8     26-09-00 16:45 Hendrikse
 * made rAzimuth in AdditInfo string more accurate, because expressed in
 * degrees
 * 
 * 7     26-07-00 16:14 Hendrikse
 * In compute() under if (transf == CONFORM) {
 * changed condition into abs (sxx+syy) < EPS10)
 * to allow for Latlons in coordsys of georef
 * 
 * 6     5-10-99 9:19 Hendrikse
 * 
 * 5     9/29/99 10:21a Wind
 * added case insensitive string comparison
 * 
 * 4     24-09-99 11:28 Hendrikse
 * moved to *.h file: #include "Engine\SpatialReference\SolveOblique.h"
 * 
 * 3     23-09-99 18:09 Hendrikse
 * Now  GeoRefCTPplanar: public GeoRefCTP, private SolveOblique
 * 
 * 2     3/12/99 3:05p Martin
 * Added support for case insensitive
//Revision 1.7  1998/09/16 17:24:43  Wim
//22beta2
//
//Revision 1.6  1997/09/26 14:02:31  Dick
//in rev 1.5 in fEqual and grConvertToSimple the offset (b1 and b2 in the definition
//of gerefsimple) was not correct computed
//
//Revision 1.5  1997/09/26 10:35:52  Dick
//in fEqual and grConvertToSimple rAvgCol and rAvgRow (the shifts instead of
//rCoeffCol[0] and rCoeffRow[0]
//
//Revision 1.4  1997/09/24 17:47:46  Wim
//Added grConvertToSimple()
//
//Revision 1.3  1997-09-24 19:14:01+02  janh
//SecondOrder (bilinear) is now treated and implemented as special case of
//Higher order. Also the corresponding Jacobian is made
//The inversion of bilinear seems not quite safe and is commented out
//
//Revision 1.2  1997/09/24 12:06:02  janh
//Addae rGapStart = rPixSize() * 2  in crdInverseofHigherOrder
//otherwise while loop will not start for Pixels larger than 1m
//
/* GeoRefCTPplanar
   Copyright Ilwis System Development ITC
   march 1995, by Wim Koolhoven
	Last change:  WK   11 Jun 98    2:23 pm
*/

#include "Engine\SpatialReference\Gr.h"
#include "Engine\SpatialReference\Grctppla.h"
#include "Engine\SpatialReference\Grsmpl.h"
#include "Engine\Base\Algorithm\Fpolynom.h"
#include "Engine\Map\Point\PNT.H"
#include "Engine\Map\Raster\Map.h"
#include "Engine\Base\DataObjects\ERR.H"
#define EPS10 1.e-10

GeoRefCTPplanar::GeoRefCTPplanar(const FileName& fn)
: GeoRefCTP(fn)
{
  String s = "Affine";
  ReadElement("GeoRefCTP", "Transformation", s);
  if (fCIStrEqual(s,"Conform"))
    transf = CONFORM;
  else if (fCIStrEqual(s , "Affine"))  
    transf = AFFINE;
  else if (fCIStrEqual(s , "SecondOrder"))  
    transf = SECONDORDER;
  else if (fCIStrEqual(s , "FullSecondOrder"))  
    transf = FULLSECONDORDER;
  else if (fCIStrEqual(s , "ThirdOrder"))  
    transf = THIRDORDER;
  else if (fCIStrEqual(s , "Projective"))  
    transf = PROJECTIVE;
  else if (fCIStrEqual(s , "Interpolate"))
    transf = INTERPOLATE;
  else if (fCIStrEqual(s , "Grid"))
    transf = GRID;
  //rmJR2C = RealMatrix(2);
  rmJC2R = RealMatrix(2);
  Compute();    
}

GeoRefCTPplanar::GeoRefCTPplanar(const FileName& fn, const CoordSystem& cs, RowCol rc, bool fSubPixelPrecise)
: GeoRefCTP(fn,cs,rc,false,fSubPixelPrecise)
{
  transf = AFFINE;
  //rmJR2C = RealMatrix(2);
  rmJC2R = RealMatrix(2);
}

void GeoRefCTPplanar::Store()
{
  if (fValid()) {
    SetAdditionalInfo(sFormula());
    SetAdditionalInfoFlag(true);
  }
  else {
    SetAdditionalInfoFlag(false);
  }

  String s;
  GeoRefCTP::Store();
  WriteElement("GeoRef", "Type", "GeoRefCTP");
  switch (transf) {
    case CONFORM:         s = "Conform";         break;
    case AFFINE:          s = "Affine";          break;
    case SECONDORDER:     s = "SecondOrder";     break;
    case FULLSECONDORDER: s = "FullSecondOrder"; break;
    case THIRDORDER:      s = "ThirdOrder";      break;
    case PROJECTIVE:      s = "Projective";      break;
    case INTERPOLATE:     s = "Interpolate";     break;
    case GRID:            s = "Grid";            break;
  }
  WriteElement("GeoRefCTP", "Transformation", s);
}

GeoRefCTPplanar::~GeoRefCTPplanar()
{
}

String GeoRefCTPplanar::sType() const
{
  return "GeoReference TiePoints";
}

void GeoRefCTPplanar::SetSigma(double s)
{
	m_rSigma = s;
}

String GeoRefCTPplanar::sFormula() const
{
  String s = "Transformation: ";
  switch (transf) {
    case CONFORM:
      s &= "Conform";
      break;
    case AFFINE:
      s &= "Affine";
      break;
    case SECONDORDER:
      s &= "SecondOrder";
      break;
    case FULLSECONDORDER:
      s &= "FullSecondOrder";
      break;
    case THIRDORDER:
      s &= "ThirdOrder";
      break;
    case PROJECTIVE:
      s &= "Projective";
      break;
    case INTERPOLATE:
      s &= "Interpolate";
      break;
    case GRID:
      s &= "Grid";
      break;
  }
	s &= String("\r\nNr of Ground Control Points: %i", tblCTP->iRecs());
  s &= String("\r\nNr of Active Ground Control Points: %i",iActive); 
	s &= String("\r\nShift of Ground-Coords grdX,grdY to Centroid:");
  s &= String("\r\nX = grdX - %.3f m", rAvgX);
  s &= String("\r\nY = grdY - %.3f m", rAvgY);
	double rRowOffSet = rAvgRow + rCoeffRow[0];
	double rColOffSet = rAvgCol + rCoeffCol[0];
	s &= String("\r\nEquations (from Ground to Picture):");
  s &= "\r\npRow = ";
  if (PROJECTIVE == transf) {
    s &= String("%.3f + \r\n(%.6g * X + %.6g * Y + %.6g) \r\n/ (%.6g * X + %.6g * Y + 1)",
      rAvgRow,
      rCoeffCol[3], rCoeffCol[4], rCoeffCol[5], rCoeffCol[6], rCoeffCol[7]);
  } else if (INTERPOLATE == transf) {
    s &= String("interpolated from four tiepoints");
  } else if (GRID == transf) {
    s &= String("interpolated from grid tiepoints");
  } else {
    s &= String("%.3f + %.6g * X + %.6g * Y", rRowOffSet, rCoeffRow[1], rCoeffRow[2]);
    if (transf >= SECONDORDER)
      s &= String(" + %.6g * X * Y", rCoeffRow[3]);
    if (transf >= FULLSECONDORDER)
      s &= String(" + %.6g * X^2 + %.6g * Y^2", rCoeffRow[4], rCoeffRow[5]);
    if (transf == THIRDORDER)
      s &= String(" + %.6g * X^3 + %.6g * X^2 * Y + %.6g * X * Y^2 + %.6g * Y3",
        rCoeffRow[6], rCoeffRow[7], rCoeffRow[8], rCoeffRow[9]);
  }
  s &= "\r\npCol = ";
  if (PROJECTIVE == transf) {
    s &= String("%.3f + \r\n(%.6g * X + %.6g * Y + %.6g) \r\n/ (%.6g * X + %.6g * Y + 1)",
      rAvgCol,
      rCoeffCol[0], rCoeffCol[1], rCoeffCol[2], rCoeffCol[6], rCoeffCol[7]);
  } else if (INTERPOLATE == transf) {
    s &= String("interpolated from four tiepoints");
  } else if (GRID == transf) {
    s &= String("interpolated from grid tiepoints");
  } else {
    s &= String("%.3f + %.6g * X + %.6g * Y", rColOffSet, rCoeffCol[1], rCoeffCol[2]);
    if (transf >= SECONDORDER)
      s &= String(" + %.6g * X * Y", rCoeffCol[3]);
    if (transf >= FULLSECONDORDER)
      s &= String(" + %.6g * X^2 + %.6g * Y^2", rCoeffCol[4], rCoeffCol[5]);
    if (transf == THIRDORDER)
      s &= String(" + %.6g * X^3 + %.6g * X^2 * Y + %.6g * X * Y^2 + %.6g * Y3",
        rCoeffCol[6], rCoeffCol[7], rCoeffCol[8], rCoeffCol[9]);
  }
	if (transf <= PROJECTIVE) {
		double rAzimuth = atan2(-rCoeffCol[2],rCoeffCol[1]);
    s &= "\r\nAzimuth of colums in rastermap:";
    s &= String("\r\n %.5g degrees", rAzimuth * 180.0/M_PI);
		double rSkewOfXisoline = atan2(rCoeffCol[1],rCoeffRow[1]) * 180.0/M_PI - 90.;
		double rSkewOfYisoline = atan2(rCoeffCol[2],rCoeffRow[2]) * 180.0/M_PI - 90.;
		double rInterSecAngle = abs(rSkewOfXisoline - rSkewOfYisoline);
		if (rInterSecAngle > 180)
			rInterSecAngle -= 180;
		s &= "\r\nGrid intersection angle is";
		s &= String("\r\n(computed at point (%.5f, %.5f):",rAvgX,rAvgY);
    s &= String("\r\n %.5g degrees", rInterSecAngle);
		s &= "\r\nPixel size:";
    s &= String("\r\n %.5g meter (or degrees) per pixel", rPixSize());
	}
  if (transf <= AFFINE) {
    s &= String("\r\n\r\n Inverse Transformation:");
		s &= String("\r\nShift of Picture pRow,pCol to Centroid:");
    s &= String("\r\nRow = pRow - %.3f ", rAvgRow);
    s &= String("\r\nCol = pCol - %.3f ", rAvgCol);
    s &= "\r\ngrdX = ";
    s &= String("%.5f + %.5f * Row + %.5f * Col", rAvgX + rCoeffX[0], rCoeffX[2], rCoeffX[1]);
    s &= "\r\ngrdY = ";
    s &= String("%.5f + %.5f * Row + %.5f * Col", rAvgY + rCoeffY[0], rCoeffY[2], rCoeffY[1]);
		double rScaleX = sqrt(rCoeffX[1]*rCoeffX[1] + rCoeffX[2]*rCoeffX[2]);
		s &= String("\r\nScale in X direction : %.5f meter (or degrees) per pixel", rScaleX);
		double rScaleY = sqrt(rCoeffY[1]*rCoeffY[1] + rCoeffY[2]*rCoeffY[2]);
    s &= String("\r\nScale in Y direction : %.5f meter (or degrees) per pixel", rScaleY);
		s &= String("\r\n\r\n Simplified Coord to RowCol equations: ('GeoRefSimple')");
		s &= "\r\nRow = ";
		rRowOffSet -= (rCoeffRow[1]*rAvgX + rCoeffRow[2]*rAvgY);
		s &= String("%.3f + %.6g * X + %.6g * Y", rRowOffSet, rCoeffRow[1], rCoeffRow[2]);
		s &= "\r\nCol = ";
		rColOffSet -= (rCoeffCol[1]*rAvgX + rCoeffCol[2]*rAvgY);
		s &= String("%.3f + %.6g * X + %.6g * Y", rColOffSet, rCoeffCol[1], rCoeffCol[2]);
		s &= "\r\nSigma = ~";
		s &= String("%.2f pixels", m_rSigma);
		GeoRef grs = grConvertToSimple();
		double a11 = grs->pgsmpl()->a11;
		s &= "\r\nRow = ";
		s &= String("%.3f + %.6g * X + %.6g * Y", grs->pgsmpl()->b2, grs->pgsmpl()->a21, grs->pgsmpl()->a22);
		s &= "\r\nCol = ";
		s &= String("%.3f + %.6g * X + %.6g * Y", grs->pgsmpl()->b1, grs->pgsmpl()->a11, grs->pgsmpl()->a12);
		s &= "\r\nPixel size (GrSmpl):";
    s &= String("\r\n %.5g meter (or degrees) per pixel", grs->rPixSize());
		double m11 = rCoeffRow[1]*rCoeffX[2] + rCoeffRow[2]*rCoeffY[2];
		double m12 = rCoeffRow[1]*rCoeffX[1] + rCoeffRow[2]*rCoeffY[1];
		double m21 = rCoeffCol[1]*rCoeffX[2] + rCoeffCol[2]*rCoeffY[2];
		double m22 = rCoeffCol[1]*rCoeffX[1] + rCoeffCol[2]*rCoeffY[1];
		s &= "\r\nRC * XY matrix = ~";
		s &= String("\r\n[%.6g   %.6g]", m11, m12);
		s &= String("\r\n[%.6g   %.6g]", m21, m22);
  }
  if(fSubPixelPrecision)
		s &= String("\r\nTiepoints have RowCol positions with sub-pixel precision");
  return s;
}

double GeoRefCTPplanar::rPixSize() const
{
  if (!fValid())
    return rUNDEF;
  if (transf == PROJECTIVE) 
    return sqrt(abs(rCoeffX[0] * rCoeffX[4] - rCoeffX[1] * rCoeffX[3]));
  else if (transf == GRID)
	  return ((backwardGridMaxY - backwardGridMinY) / _rc.Row + (backwardGridMaxX - backwardGridMinX) / _rc.Col) / 2.0; // TODO: strictly spoken we should account for "center of corners"
  else
    return sqrt(abs(rCoeffX[1] * rCoeffY[2] - rCoeffX[2] * rCoeffY[1]));
//  else
//    return rUNDEF;
}

double interpolate2D(const double wi, const double wj, const double x00, const double x10, const double x01,const double x11) {
	return x00 + wi * (x10 - x00) + wj * (x01 - x00) + wi * wj * (x11 + x00 - x01 - x10);
}

void GeoRefCTPplanar::Coord2RowCol(const Coord& c, double& rRow, double& rCol) const
{
  if (!fValid() || c.fUndef()) {
    rRow = rUNDEF;
    rCol = rUNDEF;
    return;
  }
  if (transf == GRID) {
	  Coord2RowColGrid(c, rRow, rCol);
	  return;
  } else if (transf == INTERPOLATE) {
	  double ulCol = colCol->rValue(1);
	  double ulRow = colRow->rValue(1);
	  double urCol = colCol->rValue(2);
	  double urRow = colRow->rValue(2);
	  double brCol = colCol->rValue(3);
	  double brRow = colRow->rValue(3);
	  double blCol = colCol->rValue(4);
	  double blRow = colRow->rValue(4);
	  double ulX = colX->rValue(1);
	  double ulY = colY->rValue(1);
	  double urX = colX->rValue(2);
	  double urY = colY->rValue(2);
	  double brX = colX->rValue(3);
	  double brY = colY->rValue(3);
	  double blX = colX->rValue(4);
	  double blY = colY->rValue(4);
	  double wi = (c.x - ulX) / (urX - ulX);
	  double wj = (c.y - ulY) / (blY - ulY);
	  rRow = interpolate2D(wi, wj, ulRow, urRow, blRow, brRow);
	  rCol = interpolate2D(wi, wj, ulCol, urCol, blCol, brCol);
	  return;
  }
  Coord crd = c;
  rRow = rAvgRow;
  rCol = rAvgCol;
  crd.x -= rAvgX;
  crd.y -= rAvgY;
  double X2, Y2, XY;
  switch (transf) {
    case THIRDORDER:
      // fall through
    case FULLSECONDORDER:
      X2 = crd.x * crd.x;
      Y2 = crd.y * crd.y;
      // fall through
    case SECONDORDER:
      XY = crd.x * crd.y;
      break;
  }
  switch (transf) {
    case THIRDORDER:
      rCol += (rCoeffCol[6] * crd.x + rCoeffCol[7] * crd.y) * X2 +
              (rCoeffCol[8] * crd.x + rCoeffCol[9] * crd.y) * Y2;
      rRow += (rCoeffRow[6] * crd.x + rCoeffRow[7] * crd.y) * X2 +
              (rCoeffRow[8] * crd.x + rCoeffRow[9] * crd.y) * Y2;
      // fall through
    case FULLSECONDORDER:
      rCol += rCoeffCol[4] * X2 + rCoeffCol[5] * Y2;
      rRow += rCoeffRow[4] * X2 + rCoeffRow[5] * Y2;
      // fall through
    case SECONDORDER:
      rCol += rCoeffCol[3] * XY;
      rRow += rCoeffRow[3] * XY;
      // fall through
    case AFFINE:
      // fall through
    case CONFORM:
      rCol += rCoeffCol[0];
      rRow += rCoeffRow[0];
      rCol += rCoeffCol[1] * crd.x + rCoeffCol[2] * crd.y;
      rRow += rCoeffRow[1] * crd.x + rCoeffRow[2] * crd.y;
      break;
    case PROJECTIVE:
      rCol += (rCoeffCol[0] * crd.x + rCoeffCol[1] * crd.y + rCoeffCol[2]) /
              (rCoeffCol[6] * crd.x + rCoeffCol[7] * crd.y + 1);
      rRow += (rCoeffCol[3] * crd.x + rCoeffCol[4] * crd.y + rCoeffCol[5]) /
              (rCoeffCol[6] * crd.x + rCoeffCol[7] * crd.y + 1);
      break;
  }
}

void GeoRefCTPplanar::RowCol2Coord(double rRow, double rCol, Coord& crd) const
{
  if (!fValid()) {
//    throw ErrorObject("Invalid GeoRef", 12345);
    crd = crdUNDEF;
    return;
  }    
  if (rRow == rUNDEF || rCol == rUNDEF) {
    crd = crdUNDEF;
    return;
  }
  if (transf == GRID) {
	  RowCol2CoordGrid(rRow, rCol, crd);
	  return;
  } else if (transf == INTERPOLATE) {
	  double ulCol = colCol->rValue(1);
	  double ulRow = colRow->rValue(1);
	  double urCol = colCol->rValue(2);
	  double urRow = colRow->rValue(2);
	  double brCol = colCol->rValue(3);
	  double brRow = colRow->rValue(3);
	  double blCol = colCol->rValue(4);
	  double blRow = colRow->rValue(4);
	  double ulX = colX->rValue(1);
	  double ulY = colY->rValue(1);
	  double urX = colX->rValue(2);
	  double urY = colY->rValue(2);
	  double brX = colX->rValue(3);
	  double brY = colY->rValue(3);
	  double blX = colX->rValue(4);
	  double blY = colY->rValue(4);
	  double wi = (rCol - ulCol) / (urCol - ulCol);
	  double wj = (rRow - ulRow) / (blRow - ulRow);
	  crd.x = interpolate2D(wi, wj, ulX, urX, blX, brX);
	  crd.y = interpolate2D(wi, wj, ulY, urY, blY, brY);
	  return;
  }
  rRow -= rAvgRow;
  rCol -= rAvgCol;
  crd.x = rAvgX;
  crd.y = rAvgY;
  Coord crdInv;
  switch (transf) {
    case AFFINE:
      // fall through
    case CONFORM:
      crdInv = const_cast<GeoRefCTPplanar *>(this)->crdInverseOfAffine(rCol, rRow);
      crd.x += crdInv.x;
      crd.y += crdInv.y;
      break;
    case PROJECTIVE:
      crdInv = const_cast<GeoRefCTPplanar *>(this)->crdInverseOfProjective(rCol, rRow);
      crd.x += crdInv.x;
      crd.y += crdInv.y;
      break;
    case SECONDORDER:
        // rCoeffCol and rCoeffRow are filled with coefficients that assume
        // RowCols and Coords reduced to rAvgRow rAvgX etc (Centers of Gravity)
        // used in the following conversion function:
      ///crdInv = crdInverseOfSecondOrderBilinear(rCol, rRow);
      ///crd.x += crdInv.x ;  // shift back to true position
      ///crd.y += crdInv.y ;  // (crd was already == crdAvg)
      ///break;
       // fall through
    case FULLSECONDORDER:
      // fall through
    case THIRDORDER:
      crdInv = const_cast<GeoRefCTPplanar *>(this)->crdInverseOfHigherOrder(rCol, rRow);
      crd.x += crdInv.x ;  // shift back to true position
      crd.y += crdInv.y ;  // (crd was already == crdAvg)
      break;
  }
}

bool GeoRefCTPplanar::fEqual(const IlwisObjectPtr& obj) const
{
  const GeoRefPtr* grp = dynamic_cast<const GeoRefPtr*>(&obj);
  if (grp == 0)
    return false;
  if (grp == this)
    return true;
  if (rcSize() != grp->rcSize())
    return false;
  const GeoRefCTPplanar* grc = dynamic_cast<const GeoRefCTPplanar*>(grp);
  if (0 == grc) {
    if (transf <= AFFINE) {
      GeoRef gr(cs(), rcSize(), 
                rCoeffCol[1], rCoeffCol[2],
                rCoeffRow[1], rCoeffRow[2],
                -rCoeffCol[1] * rAvgX - rCoeffCol[2] * rAvgY + rAvgCol,
                -rCoeffRow[1] * rAvgX - rCoeffRow[2] * rAvgY + rAvgRow);  //rev 1.5 and 1.6
      return gr->fEqual(obj);
    }  
    return false;
  }  
  if (transf != grc->transf)
    return false;
  if (rAvgX != grc->rAvgX || rAvgY != grc->rAvgY || rAvgCol != grc->rAvgCol || rAvgRow != grc->rAvgRow)
	  return false;
  int i, iMax = 2;  
  switch (transf) {
    case THIRDORDER:
      iMax += 4;
      // fall through
    case FULLSECONDORDER:
      iMax += 2;
      // fall through
    case SECONDORDER:
      iMax += 1;
      // fall through
    case AFFINE:
      // fall through
    case CONFORM:
      for (i = 0; i <= iMax; ++i) {
        if (rCoeffX[i] != grc->rCoeffX[i])
          return false;
        if (rCoeffY[i] != grc->rCoeffY[i])
          return false;
      }   
      return true;
    case PROJECTIVE:
      for (i = 0; i <= 7; ++i)
        if (rCoeffX[i] != grc->rCoeffX[i])
          return false;
      return true;
    default:
      if (tblCTP->sName() != grc->tblCTP->sName())
        return false;
      return true;
  }
}

int GeoRefCTPplanar::Compute()
{
  _fValid = false;
  int i, iNr;
  int iRecs = tblCTP->iRecs();
  if (iRecs == 0) return -1;
  int iRes = 0;
  Coord* crdXY = new Coord[iRecs];
  Coord* crdRowCol = new Coord[iRecs];
  double rSumX = 0;
  double rSumY = 0;
  double rSumRow = 0;
  double rSumCol = 0;
  forwardGridMatrix.clear();
  backwardGridMatrix.clear();
  iNr = 0;
  for (i = 1; i <= iRecs; ++i) {
    if (fActive(i)) {
      bool fAct = true;
      double r = colX->rValue(i);
      if (r == rUNDEF || abs(r) > 1e20)
        fAct = false;
      if (fAct) {
        r = colY->rValue(i);
        if (r == rUNDEF || abs(r) > 1e20)
          fAct = false;
      }  
      if (fAct) {
        r = colRow->rValue(i);
        if (r == rUNDEF || abs(r) > 1e6)
          fAct = false;
      }  
      if (fAct) {
        r = colCol->rValue(i);
        if (r == rUNDEF || abs(r) > 1e6)
          fAct = false;
      }  
      if (!fAct)
        SetActive(i, false);
      else {   
        rSumX += crdXY[iNr].x = colX->rValue(i);
        rSumY += crdXY[iNr].y = colY->rValue(i);
        crdRowCol[iNr].x = colCol->rValue(i) - 0.5;
        crdRowCol[iNr].y = colRow->rValue(i) - 0.5;
        rSumCol += crdRowCol[iNr].x;
        rSumRow += crdRowCol[iNr].y;
        iNr += 1;
      }  
    }  
  }  
	iActive = iNr; // added to provide additional info 
  if (iNr == 0) 
    iRes = -1;
  else if (iNr < iMinNr()) 
    iRes = -2;
  if (0 == iRes) {
    rAvgX = rSumX / iNr;
    rAvgY = rSumY / iNr;
    rAvgRow = rSumRow / iNr;
    rAvgCol = rSumCol / iNr;
    for (i = 0; i < iNr; ++i) {
      crdXY[i].x -= rAvgX;
      crdXY[i].y -= rAvgY;
      crdRowCol[i].x -= rAvgCol;
      crdRowCol[i].y -= rAvgRow;
    }
    if (transf == CONFORM) {
      double sxr, sxc, syr, syc, srr, scc, sxx, syy;
      sxr = sxc = syr = syc = srr = scc = sxx = syy = 0;
      i = 0;
      for (i = 0; i < iNr; ++i) {
        double x = crdXY[i].x;
        double y = crdXY[i].y;
        double c = crdRowCol[i].x;
        double r = crdRowCol[i].y;
        sxx += x * x;
        syy += y * y;
        scc += c * c;
        srr += r * r;
        sxc += x * c;
        syc += y * c;
        sxr += x * r;
        syr += y * r;
      }
      if (abs(scc + srr) < 1 || abs(sxx + syy) < EPS10)
        iRes = -3;
      else {  
        double a = (sxc - syr) / (scc + srr);
        double b = (sxr + syc) / (scc + srr);
        rCoeffX[1] =  a;
        rCoeffX[2] =  b;
        rCoeffY[1] =  b;
        rCoeffY[2] = -a;
        rCoeffX[0] =  0;
        rCoeffY[0] =  0;
        double rDet = a * a + b * b;
        rCoeffCol[1] =  a / rDet;
        rCoeffCol[2] =  b / rDet;
        rCoeffRow[1] =  b / rDet;
        rCoeffRow[2] = -a / rDet;
        rCoeffCol[0] =  0;
        rCoeffRow[0] =  0;
      }  
    }
    else if (transf == PROJECTIVE) {
      iRes = iFindOblique(iNr, crdRowCol, crdXY, rCoeffX);
      if (iRes == 0)
        iRes = iFindOblique(iNr, crdXY, crdRowCol, rCoeffCol);
	} else if (transf == GRID) {
		// (re-)discover minRow, maxRow, minCol, maxCol, iRows, iCols
		long nrRows;
		long nrCols;
		long nrRowsBack;
		long nrColsBack;
		double minRow;
		double maxRow;
		double minCol;
		double maxCol;
		double minX;
		double maxX;
		double minY;
		double maxY;
		{ // intentional scope block
			std::set<double> rows;
			std::set<double> cols;
			for (int i = 1; i <= iRecs; ++i) {
				if (fActive(i)) {
					double row = colRow->rValue(i) - 0.5; // colRow->rValue() is in human-readable row/cols (center of topleft pixel is (1,1)); row, col are in raster-image row/cols (center of topleft pixel is (0.5,0.5), corner of topleft pixel is (0,0))
					double col = colCol->rValue(i) - 0.5;
					double X = colX->rValue(i);
					double Y = colY->rValue(i);
					rows.insert(row);
					cols.insert(col);
					if (i == 1) {
						minRow = row;
						maxRow = row;
						minCol = col;
						maxCol = col;
						minX = X;
						minY = Y;
						maxX = X;
						maxY = Y;
					} else {
						minRow = min(row, minRow);
						maxRow = max(row, maxRow);
						minCol = min(col, minCol);
						maxCol = max(col, maxCol);
						minX = min(X, minX);
						minY = min(Y, minY);
						maxX = max(X, maxX);
						maxY = max(Y, maxY);
					}
				}
			}
			nrRows = rows.size();
			nrCols = cols.size();
			nrRowsBack = nrRows;// * 4;
			nrColsBack = nrCols;// * 4;
			forwardGridMinCol = minCol;
			forwardGridMinRow = minRow;
			forwardGridStepCol = (maxCol - minCol) / (nrCols - 1);
			forwardGridStepRow = (maxRow - minRow) / (nrRows - 1);
			backwardGridMinX = minX;
			backwardGridMinY = minY;
			backwardGridMaxX = maxX;
			backwardGridMaxY = maxY;
			backwardGridStepX = (maxX - minX) / (nrColsBack - 1); // at first instance, use the same number of tiepoints for backward transformation
			backwardGridStepY = (maxY - minY) / (nrRowsBack - 1);
		}
		// create forward transformation matrix
		forwardGridMatrix.resize(nrRows, std::vector<std::pair<double,double>>(nrCols));
		// create pointmap from tiepoints for backward  transformation
		PointMap pmRow (FileName::fnUnique("grCtpGridRow.mpp"), CoordSystem(), CoordBounds(minX, minY, maxX, maxY), ValueRange(-9999999999,9999999999,0));
		PointMap pmCol (FileName::fnUnique("grCtpGridCol.mpp"), CoordSystem(), CoordBounds(minX, minY, maxX, maxY), ValueRange(-9999999999,9999999999,0));
		// fill forward transformation matrix and pointmap
		for (int i = 1; i <= iRecs; ++i) {
			if (fActive(i)) {
				double row = colRow->rValue(i) - 0.5; // colRow->rValue() is in human-readable row/cols (center of topleft pixel is (1,1)); row, col are in raster-image row/cols (center of topleft pixel is (0.5,0.5), corner of topleft pixel is (0,0))
				double col = colCol->rValue(i) - 0.5;
				double X = colX->rValue(i);
				double Y = colY->rValue(i);
				pmRow->iAddVal(Coord(X, Y), row);
				pmCol->iAddVal(Coord(X, Y), col);
				// change row, col to tiepoint-matrix index
				row = (row - forwardGridMinRow) / forwardGridStepRow;
				col = (col - forwardGridMinCol) / forwardGridStepCol;
				forwardGridMatrix[row][col] = std::make_pair(X,Y);
			}
		}
		pmRow->Store();
		pmCol->Store();
		// compute georef for backward transformation (at first instance the nrRows and nrCols are the same as the forward transformation, assuming this gives a similar pixelsize thus an acceptable transformation result)
		GeoRef grXYBounds(FileName::fnUnique("grCtpGrid.grf"), String("GeoRefCorners(%li,%li,1,%g,%g,%g,%g)", nrRowsBack, nrColsBack, minX - backwardGridStepX / 2.0, minY - backwardGridStepY / 2.0, maxX + backwardGridStepX / 2.0, maxY + backwardGridStepY / 2.0)); // cb() results by definition in a corners-of-corners GeoRef
		grXYBounds->Store();
		backwardGridMatrix.resize(nrRowsBack, std::vector<std::pair<double,double>>(nrColsBack)); // same nrRows and nrCols as GeoRef
		// interpolate and rasterize pointmap
		//Map mpRow(FileName::fnUnique("grCtpGridRow.mpr"), String("MapMovingAverage(%S,%S,InvDist(1.0,%g),plane)", pmRow->fnObj.sFileExt(), grXYBounds->fnObj.sFileExt(), max(maxX - minX, maxY - minY) / 40.0));
		//Map mpCol(FileName::fnUnique("grCtpGridCol.mpr"), String("MapMovingAverage(%S,%S,InvDist(1.0,%g),plane)", pmCol->fnObj.sFileExt(), grXYBounds->fnObj.sFileExt(), max(maxX - minX, maxY - minY) / 40.0));
		Map mpRow(FileName::fnUnique("grCtpGridRow.mpr"), String("MapMovingAverage(%S,%S,InvDist(0.5,%g),plane)", pmRow->fnObj.sFileExt(), grXYBounds->fnObj.sFileExt(), max(maxX - minX, maxY - minY) / 40.0));
		Map mpCol(FileName::fnUnique("grCtpGridCol.mpr"), String("MapMovingAverage(%S,%S,InvDist(0.5,%g),plane)", pmCol->fnObj.sFileExt(), grXYBounds->fnObj.sFileExt(), max(maxX - minX, maxY - minY) / 40.0));
		//Map mpRow(FileName::fnUnique("grCtpGridRow.mpr"), String("MapMovingAverage(%S,%S,InvDist(0.5,%g),plane)", pmRow->fnObj.sFileExt(), grXYBounds->fnObj.sFileExt(), 3.0 * max((maxX - minX) / nrCols, (maxY - minY) / nrRows)));
		//Map mpCol(FileName::fnUnique("grCtpGridCol.mpr"), String("MapMovingAverage(%S,%S,InvDist(0.5,%g),plane)", pmCol->fnObj.sFileExt(), grXYBounds->fnObj.sFileExt(), 3.0 * max((maxX - minX) / nrCols, (maxY - minY) / nrRows)));
		//Map mpRow(FileName::fnUnique("grCtpGridRow.mpr"), String("MapNearestPoint(%S,%S,plane)", pmRow->fnObj.sFileExt(), grXYBounds->fnObj.sFileExt()));
		//Map mpCol(FileName::fnUnique("grCtpGridCol.mpr"), String("MapNearestPoint(%S,%S,plane)", pmCol->fnObj.sFileExt(), grXYBounds->fnObj.sFileExt()));
		//Map mpRow(FileName::fnUnique("grCtpGridRow.mpr"), String("MapTrendSurface(%S,%S,6)", pmRow->fnObj.sFileExt(), grXYBounds->fnObj.sFileExt()));
		//Map mpCol(FileName::fnUnique("grCtpGridCol.mpr"), String("MapTrendSurface(%S,%S,6)", pmCol->fnObj.sFileExt(), grXYBounds->fnObj.sFileExt()));
		mpRow->Calc();
		mpCol->Calc();
		// use the raster to fill backward transformation matrix
		for (long i = 0; i < nrRowsBack; ++i) {
			RealBuf rows (nrColsBack);
			RealBuf cols (nrColsBack);
			mpRow->GetLineVal(nrRowsBack - i - 1, rows); // minY must be on-top, at i==0; maxY must be on bottom, at i==nrRowsBack-1
			mpCol->GetLineVal(nrRowsBack - i - 1, cols);
			for (long j = 0; j < nrColsBack; ++j) {
				backwardGridMatrix[i][j] = std::make_pair(rows[j],cols[j]);
			}
		}
		// delete all intermediate files
		mpRow->fErase = true;
		mpCol->fErase = true;
		grXYBounds->fErase = true;
		pmRow->fErase = true;
		pmCol->fErase = true;
	} else {
      int iTerms = iMinNr();
      iRes = iFindPolynom(iTerms, iNr, crdXY, crdRowCol, rCoeffCol, rCoeffRow);
      if (iRes == 0)
        iRes = iFindPolynom(iTerms, iNr, crdRowCol, crdXY, rCoeffX, rCoeffY);
    }
  }  
  delete [] crdXY;
  delete [] crdRowCol;
  _fValid = iRes == 0;
  return iRes;
}

Coord GeoRefCTPplanar::crdInverseOfAffine(const double &rCol, const double &rRow)
{
  // Solving the following Coord2RowCol equations for X and Y:
  // Col = aX + by + c    (I)
  // Row = fX + gY + h   (II)
  // becomes:
  //  a X + b Y = Col - c
  //  f X + g Y = Row - h
  // Using Cramers determinants rule:
  // det = ag - bf ;  detX = (Col -c)g - (Row -h)b
  //    detY = (Row -h)a - (Col -c)f
  // yields X = detX/det and Y = detY / det  provided  det <> 0
  double a = rCoeffCol[1];
  double b = rCoeffCol[2];
  double c = rCoeffCol[0];
  double f = rCoeffRow[1];
  double g = rCoeffRow[2];
  double h = rCoeffRow[0];
  double rDet = a * g - b * f;
  Coord crd;
  if (abs(rDet) < EPS10)
    return crdUNDEF;
  else  {
    crd.x = ((rCol - c) * g - (rRow - h) * b ) / rDet;
    crd.y = ((rRow - h) * a - (rCol - c) * f ) / rDet;
    return crd;
  }
}
Coord GeoRefCTPplanar::crdInverseOfSecondOrderBilinear(const double &rCol, const double &rRow)
{
  // Solving the following Coord2RowCol equations for X and Y:
  // Col = a + bX + cY + dXY    (I )
  // Row = e + fX + gY + hXY    (II)
  // by substitution of Y = (Col - a - bX) / (c + dX) into  eq II
  // we get the quadratic equation in X :
  // (df -bh)X*X + (de + cf -bg -ah +hCol -dRow)X + ce -ag + gCol - cRow = 0
  // compressed into  pX*X + qX + r = 0
  double a = rCoeffCol[0];
  double b = rCoeffCol[1];
  double c = rCoeffCol[2];
  double d = rCoeffCol[3];
  double e = rCoeffRow[0];
  double f = rCoeffRow[1];
  double g = rCoeffRow[2];
  double h = rCoeffRow[3];
  double p = d * f  - b * h;
  double q = d * e + c * f  - b * g  - a * h  + h * rCol  - d * rRow;
  double r = c * e - a * g  + g * rCol  - c * rRow;

  Coord crd;
  if (abs(p) < EPS10)  {// equation not quadratic
    if (abs(q) < EPS10)  // no solution because crd.x undefined
      return crdUNDEF;
    else {
      crd.x = r / q;
      if (abs(c + d * crd.x) > EPS10)
        crd.y = (rCol - a - b * crd.x) / (c + d * crd.x);
      else
        return crdUNDEF;
      return crd;
    }
  }  // end if equation not quadratic
  else  {      // real bilinear case (quadratic)
    double discrim = q * q - 4 * p * r;
    if (discrim < 0.0) return crdUNDEF;  // no real solution
    if (abs(discrim) < EPS10) {  // single solution for crd.x (2 coinciding solutions)
      crd.x = ( - 0.5 * q / p);  //
      if (abs(c + d * crd.x) > EPS10)
        crd.y = (rCol - a - b * crd.x) / (c + d * crd.x);
      else 
        return crdUNDEF;
      return crd;
    }
    else  {                      // double solution for crd.x
      Coord crd1, crd2;
      bool fY1Exists = false; 
      bool fY2Exists = false;
      crd1.x = -0.5 * (q + sqrt(discrim)) / p;
      crd2.x = -0.5 * (q - sqrt(discrim)) / p;
      if (abs(c + d * crd1.x) > EPS10) {
        crd1.y = (rCol - a - b * crd1.x) / (c + d * crd1.x);
        fY1Exists = true;
      }
      if (abs(c + d * crd2.x) > EPS10) {
        crd2.y = (rCol - a - b * crd2.x) / (c + d * crd2.x);
        fY2Exists = true;
      }
      if (!fY1Exists)
        if (!fY2Exists)
          return crdUNDEF; // no valid crd.y values
        else
          crd = crd2;      // single crd.y value
      else if (!fY2Exists)
          crd = crd1;      // single crd.y value
      else {                // 2 (distinct) solutions for crd
        double rLength1Sq = crd1.x * crd1.x  +  crd1.y + crd1.y;
        double rLength2Sq = crd2.x * crd2.x  +  crd2.y + crd2.y;
        if (rLength1Sq < rLength2Sq) // select nearest point
          crd = crd1;
        else
          crd = crd2;            
      }    // end comparing two distinct solutions
      return crd; // return the best (if it is good enough) out of 2 solutions
    }
  }
}

Coord GeoRefCTPplanar::crdInverseOfProjective(const double &rCol, const double &rRow)
{
  // Solving the following Coord2RowCol equations for X and Y:
  // Col = aX + by + c / ( dX + eY + 1)   (I)
  // Row = fX + gY + h / ( dX + eY + 1)   (II)
  // becomes:
  // ( a - dCol)X + (b - eCol)Y = Col - c
  // ( f - dRow)X + (g - eRow)Y = Row - h
  // compressed into  pX + qX = r  and sX + tY = u
  // Using Cramers determinants rule:
  // det = pt - qs ;  detX = rt - qu  and detY = pu - rs
  // yields X = detX/det and Y = detY / det  provided  det <> 0
  double a = rCoeffCol[0];
  double b = rCoeffCol[1];
  double c = rCoeffCol[2];
  double d = rCoeffCol[6];
  double e = rCoeffCol[7];
  double f = rCoeffCol[3];
  double g = rCoeffCol[4];
  double h = rCoeffCol[5];
  double p = a - d * rCol;
  double q = b - e * rCol;
  double r = rCol  - c;
  double s = f - d * rRow;
  double t = g - e * rRow;
  double u = rRow  - h;
  double rDet = p * t - q * s;
  Coord crd;
  if (abs(rDet) < EPS10)
    return crdUNDEF;
  else  {
    crd.x = ( r * t - q * u ) / rDet;
    crd.y = ( p * u - r * s ) / rDet;
    return crd;
  }
}

void GeoRefCTPplanar::MakeJacMatrix(const Coord &crdIn , RealMatrix & rmJ)
{                     // Jacobian for 2nd/3rd order inversion
	 //double	c0 = coeffCol[0];
  double	c1 = rCoeffCol[1];
  double	c2 = rCoeffCol[2];
  double	c3 = rCoeffCol[3];
  double	c4 = rCoeffCol[4];
  double	c5 = rCoeffCol[5];
  double	c6 = rCoeffCol[6];
  double	c7 = rCoeffCol[7];
  double	c8 = rCoeffCol[8];
  double	c9 = rCoeffCol[9];
  //double	r0 = coeffRow[0];
  double	r1 = rCoeffRow[1];
  double	r2 = rCoeffRow[2];
  double	r3 = rCoeffRow[3];
  double	r4 = rCoeffRow[4];
  double	r5 = rCoeffRow[5];
  double	r6 = rCoeffRow[6];
  double	r7 = rCoeffRow[7];
  double	r8 = rCoeffRow[8];
  double	r9 = rCoeffRow[9];
  double X = crdIn.x;
  double Y = crdIn.y;
  // if transf == SECONDORDER or HIGHER
  rmJ(0,0) = c1 + c3 * Y;
	rmJ(0,1) = c2 + c3 * X;
	rmJ(1,0) = r1 + r3 * Y;
	rmJ(1,1) = r2 + r3 * X;
  if (transf == FULLSECONDORDER) {
    rmJ(0,0) +=  2 * c4 * X;
	  rmJ(0,1) +=  2 * c5 * Y;
	  rmJ(1,0) +=  2 * r4 * X;
	  rmJ(1,1) +=  2 * r5 * Y; 
	}
	if (transf == THIRDORDER) {
    double XX = X*X;
    double YX = X*Y;
	  double YY = Y*Y;
	  rmJ(0,0) +=  3 * c6 * XX  + 2 * c7 * YX  + c8 * YY;
	  rmJ(0,1) +=  c7 * XX  + 2 * c8 * YX  + 3 * c9 * YY;
	  rmJ(1,0) +=  3 * r6 * XX  + 2 * r7 * YX  + r8 * YY;
	  rmJ(1,1) +=  r7 * XX  + 2 * r8 * YX  + 3 * r9 * YY;
  }
}

Coord GeoRefCTPplanar::crdInverseOfHigherOrder(const double &rCol, const double &rRow)
{
  // Solving the following Coord2RowCol 3rd oreder equations for X and Y:
  // Col = c0 + c1X + c2Y + c3XY + c4XX + c5YY + c6XXX + c7XXY + c8XYY + c9YYY  (I )
  // Row = r0 + r1X + r2Y + r3XY + r4XX + r5YY + r6XXX + r7XXY + r8XYY + r9YYY  (II)
  // 2nd order if c6 == c7 == c8 == c9 == r6 == r7 == r8 == r9 == 0
  Coord crdNext = Coord(0,0);     // initial crd guess
  double rColNext = rCoeffCol[0]; // initial Col
  double rRowNext = rCoeffRow[0]; // initial Row
  double rDeltaCol = rCol - rColNext;
  double rDeltaRow = rRow - rRowNext;
  MakeJacMatrix(crdNext, rmJC2R);// linear appr matrix of Coord2RowCol in (0,0)
  double detJ = rmJC2R.rDet();
	if (abs(detJ) < EPS10)
		return crdUNDEF;
	else  {
    //rmJR2C = rmJC2R;no extra matrix needed (11/3/02)
		rmJC2R.Invert();
  }
  double rDeltaX = rmJC2R(0,0) * rDeltaCol + rmJC2R(0,1) * rDeltaRow;
  double rDeltaY = rmJC2R(1,0) * rDeltaCol + rmJC2R(1,1) * rDeltaRow;
  crdNext.x += rDeltaX;
  crdNext.y += rDeltaY;
  // crdNext is now 1st appr of RowCol2Coord(rRow, rCol,crd)
  double rGapStart = rPixSize() * 2;
  Coord crdTrue; 
  Coord crdGap = Coord(rGapStart,rGapStart);
  Coord crd0 = Coord(0,0);
	int iCount = 0;
  while ( !crdGap.fNear(crd0, rPixSize()) && iCount < 10) {
  	MakeJacMatrix(crdNext, rmJC2R);
    detJ = rmJC2R.rDet();
    if (abs(detJ) < EPS10)
   	  return crdUNDEF;
    else  {
      //rmJR2C = rmJC2R; no extra matrix needed (11/3/02)
			rmJC2R.Invert();
    }
	  rDeltaCol = rCol - rColNext;
		rDeltaRow = rRow - rRowNext;   // remaining error
		//improvement of crd found (linear approx) using inverse Jacobian matrix
		rDeltaX = rmJC2R(0,0) * rDeltaCol + rmJC2R(0,1) * rDeltaRow;
		rDeltaY = rmJC2R(1,0) * rDeltaCol + rmJC2R(1,1) * rDeltaRow;
		crdGap = Coord(rDeltaX, rDeltaY);
		crdNext.x += rDeltaX; //correction of crd with improvement
		crdNext.y += rDeltaY; //
		crdTrue.x = crdNext.x + rAvgX;//shift to true position for Coord2RowCol
		crdTrue.y = crdNext.y + rAvgY;
		Coord2RowCol(crdTrue, rRowNext, rColNext);
    rRowNext -= rAvgRow;  // shift back to center of gravity
	  rColNext -= rAvgCol;
		iCount++;
	}
	if (iCount < 10)
		return crdNext;
	else
		return crdUNDEF;
}

std::pair<double,double> interpolate(const std::vector<std::vector<std::pair<double,double>>> & matrix, double wi, double wj, long i0, long j0) {
	const long i1 = i0 + 1;
	const long j1 = j0 + 1;
	double x = interpolate2D(wi, wj, matrix[j0][i0].first, matrix[j0][i1].first, matrix[j1][i0].first, matrix[j1][i1].first);
	double y = interpolate2D(wi, wj, matrix[j0][i0].second, matrix[j0][i1].second, matrix[j1][i0].second, matrix[j1][i1].second);
	return std::make_pair(x,y);
}

long crop(long val, long min, long max) {
	return (val < min) ? min : ((val > max) ? max : val);
}

long floorLong(const double x) {
	return (long) floor(x);
}

long floorAndCrop(const double x, const long min, const long max) {
	const long rx = floorLong(x);
	return crop(rx, min, max);
}

std::pair<double,double> getPixelValue(const std::vector<std::vector<std::pair<double,double>>> & matrix, const double offsetX, const double offsetY, const double stepX, const double stepY, double x, double y)
{
	const long matrixWidth = matrix[0].size();
	const long matrixHeight = matrix.size();

	double fi = (x - offsetX) / stepX;
	double fj = (y - offsetY) / stepY;
	const long i = floorAndCrop(fi, 0, matrixWidth - 2);
	const long j = floorAndCrop(fj, 0, matrixHeight - 2);
	return interpolate(matrix, fi - i, fj - j, i, j);
}

void GeoRefCTPplanar::Coord2RowColGrid(const Coord& c, double& rRow, double& rCol) const
{
	std::pair<double,double> val = getPixelValue(backwardGridMatrix, backwardGridMinX, backwardGridMinY, backwardGridStepX, backwardGridStepY, c.x, c.y);
	std::pair<double,double> val2 = getPixelValue(forwardGridMatrix, forwardGridMinCol, forwardGridMinRow, forwardGridStepCol, forwardGridStepRow, val.second, val.first);
	rRow = val.first;
	rCol = val.second;
}

void GeoRefCTPplanar::RowCol2CoordGrid(const double rRow, const double rCol, Coord& crd) const
{
	std::pair<double,double> val = getPixelValue(forwardGridMatrix, forwardGridMinCol, forwardGridMinRow, forwardGridStepCol, forwardGridStepRow, rCol, rRow);
	crd.x = val.first;
	crd.y = val.second;
}

int GeoRefCTPplanar::iMinNr() const	
{
  switch (transf) {
    case CONFORM:
      return 2; 
    case AFFINE:
      return 3;
    case SECONDORDER:
      return 4; 
    case FULLSECONDORDER:
      return 6; 
    case THIRDORDER:
      return 10;
    case PROJECTIVE:
      return 4;
    case INTERPOLATE:
      return 4;
	case GRID:
      return 4;
    default:
      return 0;  
  };
}

void GeoRefCTPplanar::Rotate(bool fSwapRows, bool fSwapCols, bool fRotate)
{
  GeoRefPtr::Rotate(fSwapRows, fSwapCols, fRotate);
}

void GeoRefCTPplanar::GetDataFiles(Array<FileName>& afnDat, Array<String>* asSection, Array<String>* asEntry) const
{
  IlwisObjectPtr::GetDataFiles(afnDat, asSection, asEntry);
  FileName fnDat(fnObj, ".gr#", true);
  ObjectInfo::Add(afnDat, fnDat, fnObj.sPath());
  if (asSection != 0) {
    (*asSection) &= "TableStore";
    (*asEntry) &= "Data";
  }
}

bool GeoRefCTPplanar::fLinear() const   // TELLS WHETHER GRID IS RECTILINEAR
{
  return (transf == CONFORM) || (transf == AFFINE) || (transf == PROJECTIVE) || (transf == INTERPOLATE) || (transf == GRID && iActive <= 4);
}

GeoRef GeoRefCTPplanar::grConvertToSimple() const
{
  GeoRef gr;
  if (transf <= AFFINE) {
    gr = GeoRef(cs(), rcSize(),
                rCoeffCol[1], rCoeffCol[2],
                rCoeffRow[1], rCoeffRow[2],
                -rCoeffCol[1] * rAvgX - rCoeffCol[2] * rAvgY + rAvgCol,
                -rCoeffRow[1] * rAvgX - rCoeffRow[2] * rAvgY + rAvgRow);   //rev 1.5 and 1.6
  }
  return gr;
}

bool GeoRefCTPplanar::fNorthOriented() const
{
  if ((transf == CONFORM) || (transf == AFFINE))
    return (0 == rCoeffCol[2]) && (0 == rCoeffRow[1]);
  else
    return false;
}






