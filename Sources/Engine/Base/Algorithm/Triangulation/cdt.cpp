/* 
 * Poly2Tri Copyright (c) 2009-2010, Poly2Tri Contributors
 * http://code.google.com/p/poly2tri/
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
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
#include "Headers\toolspch.h"
#include "cdt.h"
#include "Engine\Base\Algorithm\Triangulation\TriangleStrip.h"

namespace p2t {

CDT::CDT(const vector<Coord>& points) {
	polyline.resize(points.size());
	for(int i =0; i < points.size(); ++i) {
		const Coord& c = points.at(i);
		polyline[i] = new Point(c.x, c.y);
	}
	sweep_context_ = new SweepContext(polyline);
	sweep_ = new Sweep;
}
CDT::CDT(std::vector<Point*> _polyline)
{
  sweep_context_ = new SweepContext(_polyline);
  sweep_ = new Sweep;
}

void CDT::AddHole(std::vector<Point*> polyline)
{
  sweep_context_->AddHole(polyline);
}

void CDT::AddPoint(Point* point) {
  sweep_context_->AddPoint(point);
}

void CDT::Triangulate()
{
  sweep_->Triangulate(*sweep_context_);
}

std::vector<p2t::Triangle*> CDT::GetTriangles()
{
  return sweep_context_->GetTriangles();
}

std::list<p2t::Triangle*> CDT::GetMap()
{
  return sweep_context_->GetMap();
}

void CDT::getTriangleStrips(vector<Coord>& points){
	vector<p2t::Triangle*> triangles = sweep_context_->GetTriangles();
	vector<TriEdge *> edges;
	map<int,vector<int> > triangleEdgeIndexes;

	for(int i=0; i < triangles.size(); ++i) {
		Triangle *tri = triangles.at(i);
		TriEdge *edge = new TriEdge(tri->GetPoint(0), tri->GetPoint(1));
		addEdge(i, edge, edges);
		edge = new TriEdge(tri->GetPoint(1), tri->GetPoint(2));
		addEdge(i, edge, edges);
		edge = new TriEdge(tri->GetPoint(2), tri->GetPoint(0));
		addEdge(i, edge, edges);

	}
	for(int j=0 ;j < edges.size(); ++j) {
		TriEdge *edge = edges.at(j);
		for(list<int>::iterator cur=edge->edgeOf.begin(); cur != edge->edgeOf.end(); ++cur) {
			triangleEdgeIndexes[(*cur)].push_back(j);
		}
	}

	udword *topology = new udword(triangleEdgeIndexes.size() * 3);
	for(int i = 0; i < triangleEdgeIndexes.size(); ++i) {
		topology[3*i] = triangleEdgeIndexes[i].at(0);
		topology[3*i+1] = triangleEdgeIndexes[i].at(1);
		topology[3*i+2] = triangleEdgeIndexes[i].at(2);
	}

	STRIPERCREATE sc;
	sc.DFaces			= topology;
	sc.NbFaces			= triangleEdgeIndexes.size();
	sc.AskForWords		= true;
	sc.ConnectAllStrips	= false;
	sc.OneSided			= false;
	sc.SGIAlgorithm		= true;

	Striper Strip;
	Strip.Init(sc);

	STRIPERRESULT sr;
	Strip.Compute(sr);

	uword* Refs = (uword*)sr.StripRuns;
	for(udword i=0;i<sr.NbStrips;i++)
	{
		udword NbRefs = sr.StripLengths[i];
		for(udword j=0;j<NbRefs;j++)
		{
			int n =  *Refs++;
			int k = n + 1;
		}
	}

	for(int j=0 ;j < edges.size(); ++j) {
		delete edges[j];
	}
}

void CDT::addEdge(int triIndex, TriEdge *edge,vector<TriEdge *>& edges ) {
	TriEdge *foundEdge = 0;
	for( int i=0; i < edges.size(); ++i) {
		TriEdge *tri = edges.at(i);
		if ( tri->equals(edge)) {
			foundEdge = tri;
			break;
		}
	}
	if ( foundEdge == 0) {
		edge->edgeOf.push_back(triIndex);
		edges.push_back(edge);
	}else
		foundEdge->edgeOf.push_back(triIndex);
}

CDT::~CDT()
{
  delete sweep_context_;
  delete sweep_;
}

}

bool TriEdge::equals(const TriEdge* edge) {
	return (x1 == edge->x1 && x2 == edge->x2 && y1 == edge->y1 && y2 == edge->y2) ||
		   (x1 == edge->x2 && x2 == edge->x1 && y1 == edge->y2 && y2 == edge->y1);
} 