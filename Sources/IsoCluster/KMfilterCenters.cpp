//----------------------------------------------------------------------
//	File:           KMfilterCenters.cc
//	Programmer:     David Mount
//	Last modified:  03/27/02
//	Description:    Member functions for KMfilterCenters
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
#include "KMfilterCenters.h"
#include "KMrand.h"


// standard constructor
KMfilterCenters::KMfilterCenters(int k, KMdata& p)
: KMcenters(k, p) {
	if (p.getKcTree() == NULL) {	// kc-tree not yet built?
		//  kmError("Building kc-tree", KMwarn);
		p.buildKcTree();			// build it now
	}
	sums	= kmAllocPts(kCtrs, getDim());
	sumSqVec    = kmAllocPts(kCtrs, getDim()); // Nargess
	stdv	= kmAllocPts(kCtrs,getDim());  // Nargess	
	sumSqs	= new double[kCtrs];
	weights	= new int[kCtrs];
	dists	= new double[kCtrs];
	currDist	= KM_HUGE;
	valid	= false;
}
/********************************************************************************************/
/* another constructor, added by Nargess *, the only difference with previous constructor is*/
/* in passing the centers' array as parameter.						    */
/********************************************************************************************/
KMfilterCenters::KMfilterCenters(int k, KMdata& p, vector<KMpoint> c)
: KMcenters(k, p, c) {
	valid=false;
	if (p.getKcTree() == NULL) {        // kc-tree not yet built?
		//kmError("Building kc-tree", KMwarn);	
		p.buildKcTree();                  // build it now  
	}	
	sums        = kmAllocPts(kCtrs, getDim());
	sumSqVec    = kmAllocPts(kCtrs, getDim());  // Nargess
	stdv	= kmAllocPts(kCtrs, getDim());  // Nargess	
	sumSqs      = new double[kCtrs];
	weights     = new int[kCtrs];
	dists       = new double[kCtrs];
	currDist    = KM_HUGE;
	valid       = false;
}


// copy constructor
KMfilterCenters::KMfilterCenters(const KMfilterCenters& s)
: KMcenters(s) {
	sums	= kmAllocCopyPts(kCtrs, getDim(), s.sums);
	sumSqVec    = kmAllocCopyPts(kCtrs, getDim(), s.sumSqVec);	//Nargess
	stdv	= kmAllocCopyPts(kCtrs, getDim(), s.stdv);	//Nargess
	sumSqs	= kmAllocCopy(kCtrs, s.sumSqs);
	weights	= kmAllocCopy(kCtrs, s.weights);
	dists	= kmAllocCopy(kCtrs, s.dists);
	currDist	= s.currDist;
	valid	= s.valid;
}
// assignment operator
KMfilterCenters& KMfilterCenters::operator=(const KMfilterCenters& s) {
	if (this != &s) {			// avoid self copy (x=x)
		// different sizes?
		if (kCtrs != s.kCtrs || getDim() != s.getDim()) {
			kmDeallocPts(sums);		// deallocate old storage
			kmDeallocPts(sumSqVec);	//Nargess
			kmDeallocPts(stdv);		//Nargess

			delete [] sumSqs;
			delete [] weights;
			delete [] dists;
			// allocate new storage
			sums      = kmAllocPts(s.kCtrs, s.getDim());
			sumSqVec  = kmAllocPts(s.kCtrs, s.getDim());	//Nargess
			stdv      = kmAllocPts(s.kCtrs, s.getDim());	//Nargess
			sumSqs    = new double[s.kCtrs];
			weights   = new int[s.kCtrs];
			dists     = new double[s.kCtrs];
		}
		KMcenters& base = *this;	
		base.operator=(s);		// copy base class
		// copy array contents
		kmCopyPts(kCtrs, getDim(), s.sums, sums);
		kmCopyPts(kCtrs, getDim(), s.sumSqVec, sumSqVec);    //Nargess
		kmCopyPts(kCtrs, getDim(), s.stdv, stdv);    //Nargess
		kmCopy(kCtrs, s.sumSqs, sumSqs);
		kmCopy(kCtrs, s.weights, weights);
		kmCopy(kCtrs, s.dists, dists);
		valid   = s.valid;
	}
	currDist = s.currDist;
	return *this;
}
// virtual destructor
KMfilterCenters::~KMfilterCenters() {
	kmDeallocPts(sums);
	kmDeallocPts(sumSqVec);
	kmDeallocPts(stdv);

	DeleteCenters();    // this line added by Nargess	
	delete [] sumSqs;
	delete [] weights;
	delete [] dists;
}
//-------------- The following is added by Nargess: -------------------------------
// Deletes the current set of centers.
//---------------------------------------------------------------------------------
void KMfilterCenters::DeleteCenters(){


	if (ctrs.size()!=0)
	{
		for (int i=0; i <kCtrs; i++)
		{

			if (ctrs[i]!=NULL)
				delete [] ctrs[i];
		}

		ctrs.clear();
	}
	kCtrs=0;
	valid=false;


}
//---------------end of addition by Nargess----------------------------
//----------------------------------------------------------------------
//  computeDistortion - compute total and individual distortions
//	This procedure computes the total and individual distortions for
//	a set of center points.  It invokes getNeighbors() on the
//	kc-tree for the point set,which computes the values of weights,
//	sums, and sumSqs, from which the distortion is computed as
//	follows.
//
//	Distortion Computation:
//	-----------------------
//	Assume that some center has been fixed (indexed by j in the code
//	below).  Let SUM_i denote a summation over all (wgt[j])
//	neighbors of the given center.  The data points (p[i]) and
//	center points (c[j]) are vectors, and the product of two vectors
//	means the dot product (u*v = (u.v), u^2 = (u.u)).  The
//	distortion for a single center j, denoted dists[j], is defined

