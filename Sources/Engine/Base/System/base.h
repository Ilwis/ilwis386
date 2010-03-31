/* $Log: /ILWIS 3.0/AdditionalHeaders/Shared/base.h $
 * 
 * 9     10/19/01 9:31a Martin
 * 
 * 8     20/10/00 14:42 Willem
 * Removed the double #define MAX_SEGMENTS outof base.h and tlspch.h and
 * moved it to baseconst.h. Also increased the constant from 32766 to
 * LONG_MAX -1
 * 
 * 7     6-10-99 18:22 Koolhoven
 * Removed #pragma hdrstop
 * This probably prevented the proper working of the precompiled header
 * files
 * 
 * 6     6-10-99 1:01p Martin
 * inline functions for min/max with undefs
 * 
 * 5     23-09-99 8:10a Martin
 * Added rMIN/rMAX functions for undef values
 * 
 * 4     22-04-99 10:45 Koolhoven
 * Header comments
 * 
 * 3     22-04-99 10:30 Koolhoven
 * Added sign()
// Revision 1.3  1998/03/20 14:37:40  Willem
// Added section for DAT interface constants.
// It contains the maximum number of segments ILWIS can currently store.
//
// Revision 1.2  1997/08/05 13:20:09  Wim
// Split some inline functions over multiple lines
//
/* General used types and routines, missing in standard libraries
// as well as including of all general include files
// 12/1/94, abs(), min(), max() now defined with templates - Wim
// 12/1/94, bool defined as class - jelle
   19/5/95, undef template function - wim
	 $Header: /ILWIS 3.0/AdditionalHeaders/Shared/base.h 9     10/19/01 9:31a Martin $
*/
#ifndef ILWBASE_H
#define ILWBASE_H

#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <float.h>
#include <math.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <direct.h>
#include <typeinfo.h>
#include <io.h>
#include <fstream>

#pragma warning( disable : 4786 )
#pragma warning ( disable : 4018)

#include <list>
#include <string>
#include <map>
#include "Headers\compatib.h "
#include "Headers\baseconst.h"
#include "Headers\stdafx.h"

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
inline long   longConv(short x)   { return x == shUNDEF ? iUNDEF : (long)x; }
inline long   longConv(int x)     { return (long)x; }
inline long   longConv(float x)   { return roundx(x); }
inline long   longConv(double x)  { return roundx(x); }
inline float floatConv(short x)   { return x == shUNDEF ? flUNDEF : (float)x; }
inline float floatConv(long x)    { return x == iUNDEF ? flUNDEF : (float)x; }
inline float floatConv(double x)  { return ((x == rUNDEF) || (x < -FLT_MAX) || (x > FLT_MAX)) ? flUNDEF : (float)x; }
inline double doubleConv(short x) { return x == shUNDEF ? rUNDEF : (double)x; }
inline double doubleConv(long x)  { return x == iUNDEF ? rUNDEF : (double)x; }
inline double doubleConv(float x) { return x == flUNDEF ? rUNDEF : (double)x; }

double _export doubleConv(const char* s);// in tls/basetls.c


long _export longConv(const char* s); // in tls/basetls.c

/*inline double sqr(double r) { return r * r; }
inline double abs(double r) { return r > 0 ? r : -r; }
inline long sqr(long x) { return x * x; }
inline long abs(long x) { return x > 0 ? x : -x; }
inline long sqr(int x) { return sqr(long(x)); }*/

#define SOURCE_LOCATION1 (NULL)
#define SOURCE_LOCATION2 (getEngine()->getDebugMode(typeid(this)) ? String("%s::%s line %d",typeid(this).name(),__FUNCTION__,__LINE__).scVal() : NULL)
#define SOURCE_LOCATION3 (getEngine()->getDebugMode() ? String("%s::%s line %d",typeid(this).name(),__FUNCTION__,__LINE__).scVal() : NULL)

#define sqr(x) ((x) * (x))

inline int sign(double x) { return x > 0 ? 1 : x < 0 ? -1 : 0; }

inline double rMIN(double a, double b) { return ((a<=b && a!=rUNDEF) || b==rUNDEF) ? a : b; }
inline double rMAX(double a, double b) { return (a>=b && a!=rUNDEF) ? a : b; }

inline double iMIN(int a, int b) { return ((a<=b && a!=iUNDEF) || b==iUNDEF) ? a : b; }
inline double iMAX(int a, int b) { return (a>=b && a!=iUNDEF) ? a : b; }
inline double rDEGRAD(double rDegrees) { return rDegrees != rUNDEF ? rDegrees * M_PI / 180.0 : rUNDEF; }
inline double rRADDEG(double rRad) { return rRad != rUNDEF ? rRad * 180.0 / M_PI : rUNDEF; }

struct MemoryStruct {
   MemoryStruct() { size = 0; memory = NULL; }
   unsigned char *memory;
   size_t size;
};

//#pragma hdrstop 
#endif // ILWBASE_H





