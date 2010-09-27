#ifndef BASECONST_H
#define BASECONST_H

#include <limits.h>


typedef unsigned short word;
typedef unsigned char byte;
typedef int etype;

typedef byte*	pbyte;
typedef char*	pchar;
typedef short*	pshort;
typedef long*	plong;
typedef double* pdouble;

#define M_E         2.71828182845904523536
#define M_LOG2E     1.44269504088896340736
#define M_LOG10E    0.434294481903251827651
#define M_LN2       0.693147180559945309417
#define M_LN10      2.30258509299404568402
#define M_PI        3.14159265358979323846
#define M_PI_2      1.57079632679489661923
#define M_PI_4      0.785398163397448309616
#define M_1_PI      0.318309886183790671538
#define M_2_PI      0.636619772367581343076
#define M_1_SQRTPI  0.564189583547756286948
#define M_2_SQRTPI  1.12837916709551257390
#define M_SQRT2     1.41421356237309504880
#define M_SQRT_2    0.707106781186547524401

class String;
struct Coord;
struct RowCol;

const short  shUNDEF = ((short)-SHRT_MAX);
const long   iUNDEF  = ((long)-LONG_MAX);
#define flUNDEF ((float)-1e38)

#define rUNDEF	((double)-1e308)
#define sUNDEF "?"
#define crdUNDEF Coord(rUNDEF, rUNDEF)
#define rcUNDEF   RowCol(iUNDEF, iUNDEF)
#define tUNDEF ILWIS::Time(0)


template <class T> inline T* undef(const T*) { return 0; }
inline char   undef(char  ) { return 0; }
inline byte   undef(byte  ) { return 0; }
inline short  undef(short ) { return shUNDEF;  }
inline int    undef(int   ) { return shUNDEF;  }
inline long   undef(long  ) { return iUNDEF;   }
inline float  undef(float ) { return flUNDEF;  }
inline double undef(double) { return rUNDEF;   }
inline bool   undef(bool)   { return false;   }
inline const char* undef(const String&) { return sUNDEF; }

#define MAX_SEGMENTS ((long)LONG_MAX - 1)

#endif




