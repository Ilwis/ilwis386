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
#pragma warning( disable : 4786 )

#include "Client\Headers\formelementspch.h"
#include "Client\FormElements\FldOneSelectTextOnly.h"
#include "Client\FormElements\FieldColumnLister.h"
#include "Client\FormElements\FieldList.h"
#include "Client\FormElements\flddat.h"
#include "Client\ilwis.h"
#include "Engine\Domain\dmident.h"
#include "Engine\SpatialReference\Csproj.h"
#include "Engine\SpatialReference\csviall.h"
#include "Headers\Htp\Ilwisapp.htp"
#include "Headers\Hs\Appforms.hs"
#include "Client\Forms\DatumWizard.h"
#include "Client\FormElements\fldcol.h"
#include "Client\FormElements\InfoText.h"
#include "Client\FormElements\objlist.h"
#include "Engine\Base\Algorithm\Lstsqrs.h"
#include "Engine\Base\Tokbase.h"

#define EPS18  1.0e-18

CalculateDatumParms::CalculateDatumParms()
{
}

CalculateDatumParms* CalculateDatumParms::create(long iMethod)
{
	switch (iMethod)
	{
		case 0 : return new CalculateDatumParmsGeoCenterShift();
		case 1 : return new CalculateDatumParmsInverseMolodensky();
		case 2 : return new CalculateDatumParmsBursaWolf();
		case 3 : return new CalculateDatumParmsBadekas();
	}

	return 0;  // should not be able to reach here
}

void CalculateDatumParms::Init(const PtMapDomCsy& pmdcA, const PtMapDomCsy& pmdcB, bool fHA, bool fHB)
{
	m_pmdcA = pmdcA;
	m_pmdcB = pmdcB;
	m_fUseHeightsA = fHA;
	m_fUseHeightsB = fHB;
}

bool CalculateDatumParms::fCalc(CoordCTS& ctsPivot, bool fScaleRotF)
{
	iNrPntsCommon =	iFindValidCoordsAndHeights();
	return (iNrPntsCommon >= 1);
}

CoordCTS CalculateDatumParms::ctsGetDefaultPivot(PtCoordArrays pca)	
{
	return pca.ctsFindCentroid(iGetValidPoints());
}

String CalculateDatumParms::sResult()
{
	String sMsg = String("Using %i common points: ", iNrPntsCommon);
	sMsg &= String("\r\nfrom %i points in map %S ",
					m_pmdcA.pm->iFeatures(), m_pmdcA.pm->sName());
	sMsg &= String("\r\nand %i points in map %S ",
					m_pmdcB.pm->iFeatures(), m_pmdcB.pm->sName());
	sMsg &= String("\r\n\r\nTranslations (shifts of origin):");
	sMsg &= String("\r\ndX = %.3f m ",	Dx);
	sMsg &= String("\r\ndY = %.3f m ",	Dy);
	sMsg &= String("\r\ndZ = %.3f m  \r\n",	Dz);

  return sMsg;
}

void CalculateDatumParms::UpdateDatum()
{
	//FileName fnCs = FileName(m_pmdcB.cs->sName());
	//dat->Store(fnCs.sFileExt());
	Datum* dat = datCreate();
	CoordSystemViaLatLon *csvll1 = m_pmdcA.cs->pcsViaLatLon();
	delete 	csvll1->datum;
	csvll1->datum  = dat;
	csvll1->Updated();
	csvll1->Store();
}

bool CalculateDatumParms::fGetCoordAndConvToLL(PtMapDomCsy& pmdc, CoordSystemProjection *cspr, long i, LatLon& ll)
{
  Coord cTmp = pmdc.pm->cValue(i - 1); // i starts at 1, pointmap features start counting at 0
	CoordBounds cb = pmdc.pm->cb();
	if (!cb.fContains(cTmp))
		return false;
	
	if (cspr != 0)
		ll = cspr->llConv(cTmp);
	else
		ll = LatLon(cTmp.y, cTmp.x);

	return (!ll.fUndef());
}
	
long CalculateDatumParms::iFindValidCoordsAndHeights()
{
	if (!m_pmdcA.fValid()) 
		return 0;
	if (!m_pmdcB.fValid()) 
		return 0;
	CoordSystemViaLatLon *csvll1 = m_pmdcA.cs->pcsViaLatLon();
	CoordSystemViaLatLon *csvll2 = m_pmdcB.cs->pcsViaLatLon();
	CoordSystemProjection *cspr1 = m_pmdcA.cs->pcsProjection();
	CoordSystemProjection *cspr2 = m_pmdcB.cs->pcsProjection();
	if ((0 == csvll1 && 0 == cspr1) || (0 == csvll2 && 0 == cspr2))
		return 0;
	
	if (0 != csvll1) 
		m_ell1 = csvll1->ell;
	if (0 != csvll2) 
		m_ell2 = csvll2->ell;
	m_pcarA.ellSpheroid = m_ell1;
	m_pcarB.ellSpheroid = m_ell2;
	
	long iNrOfPts = m_pmdcA.pm->iFeatures();

	m_pcarA.lalo.Resize(iNrOfPts); // fill pcar structs with inputcoords
	m_pcarB.lalo.Resize(iNrOfPts);
	m_pcarA.rHeight.Resize(iNrOfPts); // fill pcar structs with inputheights
	m_pcarB.rHeight.Resize(iNrOfPts);

	long iNrOfValidPnts = 0;
	double rNTmp = 0, rH1Tmp, rH2Tmp;
	Coord cTmp1, cTmp2;
	LatLon llTmp1, llTmp2;

	String sCol;
	Column colA, colB;
	bool fUseHeightsA = m_fUseHeightsA && (m_pmdcA.sCol.length() > 0) && m_pmdcA.pm.fValid() && m_pmdcA.pm->fTblAtt();
	bool fUseHeightsB = m_fUseHeightsB && (m_pmdcB.sCol.length() > 0) && m_pmdcB.pm.fValid() && m_pmdcB.pm->fTblAtt();
	if (fUseHeightsA)
		colA = m_pmdcA.pm->tblAtt()->col(m_pmdcA.sCol);
	if (fUseHeightsB)
		colB = m_pmdcB.pm->tblAtt()->col(m_pmdcB.sCol);

	for (long i = 1; i <= iNrOfPts; ++i )
	{
		if (!fGetCoordAndConvToLL(m_pmdcA, cspr1, i, m_pcarA.lalo[iNrOfValidPnts]))
			continue;
		if (!fGetCoordAndConvToLL(m_pmdcB, cspr2, i, m_pcarB.lalo[iNrOfValidPnts]))
			continue;

		if (fUseHeightsA)
		{
			rH1Tmp = colA->rValue(i);
			if (rH1Tmp == rUNDEF)
				continue;
		}
		else 
			rH1Tmp = 0;

		if (fUseHeightsB)
		{
			rH2Tmp = colB->rValue(i);
			if (rH2Tmp == rUNDEF)
				continue;
		}
		else 
			rH2Tmp = 0;

		if (abs(rH1Tmp) > 1.0e8 || abs(rH2Tmp) > 1.0e8) continue;
		m_pcarA.rHeight[iNrOfValidPnts] = rH1Tmp;
		m_pcarB.rHeight[iNrOfValidPnts] = rH2Tmp;
		iNrOfValidPnts++;
  }
	m_pcarA.ctsCtrPnt.Resize(iNrOfValidPnts);
	m_pcarB.ctsCtrPnt.Resize(iNrOfValidPnts);

	return iNrOfValidPnts;
}

PtCoordArrays CalculateDatumParms::GetPtCoorsA()
{
	if ((iNrPntsCommon = iFindValidCoordsAndHeights()) == 0)
		return PtCoordArrays();
	else
		return m_pcarA;
}

struct LocalCoordCTS : public CoordCTS
{
	LocalCoordCTS() : CoordCTS() {}
	LocalCoordCTS(double _x, double _y, double _z) : CoordCTS(_x, _y, _z) {}

	LocalCoordCTS operator +=(CoordCTS& cts)
	{
		x += cts.x;
		y += cts.y;
		z += cts.z;
		return *this;
	}

	LocalCoordCTS operator /(int denominator)
	{
		return LocalCoordCTS(x / denominator, y / denominator, z / denominator);
	}

};

CoordCTS PtCoordArrays::ctsFindCentroid(long iActivePnt)
{
	LatLonHeight llh;
	CoordCTS cts;
	if (iActivePnt <= 0)
		return cts;

	LocalCoordCTS lct(0, 0, 0);
	for ( long j = 0; j < iActivePnt; j++)
	{
		llh = LatLonHeight(lalo[j], rHeight[j]);
		ctsCtrPnt[j] = ellSpheroid.ctsConv(llh);

		lct += ctsCtrPnt[j];
	}
	cts = lct / iActivePnt;

	return cts;
}

