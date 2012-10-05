//----------------------------------------------------------------------
//	File:           KMCenters.h
//	Programmer:     David Mount
//	Last modified:  03/27/02
//	Description:    Include file for KMcenters
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

#ifndef KM_CENTERS_H
#define KM_CENTERS_H

#include "KMeans.h"			// kmeans includes
#include "KMdata.h"			// provides KMdata

//----------------------------------------------------------------------
//  KMcenters - set of centers
//	This object encodes the information needed for describing a set
//	of centers.  It also stores a pointer to the data set.
//
//	When copying this object, we allocate new storage for the center
//	points, but we just copy the pointer to the data set.
//----------------------------------------------------------------------

class KMcenters {
protected:
    int			kCtrs;		// number of centers
    KMdata*		pts;		// the data points
    vector<KMpoint>	ctrs;		// the centers,  modified by Nargess to
					// vector<KMpoint> type from kmPointArray
public:					// constructors, etc.
    KMcenters(int k, KMdata& p);	// standard constructor
    KMcenters(int k, KMdata& p, vector<KMpoint> c); //constructor *added by Nargess * 
    KMcenters(const KMcenters& s);	// copy constructor
					// assignment operator
    KMcenters& operator=(const KMcenters& s);
    virtual ~KMcenters();		// virtual destructor
public:					// accessors
    int getDim() const {		// get dimension
	return pts->getDim();
    }
    int getNPts() const {		// get number of points
	return pts->getNPts();
    }
    int getK() const {			// get number of centers
	return kCtrs;
    }
    KMdata& getData() {			// get the data point structure
	return *pts;
    }
    KMdata* getDataPtr(){		// get the pointer to data point structure
	return pts;
    }	
    KMpointArray getDataPts() const {	// get the data point array
	return pts->getPts();
    }
    vector<KMpoint> getCtrPts() const {	// get the center points
	return ctrs;
    }
    KMcenter& operator[](int i) {	// index centers
	return ctrs[i];
    }
    const KMcenter& operator[](int i) const {
	return ctrs[i];
    }
    void resize(int k);			// resize array

    virtual void KMcenters::print(	// print centers
        bool fancy = true);
};
#endif
