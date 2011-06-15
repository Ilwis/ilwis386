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
// kb_1.h: interface for the kb_1 class.
//
//////////////////////////////////////////////////////////////////////
/* SEBS model
   August 2007,by Lichun Wang
   Updated 2011, by Lichun Wang
*/
#ifndef ILWMAPSEBSKB1_H
#define ILWMAPSEBSKB1_H


class kb_1 
{
private:
	double nu(double,double);
	double u_h(double h,double);
	double n_h(double ustar2u_hvalue, double LAI);
	double ustar2u_h(double LAI);
	double d2h(double ustar2u_hvalue, double n_hval);
	double z02h(double ustar2u_hvalue, double d2hval);
	double m_C_d;
	//double m_Ta;
	//double m_pa;
	double m_Ct;
	//double m_u_zref;
	double m_zref;
	// output variables
	double m_z0;
	double m_d;
	double m_hc;//vegetation height
	double m_z0h;
	double m_kb;
public:
	//kb_1(double u_zref, double zref,double C_d, double Ct, double Ta, double pa);
	kb_1(double zref,double C_d, double Ct);
	void calculate(double Wfol, double LAI, double z0, double h, double d0,  
					bool useLanduse,bool useHc, bool useD0, 
					double u_ref, double p_ref, double t_ref,
					double LST_K, double qaref, bool kb_p, bool use_kb, double rKB);
	double getZ0();	
	double getD();	
	double getZ0h();
	double getHc();
	double getKB();
};

#endif // ILWMAPSEBSKB1_H
