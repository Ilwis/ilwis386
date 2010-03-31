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
/* $Log: /ILWIS 3.0/Calculator/FNCSMATH.cpp $
 * 
 * 3     3/03/00 12:49 Willem
 * Comments
 * 
 * 2     3/03/00 12:49 Willem
 * Adjusted the arguments for the pow() function
 */
// Revision 1.6  1998/09/16 17:30:36  Wim
// 22beta2
//
// Revision 1.5  1997/08/05 13:41:29  Wim
// Special cases of powx() treated
//
// Revision 1.4  1997-07-31 18:21:53+02  Wim
// Allow pow(negative value, integer)
//
// Revision 1.3  1997-07-28 16:58:29+02  Wim
// atan2 should only return invalid if both x AND y are 0.
//
// Revision 1.2  1997-07-28 16:25:24+02  Wim
// atan2(y,x): when x is 0 return rUNDEF
//
/*
	Last change:  WK    5 Aug 97    3:41 pm
*/
#include "Headers\base.h"
#include "Engine\Scripting\CALCSTCK.H"
#include "Engine\Scripting\Calcvar.h"
#include "Engine\Base\Algorithm\Random.h"
#include "Engine\Base\DataObjects\Buf.h"
#include "Engine\Scripting\FNCSMATH.H"

void rnd0_so(StackObject* soRes)
{
  RealBuf bufRes;
  bufRes.Size(soRes->iSize());
  for (short i=0; i<soRes->iSize(); i++)
    bufRes[i] = random();
  soRes->PutVal(bufRes);
}
/*
static double absx(double r)
  { return fabs(r); }

static double neg(double r)
  { return  -r; }

static double asinx(double r)
  { return ((r < -1) || (r > 1)) ? rUNDEF : asin(r); }

static double randx(double r)
  { return doubleConv(random(longConv(r))); }

static double roundy(double r)
  { return doubleConv(roundx(r)); }

static double acosx(double r)
  { return ((r < -1) || (r > 1)) ? rUNDEF : acos(r); }

static double sqrt1(double r)
  { return r < 0 ? rUNDEF : sqrt(r); }

static double logx(double r)
  { return r <= 0 ? rUNDEF : log(r); }

static double log10x(double r)
  { return r <= 0 ? rUNDEF : log10(r); }

static double degrad(double r)
 { return r / (180.0 / M_PI); }

static double raddeg(double r)
  { return r * (180.0 / M_PI); }

static double sqrx(double r)
  { return  r * r; }
*/

#define deffunc1(funcname)    \
static double funcname ## _ (const double& r)  \
{ try {                         \
    if (r == rUNDEF)            \
      return rUNDEF;            \
    else                        \
      return funcname(r);       \
  }                             \
  catch (ErrorObject&) {        \
    return rUNDEF;              \
  }                             \
}                               \

deffunc1(absx)
deffunc1(neg)
deffunc1(floor)
deffunc1(ceil)
deffunc1(roundy)
deffunc1(randx)
deffunc1(sin)
deffunc1(cos)
deffunc1(tan)
deffunc1(asinx)
deffunc1(acosx)
deffunc1(atan)
deffunc1(sinhx)
deffunc1(coshx)
deffunc1(tanh)
deffunc1(sqrt1)
deffunc1(logx)
deffunc1(log10x)
deffunc1(expx)
deffunc1(degrad)
deffunc1(raddeg)
deffunc1(sqrx)

#define mathfnc(funcname) \
void funcname ## _so (StackObject* soRes, const StackObject* so) \
{ RealBuf buf, bufRes;               \
  so->GetVal(buf);                   \
  bufRes.Size(soRes->iSize());       \
  for (short i=0; i<soRes->iSize(); i++) \
    if (buf[i] != rUNDEF) \
      bufRes[i] = funcname##_(buf[i]); \
    else  \
      bufRes[i] = rUNDEF; \
  soRes->PutVal(bufRes);  \
} \