void CalculateDatumParms::UseLocalCentersToScaleAndReduce(int iPoints, 
						CoordCTS ctsCentr1, CoordCTS ctsCentr2,
                        double& rScale2ov1, double& rScale1ov2)
{
	ctsRedXYZ1.Resize(iPoints);
	ctsRedXYZ2.Resize(iPoints);
	double dxj, dyj, dzj;
	double rAvgVectorLength_1, rAvgVectorLength_2;
	double rDx2Dy2Dz2 = 0;
	for (long j = 0; j < iPoints; j++) //sum of squares of 1st pointset
	{
		dxj = ctsRedXYZ1[j].x = m_pcarA.ctsCtrPnt[j].x - ctsCentr1.x;
		dyj = ctsRedXYZ1[j].y = m_pcarA.ctsCtrPnt[j].y - ctsCentr1.y; 
		dzj = ctsRedXYZ1[j].z = m_pcarA.ctsCtrPnt[j].z - ctsCentr1.z;
		rDx2Dy2Dz2 += sqrt(dxj * dxj + dyj * dyj + dzj * dzj);
	}
	rAvgVectorLength_1 = rDx2Dy2Dz2/iPoints;
	if (rAvgVectorLength_1 < EPS18) return;
	rDx2Dy2Dz2 = 0;
	for (long j = 0; j < iPoints; j++) //sum of squares of 2nd pointset
	{
		dxj = ctsRedXYZ2[j].x = m_pcarB.ctsCtrPnt[j].x - ctsCentr2.x;
		dyj = ctsRedXYZ2[j].y = m_pcarB.ctsCtrPnt[j].y - ctsCentr2.y; 
		dzj = ctsRedXYZ2[j].z = m_pcarB.ctsCtrPnt[j].z - ctsCentr2.z;
		rDx2Dy2Dz2 += sqrt(dxj * dxj + dyj * dyj + dzj * dzj);
	}
	rAvgVectorLength_2 = rDx2Dy2Dz2/iPoints;
	if (rAvgVectorLength_1 < EPS18 || rAvgVectorLength_2 < EPS18)
		rScale2ov1 = rScale1ov2 = 1;
	else
	{
		rScale2ov1 = rAvgVectorLength_2/rAvgVectorLength_1;
		rScale1ov2 = rAvgVectorLength_1/rAvgVectorLength_2; 
	}
	for (long j = 0; j < iPoints; j++) //rescale 2nd pointset
	{
		ctsRedXYZ2[j].x *= rScale1ov2;
		ctsRedXYZ2[j].y *= rScale1ov2;
		ctsRedXYZ2[j].z *= rScale1ov2;
	}
}

bool CalculateDatumParms::fFindRotations(int iPoints, double& rXRot, double& rYRot, double& rZRot)
{
	RealMatrix AtA(3,3);
	CVector Atb(3); 
	bool fAtASingular;
	AtA(0,0) = AtA(0,1) = AtA(0,2) = 0;///  | u |   | 1   c  -b | | x |
	AtA(1,0) = AtA(1,1) = AtA(1,2) = 0;///  | v | = |-c   1   a | | y |
	AtA(2,0) = AtA(2,1) = AtA(2,2) = 0;///  | w |   | b  -a   1 | | z |
	Atb(0) = Atb(1) = Atb(2)= 0;
	for (long j = 0; j < iPoints; j++)
	{
		AtA(0,0) += sqr(ctsRedXYZ1[j].y) + sqr(ctsRedXYZ1[j].z);
		AtA(0,1) -= ctsRedXYZ1[j].x * ctsRedXYZ1[j].y;
		AtA(0,2) -= ctsRedXYZ1[j].x * ctsRedXYZ1[j].z;
		AtA(1,0) -= ctsRedXYZ1[j].x * ctsRedXYZ1[j].y;
		AtA(1,1) += sqr(ctsRedXYZ1[j].x) + sqr(ctsRedXYZ1[j].z);
		AtA(1,2) -= ctsRedXYZ1[j].y * ctsRedXYZ1[j].z;
		AtA(2,0) -= ctsRedXYZ1[j].x * ctsRedXYZ1[j].z;
		AtA(2,1) -= ctsRedXYZ1[j].y * ctsRedXYZ1[j].z;
		AtA(2,2) += sqr(ctsRedXYZ1[j].x) + sqr(ctsRedXYZ1[j].y);
		Atb(0) += ctsRedXYZ1[j].z * ctsRedXYZ2[j].y - ctsRedXYZ1[j].y * ctsRedXYZ2[j].z;		
		Atb(1) += ctsRedXYZ1[j].x * ctsRedXYZ2[j].z - ctsRedXYZ1[j].z * ctsRedXYZ2[j].x;
		Atb(2) += ctsRedXYZ1[j].y * ctsRedXYZ2[j].x - ctsRedXYZ1[j].x * ctsRedXYZ2[j].y;
	}
	fAtASingular = AtA.fSingular();
	CVector v(3);
	if (fAtASingular)
			return false;
	else 
	{
		v = LeastSquares(AtA, Atb);
		rXRot = v(0);
		rYRot = v(1);
		rZRot = v(2);
		return true;
	}
}

void CalculateDatumParms::FindTranslations(CoordCTS ctsCntr1, CoordCTS ctsCntr2,
								double rXRot, double rYRot, double rZRot,
								double& rTx, double& rTy, double& rTz,
								double rScale2ov1)
{
		rTx = ctsCntr2.x - rScale2ov1*ctsCntr1.x - rZRot*ctsCntr1.y + rYRot*ctsCntr1.z; 
		rTy = ctsCntr2.y + rZRot*ctsCntr1.x - rScale2ov1*ctsCntr1.y - rXRot*ctsCntr1.z; 
		rTz = ctsCntr2.z - rYRot*ctsCntr1.x + rXRot*ctsCntr1.y - rScale2ov1*ctsCntr1.z; 
}
	
///=========================Geo-Center Shift 

CalculateDatumParmsGeoCenterShift::CalculateDatumParmsGeoCenterShift()
:CalculateDatumParms()
{
}

bool CalculateDatumParmsGeoCenterShift::fCalc(CoordCTS& ctsPivot, bool fScaleRotF)
{
	return (CalculateDatumParms::fCalc(ctsPivot, fScaleRotF) &&
														fSolveMeanGeoCenterShift(Dx, Dy, Dz));
}

bool CalculateDatumParmsGeoCenterShift::fSolveMeanGeoCenterShift(double& dx,	double& dy, double& dz)	
{
	CoordCTS ctsA = m_pcarA.ctsFindCentroid(iNrPntsCommon);
	CoordCTS ctsB = m_pcarB.ctsFindCentroid(iNrPntsCommon);
	dx = ctsB.x - ctsA.x;
	dy = ctsB.y - ctsA.y;
	dz = ctsB.z - ctsA.z;

	return true;
}

String CalculateDatumParmsGeoCenterShift::sResult()
{
	String sMsg = CalculateDatumParms::sResult();

  return sMsg;
}

Datum* CalculateDatumParmsGeoCenterShift::datCreate()
{
	return new MolodenskyDatum(m_pmdcA.cs->pcsViaLatLon()->ell, Dx, Dy, Dz);
}

///==================Inversion of the Molodensky Transformation equations

CalculateDatumParmsInverseMolodensky::CalculateDatumParmsInverseMolodensky()
:CalculateDatumParms()
{
}
																		
bool CalculateDatumParmsInverseMolodensky::fCalc(CoordCTS& ctsPivot, bool fScaleRotF)
{
	return (CalculateDatumParms::fCalc(ctsPivot, fScaleRotF) &&
                               fSolveInverseMolodSystem(Dx, Dy, Dz));
}

double det(double a, double b, double c,
					 double d, double e, double f,
					 double g, double h, double i)
{
	return a*e*i + b*f*g + c*d*h - g*e*c - d*b*i - a*h*f;
}

