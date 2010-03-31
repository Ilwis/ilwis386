/* ProjectionStereoPolar  Polar Stereographic
   Copyright Ilwis System Development ITC,
   J Hendrikse and I Holleman KNMI, Netherlands
	Last change:  JHE   3 Oct 07    3:03 pm
*/

#include "Engine\SpatialReference\StereoPolar.h"
#include "Engine\Base\DataObjects\err.h"
#define  EPS10 1.e-10

ProjectionStereoPolar::ProjectionStereoPolar(const Ellipsoid& ell)
: ProjectionStereoGraphic(ell)
{
  fUseParam[pvX0] = false;
  fUseParam[pvY0] = false;
  fUseParam[pvLON0] = true;
  fUseParam[pvLATTS] = true;
  fUseParam[pvK0] = false;
  fUseParam[pvNORTH] = true;
	///fUseParam[pvK0] = true;
  ///k0 = 0.994;
  x0 = 0.0;
  y0 = 0.0;
  //Param(pvZONE, 1);
}

void ProjectionStereoPolar::Param(ProjectionParamValue pv, long iValue)
{
  ProjectionPtr::Param(pv, iValue);
  if (pvNORTH == pv) {
    if (fNorth)
      phi0 = M_PI_2;
    else
      phi0 = -M_PI_2;
  }
  double e = ell.e;
  //The ellipsoide scale factor G(Bw) is calculated from: 
	g_Bw = (1-e)*(1+e*sin(phits)) / ( (1+e)*(1-e*sin(phits)) ); 
	G_Bw = pow(g_Bw,(e/2)) * sqrt( (1-(e*sin(phits))*(e*sin(phits))) / (1-ell.e2) ); 
 double test2_tsfn = tsfn(phits);
	// The scaling factor Z_BW) is now equal to:
	if (abs (abs(phits) - M_PI_2) < EPS10)
		Z_Bw = 1;
	else
		Z_Bw = 2 * tan(M_PI_4 - phits/2) * G_Bw / cos(phits) ;
}

XY ProjectionStereoPolar::xyConv(const PhiLam& _pl) const
{
  PhiLam pl = _pl;   // local copy of input _pl
  XY xy;
  double e = ell.e;

	// The ellipsoide factor F(B) is given by:
	double f_B = (1-e)*(1+e*sin(pl.Phi)) / ( (1+e)*(1-e*sin(pl.Phi)) ) ;
	double F_B = pow(f_B,(e/2)) / sqrt(1-ell.e2); 
	double test1_tsfn = tsfn(pl.Phi);
// The projection distance as a function of latitude R(B) is now given by:
	// at unit ellipsoid scale !!
	double R_B = 2 * tan(M_PI_4 - pl.Phi/2) * F_B;

	//	The projection distance is scaled such that 
//	the magnification of the projection is 1 at the latitude 
//	of true scale (Bw). 'Breedte op Ware schaal'

	// The Cartesian image coordinates are calculated 
	// in a reference frame with the y-axis pointing north, 
	// parallel to the reference meridian and a perpendiculr x-axis. 
	// The Cartesian coordinates X (distance to reference meridian) 
	// and Y (distance to northpole along reference meridian) are:

	double X_L_B = R_B * sin(pl.Lam - lam0) / Z_Bw ;
	double Y_L_B = -R_B * cos(pl.Lam - lam0) / Z_Bw ;
	xy.x = X_L_B;
	xy.y = Y_L_B;
	return xy;
}

PhiLam ProjectionStereoPolar::plConv(const XY& _xy) const
{
  XY xy = _xy;
  PhiLam pl;
  double e = ell.e;
	double test3_tsfn = tsfn(phits);
	// The distance from the projection origin (northpole) R(I,J) and the longitude L(I,J) are found using:
	double R_I_J = Z_Bw * sqrt( xy.x * xy.x + xy.y * xy.y ); 
	double L_I_J = lam0 + atan2( xy.x , -xy.y );
	
	//The latitude can be calculated from the projection distance 
	//by a two-step iteration. 
	//During the first step, the ellipsoide part of 
	//projection distance F(B) is neglected. 

	//Using the first estimate of the latitude B1, 
	//the ellipsoide correction factor can be estimated. 
	//Subsequently, an improved estimate of the latitude can be obtained.
	//B1 = M_PI_2 - 2 * atan( R_I_J / (2*ell.a) ) ;
	
	//B = M_PI_2 - 2* atan( R_I_J / (2*ell.a*F_B1) );
	
	double rPhiApprox = M_PI_2 - 2 * atan( R_I_J /2 ) ;
	double f_B = (1-e)*(1+e*sin(rPhiApprox)) / ( (1+e)*(1-e*sin(rPhiApprox)) ) ;
	double F_B = pow(f_B,(e/2)) / sqrt(1-ell.e2); 
	
	double rPhiNext = M_PI_2 - 2* atan( R_I_J / (2*F_B) );
	int iCount = 0;
	while (abs(rPhiNext - rPhiApprox) > EPS10) {
	  rPhiApprox = rPhiNext;
		f_B = (1-e)*(1+e*sin(rPhiApprox)) / ( (1+e)*(1-e*sin(rPhiApprox)) ) ;
		F_B = pow(f_B,(e/2)) / sqrt(1-ell.e2); 
	  rPhiNext = M_PI_2 - 2* atan( R_I_J / (2*F_B) );
	  iCount++;
	  if (iCount > 10) return pl;
	}
	pl.Phi = rPhiNext;	// (geodetic) latitude on ellipsoid (approximated)
	pl.Lam = L_I_J;
	return pl;
}