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
// $Log: /ILWIS 3.0/Table/Spatcorr.cpp $
 * 
 * 15    23-06-03 17:33 Koolhoven
 * in create() calculate maxdiff with chosen distance method
 * removed unneeded members fCsyLatLonIn and fCsyProjectionIn 
 * removed conditional multiplication with 111111.1
 * removed commented away code
 * 
 * 14    28-05-03 19:52 Hendrikse
 * the parts commented in or out recently concerning lags for spherical
 * should be checked still
 * 
 * 13    17-04-03 11:15 Hendrikse
 * re-activated assignment of rDifX and rDifY, for the use in
 * Bidirectional case
 * 
 * 12    10-03-03 12:25 Hendrikse
 * moved the case cs()->pcsLatLon further in the create function to treat
 * it in combination with Spherical distance case to set the map limits
 * correctly, in order to compute the nr of distance classes better
 * 
 * 11    4-03-03 10:31 Willem
 * - Changed: removed mismatch netween syntax in sSyntax() function as
 * comapred with code
 * 
 * 10    15-02-03 16:39 Hendrikse
 * debugged spaircolumn and condition of semivar display
 * replaced Planar, Spherical by Plane, Sphere
 * 
 * 9     7-10-02 11:42 Hendrikse
 * implemented use of m_distMeth = Distance::distSPHERE; etc from new
 * Class Distance in CoordSystems project
 * 
 * 8     5-07-02 19:34 Koolhoven
 * Prevent warnings with Visual Studio .Net
 * 
 * 7     14-11-00 18:12 Hendrikse
 * restored texts in sAddInfo because the CR Newline in de
 * IlwisString\table.s file doesn't function well, (shows pipe symbols in
 * stead after once opening the AdditInfo tab from the properties form
 * 
 * 6     16-06-00 19:32 Hendrikse
 * debugged and simplified description strings  sDescAvLag etc
 * made them also language independant
 * 
 * 5     10-01-00 10:29 Hendrikse
 * added String TableSpatCorr::sAddInfo() const in order to show to user:
 * rDataMean and rDataVariance
 * 
 * 4     9-09-99 11:29a Martin
 * added 2.22 stuff
 * 
 * 3     9/08/99 11:55a Wind
 * comments
 * 
 * 2     9/08/99 10:27a Wind
 * adpated to use of quoted file names in sExpression()
*/
// Revision 1.7  1998/09/16 17:25:32  Wim
// 22beta2
//
// Revision 1.6  1998/02/09 14:26:00  Dick
// Tranquilizer gave wrong percentage
//
// Revision 1.5  1997/10/07 13:43:33  Wim
// Correlation and Variance Columns now have reasonable ValueRange
//
// Revision 1.4  1997-08-13 18:06:11+02  Dick
// added tranquilizer message 'Spatatial correlation'
//
// Revision 1.3  1997/08/13 15:15:24  martin
// *** empty log message ***
//
// Revision 1.2  1997/08/13 11:50:14  martin
// It now checks before any calculation if there are sufficient points
//
/* TableSpatCorr
   Copyright Ilwis System Development ITC
   april 1995, by Jelle Wind
	Last change:  JH   24 Aug 98    4:54 pm
*/

#include "Applications\Table\Spatcorr.h"
#include "Engine\Table\tblstore.h"
#include "Engine\Table\COLSTORE.H"
#include <float.h>
#include "Headers\Htp\Ilwisapp.htp"
#include "Headers\Err\Ilwisapp.err"
#include "Headers\Hs\tbl.hs"
#include "Headers\Hs\Table.hs"
#include "Engine\SpatialReference\Cslatlon.h"
#include "Engine\SpatialReference\Coordsys.h"

#define MAXLIMIT 74L
#define sSPTLog        "Log"
#define sSPTOmniDirec  "OmniDirec"
#define sSPTBiDirec    "BiDirec"
#define sSPTBiDirecBW  "BiDirecBW"

static void DirecFrom_360To360(const FileName& fn) {
  throw ErrorObject(WhatError(String(STBLErrDirecFrom_360To360), errTableSpatCorr +6), fn);
}

static void TolerFrom0To45(const FileName& fn) {
  throw ErrorObject(WhatError(String(STBLErrTolerFrom0To45), errTableSpatCorr +6), fn);
}

static void BandWidthPositive(const FileName& fn) {
  throw ErrorObject(WhatError(String(STBLErrBandWidthPositive), errTableSpatCorr +5), fn);
}
 
