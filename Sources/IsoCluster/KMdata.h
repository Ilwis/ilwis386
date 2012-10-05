//----------------------------------------------------------------------
//	File:           KMData.h
//	Programmer:     David Mount
//	Last modified:  03/27/02
//	Description:    Include file for KMdata
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

#ifndef KM_DATA_H
#define KM_DATA_H

#include "KMeans.h"			// kmeans includes
#include "KCtree.h"			// kc-tree includes

//----------------------------------------------------------------------
//  KMdata - data point set
//  	This object represents a set of data points in d-space.  The
//  	array can be resized.  Doing so destroys the existing contents.
// 
// 	In addition to the points, we also (optionally) provide a
// 	kc-tree data structure for the points as well.  This is
// 	constructed by first initializing the points and then calling
// 	buildKcTree().
//
// 	We support a virtual function samplePt and samplePts, which
// 	sample one or a set of random center points.  In this version,
// 	the sample is just a random sample of the point set.  However,
// 	it is possible to derive classes from this in which sampling is
// 	done by some more sophisticated method.
//
// 	Note that this structure does not support copying or
// 	assignments.  If you want to resuse the structure, the only way
// 	to do so is to first apply resize(), which destroys the kc-tree
// 	(if it exists), and then assign to it a new set of points.
//----------------------------------------------------------------------

class KMdata {
private:
    int			dim;		// dimension
    int			maxPts;		// max number of points
    int			nPts;		// number of data points
//    KMdataArray		pts;		// the data points
    KCtree*		kcTree;		// kc-tree for the points
private:				// copy functions (not implemented)
    KMdata(const KMdata& p)		// copy constructor
      { assert(false); }
    KMdata& operator=(const KMdata& p)	// assignment operator
      { assert(false);  return *this; }
public:
	static Tranquilizer trq;
    KMdataArray               pts;            // the data points
    KMdata(int d, int n);		// standard constructor

    KMdata(int d, int n, KMdataArray p); //another constructor, * added by Nargess *	
    int getDim() const {		// get dimension
	return dim;
    }
    int getNPts() const {		// get number of points
	return nPts;
    }
    KMdataArray getPts() const {	// get the points
	return pts;
    }
    KCtree* getKcTree() const {		// get kc-tree
	return kcTree;
    }
    KMdataPoint& operator[](int i) {	// index
	return pts[i];
    }
    const KMdataPoint& operator[](int i) const {
	return pts[i];
    }
    void setNPts(int n) {		// set number of points
	assert(n <= maxPts);		// can't be more than array size
	nPts = n;
    }
    void buildKcTree();			// build the kc-tree for points

    virtual void sampleCtr(		// sample a center point
	KMpoint		sample);		// where to store sample

    virtual void sampleCtrs(		// sample center points
	vector<KMpoint>	sample,			// where to store sample
	int		k,			// number of points to sample
	bool		allowDuplicate);	// allowing duplicates?

    void resize(int d, int n);		// resize array

    void print(				// print data points
    	bool		fancy = true) {		// nicely formatted?
	kmPrintPts("Data_Points", pts, nPts, dim, fancy);
    }

    virtual ~KMdata();			// destructor
};
#endif