bool CalculateDatumParmsInverseMolodensky::fSolveInverseMolodSystem(double& dx,
																																		double& dy,
																																		double& dz)
{
	double a1 = m_pmdcA.cs->pcsViaLatLon()->ell.a;
	double f1 = m_pmdcA.cs->pcsViaLatLon()->ell.f;
	double a = m_pmdcB.cs->pcsViaLatLon()->ell.a;
	double f = m_pmdcB.cs->pcsViaLatLon()->ell.f;
	RealArray dPhi(iNrPntsCommon);
	RealArray dLam(iNrPntsCommon);
	RealArray dHei(iNrPntsCommon);
	RealArray sinPhi(iNrPntsCommon);
	RealArray cosPhi(iNrPntsCommon);
	RealArray sinLam(iNrPntsCommon);
	RealArray cosLam(iNrPntsCommon);
	RealArray sin2Phi(iNrPntsCommon);
	RealArray Hei(iNrPntsCommon);
	double rNTmp = 0;
	for (long i = 0; i < iNrPntsCommon; i++) 
	{
		dPhi[i] = (m_pcarA.lalo[i].Lat - m_pcarB.lalo[i].Lat) *  M_PI/180.0;
		dLam[i] = (m_pcarA.lalo[i].Lon - m_pcarB.lalo[i].Lon) *  M_PI/180.0;
		dHei[i] = m_pcarA.rHeight[i] - m_pcarB.rHeight[i];
		sinPhi[i] = sin(m_pcarB.lalo[i].Lat *  M_PI/180.0);
		cosPhi[i] = cos(m_pcarB.lalo[i].Lat *  M_PI/180.0);
		sinLam[i] = sin(m_pcarB.lalo[i].Lon *  M_PI/180.0);
		cosLam[i]= cos(m_pcarB.lalo[i].Lon *  M_PI/180.0);
		Hei[i] = m_pcarB.rHeight[i];
		sin2Phi[i] = sinPhi[i] * sinPhi[i];
		rNTmp += sin2Phi[i];
	}
	long iNrOfConditionEquations = 3 * iNrPntsCommon;
	double mA[4500][3];/// overdet system of at most 4500 equations with 3 unknowns
	double cB[4500];		/// i.e. maximum of max 1500 control points
	double AtA[3][3]; /// matrix of normal equations
	double AtB[3];    /// its right-hand vector
	if (iNrPntsCommon > 1498) return false; // beyond max nr of points for solving system
	/// ======== Solve Molodensky eqs for unknowns dx dy dz 
	/// ======== and compute RMS errors for dPhi,dLam and dHei
	double da = a1 - a;
	double df = f1 - f;
	double ee = 2*f - f*f;
	double rTmp = 1 - ee * rNTmp/iNrPntsCommon;
	/// N = average radius of curvature in the prime vertical of Local ellipsoid
	double N = a / sqrt(rTmp);
	///M = average radius of curvature in the meridian of Local ellipsoid
	double M = a * (1 - ee) / sqrt(rTmp * rTmp * rTmp);
	double b = (1 - f) * a;

	for ( int i = 0; i < iNrPntsCommon; i++) 
	{
		mA[3*i][0] = sinPhi[i] * cosLam[i];
		mA[3*i][1] = sinPhi[i] * sinLam[i];
		mA[3*i][2] = -cosPhi[i];
		cB[3*i] = da*(N * ee * sinPhi[i] * cosPhi[i])/a + df*(M*a/b + N*b/a)*sinPhi[i]*cosPhi[i]
		- dPhi[i]*(M + Hei[i]);//* sinOneSec; 
			 //dPhi[i], equation for latitude difference in point [i]
		mA[3*i+1][0] = sinLam[i];
		mA[3*i+1][1] = -cosLam[i];
		mA[3*i+1][2] = 0;
		cB[3*i+1] = -(N + Hei[i]) * cosPhi[i] * dLam[i];// * sinOneSec; 
			 //dLam[i], equation for longitude difference in point [i]
		mA[3*i+2][0] = cosPhi[i] * cosLam[i];
		mA[3*i+2][1] = cosPhi[i] * sinLam[i];
		mA[3*i+2][2] = sinPhi[i];
		cB[3*i+2] = dHei[i] + da* a / N - df * b * N * sinPhi[i] * sinPhi[i] / a; 
			// dHei[i], equation for height difference in point [i]
	}
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++)
			AtA[i][j] = 0;
		AtB[i] = 0;
	}
	for (int i = 0; i < 3; i++) {
		for (int j = i; j < 3; j++) 
			for (int k = 0; k < iNrOfConditionEquations; k++) {
	  		AtA[j][i] += mA[k][i]*mA[k][j]; 
				AtA[i][j] = AtA[j][i];
			}
		for (int k = 0; k < iNrOfConditionEquations; k++)
			AtB[i] += mA[k][i]*cB[k];
	}
	// Cramer's Rule for dx and dy in "dlam - equations":
	double rDetXYZ = det( AtA[0][0],AtA[0][1],AtA[0][2], 
				AtA[1][0],AtA[1][1],AtA[1][2],
				AtA[2][0],AtA[2][1],AtA[2][2]);
	if  (  rDetXYZ < EPS18) //the system is singular;
		return false;
	else 
	{
		double rDetX = det( AtB[0],AtA[0][1],AtA[0][2],
						AtB[1],AtA[1][1],AtA[1][2],
						AtB[2],AtA[2][1],AtA[2][2]);
		double rDetY = det( AtA[0][0],AtB[0],AtA[0][2],
						AtA[1][0],AtB[1],AtA[1][2],
						AtA[2][0],AtB[2],AtA[2][2]);
		double rDetZ = det( AtA[0][0],AtA[0][1],AtB[0],
						AtA[1][0],AtA[1][1],AtB[1],
						AtA[2][0],AtA[2][1],AtB[2]);
		dx = -rDetX / rDetXYZ;
		dy = -rDetY / rDetXYZ;
		dz = -rDetZ / rDetXYZ;
		return true;
	}
}


String CalculateDatumParmsInverseMolodensky::sResult()
{
	String sMsg = CalculateDatumParms::sResult();

  return sMsg;
}

Datum* CalculateDatumParmsInverseMolodensky::datCreate()
{
	return new MolodenskyDatum(m_pmdcA.cs->pcsViaLatLon()->ell, Dx, Dy, Dz);
}

///===============Solve Shifts, ScaleDifference  and Rotations of Bursa Wolf transformation

#define EPS18  1.0e-18

CalculateDatumParmsBursaWolf::CalculateDatumParmsBursaWolf()
:CalculateDatumParms()
{
}
																		
bool CalculateDatumParmsBursaWolf::fCalc(CoordCTS& ctsPivot, bool fScaleRotF)
{
	return (CalculateDatumParms::fCalc(ctsPivot, fScaleRotF) &&
				                       fSolveBursaWolf(Dx, Dy, Dz, Rx, Ry, Rz, dS, fScaleRotF) );
}

bool CalculateDatumParmsBursaWolf::fSolveBursaWolf(double& dx, double& dy, double& dz,
																									 double& Rx, double& Ry, double& Rz,
																									 double& dScale, bool fScaleRotF)
{	
	CoordCTS ctsCentroidA = m_pcarA.ctsFindCentroid(iNrPntsCommon);
	CoordCTS ctsCentroidB = m_pcarB.ctsFindCentroid(iNrPntsCommon);
	/// If no fScaleRotF_irst, we use the method from
	/// G. Strang v Hees 'Globale en Locale Geod Systemen Delft 3e druk
	/// Hfdsk 4 vergelijking (14) en (15) Gelineariseerde gelijkv transf.
	if (!fScaleRotF)
	{	
		double dxj, dyj, dzj, dx12j, dy12j, dz12j;
		double rDx = 0, rDy = 0, rDz = 0;
		double rDxDy = 0, rDxDz = 0, rDyDz = 0;
		double rDx2Dy2 = 0, rDx2Dz2 = 0, rDy2Dz2 = 0;
		double rDx2Dy2Dz2 = 0;
		RealMatrix AtA(7,7);
		CVector Atb(7); 
		bool fAtASingular;
		AtA(0,0) = AtA(1,1) = AtA(2,2) = iNrPntsCommon; 
		AtA(0,1) = AtA(0,2) = AtA(1,2) = AtA(1,4) = AtA(2,1) = AtA(2,5) = 0;
		AtA(1,0) = AtA(2,0) = AtA(3,0) = AtA(3,6) = AtA(4,1) = AtA(4,6) = 0;
		AtA(0,3) = AtA(5,2) = AtA(5,6) = AtA(6,3) = AtA(6,4) = AtA(6,5) = 0;
		Atb(0)=Atb(1)=Atb(2)=Atb(3)=Atb(4)=Atb(5)=Atb(6)= 0;
		for (long j = 0; j < iNrPntsCommon; j++) {
			dxj = m_pcarA.ctsCtrPnt[j].x - ctsCentroidA.x; rDx += dxj;//add reduced diffs
			dyj = m_pcarA.ctsCtrPnt[j].y - ctsCentroidA.y; rDy += dyj;
			dzj = m_pcarA.ctsCtrPnt[j].z - ctsCentroidA.z; rDz += dzj;
			rDxDy += dxj * dyj;
			rDxDz += dxj * dzj;
			rDyDz += dyj * dzj;
			rDx2Dy2 += dxj * dxj + dyj * dyj;
			rDx2Dz2 += dxj * dxj + dzj * dzj;
			rDy2Dz2 += dyj * dyj + dzj * dzj;
			rDx2Dy2Dz2 += dxj * dxj + dyj * dyj + dzj * dzj;
			dx12j = m_pcarB.ctsCtrPnt[j].x - m_pcarA.ctsCtrPnt[j].x;
			dy12j = m_pcarB.ctsCtrPnt[j].y - m_pcarA.ctsCtrPnt[j].y;
			dz12j = m_pcarB.ctsCtrPnt[j].z - m_pcarA.ctsCtrPnt[j].z;
			Atb(0) += dx12j;
			Atb(1) += dy12j;
			Atb(2) += dz12j;
			Atb(3) += dy12j * dzj - dz12j * dyj;
			Atb(4) += dz12j * dxj - dx12j * dzj;
			Atb(5) += dx12j * dyj - dy12j * dxj;
			Atb(6) += dx12j*dxj + dy12j*dyj + dz12j*dzj;
		}
		AtA(1,3)= AtA(2,6)= AtA(3,1)= AtA(6,2)= rDz;
		AtA(0,4)= AtA(4,0)= -rDz;
		AtA(0,5)= AtA(1,6)= AtA(5,0)= AtA(6,1)= rDy;
		AtA(2,3)= AtA(3,2)= -rDy;
		AtA(0,6)= AtA(2,4)= AtA(4,2)= AtA(6,0)= rDx;
		AtA(1,5)= AtA(5,1)= -rDx;
		AtA(3,4)= AtA(4,3)= -rDxDy;
		AtA(3,5)= AtA(5,3)= -rDxDz;
		AtA(4,5)= AtA(5,4)= -rDyDz;
		AtA(5,5) = rDx2Dy2;
		AtA(4,4) = rDx2Dz2;
		AtA(3,3) = rDy2Dz2;
		AtA(6,6) = rDx2Dy2Dz2;
		fAtASingular = AtA.fSingular();
		CVector v(7);
	  if (fAtASingular)
			return false;
		else 
			v = LeastSquares(AtA, Atb);
		/// solutions are for the Transformation from local to global (WGS)  
		
		double dx_ = v(0);// Molod-Badekas shifts
		double dy_ = v(1);// are used to undo the
		double dz_ = v(2);// local pivot effect
		Rx = v(3); 
		Ry = v(4);
		Rz = v(5);
		dScale = v(6);
		double ctX = ctsCentroidA.x;//pivot in local coordsys
		double ctY = ctsCentroidA.y;//is pre-multiplied by the rotation-
		double ctZ = ctsCentroidA.z;//correction matrix
		dx  = dx_ - ( dScale*ctX + Rz*ctY - Ry*ctZ );
		dy  = dy_ - ( -Rz*ctX + dScale*ctY + Rx*ctZ );
		dz  = dz_ - ( Ry*ctX - Rx*ctY + dScale*ctZ );
	}
				/// If the correlation between shifts and rotaion about pivot far away
				/// must be avoided we solve the 7 parameters in successive steps:
	else  /// Solve BursaWolf params in the order: scale-ratio, rotations, shifts.
	{
		 /// Solve BursaWolf params in the order: scale-ratio, rotations, shifts.
		double rScale2over1, rScale1over2;
		UseLocalCentersToScaleAndReduce(iNrPntsCommon, ctsCentroidA, ctsCentroidB, rScale2over1, rScale1over2);
		dScale = rScale2over1 - 1; 
		double rXR, rYR, rZR; // rotations to be found
		if (!fFindRotations(iNrPntsCommon, rXR, rYR, rZR))
			return false;
		Rx = rXR; 
		Ry = rYR; 
		Rz = rZR;
		double rTx, rTy, rTz; // translations to be found
		FindTranslations(ctsCentroidA, ctsCentroidB,  rXR, rYR, rZR, rTx, rTy, rTz, rScale2over1);
		dx = rTx; 
		dy = rTy; 
		dz = rTz;
		/// Thus, first the reduced cartesian coordinates are brought to equal scale
		/// Then the linearized rotations are computed putting sin a = a, cos a = 1
		/// At last the translations are computed
	}
	return true;
}