const char* TableSpatCorr::sSyntax()
{
  return "TableSpatCorr(pntmap)\n"
				"TableSpatCorr(pntmap,LagSpacing[,plane|sphere])\n"
         "TableSpatCorr(pntmap,LagSpacing[,Direction[,Tolerance[,BandWidth]]])";
}

IlwisObjectPtr * createTableSpatCorr(const FileName& fn, IlwisObjectPtr& ptr, const String& sExpr, vector<void *> parms ) {
	if ( sExpr != "")
		return (IlwisObjectPtr *)TableSpatCorr::create(fn, (TablePtr &)ptr, sExpr);
	else
		return (IlwisObjectPtr *)new TableSpatCorr(fn, (TablePtr &)ptr);
}

TableSpatCorr* TableSpatCorr::create(const FileName& fn, TablePtr& p,
                const String& sExpr)
{
  Array<String> as;
  bool ferror = true;
  SPT esp;
//  String sSPType;
  double rLagSpacing;
  long iNrDistClasses;
  double rDirection;
  double rTolerance;
  double rBandWidth;

  rLagSpacing = rDirection = 1;
  rTolerance = 45.0;
  rBandWidth = 1e10;
  iNrDistClasses = 0L;
  
  int iParms=IlwisObjectPtr::iParseParm(sExpr, as);
  PointMap pmp(as[0], fn.sPath());
  /////
  Coord crdMin, crdMax; 
  pmp->Bounds(crdMin, crdMax);
	
	bool fSphericDist = false; //default
   switch (iParms)
  {
  case 1:
    esp = eLogT;
    iNrDistClasses = MAXLIMIT;
    break;
  case 2:
    esp = eOmniDirT;
    break;
  case 3:
			esp = eOmniDirT;
		if (fCIStrEqual("plane", as[2]))
			fSphericDist = false;
		else if (fCIStrEqual("sphere", as[2]))
			fSphericDist = true;
		else {
    esp = eBiDirecT;
    rDirection = as[2].rVal();
		}
		if (rDirection == rUNDEF)
			throw ErrorExpression(sExpr, sSyntax()); 
    break;
  case 4:
    esp = eBiDirecT;
    rDirection = as[2].rVal();
    rTolerance = as[3].rVal();
    break;
  case 5:
    esp = eBiDirecBWT;
    rDirection = as[2].rVal();
    rTolerance = as[3].rVal();
    rBandWidth = as[4].rVal();
    break;
  default:
    throw ErrorExpression(sExpr, sSyntax()); 
  }
   if (rDirection < -360.0 || rDirection > 360.0) 
    DirecFrom_360To360(fn);
  if (rTolerance <= 0.0 || rTolerance > 45.0) 
    TolerFrom0To45(fn);
  if (rBandWidth <= 0 )
    BandWidthPositive(fn);

  Distance dis = Distance(pmp->cs(), fSphericDist ? Distance::distSPHERE : Distance::distPLANE);
  double rMaxDiff = dis.rDistance(crdMin, crdMax);

  if (iParms > 1) {
    rLagSpacing = as[1].rVal();
    if (rLagSpacing <= 0) 
      throw ErrorObject(WhatError(String(STBLErrLagSpacingNotPos), errTableSpatCorr +3), fn);
    iNrDistClasses = 1 + (long)(rMaxDiff / rLagSpacing);
    iNrDistClasses = min (iNrDistClasses,100);
  }  
  return new TableSpatCorr(fn, p, pmp, esp, rLagSpacing, iNrDistClasses,
                rDirection, rTolerance, rBandWidth, fSphericDist);
}

TableSpatCorr::TableSpatCorr(const FileName& fn, TablePtr& p)
: TableVirtual(fn, p)
{
  fNeedFreeze = true;
  ReadElement("TableSpatCorr", "PointMap", pmp);
  String sSPType;
	Table tbl= pmp->tblAtt(); 
//	tbl->LoadData();
  ReadElement("TableSpatCorr", "SpatCorrType", sSPType);
  eSPType = eOmniDirT;
  if (sSPTLog == sSPType)
    eSPType = eLogT;
  else if (sSPTOmniDirec == sSPType)
    eSPType = eOmniDirT;
  else if (sSPTBiDirec == sSPType)
    eSPType = eBiDirecT;
  else if (sSPTBiDirecBW == sSPType)
    eSPType = eBiDirecBWT;

	m_distMeth = Distance::distPLANE;	//default
  String sDistanceMethod;
  ReadElement("TableSpatCorr", "DistanceMethod",sDistanceMethod);  
  if (fCIStrEqual("sphere", sDistanceMethod)) 
    m_distMeth = Distance::distSPHERE;
	else
		m_distMeth = Distance::distPLANE;
	
  ReadElement("TableSpatCorr", "LagLength", c_rLagLength);
  ReadElement("TableSpatCorr", "NrDistClasses", c_iNrDistClasses);
  ReadElement("TableSpatCorr", "Direction", c_rDirection);
  ReadElement("TableSpatCorr", "Tolerance", c_rTolerance);
  ReadElement("TableSpatCorr", "BandWidth", c_rBandWidth);

  if (eSPType == eBiDirecT)
    c_rBandWidth = rUNDEF;
  objdep.Add(pmp.ptr());
  Init();
}