//	to be the sum of squared distances from each point to its
//	closest center,  That is:
//
//	    dists[j] = SUM_i (p[i] - c[j])^2
//		= SUM_i (p[i]^2 - 2*c[j]*p[i] + c[j]^2)
//		= SUM_i p[i]^2 - 2*c[j]*SUM_i p[i] + wgt[j]*c[j]^2
//		= sumSqs[j] - 2*(c[j].sums[j]) + wgt[j]*(c[j]^2)
//
//	Thus the individual distortion can be computed from these
//	quantities.  The total distortion is the sum of the individual
//	distortions.
//----------------------------------------------------------------------

void KMfilterCenters::computeDistortion() // compute distortions
{

	KCtree* t = getData().getKcTree();
	assert(t != NULL);				// tree better exist
	if (kCtrs > 0)
		t->getNeighbors(*this);			// get neighbors

	double totDist = 0;
	double local_cDotS=0,local_cDotC=0;           //Nargess added

	for (int j = 0; j < kCtrs; j++) {
		double cDotC = 0;			// init: (c[j] . c[j])
		double cDotS = 0;			// init: (c[j] . sum[j])


		for (int d = 0; d < getDim(); d++) {	// compute dot products

			/* this section is modified by Nargess to calculate Stdv vector */
			local_cDotC=ctrs[j][d] * ctrs[j][d];
			local_cDotS=ctrs[j][d] * sums[j][d];	
			cDotC += local_cDotC;
			cDotS += local_cDotS;

			stdv[j][d]=sqrt((sumSqVec[j][d]-2*local_cDotS+weights[j]*local_cDotC)/weights[j]);
		}
		// final distortion
		dists[j] = sumSqs[j] - 2*cDotS + weights[j]*cDotC;
		totDist += dists[j];
	}
	currDist = totDist;				// save total distortion

	valid = true;

}

//----------------------------------------------------------------------
//  moveToCentroid
//	This procedure moves each center point to the centroid of its
//	associated cluster.  We call getNeighbors() if necessary to
//	compute the weights and sums.  The centroid is the weighted
//	average of the sum of neighbors.  Thus the 
//
//	    ctrs[j][d] = sums[j][d] / weights[j].
//
//----------------------------------------------------------------------
// move center to cluster centroid
void KMfilterCenters::moveToCentroid(vector<int>& to_keep)
{
	// Nargess: deleted the next line of code for IsoClus. since steps 2 
	// to 4 are in a loop, and we will calculate distortions in step 2 anyways.
	// ie, we need to get the mean of the previous iterations clusters,
	// and then distribute the points (calculate distortions) with
	// regard to the new centers.
	//if (!valid) computeDistortion();		// compute sums if needed

	for (int j = 0; j < kCtrs; j++) {
		int wgt = weights[to_keep[j]];			// weight of this center
		if (wgt > 0) {				// update only if weight > 0
			for (int d = 0; d < getDim(); d++) {
				ctrs[j][d] = sums[to_keep[j]][d]/wgt;
			}
		}
	}
	valid = false;				// distortions now invalid
}

