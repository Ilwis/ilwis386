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
/* ProjectionMollweide
   Copyright Ilwis System Development ITC
   march 1995, by Wim Koolhoven
	Last change:  WK   21 Mar 96    2:38 pm
*/

#include "Engine\SpatialReference\MOLL.H"

ProjectionMollweide::ProjectionMollweide(const Ellipsoid& ell)
: ProjectionPtr(ell)
{
  fEll = true;
  authset(apa);
  qp = qsfn(M_PI_2);
}

#define C1R     .90031631615710606956 //=2*sqrt(2)/pi
#define C2R     1.41421356237309504880 // sqrt(2)
#define EPS     1e-15
#define EPS10   1e-10
#define EPS8    1e-8
#define NITER   10

void ProjectionMollweide::Prepare()
{
// find tot area of reference ellpsoid:(default WGS
		double a = ell.a;
		double a2 = ell.a*ell.a;
		double e2 = ell.e2;
		double e = ell.e;
		if (e > EPS)
			fTrulyEllipsoid = true;
		else
			fTrulyEllipsoid = false;
		double p;
		if (fTrulyEllipsoid) {// ellipsoid 
			c = a2*(1-e2)*M_PI/e/2;
			double b = 2*e/(1-e*e);
			double OppEllipsoid = 2*c*(b+log((1+e)/(1-e)));
			// find dimension of Mollweide ellpse:
			p2 = OppEllipsoid/M_PI/2; // ~81179464998630.for wgs
			//p is short axis Moll ellipse 9009964.7612313 for wgs
		}
		else 
			p2 = a2*2; // 2 for unit sphere
		p = sqrt(p2); //sqrt(2) ~ 1.4142 for unit sphere
		rLatitWithTrueScale = rLatitudeWithTrueScale();
}

XY ProjectionMollweide::xyConv(const PhiLam& pl) const
{
  XY xy;
	PhiLam _pl = pl;
  if (abs(pl.Phi)> M_PI_2) return xy;
  if (abs(pl.Lam)> M_PI) return xy;
  //double rBeta;
  double ph, th, dth;
	th = pl.Phi;
  if (fTrulyEllipsoid) {/* ellipsoid */
		double qs = qsfn(pl.Phi);
		double qsoverqp = qs/qp;
		th= asin(qsfn(pl.Phi) / qp);//authalic lat 
		double thDegrees = th*180.0/M_PI;
		// equating Frustum (fr 0 to Phi) and ellips-strip (fr 0 to th)
		double theta;
		double t2 = ell.e * sin(pl.Phi); // t2 = var in Frustum integral
		double h = 2*t2/(1-t2*t2) + log((1+t2)/(1-t2));
		double rFrust = c * h; // 
		theta = pl.Phi; //first guess theta for Newton-iteration
		ph = rFrust / p2 ;
		double phCheck = M_PI * sin(pl.Phi);//spheric case
		for (int j = NITER; j ; --j) {
			dth = (ph - theta - sin(theta)) / (1. + cos(theta));
			theta += dth;
			if (abs(dth) < EPS)
				break;
		}
		theta *= .5;
		xy.x = sqrt(p2)* pl.Lam / M_PI_2 * cos(theta) / ell.a;
		xy.y = sqrt(p2)* sin(theta) / ell.a;
//		return xy;
	}
  else  { /*sphere*/
		ph = M_PI * sin(th);
	
		for (int i = NITER; i ; --i) {
			dth = (ph - th - sin(th)) / (1. + cos(th));
			th += dth;
			if (abs(dth) < EPS)
				break;
		}
		th *= .5;
		double cossqth = cos(th)*cos(th);
		double thplussin2th_sqrd = (th+sin(2*th))*(th+sin(2*th));
		double froverpisqrd = 4/M_PI/M_PI;
		double one = 1/2/cossqth + thplussin2th_sqrd*froverpisqrd;
		xy.x = C1R * pl.Lam * cos(th);
		xy.y = C2R * sin(th);
	}
	double h = rScaleAlongMeridian(_pl);
	double k = rScaleAlongParallel(_pl);
	 double rScaleArea = h * k;
  return xy;
}