String CalculateDatumParmsBursaWolf::sResult()
{
		ctsUsedPivot.x = 0.0;
		ctsUsedPivot.y = 0.0;
		ctsUsedPivot.z = 0.0;

	String sMsg = CalculateDatumParms::sResult();
	sMsg &= String("\r\nRotations:");
	sMsg &= String("\r\nRx = %.6f micro-rad = %.4f arcsec",
															 Rx*1000000, Rx*180*3600/M_PI);
	sMsg &= String("\r\nRy = %.6f micro-rad = %.4f arcsec", 
															 Ry*1000000, Ry*180*3600/M_PI);
	sMsg &= String("\r\nRz = %.6f micro-rad = %.4f arcsec", 
															 Rz*1000000, Rz*180*3600/M_PI);
	sMsg &= String("\r\n\r\nScale difference:");
	sMsg &= String("\r\ndScale = %.12f = %.6f ppm",dS, dS*1000000);

	/////////// Find accuracy of datumshift for all ctrl points:
	double diff, diffLatLonmm, diffHmm;
	double RMS_Phi = 0; // rootMeansSquare error in latitude shift
	double RMS_Lam = 0; // rootMeansSquare error in longitude shift
	double RMS_Hei = 0; // rootMeansSquare error in height shift
	double maxPhiErr = -1;
	double maxLamErr = -1;
	double maxHeiErr = -1;
	long iPhi, iLam, iHei;
	LatLonHeight llh_i, llhDatumShifted;
	CoordCTS ctsIn, ctsOut;
	sMsg &= String("\r\n\r\nAccuracy for Lat, Lon and Height:\r\n");
	sMsg &= String("using %li common points, ",iNrPntsCommon);
	sMsg &= String("\r\n\r\nDeviations: (dLat, dLon, dHeight) in mm\r\n");
	sMsg &= m_ell2.sName;
	sMsg &= String(" (transformed from ");
	sMsg &= m_ell1.sName;
	sMsg &= String(")  -- (minus) ");
	sMsg &= m_ell2.sName;
	sMsg &= String(" (given) ");
	sMsg &= String("\r\n\r\nPointnr");
	double cosPhiFactor;
	for ( long i = 0; i < iNrPntsCommon; i++) 
	{		
		llh_i = LatLonHeight(m_pcarA.lalo[i], m_pcarA.rHeight[i]);
		cosPhiFactor = cos(llh_i.Lat*M_PI/180.0);
		ctsIn = m_ell1.ctsConv(llh_i);
		ctsOut = m_ell2.ctsConv(ctsIn, ctsUsedPivot, Dx, Dy, Dz, Rx, Ry, Rz, dS);  
		llhDatumShifted = m_ell2.llhConv(ctsOut);
		///////////////////// latitudes difference
		diff = (llhDatumShifted.Lat - m_pcarB.lalo[i].Lat);
		diffLatLonmm = diff*111111111.1;//~from degr to millimeter
		sMsg &= String("\r\nnr %li:\t %.6f ", i+1,diffLatLonmm);
		if (abs(diff) > maxPhiErr) {
			maxPhiErr = abs(diff);
			iPhi = i + 1;
		}
	 	RMS_Phi += diff * diff;
		///////////////////// longitudes difference
		diff = (llhDatumShifted.Lon - m_pcarB.lalo[i].Lon);
		diffLatLonmm = diff* cosPhiFactor * 111111111.1;//~from degr to millimeter
		sMsg &= String("\t%.6f ", diffLatLonmm);
		if (abs(diff) > maxLamErr) {
			maxLamErr = abs(diff);
			iLam = i + 1;
		}
	 	RMS_Lam += diff * diff;
		///////////////////// heights difference
		diff = llhDatumShifted.rHeight - m_pcarB.rHeight[i];
		diffHmm = diff*1000;// to millimeter
		sMsg &= String("\t%.6f ", diffHmm);
		if (abs(diff) > maxHeiErr) {
			maxHeiErr = abs(diff);
			iHei = i + 1;
		}
	 	RMS_Hei += diff * diff;
	}
	RMS_Phi = sqrt(RMS_Phi / iNrPntsCommon);//RMS in degrees
	RMS_Lam = sqrt(RMS_Lam / iNrPntsCommon);
	RMS_Hei = sqrt(RMS_Hei / iNrPntsCommon);
	double RMS_Phi_mm = RMS_Phi * 111111111.1;//convert from degr to mm
	double RMS_Lam_mm = RMS_Lam * cosPhiFactor * 111111111.1;
	double maxPhiErr_mm = maxPhiErr * 111111111.1; //conv from degr to mm
	double maxLamErr_mm = maxLamErr * cosPhiFactor * 111111111.1;
	
	//==================================================================
	//========== display these results in a message box ================
	//==================================================================
	sMsg &= String("\r\nout of  %li valid control points.\r\n",m_pmdcA.pm->iFeatures());
	sMsg &= String("by retransforming the active points through CTS conversion.\r\n");
	sMsg &= String("\r\nRMS_Lat = %.9f arcsec (= ~ %.8f mm)",RMS_Phi*3600.0, RMS_Phi_mm);
	sMsg &= String("\r\nRMS_Lon = %.9f arcsec (= ~ %.8f mm)",RMS_Lam*3600.0, RMS_Lam_mm);
	sMsg &= String("\r\nRMS_Height = %.3f mm",RMS_Hei*1000);
	sMsg &= String("\r\nMax Lat Error = %.9f arcsec (~ %.8f mm) at pnt %li",maxPhiErr*3600.0, maxPhiErr_mm, iPhi);
	sMsg &= String("\r\nMax Lon Error = %.9f arcsec (~ %.8f mm) at pnt %li",maxLamErr*3600.0, maxLamErr_mm, iLam);
	sMsg &= String("\r\nMax Height Error = %.3f mm at pnt %li",maxHeiErr*1000, iHei);
	
  return sMsg;
}

Datum* CalculateDatumParmsBursaWolf::datCreate()
{
	return new BursaWolfDatum(m_pmdcA.cs->pcsViaLatLon()->ell, Dx, Dy, Dz, Rx, Ry, Rz, dS);
}

///===============Solve Shifts, ScaleDifference  and Rotations of Badekas transformation

CalculateDatumParmsBadekas::CalculateDatumParmsBadekas()
:CalculateDatumParms()
{
}
																		
bool CalculateDatumParmsBadekas::fCalc(CoordCTS& ctsPivot, bool fScaleRotF)
{
	if (CalculateDatumParms::fCalc(ctsPivot, fScaleRotF) &&
				fSolveBadekas(Dx, Dy, Dz, Rx, Ry, Rz, dS, ctsPivot, fScaleRotF))
		return true;
	else 
		return false;
}

