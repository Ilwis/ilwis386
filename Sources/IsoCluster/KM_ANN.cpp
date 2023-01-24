//----------------------------------------------------------------------
//      File:           KM_ANN.cc
//      Programmer:     David Mount
//      Last modified:  03/27/02
//      Description:    Utilities from ANN
//----------------------------------------------------------------------
// Copyright (c) 2002 University of Maryland and David Mount.  All
// Rights Reserved.
//
// Permission to use, copy, and distribute this software and its
// documentation is hereby granted free of charge, provided that
// (1) it is not a component of a commercial product, and
// (2) this notice appears in all copies of the software and
//     related documentation.
//
// The University of Maryland (U.M.) and the authors make no represen-
// tations about the suitability or fitness of this software for any
// purpose.  It is provided "as is" without express or implied warranty.
//----------------------------------------------------------------------
#include "Headers\toolspch.h"
#include "Engine\Map\Raster\MapList\maplist.h"
#include "KM_ANN.h"			// KM-ANN includes
#include "KMrand.h"			// random number includes

//----------------------------------------------------------------------
//  Point methods
//----------------------------------------------------------------------

//----------------------------------------------------------------------
//  Distance utility.
//	(Note: In the nearest neighbor search, most distances are
//	computed using partial distance calculations, not this
//	procedure.)
//----------------------------------------------------------------------

KMdist kmDist(			// interpoint squared distance
    int			dim,
    KMpoint		p,
    KMpoint		q)
{
    register int d;
    register KMcoord diff;
    register KMcoord dist;

    dist = 0;
    for (d = 0; d < dim; d++) {
	diff = p[d] - q[d];
	dist = KM_SUM(dist, KM_POW(diff));
    }
    return dist;
}

//----------------------------------------------------------------------
// kmEqualPts - test two points for equality
//----------------------------------------------------------------------
bool kmEqualPts(			// are two points equal?
    int			dim,			// dimension
    KMpoint		p1,			// the points
    KMpoint		p2)
{
    for (int d = 0; d < dim; d++) {
	if (p1[d] != p2[d]) return false;
    }
    return true;
}

//----------------------------------------------------------------------
//  Point allocation/deallocation/copying:
//----------------------------------------------------------------------
KMpoint kmAllocPt(int dim, KMcoord c)	// allocate a point
{
    KMpoint p = new KMcoord[dim];
    if (!p)
	kmError("Memory Allocation failed in 'kmAllocPt' for 'p'", KMabort);

    for (int i = 0; i < dim; i++) p[i] = c;
    return p;
}
void kmDeallocPt(KMpoint &p)		// deallocate one point
{
    delete [] p;
    p = NULL;
}

KMpointArray kmAllocPts(int n, int dim)	// allocate n pts in dim
{
	if (n > 0) {
		KMpointArray pa = new KMpoint[n];		// allocate points
		/* Nargess: added  memory allocation check */
		if (!pa)
			kmError("Memory Allocation failed in 'kmAllocPts' for 'pa' ", KMabort);

		KMpoint	  p  = new KMcoord[n*dim];	// allocate space for coords

		/* Nargess: added  memory allocation check */
		if (!p)
			kmError("Memory Allocation failed in 'kmAllocPts' for 'p'", KMabort);

		
		for (int i = 0; i < n; i++) {
		pa[i] = &(p[i*dim]);
		}

		return pa;
	} else
		return 0;
}

void kmDeallocPts(KMpointArray &pa)	// deallocate points
{
	if (pa != 0) {
		delete [] pa[0];				// dealloc coordinate storage
		delete [] pa;				// dealloc points
		pa = NULL;
	}
}
   
//----------------------------------------------------------------------
//  Point and other type copying:
//----------------------------------------------------------------------

KMpoint kmAllocCopyPt(			// allocate and copy point
    int			dim,
    const KMpoint	source)
{
    KMpoint p = new KMcoord[dim];
    for (int i = 0; i < dim; i++) p[i] = source[i];
    return p;
}

void kmCopyPt(				// copy point w/o allocation
    int			dim,
    const KMpoint	source,
    KMpoint		dest)
{
    for (int i = 0; i < dim; i++) dest[i] = source[i];
}

void kmCopyPts(				// copy point array w/o allocation
    int			n,			// number of points
    int			dim,			// dimension
    const KMpointArray	source,			// source point
    KMpointArray	dest)			// destination point
{
    for (int j = 0; j < n; j++) {
        for (int i = 0; i < dim; i++) {
	    dest[j][i] = source[j][i];
	}
    }
}

KMpointArray kmAllocCopyPts(		// allocate and copy point array
    int			n,			// number of points
    int			dim,			// dimension
    const KMpointArray	source)			// source point
{
    KMpointArray dest = kmAllocPts(n, dim);
    for (int j = 0; j < n; j++) {
        for (int i = 0; i < dim; i++) {
	    dest[j][i] = source[j][i];
	}
    }
    return dest;
}

//----------------------------------------------------------------------
//  Methods for orthogonal rectangles:
//	kmAssignRect() assigns the coordinates of one rectangle to
//	another.  The two rectangles must have the same dimension
//	(and it is not possible to test this here).
//
//	inside() returns true if a point lies inside the (closed)
//	rectangle and false otherwise.
//	
//	expand(d,x,r) expands this rectangle by a factor of x, centrally
//	about its origin and stores the resulting rectangle in r.
//----------------------------------------------------------------------
   
						// assign one rect to another
void kmAssignRect(int dim, KMorthRect &dest, const KMorthRect &source)
{
    for (int i = 0; i < dim; i++) {
	dest.lo[i] = source.lo[i];
	dest.hi[i] = source.hi[i];
    }
}
						// is point inside rectangle?
bool KMorthRect::inside(int dim, KMpoint p)
{
    for (int i = 0; i < dim; i++) {
	if (p[i] < lo[i] || p[i] > hi[i]) return false;
    }
    return true;
}
    						// expand by factor x
void KMorthRect::expand(int dim, double x, KMorthRect r)
{
    for (int i = 0; i < dim; i++) {
	KMcoord wid = hi[i] - lo[i];
	r.lo[i] = lo[i] - (wid/2)*(x - 1);
	r.hi[i] = hi[i] + (wid/2)*(x - 1);
    }
}
    						// sample uniformly
void KMorthRect::sample(int dim, KMpoint p)
{
    for (int i = 0; i < dim; i++)
	p[i] = kmRanUnif(lo[i], hi[i]);
}



