TableSpatCorr::TableSpatCorr(const FileName& fn, TablePtr& p,const PointMap& pmap,
                 SPT esp, double rLagLength, long iNrDistClasses,
                 double rDirection, double rTolerance, double rBandWidth,
								 bool fSphericDist)
: TableVirtual(fn, p, true), //FileName(fn, ".TB#", true), Domain("none")),  
  pmp(pmap),
  eSPType(esp),
//  c_sSPType(sSPType),
  c_rLagLength(rLagLength),
  c_iNrDistClasses(iNrDistClasses),
  c_rDirection(rDirection),
  c_rTolerance(rTolerance),
  c_rBandWidth(rBandWidth)
{
	//Table tbl= pmp->tblAtt(); 
	//tbl->LoadData();
  ptr.SetDomain(Domain("none"));
  ptr.iRecNew(c_iNrDistClasses);
  if (pmp->dm()->pdv() == 0)
    throw ErrorValueDomain(pmp->dm()->sName(true, fnObj.sPath()), pmp->sTypeName(), 
                          errTableSpatCorr);
  fNeedFreeze = true;
  objdep.Add(pmp.ptr());
  if (!fnObj.fValid())
    objtime = objdep.tmNewest();
	if (fSphericDist)
		m_distMeth = Distance::distSPHERE;
	else
		m_distMeth = Distance::distPLANE;
}

TableSpatCorr::~TableSpatCorr()
{
}

void TableSpatCorr::Store()
{
  TableVirtual::Store();
  WriteElement("TableVirtual", "Type", "TableSpatCorr");
  WriteElement("TableSpatCorr", "PointMap", pmp);
  String sSPType;
  switch (eSPType) {
    case eLogT:       sSPType = sSPTLog;       break;
    case eOmniDirT:   sSPType = sSPTOmniDirec; break;
    case eBiDirecT:   sSPType = sSPTBiDirec;   break;
    case eBiDirecBWT: sSPType = sSPTBiDirecBW; break;
  }
  WriteElement("TableSpatCorr", "SpatCorrType", sSPType);
	String sDistanceMethod;
	switch (m_distMeth) {
		case Distance::distPLANE: sDistanceMethod = "plane"; break; 
    case Distance::distSPHERE: sDistanceMethod = "sphere"; break; 
  } 
	WriteElement("TableSpatCorr", "DistanceMethod", sDistanceMethod);
  WriteElement("TableSpatCorr", "LagLength", c_rLagLength);
  WriteElement("TableSpatCorr", "NrDistClasses", c_iNrDistClasses);
  WriteElement("TableSpatCorr", "Direction", c_rDirection);
  WriteElement("TableSpatCorr", "Tolerance", c_rTolerance);
  WriteElement("TableSpatCorr", "BandWidth", c_rBandWidth);
  
}

String TableSpatCorr::sAddInfo() const
{
  String s = String("Average over all data:\r\n %.3lf (in data units)\r\n", 
											rDataMean);
  //s &= "\r\nVariance over all data\r\n (mean of all squared deviations from the average)";
  s &= String("Variance over all data\r\n (mean of squared deviations):\r\n %.3lf (in squared data units)",
								rDataVariance);
  return s;
}

void TableSpatCorr::DetLimits(double rMaxDiff)
// make interval table for grouping of observations based on distances.
// table is based on row Er (see below) and decimal multiples.
{
  int Er[] = {10, 12, 15, 18, 22, 27, 33, 39, 47, 56, 68, 82, 100};
  int ic;// counter
  // compute first power of 10 >= rMaxDiff
  int impow10 = (int) log10(rMaxDiff) - 1;
  int iErc = 0;
  while (Er[iErc] * pow(10.0, (double) impow10) < rMaxDiff)
    iErc++;
  for (ic = c_iNrDistClasses; ic > 0; ic--)  {
    rDistLimits[ic] = Er[iErc--] * pow(10.0, (double) impow10);
    if (iErc < 0) {
      impow10--; iErc = 11;
    }
  }
}

