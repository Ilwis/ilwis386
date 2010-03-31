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
/* Ellipsoid
   Copyright Ilwis System Development ITC
   march 1996, by Wim Koolhoven
	Last change:  WK   10 Jul 97    9:39 am
*/

/*
   Formules uit Co"ordinaattransformaties en kaartprojecties
   A.J.M. K"osters, Afdeling MTO, Meetkundige Dienst
   Delft, augustus 1993
   G. Strang v Hees 'Globale en Locale Geod. Systemen Delft 3e druk, 1997
	 Hfdst 2 Ellipsoidale systemen vergelijking (6) en (7)
	 Hfdsk 4 vergel.  (9), (10) en (11) Gelineariseerde gelijkv transf.

	ellips.def afgeleid uit ellips.dat van MaDTran
*/   

#include "Engine\SpatialReference\Ellips.h"
#include "Engine\Base\DataObjects\ERR.H"
#include "Headers\Err\ILWISDAT.ERR"
#include "Headers\Hs\proj.hs"
#include "Engine\SpatialReference\Ellips.h"
#include "Engine\Applications\objvirt.h"
#include "Engine\Applications\ModuleMap.h"
#include "Engine\Base\System\Engine.h"

#define EPS15	1.e-15
#define EPS12	1.e-12

Ellipsoid::Ellipsoid()
: sName("Sphere")
{
  init(6371007.1809185,0);  // sphere with equal area as WGS 84
//  init(6378137.0, 298.257223563);
}

Ellipsoid::Ellipsoid (double a, double f1)
: sName("User Defined")
{
  if (f1 == 0)
    sName = "Sphere";
  init(a,f1);
}

Ellipsoid::Ellipsoid(const String& sEll)
: sName(sEll)
{
  if (fCIStrEqual("Sphere" , sEll)) {
    init(6371007.1809185,0);  // sphere with equal area as WGS 84
    return;
  }
  String s;
  String sPath = getEngine()->getContext()->sIlwDir();
  sPath &= "\\Resources\\Def\\ellips.def";
  char sBuf[100];
  if (0 == GetPrivateProfileString("Ellipsoids", sName.scVal(), "", sBuf, 100, sPath.scVal())) 
    throw ErrorNotFound(sEll);
  double rMajor, f1;  
  if (2 != sscanf(sBuf, "%lf,%lf", &rMajor, &f1))
    InvalidEllipsoidDef();
  init(rMajor,f1);  
}

void Ellipsoid::InvalidEllipsoidDef() {
  throw ErrorObject(SPRJErrInvalidEllipsDefine, errEllipsoid);
}
    
void Ellipsoid::init(double rMajor, double f1)
{
  a = rMajor;
  if (f1 <= 1)
    f = 0;
  else  
    f = 1/f1;
  e2 = 2 * f - f * f;
  e = sqrt(e2);
  b = a * (1 - f);
}

bool Ellipsoid::fEqual(const Ellipsoid& ell) const
{
  return a == ell.a && f == ell.f;
}

void Ellipsoid::GetEllipsoids(char* sBuf, int iSize)
{
  String sPath = getEngine()->getContext()->sIlwDir();
  sPath &= "\\Resources\\Def\\ellips.def";
  GetPrivateProfileString("Ellipsoids", NULL, "", sBuf, iSize, sPath.scVal());
}


// Koesters pag. 12, formules 4 en 5.
CoordCTS Ellipsoid::ctsConv(const LatLonHeight& llh) const
{
  CoordCTS cts;
  double phi = llh.Phi();
  double lambda = llh.Lambda();
  double sinPhi = sin(phi);
  double cosPhi = cos(phi);
  double sinLambda = sin(lambda);
  double cosLambda = cos(lambda);
  double N = a / sqrt(1 - e2 * sinPhi * sinPhi);
  double h = llh.rHeight;
  cts.x = (N + h) * cosPhi * cosLambda;
  cts.y = (N + h) * cosPhi * sinLambda;
  cts.z = (N * (1 - e2) + h) * sinPhi;
  return cts;
}

 //G. Strang v Hees 'Globale en Locale Geod. Systemen
 //Hfdst 2 Ellipsoidale systemen vergelijking (6) en (7) (iteratief bepalen van phi)
