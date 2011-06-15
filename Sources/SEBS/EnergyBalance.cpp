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

 Created on: 2007-28-8 by ITC, Lichun Wang
 ***************************************************************/
// EnergyBalance.h: interface for the EnergyBalance class.

#include "Engine\Domain\Dmvalue.h"
#include "SEBS\EnergyBalance.h"


EnergyBalance::EnergyBalance(){
}

double EnergyBalance::PSIm_y (double Y)
	{
	//	 Integrated stability function
	//	 1a. Stability correction function for momentum, eq.(16)
	//	 Y=-z/L, z is the height, L the Obukhov length, both specified in the calling statement.
	//
	//
	//	 For stable conditions
	//	 we use the expressions proposed by Beljaars and Holtslag (1991)
	//	 and evaluated by Van den Hurk and Holtslag (1995) can be used. These are given in the following:

	double PSIm_y;
	if (Y < 0.0) {

	    const double a = 1.0;
	    const double b = 0.667;
	    const double c = 5.0;
	    const double d = 0.35; //     ; constants, p. 122, vdH&H

		//According to Beljaars & Holtslag, 1991,
		//Y = (z-d0)/L, thus is positive for stable condition
		//Since we defined Y = - (z-d0)/L, we need to set the sign right here, thus
		//Ys = -Y

		double Ys = -Y;

	    PSIm_y = - (a * Ys + b*(Ys - c/d)*exp(-d*Ys)+b*c/d); // eq. 13


	} else {
		//	For unstable conditions

	     const double a = 0.33;
	     const double b = 0.41; //    ; constants, p. 443

	     double PSI0 = -log(a) + pow(3,(1/2.))*b*pow(a,(1/3.))*M_PI/6.; //     ;p.443
	     double b_3 = pow(b, (-3.));
		 double Yb;	
		 if (Y <= b_3)
			 Yb = Y;	
		 else 
	         Yb = b_3;
	     double x = pow((Yb/a), (1/3.));
		 PSIm_y = log(a+Yb) - 3*b*pow(Yb,(1/3.)) + b*pow(a,(1/3.))/2.*log(pow((1+x),2.)/(1-x+pow(x,2.))) + pow(3,(1/2.))*b*pow(a,(1/3.))*atan((2*x-1)/pow(3,(1/2.))) + PSI0;

	   }
	   return PSIm_y;
	}

	
