#pragma once
 
typedef signed char			sbyte;
typedef unsigned char		ubyte;
typedef signed short		sword;
typedef unsigned short		uword;
typedef signed int			sdword;
typedef unsigned int		udword;
typedef signed __int64		sqword;
typedef unsigned __int64	uqword;
typedef float				sfloat;

#define	null	NULL
#define RELEASE(x)		{ if (x != null) delete x;		x = null; }
#define RELEASEARRAY(x)	{ if (x != null) delete []x;	x = null; }

#include "RevisitedRadix.h"
#include "CustomArray.h"
#include "Adjacency.h"
#include "Striper.h"