LatLonHeight Ellipsoid::llhConv(const CoordCTS& cts) const
{ 
  LatLonHeight llh;
	double r = sqrt(cts.x * cts.x + cts.y * cts.y); 
	//double eps = (a*a - b*b) / (a*a);
	double phiTry = 0;
	double phiNext = atan2((cts.z + e2*cts.z),r);
	double N = a; // start approximation
	while (abs(phiNext - phiTry) > EPS15 ) {
		phiTry = phiNext; // update trial phi
		N = a / sqrt(1 - e2*sin(phiNext)*sin(phiNext));
		phiNext = atan2((cts.z + e2*N*sin(phiNext)),r);
	}
	llh.Phi(phiNext);//set phi back to lat in degrees
	llh.Lambda(atan2( cts.y, cts.x));// lon in degrees
	llh.rHeight = r*cos(phiNext) + cts.z*sin(phiNext) 
		- a*sqrt(1 - e2*sin(phiNext)*sin(phiNext));
	return llh;/*
}
// Koesters pag. 12, formules 6, 7 en 8.
// methode van Bowring
LatLonHeight Ellipsoid::llhConv(const CoordCTS& cts) const
{ 
  LatLonHeight llh;*/
  double eps2 = (a*a - b*b) / (b*b);
  //double r = sqrt(cts.x * cts.x + cts.y * cts.y);
  double tanTheta = cts.z * a / (r * b);
  double theta = atan(tanTheta);
  double sinTheta = sin(theta);
  double cosTheta = cos(theta);
  double sin3Theta = sinTheta * sinTheta * sinTheta;
  double cos3Theta = cosTheta * cosTheta * cosTheta;
  double tanPhi = (cts.z + eps2 * b * sin3Theta) / (r - e2 * a * cos3Theta);
  double phi = atan(tanPhi);
  double sinPhi = sin(phi);
  double cosPhi = cos(phi);
  N = a / sqrt(1 - e2 * sinPhi * sinPhi);
  llh.Phi(phi);
  llh.Lambda(atan2(cts.y, cts.x));
  llh.rHeight = r / cosPhi - N;
  return llh;
}

LatLonHeight Ellipsoid::llhConv(const Ellipsoid& ell, const LatLonHeight& llh) const
{
  double da = a - ell.a;
  double df = f - ell.f;
  
  double phi = llh.Phi();
  double lam = llh.Lambda();
  double h = llh.rHeight;
  double sinPhi = sin(phi);
  double cosPhi = cos(phi);
  double sinLam = sin(lam);
  double cosLam = cos(lam);
  double sin2Phi = sinPhi * sinPhi;

  // n = radius of curvature in the prime vertical
  double n = ell.a / sqrt(1 - ell.e2 * sin2Phi);
  // m = radius of curvature in the meridian
  double rTmp = 1 - ell.e2 * sin2Phi;
  double m = ell.a * (1 - ell.e2) / sqrt(rTmp * rTmp * rTmp);
  
  double dPhi, dLam, dh;
  
  dPhi = + da * (n * ell.e2 * sinPhi * cosPhi) / ell.a
         + df * (m * ell.a / ell.b + n * ell.b / ell.a) * sinPhi * cosPhi;
  dPhi /= m + h;
  
  dLam = 0;
  
  dh = - da * ell.a / n + df * n * sin2Phi * ell.b / ell.a;

  phi += dPhi;
  lam += dLam;
  h += dh;
  LatLonHeight llhRes;
  llhRes.Phi(phi);
  llhRes.Lambda(lam);
  llhRes.rHeight = h;           
  return llhRes;     
}

/*
	Formules from "Map Projections - A Working Manual"  John P.Snyder USGS Professional Paper 1395
	US Govt Printing Office, Washington 1987  page 199
	following Robbin's inverse of 'Clarke's best formula' (see also Bomford G. 1971, Geodesy: 
	Oxford England, Clarendon Press
  suitable for lines up to 800 km;  
*/
double Ellipsoid::rEllipsoidalDistance(const LatLon&
																			 llBegin, const LatLon& llEnd) const
																			//plStart, PhiLam plEnd, Ellips ell,
                        //double& rDist, double& rAzim)