PhiLam ProjectionMollweide::plConv(const XY& xy) const
{
  PhiLam pl;
  double th, s;
  th = xy.y / C2R;
  s = abs(th);
  if (s < 1) {
    th = asin(th);
    pl.Lam = xy.x / (C1R * cos(th));
    th += th;
    pl.Phi = asin((th + sin(th)) / M_PI);
  } 
  else if ((s - EPS10) > 1.)
    {}
  else {
    pl.Lam = 0.;
    pl.Phi = th < 0. ? -M_PI_2 : M_PI_2;
  }
  if (fTrulyEllipsoid) {     /* ellipsoid */
		//back from authalic to ellipsoidal lat
		double q = qp * sin (pl.Phi);  // JPSnyder (3-17)
		///double R_authal = ell.a*sqrt(qp/2);
	  pl.Phi = phi1(q);    // JPSnyder (3-16)

		// solve theta equation for phi, if theta is given
		double theta = asin(ell.a*xy.y/sqrt(p2));
		double G = 2*theta + sin(2*theta);
		double K = c/p2;
		double dt = 100; //succesive increments 
		double t = ell.e*sin(theta); //start VALUE for iteration
		double H = 2*t/(1-t*t) + log((t+1)/(t-1));//integrand
		// c*H is area of frustum from 0 to pl.Phi
		double AreaFrust = c * H;
		double dHdt;// = 4/(1-t*t)/(1-t*t);
		//Newton-Raphson t2-t1 = - H/dHdt  
		for (int i = NITER; i ; --i) {
			H = 2*t/(1-t*t) + log((1+t)/(1-t));
			dHdt = 4/(1-t*t)/(1-t*t);
			dt = -(K*H - G) / K/dHdt;
			t += dt;
			if (abs(dt) < EPS)
				break;
		}
		pl.Phi = asin(t/ell.e);
		pl.Lam = xy.x* ell.a/ cos(theta)/sqrt(p2) * M_PI_2;
	}
	
  return pl;
}

Datum* ProjectionMollweide::datumDefault() const
{
  return new MolodenskyDatum("WGS 1984", "");
}

double ProjectionMollweide::rScaleAlongMeridian(const PhiLam& pl) const
{
  PhiLam plN = pl; plN.Phi += 0.0000001;
  XY xy, xyE, xyN; //xyAt_pl, xyEastward, Northward;
  /// compute local radius of curvature of meridian  (Snyder 4 - 22):
  double rho = (1- ell.e2)/pow((1-ell.e2*sin(pl.Phi)*sin(pl.Phi)),1.5);
  /// compute local radius of curvature in prime meridian plane  (Snyder 4 - 23):
  double nu = 1/sqrt(1-ell.e2*sin(pl.Phi)*sin(pl.Phi));
  double ph, th, dth, thN;
  th = pl.Phi;
  thN = plN.Phi;
	if ( !ell.fSpherical()) {/* ellipsoid */
		
		// equating Frustum (fr 0 to Phi) and ellips-strip (fr 0 to th)
		double theta;
		double t2 = ell.e * sin(pl.Phi); // t2 = var in Frustum integral
		double h = 2*t2/(1-t2*t2) + log((1+t2)/(1-t2));
		double rFrust = c * h; // 
		theta = pl.Phi; //first guess theta for Newton-iteration
		ph = rFrust / p2 ;
		double phCheck = M_PI * sin(pl.Phi);//spheric case
		for (int j = NITER; j ; --j) {
			dth = (ph - theta - sin(theta)) / (1. + cos(theta));
			theta += dth;
			if (abs(dth) < EPS)
				break;
		}
		theta *= .5;
		xy.x = sqrt(p2)* pl.Lam / M_PI_2 * cos(theta) / ell.a;
		xy.y = sqrt(p2)* sin(theta) / ell.a;
		
		/// plN forward projected
		 // equating Frustum (fr 0 to Phi) and ellips-strip (from 0 to theta)
		t2 = ell.e * sin(plN.Phi); // t2 = var in Frustum integral
		h = 2*t2/(1-t2*t2) + log((1+t2)/(1-t2));
		rFrust = c * h; // 
		theta = plN.Phi; //first guess theta for Newton-Raphson iteration
		ph = rFrust / p2 ;
		phCheck = M_PI * sin(plN.Phi);//spheric case
		for (int j = NITER; j ; --j) {
			dth = (ph - theta - sin(theta)) / (1. + cos(theta));
			theta += dth;
			if (abs(dth) < EPS)
				break;
		}
		theta *= .5;
		xyN.x = sqrt(p2)* plN.Lam / M_PI_2 * cos(theta) / ell.a;
		xyN.y = sqrt(p2)* sin(theta) / ell.a;
  }
  else  
  { 
	/* sphere */
	  ph = M_PI * sin(th);
	
	  for (int i = NITER; i ; --i) {
		dth = (ph - th - sin(th)) / (1. + cos(th));
		th += dth;
		if (abs(dth) < EPS)
			break;
	  }
    th *= .5;
	
    xy.x = C1R * pl.Lam * cos(th);
    xy.y = C2R * sin(th);
		 /// compute differentally the scalefactor h (northward):

		// forward projection of plN
		double phN = M_PI * sin(thN);
		for (int i = NITER; i ; --i) {
			dth = (phN - thN - sin(thN)) / (1. + cos(thN));
			thN += dth;
			if (abs(dth) < EPS)
				break;
		}
			thN *= .5;
		xyN.x = C1R * plN.Lam * cos(thN);
    xyN.y = C2R * sin(thN);
	}
  double rScaleNorth = (xyN.y - xy.y) / (plN.Phi - pl.Phi) / rho;
	return rScaleNorth;
}