//----------------------------------------------------------------------
//  swapOneCenter
//	Swaps one center point with a sample point.  Optionally we make
//	sure that the new point is not a duplicate of any of the centers
//	(including the point being replaced).
//----------------------------------------------------------------------
void KMfilterCenters::swapOneCenter(		// swap one center
									bool allowDuplicate)			// allow duplicate centers
{
	int rj = kmRanInt(kCtrs);			// index of center to replace
	int dim = getDim();
	KMpoint p = kmAllocPt(dim);			// alloc replacement point
	pts->sampleCtr(p);				// sample a replacement
	if (!allowDuplicate) {			// duplicates not allowed?
		bool dupFound;				// was a duplicate found?
		do {					// repeat until successful
			dupFound = false;
			for (int j = 0; j < kCtrs; j++) { 	// search for duplicates
				if (kmEqualPts(dim, p, ctrs[j])) {
					dupFound = true;
					pts->sampleCtr(p);		// try again
					break;
				}
			}
		} while (dupFound);
	}
	kmCopyPt(dim, p, ctrs[rj]);			// copy sampled point
	if (kmStatLev >= ISOTRACE) {                   // output swap info
		*kmOut << "\tswapping: ";
		kmPrintPt(p, getDim(), true);
		*kmOut << "<-->Center[" << rj << "]\n";
	}
	kmDeallocPt(p);				// deallocate point storage
	valid = false;
}

//----------------------------------------------------------------------
//  print centers and distortions
//----------------------------------------------------------------------

void KMfilterCenters::print()	{	// print centers and distortion

	*kmOut<<"Printing centers and distortions\n";

	for (int j = 0; j < kCtrs; j++) {
		*kmOut << "    " << setw(4) << j << "\t";
		kmPrintPt(ctrs[j], getDim(), true);
		*kmOut << " dist = " << setw(8) << dists[j] << endl;
	}
}
//----------------------------------------------------------------------
// Nargess: UpdateCenters: if any center was deleted, update the 'centers' 
// array to keep only those centers whose indices appear in 'to_keep',
// and set valid to false.
//------------------------------------------------------------------------
void KMfilterCenters::UpdateCenters(vector<int>& to_keep)
{

	int num=to_keep.size(), i;

	vector<KMpoint> new_ctrs;

	int old=ctrs.size(), j=0;
	for (i=0; i < old; i++)
	{
		if (j < to_keep.size() && i==to_keep[j])
		{	
			KMpoint P=new KMcoord[getDim()];
			kmCopyPt(getDim(), ctrs[i], P);
			new_ctrs.push_back(P);
			j++;
		}
	}

	DeleteCenters();

	for (i=0; i< num; i++)
		ctrs.push_back(new_ctrs[i]);

	kCtrs=num;
	valid=false;

	to_keep.clear();

}
/************************************************************************************/
/* Delets the current 'centers' vector, and initialize it to have KMpoints with the */
/* same coordinates as Points that elements in the passed vector parameter point to.*/
/* If number of centers has changed we need to deallocate/allocate other arrays     */
/* (sums, sumSqVec, etc) to account for this change.				    */
/************************************************************************************/
void KMfilterCenters::SetFilterCenters(const vector<IsoCluster::Point*> centers)
{

	int  c=centers.size();

	int i, old_kCtrs;
	KMpoint P;//=new KMcoord[getDim()];


	//old_kCtrs=kCtrs;
	old_kCtrs=ctrs.size();
	DeleteCenters();


	for (i=0; i < c; i++)
	{

		P=kmAllocCopyPt(getDim(), centers[i]->getPoint());
		ctrs.push_back(P);

	}

	kCtrs=c;	
	valid       = false;

	if (old_kCtrs!=c)
	{

		kmDeallocPts(sums);
		kmDeallocPts(sumSqVec);
		kmDeallocPts(stdv);


		delete [] sumSqs;


		delete [] weights;
		delete [] dists;
		sums        = kmAllocPts(kCtrs, getDim());
		sumSqVec    = kmAllocPts(kCtrs, getDim()); 
		stdv        = kmAllocPts(kCtrs,getDim());  
		sumSqs      = new double[kCtrs];
		weights     = new int[kCtrs];
		dists       = new double[kCtrs];
		currDist    = KM_HUGE;

	}

}








