void EnergyBalance::calculate(double Zref, double hi, double d0, double z0m,
			double z0h, double fc, double Uref, double Tref, double Pref,
			double qaref, double Tsk, double Ps, double SWd, double LWd,
			double albedo, double emsi,bool use_lai,double lai) {

	//Zref - Reference height (m)
	//hi - Height of the PBL (m), if not available, then use default 1000m around noon,
	//d0 - Zero plane displacement height (m)
	//z0m - Roughness height for momentum tranfer (m)
	//z0h - Roughness height for heat tranfer (m)
	//fc - Fractional vegetaion cover (-)
	//Uref - Wind speed at reference height (m/s)
	//Tref - Air temperature at reference height (0C)
	//Pref - Pressure at reference height (Pa),
	//qaref - Specific humidity at reference height (kg/kg)
	//Tsk - Surface temperature (0K)
	//Ps - Surafce pressure (Pa)
	//SWd - Downward Solar Radiation (Watt/m^2)
	//LWd - Downward long wave radiation (Watt/m^2)",
	//albedo - Albedo (-)
	//emsi - Emissivity of the surface (-)
	
	// Convert to proper units
	double Trefk = Tref + 273.15; // Air temperature at reference height (K)
	double Theta_a = Trefk / pow((Pref / P0), .286); 
	double Theta_v = Tsk * (1 + 0.61 * qaref); // virtual surface temperature   
	// Potential air temperature at reference height (K)
	double Theta_s = Tsk / pow((Ps / P0), .286); // potential surface temperature

	// Calculate intermediate results

	// Some formulas are from Campbell & Norman, 1998.
	double eact = Pref * qaref * (Rv / Rd); // actual vapour pressure
	double T_v = Tsk * (1 + 0.61 * qaref); // ; surface virtual temperature
	double rhoa = Ps / (Rd * T_v); // surface air density (kg/m3)
	double rhoam = Ps / (Rd * Tsk * (1 - 0.378 * eact / Ps)); 
	// moist air density (kg / m3), this the same as rhoa
	double rhoacp = rhoa * Cp; // ; specif air heat capacity (J K-1 m-3)
	t_rhoa = rhoa;
	t_rhoacp = rhoacp; 

	// Here, we use the average PBL and Surface Temperature
	// T_0pbl = 0.5*(Tsk + Trefk) ; mean temperature
	double T_0pbl = Tsk; // mean temperature
	// T_0pbl = Trefk ; air temperature
	double Tcn = T_0pbl - 273.15; // ; mean temperature converted to 0C.
	double Acn = 611.; // ; Pa
	double Bcn = 17.502;
	double Ccn = 240.97; // ; 0C
	double esat = Acn * exp(Bcn * Tcn / (Tcn + Ccn)); // ;Pa,(3.8),p.41
	double slope = Bcn * Ccn * esat / pow((Ccn + Tcn), 2.); // ;Pa*0C-1,(3.9)
	// double deltae = esat - eact;

	
	double sigma2 = 5.678E-8; // Stefan-Boltzmann's constant

	// Net Radiation
	if (albedo == rUNDEF || SWd==rUNDEF || emsi==rUNDEF || LWd==rUNDEF || Tsk ==rUNDEF){
		Rn = rUNDEF;
		G0 = rUNDEF;
	}
	else{
		Rn = (1.0 - albedo) * SWd + emsi * LWd - emsi * sigma2* pow(Tsk, 4.);
		if (use_lai)
			G0 = Rn*0.34*exp(-0.46*lai);
		else
		    G0 = Rn * (0.05 + (1 - fc) * (0.315 - 0.05)); // ; This is good for COTTON!
	}
	
	// We assume G0 = 0.3*Rn for bare soil (Kustas et al., 1989) and
	// 0.05*Rn for full vegetation canopy

	// Stability parameters

	// We will determine the Monin-Obukhov length using the definition
	// given by Brutsaert (1982), P.65, eqn. 5.25.
	// i.e. L = - ustar^3*rhoa/(k*g*((H/Ta*Cp)+0.61*E))
	// By using the quantity, ef=Le*E/(Rn-G0), we can write
	// H = (1-ef)*(Rn-G0) and E = ef/Le*(Rn-G0)
	// So that L =-ustar^3*rhoam/(k*g*((1-ef)/(Ta*CP)+0.61*ef/Le)*(Rn-G0))
	// From this eqn., it is obvious that 
	// L=f(ustar^3) and L=f(ef^-1)
	//
	// For complete dry areas, we get
	//
	// L_dry = - ustar^3*rhoa/(k*g*((Rn-G0)/(T_0pbl*Cp))) ; 
	// H = Rn -G0
	// The latent heat of vaporization at 30C is
	// L_e = 2.430e+06 ; (J KG-1) taken from Brutsaert, 1982, p.41,tab. 3.4,
	// more exact values can be obtained from eqn(3.22, 3.24a,b)
	//
	// For complete wet areas, we get
	// L_wet = - ustar^3*rhoa/(k*g*(0.61*(Rn-G0)/L_e)) ; E = (Rn-G0)/Le
	//
	// Here we start to solve the system of three equations
	// i.e. the equation of momentum transfer, the equation of heat transfer
	// the equation for the stability length.
	//
	// We use ASL functions of Brutsaert, 1999, if Zref < hst, the ASL height
	// We use BAS functions of Brutsaert, 1999, if Zref > hst, Zref <= hi, the PBL height.
	// The ASL height
	// hst = alfa*hi, with alfa=0.10~0.15,
	// over moderately rough surfaces, or
	// hst=beta*z0, with beta= 100~150.

	double alfa = 0.12;
	double beta = 125.; // These are mid values as given by Brutsaert,1999
	double hst = max(alfa * hi, beta * z0m);
			
	// Initial guess for u*, H and L assuming neutral stability
	// Iterations to replace the Broyden method
	double zd0 = Zref - d0;
	double ku = k * Uref;  
	double zdm = log(zd0 / z0m);
	double zdh = log(zd0 / z0h);
	double T0ta = Theta_s - Theta_a; // Surface, PBL potential Tsk - Trefk; 
	t_T0ta =T0ta;
	double CH = T0ta * k * rhoacp;
	double CL = -rhoam *Cp * Theta_v / (k * g);
	double RUstar = ku / zdm;
	double RH = CH * RUstar / zdh;
	double RH0 = RH;
	double Reps = 10.0;
	int steps = 0;
	double RL=0;
	if (Zref >= hst){
		while(Reps > 0.01 && steps < 100){
			RL = CL * pow(RUstar, 3) / RH;
			RUstar = ku / (zdm - Bw(hi,d0,RL, z0m));
			RH = CH * RUstar / (zdh - Cw(hi,d0,RL,z0m,z0h));
	        Reps   = abs(RH0 - RH); 
	        RH0=RH;
	        steps++;
		}
	}	
	else{
		while (Reps > 0.01 && steps < 100){
			RL     = CL * pow(RUstar, 3) / RH;
			RUstar = ku / (zdm - PSIm_y(-zd0/RL) + PSIm_y(-z0m/RL));
			RH     = CH * RUstar / (zdh - PSIh_y(-zd0/RL) + PSIh_y(-z0h/RL));
			Reps   = abs(RH0 - RH);
			RH0=RH;
			steps++;
		}
	}
			
	ustar = RUstar;
	H = RH;
	double L = RL;
			
	// Stability parameters
	//
	// For complete dry areas, we get

	double L_dry = -pow(ustar, 3) * rhoa
		 / (k * g * ((Rn - G0) / (T_0pbl * Cp))); // H = Rn -G0

	// The latent heat of vaporization at 30C is
	// L_e = 2.430e+06 ; (J KG-1) taken from Brutsaert, 1982, p.41,tab. 3.4,
	// more exact values can be obtained from eqn(3.22, 3.24a,b)
	// For complete wet areas, we get

	double L_wet = -pow(ustar, 3) * rhoam
			/ (k * g * (0.61 * (Rn - G0) / L_e)); // E = (Rn -G0)/Le

	// A good idea could be to take L_dry and L_wet respectively as
	// the max and min stability length.
	// This has the advantage that the feedback effects between land
	// surface and the PBL are considered.
	// For theoretical limits, H=Rn-G0, and E=Rn-G0 respectively.
	// (Note: For wet surfaces in a dry climate, E may be bigger than
	// Rn-G0 due to negative H,
	// i.e. the Oasis effect. In this case, a small modification of
	// L_wet can be expected. This is ignored for the time being)
	// Previously, we interpolated mu_i between mu_i(0) and
	// mu_i(-1500), by means of temperature difference
	// Though other factors may aslo have influences as seen in the
	// calculation of T0Ta_l and T0Ta_u,
	// the uncertainties associated to those factors do not warrant
	// their adequate applications for this case.
	// This is consistant with the definition of resistences for the
	// limiting cases, that is, that we ignore the stable case
	// over the whole region.
	//
	// ASL or Mixed layer

	double C_wet;
	if (Zref < hst)
		C_wet = PSIh_y(-Zref/L_wet);
	else
		C_wet = Cw(Zref, d0,L_wet, z0m, z0h);

	// EB resistances at limiting cases
	double re_wet = (log((Zref - d0) / z0h) - C_wet) / (k * ustar);

	if (re_wet <= 0.)
		re_wet = log((Zref - d0) / z0h) / (k * ustar);
	t_re_wet = re_wet;
	// Here we utilise the energy balance consideration at limiting cases

	if (Rn == rUNDEF || G0 == rUNDEF){
		H_dry = rUNDEF;
		H_wet = rUNDEF;
	}
	else {
		H_dry = Rn - G0;
		H_wet = ((Rn - G0) - (rhoacp / re_wet) * esat / gamma)
			/ (1.0 + slope / gamma);
	}	
	// Here we sssume that the overlaying air is COMEPLETELY DRY!
	
	// calculate evaporative fraction image

	//double T0ta = Theta_s - Theta_a; // Surface, PBL potential
	// temperature difference

	// Stability corrections
	double C_i;
	if (Zref < hst)
		C_i = PSIh_y(-(Zref-d0)/L) - PSIh_y(-z0h/L);
	else
		C_i = Cw(Zref, d0,L, z0m, z0h);
	t_C_i = C_i;

	// Actual resistance to heat transfer
	double re_i = (log((Zref - d0) / z0h) - C_i) / (k * ustar);
	if (re_i <= 0.)
		re_i = log((Zref - d0) / z0h) / (k * ustar);
	t_re_i = re_i;

	// Sensible heat flux

	H_i = rhoacp * T0ta / re_i;
	if (H_i > H_dry && H_dry != rUNDEF)
		H_i = H_dry; // H_i clearly can not be bigger than H_dry
	if (H_i < H_wet && H_wet != rUNDEF)
		H_i = H_wet; // H_i clearly can not be smaller than H_wet

	// This form is derived from energy balance considerations
	// The relative evaporation is given as evap_re = LE/LE_wet

	double evap_re;
	if (H_dry == rUNDEF || H_wet == rUNDEF)
		evap_re = rUNDEF;
	else if (H_dry <= H_wet)
		evap_re = 1.0; // Water & wet surfaces
	else
		evap_re = 1. - (H_i - H_wet) / (H_dry - H_wet);
	
	// evap_re1 = (H_dry - H_i)/(H_dry - H_wet) ; this is the same as above.
	// The evporative fraction ef=
	//evap_fr = evap_re * (1.0 - H_wet / H_dry);
	// LE/(H+LE)=LE/(Rn-G0)=evap_re*LE_wet/(Rn-G0)

	if (evap_re == rUNDEF || Rn == rUNDEF || G0 == rUNDEF)
		evap_fr = rUNDEF;
	else if ((Rn - G0) != 0)
		evap_fr = evap_re * (Rn - G0 - H_wet) / (Rn - G0);
	else
		evap_fr = 1.; // for negative available energy
	if (evap_fr > 10)
		evap_fr = 10;
	LE = evap_fr * (Rn - G0);
	Xevap = evap_re;
}
	
