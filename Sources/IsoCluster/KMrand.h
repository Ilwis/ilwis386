//----------------------------------------------------------------------
//	File:		KMrand.h
//	Programmer:	Sunil Arya and David Mount
//	Last modified:	03/27/02
//	Description:	Basic include file for random point generators
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

#ifndef KM_RAND_H
#define KM_RAND_H

//----------------------------------------------------------------------
//  Basic includes
//----------------------------------------------------------------------
#include <math.h>			// math routines
#include "KMeans.h"			// KMeans includes

//----------------------------------------------------------------------
//  Globals
//----------------------------------------------------------------------
extern	int	kmIdum;			// used for random number generation

//----------------------------------------------------------------------
//  External entry points
//----------------------------------------------------------------------

int kmRanInt(			// random integer
	int		n);		// in the range [0,n-1]

double kmRanUnif(		// random uniform in [lo,hi]
	double		lo = 0.0,
	double		hi = 1.0);

void kmUniformPts(		// uniform distribution
	KMpointArray	pa,		// point array (modified)
	int		n,		// number of points
	int		dim);		// dimension

void kmGaussPts(			// Gaussian distribution
	KMpointArray	pa,		// point array (modified)
	int		n,		// number of points
	int		dim,		// dimension
	double		std_dev);	// standard deviation

void kmCoGaussPts(		// correlated-Gaussian distribution
	KMpointArray	pa,		// point array (modified)
	int		n,		// number of points
	int		dim,		// dimension
	double		correlation);	// correlation

void kmLaplacePts(		// Laplacian distribution
	KMpointArray	pa,		// point array (modified)
	int		n,		// number of points
	int		dim);		// dimension

void kmCoLaplacePts(		// correlated-Laplacian distribution
	KMpointArray	pa,		// point array (modified)
	int		n,		// number of points
	int		dim,		// dimension
	double		correlation);	// correlation

void kmClusGaussPts(		// clustered-Gaussian distribution
	KMpointArray	pa,		// point array (modified)
	int		n,		// number of points
	int		dim,		// dimension
	int		n_col,		// number of colors (clusters)
	bool		new_clust,	// generate new cluster centers
	double		std_dev,	// standard deviation within clusters
	double		&clus_sep);	// cluster separation (returned)

void kmClusGaussPts2(		// clustered-Gaussian distribution
	KMpointArray	pa,	       // point array (modified)
	int		n,	       // number of points
	int		dim,	       // dimension
	int		n_col,	       // number of colors (clusters)
	bool		new_clust,     // generate new cluster centers
	double		std_m,	       // mean of standard deviation within clusters
	double		std_std);      // standard deviation of standard deviations within clusters



KMpointArray kmGetCGclusters(); // get clustered-gauss cluster centers

void kmClusOrthFlats(           // clustered along orthogonal flats
	KMpointArray	pa,		// point array (modified)
	int		n,		// number of points
	int		dim,		// dimension
	int		n_col,		// number of colors
	bool		new_clust,	// generate new clusters.
	double		std_dev,	// standard deviation within clusters
	int		max_dim);	// maximum dimension of the flats

void kmClusEllipsoids(		// clustered around ellipsoids
	KMpointArray	pa,		// point array (modified)
	int		n,		// number of points
	int		dim,		// dimension
	int		n_col,		// number of colors
	bool		new_clust,	// generate new clusters.
	double		std_dev_small,	// small standard deviation
	double		std_dev_lo,	// low standard deviation for ellipses
	double		std_dev_hi,	// high standard deviation for ellipses
	int		max_dim);	// maximum dimension of the flats

void kmMultiClus(		// multi-sized clusters
	KMpointArray	pa,		// point array (modified)
	int		n,		// number of points
	int		dim,		// dimension
	int		&k,		// number of clusters (returned)
	double		base_dev);	// base standard deviation

#endif
