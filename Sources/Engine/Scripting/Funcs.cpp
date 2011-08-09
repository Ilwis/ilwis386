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
/*
// $Log: /ILWIS 3.0/Calculator/Funcs.cpp $
 * 
 * 14    2-06-05 18:29 Retsios
 * [bug=6538] Added a custom function to set the domain of the output map
 * of the 2-parameter ifnotundef (previously the same function was used as
 * with ifundef)
 * 
 * 13    22-03-02 15:36 Koolhoven
 * removed changes of version 11, checks on DomainClass are not
 * understandable and caused problems (eg ID always resulted in undef)
 * 
 * 12    2/21/02 9:44a Martin
 * transformed some of the huge macros in real functions, cant debug
 * otherwise
 * 
 * 11    2/20/02 2:38p Martin
 * expanded "define function" to a real function to enable debugging
 * 
 * 10    11/07/01 2:05p Martin
 * function cnt also works on domain sort now
 * 
 * 9     11/05/01 11:39a Martin
 * OperEq and OperNEQ now also use the ValueCompatibleDomains function to
 * check the validity of the domains used.
 * 
 * 8     22-02-01 3:48p Martin
 * ifnotundef used the same function to check if valid params were given
 * as ifundef. The functions are not symetric if they have two params so
 * the function of ifundef was appropriate. ifnotundef has now its own
 * function
 * 
 * 7     12-07-00 8:20a Martin
 * solved several bugs in the calculator
 * 
 * 6     4/04/00 4:20p Wind
 * added function cnt for entire columns that counts all records that are
 * not undef
 * 
 * 5     4/04/00 1:55p Wind
 * added functions dms, sec, lat, lon, degree for conversion between
 * strings and degrees/seconds/lat/lon
 * 
 * 4     17-01-00 12:08 Wind
 * comment
 * 
 * 2     12-01-00 11:38 Wind
 * cosmetic changes
 */
// Revision 1.8  1998/10/08 15:45:02  Wim
// Added name_so() and code_so() to convert the items in a domain sort
// to a string as name resp. code.
//
// Revision 1.7  1998-09-16 18:30:36+01  Wim
// 22beta2
//
// Revision 1.6  1997/09/18 07:50:11  Wim
// Made check on param types in fCheckParms less strict
// vt has a double definition both data type and sructore type,
// this is confusion and causing problems
//
// Revision 1.5  1997-09-11 15:39:44+02  Wim
// fCheckInParms() also checks for special requirements to VarTypes of a function
//
// Revision 1.4  1997-08-26 16:44:49+02  Wim
// Changed DomainFunction for asin, acos, atan, atan2
// from RealDomain to MinHalfToHalfPiDomain, ZeroToPiDomain, MinPiToPiDomain
//
// Revision 1.3  1997-08-20 15:54:42+02  Wim
// Redefined AND and OR operators to behave like real three way logic
//
// Revision 1.2  1997-08-06 10:27:51+02  Wim
// rnd now uses LongPosDomain
//
/*
	Last change:  WK    8 Oct 98    4:36 pm
*/
#define FUNCS_C
#include "Engine\Table\Col.h"
#include "Engine\Map\Raster\Map.h"
#include "Engine\Domain\Dmvalue.h"
#include "Engine\Domain\dmsort.h"
#include "Engine\Domain\dmclass.h"
#include "Engine\Base\DataObjects\ARRAY.H"
#include "Engine\Scripting\CALCSTCK.H"
#include "Engine\Scripting\Calcvar.h"
#include "Engine\Scripting\Parscnst.h"
#include "Engine\Scripting\FUNCS.H"
#include "Engine\Base\Algorithm\Random.h"
#include "Engine\Base\DataObjects\Buf.h"
#include "Engine\Scripting\Funcsdm.h"
#include "Engine\Scripting\FNCSMATH.H"
#include "Engine\Scripting\Fncsmisc.h"

inline double add(double r1, double r2)
{  return r1+r2; }

inline long add(long l1, long l2)
{  return l1+l2; }

inline double sub(double r1, double r2)
{  return r1-r2; }

inline long sub(long l1, long l2)
{  return l1-l2; }

inline double times(double r1, double r2)
{  return r1*r2; }

inline long times(long l1, long l2)
{  return l1*l2; }

#define deffunc2real(funcname)    \
static double funcname ## _r (double r1, double r2)  \
{ if ((r1 == rUNDEF) || (r2 == rUNDEF))  \
    return rUNDEF;            \
  else                        \
    return funcname(r1, r2);  \
}

#define deffunc2long(funcname)    \
static double funcname ## _l (long l1, long l2)  \
{ if ((l1 == iUNDEF) || (l2 == iUNDEF))  \
    return iUNDEF;            \
  else                        \
    return funcname(l1, l2);  \
}

deffunc2real(add)
deffunc2long(add)
deffunc2real(sub)
deffunc2long(sub)
deffunc2real(times)
deffunc2long(times)

#define deffunc2fnc(funcname) \
static void funcname ## 2_so (StackObject* soRes, const StackObject* so0, const StackObject* so1) \
{ if (so0->sot == sotRealVal || so1->sot == sotRealVal || soRes->sot == sotRealVal) {\
    RealBuf buf0, buf1, bufRes;\
    so0->GetVal(buf0);\
    so1->GetVal(buf1);\
    bufRes.Size(soRes->iSize());\
    for (short i=0; i<soRes->iSize(); i++)\
      bufRes[i] = funcname##_r(buf0[i], buf1[i]);\
    soRes->PutVal(bufRes);\
  }\
  else {\
    LongBuf buf0, buf1, bufRes;\
    so0->GetVal(buf0);\
    so1->GetVal(buf1);\
    bufRes.Size(soRes->iSize());\
    for (short i=0; i<soRes->iSize(); i++)\
      bufRes[i] = funcname##_l(buf0[i], buf1[i]);\
    soRes->PutVal(bufRes);\
  }\
}

static void add2_so (StackObject* soRes, const StackObject* so0, const StackObject* so1) 
{ 
	if (so0->sot == sotRealVal || so1->sot == sotRealVal || soRes->sot == sotRealVal) 
	{
    RealBuf buf0, buf1, bufRes;
    so0->GetVal(buf0);
    so1->GetVal(buf1);
    bufRes.Size(soRes->iSize());
    for (short i=0; i<soRes->iSize(); i++)
      bufRes[i] = add_r(buf0[i], buf1[i]);
    soRes->PutVal(bufRes);
	//soRes->dvs.SetDomain(Domain("d:\\Data\\ILWIS\\at2.dom"));
  }
  else 
	{
    LongBuf buf0, buf1, bufRes;
    so0->GetVal(buf0);
    so1->GetVal(buf1);
    bufRes.Size(soRes->iSize());
    for (short i=0; i<soRes->iSize(); i++)
      bufRes[i] = add_l(buf0[i], buf1[i]);
    soRes->PutVal(bufRes);
  }
}