double EnergyBalance::Bw(double hi, double d0,double L, double z0) {
		// Bulk Stability function for momentum, eq.(22), (26)
		// hi: Height of ABL or PBL
		// L: The Obukhov length
		// z0: Surface roughnes height for momentum
		//
		// The ASL height
		// hst = alfa*hi, with alfa=0.10~0.15,
		// over moderately rough surfaces, or
		// hst=beta*z0, with beta= 100~150.
		//
		// Typical values:
		// The equations describe the Free convective conditions in the mixed layer,
		// provided the top of the ABL satisfies the condition -hst > 2L.
		// 
		// For a surface with moderate roughness and hi=1000m, alfa=0.12, this
		// is -hi/L > 17 and -L <60 (and similar values over very rough terrain).

		const double alfa = 0.12;
		const double beta = 125.; // ; These are mid values as given by Brutsaert, 1999

		double B0 = (alfa / beta) * hi;
		double B1 = -z0 / L;
		double B11 = -alfa * (hi-d0) / L;
		double B21 = hi / (beta * z0);
		double B22 = -beta * z0 / L;

		// NOTE: The minus (-) sign infront of B1, B11, B22 are necessary,though not
		// clearly specified by Brutsaert, 1999. This is consistent with the integral
		// form given in (16)&(17) in which y, the variable is defined as -z/L.
		// (z0 LT (alfa/beta)*hi): Bw = -ALOG(alfa) + PSIm_y(alfa*hi/L) -
		// PSIm_y(z0/L) ;(22)
		// (z0 GE (alfa/beta)*hi): Bw = ALOG(hi/(beta*z0)) + PSIm_y(beta*z0/L)
		// - PSIm_y(z0/L) ;(26)

		// For stable conditions

		double Bw;
		if (B1 < 0.) {
			Bw = -2.2 * log(1 + (-B1)); // Brutsaert, 1982, Eq. 4.93, p.84
		} else {

			// For unstable conditions

			if (z0 < B0)
				Bw = -log(alfa) + PSIm_y(B11) - PSIm_y(B1);
			else
				Bw = log(B21) + PSIm_y(B22) - PSIm_y(B1);

			if (Bw < 0.)
				Bw = 0.; // This results from unfortunate parameter combination!

		}

		return Bw;
}


