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

 Created on: 2007-28-8
 ***************************************************************/
// kb_1.cpp: implementation of the kb_1 class.
//
//////////////////////////////////////////////////////////////////////
/* SEBS model
   August 2007, by Lichun Wang
*/

#include "Engine\Domain\Dmvalue.h"
#include "SEBS\kb_1.h"

	kb_1::kb_1(double zref,	double C_d, double Ct):
	m_C_d(C_d),
	m_Ct(Ct),
	m_zref(zref)
	{
	}
	
	double kb_1::nu(double m_Ta, double m_pa) {

		//  nu: Kinematic viscosity of air, (m^2/s)
		//  Ta: ambient temperature of the air, (0C)
		//  pa: ambient pressure of the air, (0C)
		// 
		//  The values are taken from Brutsaert, 1982, p.46, Tab. 3.6
		//  which are good for Ta (0C) [-20,40]
		//  and pressure at 1 atm. or 101325 Pa.
		//  More exact formula may be used when available.
		//
		//  Tb = [-20.,-10.,0.,10.,20.,30.,40]
		//  nub = [1.158,1.243,1.328,1.418,1.509,1.602,1.700]
		//  coef = POLY_FIT(Tb,nub,2) ;fit a 2nd order polynormal function
		//  nu = coef(0)+coef(1)*Ta+coef(2)*Ta^2. 
		//  The fitting is better than 0.06%

		//  constants for Massman 1999 formula for nu.
		//  This formula gives exactly the same values for 1 atm. as
		//  Brutsaert's table.

		double p0 = 101325.; // Pa
		double T0 = 273.15; // K
		double T = m_Ta + T0; // K

		double nuval = 1.327 * (p0 / m_pa) * pow((T / T0), 1.81); 
		// (10 cm^2/s) This is from Massman.
		return nuval * 1.E-5; // The last factor should be there!
	}

	void kb_1::calculate(double	Wfol, double LAI, double z0, double h, double d0,
						bool useLanduse,bool useHc,bool useD0,
						double u_ref, double p_ref, double t_ref) {

		//  KB_1M, Massman, 1999, (24)
		//  This a surrogate for the full LNF model for describing the combined
		//  canopy and soil boundary layer effects on kBH_1
		//  The formulation follows Su (2001), HESS
		//  u_zref: wind speed at reference height
		//  zref, reference height, 2h <= zref <= hst=(0.1 ~0.15)hi, (=za)
		//  u_h: wind speed at canopy height
		//  h: canopy height
		//  LAI: canopy total leaf area index
		//  C_d: foliage drag coefficient, = 0.2
		//  Ct: heat transfer coefficient
		//  !lambda: light extinction coefficient, 0.<=lambda<=1.0, default=0.5
		//  !OMEGA: foliage cluster factor, 0.<=OMEGA<=1.0, default=1.0 for no
		//  cluster
		//  Wfol: Fractional canopy cover (-)
		//  Ta: ambient air temperature (0C)
		//  pa: ambient air pressure (Pa)

		// ; Define some constants
		double k = 0.4; // Von Karman constant
		double Pr = 0.7; // Prandtl number

		// ; nu ; kinematic viscosity (M^2/s), Brutsaert 1982 should have this
		// formula.

		double hs = 0.009; // 0.009 ~ 0.024, height of soil roughness obstacles
		// Su et al., 1997, IJRS, p.2105-2124.

		double Wsoil = 1. - Wfol; // Wfol is given as an input here.
		//if (Wfol == 0.0)
		//h = hs; // Use smooth bare soil roughness

		double ust2u_h = ustar2u_h(LAI);
		double n_hvalue = n_h(ust2u_h, LAI);
		double d2hval = d2h(ust2u_h, n_hvalue);
		if (!useHc && useLanduse)
			h = z0/0.136; //(0.136*0.333);
		m_hc = h;
		if (!useLanduse){
			z0 = z02h(ust2u_h, d2hval) * h; //0.136;  
			if (Wfol <= 0)
				z0=0.005;
		}
		m_z0 = z0;
		
		if (useD0)
			m_d = d0;
		else
			m_d = d2hval * h; //d2h(ust2u_h) * h;
		double u_h0 = u_h(h,u_ref);
		if (u_h0 < 0)
			u_h0 = 0;
		double ustarh = ust2u_h * u_h0;
		double nu0 = nu(t_ref,p_ref);
		
		// ; 1st term
		double kB_1CM;
		if (n_hvalue != 0)
			kB_1CM = k * m_C_d
					/ (4 * m_Ct * ust2u_h * (1 - exp(-n_hvalue / 2.))); // (22)
		
		// Choudhury and Monteith, 1988
		else
			kB_1CM = 0.0;

		// ; 2nd term
		double Rehstarh = 0.0; 
		if (nu0 != 0)
			Rehstarh = ustarh * h / nu0; // h is canopy height, Reynolds
		// number, = hu*/v
		double Ctstar = pow(Pr, (-2. / 3.))
				* pow(Rehstarh, (-1. / 2.));

		// 3rd term
		// double ustars = u_zref * k / log(m_zref / hs); // hs is the soil
		double ustars = u_ref * k / log(m_zref / hs); // hs is the soil
		// roughness
		double Restars = 0.0;   
		if (nu0 != 0 )
			Restars = ustars * hs / nu0;
		double kB_1B82 = 2.46 * pow(Restars, (1. / 4.)) - log(7.4); // (Brutsaert,1982)

		// kB_1M = Ak1*(k*C_d/Ct)*Wfol + Ak2
		// *(k*ustar2u_h(h)*z02h(h)/Ctstar)*Wsoil $
		//  - Ak3*k/ustar2u_h(h)
		//
		// We replace the above expression by Brutsaert, 1982.
		//
		// ckB_1M = kB_1CM*Wfol^2. + (k*ust2u_h * z0/h /
		// Ctstar)*Wfol^2.*Wsoil^2. $
		//  +kB_1B82*Wsoil^2.
		//
		//  The weight of the second term is changed!
		double ckB_1M = 0.0;
		//if (Ctstar != 0)
		ckB_1M = kB_1CM * pow(Wfol, 2.)
				+ (k * ust2u_h * z0 / h / Ctstar) * Wfol * Wsoil * 2. + kB_1B82
				* pow(Wsoil, 2.);
		m_z0h = z0 / exp(ckB_1M);
	}

	double kb_1::u_h(double h, double u_zref) {
		// ; This function returns the wind speed, u_h, at canopy height h,
		// given wind speed u(zref) at reference height, zref > 2h
		// Surface layer similarity theory is invoked.

		// For the time being, we derive u(h) from u(zref), by assuming
		// neutral condition and by taking simple values for d and z0.
		// This must be improved, of course in due course.
		//
		// The following formulas are from Brutsaert, 1982, Chap. 4.
		// and also used in Bluemel, 1999.

		//double d = 0.667*h; //(d=h*2/3)
		//double z0 = 0.136 * h; 
		return u_zref * (log(h - m_d) - log(m_z0))
				/ (log(m_zref - m_d) - log(m_z0));
	}

	double kb_1::n_h(double ustar2u_hvalue, double LAI) {
		// ; n_h: within-canopy wind speed profile extinction coefficient, (13)
		// ; Z=h: canopy height
		// ; zeta(h): cumulative leaf drag area per unit planform area

		return m_C_d * LAI / (2. * pow(ustar2u_hvalue, 2.)); // (6,7)
	}

	double kb_1::ustar2u_h(double LAI) {
		// u*/u(h), (14)
		// h: canopy height
		//
		// Model constants related to the bulk surface drag coefficient
		// (=2u*^2/u(h)^2)
		// and to the substrate or soil drag coefficient, Massman (1997)

		double c1 = 0.320;
		double c2 = 0.264;
		double c3 = 15.1;

		return c1 - c2 * exp(-c3 * m_C_d * LAI); // (6,7)

	}

	double kb_1::d2h(double ustar2u_hvalue, double n_hval) {
		// d/h
		// Z: height variable
		// h: canopy height
		//
		// d2hB = 0.667 ; This is from Brutsaert (1982)
		return 1. - 1. / (2. * n_hval) * (1. - exp(-2 * n_hval)); 
		// This is from Massman (1997)

	}

	double kb_1::z02h(double ustar2u_hvalue, double d2hval) {

		// z0/h, (16), Su et al. (2001), Eq.10
		// z0: roughness length for momentum transfer
		// Z: height variable
		// h: canopy height
		//
		// ksi=Z/h
		// change d(ksi) to d(z)/h and [0,1] to [0,h] in the integration

		double k = 0.40; // von Karman constant

		//z02h = 0.136 ; This is from Brutsaert (1982)
		//return (1. - d2h(h, C_d, LAI)) * Math.exp(-k * Math.pow(ustar2u_h(h, C_d, LAI), (-1))); // (16)
		double powustar2u_hvalue = pow(ustar2u_hvalue, (-1.));
		double kp = -k * powustar2u_hvalue;
		double expval = exp(kp);  
		return (1. - d2hval) * expval;
	}

	double kb_1::getZ0() {
		return m_z0;
	}

	double kb_1::getD() {
		return m_d;
	}

	double kb_1::getZ0h() {
		return m_z0h;
	}
	double kb_1::getHc() {
		return m_hc;
	}


