//----------------------------------------------------------------------
//	File:           KMfilterCenters.h
//	Programmer:     David Mount
//	Last modified:  03/27/02
//	Description:    Include file for KMfilterCenters
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

#ifndef KM_FILTER_CENTERS_H
#define KM_FILTER_CENTERS_H

#include "KMcenters.h"			// provides KMcenters
#include "Point.h" 
#include "Image.h"
//----------------------------------------------------------------------
//  KMfilterCenters - centers with filtering for computing distortions
//  	This extends the KMcenters class, by providing a more efficient
//  	algorithm for computing distortions, by the filtering algorithm.
//  	This algorithm makes use of a data structure called a kc-tree,
//  	which is given in the file KCtree.h.  In addition to the
//  	KMcenter functions we provide the following additional
//  	functions:
//
//  	getDist()
//  	getDists()
//  		Computes the total and individual distortions,
//  		respectively, for the centers points (see definitions
//  		below).
//	/* Modified by Nargess, to pass parameter. */
//  	moveToCentroid(vector<int>& to_keep)
//  		Moves the center points to the centroids of their
//  		associated neighborhoods.
//
//  	These functions are not computed independently.  In particular,
//  	for a given set of centers, they can each be computed very
//  	efficiently (in O(k*d) time) provided that some intermediate
//  	values has already been computed.  We maintain a status variable
//  	"valid," which indicates whether these intermediate values have
//  	been computed and are current.  The intermediate values are
//  	computed by computeDistortion().
//
//  	>> If you modify this class note that any function that     <<
//  	>> modifies the center or data points must set valid=false. <<
//
//	Immediate access:
//	-----------------
//	To disable the automatic recomputation of distortions on
//	getDist() and getDists(), call them with a "false" argument.
//
//	Distortion Overview:
//  	--------------------
//  	Let C[j] denote the j-th center.  For each j in [0..k-1], define
//  	the j-th neighborhood V(j), to be the set of data points that
//  	are closer to j than to any other center.  The "j-th distortion"
//  	is defined to be the sum of squared distances of every point in
//  	V(j) to the j-th center.  The "total distortion" is the sum of
//  	the distortions over all the centers.
//
//	Intermediate Values:
//  	--------------------
//	Instead of computing distortions from scratch by brute force
//	(which would take O(n*k*d) time), we use an algorithm called the
//	filtering algorithm.  This algorithm does not compute the
//	distortion directly, but instead computes the following
//	intermediate values, from which the distortion can be computed
//	efficiently.  Let j be an index in [0..k-1].  The notation (u.v)
//	denotes the dot product of vectors u and v.
//
//  	KMpoint sums[j]		Vector sum of points in V(j)
//  	double sumsSqs[j]	Sum of (u.u) for all u in V(j)
//  	double weights[j]	Number of data points such that
//  				  this C[j] is closest
//
//  	See the function computeDistortion() and moveToCentroid() for
//  	explanations of how these quantities are combined to compute
//  	the total distortion and move centers to their centroids.
//
//	Final Values:
//  	-------------
//  	Given the above intermediate values, we then compute the
//  	following final distortion values.
//
//  	double dists[j]		Total distortion for points of V(j)
//  	double currDist		Current total distortion
//
// 	Although they are not used by this program, the center
// 	distortions are useful, because they may be used in a more
// 	general clustering algorithm to determine whether clusters
// 	should be split or merged.
//----------------------------------------------------------------------

#include <vector>
extern vector<vector<int> > clusters;

class KMfilterCenters : public KMcenters{

  friend class Image;
protected:			// intermediates
    KMpointArray	sums;		// vector sum of points
    KMpointArray        sumSqVec;	// Nargess: vector of sum of squared points. 
    KMpointArray        stdv;		// Nargess: array of standard deviation vectors. 
    double*		sumSqs;		// sum of squares
    int*		weights;	// the weight of each center
protected:			// distortion data
    double*		dists;		// individual distortions
    double		currDist;	// current total distortion
    bool		valid;		// are sums/distortions valid?
protected:			// local utilities
    void computeDistortion();		  		// compute distortions
    void moveToCentroid(vector<int>& to_keep);		// move centers to cluster centroids
    							// swap one center
    void swapOneCenter(bool allowDuplicate = true);
public:
    KMfilterCenters(int k, KMdata& p);	// standard constructor

    KMfilterCenters(int k, KMdata& p, vector<KMpoint> c);//*another constructor added by Nargess*
					// copy constructor
    KMfilterCenters(const KMfilterCenters& s);
					// assignment operator
    KMfilterCenters& operator=(const KMfilterCenters& s);

    virtual ~KMfilterCenters();		// virtual destructor

public:					// public accessors
    					// returns sums
    KMpointArray getSums(bool autoUpdate = true) {
	if (autoUpdate && !valid) computeDistortion();
	return sums;
    }
  
/* the following functions are added by Nargess */

     KMpointArray getSumSqVec(bool autoUpdate = true) {
        if (autoUpdate && !valid) computeDistortion();
        return sumSqVec;
    }

    KMpointArray getStdv(bool autoUpdate = true) {  
        if (autoUpdate && !valid) computeDistortion();
        return stdv;
    }

/* end Nargess addition		     */

    					// returns sums of squares
    double* getSumSqs(bool autoUpdate = true) {
	if (autoUpdate && !valid) computeDistortion();
	return sumSqs;
    }
    					// returns weights
    int* getWeights(bool autoUpdate = true) {
	if (autoUpdate && !valid) computeDistortion();
	return weights;
    }
					// returns total distortion
    double getDist(bool autoUpdate = true)	{
	if (autoUpdate && !valid) computeDistortion();
	return currDist;
    }
					// returns average distortion
    double getAvgDist(bool autoUpdate = true)	{
	if (autoUpdate && !valid) computeDistortion();
	return currDist/double(getNPts());
    }
					// returns individual distortions
    double* getDists(bool autoUpdate = true) {
	if (autoUpdate && !valid) computeDistortion();
	return dists;
    }



    void genRandom() {			// generate random centers
	pts->sampleCtrs(ctrs, kCtrs, false);
	valid = false;
    }
    void lloyd1Stage(vector<int>& to_keep){		// one stage of LLoyd's algorithm
	moveToCentroid(to_keep);
    }
    void swap1Stage() {			// one stage of swap heuristic
	swapOneCenter();
    }
    virtual void print();		// print centers and distortions


   /* the following functions are added by Nargess */

   void UpdateCenters(vector<int,allocator<int> > &);
   
   void ComputeDist() {
	if (!valid)
		computeDistortion();
		//cout<<"in filter compute distortions, # clusters: "<<clusters.size()<<endl;
   }
   void SetFilterCenters(const vector<IsoCluster::Point *> );
   void DeleteCenters(); 	
};
#endif