bool CalculateDatumParmsBadekas::fSolveBadekas(double& dx, double& dy, double& dz,
																							 double& Rx, double& Ry, double& Rz,
																							double& dScale, CoordCTS ctsPiv, bool fScaleRotF)
{
	/// If no fScaleRotF_irst, we use the method from
	/// G. Strang v Hees 'Globale en Locale Geod Systemen Delft 3e druk
	/// Hfdsk 4 vergelijking (14) en (15) Gelineariseerde gelijkv transf.

	CoordCTS ctsCentroidA = m_pcarA.ctsFindCentroid(iNrPntsCommon);
	CoordCTS ctsCentroidB = m_pcarB.ctsFindCentroid(iNrPntsCommon);

	if (!fScaleRotF)
	{	
		double dxj, dyj, dzj, dx12j, dy12j, dz12j;
		double rDx = 0, rDy = 0, rDz = 0;
		double rDxDy = 0, rDxDz = 0, rDyDz = 0;
		double rDx2Dy2 = 0, rDx2Dz2 = 0, rDy2Dz2 = 0;
		double rDx2Dy2Dz2 = 0;
		RealMatrix AtA(7,7);
		CVector Atb(7); 
		bool fAtASingular;
		AtA(0,0) = AtA(1,1) = AtA(2,2) = iNrPntsCommon; 
		AtA(0,1) = AtA(0,2) = AtA(1,2) = AtA(1,4) = AtA(2,1) = AtA(2,5) = 0;
		AtA(1,0) = AtA(2,0) = AtA(3,0) = AtA(3,6) = AtA(4,1) = AtA(4,6) = 0;
		AtA(0,3) = AtA(5,2) = AtA(5,6) = AtA(6,3) = AtA(6,4) = AtA(6,5) = 0;
		Atb(0)=Atb(1)=Atb(2)=Atb(3)=Atb(4)=Atb(5)=Atb(6)= 0;
		for (long j = 0; j < iNrPntsCommon; j++) {
			dxj = m_pcarA.ctsCtrPnt[j].x - ctsPiv.x; rDx += dxj;//add reduced diffs
			dyj = m_pcarA.ctsCtrPnt[j].y - ctsPiv.y; rDy += dyj;
			dzj = m_pcarA.ctsCtrPnt[j].z - ctsPiv.z; rDz += dzj;
			rDxDy += dxj * dyj;
			rDxDz += dxj * dzj;
			rDyDz += dyj * dzj;
			rDx2Dy2 += dxj * dxj + dyj * dyj;
			rDx2Dz2 += dxj * dxj + dzj * dzj;
			rDy2Dz2 += dyj * dyj + dzj * dzj;
			rDx2Dy2Dz2 += dxj * dxj + dyj * dyj + dzj * dzj;
			dx12j = m_pcarB.ctsCtrPnt[j].x - m_pcarA.ctsCtrPnt[j].x;
			dy12j = m_pcarB.ctsCtrPnt[j].y - m_pcarA.ctsCtrPnt[j].y;
			dz12j = m_pcarB.ctsCtrPnt[j].z - m_pcarA.ctsCtrPnt[j].z;
			Atb(0) += dx12j;
			Atb(1) += dy12j;
			Atb(2) += dz12j;
			Atb(3) += dy12j * dzj - dz12j * dyj;
			Atb(4) += dz12j * dxj - dx12j * dzj;
			Atb(5) += dx12j * dyj - dy12j * dxj;
			Atb(6) += dx12j*dxj + dy12j*dyj + dz12j*dzj;
		}
		AtA(1,3)= AtA(2,6)= AtA(3,1)= AtA(6,2)= rDz;
		AtA(0,4)= AtA(4,0)= -rDz;
		AtA(0,5)= AtA(1,6)= AtA(5,0)= AtA(6,1)= rDy;
		AtA(2,3)= AtA(3,2)= -rDy;
		AtA(0,6)= AtA(2,4)= AtA(4,2)= AtA(6,0)= rDx;
		AtA(1,5)= AtA(5,1)= -rDx;
		AtA(3,4)= AtA(4,3)= -rDxDy;
		AtA(3,5)= AtA(5,3)= -rDxDz;
		AtA(4,5)= AtA(5,4)= -rDyDz;
		AtA(5,5) = rDx2Dy2;
		AtA(4,4) = rDx2Dz2;
		AtA(3,3) = rDy2Dz2;
		AtA(6,6) = rDx2Dy2Dz2;
		fAtASingular = AtA.fSingular();
		CVector v(7);
	  if (fAtASingular)
			return false;
		else 
			v = LeastSquares(AtA, Atb);
		/// solutions are for the Transformation from local to global (WGS)  
		
		dx = v(0);
		dy = v(1);
		dz = v(2);
		Rx = v(3);
		Ry = v(4);
		Rz = v(5);
		dScale = v(6);
	}
				/// If the correlation between shifts and rotaion about pivot far away
				/// must be avoided we solve the 7 parameters in successive steps:
	else  /// Solve BursaWolf params in the order: scale-ratio, rotations, shifts.
	{
		double rScale2over1, rScale1over2;
		UseLocalCentersToScaleAndReduce(iNrPntsCommon, ctsCentroidA, ctsCentroidB, rScale2over1, rScale1over2);
		dScale = rScale2over1 - 1; 
		double rXR, rYR, rZR; // rotations to be found
		if (!fFindRotations(iNrPntsCommon, rXR, rYR, rZR))
			return false;
		Rx = rXR; 
		Ry = rYR; 
		Rz = rZR;
		double rTx, rTy, rTz; // translations to be found
		ctsCentroidA.x -= ctsPiv.x;
		ctsCentroidA.y -= ctsPiv.y;
		ctsCentroidA.z -= ctsPiv.z;
		ctsCentroidB.x -= ctsPiv.x;
		ctsCentroidB.y -= ctsPiv.y;
		ctsCentroidB.z -= ctsPiv.z;
		FindTranslations(ctsCentroidA, ctsCentroidB, rXR, rYR, rZR, rTx, rTy, rTz, rScale2over1);
		dx = rTx; 
		dy = rTy; 
		dz = rTz;
		/// Thus, first the reduced cartesian coordinates are brought to equal scale
		/// Then the linearized rotations are computed putting sin a = a, cos a = 1
		/// At last the translations are computed
		/// solutions are for the Transformation from local to global (WGS)  
	}
	X0 = ctsUsedPivot.x;
	Y0 = ctsUsedPivot.y;
	Z0 = ctsUsedPivot.z;
	return true;
}

