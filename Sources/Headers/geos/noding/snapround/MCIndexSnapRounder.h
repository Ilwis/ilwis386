/**********************************************************************
 * $Id: MCIndexSnapRounder.h 2319 2009-04-07 19:00:36Z strk $
 *
 * GEOS - Geometry Engine Open Source
 * http://geos.refractions.net
 *
 * Copyright (C) 2006      Refractions Research Inc.
 *
 * This is free software; you can redistribute and/or modify it under
 * the terms of the GNU Lesser General Public Licence as published
 * by the Free Software Foundation. 
 * See the COPYING file for more information.
 *
 **********************************************************************
 *
 * Last port: noding/snapround/MCIndexSnapRounder.java rev. 1.3 (JTS-1.9)
 *
 **********************************************************************/

#ifndef GEOS_NODING_SNAPROUND_MCINDEXSNAPROUNDER_H
#define GEOS_NODING_SNAPROUND_MCINDEXSNAPROUNDER_H

#include <vector>

#include <geos/inline.h>

#include <geos/noding/Noder.h> // for inheritance
#include <geos/algorithm/LineIntersector.h> // for composition
#include <geos/geom/Coordinate.h> // for use in vector

// Forward declarations
namespace geos {
	namespace geom {
		class PrecisionModel;
	}
	namespace algorithm {
		class LineIntersector;
	}
	namespace noding {
		class SegmentString;
		class NodedSegmentString;
		class MCIndexNoder;
		namespace snapround {
			//class HotPixel;
			class MCIndexPointSnapper;
		}
	}
}

namespace geos {
namespace noding { // geos::noding
namespace snapround { // geos::noding::snapround


/** \brief
 * Uses Snap Rounding to compute a rounded,
 * fully noded arrangement from a set of SegmentString
 *
 * Implements the Snap Rounding technique described in Hobby, Guibas & Marimont,
 * and Goodrich et al.
 *
 * Snap Rounding assumes that all vertices lie on a uniform grid
 * (hence the precision model of the input must be fixed precision,
 * and all the input vertices must be rounded to that precision).
 * 
 * This implementation uses a monotone chains and a spatial index to
 * speed up the intersection tests.
 *
 * This implementation appears to be fully robust using an integer
 * precision model.
 *
 * It will function with non-integer precision models, but the
 * results are not 100% guaranteed to be correctly noded.
 */
class MCIndexSnapRounder: public Noder { // implments Noder

private:

	/// externally owned
	geom::PrecisionModel& pm;

	algorithm::LineIntersector li;

	double scaleFactor;

	std::vector<SegmentString*>* nodedSegStrings;

	std::auto_ptr<MCIndexPointSnapper> pointSnapper;

	void snapRound(MCIndexNoder& noder, std::vector<SegmentString*>* segStrings);

	
	/**
	 * Computes all interior intersections in the collection of SegmentStrings,
	 * and push their Coordinate to the provided vector.
	 *
	 * Does NOT node the segStrings.
	 *
	 */
	void findInteriorIntersections(MCIndexNoder& noder,
			std::vector<SegmentString*>* segStrings,
			std::vector<geom::Coordinate>& intersections);

	/**
	 * Computes nodes introduced as a result of snapping
	 * segments to snap points (hot pixels)
	 */
	void computeIntersectionSnaps(std::vector<geom::Coordinate>& snapPts);

	/**
	 * Performs a brute-force comparison of every segment in each {@link SegmentString}.
	 * This has n^2 performance.
	 */
	void computeVertexSnaps(NodedSegmentString* e);
	
	void checkCorrectness(std::vector<SegmentString*>& inputSegmentStrings);

public:

	MCIndexSnapRounder(geom::PrecisionModel& nPm);

	std::vector<SegmentString*>* getNodedSubstrings() const;

	void computeNodes(std::vector<SegmentString*>* segStrings);
 
	/**
	 * Computes nodes introduced as a result of
	 * snapping segments to vertices of other segments
	 *
	 * @param segStrings the list of segment strings to snap together
	 *        NOTE: they *must* be instances of NodedSegmentString, or
	 * 	  an assertion will fail.
	 */
	void computeVertexSnaps(std::vector<SegmentString*>& edges);

};


} // namespace geos::noding::snapround
} // namespace geos::noding
} // namespace geos

#ifdef GEOS_INLINE
# include <geos/noding/snapround/MCIndexSnapRounder.inl>
#endif

#endif // GEOS_NODING_SNAPROUND_MCINDEXSNAPROUNDER_H

/**********************************************************************
 * $Log$
 * Revision 1.2  2006/03/24 09:52:41  strk
 * USE_INLINE => GEOS_INLINE
 *
 * Revision 1.1  2006/03/14 12:55:56  strk
 * Headers split: geomgraphindex.h, nodingSnapround.h
 *
 **********************************************************************/