static void sub2_so (StackObject* soRes, const StackObject* so0, const StackObject* so1) 
{ 
	if (so0->sot == sotRealVal || so1->sot == sotRealVal || soRes->sot == sotRealVal) 
	{
    RealBuf buf0, buf1, bufRes;
    so0->GetVal(buf0);
    so1->GetVal(buf1);
    bufRes.Size(soRes->iSize());
    for (short i=0; i<soRes->iSize(); i++)
      bufRes[i] = sub_r(buf0[i], buf1[i]);
    soRes->PutVal(bufRes);
  }
  else 
	{
    LongBuf buf0, buf1, bufRes;
    so0->GetVal(buf0);
    so1->GetVal(buf1);
    bufRes.Size(soRes->iSize());
    for (short i=0; i<soRes->iSize(); i++)
      bufRes[i] = sub_l(buf0[i], buf1[i]);
    soRes->PutVal(bufRes);
  }
}

static void times2_so (StackObject* soRes, const StackObject* so0, const StackObject* so1) 
{ 
	if (so0->sot == sotRealVal || so1->sot == sotRealVal || soRes->sot == sotRealVal) 
	{
    RealBuf buf0, buf1, bufRes;
    so0->GetVal(buf0);
    so1->GetVal(buf1);
    bufRes.Size(soRes->iSize());
    for (short i=0; i<soRes->iSize(); i++)
      bufRes[i] = times_r(buf0[i], buf1[i]);
    soRes->PutVal(bufRes);
  }
  else 
	{
    LongBuf buf0, buf1, bufRes;
    so0->GetVal(buf0);
    so1->GetVal(buf1);
    bufRes.Size(soRes->iSize());
    for (short i=0; i<soRes->iSize(); i++)
      bufRes[i] = times_l(buf0[i], buf1[i]);
    soRes->PutVal(bufRes);
  }
}

#define EPS	1e-8

inline long eql(double r1, double r2)
{  
	return fabs(r1 - r2) < EPS; 
}

inline long eql(long l1, long l2)
{  return l1==l2; }

inline long eql(const String& s1, const String& s2)
{  return s1==s2; }

inline long neq(double r1, double r2)
{  return r1!=r2; }

inline long neq(long l1, long l2)
{  return l1!=l2; }

inline long neq(const String& s1, const String& s2)
{  return s1!=s2; }

inline long gt(double r1, double r2)
{  return r1>r2; }

inline long gt(long l1, long l2)
{  return l1>l2; }

inline long gt(const String& s1, const String& s2)
{  return s1>s2; }

inline long lt(double r1, double r2)
{  return r1<r2; }

inline long lt(long l1, long l2)
{  return l1<l2; }

inline long lt(const String& s1, const String& s2)
{  return s1<s2; }

inline long ge(double r1, double r2)
{  return r1>=r2; }

inline long ge(long l1, long l2)
{  return l1>=l2; }

inline long ge(const String& s1, const String& s2)
{  return s1>=s2; }

inline long le(double r1, double r2)
{  return r1<=r2; }

inline long le(long l1, long l2)
{  return l1<=l2; }

inline long le(const String& s1, const String& s2)
{  return s1<=s2; }

#define defeqreal(funcname)    \
static long funcname ## _r (double r1, double r2)  \
{ if ((r1 == rUNDEF) || (r2 == rUNDEF))  \
    return iUNDEF;            \
  else                        \
    return funcname(r1, r2);  \
}

#define defeqlong(funcname)    \
static long funcname ## _l (long l1, long l2)  \
{ if ((l1 == iUNDEF) || (l2 == iUNDEF))  \
    return iUNDEF;            \
  else                        \
    return funcname(l1, l2);  \
}

#define defeqstr(funcname)    \
static long funcname ## _s (const String& s1, const String& s2)  \
{ if ((s1 == sUNDEF) || (s2 == sUNDEF))  \
    return iUNDEF;            \
  else                        \
    return funcname(s1, s2);  \
}

defeqreal(eql)
defeqlong(eql)
defeqstr(eql)
defeqreal(neq)
defeqlong(neq)
defeqstr(neq)
defeqreal(gt)
defeqlong(gt)
defeqstr(gt)
defeqreal(lt)
defeqlong(lt)
defeqstr(lt)
defeqreal(ge)
defeqlong(ge)
defeqstr(ge)
defeqreal(le)
defeqlong(le)
defeqstr(le)

#define defeqfnc(funcname) \
static void funcname ## _so (StackObject* soRes, const StackObject* so0, const StackObject* so1) \
{ if (so0->sot == sotRealVal || so1->sot == sotRealVal) {\
    RealBuf buf0, buf1;\
    LongBuf bufRes;\
    so0->GetVal(buf0);\
    so1->GetVal(buf1);\
    bufRes.Size(soRes->iSize());\
    for (short i=0; i<soRes->iSize(); i++)\
      bufRes[i] = funcname##_r(buf0[i], buf1[i]);\
    soRes->PutVal(bufRes);\
  }\
  else {\
    LongBuf buf0, buf1, bufRes;\
    so0->GetVal(buf0);\
    so1->GetVal(buf1);\
    bufRes.Size(soRes->iSize());\
    for (short i=0; i<soRes->iSize(); i++)\
      bufRes[i] = funcname##_l(buf0[i], buf1[i]);\
    soRes->PutVal(bufRes);\
  }\
}

defeqfnc(eql)
defeqfnc(neq)
defeqfnc(gt)
defeqfnc(lt)
defeqfnc(ge)
defeqfnc(le)

static void addstrval(StackObject* soRes, const StackObject* so0, const StackObject* so1)
{
  StringBuf buf0, buf1, bufRes;
  so0->GetVal(buf0);
  so1->GetVal(buf1);
  bufRes.Size(soRes->iSize());
  for (short i=0; i<soRes->iSize(); i++) {
    if ((buf0[i] == sUNDEF) || (buf1[i] == sUNDEF))
      bufRes[i] = sUNDEF;
    else {
      bufRes[i] = buf0[i];
      bufRes[i] &= buf1[i];
    }
  }
  soRes->PutVal(bufRes);
}