{
	double c = rUNDEF;
	if (abs(llBegin.Lat) + EPS15 > 90 || abs(llEnd.Lat) + EPS15 > 90 )
		return c; // invalid latitudes for reliable computation
  double phi1 = llBegin.Lat* M_PI/180.0; //conversion to radians
  double lam1 = llBegin.Lon* M_PI/180.0;
  double phi2 = llEnd.Lat * M_PI/180.0; //conversion to radians
  double lam2 = llEnd.Lon * M_PI/180.0;
  //double a = ell.a;
  double f_inv = 1 / f;
  //double b = a / (1.0 + 1.0 /f_inv);
  //double e2 = (a*a -b*b) /a/a;
  //double e = sqrt(e2);
  double N1, N, psi, s;
  double G, H, s2, s4;
  long signCosAz;
  N1 = a /sqrt(1 - e2 * sin(phi1)*sin(phi1));
	N = a /sqrt(1 - e2 * sin(phi2)*sin(phi2));
	psi = atan((1-e2)*tan(phi2) + e2*N1*sin(phi1)/N/cos(phi2));
	double azim = atan2(sin(lam2-lam1),cos(phi1)*tan(psi) - sin(phi1)*cos(lam2-lam1));
	signCosAz = (cos(azim)>0)?1:-1;
	if (abs(sin(azim)) < EPS12 )
		s = signCosAz * asin(cos(phi1)*sin(psi) - sin(phi1)*cos(psi));
	else
		s = asin(sin(lam2-lam1)*cos(psi)/sin(azim));
	G = e*sin(phi1)/sqrt(1 - e2);
	H = e * cos(phi1)*cos(azim)/sqrt(1 - e2);
	s2 = s*s;  s4 =s2*s2;
	c = N1*s*(1.0-s2*H*H*(1.0-H*H)/6+(s*s2/8)*G*H*(1-2*H*H)
		+(s4/120)*(H*H*(4-7*H*H)-3*G*G*(1-7*H*H)) - (s4*s/48)*G*H);
  return c;
  //rAzim = azim  * 180.0 / M_PI;
}

double Ellipsoid::rEllipsoidalAzimuth(const LatLon& llBegin, const LatLon& llEnd) const
{
	double rAzim = rUNDEF;
	if (abs(llBegin.Lat) + EPS15 > 90 || abs(llEnd.Lat) + EPS15 > 90 )
		return rAzim; // invalid latitudes for reliable computation
  double phi1 = llBegin.Lat* M_PI/180.0; //conversion to radians
  double lam1 = llBegin.Lon* M_PI/180.0;
  double phi2 = llEnd.Lat * M_PI/180.0; //conversion to radians
  double lam2 = llEnd.Lon * M_PI/180.0;
  double f_inv = 1 / f;
  double N1, N, psi;
  N1 = a /sqrt(1 - e2 * sin(phi1)*sin(phi1));
	N = a /sqrt(1 - e2 * sin(phi2)*sin(phi2));
	psi = atan((1-e2)*tan(phi2) + e2*N1*sin(phi1)/N/cos(phi2));
	double azim = atan2(sin(lam2-lam1),cos(phi1)*tan(psi) - sin(phi1)*cos(lam2-lam1));
	rAzim = azim  * 180.0 / M_PI;
	return rAzim;
}

///G. Strang v Hees 'Globale en Lokale Geod. Systemen Delft 3e druk, 1997
///Hfdsk 4 vergel.(10)

CoordCTS Ellipsoid::ctsConv(const CoordCTS& ctsIn, const CoordCTS& ctsPivot,
														double tx, double ty, double tz, 
														double Rx, double Ry, double Rz,
														double s) const//3D linearized similarity
{
	///3 + 3 + 3 + 1 transformation constants 
//double x0, y0, z0; // position of transf centroid (0,0,0 if centroid == geocenter) 
//double tx, ty, tz; // translations (shifts)  meters
//double Rx, Ry, Rz; // rotations (off-diagonal elements in rot matrix, quasi sines) radians
//double s;          //  scale difference (diagonal elements in rot matrix) nano-radians
	CoordCTS ctsOut;
	double x = ctsIn.x;
	double y = ctsIn.y;
	double z = ctsIn.z; 
	double x0 = ctsPivot.x;
	double y0 = ctsPivot.y;
	double z0 = ctsPivot.z;  
	// input coordinates from reference_system_1 (meters geocentric)

	ctsOut.x = x + tx + s * (x - x0) + Rz * (y - y0) - Ry * (z - z0) ;
	ctsOut.y = y + ty - Rz * (x - x0) + s * (y - y0) + Rx * (z - z0) ;
	ctsOut.z = z + tz + Ry * (x - x0) - Rx * (y - y0) + s * (z - z0) ;
  // output coordinates to reference_system_2  (meters geocentric)

	return ctsOut;
}