int TableSpatCorr::iGroup(double rD)
// look with given distance rD for place in interval table
{
  int il,im,ih;
  il = 1; ih = c_iNrDistClasses;
  if (rD > rDistLimits[ih])
    return (c_iNrDistClasses+1);
  if (rD < rDistLimits[il])
    return (il);
  while (ih-il > 1) {
    im = (il+ih) / 2;
    if (rD > rDistLimits[im])
      il = im;
    else
      ih = im;
  }
  return ih;
}

bool TableSpatCorr::StatSemiVar()
{  
  long iNrPoints = pmp->iFeatures();        // iNrPoints=number of points( local copy for speed)
  long iPnt;
  LongArray iNrObserv1(1), iNrObserv2(1);
  RealArray rSemValue1(1), rSemValue2(1);
  RealArray rDistAvg1(1),rDistAvg2(1);

  ArrayLarge<double> rX(iValidPoints+1);
  ArrayLarge<double> rY(iValidPoints+1);
  ArrayLarge<double> rZ(iValidPoints+1);
  // copy input coords to X Y and Z
  long j = 1;
  Coord crdI;
  for (long i = 0; i < iNrPoints; i++) {
    if (fInvalid[i])
      continue;
    rZ[j] = pmp->rValue(i);
    crdI = pmp->cValue(i);
    rX[j] = crdI.x;
    rY[j] = crdI.y;
    j++;
  }  
  double rSin, rCos, rXt, rYt, rTol;
  double rDegToRad = M_PI /180.0;
  //if Bidirectional rotate X and Y 
  if (eSPType == eBiDirecT ||eSPType == eBiDirecBWT)  {
    rSin = sin(c_rDirection * rDegToRad);
    rCos = cos(c_rDirection * rDegToRad);
    for (long i = 0; i < iValidPoints; i++) {
      rXt = rSin * rX[i] +  rCos * rY[i];
      rYt =-rCos * rX[i] +  rSin * rY[i];
      rX[i] = rXt;
      rY[i] = rYt;
    } 
    // compute Tolerance
    rTol = tan (c_rTolerance * rDegToRad);
  }

	  // size array's to store the data and zero the value's
  iNrObserv1.Resize(c_iNrDistClasses+1);     // Number of observations
  rSemValue1.Resize(c_iNrDistClasses+1);     // Semivariogram values
  rDistAvg1.Resize(c_iNrDistClasses+1);
  rDistAvg2.Resize(c_iNrDistClasses+1);
  // Offset parameter removed from Resize functions
  for (iPnt = 0; iPnt < c_iNrDistClasses; iPnt++)  {
    iNrObserv1[iPnt] = 0;
    rSemValue1[iPnt] = 0.0;
    rDistAvg1[iPnt] = 0.0;
  }
  if (eSPType == eBiDirecT || eSPType == eBiDirecBWT) {
    iNrObserv2.Resize(c_iNrDistClasses+1);     // Number of observations
    rSemValue2.Resize(c_iNrDistClasses+1);     // Semivariogram values
     // Offset parameter removed from Resize functions
    for (iPnt = 0; iPnt < c_iNrDistClasses; iPnt++)  {
      iNrObserv2[iPnt] = 0;
      rSemValue2[iPnt] = 0.0;
      rDistAvg2[iPnt] = 0.0;
    }
  }
	
	dis = Distance(pmp->cs(), m_distMeth);
  double rDifX,rDifY,rDifZ, rDis;
  Coord cPoint_i, cPoint_j;
  trq.SetTitle(STBLTitleSpatialCorrelation);
  trq.SetText(STBLCalculate);
  for (long i = 0; i < iValidPoints; i++) {
    if (trq.fUpdate(i, iValidPoints)) 
      return false;
    for (j = i+1; j < iValidPoints; j++) {
      rDifX = rX[i] - rX[j];
      rDifY = rY[i] - rY[j];
      //rDis = sqrt(rDifX * rDifX + rDifY * rDifY);
			cPoint_i = Coord(rX[i],rY[i]);
			cPoint_j = Coord(rX[j],rY[j]);
			///rDis = sqrt(rDist2(cPoint_i, cPoint_j));
			rDis = dis.rDistance(cPoint_i, cPoint_j);
      int iGrpNr = iGroup(rDis);
      if (iGrpNr > c_iNrDistClasses)
        continue;
      rDifZ = rZ[i] - rZ[j];
      switch (eSPType)  {
        case eOmniDirT:
          iNrObserv1[iGrpNr]++;
					iNrObserv[iGrpNr]++; // to fill also the omnidir NrofPairs column
          rDistAvg1[iGrpNr] += rDis;
          rSemValue1[iGrpNr] += (rDifZ * rDifZ)/2.0;
          break;

        case eBiDirecT:
        case eBiDirecBWT:
          rDifX = abs (rDifX);
          rDifY = abs (rDifY);
          if (rDifY <= rDifX && rDifY <= rDifX * rTol)  {
            if (eSPType!=eBiDirecBWT || rDifY<=c_rBandWidth)    {
              iNrObserv1[iGrpNr]++;
              rDistAvg1[iGrpNr] += rDis;
              rSemValue1[iGrpNr] += (rDifZ * rDifZ)/2.0;
            }
          } 
          else if(rDifX <= rDifY *rTol)  {
            if (eSPType!=eBiDirecBWT || rDifX<=c_rBandWidth)    {
              iNrObserv2[iGrpNr]++;
              rDistAvg2[iGrpNr] += rDis;
              rSemValue2[iGrpNr] += (rDifZ * rDifZ)/2.0;
            }  
          }
        break;

      }    
    }
  }
   // find distance average per lag (== per distclass): for iNrObserv1 and 2 !!
  for (int iGrpNr = 1; iGrpNr <= c_iNrDistClasses; iGrpNr++) {
    if (iNrObserv1[iGrpNr] != 0)
      rDistAvg1[iGrpNr] /= (double)iNrObserv1[iGrpNr];
    else 
      rDistAvg1[iGrpNr] = rUNDEF;
    if (iNrObserv2[iGrpNr] != 0)
      rDistAvg2[iGrpNr] /= (double)iNrObserv2[iGrpNr];
    else 
      rDistAvg2[iGrpNr] = rUNDEF;
  }
  // write data in table
  for (int iRec = 1; iRec <= c_iNrDistClasses; iRec++) {
    colDist->PutVal(iRec, rDistLimits[iRec]- c_rLagLength/2.0);
		colPairs->PutVal(iRec, iNrObserv[iRec]);
    colDistAvg1->PutVal(iRec, rDistAvg1[iRec]);
    if (iNrObserv1[iRec] < 2)
			colSem1->PutVal(iRec, rUNDEF);
		else
      colSem1->PutVal(iRec, rSemValue1[iRec] / iNrObserv1[iRec]);
  }
  if (eSPType == eBiDirecT || eSPType == eBiDirecBWT) {
    for (int iRec = 1; iRec <= c_iNrDistClasses; iRec++) {
      colPairs1->PutVal(iRec, iNrObserv1[iRec]);
      colPairs2->PutVal(iRec, iNrObserv2[iRec]);
      colDistAvg2->PutVal(iRec, rDistAvg2[iRec]);
      if (iNrObserv2[iRec] < 2)
				colSem2->PutVal(iRec, rUNDEF);
			else
        colSem2->PutVal(iRec, rSemValue2[iRec] / iNrObserv2[iRec]);
    }

  }
  return true;
}


