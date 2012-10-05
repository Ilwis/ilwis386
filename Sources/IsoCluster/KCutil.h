//----------------------------------------------------------------------
//	File:		KCutil.h
//	Programmer:	David Mount
//	Last modified:	03/27/02
//	Description:	Declarations for kc-tree utilities
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

#ifndef KC_UTIL_H
#define KC_UTIL_H

#include "KCtree.h"			// kc-tree declarations

//----------------------------------------------------------------------
//  externally accessible functions
//----------------------------------------------------------------------

void kmEnclRect(		// compute smallest enclosing rectangle
    KMpointArray	pa,		// point array
    KMidxArray		pidx,		// point indices
    int			n,		// number of points
    int			dim,		// dimension
    KMorthRect	&bnds);			// bounding cube (returned)

KMcoord kmSpread(		// compute point spread along dimension
    KMpointArray	pa,		// point array
    KMidxArray		pidx,		// point indices
    int			n,		// number of points
    int			d);		// dimension to check

void kmMinMax(			// compute min and max coordinates along dim
    KMpointArray	pa,		// point array
    KMidxArray		pidx,		// point indices
    int			n,		// number of points
    int			d,		// dimension to check
    KMcoord&		min,		// minimum value (returned)
    KMcoord&		max);		// maximum value (returned)

void kmPlaneSplit(		// split points by a plane
    KMpointArray	pa,		// points to split
    KMidxArray		pidx,		// point indices
    int			n,		// number of points
    int			d,		// dimension along which to split
    KMcoord		cv,		// cutting value
    int			&br1,		// first break (values < cv)
    int			&br2);		// second break (values == cv)

void sl_midpt_split(			// sliding midpoint kd-splitter
    KMpointArray	pa,		// point array (unaltered)
    KMidxArray		pidx,		// point indices (permuted on return)
    const KMorthRect	&bnds,		// bounding rectangle for cell
    int			n,		// number of points
    int			dim,		// dimension of space
    int			&cut_dim,	// cutting dimension (returned)
    KMcoord		&cut_val,	// cutting value (returned)
    int			&n_lo);		// num of points on low side (returned)

#endif