static void divideval(StackObject* soRes, const StackObject* so0, const StackObject* so1)
{
  if (so0->sot == sotRealVal || so1->sot == sotRealVal) {
    RealBuf buf0, buf1, bufRes;
    so0->GetVal(buf0);
    so1->GetVal(buf1);
    bufRes.Size(soRes->iSize());
    for (short i=0; i<soRes->iSize(); i++) {
      if ((buf0[i] == rUNDEF) || (buf1[i] == rUNDEF) || (buf1[i] == 0))
        bufRes[i] = rUNDEF;
      else
        bufRes[i] = buf0[i] / buf1[i];
    }
    soRes->PutVal(bufRes);
  }
  else {
    LongBuf buf0, buf1;
    RealBuf bufRes;
    so0->GetVal(buf0);
    so1->GetVal(buf1);
    bufRes.Size(soRes->iSize());
    for (short i=0; i<soRes->iSize(); i++) {
      if ((buf0[i] == iUNDEF) || (buf1[i] == iUNDEF) || (buf1[i] == 0))
        bufRes[i] = rUNDEF;
      else
        bufRes[i] = ((double)buf0[i]) / buf1[i];
    }
    soRes->PutVal(bufRes);
  }
}

static void div_so(StackObject* soRes, const StackObject* so0, const StackObject* so1)
{
  LongBuf buf0, buf1;
  LongBuf bufRes;
  so0->GetVal(buf0);
  so1->GetVal(buf1);
  bufRes.Size(soRes->iSize());
  for (short i=0; i<soRes->iSize(); i++) {
    if ((buf0[i] == iUNDEF) || (buf1[i] == iUNDEF) || (buf1[i] == 0))
      bufRes[i] = iUNDEF;
    else
      bufRes[i] = buf0[i] / buf1[i];
  }
  soRes->PutVal(bufRes);
}

static void mod_so(StackObject* soRes, const StackObject* so0, const StackObject* so1)
{
  LongBuf buf0, buf1;
  LongBuf bufRes;
  so0->GetVal(buf0);
  so1->GetVal(buf1);
  bufRes.Size(soRes->iSize());
  for (short i=0; i<soRes->iSize(); i++) {
    if ((buf0[i] == iUNDEF) || (buf1[i] == iUNDEF) || (buf1[i] == 0))
      bufRes[i] = iUNDEF;
    else
      bufRes[i] = buf0[i] % buf1[i];
  }
  soRes->PutVal(bufRes);
}

#define defeqrawfnc(funcname) \
static void funcname ## raw_so (StackObject* soRes, const StackObject* so0, const StackObject* so1) \
{ if (so0->sot == sotLongRaw && so1->sot == sotLongRaw) {\
    LongBuf buf0, buf1;\
    LongBuf bufRes;\
    so0->GetRaw(buf0);\
    so1->GetRaw(buf1);\
    bufRes.Size(soRes->iSize());\
    for (short i=0; i<soRes->iSize(); i++)\
      bufRes[i] = funcname##_l(buf0[i], buf1[i]);\
    soRes->PutVal(bufRes);\
  }\
  else {\
    StringBuf buf0, buf1; \
    LongBuf bufRes;\
    so0->GetVal(buf0);\
    so1->GetVal(buf1);\
    bufRes.Size(soRes->iSize());\
    for (short i=0; i<soRes->iSize(); i++)\
      bufRes[i] = funcname##_s(buf0[i], buf1[i]);\
    soRes->PutVal(bufRes);\
  }\
}

#define defcomparerawfnc(funcname) \
static void funcname ## raw_so (StackObject* soRes, const StackObject* so0, const StackObject* so1) \
{ short i;\
  if (so0->sot == sotLongRaw && so1->sot == sotLongRaw) {\
    LongBuf buf0, buf1;\
    LongBuf bufRes;\
    so0->GetRaw(buf0);\
    DomainClass* pdc0 = so0->dvs.dm()->pdc();\
    if (pdc0)\
      for (i=0; i<soRes->iSize(); i++)\
       buf0[i] = pdc0->iOrd(buf0[i]);\
    so1->GetRaw(buf1);\
    DomainClass* pdc1 = so1->dvs.dm()->pdc();\
    if (pdc1)\
      for (i=0; i<soRes->iSize(); i++)\
       buf1[i] = pdc1->iOrd(buf1[i]);\
    bufRes.Size(soRes->iSize());\
    if (pdc0 && pdc1)\
      for (i=0; i<soRes->iSize(); i++)\
        bufRes[i] = funcname##_l(buf0[i], buf1[i]);\
    else \
      for (i=0; i<soRes->iSize(); i++)\
        bufRes[i] = iUNDEF; \
    soRes->PutVal(bufRes);\
  }\
  else {\
    StringBuf buf0, buf1; \
    LongBuf bufRes;\
    so0->GetVal(buf0);\
    so1->GetVal(buf1);\
    bufRes.Size(soRes->iSize());\
    for (short i=0; i<soRes->iSize(); i++)\
      bufRes[i] = funcname##_s(buf0[i], buf1[i]);\
    soRes->PutVal(bufRes);\
  }\
}

defeqrawfnc(eql)
defeqrawfnc(neq)

defcomparerawfnc(gt)
defcomparerawfnc(lt)
defcomparerawfnc(ge)
defcomparerawfnc(le)

/*static long not(long l)
{ return l == 0; }*/

inline long andx(long l1, long l2)
{
  if (0 == l1 || 0 == l2)
    return 0;
  else if (iUNDEF == l1 || iUNDEF == l2)
    return iUNDEF;
  else
    return 1;
}

inline long orx(long l1, long l2)
{
  if (0 == l1 && 0 == l2)
    return 0;
  else if ((0 == l1 || iUNDEF == l1) &&
           (0 == l2 || iUNDEF == l2))
    return iUNDEF;
  else
    return 1;
}

inline long xor(long l1, long l2)
{
  if (iUNDEF == l1 || iUNDEF == l2)
    return iUNDEF;
  else
    return l1 != l2;
}

static void not_so (StackObject* soRes, const StackObject* so)
{ LongBuf buf, bufRes;
  so->GetVal(buf);
  bufRes.Size(soRes->iSize());
  for (short i=0; i<soRes->iSize(); i++)
    if (buf[i] == iUNDEF)
      bufRes[i] = iUNDEF;
    else
      bufRes[i] = buf[i] == 0;
  soRes->PutVal(bufRes);
}

