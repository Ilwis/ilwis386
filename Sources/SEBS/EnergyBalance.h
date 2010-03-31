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
// EnergyBalance.h: interface for the EnergyBalance class.
//
//////////////////////////////////////////////////////////////////////
/* SEBS model
   August 2007,by Lichun Wang
*/
#ifndef ILWMAPSEBSMODELEnergyBalance_H
#define ILWMAPSEBSMODELEnergyBalance_H

#include "Engine\Applications\MAPFMAP.H"

#define M_PI 3.14159265358979323846
#define gamma 67. // ; psychometric constant (Pa K-1)
#define Rv 461.05 // ; specific gas constant water vapour
#define Rd 287.04 // ; specific gas constant dry air (J
#define Cp 1005. // ; specific heat (J kg-1 K-1)
#define P0 100000. // ; sea-level pressure (Pa)
#define g 9.81 // ; accelation due to gravity, (M S-2)
#define k 0.40 // ; von Karman's constant
#define sigma 5.678E-8 // ; Stefan-Boltzmann's constant
#define L_e 2.430e+06 // ; Latent heat of vaporazation (J

class EnergyBalance 
{

public:
	EnergyBalance();
	void calculate(double Zref, double hi, double d0, double z0m,
			double z0h, double fc, double Uref, double Tref, double Pref,
			double qaref, double Tsk, double Ps, double SWd, double LWd,
			double albedo, double emsi);
	double getUstar();
	double getH();
	double getLE();
	double getG0();
	double getRn();
	double getH_dry();
	double getH_wet();
	double getH_i();
	double getXevap();
	double getEvap_fr();
	double getT_re_wet();
	double getT_re_i();
	double getT_T0ta();
	double getT_rhoa();
	double getT_rhoacp();
	double getT_C_i();

private:
	// output variables
	double ustar;
	double H;
	double LE;
	double Rn;
	double G0;
	double H_dry;
	double H_wet;
	double H_i;
	double Xevap;
	double evap_fr;
	double t_re_wet;
	double t_re_i; 
	double t_T0ta;
	double t_rhoa;
	double t_rhoacp;
    double t_C_i;


	
	double Bw(double hi, double d0, double L, double z0);
	double Cw(double hi, double d0, double L, double z0, double z0h);
	double PSIh_y(double Y);
	double PSIm_y (double Y);
};


#endif // ILWMAPSEBSMODELEnergyBalance_H
