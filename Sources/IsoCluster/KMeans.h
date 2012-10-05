//----------------------------------------------------------------------
//	File:           KMeans.h
//	Programmer:     David Mount
//	Last modified:  03/27/02
//	Description:    Include file for kmeans algorithms.
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

#ifndef KMEANS_H
#define KMEANS_H

#include "KM_ANN.h"			// basic definitions
#include <vector>

using namespace std;			// make standard names available

//----------------------------------------------------------------------
//  Important strings
//----------------------------------------------------------------------
const string KMshortName    = "KMlocal";
const string KMlongName	    = "KMlocal (k-means clustering by local search)";
const string KMversion	    = "1.0";
const string KMversionCmt   = "(Not for distribution)";
const string KMcopyright    = "David M. Mount";
const string KMlatestRev    = "March 27, 2002";

//------------------------------------------------------------------------
//  Type definitions
//	Although data points and centers are of the same type
//	as a KMpoint, we distinguish these types here for the
//	sake of making the code a little easier to interpret.
//
//	KMdataPoint	Used for k-means data points.
//	KMcenter	Used for k-means center points.
//	KMpoint		Used for any other points and intermediate
//			results used in the program.
//------------------------------------------------------------------------

typedef KMpoint		KMdataPoint;	// data point
typedef KMpoint		KMcenter;	// center point

typedef KMpointArray	KMdataArray;	// array of data points
typedef KMpointArray	KMcenterArray;	// array of center

typedef KMidx		KMdataIdx;	// a data point index
typedef KMidx		KMctrIdx;	// a center point index
typedef KMdataIdx	*KMdatIdxArray;	// array of data indices
typedef KMctrIdx	*KMctrIdxArray;	// array of center indices

//------------------------------------------------------------------------
//  Global constants
//------------------------------------------------------------------------

const double KM_ERR	 = 1E-6;	// epsilon (for floating compares)
const double KM_HUGE	 = DBL_MAX;	// huge double value
const int    KM_HUGE_INT = INT_MAX;	// huge int value

enum KMerr {KMwarn = 0, KMabort = 1};	// what to do in case of error

enum StatLev {				// output statistics levels
	SILENT,				// no output
	EXEC_TIME,			// just execution time
	SUMMARY,			// summary of entire algorithm
	PHASE,				// summary of each phase
	RUN,				// summary of each run
	STAGE,				// summary of each stage
	ISOTRACE,
	N_STAT_LEVELS};			// number of levels

enum KMalg {				// k-means algorithm names
	LLOYD,				// Lloyd's (using filtering)
	SWAP,				// swap heuristic
	HYBRID,				// hybrid algorithm
	RANDOM,				// random centers
	N_KM_ALGS};			// number of algorithms

//----------------------------------------------------------------------
//  Global variables
//----------------------------------------------------------------------

extern StatLev		kmStatLev;	// statistics output level
extern ostream*		kmOut;		// standard output stream
extern ostream*		kmErr;		// error output stream
extern istream*		kmIn;		// input stream

//----------------------------------------------------------------------
//  Printing utilities
//----------------------------------------------------------------------

void kmPrintPt(				// print a point
    KMpoint		p,			// the point
    int			dim,			// the dimension
    bool		fancy = true);		// print plain or fancy?

void kmPrintPts(			// print points
    string		title,			// name of point set
    KMpointArray	pa,			// the point array
    int			n,			// number of points
    int			dim,			// the dimension
    bool		fancy = true);		// print plain or fancy?

/* added by Nargess */
void kmPrintPts(                        // print points
    string              title,                  // name of point set
    vector<KMpoint>     pa,                     // the point array
    int                 n,                      // number of points
    int                 dim,                    // the dimension
    bool                fancy = true);          // print plain or fancy?


//----------------------------------------------------------------------
//  Utility function declarations
//----------------------------------------------------------------------

void kmError(				// error routine
    const string	&msg,			// error message
    KMerr		level);			// abort afterwards

void kmExit(int x = 0);                 // exit the program

#endif
