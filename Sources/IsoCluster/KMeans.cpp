//----------------------------------------------------------------------
//	File:           KMeans.cc
//	Programmer:     David Mount
//	Last modified:  03/27/02
//	Description:    Shared utilities for k-means.
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
#include <iostream>
#include "KMeans.h"			// kmeans includes
#include "KCtree.h"			// kc tree 
#include "KMrand.h"			// random number generators

//------------------------------------------------------------------------
//  Global data (shared by all files)
//	The following variables are used by all the procedures and are
//	initialized in kmInitGlobals().  kmInitTime is the CPU time
//	needed to initialize things before the first stage.
//------------------------------------------------------------------------

StatLev		kmStatLev	= SILENT;// global statistics output level
ostream*	kmOut		= &cout; // standard output stream
ostream*	kmErr		= &cerr; // output error stream
istream*	kmIn		= &cin;	 // input stream

//----------------------------------------------------------------------
//  Output utilities
//----------------------------------------------------------------------

void kmPrintPt(				// print a point
    KMpoint		p,			// the point
    int			dim,			// the dimension
    bool		fancy)			// print plain or fancy?
{
    if (fancy) *kmOut << "[ ";
    for (int i = 0; i < dim; i++) {
	*kmOut << setw(8) << p[i];
	if (i < dim-1) *kmOut << " ";
    }
    if (fancy) *kmOut << " ]";
}

void kmPrintPts(			// print points
    string		title,			// name of point set
    KMpointArray	pa,			// the point array
    int			n,			// number of points
    int			dim,			// the dimension
    bool		fancy)		        // print plain or fancy?
{
    *kmOut << "  (" << title << ":\n";
    for (int i = 0; i < n; i++) {
	*kmOut << "    " << i << "\t";
	kmPrintPt(pa[i], dim, fancy);
	*kmOut << "\n";
    }
    *kmOut << "  )" << endl;
}

/* added Nargess */
void kmPrintPts(                        // print points
    string              title,                  // name of point set
    vector<KMpoint>     pa,                     // the point array
    int                 n,                      // number of points
    int                 dim,                    // the dimension
    bool                fancy)                  // print plain or fancy?
{
    *kmOut << "  (" << title << ":\n";
    for (int i = 0; i < n; i++) {
        *kmOut << "    " << i << "\t";
        kmPrintPt(pa[i], dim, fancy);
        *kmOut << "\n";
    }
    *kmOut << "  )" << endl;
}

//------------------------------------------------------------------------
//  kmError - print error message
//  	If KMerr is KMabort we also abort the program.
//------------------------------------------------------------------------

void kmError(				// error routine
    const string	&msg,		// error message
    KMerr		level)		// abort afterwards
{
    if (level == KMabort) {
	*kmErr << "kmlocal: ERROR------->" << msg << "<-------------ERROR"
	       << endl;
	*kmOut << "kmlocal: ERROR------->" << msg << "<-------------ERROR"
	       << endl;
	kmExit(1);
    }
    else {
	*kmErr << "kmlocal: WARNING----->" << msg << "<-------------WARNING"
	       << endl;
	*kmOut << "kmlocal: WARNING----->" << msg << "<-------------WARNING"
	       << endl;
    }
}

//------------------------------------------------------------------------
//  kmExit - exit from program
//  	This is used mostly because Borland C++ deletes the command
//	prompt window immediately on exit.  This keeps until
//	the user verifies deletion.
//------------------------------------------------------------------------


void kmExit(int status)			// exit program
{
    #ifdef _BORLAND_C
	char ch;
	if (kmIn == &cin) {			// input from std in
	    cerr << "Hit return to continue..." << endl;
	    kmIn->get(ch);
	}
    #endif
    exit(status);
}
