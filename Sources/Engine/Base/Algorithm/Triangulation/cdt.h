/* 
 * Poly2Tri Copyright (c) 2009-2010, Poly2Tri Contributors
 * http://code.google.com/p/poly2tri/
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 /* are permitted provided that the following conditions are met:
 *
 * * Redistributions of source code must retain the above copyright notice,
 *   this list of conditions and the following disclaimer.
 * * Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 * * Neither the name of Poly2Tri nor the names of its contributors may be
 *   used to endorse or promote products derived from this software without specific
 *   prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
 
#ifndef CDT_H
#define CDT_H
#include "Headers\compatib.h"
#include "Headers\baseconst.h"
#include "advancing_front.h"
#include "sweep_context.h"
#include "sweep.h"

struct TriEdge{
	TriEdge(p2t::Point *p1, p2t::Point *p2) : x1(p1->x), y1(p1->y), x2(p2->x), y2(p2->y) {}
	double x1,y1;
	double x2,y2;
	list<int> edgeOf;
	bool equals(const TriEdge* edge);
};

namespace p2t {

class _export CDT
{
public:

/// Constructor
CDT(const geos::geom::LineString *ring);
CDT(std::vector<Point*> polyline);
CDT(const vector<Coord>& points);
/// Destructor
~CDT();
/// Add a hole
void AddHole(std::vector<Point*> polyline);
void AddHole(const geos::geom::LineString *ring);
/// Add a single point
void AddPoint(Point* point);
/// Triangulate points
void Triangulate();
/// Get Delaunay triangles
std::vector<Triangle*> GetTriangles();
void getTriangleStrips(vector<Coord>& points);

/// Get triangle map
std::list<Triangle*> GetMap();

private:
void addEdge(int triIndex, TriEdge *edge,vector<TriEdge *>& edges );

SweepContext* sweep_context_;
Sweep* sweep_;
vector<Point *> polyline;

};

}

#endif