#define deflogic2fnc(funcname) \
static void funcname ## _so (StackObject* soRes, const StackObject* so0, const StackObject* so1) \
{ LongBuf buf0, buf1, bufRes;               \
  so0->GetVal(buf0);                   \
  so1->GetVal(buf1);                   \
  bufRes.Size(soRes->iSize());       \
  for (short i=0; i<soRes->iSize(); i++) \
    bufRes[i] = funcname(buf0[i],buf1[i]); \
  soRes->PutVal(bufRes);  \
}

deflogic2fnc(andx)
deflogic2fnc(orx)
deflogic2fnc(xor)

static double sgn(double r)
  { return  r == 0 ? 0 : (r < 0 ? -1 : 1) ; }

static long sgn(long l)
  { return  l == 0 ? 0 : (l < 0 ? -1 : 1) ; }

static void sgn_so (StackObject* soRes, const StackObject* so)
{
  if (so->sot == sotLongVal) {
    LongBuf buf, bufRes;
    so->GetVal(buf);
    bufRes.Size(soRes->iSize());
    for (short i=0; i<soRes->iSize(); i++)
      if (buf[i] == iUNDEF)
        bufRes[i] = iUNDEF;
      else
        bufRes[i] = sgn(buf[i]);
    soRes->PutVal(bufRes);
  }
  else {
    RealBuf buf;
    LongBuf bufRes;
    so->GetVal(buf);
    bufRes.Size(soRes->iSize());
    for (short i=0; i<soRes->iSize(); i++)
      if (buf[i] == rUNDEF)
        bufRes[i] = iUNDEF;
      else
        bufRes[i] = sgn(buf[i]);
    soRes->PutVal(bufRes);
  }
}

/*
void leraw_so (StackObject* soRes, const StackObject* so0, const StackObject* so1)
{ short i;
  if (so0->sot == sotLongRaw && so1->sot == sotLongRaw) {
    LongBuf buf0, buf1;
    LongBuf bufRes;
    so0->GetRaw(buf0);
    DomainClass* pdc0 = so0->dm->pdc();
    if (pdc0)
      for (i=0; i<soRes->iSize(); i++)
       buf0[i] = pdc0->iOrd(buf0[i]);
    so1->GetRaw(buf1);
    DomainClass* pdc1 = so1->dm->pdc();
    if (pdc1)
      for (i=0; i<soRes->iSize(); i++)
       buf1[i] = pdc1->iOrd(buf1[i]);
    bufRes.Size(soRes->iSize());
    if (pdc0 && pdc1)
      for (i=0; i<soRes->iSize(); i++)
        bufRes[i] = le_l(buf0[i], buf1[i]);
    else
      for (i=0; i<soRes->iSize(); i++)
        bufRes[i] = iUNDEF;
    soRes->PutVal(bufRes);
  }
  else {
    StringBuf buf0, buf1;
    LongBuf bufRes;
    so0->GetVal(buf0);
    so1->GetVal(buf1);
    bufRes.Size(soRes->iSize());
    for (short i=0; i<soRes->iSize(); i++)
      bufRes[i] = le_s(buf0[i], buf1[i]);
    soRes->PutVal(bufRes);
  }
}
*/


InternFuncDesc::InternFuncDesc(char* sFuncName, InternalFunc fi, FuncMath fm, FuncResultDomain fdom,
                 FuncCheckDomains fchdom, short iPrms, VarType vtResult ...)
{
  sFunc = sFuncName;
  fp = fi;
  fpdm = fdom;
  fpchdm = fchdom;
  fmath = fm;
  vtRes = vtResult;
  va_list args;
  va_start(args, vtResult);
  for (short i=0; i < iPrms; i++)
    vtl &= va_arg(args, VarType);
  va_end(args);
//  iFirstParmToCheck = 0;
}

InternFuncDesc* InternFuncDesc::fdsCopy() const
{
  InternFuncDesc* fds = new InternFuncDesc(sFunc, fp, fmath, fpdm, fpchdm, 0, vtRes);
//  fds->iFirstParmToCheck = iFirstParmToCheck;
  for (int i=0; i < vtl.iSize(); ++i)
    fds->vtl &= vtl[i];
  return fds;
}