double ProjectionMollweide::rScaleAlongParallel(const PhiLam& pl) const
{
	PhiLam plE = pl; plE.Lam += 0.0000001;
  XY xy, xyE, xyN; //xyAt_pl, xyEastward, Northward;
  /// compute local radius of curvature of meridian  (Snyder 4 - 22):
  double rho = (1- ell.e2)/pow((1-ell.e2*sin(pl.Phi)*sin(pl.Phi)),1.5);
  /// compute local radius of curvature in prime meridian plane  (Snyder 4 - 23):
  double nu = 1/sqrt(1-ell.e2*sin(pl.Phi)*sin(pl.Phi));
  double ph, th, dth, thE;
  th = pl.Phi;
  thE = plE.Phi;
	if (fTrulyEllipsoid) {/* ellipsoid */
		double qs = qsfn(pl.Phi);
		double qsoverqp = qs/qp;
		th = asin(qsfn(pl.Phi) / qp);//authalic lat 
		double thDegrees = th*180.0/M_PI;
		// equating Frustum (fr 0 to Phi) and ellips-strip (fr 0 to th)
		double theta;
		double t2 = ell.e * sin(pl.Phi); // t2 = var in Frustum integral
		double h = 2*t2/(1-t2*t2) + log((1+t2)/(1-t2));
		double rFrust = c * h; // 
		theta = pl.Phi; //first guess theta for Newton-iteration
		ph = rFrust / p2 ;
		double phCheck = M_PI * sin(pl.Phi);//spheric case
		for (int j = NITER; j ; --j) {
			dth = (ph - theta - sin(theta)) / (1. + cos(theta));
			theta += dth;
			if (abs(dth) < EPS)
				break;
		}
		theta *= .5;
		xy.x = sqrt(p2)* pl.Lam / M_PI_2 * cos(theta) / ell.a;
		xy.y = sqrt(p2)* sin(theta) / ell.a;
		// forward projection of plE
		double qsE = qsfn(plE.Phi);
		double qsoverqpE = qsE/qp;
		thE = asin(qsfn(plE.Phi) / qp);//authalic lat 
		t2 = ell.e * sin(plE.Phi); // t2 = var in Frustum integral
		h = 2*t2/(1-t2*t2) + log((1+t2)/(1-t2));
		rFrust = c * h; // 
		theta = pl.Phi; //first guess theta for Newton-iteration
		ph = rFrust / p2 ;
		for (int j = NITER; j ; --j) {
			dth = (ph - theta - sin(theta)) / (1. + cos(theta));
			theta += dth;
			if (abs(dth) < EPS)
				break;
		}
		theta *= .5;
		xyE.x = sqrt(p2)* plE.Lam / M_PI_2 * cos(theta) / ell.a;
		xyE.y = sqrt(p2)* sin(theta) / ell.a;
	}
  else  {	/* sphere */
	  ph = M_PI * sin(th);
	
	  for (int i = NITER; i ; --i) {
		dth = (ph - th - sin(th)) / (1. + cos(th));
		th += dth;
		if (abs(dth) < EPS)
			break;
	  }
    th *= .5;
	
    xy.x = C1R * pl.Lam * cos(th);
    xy.y = C2R * sin(th);
		// forward projection of plE
		double phE = M_PI * sin(thE);
		for (int i = NITER; i ; --i) {
			dth = (phE - thE - sin(thE)) / (1. + cos(thE));
			thE += dth;
			if (abs(dth) < EPS)
				break;
		}
    thE *= .5;
		xyE.x = C1R * plE.Lam * cos(thE);
    xyE.y = C2R * sin(thE);
	}
	double rScaleEast = (xyE.x - xy.x) / (plE.Lam - pl.Lam) / cos(pl.Phi) / nu;
	return rScaleEast;
}

double ProjectionMollweide::rLatitudeWithTrueScale() const
{
	int j = 0;
	PhiLam plLL, plLLnext; // 4 corners of moving quadrangle
		plLL.Lam = 0.0;
	double phiStep = EPS10;
	if (fTrulyEllipsoid) { 
		plLL.Phi = 0.71519;//start from ~40.98 deg (close to WGS join)  down
	}
	else	{
		plLL.Phi = 0.71099;//start from ~40.76 (close to auth sphe) down
	}
	
	///double rScaleAloPar = rScaleAlongParallel(plLL);//initial scale
	///double rScaleAloMer = rScaleAlongMeridian(plLL);//initial scale
	while (j<1000000) {
		plLLnext.Phi = plLL.Phi - phiStep;
		plLLnext.Lam = 0.0;		
		if (abs(rScaleAlongParallel(plLLnext)) < 1) break;
		plLL = plLLnext;
		++j;
	}
	///rScaleAloPar = rScaleAlongParallel(plLL);//final scale
	///rScaleAloMer = rScaleAlongMeridian(plLL);//final scale
	///double h_Times_k = rScaleAloPar*rScaleAloMer; // area scale
	return plLL.Phi;
}

