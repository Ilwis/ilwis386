/* ProjectionLaBelge1972
   Copyright Ilwis System Development ITC
   sep 2007, by Jan Hendrikse
	Last change:  JH   sep 20078    3:39 pm
*/

#include "Engine\SpatialReference\LaBelge72.h"
#include "Engine\Base\DataObjects\ERR.H"
#include "Headers\Hs\proj.hs"
#define EPS15 1.e-15

ProjectionLaBelge1972::ProjectionLaBelge1972(const Ellipsoid& ell)
: ProjectionLambConfConic(ell)
{
  fUseParam[pvX0] = false;
  fUseParam[pvY0] = false;
  fUseParam[pvLON0] = false;
  fUseParam[pvLAT0] = false;
  fUseParam[pvLAT1] = false;
  fUseParam[pvLAT2] = false;
  fUseParam[pvK0] = false;
  lam0 =  (4 + 22/60.0 + 2.925/3600.0)* M_PI / 180; // meridian of Brussels in radians
	phi0 = M_PI_2; //central (geodetic) latitude on ellipsoid 
	rPhi1 = (49 + 50/60 + 00.00204/3600)*M_PI/180.0;
	rPhi2 = (51 + 10/60 + 00.00204/3600)*M_PI/180.0;
  x0 = 150000.013;
  y0 = 5400088.438;
  k0 = 1.0; 
}

void ProjectionLaBelge1972::Prepare()
{
 //  first compute the constants related to the 
 // geometry of the given cone and ellipsoid
      
	double m1 = msfn(rPhi1);
  double m2 = msfn(rPhi2);
  double t1 = tsfn(rPhi1);
  double t2 = tsfn(rPhi2);

  rN = log(m1 / m2) / log(t1 / t2);  // cone constant
  rF =  m1 * pow(t1, -rN) / rN ;
  rRho0 = rF * pow(tsfn(phi0), rN);
}

XY ProjectionLaBelge1972::xyConv(const PhiLam& pl) const
 /* ellipsoid */
{
  XY xy;
  double rRho;
  if (abs(pl.Phi) >  M_PI_2 - EPS15) return xy;// point beyond pole
  if (abs(abs(pl.Phi) - M_PI_2) < EPS15) { // point near pole
    if ((pl.Phi * rN) <= 0.) return xy; 
      rRho = 0.;
  }
  else
    rRho = rF * pow(tsfn(pl.Phi), rN);
  double rTheta = pl.Lam * rN;
  xy.x =  (rRho * sin(rTheta) ) * k0;
  xy.y =  (rRho0 - rRho * cos(rTheta) ) * k0;
  return  xy;
}

PhiLam ProjectionLaBelge1972::plConv(const XY& xy) const
  /* ellipsoid */
{
  PhiLam pl;
  double rRho;
  double rY = rRho0 - xy.y / k0;
  double rX = xy.x / k0;
  rRho = sqrt(rX*rX + rY*rY);
  if (rRho >0 ) {
    if (rN < 0.) {         // Southern Hemisphere
      rRho = -rRho;
      rX = -rX;
      rY = -rY;
    }
    pl.Phi = phi2(pow(rRho / rF, 1./ rN));
    if (abs(pl.Phi) >= M_PI_2) return pl;
    pl.Lam = atan2(rX, rY) / rN;
  }
  else {      //  North or South Pole
      pl.Lam = 0.;
      pl.Phi = rN > 0. ? M_PI_2 : - M_PI_2;
  }
  return pl;
}

Datum* ProjectionLaBelge1972::datumDefault() const
{
  return new MolodenskyDatum("Belgian Datum 1972", "");
}