bool TableSpatCorr::fFreezing()
{
  Init();
  ptr.DeleteRec(iOffset(),iRecs());
  ptr.iRecNew(c_iNrDistClasses);

  long iNrPoints = pmp->iFeatures();        // iNrPoints=number of points( local copy for speed)
   if (iNrPoints < 3 )
   {
        throw ErrorObject(WhatError(STBLErrTooFewPoints, errTableSpatCorr+1),
                      sTypeName());
   }
	dis = Distance(pmp->cs(), m_distMeth);
  // rDistLimits contains limit values for sorting
  rDistLimits.Resize(c_iNrDistClasses+2);
  iNrObserv.Resize(c_iNrDistClasses+1);     // Number of observations
  RealArray rSumMoranI(c_iNrDistClasses+1);     // Sum of (Zi - Avg) (Zj - Avg) Moran's I
  RealArray rSumGearyC(c_iNrDistClasses+1);     // Sum of (Zi - Zj) (Zi - Zj)   Geary's c
   // Offset parameter removed from Resize functions
  double rZSumSquaredDiffs = 0.0;              // Sum of (Zi - Avg) (Zi - Avg) Both
  Coord crd;
  double rZ;
  int iRec;

  double rZmin =  DBL_MAX;
  double rZmax = -DBL_MAX;
  // look for max in min values X, Y and Z, and compute average of Z
  Coord crdMin, crdMax;
  pmp->Bounds(crdMin, crdMax);
  double rDiffX = crdMax.x - crdMin.x;
  double rDiffY = crdMax.y - crdMin.y;
  double rMaxDiff = sqrt(rDiffX * rDiffX + rDiffY * rDiffY) / 2.0;
  if (eSPType == eLogT)
    DetLimits(rMaxDiff);
  else {
    double c_rInterval = c_rLagLength/2.0;
    for (long i = 0; i <= c_iNrDistClasses; i++) {
      rDistLimits[i+1] = (double) i * c_rLagLength + c_rInterval;
		}
  }
  double rZAvg = 0.0;
//  ArrayLarge<bool> fInvalid(iNrPoints, 1);

  fInvalid.Resize(iNrPoints+1);
  iValidPoints = 0;
  for (long iPnt = 0; iPnt < iNrPoints; iPnt++) {
    crd = pmp->cValue(iPnt);
    rZ = pmp->rValue(iPnt);
    bool f = crd.x  == rUNDEF || crd.y == rUNDEF || rZ == rUNDEF;
    fInvalid[iPnt] = f;
    if (fInvalid[iPnt])
      continue;
    iValidPoints++;
      // look for domain of X, Y and attribute
    if (rZmin > rZ)
      rZmin = rZ;
    else if (rZmax < rZ)
      rZmax = rZ;
    rZAvg += rZ;
  }
  if (iValidPoints < 3)
    throw ErrorObject(WhatError(STBLErrTooFewValidPoints, errTableSpatCorr+2),
                      sTypeName());
  bool fInterrupted;  // call semivariogram if appropriate and check if stop pressed
  if (eSPType != eLogT) {
    fInterrupted = StatSemiVar();
    if (!fInterrupted)
       return(false);
  }   

  rZAvg = rZAvg / (double) iValidPoints;

  // zero Sum's
	for (long iPnt = 1; iPnt <= c_iNrDistClasses; iPnt++)  {
		iNrObserv[iPnt] = 0;
		rSumMoranI[iPnt] = 0.0;
		rSumGearyC[iPnt] = 0.0;
	}
  double rZi, rZj, rZiMinAvg, rDifZ;
  Coord crdI, crdJ;
  //double rt1, rt2;  // temp storage
  trq.SetTitle(STBLTitleSpatialCorrelation);
  trq.SetText(STBLCalculate);
  for (long i = 0; i < iNrPoints; i++) {
    if (fInvalid[i])
      continue;
    if (trq.fUpdate(i, iNrPoints))      //revision 1.6 iNrPoints was iValidPoints
      return false;
    rZi = pmp->rValue(i);
    rZiMinAvg = rZi - rZAvg;
    rZSumSquaredDiffs += rZiMinAvg * rZiMinAvg;
    crdI = pmp->cValue(i);
    for (long j = i+1; j < iNrPoints; j++) {
      if (fInvalid[j])
        continue;
      crdJ = pmp->cValue(j);
			double rDis = dis.rDistance(crdI, crdJ);
      int iGrpNr = iGroup(rDis);
      if (iGrpNr > c_iNrDistClasses)
        continue;
      rZj = pmp->rValue(j);
      rDifZ = rZi - rZj;
			iNrObserv[iGrpNr]++;
			rSumMoranI[iGrpNr] += rZiMinAvg * (rZj - rZAvg);
			rSumGearyC[iGrpNr] += rDifZ * rDifZ;
		}
  }
	rDataMean = rZAvg; 
  rDataVariance = rZSumSquaredDiffs / iValidPoints;
  for (iRec = 1; iRec <= c_iNrDistClasses; iRec++) {
    if( eSPType == eLogT)
     // in case of statsemi var distance already filled
                          // with value of the 'middle of the interval
			colDist ->PutVal(iRec, rDistLimits[iRec]);

		colPairs->PutVal(iRec, iNrObserv[iRec]);
		int tel = iNrObserv[iRec] ;
    if ( tel < 2) {
      colCorr->PutVal(iRec, rUNDEF);
      colVar ->PutVal(iRec, rUNDEF);
    }
    else  {
      colCorr->PutVal(iRec,
                 iValidPoints * rSumMoranI[iRec] /
                   iNrObserv[iRec] / rZSumSquaredDiffs);
      colVar->PutVal(iRec,
                 (iValidPoints-1) * rSumGearyC[iRec] / 2.0 /
                    iNrObserv[iRec] / rZSumSquaredDiffs);
    }
  }
  // delete undefined records at end and begin of table only in case Log
  int iTop;
  if ( eSPType == eLogT)  {
    for (iTop = c_iNrDistClasses; iTop > 0 ; iTop--)
      if (iNrObserv[iTop] > 2) break;
    ptr.DeleteRec(iTop+1, c_iNrDistClasses - iTop);
    if (iTop > 10)  {
      for(iRec = 1; iRec < (iTop - 10); iRec++)
        if (iNrObserv[iRec] > 2) break;
    }
    ptr.DeleteRec(1, iRec - 1);
  } 
	ptr.SetAdditionalInfoFlag(true);
  ptr.SetAdditionalInfo(sAddInfo());
  return true;
}


