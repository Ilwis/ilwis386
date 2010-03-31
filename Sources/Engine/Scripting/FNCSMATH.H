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
// $Log: FNCSMATH.H $
// Revision 1.5  1998/09/16 17:30:36  Wim
// 22beta2
//
// Revision 1.4  1997/07/30 08:36:32  Wim
// Syntax error
//
// Revision 1.3  1997-07-30 10:34:59+02  Wim
// raddeg now adds 360 when result of fmod is smaller than 0
//
// Revision 1.2  1997-07-25 16:37:00+02  Wim
// sinhx() should call sinh(), and coshx() should call cosh()
// were calling sin() and cos() resp. (bug 2169)
//
/* fncsmath.h
	Last change:  WK   30 Jul 97   10:36 am
*/

extern void rnd0_so(StackObject* soRes);

#define defextmath1fnc(funcname) \
extern void funcname ##_so (StackObject* soRes, const StackObject* so);

defextmath1fnc(absx)
defextmath1fnc(neg)
defextmath1fnc(floor)
defextmath1fnc(ceil)
defextmath1fnc(roundy)
defextmath1fnc(randx)
defextmath1fnc(sin)
defextmath1fnc(cos)
defextmath1fnc(tan)
defextmath1fnc(asinx)
defextmath1fnc(acosx)
defextmath1fnc(atan)
defextmath1fnc(sinhx)
defextmath1fnc(coshx)
defextmath1fnc(tanh)
defextmath1fnc(sqrt1)
defextmath1fnc(logx)
defextmath1fnc(log10x)
defextmath1fnc(expx)
defextmath1fnc(degrad)
defextmath1fnc(raddeg)
defextmath1fnc(sqrx)

#define defextmath2fnc(funcname) \
extern void funcname ## 2_so (StackObject* soRes, const StackObject* so0, const StackObject* so1);

defextmath2fnc(atan2x)
defextmath2fnc(powx)
defextmath2fnc(hypo)
defextmath2fnc(sq)


inline double absx(double r)
  { return fabs(r); }

inline double neg(double r)
  { return  -r; }

inline double asinx(double r)
  { return ((r < -1) || (r > 1)) ? rUNDEF : asin(r); }

inline double randx(double r)
  { return doubleConv(random(longConv(r))); }

inline double roundy(double r)
  { return doubleConv(roundx(r)); }

inline double acosx(double r)
  { return ((r < -1) || (r > 1)) ? rUNDEF : acos(r); }

inline double sqrt1(double r)
  { return r < 0 ? rUNDEF : sqrt(r); }

inline double expx(double r)
  { return r > 700 ? rUNDEF : exp(r); }

inline double logx(double r)
  { return r <= 0 ? rUNDEF : log(r); }

inline double log10x(double r)
  { return r <= 0 ? rUNDEF : log10(r); }

inline double degrad(double r)
 { return r / (180.0 / M_PI); }

inline double raddeg(double r) {
  r = fmod(r * (180.0 / M_PI), 360);
  if (r < 0) r += 360;
  return r;
}

inline double sqrx(double r)
  { return  r * r; }

inline double sinhx(double r)
  { return (r > 700) || (r < - 700) ? rUNDEF : sinh(r); }

inline double coshx(double r)
  { return (r > 700) || (r < - 700) ? rUNDEF : cosh(r); }