String CalculateDatumParmsBadekas::sResult()
{
	String sMsg = String("Datum point (pivot of rotations)");
	sMsg &= String("\r\nXo = %7.6f", ctsUsedPivot.x);
	sMsg &= String("\r\nYo = %7.6f", ctsUsedPivot.y);
	sMsg &= String("\r\nZo = %7.6f\r\n\r\n", ctsUsedPivot.z);
	sMsg &= CalculateDatumParms::sResult();
	sMsg &= String("\r\nRotations:");
	sMsg &= String("\r\nRot X = %.6f micro-rad = %.4f arcsec",
																Rx*1000000, Rx*180*3600/M_PI);
	sMsg &= String("\r\nRot Y = %.6f micro-rad = %.4f arcsec", 
																Ry*1000000, Ry*180*3600/M_PI);
	sMsg &= String("\r\nRot Z = %.6f micro-rad = %.4f arcsec", 
																Rz*1000000, Rz*180*3600/M_PI);
	sMsg &= String("\r\n\r\nScale difference:");
	sMsg &= String("\r\ndScale = %.12f = %.6f ppm",dS, dS*1000000);
	
	///////////
	/////////// Find accuracy of datumshift for all ctrl points:
	double diff, diffLatLonmm, diffHmm;
	double RMS_Phi = 0; // rootMeansSquare error in latitude shift
	double RMS_Lam = 0; // rootMeansSquare error in longitude shift
	double RMS_Hei = 0; // rootMeansSquare error in height shift
	double maxPhiErr = -1;
	double maxLamErr = -1;
	double maxHeiErr = -1;
	long iPhi, iLam, iHei;
	LatLonHeight llh_i, llhDatumShifted;
	CoordCTS ctsIn, ctsOut;
	sMsg &= String("\r\n\r\nAccuracy for Lat, Lon and Height:\r\n");
	sMsg &= String("using %li common points, ",iNrPntsCommon);
	sMsg &= String("\r\n\r\nDeviations: (dLat, dLon, dHeight) in mm\r\n");
	sMsg &= m_ell2.sName;
	sMsg &= String(" (transformed from ");
	sMsg &= m_ell1.sName;
	sMsg &= String(")  -- (minus) ");
	sMsg &= m_ell2.sName;
	sMsg &= String(" (given) ");
	sMsg &= String("\r\n\r\nPointnr");
	double cosPhiFactor;
	for ( long i = 0; i < iNrPntsCommon; i++) 
	{		
		llh_i = LatLonHeight(m_pcarA.lalo[i], m_pcarA.rHeight[i]);
		cosPhiFactor = cos(llh_i.Lat*M_PI/180.0);
		ctsIn = m_ell1.ctsConv(llh_i);
		ctsOut = m_ell2.ctsConv(ctsIn, ctsUsedPivot, Dx, Dy, Dz, Rx, Ry, Rz, dS);  
		llhDatumShifted = m_ell2.llhConv(ctsOut);
		diff = (llhDatumShifted.Lat - m_pcarB.lalo[i].Lat);
		diffLatLonmm = diff*111111111.1;//~from degr to millimeter
		sMsg &= String("\r\nnr %li:\t %.6f ", i+1,diffLatLonmm);
		if (abs(diff) > maxPhiErr) {
			maxPhiErr = abs(diff);
			iPhi = i + 1;
		}
	 	RMS_Phi += diff * diff;
		
		diff = (llhDatumShifted.Lon - m_pcarB.lalo[i].Lon);
		diffLatLonmm = diff* cosPhiFactor * 111111111.1;//~from degr to millimeter
		sMsg &= String("\t%.6f ", diffLatLonmm);
		if (abs(diff) > maxLamErr) {
			maxLamErr = abs(diff);
			iLam = i + 1;
		}
	 	RMS_Lam += diff * diff;
		diff = llhDatumShifted.rHeight - m_pcarB.rHeight[i];
		diffHmm = diff*1000;// to millimeter
		sMsg &= String("\t%.6f ", diffHmm);
		if (abs(diff) > maxHeiErr) {
			maxHeiErr = abs(diff);
			iHei = i + 1;
		}
	 	RMS_Hei += diff * diff;
	}
	RMS_Phi = sqrt(RMS_Phi / iNrPntsCommon);//RMS in degrees
	RMS_Lam = sqrt(RMS_Lam / iNrPntsCommon);
	RMS_Hei = sqrt(RMS_Hei / iNrPntsCommon);
	double RMS_Phi_mm = RMS_Phi * 111111111.1;//convert from degr to mm
	double RMS_Lam_mm = RMS_Lam * cosPhiFactor * 111111111.1;
	double maxPhiErr_m = maxPhiErr * 111111111.1; //conv from degr to mm
	double maxLamErr_m = maxLamErr * cosPhiFactor * 111111111.1;
	
	//==================================================================
	//========== display these results in a message box ================
	//==================================================================
	
	sMsg &= String("\r\nout of  %li valid control points.\r\n",m_pmdcA.pm->iFeatures());
	sMsg &= String("by retransforming the active points through CTS conversion.\r\n");
	sMsg &= String("\r\nRMS_Lat = %.9f arcsec (= ~ %.8f mm)",RMS_Phi*3600.0, RMS_Phi_mm);
	sMsg &= String("\r\nRMS_Lon = %.9f arcsec (= ~ %.8f mm)",RMS_Lam*3600.0, RMS_Lam_mm);
	sMsg &= String("\r\nRMS_Height = %.3f mm",RMS_Hei*1000);
	sMsg &= String("\r\nMax Lat Error = %.9f arcsec (~ %.8f mm) at pnt %li",maxPhiErr*3600.0, maxPhiErr_m, iPhi);
	sMsg &= String("\r\nMax Lon Error = %.9f arcsec (~ %.8f mm) at pnt %li",maxLamErr*3600.0, maxLamErr_m, iLam);
	sMsg &= String("\r\nMax Height Error = %.3f mm at pnt %li",maxHeiErr*1000, iHei);
	
	return sMsg;
}

Datum* CalculateDatumParmsBadekas::datCreate()
{
	return new BadekasDatum(m_pmdcA.cs->pcsViaLatLon()->ell, Dx, Dy, Dz, 
													Rx, Ry, Rz, dS, X0, Y0, Z0);
}


///=====================================================================================================
/// ---------------------- SelectPointMapsPage
SelectPointMapsPage::SelectPointMapsPage(DatumWizard* dw, const char* sPar)
: FormBaseWizardPage(TR("Find Datum Parameters from Common Control Points"))
{
	if (sPar) {
    TextInput inp(sPar);
    TokenizerBase tokenizer(&inp);
    String sVal;
    for (;;) {
      Token tok = tokenizer.tokGet();
      sVal = tok.sVal();
      if (sVal == "")
        break;
      FileName fn(sVal);
      if (fn.sExt == "" || fn.sExt == ".mpp")
        if (sMapA == "")
          sMapA = fn.sFullNameQuoted(false);
        else  if (sMapB == "")
          sMapB = fn.sFullNameQuoted(false);
    }
  }
	m_dw = dw;
	m_dw->fUseHeightsA = false;
	m_dw->fUseHeightsB = false;
	iNrPntsA = 9999;
	iNrPntsB = 9999;
	FormEntry* root = feRoot();
	StaticText* st = new StaticText(root, TR("Select Point Maps and optional Height columns"), true);
	st->SetIndependentPos();
	fgA = new FieldGroup(root,true);
	fldMapA = new FieldPointMap(fgA, TR("Point Map with &Local Datum"), &sMapA, new MapListerDomainType(".mpp", dmIDENT, false));;
	fldMapA->SetCallBack((NotifyProc)&SelectPointMapsPage::MapACallBack);
	cbUseHeightsA = new CheckBox(fgA, TR("Use Heights (m)"), &m_dw->fUseHeightsA);
	fldColA = new FieldColumn(cbUseHeightsA,"",Table(),&sColA, dmVALUE);
	String sPntsA = String(TR("Number of control points %li").c_str(), iNrPntsA);
	stNrPntsA = new StaticText(fgA, sPntsA);
	stNrPntsA->Align(cbUseHeightsA, AL_UNDER);
	stNrPntsA->SetIndependentPos();
	String sCsyA = String(TR("&Coordinate System").c_str(), sCoordSysA);
	stCsyNameA = new StaticText(fgA, sCsyA);
	stCsyNameA->SetWidth(150);
	stCsyNameA->SetIndependentPos();
	sDatumA= String();
	String sDatumNameA = String(TR("Current Datum is %S").c_str(), sDatumA);
	stDatumNameA = new StaticText(fgA, sDatumNameA);
	stDatumNameA->SetWidth(150);
	stDatumNameA->SetIndependentPos();
	// clean the message places 
	stNrPntsA->SetVal("");
	stCsyNameA->SetVal("");
	stDatumNameA->SetVal("");
	
	fgB = new FieldGroup(root,true);
	fgB->Align(fgA, AL_UNDER);
	fldMapB = new FieldPointMap(fgB, TR("Point Map with &Global Datum"), &sMapB, new MapListerDomainType(".mpp", dmIDENT, false));
	fldMapB->SetCallBack((NotifyProc)&SelectPointMapsPage::MapBCallBack);
	cbUseHeightsB = new CheckBox(fgB, TR("Use Heights (m)"), &m_dw->fUseHeightsB);
	fldColB = new FieldColumn(cbUseHeightsB,"",Table(),&sColB, dmVALUE);
	String sPntsB = String(TR("Number of control points %li").c_str(), iNrPntsB);
	stNrPntsB = new StaticText(fgB, sPntsB);
	stNrPntsB->Align(cbUseHeightsB, AL_UNDER);
	stNrPntsB->SetIndependentPos();
	String sCsyB = String(TR("&Coordinate System").c_str(),sCoordSysB);
	stCsyNameB = new StaticText(fgB, sCsyB);
	stCsyNameB->SetWidth(150);
	stCsyNameB->SetIndependentPos();
	sDatumB= String();
	String sDatumNameB = String(TR("Global Datum is %S").c_str(), sDatumB);
	stDatumNameB = new StaticText(fgB, sDatumNameB);
	stDatumNameB->SetWidth(150);
	stDatumNameB->SetIndependentPos();
	stRemark = new InfoText(root,"xxxxxxxxxxxxxiiiiiiiixxxxxxxxxxxxxxxxxxxxxxxxxx");//reserve char positions
	// clean the message places 
	stRemark->SetVal(""); 
	stNrPntsB->SetVal("");
	stCsyNameB->SetVal("");
	stDatumNameB->SetVal("");
}

void SelectPointMapsPage::SetDefaultPointMaps()
{
	if (m_dw->fnPntA.fValid() && (m_dw->fnPntA.sExt == "" || m_dw->fnPntA.sExt == ".mpp"))
		sMapA = m_dw->fnPntA.sFullNameQuoted(false);

	if (m_dw->fnPntB.fValid() && (m_dw->fnPntB.sExt == "" || m_dw->fnPntB.sExt == ".mpp"))
		sMapB = m_dw->fnPntB.sFullNameQuoted(false);
}

BOOL SelectPointMapsPage::OnSetActive() 
{
	FormBaseWizardPage::OnSetActive();
	m_dw->SetIcon(zIcon("CsyIcon"), TRUE);
	m_dw->SetWizardButtons(0);
	if (fCheckDomainEquality(m_dw->pmdcA, m_dw->pmdcB))
		m_dw->SetWizardButtons(PSWIZB_NEXT);
	return TRUE;
}	

