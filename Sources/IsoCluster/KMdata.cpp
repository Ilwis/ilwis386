//----------------------------------------------------------------------
//	File:           KMdata.cc
//	Programmer:     David Mount
//	Last modified:  03/27/02
//	Description:    Functions for KMdata
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
#include "KMdata.h"
#include "KMrand.h"			// provides kmRanInt()

Tranquilizer KMdata::trq ;
					// standard constructor
KMdata::KMdata(int d, int n) : dim(d), maxPts(n), nPts(n) {
    pts = kmAllocPts(n, d);
    kcTree = NULL;
}
KMdata:: KMdata(int d, int n, KMdataArray p):dim(d), maxPts(n), nPts(n),pts(p){

kcTree=NULL;

}

KMdata::~KMdata() {			// destructor
	trq.SetTitle("Isocluster");
	trq.SetText("Freeing memory....");
    kmDeallocPts(pts);				// deallocate point array
    delete kcTree;				// deallocate kc-tree
}

void KMdata::buildKcTree() {		// build kc-tree for points
    if (kcTree != NULL) {
 	delete kcTree;		}	// destroy existing tree
    kcTree = new KCtree(pts, nPts, dim);	// construct the tree

}

void KMdata::resize(int d, int n) {	// resize point array
    if (d != dim || n != nPts) {		// size change?
	dim = d;
	nPts = n;
	kmDeallocPts(pts);			// deallocate old points
	pts = kmAllocPts(nPts, dim);
    }
    if (kcTree != NULL) {			// kc-tree exists?
	delete kcTree;				// deallocate kc-tree
	kcTree = NULL;
    }
}

//------------------------------------------------------------------------
//  sampleCtr - Sample a center point at random.
//	Generates a randomly sampled center point.
//------------------------------------------------------------------------

void KMdata::sampleCtr(			// sample a center point
    KMcenter	sample)				// where to store sample
{
    int ri = kmRanInt(nPts);			// generate random index
    kmCopyPt(dim, pts[ri], sample);		// copy to destination
}

//------------------------------------------------------------------------
//  sampleCtrs - Sample center points at random.
//	Generates a set of center points by sampling (allowing or
//	disallowing duplicates) from this point set.  It is assumed that
//	the point storage has already been allocated.
//------------------------------------------------------------------------

void KMdata::sampleCtrs(			// sample points randomly
    vector<KMpoint>	sample,			// where to store sample
    int			k,			// number of points to sample
    bool		allowDuplicate)		// sample with replacement?
{
    if (!allowDuplicate)			// duplicates not allowed
	assert(k <= nPts);			// can't do more than nPts

    int* sampIdx = new int[k];			// allocate index array

    for (int i = 0; i < k; i++) {		// sample each point of sample
	int ri = kmRanInt(nPts);		// random index in pts
	if (!allowDuplicate) {			// duplicates not allowed?
	    bool dupFound;			// duplicate found flag
    	    do {				// repeat until successful
		dupFound = false;
		for (int j = 0; j < i; j++) { 	// search for duplicates
		    if (sampIdx[j] == ri) {	// duplicate found
			dupFound = true;
			ri = kmRanInt(nPts);	// try again
			break;
		    }
	    	}
	    } while (dupFound);
	}
	kmCopyPt(dim, pts[ri], sample[i]);	// copy sample point
	sampIdx[i] = ri;			// save index
    }
    delete [] sampIdx;
}