mathfnc(absx)
mathfnc(neg)
mathfnc(floor)
mathfnc(ceil)
mathfnc(roundy)
mathfnc(randx)
mathfnc(sin)
mathfnc(cos)
mathfnc(tan)
mathfnc(asinx)
mathfnc(acosx)
mathfnc(atan)
mathfnc(sinhx)
mathfnc(coshx)
mathfnc(tanh)
mathfnc(sqrt1)
mathfnc(logx)
mathfnc(log10x)
mathfnc(expx)
mathfnc(degrad)
mathfnc(raddeg)
mathfnc(sqrx)

static double atan2x(const double& r1, const double& r2)
//{ return atan2(r1,r2); }
{  return (r1 == 0 && r2 == 0) ? rUNDEF : atan2(r1,r2); }

static double powx(const double& r1, const double& r2)
{
  if (r1 == 0) {
    if (r2 > 0)
      return 0;
    else
      return rUNDEF;
  }
  else if (r1 < 0) {
    if (floor(r2) != r2)
      return rUNDEF;
  }
  return pow(r1, r2);
}

/*
static long powx(long l1, long l2)
{  return (l1 < 0) ? iUNDEF : longConv(pow(l1,l2)); }
*/

static double hypo(const double& r1, const double& r2)
{  return sqrt(r1*r1+r2*r2); }

/*
static double hypo(long l1, long l2)
{  return sqrt(l1*l1+l2*l2); }
*/
static double sq(const double& r1, const double& r2)
{  return r1*r1+r2*r2; }
/*
static long sq(long l1, long l2)
{  return l1*l1 + l2*l2; }
*/
#define deffunc2real(funcname)    \
static double funcname ## _r (const double& r1, const double& r2)  \
{ try {                         \
    if ((r1 == rUNDEF) || (r2 == rUNDEF))  \
      return rUNDEF;            \
    else                        \
      return funcname(r1, r2);  \
  }                             \
  catch (ErrorObject&) {        \
    return rUNDEF;              \
  }                             \
}                               \

#define deffunc2long(funcname)    \
static double funcname ## _l (long l1, long l2)  \
{ try {                                    \
    if ((l1 == iUNDEF) || (l2 == iUNDEF))  \
      return rUNDEF;            \
    else                        \
      return funcname(l1, l2);  \
  }                             \
  catch (ErrorObject&) {        \
    return rUNDEF;              \
  }                             \
}                               \

deffunc2real(atan2x)
deffunc2long(atan2x)
deffunc2real(powx)
deffunc2long(powx)
deffunc2real(hypo)
deffunc2long(hypo)
deffunc2real(sq)
deffunc2long(sq)

#define defmath2fnc(funcname) \
void funcname ## 2_so (StackObject* soRes, const StackObject* so0, const StackObject* so1) \
{ if (so0->sot == sotRealVal || so1->sot == sotRealVal) {\
    RealBuf buf0, buf1, bufRes;\
    so0->GetVal(buf0);\
    so1->GetVal(buf1);\
    bufRes.Size(soRes->iSize());\
    for (short i=0; i<soRes->iSize(); i++)\
      bufRes[i] = funcname##_r(buf0[i], buf1[i]);\
    soRes->PutVal(bufRes);\
  }\
  else if (soRes->sot == sotRealVal) {\
    LongBuf buf0, buf1;\
    RealBuf bufRes;\
    so0->GetVal(buf0);\
    so1->GetVal(buf1);\
    bufRes.Size(soRes->iSize());\
    for (short i=0; i<soRes->iSize(); i++)\
      bufRes[i] = funcname##_l(buf0[i], buf1[i]);\
    soRes->PutVal(bufRes);\
  }\
  else {\
    LongBuf buf0, buf1, bufRes;\
    so0->GetVal(buf0);\
    so1->GetVal(buf1);\
    bufRes.Size(soRes->iSize());\
    for (short i=0; i<soRes->iSize(); i++)\
      bufRes[i] = longConv(funcname##_l(buf0[i], buf1[i]));\
    soRes->PutVal(bufRes);\
  }\
}

defmath2fnc(atan2x)
defmath2fnc(powx)
defmath2fnc(hypo)
defmath2fnc(sq)




