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
#ifndef TLSPCH_H
#define TLSPCH_H

#include <direct.h>
#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <float.h>
#include <math.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <typeinfo.h>
#include <string>
#include <functional>
#include <strstream>
#include <typeinfo>

#include "Headers\compatib.h"
#include "Headers\baseconst.h"

#ifundef 

inline char * pcYesNo(bool f) { return f ? "Yes" : "No"; }
inline long round(float x)
  {  return (long)floor(x + 0.5); }
inline long rounding(double x)
  {  return (long)floor(x + 0.5); }
inline long roundx(float x) {
    if ((x == flUNDEF) || (x > LONG_MAX) || (x < LONG_MIN))
      return iUNDEF;
    else
      return (long)floor(x + 0.5);
  }
inline long roundx(double x) {
    if ((x == rUNDEF) || (x > LONG_MAX) || (x < LONG_MIN))
      return iUNDEF;
    else
      return (long)floor(x + 0.5);
  }

inline byte   byteConv(short x)   { return x < 0 ? (byte)0 : x > 255 ? (byte)0 : (byte)x; }
inline byte   byteConv(long  x)   { return x < 0 ? (byte)0 : x > 255 ? (byte)0 : (byte)x; }
inline byte   byteConv(int x)     { return x < 0 ? (byte)0 : x > 255 ? (byte)0 : (byte)x; }
inline byte   byteConv(double x)  { return byteConv(roundx(x)); }
inline short  shortConv (long  x) { return  ((x == iUNDEF) || (x > SHRT_MAX) || (x < SHRT_MIN)) ? shUNDEF : (short)x; }
inline short  shortConv(double x) { return shortConv(roundx(x)); }
inline short  shortConv(short x)  { return x; }
inline short  shortConv(int x)    { return ((x == iUNDEF) || (x > SHRT_MAX) || (x < SHRT_MIN)) ? shUNDEF : (short)x;}
inline long   longConv(short x)   { return x <= shUNDEF ? iUNDEF : (long)x; } // Here we consider -32768 the same as SHUNDEF (-32767). Some imported maps have this value.
inline long   longConv(int x)     { return (long)x; }
inline long   longConv(float x)   { return roundx(x); }
inline long   longConv(double x)  { return roundx(x); }
inline float floatConv(short x)   { return x <= shUNDEF ? flUNDEF : (float)x; } // see above
inline float floatConv(long x)    { return x == iUNDEF ? flUNDEF : (float)x; }
inline float floatConv(double x)  { return ((x == rUNDEF) || (x < -FLT_MAX) || (x > FLT_MAX)) ? flUNDEF : (float)x; }
inline double doubleConv(short x) { return x <= shUNDEF ? rUNDEF : (double)x; } // see above
inline double doubleConv(long x)  { return x == iUNDEF ? rUNDEF : (double)x; }
inline double doubleConv(float x) { return x == flUNDEF ? rUNDEF : (double)x; }

double _export doubleConv(const char* s);// in tls/basetls.c


long _export longConv(const char* s); // in tls/basetls.c

inline double sqr(double r) { return r * r; }
inline long sqr(long x) { return x * x; }
inline long sqr(int x) { return sqr(long(x)); }

#include "Engine\Base\DataObjects\Dat2.h"

typedef Buf<RowCol>   RowColBuf;
typedef Buf<Coord>    CoordBuf;
typedef Buf<String>  StringBuf;

typedef BufExt<RowCol> RowColBufExt;
typedef BufExt<Coord>  CoordBufExt;


#endif