LRESULT SelectPointMapsPage::OnWizardNext()
{
	feRoot()->StoreData();
	m_dw->pmdcA.sCol = fldColA->sName();
	m_dw->pmdcB.sCol = fldColB->sName();

	if (m_dw->cdp)
		delete m_dw->cdp;  // To make sure we create the correct one without memory leaks
	m_dw->cdp = CalculateDatumParms::create(3);//case Badekas to make pivot
	m_dw->cdp->Init(m_dw->pmdcA, m_dw->pmdcB, m_dw->fUseHeightsA, m_dw->fUseHeightsB);
	if (0 != m_dw->cdp)
	{
		PtCoordArrays pca = m_dw->cdp->GetPtCoorsA();
		if (!pca.fValid()) return -1;
		CoordCTS ctsDefaultPivot = m_dw->cdp->ctsGetDefaultPivot(pca);
		m_dw->ctsWizPivot = ctsDefaultPivot;
	}

	SwitchToNextPage(m_dw->ppSelectMethod);
	return -1;
}

bool SelectPointMapsPage::fCheckDomainEquality(PtMapDomCsy pmdcA, PtMapDomCsy pmdcB)
{
	if (!pmdcA.pm.fValid() || !pmdcB.pm.fValid() )
		return false;

	if (pmdcA.dm.fValid() && pmdcB.dm.fValid() && pmdcA.dm == pmdcB.dm)
		EnableOK();
	else
	{
		m_dw->SetWizardButtons(0);
		stRemark->SetVal("Maps should have equal identifier domains");
		pmdcA.dm = Domain();  // don't leave old domains
		pmdcB.dm = Domain();  
		return false;
	}
	return true; // domain and coordsys checks were succesful
}

PtMapDomCsy SelectPointMapsPage::BuildAndCheckStruct(const String& sMap)
{
	if (sMap.length() == 0)
		return PtMapDomCsy();

	FileName fnMap(sMap);
	if (!fnMap.fValid())
		return PtMapDomCsy();

	PtMapDomCsy pmdc;
	pmdc.pm = PointMap(fnMap);
	pmdc.cs = pmdc.pm->cs();
	if (!pmdc.pm->dm()->pdid())  // Only DomainIdentifier allowed for the PointMap
	{
		stRemark->SetVal("Map should have identifier domain");
		pmdc.dm = Domain();  // No left-overs please
		return PtMapDomCsy();
	}
	pmdc.dm = pmdc.pm->dm();
	CoordSystemViaLatLon* csvll = pmdc.cs->pcsViaLatLon();
	if (0 == csvll)
	{	
		stRemark->SetVal("Coordinate System should be LatLon or Projected");
		return PtMapDomCsy();
	}
	Ellipsoid ell = pmdc.cs->pcsViaLatLon()->ell;
	if (ell.fSpherical())
	{
		stRemark->SetVal("No valid ellipsoid set");
		return PtMapDomCsy();
	}

	return pmdc;
}

int SelectPointMapsPage::MapACallBack(Event*)
{
	String sOldMapName = sMapA;
	fgA->StoreData();
	if (sMapA == sOldMapName)
		return 0;

	stRemark->SetVal("");
	stNrPntsA->SetVal("");
	stCsyNameA->SetVal("");
	stDatumNameA->SetVal("");
	DisableOK();
	m_dw->pmdcA = BuildAndCheckStruct(sMapA);

	if (!m_dw->pmdcA.fValid()) 
		return 0;

	iNrPntsA = m_dw->pmdcA.pm->iFeatures();
	String sPntsA = String(TR("Number of control points %li").c_str(), iNrPntsA);
	stNrPntsA->SetVal(sPntsA);

	sCoordSysA = m_dw->pmdcA.cs->sName();
	sEllipsoidA = m_dw->pmdcA.cs->pcsViaLatLon()->ell.sName;
	if(m_dw->pmdcA.cs->pcsViaLatLon()->datum != 0)
		sDatumA = m_dw->pmdcA.cs->pcsViaLatLon()->datum->sName();
	else
		sDatumA = String("unspecified");
	String sCsyA = String(TR("Coordinate System %S with Ellipsoid %S").c_str(), sCoordSysA, sEllipsoidA);
	stCsyNameA->SetVal(sCsyA);
	String sDatumNameA = String(TR("Current Datum is %S").c_str(), sDatumA);
	stDatumNameA->SetVal(sDatumNameA);
	if (fCheckDomainEquality(m_dw->pmdcA, m_dw->pmdcB))
		m_dw->SetWizardButtons(PSWIZB_NEXT);
	fldColA->FillWithColumns(m_dw->pmdcA.pm->tblAtt().ptr());
	return 0;
}

int SelectPointMapsPage::MapBCallBack(Event*)
{
	String sOldMapName = sMapB;
	fgB->StoreData();
	if (sMapB == sOldMapName)
		return 0;

	stRemark->SetVal("");
	stNrPntsB->SetVal("");
	stCsyNameB->SetVal("");
	stDatumNameB->SetVal("");
	DisableOK();
	m_dw->pmdcB = BuildAndCheckStruct(sMapB);
	if (!m_dw->pmdcB.fValid()) 
		return 0;

	iNrPntsB = m_dw->pmdcB.pm->iFeatures();
	String sPntsB = String(TR("Number of control points %li").c_str(), iNrPntsB);
	stNrPntsB->SetVal(sPntsB);

	sCoordSysB = m_dw->pmdcB.cs->sName();
	sEllipsoidB = m_dw->pmdcB.cs->pcsViaLatLon()->ell.sName;
	if(m_dw->pmdcB.cs->pcsViaLatLon()->datum != 0)
		sDatumB = m_dw->pmdcB.cs->pcsViaLatLon()->datum->sName();
	else
		sDatumB = String("unspecified");
	String sCsyB = String(TR("Coordinate System %S with Ellipsoid %S").c_str(), sCoordSysB, sEllipsoidB);
	stCsyNameB->SetVal(sCsyB);
	String sDatumNameB = String(TR("Global Datum is %S").c_str(), sDatumB);
	stDatumNameB->SetVal(sDatumNameB);
	if (fCheckDomainEquality(m_dw->pmdcA, m_dw->pmdcB))
		m_dw->SetWizardButtons(PSWIZB_NEXT);
	fldColB->FillWithColumns(m_dw->pmdcB.pm->tblAtt().ptr());
	return 0;
}