void InternFuncDesc::CreateInternFuncs(Array<InternFuncDesc*>& fdl)
{
  fdl &= new InternFuncDesc("rnd",      (InternalFunc)rnd0_so, 0,
                            Min1To1Domain, (FuncCheckDomains)fValueDomains,
                            0, vtVALUE );
  fdl &= new InternFuncDesc("neg",      (InternalFunc)neg_so, neg,
                            FuncDomains, (FuncCheckDomains)fValueDomains,
                            1, vtVALUE, vtVALUE );
  fdl &= new InternFuncDesc("sgn",      (InternalFunc)sgn_so, neg,
                            Min1To1IntDomain, (FuncCheckDomains)fValueDomains,
                            1, vtVALUE, vtVALUE );
  fdl &= new InternFuncDesc("abs",      (InternalFunc)absx_so, absx,
                            FuncDomains, (FuncCheckDomains)fValueDomains,
                            1, vtVALUE, vtVALUE );
  fdl &= new InternFuncDesc("floor",      (InternalFunc)floor_so, floor,
                            FuncDomainsInt, (FuncCheckDomains)fValueDomains,
                            1, vtVALUE, vtVALUE );
  fdl &= new InternFuncDesc("ceil",      (InternalFunc)ceil_so, ceil,
                            FuncDomainsInt, (FuncCheckDomains)fValueDomains,
                            1, vtVALUE, vtVALUE );
  fdl &= new InternFuncDesc("round",      (InternalFunc)roundy_so, roundy,
                            FuncDomainsInt, (FuncCheckDomains)fValueDomains,
                            1, vtVALUE, vtVALUE );
  fdl &= new InternFuncDesc("rnd",      (InternalFunc)randx_so, randx,
                            LongPosDomain, (FuncCheckDomains)fValueDomains,
                            1, vtVALUE, vtVALUE );
  fdl &= new InternFuncDesc("sqrt",      (InternalFunc)sqrt1_so, sqrt,
                            FuncNonNegDomains, (FuncCheckDomains)fValueDomains,
                            1, vtVALUE, vtVALUE );
  fdl &= new InternFuncDesc("log",      (InternalFunc)log10x_so, log10x,
                            FuncPosDomains, (FuncCheckDomains)fValueDomains,
                            1, vtVALUE, vtVALUE );
  fdl &= new InternFuncDesc("ln",       (InternalFunc)logx_so, logx,
                            FuncPosDomains, (FuncCheckDomains)fValueDomains,
                            1, vtVALUE, vtVALUE );
  fdl &= new InternFuncDesc("log10",       (InternalFunc)log10x_so, log10x,
                            FuncPosDomains, (FuncCheckDomains)fValueDomains,
                            1, vtVALUE, vtVALUE );
  fdl &= new InternFuncDesc("exp",      (InternalFunc)expx_so, expx,
                            ExpDomains, (FuncCheckDomains)fValueDomains,
                            1, vtVALUE, vtVALUE );
  fdl &= new InternFuncDesc("sq",      (InternalFunc)sqrx_so, sqrx,
                            FuncDomains, (FuncCheckDomains)fValueDomains,
                            1, vtVALUE, vtVALUE );
  fdl &= new InternFuncDesc("degrad",   (InternalFunc)degrad_so, degrad,
                            FuncDomains, (FuncCheckDomains)fValueDomains,
                            1, vtVALUE, vtVALUE );
  fdl &= new InternFuncDesc("raddeg",   (InternalFunc)raddeg_so, 0,
                            DegDomain, (FuncCheckDomains)fValueDomains,
                            1, vtVALUE, vtVALUE );
  fdl &= new InternFuncDesc("sin",      (InternalFunc)sin_so, sin,
                            Min1To1Domain, (FuncCheckDomains)fValueDomains,
                            1, vtVALUE, vtVALUE );
  fdl &= new InternFuncDesc("cos",      (InternalFunc)cos_so, cos,
                            Min1To1Domain, (FuncCheckDomains)fValueDomains,
                            1, vtVALUE, vtVALUE );
  fdl &= new InternFuncDesc("tan",      (InternalFunc)tan_so, tan,
                            RealDomain, (FuncCheckDomains)fValueDomains,
                            1, vtVALUE, vtVALUE );

  fdl &= new InternFuncDesc("asin",     (InternalFunc)asinx_so, asinx,
                            MinHalfPiToHalfPiDomain, (FuncCheckDomains)fValueDomains,
                            1, vtVALUE, vtVALUE );
  fdl &= new InternFuncDesc("acos",     (InternalFunc)acosx_so, acosx,
                            ZeroToPiDomain, (FuncCheckDomains)fValueDomains,
                            1, vtVALUE, vtVALUE );
  fdl &= new InternFuncDesc("atan",     (InternalFunc)atan_so, atan,
                            MinHalfPiToHalfPiDomain, (FuncCheckDomains)fValueDomains,
                            1, vtVALUE, vtVALUE );
  fdl &= new InternFuncDesc("atan2",  (InternalFunc)atan2x2_so, 0,
                            MinPiToPiDomain, (FuncCheckDomains)fValueDomains,
                            2, vtVALUE, vtVALUE, vtVALUE );
  fdl &= new InternFuncDesc("sinh",     (InternalFunc)sinhx_so, sinhx,
                            RealDomain, (FuncCheckDomains)fValueDomains,
                            1, vtVALUE, vtVALUE );
  fdl &= new InternFuncDesc("cosh",     (InternalFunc)coshx_so, coshx,
                            RealDomain, (FuncCheckDomains)fValueDomains,
                            1, vtVALUE, vtVALUE );
  fdl &= new InternFuncDesc("tanh",     (InternalFunc)tanh_so, tanh,
                            RealDomain, (FuncCheckDomains)fValueDomains,
                            1, vtVALUE, vtVALUE );
  fdl &= new InternFuncDesc("isundef",  (InternalFunc)isundef_so, 0,
                            BitDomain,  (FuncCheckDomains)fValueDomains,
                            1, vtVALUE, vtVALUE );
  fdl &= new InternFuncDesc("isundef",  (InternalFunc)isundefraw_so, 0,
                            BitDomain,  (FuncCheckDomains)fStringCompatibleDomains,
                            1, vtVALUE, vtSTRING );
  fdl &= new InternFuncDesc("time",  (InternalFunc)time_so, 0,
                            RealDomain,  (FuncCheckDomains)fStringCompatibleDomains,
                            1, vtVALUE, vtSTRING );

  fdl &= new InternFuncDesc(operPLUS,   (InternalFunc)add2_so, 0,
                            AddDomains, (FuncCheckDomains)fValueDomains,
                            2, vtVALUE, vtVALUE, vtVALUE );
  fdl &= new InternFuncDesc(operPLUS,   (InternalFunc)addstrval, 0,
                            StringDomain, (FuncCheckDomains)fStringCompatibleDomains,
                            2, vtSTRING, vtSTRING, vtSTRING );
  fdl &= new InternFuncDesc(operMIN,   (InternalFunc)sub2_so, 0,
                            SubtractDomains, (FuncCheckDomains)fValueDomains,
                            2, vtVALUE, vtVALUE, vtVALUE );
  fdl &= new InternFuncDesc(operTIMES,   (InternalFunc)times2_so, 0,
                            TimesDomains, (FuncCheckDomains)fValueDomains,
                            2, vtVALUE, vtVALUE, vtVALUE );
  fdl &= new InternFuncDesc(operDIVIDE,   (InternalFunc)divideval, 0,
                            DivideDomains, (FuncCheckDomains)fValueDomains,
                            2, vtVALUE, vtVALUE, vtVALUE );
  fdl &= new InternFuncDesc(operEQ,     (InternalFunc)eql_so, 0,
                            BoolDomain, (FuncCheckDomains)fValueDomains,
                            2, vtVALUE, vtVALUE, vtVALUE );
	fdl &= new InternFuncDesc(operEQ,     (InternalFunc)eql_so, 0,
                            BoolDomain, (FuncCheckDomains)fValueCompatibleDomains,
                            2, vtVALUE, vtVALUE, vtVALUE );	
  fdl &= new InternFuncDesc(operNE,     (InternalFunc)neq_so, 0,
                            BoolDomain, (FuncCheckDomains)fValueDomains,
                            2, vtVALUE, vtVALUE, vtVALUE );
  fdl &= new InternFuncDesc(operNE,     (InternalFunc)neq_so, 0,
                            BoolDomain, (FuncCheckDomains)fValueCompatibleDomains,
                            2, vtVALUE, vtVALUE, vtVALUE );	
  fdl &= new InternFuncDesc(operGT,     (InternalFunc)gt_so, 0,
                            BoolDomain, (FuncCheckDomains)fValueDomains,
                            2, vtVALUE, vtVALUE, vtVALUE );
  fdl &= new InternFuncDesc(operLT,     (InternalFunc)lt_so, 0,
                            BoolDomain, (FuncCheckDomains)fValueDomains,
                            2, vtVALUE, vtVALUE, vtVALUE );
  fdl &= new InternFuncDesc(operGE,     (InternalFunc)ge_so, 0,
                            BoolDomain, (FuncCheckDomains)fValueDomains,
                            2, vtVALUE, vtVALUE, vtVALUE );
  fdl &= new InternFuncDesc(operLE,     (InternalFunc)le_so, 0,
                            BoolDomain, (FuncCheckDomains)fValueDomains,
                            2, vtVALUE, vtVALUE, vtVALUE );
  fdl &= new InternFuncDesc(operEQ,     (InternalFunc)eqlraw_so, 0,
                            BoolDomainFromRaws, (FuncCheckDomains)fStringCompatibleDomains,
                            2, vtVALUE, vtSTRING, vtSTRING );
  fdl &= new InternFuncDesc(operNE,     (InternalFunc)neqraw_so, 0,
                            BoolDomainFromRaws, (FuncCheckDomains)fStringCompatibleDomains,
                            2, vtVALUE, vtSTRING, vtSTRING );
  fdl &= new InternFuncDesc(operLT,     (InternalFunc)ltraw_so, 0,
                            BoolDomainFromRaws, (FuncCheckDomains)fStringCompatibleDomainsNoID,
                            2, vtVALUE, vtSTRING, vtSTRING );
  fdl &= new InternFuncDesc(operLE,     (InternalFunc)leraw_so, 0,
                            BoolDomainFromRaws, (FuncCheckDomains)fStringCompatibleDomainsNoID,
                            2, vtVALUE, vtSTRING, vtSTRING );
  fdl &= new InternFuncDesc(operGT,     (InternalFunc)gtraw_so, 0,
                            BoolDomainFromRaws, (FuncCheckDomains)fStringCompatibleDomainsNoID,
                            2, vtVALUE, vtSTRING, vtSTRING );
  fdl &= new InternFuncDesc(operGE,     (InternalFunc)geraw_so, 0,
                            BoolDomainFromRaws, (FuncCheckDomains)fStringCompatibleDomainsNoID,
                            2, vtVALUE, vtSTRING, vtSTRING );
  fdl &= new InternFuncDesc("strlt",     (InternalFunc)ltraw_so, 0,
                            BoolDomainFromStrings, (FuncCheckDomains)fStringCompatibleDomains,
                            2, vtVALUE, vtSTRING, vtSTRING );
  fdl &= new InternFuncDesc("strle",     (InternalFunc)leraw_so, 0,
                            BoolDomainFromStrings, (FuncCheckDomains)fStringCompatibleDomains,
                            2, vtVALUE, vtSTRING, vtSTRING );
  fdl &= new InternFuncDesc("strgt",     (InternalFunc)gtraw_so, 0,
                            BoolDomainFromStrings, (FuncCheckDomains)fStringCompatibleDomains,
                            2, vtVALUE, vtSTRING, vtSTRING );
  fdl &= new InternFuncDesc("strge",     (InternalFunc)geraw_so, 0,
                            BoolDomainFromStrings, (FuncCheckDomains)fStringCompatibleDomains,
                            2, vtVALUE, vtSTRING, vtSTRING );

  fdl &= new InternFuncDesc("ndvi",  (InternalFunc)ndvi_so, 0,
                            Min1To1Domain, (FuncCheckDomains)fValueDomains,
                            2, vtVALUE, vtVALUE, vtVALUE );
  fdl &= new InternFuncDesc("pow",  (InternalFunc)powx2_so, 0,
                            RealDomain, (FuncCheckDomains)fValueDomains,
                            2, vtVALUE, vtVALUE, vtVALUE );
  fdl &= new InternFuncDesc(operPOWER,  (InternalFunc)powx2_so, 0,
                            RealDomain, (FuncCheckDomains)fValueDomains,
                            2, vtVALUE, vtVALUE, vtVALUE );
  fdl &= new InternFuncDesc("hyp", (InternalFunc)hypo2_so , 0,
                            HypoDomain, (FuncCheckDomains)fValueDomains,
                            2, vtVALUE, vtVALUE, vtVALUE );
  fdl &= new InternFuncDesc("sq",      (InternalFunc)sq2_so, 0,
                            RealOrLongDomain, (FuncCheckDomains)fValueDomains,
                            2, vtVALUE, vtVALUE, vtVALUE );
  fdl &= new InternFuncDesc("iff",      (InternalFunc)iff_so, 0,
                            IffDomain, (FuncCheckDomains)fIffCheckDomains,
                            3, vtVALUE, vtVALUE, vtVALUE, vtVALUE );
  fdl &= new InternFuncDesc("ifundef",      (InternalFunc)ifundef2_so, 0,
                            IfUndefDomain, (FuncCheckDomains)fIfUndefCheckDomains,
                            2, vtVALUE, vtVALUE, vtVALUE );
  fdl &= new InternFuncDesc("ifnotundef",      (InternalFunc)ifnotundef2_so, 0,
                            IfNotUndefDomain, (FuncCheckDomains)fIfNotUndefCheckDomains,
                            2, vtVALUE, vtVALUE, vtVALUE );
  fdl &= new InternFuncDesc("ifundef",      (InternalFunc)ifundef3_so, 0,
                            IffDomain, (FuncCheckDomains)fIfUndefCheckDomains,
                            3, vtVALUE, vtVALUE, vtVALUE, vtVALUE );
  fdl &= new InternFuncDesc("ifnotundef",      (InternalFunc)ifnotundef3_so, 0,
                            IffDomain, (FuncCheckDomains)fIfUndefCheckDomains,
                            3, vtVALUE, vtVALUE, vtVALUE, vtVALUE );

  fdl &= new InternFuncDesc(operDIV,    (InternalFunc)div_so, 0,
                            DivDomains, (FuncCheckDomains)fValueDomains,
                            2, vtVALUE, vtVALUE, vtVALUE );
  fdl &= new InternFuncDesc(operMOD,    (InternalFunc)mod_so, 0,
                            ModDomains, (FuncCheckDomains)fValueDomains,
                            2, vtVALUE, vtVALUE, vtVALUE );

  fdl &= new InternFuncDesc("not",      (InternalFunc)not_so, 0,
                            BoolDomain, (FuncCheckDomains)fValueDomains,
                            1, vtVALUE, vtVALUE);
  fdl &= new InternFuncDesc(operAND,    (InternalFunc)andx_so, 0,
                            BoolDomain, (FuncCheckDomains)fCompatibleDomains,
                            2, vtVALUE, vtVALUE, vtVALUE );
  fdl &= new InternFuncDesc(operOR,     (InternalFunc)orx_so, 0,
                            BoolDomain, (FuncCheckDomains)fCompatibleDomains,
                            2, vtVALUE, vtVALUE, vtVALUE );
  fdl &= new InternFuncDesc(operXOR,    (InternalFunc)xor_so, 0,
                            BoolDomain, (FuncCheckDomains)fCompatibleDomains,
                            2, vtVALUE, vtVALUE, vtVALUE );
  // string functions
  fdl &= new InternFuncDesc("length",   (InternalFunc)length_so, 0,
                            IntDomain, (FuncCheckDomains)fStringCompatibleDomains,
                            1, vtVALUE, vtSTRING);
  fdl &= new InternFuncDesc("sub",   (InternalFunc)substr_so, 0,
                            StringDomain, (FuncCheckDomains)fString1Values,
                            3, vtSTRING, vtSTRING, vtVALUE, vtVALUE);
  fdl &= new InternFuncDesc("left",   (InternalFunc)leftstr_so, 0,
                            StringDomain, (FuncCheckDomains)fString1Values,
                            2, vtSTRING, vtSTRING, vtVALUE);
  fdl &= new InternFuncDesc("right",   (InternalFunc)rightstr_so, 0,
                            StringDomain, (FuncCheckDomains)fString1Values,
                            2, vtSTRING, vtSTRING, vtVALUE);
  fdl &= new InternFuncDesc("in",   (InternalFunc)instr_so, 0,
                            BoolDomain, (FuncCheckDomains)fStringCompatibleDomains,
                            2, vtVALUE, vtSTRING, vtSTRING);
  fdl &= new InternFuncDesc("strpos",   (InternalFunc)strpos_so, 0,
                            IntDomain, (FuncCheckDomains)fStringCompatibleDomains,
                            2, vtVALUE, vtSTRING, vtSTRING);
  fdl &= new InternFuncDesc("inrange",   (InternalFunc)inrange_so, 0,
                            BoolDomain, (FuncCheckDomains)fValueDomains,
                            3, vtVALUE, vtVALUE, vtVALUE, vtVALUE);
  // min / max
  fdl &= new InternFuncDesc("min",      (InternalFunc)min2_so, 0,
                            MinDomains,  (FuncCheckDomains)fValueDomains,
                            2, vtVALUE, vtVALUE, vtVALUE );
  fdl &= new InternFuncDesc("max",      (InternalFunc)max2_so, 0,
                            MaxDomains,  (FuncCheckDomains)fValueDomains,
                            2, vtVALUE, vtVALUE, vtVALUE );
  fdl &= new InternFuncDesc("min",      (InternalFunc)min3_so, 0,
                            MinDomains,  (FuncCheckDomains)fValueDomains,
                            3, vtVALUE, vtVALUE, vtVALUE, vtVALUE );
  fdl &= new InternFuncDesc("max",      (InternalFunc)max3_so, 0,
                            MaxDomains,  (FuncCheckDomains)fValueDomains,
                            3, vtVALUE, vtVALUE, vtVALUE, vtVALUE );
  fdl &= new InternFuncDesc("avg",      (InternalFunc)avgcol_so, 0,
                            RealDomain,  (FuncCheckDomains) fValueDomains,
                            1, vtVALUE, vtCOLUMN);
  fdl &= new InternFuncDesc("sum",      (InternalFunc)sumcol_so, 0,
                            RealDomain,  (FuncCheckDomains) fValueDomains,
                            1, vtVALUE, vtCOLUMN);
  fdl &= new InternFuncDesc("cnt",      (InternalFunc)cntcol_so, 0,
                            RealDomain,  (FuncCheckDomains) fDontCare,
                            1, vtVALUE, vtCOLUMN);
  fdl &= new InternFuncDesc("var",      (InternalFunc)varcol_so, 0,
                            RealDomain,  (FuncCheckDomains) fValueDomains,
                            1, vtVALUE, vtCOLUMN);
  fdl &= new InternFuncDesc("stdev",    (InternalFunc)stdevcol_so, 0,
                            RealDomain,  (FuncCheckDomains) fValueDomains,
                            1, vtVALUE, vtCOLUMN);
  fdl &= new InternFuncDesc("std",      (InternalFunc)stdevcol_so, 0,
                            RealDomain,  (FuncCheckDomains) fValueDomains,
                            1, vtVALUE, vtCOLUMN);
  fdl &= new InternFuncDesc("stderr",   (InternalFunc)stderrcol_so, 0,
                            RealDomain,  (FuncCheckDomains) fValueDomains,
                            1, vtVALUE, vtCOLUMN);
  fdl &= new InternFuncDesc("min",      (InternalFunc)mincol_so, 0,
                            SameDomain,  (FuncCheckDomains) fValueDomains,
                            1, vtVALUE, vtCOLUMN);
  fdl &= new InternFuncDesc("max",      (InternalFunc)maxcol_so, 0,
                            SameDomain,  (FuncCheckDomains) fValueDomains,
                            1, vtVALUE, vtCOLUMN);
  fdl &= new InternFuncDesc("ttest",    (InternalFunc)ttestcol_so, 0,
                            RealDomain,  (FuncCheckDomains) fValueDomains,
                            2, vtVALUE, vtVALUE, vtCOLUMN);
  fdl &= new InternFuncDesc("chisquare", (InternalFunc)chisquarecol_so, 0,
                            RealDomain,  (FuncCheckDomains) fValueDomains,
                            2, vtVALUE, vtCOLUMN, vtCOLUMN);
  fdl &= new InternFuncDesc("corr",      (InternalFunc)corrcol_so, 0,
                            RealDomain,  (FuncCheckDomains) fValueDomains,
                            2, vtVALUE, vtCOLUMN, vtCOLUMN);
  fdl &= new InternFuncDesc("cov",       (InternalFunc)covarcol_so, 0,
                            RealDomain,  (FuncCheckDomains) fValueDomains,
                            2, vtVALUE, vtCOLUMN, vtCOLUMN);
  fdl &= new InternFuncDesc("covar",     (InternalFunc)covarcol_so, 0,
                            RealDomain,  (FuncCheckDomains) fValueDomains,
                            2, vtVALUE, vtCOLUMN, vtCOLUMN);

  fdl &= new InternFuncDesc("name",   (InternalFunc)name_so, 0,
                            StringDomain, (FuncCheckDomains)fSortDomains,
                            1, vtSTRING, vtSTRING);
  fdl &= new InternFuncDesc("code",   (InternalFunc)code_so, 0,
                            StringDomain, (FuncCheckDomains)fSortDomains,
                            1, vtSTRING, vtSTRING);
  fdl &= new InternFuncDesc("dms",   (InternalFunc)dms_so, 0,
                            StringDomain, (FuncCheckDomains)fValueDomains,
                            1, vtSTRING, vtVALUE);
  fdl &= new InternFuncDesc("lat",   (InternalFunc)lat_so, 0,
                            StringDomain, (FuncCheckDomains)fValueDomains,
                            1, vtSTRING, vtVALUE);
  fdl &= new InternFuncDesc("lon",   (InternalFunc)lon_so, 0,
                            StringDomain, (FuncCheckDomains)fValueDomains,
                            1, vtSTRING, vtVALUE);
  fdl &= new InternFuncDesc("sec",   (InternalFunc)sec_so, 0,
                            RealDomain, (FuncCheckDomains)fStringCompatibleDomains,
                            1, vtVALUE, vtSTRING); 
  fdl &= new InternFuncDesc("degree",   (InternalFunc)degree_so, 0,
                            RealDomain, (FuncCheckDomains)fStringCompatibleDomains,
                            1, vtVALUE, vtSTRING); 

  fdl &= new InternFuncDesc("string",      (InternalFunc)string_so, 0,
                            StringDomain,  (FuncCheckDomains) fValueDomains,
                            1, vtSTRING, vtVALUE);
  fdl &= new InternFuncDesc("string",      (InternalFunc)string_so, 0,
                            StringDomain,  (FuncCheckDomains) fCoordDomains,
                            1, vtSTRING, vtCOORD);
  fdl &= new InternFuncDesc("value",      (InternalFunc)value_so, 0,
                            RealDomain,  (FuncCheckDomains) fStringCompatibleDomains,
                            1, vtVALUE, vtSTRING);

  fdl &= new InternFuncDesc("dist",      (InternalFunc)dist_so, 0,
                            RealDomain,  (FuncCheckDomains) fCoordDomains,
                            2, vtVALUE, vtCOORD, vtCOORD);
  fdl &= new InternFuncDesc("dist2",      (InternalFunc)dist2_so, 0,
                            RealDomain,  (FuncCheckDomains) fCoordDomains,
                            2, vtVALUE, vtCOORD, vtCOORD);
  fdl &= new InternFuncDesc("crdx",      (InternalFunc)crdx_so, 0,
                            RealDomain,  (FuncCheckDomains) fCoordDomains,
                            1, vtVALUE, vtCOORD);
  fdl &= new InternFuncDesc("crdy",      (InternalFunc)crdy_so, 0,
                            RealDomain,  (FuncCheckDomains) fCoordDomains,
                            1, vtVALUE, vtCOORD);
  fdl &= new InternFuncDesc("clrred",      (InternalFunc)clrred_so, 0,
                            ImageDomain,  (FuncCheckDomains) fColorDomains,
                            1, vtCOLOR, vtCOLOR);
  fdl &= new InternFuncDesc("clrgreen",      (InternalFunc)clrgreen_so, 0,
                            ImageDomain,  (FuncCheckDomains) fColorDomains,
                            1, vtCOLOR, vtCOLOR);
  fdl &= new InternFuncDesc("clrblue",      (InternalFunc)clrblue_so, 0,
                            ImageDomain,  (FuncCheckDomains) fColorDomains,
                            1, vtCOLOR, vtCOLOR);
  fdl &= new InternFuncDesc("clryellow",      (InternalFunc)clryellow_so, 0,
                            ImageDomain,  (FuncCheckDomains) fColorDomains,
                            1, vtCOLOR, vtCOLOR);
  fdl &= new InternFuncDesc("clrmagenta",      (InternalFunc)clrmagenta_so, 0,
                            ImageDomain,  (FuncCheckDomains) fColorDomains,
                            1, vtCOLOR, vtCOLOR);
  fdl &= new InternFuncDesc("clrcyan",      (InternalFunc)clrcyan_so, 0,
                            ImageDomain,  (FuncCheckDomains) fColorDomains,
                            1, vtCOLOR, vtCOLOR);
  fdl &= new InternFuncDesc("clrgrey",      (InternalFunc)clrgrey_so, 0,
                            ImageDomain,  (FuncCheckDomains) fColorDomains,
                            1, vtCOLOR, vtCOLOR);
  fdl &= new InternFuncDesc("clrhue",      (InternalFunc)clrhue_so, 0,
                            ImageDomain,  (FuncCheckDomains) fColorDomains,
                            1, vtCOLOR, vtCOLOR);
  fdl &= new InternFuncDesc("clrsat",      (InternalFunc)clrsat_so, 0,
                            ImageDomain,  (FuncCheckDomains) fColorDomains,
                            1, vtCOLOR, vtCOLOR);
  fdl &= new InternFuncDesc("clrintens",      (InternalFunc)clrintens_so, 0,
                            ImageDomain,  (FuncCheckDomains) fColorDomains,
                            1, vtCOLOR, vtCOLOR);
}