double EnergyBalance::Cw(double hi, double d0,double L, double z0, double z0h) {
		// Bulk Stability function for heat tranfer, eq.(23), (27)
		// hi: Height of ABL or PBL
		// L: The Obukhov length
		// z0: Surface roughnes height for momentum
		// z0h: Surface roughnes height for height transfer
		//
		// ;
		// ; The ASL height
		// ; hst = alfa*hi, with alfa=0.10~0.15,
		// ; over moderately rough surfaces, or
		// ; hst=beta*z0, with beta= 100~150.

		double alfa = 0.12;
		double beta = 125.; // These are mid values as given by Brutsaert, 1999

		double C0 = (alfa / beta) * hi;
		double C1 = -z0h / L;
		double C11 = -alfa * (hi-d0) / L;
		double C21 = hi / (beta * z0);
		double C22 = -beta * z0 / L;

		// NOTE: Similar to Bw, The minus (-) sign infront of C1, C11, C22 are necessary, though not
		// clearly specified by Brutsaert, 1999. This is consistent with the integral
		// form given in (16)&(17) in which y, the variable is defined as -z/L.
		// (z0 LT (alfa/beta)*hi): Cw = -ALOG(alfa) + PSIh_y(alfa*hi/L) -
		// PSIh_y(z0h/L) ;(22)
		// (z0 GE (alfa/beta)*hi): Cw = ALOG(hi/(beta*z0)) + PSIh_y(beta*z0/L)
		// - PSIh_y(z0h/L) ;(26)

		// For stable conditions

		double Cw;
		if (C1 < 0.)
			Cw = -7.6 * log(1 + (-C1)); // Brutsaert, 1982, Eq. 4.93,p.84
		else {

			// ; For unstable conditions

			if (z0 < C0)
				Cw = -log(alfa) + PSIh_y(C11) - PSIh_y(C1);
			else
				Cw = log(C21) + PSIh_y(C22) - PSIh_y(C1);

			if (Cw < 0.)
				Cw = 0.; // This results from unforunate parameter combination!

		}

		return Cw;
}
	
