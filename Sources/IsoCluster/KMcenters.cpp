//----------------------------------------------------------------------
//	File:           KMcenters.cc
//	Programmer:     David Mount
//	Last modified:  03/27/02
//	Description:    Functions for KMcenters
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
#include "KMcenters.h"
    					// standard constructor
KMcenters::KMcenters(int k, KMdata& p)
    : kCtrs(k), pts(&p) {
}
//constructor with all the fields passed as parameter. **added by Nargess**
KMcenters::KMcenters(int k, KMdata& p, vector<KMpoint> c):kCtrs(k),pts(&p),ctrs(c){
}
    					// copy constructor
KMcenters::KMcenters(const KMcenters& s)
    : kCtrs(s.kCtrs), pts(s.pts),ctrs(s.ctrs) {
}
    					// assignment operator
KMcenters& KMcenters::operator=(const KMcenters& s) {
    if (this != &s) {			// avoid self assignment (x=x)
					// size change?
	if (kCtrs != s.kCtrs || getDim() != s.getDim()) {
	  ctrs.clear(); 
	}
	kCtrs = s.kCtrs;
	pts = s.pts;
    }
    return *this;
}

KMcenters::~KMcenters() {		// destructor
    ctrs.clear();
}

//void KMcenters::resize(int k) {		// resize array (if needed)
//    if (k == kCtrs) return;
//    kCtrs = k;
//    kmDeallocPts(ctrs);
//    ctrs = kmAllocPts(kCtrs, pts->getDim());
//}

void KMcenters::print(			// print centers
    bool fancy) {
    kmPrintPts("Center_Points", ctrs, getK(), fancy);
}
