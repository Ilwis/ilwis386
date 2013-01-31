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
// round.c
// round function
// by Wim Koolhoven
// (c) Computer Department ITC

#define ROUND_C
#include "Headers\toolspch.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#include "Engine\Base\Round.h"

double rRound(double r)
{
  if (r < 7)
    if (r < 1e-10)
      return 1e-10;
    else
      return rRound(r*10)/10;
  else if (r > 70)
    if (r > 1e30)
      return 1e30;
    else
      return rRound(r/10)*10;
  else if (r < 17)
    return 10;
  else if (r <= 25)
    return 20;
  else
    return 50;
}

double rRound60(double r)
{
	if (r < 1)
		return rRound(r);
	else if (r < 1.4)
		return 1;
	else if (r < 3.5)
		return 2;
	else if (r < 7)
		return 5;
	else if (r < 14)
		return 10;
	else if (r < 24)
		return 20;
	else if (r < 44)
		return 30;
	else 
		return 60;
}

double rRoundDMS(double rDflt)
{
	const double rSEC = 1/3600.0;
	const double rMIN = 1/60.0;
	if (rDflt < rMIN)
		return rRound60(rDflt / rSEC) * rSEC;
	else if (rDflt < 1)
		return rRound60(rDflt / rMIN) * rMIN;
	else 
		return rRound60(rDflt);
}

double niceOne(double v) {
	double rest = v - (int)v;
	if ( rest > 0) {
		if ( rest > 0.5)
			v++;
		else
			v = (int)v + 0.5;
	}
	return v;
}

RangeReal roundRange(double rmin, double rmax, double& step) {
	double tickLimits []  = {0,0.1,0.2,0.25,0.5,1.0};
	double range = rmax - rmin;
	//long d =  abs(log10(range)) + 1;
	long d =  log10(range) + 1;
	step = range / pow(10.0,abs(d));
	int i = 0;
	while(i < 6) {
		if ( step < tickLimits[i]) {
			step = tickLimits[i];
			break;
		}
		++i;
	}
	step = step * pow(10.0,d - 1);
	double lower = step * round(rmin / step);
	double intpart;
	double r = modf (rmax / step , &intpart);
	double upper = step * round(r ==0 ? intpart : 1 + intpart );
	return RangeReal(lower, upper);


}