double EnergyBalance::PSIh_y(double Y) {
		// Integrated stability function 1b.
		// Stability correction function for heat, eq.(17)
		// Y=-z/L, z is the height, L the Obukhov length, both specified in
		// the calling statement.
		//
		// For stable conditions
		// we use the expressions proposed by Beljaars and Holtslag (1991)
		// and evaluated by Van den Hurk and Holtslag (1995) can be used.
		// These are given in the following:

		double PSIh_y;
		if (Y < 0.0) {

			double a = 1.0;
			double b = 0.667;
			double c = 5.0;
			double d = 0.35; // constants, p. 122, vdH&H

			// According to Beljaars & Holtslag, 1991,
			// Y = (z-d0)/L, thus is positive for stable condition
			// Since we defined Y = - (z-d0)/L, we need to set the sign right here, thus
			// Ys = -Y
			double Ys = -Y;

			PSIh_y = -(pow((1 + 2 * a / 3 * Ys), 1.5) + b * (Ys - c / d)
					* exp(-d * Ys) + (b * c / d - 1)); // eq. 13

		} else {

			// ; For unstable conditions
			// ; Y = ABS(Y)

			double c = 0.33;
			double d = 0.057;
			double n = 0.78; // constants, p. 443, Brutsaert, 1999

			PSIh_y = (1 - d) / n * log((c + pow(Y, n)) / c);
		}

		return PSIh_y;
}
	
	double EnergyBalance::getUstar() {
		return ustar;
	}

	double EnergyBalance::getH() {
		return H;
	}

	double EnergyBalance::getLE() {
		return LE;
	}

	double EnergyBalance::getG0() {
		return G0;
	}

	double EnergyBalance::getRn() {
		return Rn;
	}

	double EnergyBalance::getH_dry() {
		return H_dry;
	}

	double EnergyBalance::getH_wet() {
		return H_wet;
	}

	double EnergyBalance::getH_i() {
		return H_i;
	}

	double EnergyBalance::getXevap() {
		return Xevap;
	}

	double EnergyBalance::getEvap_fr() {
		return evap_fr;
	}
	double EnergyBalance::getT_re_wet() {
		return t_re_wet;
	}
	double EnergyBalance::getT_re_i() {
		return t_re_i;
	}
	double EnergyBalance::getT_T0ta() {
		return t_T0ta;
	}
	double EnergyBalance::getT_rhoa() {
		return t_rhoa;
	}
	double EnergyBalance::getT_rhoacp() {
		return t_rhoacp;
	}
	double EnergyBalance::getT_C_i() {
		return t_C_i;
	}