InternFuncDesc* InternFuncDesc::fdFindFunc(const Array<InternFuncDesc*>& fdl,
                           const String& sFunc, Array<CalcVariable>& acv)
{
  int iWrongParmDummy = shUNDEF;
  int iParms = acv.iSize();
  for (int i=0; i < fdl.iSize(); i++) {
    InternFuncDesc* fd = fdl[i];
    if (0 != _stricmp(sFunc.c_str(), fd->sFunc)) // not the same name
      continue;
    if (iParms != fd->iParms()) // incorrect nr of parameters
      continue;
    if (!fdl[i]->fCheckParms(acv, iWrongParmDummy)) // wrong param types
      continue;
    return fd;
  }
  return 0;
}

bool InternFuncDesc::fCheckParms(const Array<CalcVariable>& acv, int& iWrongParm) const
{
  if (!(*fpchdm)(acv, 0, iWrongParm))
    return false;
  // Wim (18/9/97): It seems to me that the definition of vt is not good enough,
  // content and structure are trying to be stored in the same variable
  // this leads to problems.
  for (int j = 0; j < acv.iSize(); j++)
    switch (vtl[j]) {
      case vtVALUE:
      case vtSTRING:
        break; // all ok
      case vtCOLUMN:
      case vtMAP:
      case vtTABLE:
      case vtTABLE2:
        if (acv[j]->vt != vtl[j]) {
          iWrongParm = j;
          return false;
        }
      case vtCOORD:
      case vtCOLOR:
        if (acv[j]->vt != vtl[j] &&
            acv[j]->vt != vtCOLUMN &&
            acv[j]->vt != vtMAP) {
          iWrongParm = j;
          return false;
        }
    }
  return true;
}





