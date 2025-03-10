/**********************************************************************
 * $Id: NodedSegmentString.h 2320 2009-04-08 09:42:55Z strk $
 *
 * GEOS - Geometry Engine Open Source
 * http://geos.refractions.net
 *
 * Copyright (C) 2006 Refractions Research Inc.
 *
 * This is free software; you can redistribute and/or modify it under
 * the terms of the GNU Lesser General Public Licence as published
 * by the Free Software Foundation. 
 * See the COPYING file for more information.
 *
 *
 **********************************************************************
 *
 * Last port: noding/NodedSegmentString.java rev. 1.2 (JTS-1.9)
 *
 **********************************************************************/

#ifndef GEOS_NODING_NODEDSEGMENTSTRING_H
#define GEOS_NODING_NODEDSEGMENTSTRING_H

#include <geos/noding/NodableSegmentString.h> // for inheritance
#include <geos/geom/CoordinateSequence.h> // for inlines
#include <geos/algorithm/LineIntersector.h>
#include <geos/noding/SegmentNode.h>
#include <geos/noding/SegmentNodeList.h>
#include <geos/noding/SegmentString.h>
//#include <geos/noding/Octant.h>
#include <geos/geom/Coordinate.h>

//using namespace 

namespace geos {
namespace noding { // geos::noding

/** \brief
 * Represents a list of contiguous line segments,
 * and supports noding the segments.
 *
 * The line segments are represented by an array of {@link Coordinate}s.
 * Intended to optimize the noding of contiguous segments by
 * reducing the number of allocated objects.
 * SegmentStrings can carry a context object, which is useful
 * for preserving topological or parentage information.
 * All noded substrings are initialized with the same context object.
 *
 */
class NodedSegmentString : public NodableSegmentString 
{
public:

	static void getNodedSubstrings(SegmentString::ConstVect* segStrings,
			SegmentString::NonConstVect* resultEdgelist)
	{
		for (size_t i=0, n=segStrings->size(); i<n; i++)
		{
			NodedSegmentString * nss = (NodedSegmentString *)((*segStrings)[i]);
			nss->getNodeList().addSplitEdges( resultEdgelist);
		}
	}

	static void getNodedSubstrings(
			const SegmentString::NonConstVect& segStrings,
			SegmentString::NonConstVect* resultEdgeList);

	/// Returns allocated object
	static SegmentString::NonConstVect* getNodedSubstrings(
			const SegmentString::NonConstVect& segStrings);


	/**
	 * Creates a new segment string from a list of vertices.
	 *
	 * @param newPts CoordinateSequence representing the string,
	 *                externally owned
	 *
	 * @param data the user-defined data of this segment string
	 *             (may be null)
	 */
	NodedSegmentString(geom::CoordinateSequence *newPts,
			const void* newContext)
		:
		NodableSegmentString(newContext),
		nodeList(this),
		pts(newPts)
	{ }

	~NodedSegmentString()
	{ }

	/**
	 * Adds an intersection node for a given point and segment to this segment string.
	 * If an intersection already exists for this exact location, the existing
	 * node will be returned.
	 * 
	 * @param intPt the location of the intersection
	 * @param segmentIndex the index of the segment containing the intersection
	 * @return the intersection node for the point
	 */
	SegmentNode * addIntersectionNode( geom::Coordinate * intPt, size_t segmentIndex) 
	{
		size_t normalizedSegmentIndex = segmentIndex;

		// normalize the intersection point location
		size_t nextSegIndex = normalizedSegmentIndex + 1;
		if (nextSegIndex < size()) 
		{
			const geom::Coordinate &nextPt = getCoordinate( nextSegIndex);

			// Normalize segment index if intPt falls on vertex
			// The check for point equality is 2D only - Z values are ignored
			if ( intPt->equals2D( nextPt )) 
			{
				normalizedSegmentIndex = nextSegIndex;
			}
		}

		// Add the intersection point to edge intersection list.
		SegmentNode * ei = getNodeList().add( *intPt, normalizedSegmentIndex);
		return ei;
	}

	SegmentNodeList& getNodeList();

	const SegmentNodeList& getNodeList() const;

	virtual unsigned int size() const
	{
		return pts->size();
	}

	virtual const geom::Coordinate& getCoordinate(unsigned int i) const;

	virtual geom::CoordinateSequence* getCoordinates() const;

	virtual bool isClosed() const;

	virtual std::ostream& print(std::ostream& os) const;


	/** \brief
	 * Gets the octant of the segment starting at vertex index.
	 *
	 * @param index the index of the vertex starting the segment. 
	 *        Must not be the last index in the vertex list
	 * @return the octant of the segment at the vertex
	 */
	int getSegmentOctant(unsigned int index) const;

	/** \brief
	 * Add {SegmentNode}s for one or both
	 * intersections found for a segment of an edge to the edge
	 * intersection list.
	 */
	void addIntersections(algorithm::LineIntersector *li,
			unsigned int segmentIndex, int geomIndex);

	/** \brief
	 * Add an SegmentNode for intersection intIndex.
	 *
	 * An intersection that falls exactly on a vertex
	 * of the SegmentString is normalized
	 * to use the higher of the two possible segmentIndexes
	 */
	void addIntersection(algorithm::LineIntersector *li,
			unsigned int segmentIndex,
			int geomIndex, int intIndex);

	/** \brief
	 * Add an SegmentNode for intersection intIndex.
	 *
	 * An intersection that falls exactly on a vertex of the
	 * edge is normalized
	 * to use the higher of the two possible segmentIndexes
	 */
	void addIntersection(const geom::Coordinate& intPt,
			unsigned int segmentIndex);


private:

	SegmentNodeList nodeList;

	geom::CoordinateSequence *pts;

};

} // namespace geos::noding
} // namespace geos

#endif // GEOS_NODING_NODEDSEGMENTSTRING_H
/**********************************************************************
 * $Log$
 **********************************************************************/

