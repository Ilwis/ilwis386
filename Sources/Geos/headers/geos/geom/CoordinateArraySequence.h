/**********************************************************************
 * $Id: CoordinateArraySequence.h 2678 2009-10-17 12:28:41Z strk $
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
 **********************************************************************/

#ifndef GEOS_GEOM_COORDINATEARRAYSEQUENCE_H
#define GEOS_GEOM_COORDINATEARRAYSEQUENCE_H

#include <geos/export.h>
#include <vector>

//#include <geos/platform.h>
#include <geos/geom/CoordinateSequence.h>

#include <geos/inline.h>

// Forward declarations
namespace geos {
	namespace geom { 
		class Coordinate;
	}
}


namespace geos {
namespace geom { // geos.geom

/// The default implementation of CoordinateSequence
class GEOS_DLL CoordinateArraySequence : public CoordinateSequence {
public:

	_export CoordinateArraySequence(const CoordinateArraySequence &cl);

	_export CoordinateSequence *clone() const;

	//const Coordinate& getCoordinate(int pos) const;
	_export const Coordinate& getAt(size_t pos) const;

	/// Copy Coordinate at position i to Coordinate c
	_export virtual void getAt(size_t i, Coordinate& c) const;

	//int size() const;
	_export size_t getSize() const;

	// @deprecated
	_export const std::vector<Coordinate>* toVector() const;

	// See dox in CoordinateSequence.h
	_export void toVector(std::vector<Coordinate>&) const;

	/// Construct an empty sequence
	_export CoordinateArraySequence();

	/// Construct sequence taking ownership of given Coordinate vector
	_export CoordinateArraySequence(std::vector<Coordinate> *coords);

	/// Construct sequence allocating space for n coordinates
	_export CoordinateArraySequence(size_t n);

	_export ~CoordinateArraySequence();

	_export bool isEmpty() const;

	_export void add(const Coordinate& c);

	_export virtual void add(const Coordinate& c, bool allowRepeated);

	/** \brief
	 * Inserts the specified coordinate at the specified position in
	 * this list.
	 *
	 * @param i the position at which to insert
	 * @param coord the coordinate to insert
	 * @param allowRepeated if set to false, repeated coordinates are
	 *                      collapsed
	 *
	 * NOTE: this is a CoordinateList interface in JTS
	 */
	_export virtual void add(size_t i, const Coordinate& coord, bool allowRepeated);

	_export void setAt(const Coordinate& c, size_t pos);

	_export void deleteAt(size_t pos);

	_export std::string toString() const;

	_export void setPoints(const std::vector<Coordinate> &v);

	_export double getOrdinate(size_t index,
			size_t ordinateIndex) const;

	_export void setOrdinate(size_t index, size_t ordinateIndex,
			double value);

	_export void expandEnvelope(Envelope &env) const;

	_export size_t getDimension() const { return 3; }

	_export void apply_rw(const CoordinateFilter *filter); 

	_export void apply_ro(CoordinateFilter *filter) const; 

	_export virtual CoordinateSequence& removeRepeatedPoints();

private:
	std::vector<Coordinate> *vect;
};

/// This is for backward API compatibility
typedef CoordinateArraySequence DefaultCoordinateSequence;

} // namespace geos.geom
} // namespace geos

//#ifdef GEOS_INLINE
//# include "geos/geom/CoordinateArraySequence.inl"
//#endif

#endif // ndef GEOS_GEOM_COORDINATEARRAYSEQUENCE_H

/**********************************************************************
 * $Log$
 * Revision 1.4  2006/06/12 10:10:39  strk
 * Fixed getGeometryN() to take size_t rather then int, changed unsigned int parameters to size_t.
 *
 * Revision 1.3  2006/05/03 08:58:34  strk
 * added new non-static CoordinateSequence::removeRepeatedPoints() mutator.
 *
 * Revision 1.2  2006/03/24 09:52:41  strk
 * USE_INLINE => GEOS_INLINE
 *
 * Revision 1.1  2006/03/09 16:46:49  strk
 * geos::geom namespace definition, first pass at headers split
 *
 **********************************************************************/