void TableSpatCorr::Init()
{
  String sDes1,sDes2;
  sFreezeTitle = "TableSpatCorr";
  htpFreeze = htpTableSpatCorrT;

  String sDescAvLag = STBLMsgAvgPointDistanceAll;
  String sDescAvLag1 = STBLMsgAvgPointDistance1;
  String sDescAvLag2 = STBLMsgAvgPointDistance2;

  if (pts==0)
    return;
  colDist = pts->col("Distance");
  if (!colDist.fValid()) {
    colDist = pts->colNew("Distance", Domain("distance"));
    colDist->sDescription = STBLMsgPointDistance;
  }
  colDist->SetOwnedByTable(true);
  colDist->SetReadOnly(true);

  colPairs = pts->col("NrPairs");
  if (!colPairs.fValid()) {
    colPairs = pts->colNew("NrPairs", Domain("count"));
    colPairs->sDescription = STBLMsgNumberPointPairs;
  }
  colPairs->SetOwnedByTable(true);
  colPairs->SetReadOnly(true);

//  if ( eSPType ==eLogT) {

    colCorr = pts->col("I");
    if (!colCorr.fValid())
      colCorr = pts->col("Correlation");
    if (!colCorr.fValid()) {
      colCorr = pts->colNew("I", Domain("value"), ValueRange(-1e10,1e10,0.001));
      colCorr->sDescription = STBLMsgCorrPointPairs;
    }
    colCorr->SetOwnedByTable(true);
    colCorr->SetReadOnly(true);

    colVar = pts->col("c");
    if (!colVar.fValid())
      colVar = pts->col("Variance");
    if (!colVar.fValid()) {
      colVar = pts->colNew("c", Domain("value"), ValueRange(0,1e10,0.01));
      colVar->sDescription = STBLMsgNormVarPointPairs;
    }
    colVar->SetOwnedByTable(true);
    colVar->SetReadOnly(true);


// }   
// else {
if (eSPType == eOmniDirT)  {
   colDistAvg1 = pts->col("AvgLag");
   if (!colDistAvg1.fValid()) {
     colDistAvg1 = pts->colNew("AvgLag", Domain("distance"));
     colDistAvg1->sDescription = sDescAvLag;
   }
   colDistAvg1->SetOwnedByTable(true);
   colDistAvg1->SetReadOnly(true);
   colSem1 = pts->col("SemiVar");
   if (!colSem1.fValid()) {
     colSem1 = pts->colNew("SemiVar", Domain("value"), ValueRange(0,1e10,0.01));
     colSem1->sDescription = STBLMsgOmniDirSemiVariogram;
   }
   colSem1->SetOwnedByTable(true);
   colSem1->SetReadOnly(true);
}
else if (eSPType > eOmniDirT)  {
   sDes1 = String("%S %.1f %S %.1f", STBLOthAngle,
                    c_rDirection, STBLOthTolerance, c_rTolerance);
   sDes2 = String("%S %.1f %S %.1f", STBLOthAngle,
                    c_rDirection + 90.0, STBLOthTolerance, c_rTolerance);
   if (eSPType == eBiDirecBWT)  {
     sDes1 = String("%S %S %.1f",sDes1,STBLOthBandWidth,c_rBandWidth);
     sDes2 = String("%S %S %.1f",sDes2,STBLOthBandWidth,c_rBandWidth);
   } 
	 
   colDistAvg1 = pts->col("AvgLag1");

   if (!colDistAvg1.fValid()) {
     colDistAvg1 = pts->colNew("AvgLag1", Domain("distance"));
     sDescAvLag1 &= sDes1;
     colDistAvg1->sDescription = sDescAvLag1;
   }
   colDistAvg1->SetOwnedByTable(true);
   colDistAvg1->SetReadOnly(true);

   colPairs1 = pts->col("NrPairs1");
   if (!colPairs1.fValid()) {
     colPairs1 = pts->colNew("NrPairs1", Domain("count"));
     colPairs1->sDescription = String(STBLMsgNumberPointPairs_S.scVal(), sDes1);
   }
   colPairs1->SetOwnedByTable(true);
   colPairs1->SetReadOnly(true);

   colSem1 = pts->col("SemiVar1");
   if (!colSem1.fValid()) {
     colSem1 = pts->colNew("SemiVar1", Domain("value"), ValueRange(0,1e10,0.01));
     colSem1->sDescription = String(STBLMsgSemiVariance_S.scVal(), sDes1);
   }
   colSem1->SetOwnedByTable(true);
   colSem1->SetReadOnly(true);

   colDistAvg2 = pts->col("AvgLag2");
   if (!colDistAvg2.fValid()) {
     colDistAvg2 = pts->colNew("AvgLag2", Domain("distance"));
     sDescAvLag2 &= sDes2;
     colDistAvg2->sDescription = sDescAvLag2;
   }
   colDistAvg2->SetOwnedByTable(true);
   colDistAvg2->SetReadOnly(true);

   colPairs2 = pts->col("NrPairs2");
   if (!colPairs2.fValid()) {
     colPairs2 = pts->colNew("NrPairs2", Domain("count"));
     colPairs2->sDescription = String(STBLMsgNumberPointPairs_S.scVal(), sDes2);
   }
   colPairs2->SetOwnedByTable(true);
   colPairs2->SetReadOnly(true);

   colSem2 = pts->col("SemiVar2");
   if (!colSem2.fValid()) {
     colSem2 = pts->colNew("SemiVar2", Domain("value"), ValueRange(0,1e10,0.01));
     colSem2->sDescription = String(STBLMsgSemiVariance_S.scVal(), sDes2);
   }
   colSem2->SetOwnedByTable(true);
   colSem2->SetReadOnly(true);
 }
}