///---------------- SelectMethodPage
SelectMethodPage::SelectMethodPage(DatumWizard* dw) : FormBaseWizardPage(TR("Find Datum Parameters from Common Control Points"))
{
	m_dw = dw;
	m_dw->iMethod = 0;
	
	m_dw->fScaleAndRotationFirst = false;
	FormEntry* root = feRoot();
	StaticText* st = new StaticText(root, TR("Select Datum Transformation Method"), true);
	st->SetIndependentPos();
	rgMethod = new RadioGroup(root, "", &m_dw->iMethod, true);
	rgMethod->SetIndependentPos();
	rgMethod->SetCallBack((NotifyProc)&SelectMethodPage::MethodCallBack);
	RadioButton *rb1 = new RadioButton(rgMethod, "&Geo-centric shift");
	RadioButton *rb2 = new RadioButton(rgMethod, "&Molodensky");
	rb2->Align(rb1, AL_UNDER);
	RadioButton *rb3 = new RadioButton(rgMethod, "Bursa &Wolf");
	rb3->Align(rb2, AL_UNDER);
	//cbScaleAndRotationFirstBW = new CheckBox(rb3, TR("&Calculate Scale and Rotations First"), &m_dw->fScaleAndRotationFirst);
	//cbScaleAndRotationFirstBW->Align(rb3, AL_AFTER);
	RadioButton *rb4 = new RadioButton(rgMethod, "Molodensky &Badekas");
	rb4->Align(rb3, AL_UNDER);
	//cbScaleAndRotationFirstMB = new CheckBox(rb4, TR("&Calculate Scale and Rotations First"), &m_dw->fScaleAndRotationFirst);
	//cbScaleAndRotationFirstMB->Align(rb4, AL_AFTER);
	stRemark = new InfoText(root,"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
	stRemark->SetHeight(40);
	stRemark->Align(rb4, AL_UNDER);
	m_fgUserDefP = new FieldGroup(rb4, true);
	m_fgUserDefP->Align(stRemark, AL_UNDER);
  frXCoord = new FieldReal(m_fgUserDefP, TR("&Xo ="), &m_dw->ctsWizPivot.x, ValueRange(-1e7,1e7,0.001));
  frYCoord = new FieldReal(m_fgUserDefP, TR("&Yo ="), &m_dw->ctsWizPivot.y, ValueRange(-1e7,1e7,0.001));
  frYCoord->Align(frXCoord, AL_AFTER);
	frZCoord = new FieldReal(m_fgUserDefP, TR("&Zo ="), &m_dw->ctsWizPivot.z, ValueRange(-1e7,1e7,0.001));
  frZCoord->Align(frYCoord, AL_AFTER);
}

int SelectMethodPage::MethodCallBack(Event*)
{
	rgMethod->StoreData();
	stRemark->SetVal("");

	switch (m_dw->iMethod) {
    case 0:
      stRemark->SetVal("Molodensky parameters, \ncomputed from mean shift of geo-centric coordinates");
      break;
    case 1: 
			stRemark->SetVal("Molodensky parameters, \ncomputed from inverted transformation equations");
			break;
		case 2: 
			stRemark->SetVal("Bursa Wolf, 7 parameters, \nshifts, rotations and scale change");
			break;
		case 3: 
			{
				stRemark->SetVal("Badekas, 7 + 3 parameters, \nshifts, rotations and scale change \nwith local rotation pivot:");
				m_fgUserDefP->StoreData();
				if (m_dw->cdp)
					delete m_dw->cdp;  // To make sure we create the correct one without memory leaks
				m_dw->cdp = CalculateDatumParms::create(m_dw->iMethod);

				m_dw->cdp->Init(m_dw->pmdcA, m_dw->pmdcB, m_dw->fUseHeightsA, m_dw->fUseHeightsB);
				PtCoordArrays pca = m_dw->cdp->GetPtCoorsA();
				if (!pca.fValid()) //PtCoordArrays 
					return 0;

				frXCoord->SetVal(m_dw->ctsWizPivot.x);
				frYCoord->SetVal(m_dw->ctsWizPivot.y);
				frZCoord->SetVal(m_dw->ctsWizPivot.z);
				break;
			}
  }
	return 0;
}

BOOL SelectMethodPage::OnSetActive() 
{
	FormBaseWizardPage::OnSetActive();

	if (m_dw->pmdcA.fValid() && m_dw->pmdcB.fValid())
		m_dw->SetWizardButtons(PSWIZB_BACK | PSWIZB_NEXT);
	else
		m_dw->SetWizardButtons(PSWIZB_BACK);
	if (m_dw->cdp)
					delete m_dw->cdp;  // To make sure we create the correct one without memory leaks
				m_dw->cdp = CalculateDatumParms::create(m_dw->iMethod);

				m_dw->cdp->Init(m_dw->pmdcA, m_dw->pmdcB, m_dw->fUseHeightsA, m_dw->fUseHeightsB);
	frXCoord->SetVal(m_dw->ctsWizPivot.x);
	frYCoord->SetVal(m_dw->ctsWizPivot.y);
	frZCoord->SetVal(m_dw->ctsWizPivot.z);
	return TRUE;
}

BOOL SelectMethodPage::OnKillActive()
{
	FormBaseWizardPage::OnKillActive();
	//cbScaleAndRotationFirstBW->StoreData();
	//cbScaleAndRotationFirstMB->StoreData();

	return TRUE;
}


LRESULT SelectMethodPage::OnWizardNext()
{
	frXCoord->StoreData();
	frYCoord->StoreData();
	frZCoord->StoreData();
	SwitchToNextPage(m_dw->ppUpdateDatum);
	
	return -1;
}

///---------------- UpdateDatumPage

BEGIN_MESSAGE_MAP(UpdateDatumPage, FormBaseWizardPage)
	ON_WM_TIMER()
END_MESSAGE_MAP()


UpdateDatumPage::UpdateDatumPage(DatumWizard* dw) : FormBaseWizardPage(TR("Find Datum Parameters from Common Control Points"))
{
	m_dw = dw;

	FormEntry* root = feRoot();
	StaticText* st = new StaticText(root, TR("Datum Calculation Results"), true);
	st->SetIndependentPos();
	stRemark = new InfoText(root,"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
	stRemark2 = new InfoText(root,"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
	fsm = new FieldStringMulti(root, &m_s, true);
	fsm->SetHeight(210);
	fsm->SetWidth(160);
	cbUpdateDatum = new CheckBox(root, String(), &m_dw->fUpdateDatum);
	cbUpdateDatum->SetIndependentPos();
	stCsyNameA1 = new StaticText(root, TR("&Save Datum Parameters as user-defined Datum"));
	stCsyNameA1->Align(cbUpdateDatum, AL_AFTER);
	stCsyNameA2 = new StaticText(root, "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
	stCsyNameA2->Align(stCsyNameA1, AL_UNDER);
	stCsyNameA2->SetWidth(180);
	stCsyNameA2->psn->iBndUp = -10;
	stRemark3 = new InfoText(root,"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
	stRemark3->Align(fsm, AL_UNDER);
}

void UpdateDatumPage::SetShiftString(const String& sShift)
{
	m_s = sShift;
}

BOOL UpdateDatumPage::OnSetActive() 
{
	stRemark->SetVal("");
	stRemark2->SetVal("");
	String sCoordSysA = m_dw->pmdcA.cs->sName();
	stCsyNameA2->SetVal(String(TR("in Coordinate System %S").c_str(), sCoordSysA));
	cbUpdateDatum->StoreData();
	String sDatumTrans;
	switch (m_dw->iMethod)
	{
		case 0 : sDatumTrans = String("Molodensky "); break;
		case 1 : sDatumTrans = String("Molodensky "); break;
		case 2 : sDatumTrans = String("BursaWolf "); break;
		case 3 : sDatumTrans = String("Badekas ");
	}
	sDatumTrans	&= "Datum transformation parameters";
	stRemark->SetVal(sDatumTrans);
	
	String sLocaltoGlobal = String("From Local ellipsoid ");
	sLocaltoGlobal &= m_dw->pmdcA.cs->pcsViaLatLon()->ell.sName;
	sLocaltoGlobal &= String(" to Global ellipsoid ");
	sLocaltoGlobal &= m_dw->pmdcB.cs->pcsViaLatLon()->ell.sName;
	stRemark2->SetVal(sLocaltoGlobal);

	FormBaseWizardPage::OnSetActive();

	String sOutputText;
	bool fSucces = false;
	m_dw->cdp = CalculateDatumParms::create(m_dw->iMethod);
	m_dw->cdp->ctsUsedPivot = CoordCTS(m_dw->ctsWizPivot);
	m_dw->cdp->Init(m_dw->pmdcA, m_dw->pmdcB, m_dw->fUseHeightsA, m_dw->fUseHeightsB);
	fSucces = m_dw->cdp->fCalc(m_dw->ctsWizPivot, m_dw->fScaleAndRotationFirst);
	//cbUpdateDatum->SetVal(true);

	if (!fSucces)
	{
		//m_dw->SetWizardButtons(PSWIZB_BACK | PSWIZB_DISABLEDFINISH);
		cbUpdateDatum->Hide();
		stCsyNameA1->Hide();
		stCsyNameA2->Hide();
		stRemark3->Show();
		stRemark3->SetVal("To save the datum parameters, calculation must be possible");
		fsm->SetVal("calculation of parameters failed");
	}
	else if (m_dw->pmdcB.cs->pcsViaLatLon()->ell.sName != "WGS 84") // Check op WGS84 voor de eerste pointmap
	{
		//m_dw->SetWizardButtons(PSWIZB_BACK | PSWIZB_DISABLEDFINISH);
		cbUpdateDatum->Hide();
		stCsyNameA1->Hide();
		stCsyNameA2->Hide();
		stRemark3->Show();
		stRemark3->SetVal("To save the datum paramaters, second map should have the WGS84 ellipsoid");
		fsm->SetVal(m_dw->cdp->sResult());
	}
	else
	{
		//m_dw->SetWizardButtons(PSWIZB_BACK | PSWIZB_FINISH);
		cbUpdateDatum->Show();
		stCsyNameA1->Show();
		stCsyNameA2->Show();
		stRemark3->SetVal("");
		stRemark3->Hide();
		fsm->SetVal(m_dw->cdp->sResult());
	}
	m_nTimer = SetTimer(1, 50, 0);//nodig omdat ScrollToTopLine hier te vroeg gebeurt (PropertySheet is nog niet klaar)
	// lukt niet met PostMessage 

	return TRUE;
}

void UpdateDatumPage::OnTimer(UINT nIDEvent) 
{ 
	KillTimer(m_nTimer);
	fsm->ScrollToTopLine(); //zo zet ik de scrollbar bovenaan

	// Call base class handler.
	FormBaseWizardPage::OnTimer(nIDEvent);
}

// compare with exec() function
BOOL UpdateDatumPage::OnWizardFinish()
{
	cbUpdateDatum->StoreData();
	if (m_dw->fUpdateDatum)
			m_dw->cdp->UpdateDatum();
	return -1;
}

LRESULT UpdateDatumPage::OnWizardBack()
{
	EnablePreviousPage();
	return -1;
}		

// The  wizard implementation

DatumWizard::DatumWizard(CWnd* wnd, const char* sPar)
: CPropertySheet(TR("Find Datum Parameters from Common Control Points").c_str(), wnd), 
	cdp(0), fUpdateDatum(false), fScaleAndRotationFirst(false)
{
	// Initialize the wizard
	SetWizardMode();

	ppSelectPointMaps = new SelectPointMapsPage(this, sPar);
	ppSelectPointMaps->SetMenHelpTopic("ilwisapp\find_datum_trans_params_inputpage.htm");
	ppSelectPointMaps->create();
	AddPage(ppSelectPointMaps);

	// which method to choose
	ppSelectMethod = new SelectMethodPage(this);
	ppSelectMethod->SetMenHelpTopic("ilwisapp\find_datum_trans_params_methodpage.htm");
	ppSelectMethod->create();
	AddPage(ppSelectMethod);
	ppSelectMethod->SetPPDisable();

	// 
	ppUpdateDatum = new UpdateDatumPage(this);
	ppUpdateDatum->SetMenHelpTopic("ilwisapp\find_datum_trans_params_outputpage.htm");
	ppUpdateDatum->create();
	AddPage(ppUpdateDatum);
	ppUpdateDatum->SetPPDisable();
}

DatumWizard::~DatumWizard()
{
	if (cdp)
		delete cdp;
}