void TableSpatCorr::UnFreeze()
{
  if (colDist.fValid()) {
    pts->RemoveCol(colDist);
    colDist = Column();
  }
  if (colDistAvg1.fValid()) {
    pts->RemoveCol(colDistAvg1);
    colDistAvg1 = Column();
  }
  if (colDistAvg2.fValid()) {
    pts->RemoveCol(colDistAvg2);
    colDistAvg2 = Column();
  }

  if (colPairs.fValid()) {
    pts->RemoveCol(colPairs);
    colPairs = Column();
  }  
  if (colCorr.fValid()) {
    pts->RemoveCol(colCorr);
    colCorr = Column();
  }  
  if (colVar.fValid()) {
    pts->RemoveCol(colVar);
    colVar = Column();
  }
  if (colPairs1.fValid()) {
    pts->RemoveCol(colPairs1);
    colPairs1 = Column();
  }

  if (colSem1.fValid()) {
    pts->RemoveCol(colSem1);
    colSem1 = Column();
  }
  if (colPairs2.fValid()) {
    pts->RemoveCol(colPairs2);
    colPairs2 = Column();
  }
  if (colSem2.fValid()) {
    pts->RemoveCol(colSem2);
    colSem2 = Column();
  }
  
  TableVirtual::UnFreeze();
}

String TableSpatCorr::sExpression() const
{
	String sDistanceMethod;
	if (m_distMeth == Distance::distSPHERE)
		sDistanceMethod = String("sphere");
	else 
		sDistanceMethod = String("plane");
  switch (eSPType)
  {
    case eLogT:
      return String("TableSpatCorr(%S)", pmp->sNameQuoted(true, fnObj.sPath()));
    case eOmniDirT:
       return String("TableSpatCorr(%S,%.2f,%S)",
        pmp->sNameQuoted(true, fnObj.sPath()),c_rLagLength,sDistanceMethod);
    case eBiDirecT:
      return String("TableSpatCorr(%S,%.2f,%.1f,%.1f)",
        pmp->sNameQuoted(true, fnObj.sPath()),c_rLagLength,
                   c_rDirection,c_rTolerance);
    case eBiDirecBWT:
      return String("TableSpatCorr(%S,%.2f,%.1f,%.1f,%.1f)",
        pmp->sNameQuoted(true, fnObj.sPath()),c_rLagLength,
                   c_rDirection,c_rTolerance,
                   c_rBandWidth);
  }
  return "";
}




