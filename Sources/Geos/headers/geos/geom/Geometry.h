/**********************************************************************
 * $Id: Geometry.h 2757 2009-12-01 15:39:41Z mloskot $
 *
 * GEOS - Geometry Engine Open Source
 * http://geos.refractions.net
 *
 * Copyright (C) 2009  Sandro Santilli <strk@keybit.net>
 * Copyright (C) 2005 2006 Refractions Research Inc.
 * Copyright (C) 2001-2002 Vivid Solutions Inc.
 *
 * This is free software; you can redistribute and/or modify it under
 * the terms of the GNU Lesser General Public Licence as published
 * by the Free Software Foundation. 
 * See the COPYING file for more information.
 *
 **********************************************************************
 *
 * Last port: geom/Geometry.java rev. 1.112
 *
 **********************************************************************/

#ifndef GEOS_GEOM_GEOMETRY_H
#define GEOS_GEOM_GEOMETRY_H

#include <geos/export.h>
#include <geos/platform.h>
#include <geos/inline.h>
#include <geos/geom/Envelope.h>
#include <geos/geom/Dimension.h> // for Dimension::DimensionType
#include <geos/geom/GeometryComponentFilter.h> // for inheritance

#include <string>
#include <iostream>
#include <vector>
#include <memory>

// Forward declarations
namespace geos {
	namespace geom {
		class Coordinate;
		class CoordinateFilter;
		class CoordinateSequence;
		class CoordinateSequenceFilter;
		class GeometryComponentFilter;
		class GeometryFactory;
		class GeometryFilter;
		class IntersectionMatrix;
		class PrecisionModel;
		class Point;
	}
	namespace io { // geos.io
		class Unload;
	} // namespace geos.io
}

namespace geos {
namespace geom { // geos::geom

/// Geometry types
enum GeometryTypeId {
	/// a point
	GEOS_POINT,
	/// a linestring
	GEOS_LINESTRING,
	/// a linear ring (linestring with 1st point == last point)
	GEOS_LINEARRING,
	/// a polygon
	GEOS_POLYGON,
	/// a collection of points
	GEOS_MULTIPOINT,
	/// a collection of linestrings
	GEOS_MULTILINESTRING,
	/// a collection of polygons
	GEOS_MULTIPOLYGON,
	/// a collection of heterogeneus geometries
	GEOS_GEOMETRYCOLLECTION
};

/**
 * \class Geometry geom.h geos.h
 *
 * \brief Basic implementation of Geometry, constructed and
 * destructed by GeometryFactory.
 *
 *  <code>clone</code> returns a deep copy of the object.
 *  Use GeometryFactory to construct.
 *
 *  <H3>Binary Predicates</H3>
 * Because it is not clear at this time
 * what semantics for spatial
 *  analysis methods involving <code>GeometryCollection</code>s would be useful,
 *  <code>GeometryCollection</code>s are not supported as arguments to binary
 *  predicates (other than <code>convexHull</code>) or the <code>relate</code>
 *  method.
 *
 *  <H3>Set-Theoretic Methods</H3>
 *
 *  The spatial analysis methods will
 *  return the most specific class possible to represent the result. If the
 *  result is homogeneous, a <code>Point</code>, <code>LineString</code>, or
 *  <code>Polygon</code> will be returned if the result contains a single
 *  element; otherwise, a <code>MultiPoint</code>, <code>MultiLineString</code>,
 *  or <code>MultiPolygon</code> will be returned. If the result is
 *  heterogeneous a <code>GeometryCollection</code> will be returned. <P>
 *
 *  Because it is not clear at this time what semantics for set-theoretic
 *  methods involving <code>GeometryCollection</code>s would be useful,
 * <code>GeometryCollections</code>
 *  are not supported as arguments to the set-theoretic methods.
 *
 *  <H4>Representation of Computed Geometries </H4>
 *
 *  The SFS states that the result
 *  of a set-theoretic method is the "point-set" result of the usual
 *  set-theoretic definition of the operation (SFS 3.2.21.1). However, there are
 *  sometimes many ways of representing a point set as a <code>Geometry</code>.
 *  <P>
 *
 *  The SFS does not specify an unambiguous representation of a given point set
 *  returned from a spatial analysis method. One goal of JTS is to make this
 *  specification precise and unambiguous. JTS will use a canonical form for
 *  <code>Geometry</code>s returned from spatial analysis methods. The canonical
 *  form is a <code>Geometry</code> which is simple and noded:
 *  <UL>
 *    <LI> Simple means that the Geometry returned will be simple according to
 *    the JTS definition of <code>isSimple</code>.
 *    <LI> Noded applies only to overlays involving <code>LineString</code>s. It
 *    means that all intersection points on <code>LineString</code>s will be
 *    present as endpoints of <code>LineString</code>s in the result.
 *  </UL>
 *  This definition implies that non-simple geometries which are arguments to
 *  spatial analysis methods must be subjected to a line-dissolve process to
 *  ensure that the results are simple.
 *
 *  <H4> Constructed Points And The Precision Model </H4>
 *
 *  The results computed by the set-theoretic methods may
 *  contain constructed points which are not present in the input Geometry.
 *  These new points arise from intersections between line segments in the
 *  edges of the input Geometry. In the general case it is not
 *  possible to represent constructed points exactly. This is due to the fact
 *  that the coordinates of an intersection point may contain twice as many bits
 *  of precision as the coordinates of the input line segments. In order to
 *  represent these constructed points explicitly, JTS must truncate them to fit
 *  the PrecisionModel. 
 *
 *  Unfortunately, truncating coordinates moves them slightly. Line segments
 *  which would not be coincident in the exact result may become coincident in
 *  the truncated representation. This in turn leads to "topology collapses" --
 *  situations where a computed element has a lower dimension than it would in
 *  the exact result. 
 *
 *  When JTS detects topology collapses during the computation of spatial
 *  analysis methods, it will throw an exception. If possible the exception will
 *  report the location of the collapse. 
 *
 *  equals(Object) and hashCode are not overridden, so that when two
 *  topologically equal Geometries are added to HashMaps and HashSets, they
 *  remain distinct. This behaviour is desired in many cases.
 *
 */
class Geometry {

public:

	friend class GeometryFactory;

	friend std::ostream& operator<< (std::ostream& os, const Geometry& geom);

	/// A vector of const Geometry pointers
	typedef std::vector<const Geometry *> ConstVect;

	/// A vector of non-const Geometry pointers
	typedef std::vector<Geometry *> NonConstVect;

	/// An auto_ptr of Geometry
	typedef std::auto_ptr<Geometry> AutoPtr;

	/// Make a deep-copy of this Geometry
	virtual Geometry* clone() const=0;

	/// Destroy Geometry and all components
	virtual ~Geometry();


	/**
	 * \brief
	 * Gets the factory which contains the context in which this
	 * geometry was created.
	 *
	 * @return the factory for this geometry
	 */
	_export const GeometryFactory* getFactory() const { return factory; }

	/**
	* \brief
	* A simple scheme for applications to add their own custom data to
	* a Geometry.
	* An example use might be to add an object representing a
	* Coordinate Reference System.
	* 
	* Note that user data objects are not present in geometries created
	* by construction methods.
	*
	* @param newUserData an object, the semantics for which are
	* defined by the application using this Geometry
	*/
	_export void setUserData(void* newUserData) { userData=newUserData; }

	/**
	* \brief
	* Gets the user data object for this geometry, if any.
	*
	* @return the user data object, or <code>null</code> if none set
	*/
	_export void* getUserData() { return userData; }

	/*
	 * \brief
	 * Returns the ID of the Spatial Reference System used by the
	 * <code>Geometry</code>.
	 *
	 * GEOS supports Spatial Reference System information in the simple way
	 * defined in the SFS. A Spatial Reference System ID (SRID) is present
	 * in each <code>Geometry</code> object. <code>Geometry</code>
	 * provides basic accessor operations for this field, but no others.
	 * The SRID is represented as an integer.
	 *
	 * @return the ID of the coordinate space in which the
	 * <code>Geometry</code> is defined.
	 *
	 */
	_export virtual int getSRID() const { return SRID; }

	/*
	 * Sets the ID of the Spatial Reference System used by the
	 * <code>Geometry</code>.
	 */
	_export virtual void setSRID(int newSRID) { SRID=newSRID; }

	/**
	 * \brief
	 * Get the PrecisionModel used to create this Geometry.
	 */
	_export const PrecisionModel* getPrecisionModel() const;

	/// \brief
	/// Returns a vertex of this Geometry,
	/// or NULL if this is the empty geometry
	///
	_export virtual const Coordinate* getCoordinate() const=0; //Abstract

	/**
	 * \brief
	 * Returns this Geometry vertices.
	 * Caller takes ownership of the returned object.
	 */
	_export virtual CoordinateSequence* getCoordinates() const=0; //Abstract

	/// Returns the count of this Geometrys vertices.
	_export virtual size_t getNumPoints() const=0; //Abstract

	/// Returns false if the Geometry not simple.
	_export virtual bool isSimple() const; 

	/// Return a string representation of this Geometry type
	_export virtual std::string getGeometryType() const=0; //Abstract

	/// Return an integer representation of this Geometry type
	_export virtual GeometryTypeId getGeometryTypeId() const=0; //Abstract

	/// Returns the number of geometries in this collection
	/// (or 1 if this is not a collection)
	_export virtual size_t getNumGeometries() const { return 1; }

	/// Returns a pointer to the nth Geometry int this collection
	/// (or self if this is not a collection)
	_export virtual const Geometry* getGeometryN(size_t /*n*/) const { return this; }

	/**
	 * \brief Tests the validity of this <code>Geometry</code>.
	 *
	 * Subclasses provide their own definition of "valid".
	 *
	 * @return <code>true</code> if this <code>Geometry</code> is valid
	 *
	 * @see IsValidOp
	 */
	_export virtual bool isValid() const;

	/// Returns whether or not the set of points in this Geometry is empty.
	_export virtual bool isEmpty() const=0; //Abstract

	/// Polygon overrides to check for actual rectangle
	_export virtual bool isRectangle() const { return false; }

	/// Returns the dimension of this Geometry (0=point, 1=line, 2=surface)
	_export virtual Dimension::DimensionType getDimension() const=0; //Abstract

	/**
	 * \brief
	 * Returns the boundary, or an empty geometry of appropriate
	 * dimension if this <code>Geometry</code>  is empty.
	 *
	 * (In the case of zero-dimensional geometries, 
	 * an empty GeometryCollection is returned.)
	 * For a discussion of this function, see the OpenGIS Simple
	 * Features Specification. As stated in SFS Section 2.1.13.1,
	 * "the boundary of a Geometry is a set of Geometries of the
	 * next lower dimension."
	 *
	 * @return  the closure of the combinatorial boundary
	 *          of this <code>Geometry</code>.
	 *          Ownershipof the returned object transferred to caller.
	 */
	_export virtual Geometry* getBoundary() const=0; //Abstract

	/// Returns the dimension of this Geometrys inherent boundary.
	_export virtual int getBoundaryDimension() const=0; //Abstract

	/// Returns this Geometrys bounding box.
	_export virtual Geometry* getEnvelope() const;

	/** \brief
	 * Returns the minimum and maximum x and y values in this Geometry,
	 * or a null Envelope if this Geometry is empty.
	 */
	_export virtual const Envelope* getEnvelopeInternal() const;

	/**
	 * Tests whether this geometry is disjoint from the specified geometry.
	 * 
	 * The <code>disjoint</code> predicate has the following equivalent
	 * definitions:
	 *  - The two geometries have no point in common
	 *  - The DE-9IM Intersection Matrix for the two geometries matches
	 *    <code>[FF*FF****]</code>
	 *  - <code>! g.intersects(this)</code>
	 *    (<code>disjoint</code> is the inverse of <code>intersects</code>)
	 *
	 * @param  g  the Geometry with which to compare this Geometry
	 * @return true if the two <code>Geometry</code>s are disjoint
	 *
	 * @see Geometry::intersects
	 */
	_export virtual bool disjoint(const Geometry *other) const;

	/** \brief
	 * Returns true if the DE-9IM intersection matrix for the two
	 * Geometrys is FT*******, F**T***** or F***T****.
	 */
	_export virtual bool touches(const Geometry *other) const;

	/// Returns true if disjoint returns false.
	_export virtual bool intersects(const Geometry *g) const;

	/**
	 * Tests whether this geometry crosses the specified geometry.
	 * 
	 * The <code>crosses</code> predicate has the following equivalent
	 * definitions:
	 *  - The geometries have some but not all interior points in common.
	 *  - The DE-9IM Intersection Matrix for the two geometries matches
	 *    - <code>[T*T******]</code> (for P/L, P/A, and L/A situations)
	 *    - <code>[T*****T**]</code> (for L/P, A/P, and A/L situations)
	 *    - <code>[0********]</code> (for L/L situations)
	 * For any other combination of dimensions this predicate returns
	 * <code>false</code>.
	 * 
	 * The SFS defined this predicate only for P/L, P/A, L/L, and L/A
	 * situations.
	 * JTS extends the definition to apply to L/P, A/P and A/L situations
	 * as well, in order to make the relation symmetric.
	 *
	 * @param  g  the <code>Geometry</code> with which to compare this
	 *            <code>Geometry</code>
	 *@return  <code>true</code> if the two <code>Geometry</code>s cross.
	 */
	_export virtual bool crosses(const Geometry *g) const;

	/** \brief
	 * Returns true if the DE-9IM intersection matrix for the two
	 * Geometrys is T*F**F***.
	 */
	_export virtual bool within(const Geometry *g) const;

	/// Returns true if other.within(this) returns true.
	_export virtual bool contains(const Geometry *g) const;

	/** \brief
	 * Returns true if the DE-9IM intersection matrix for the two
	 * Geometrys is T*T***T** (for two points or two surfaces)
	 * 1*T***T** (for two curves).
	 */
	_export virtual bool overlaps(const Geometry *g) const;

	/**
	 * \brief
	 * Returns true if the elements in the DE-9IM intersection matrix
	 * for the two Geometrys match the elements in intersectionPattern.
	 *
	 * IntersectionPattern elements may be: 0 1 2 T ( = 0, 1 or 2)
	 * F ( = -1) * ( = -1, 0, 1 or 2).
	 *
	 * For more information on the DE-9IM, see the OpenGIS Simple
	 * Features Specification.
	 *
	 * @throws util::IllegalArgumentException if either arg is a collection
	 *
	 */
	_export virtual bool relate(const Geometry *g,
			const std::string& intersectionPattern) const;

	bool relate(const Geometry& g, const std::string& intersectionPattern) const
	{
		return relate(&g, intersectionPattern);
	}

	/// Returns the DE-9IM intersection matrix for the two Geometrys.
	_export virtual IntersectionMatrix* relate(const Geometry *g) const;
	IntersectionMatrix* relate(const Geometry &g) const {
		return relate(&g);
	}

	/**
	 * \brief
	 * Returns true if the DE-9IM intersection matrix for the two
	 * Geometrys is T*F**FFF*.
	 */
	_export virtual bool equals(const Geometry *g) const;

	/** \brief
	 * Returns <code>true</code> if this geometry covers the
	 * specified geometry.
	 * 
	 * The <code>covers</code> predicate has the following
	 * equivalent definitions:
	 * 
	 * - Every point of the other geometry is a point of this geometry.
	 * - The DE-9IM Intersection Matrix for the two geometries is
	 *    <code>T*****FF*</code>
	 *    or <code>*T****FF*</code>
	 *    or <code>***T**FF*</code>
	 *    or <code>****T*FF*</code>
	 * - <code>g.coveredBy(this)</code>
	 *   (<code>covers</code> is the inverse of <code>coveredBy</code>)
	 * 
	 * If either geometry is empty, the value of this predicate
	 * is <tt>false</tt>.
	 *
	 * This predicate is similar to {@link #contains},
	 * but is more inclusive (i.e. returns <tt>true</tt> for more cases).
	 * In particular, unlike <code>contains</code> it does not distinguish
	 * between points in the boundary and in the interior of geometries.
	 * For most situations, <code>covers</code> should be used in
	 * preference to <code>contains</code>.
	 * As an added benefit, <code>covers</code> is more amenable to
	 * optimization, and hence should be more performant.
	 *
	 * @param  g 
	 *         the <code>Geometry</code> with which to compare this
	 *         <code>Geometry</code>
	 * 
	 * @return <code>true</code> if this <code>Geometry</code>
	 *                           covers <code>g</code>
	 *
	 * @see Geometry::contains
	 * @see Geometry::coveredBy
	 */
	_export bool covers(const Geometry* g) const;

	/** \brief
	 * Tests whether this geometry is covered by the
	 * specified geometry.
	 *
	 * The <code>coveredBy</code> predicate has the following
	 * equivalent definitions:
	 *
	 *  - Every point of this geometry is a point of the other geometry.
	 *  - The DE-9IM Intersection Matrix for the two geometries matches
	 *       <code>[T*F**F***]</code>
	 *    or <code>[*TF**F***]</code>
	 *    or <code>[**FT*F***]</code>
	 *    or <code>[**F*TF***]</code>
	 *  - <code>g.covers(this)</code>
	 *    (<code>coveredBy</code> is the converse of <code>covers</code>)
	 * 
	 * If either geometry is empty, the value of this predicate
	 * is <tt>false</tt>.
	 * 
	 * This predicate is similar to {@link #within},
	 * but is more inclusive (i.e. returns <tt>true</tt> for more cases).
	 *
	 * @param  g  the <code>Geometry</code> with which to compare
	 *            this <code>Geometry</code>
	 * @return  <code>true</code> if this <code>Geometry</code>
	 *                            is covered by <code>g</code>
	 *
	 * @see Geometry#within
	 * @see Geometry#covers
	 */
	bool coveredBy(const Geometry* g) const {
	  return g->covers(this);
	}


	/// Returns the Well-known Text representation of this Geometry.
	_export virtual std::string toString() const;

	_export virtual std::string toText() const;
	
	/// Returns a buffer region around this Geometry having the given width.
	//
	/// @throws util::TopologyException if a robustness error occurs
	///
	_export virtual Geometry* buffer(double distance) const;

	/// \brief
	/// Returns a buffer region around this Geometry having the
	/// given width and with a specified number of segments used
	/// to approximate curves.
	//
	/// @throws util::TopologyException if a robustness error occurs
	///
	_export virtual Geometry* buffer(double distance,int quadrantSegments) const;

	/** \brief
	 * Computes a buffer area around this geometry having the given
	 * width and with a specified accuracy of approximation for circular
	 * arcs, and using a specified end cap style.
	 * 
	 * Buffer area boundaries can contain circular arcs.
	 * To represent these arcs using linear geometry they must be
	 * approximated with line segments.
	 *
	 * The <code>quadrantSegments</code> argument allows controlling the
	 * accuracy of the approximation by specifying the number of line
	 * segments used to represent a quadrant of a circle
	 * 
	 * The end cap style specifies the buffer geometry that will be
	 * created at the ends of linestrings.  The styles provided are:
	 * 
	 * - BufferOp::CAP_ROUND - (default) a semi-circle
	 * - BufferOp::CAP_BUTT  - a straight line perpendicular to the
	 *                         end segment
	 * - BufferOp::CAP_SQUARE - a half-square
	 * 
	 *
	 * @param distance the width of the buffer
	 *                 (may be positive, negative or 0)
	 *
	 * @param quadrantSegments the number of line segments used
	 *                         to represent a quadrant of a circle
	 *
	 * @param endCapStyle the end cap style to use
	 *
	 * @return an area geometry representing the buffer region
	 *
	 * @throws util::TopologyException if a robustness error occurs
	 *
	 * @see BufferOp
	 */
	_export virtual Geometry* buffer(double distance, int quadrantSegments,
			int endCapStyle) const;

	/// \brief
	/// Returns the smallest convex Polygon that contains
	/// all the points in the Geometry.
	_export virtual Geometry* convexHull() const;

	/** \brief
	 * Returns a Geometry representing the points shared by
	 * this Geometry and other.
	 *
	 * @throws util::TopologyException if a robustness error occurs
	 * @throws util::IllegalArgumentException if either input is a
	 *         non-empty GeometryCollection
	 *
	 */
	_export virtual Geometry* intersection(const Geometry *other) const;

	/** \brief
	 * Returns a Geometry representing all the points in this Geometry
	 * and other.
	 *
	 * @throws util::TopologyException if a robustness error occurs
	 * @throws util::IllegalArgumentException if either input is a
	 *         non-empty GeometryCollection
	 *
	 */
	_export virtual Geometry* Union(const Geometry *other) const;
		// throw(IllegalArgumentException *, TopologyException *);

	/**
	 * \brief
	 * Returns a Geometry representing the points making up this
	 * Geometry that do not make up other.
	 *
	 * @throws util::TopologyException if a robustness error occurs
	 * @throws util::IllegalArgumentException if either input is a
	 *         non-empty GeometryCollection
	 *
	 */
	_export virtual Geometry* difference(const Geometry *other) const;

	/** \brief
	 * Returns a set combining the points in this Geometry not in other,
	 * and the points in other not in this Geometry.
	 *
	 * @throws util::TopologyException if a robustness error occurs
	 * @throws util::IllegalArgumentException if either input is a
	 *         non-empty GeometryCollection
	 *
	 */
	_export virtual Geometry* symDifference(const Geometry *other) const;

	/** \brief
	 * Returns true if the two Geometrys are exactly equal,
	 * up to a specified tolerance.
	 */
	virtual bool equalsExact(const Geometry *other, double tolerance=0)
		const=0; //Abstract

	virtual void apply_rw(const CoordinateFilter *filter)=0; //Abstract
	virtual void apply_ro(CoordinateFilter *filter) const=0; //Abstract
	_export virtual void apply_rw(GeometryFilter *filter);
	_export virtual void apply_ro(GeometryFilter *filter) const;
	_export virtual void apply_rw(GeometryComponentFilter *filter);
	_export virtual void apply_ro(GeometryComponentFilter *filter) const;

	/**
	 *  Performs an operation on the coordinates in this Geometry's
	 *  CoordinateSequences.s
	 *  If the filter reports that a coordinate value has been changed,
	 *  {@link #geometryChanged} will be called automatically.
	 *
	 * @param  filter  the filter to apply
	 */
	_export virtual void apply_rw(CoordinateSequenceFilter& filter)=0;

	/**
	 *  Performs a read-only operation on the coordinates in this
	 *  Geometry's CoordinateSequences.
	 *
	 * @param  filter  the filter to apply
	 */
	_export virtual void apply_ro(CoordinateSequenceFilter& filter) const=0;

	/** \brief
	 * Apply a fiter to each component of this geometry.
	 * The filter is expected to provide a .filter(const Geometry*)
	 * method.
	 * 
	 * I intend similar templated methods to replace
	 * all the virtual apply_rw and apply_ro functions...
	 *                --strk(2005-02-06);
	 */
	template <class T>
	void applyComponentFilter(T& f) const
	{
		for(size_t i=0, n=getNumGeometries(); i<n; ++i)
			f.filter(getGeometryN(i));
	}

	/// Converts this Geometry to normal form (or  canonical form).
	virtual void normalize()=0; //Abstract

	_export virtual int compareTo(const Geometry *geom) const;

	/** \brief
	 * Returns the minimum distance between this Geometry
	 * and the Geometry g
	 */
	_export virtual double distance(const Geometry *g) const;

	/// Returns the area of this Geometry.
	_export virtual double getArea() const;

	/// Returns the length of this Geometry.
	_export virtual double getLength() const;

	/** \brief
	 * Tests whether the distance from this Geometry  to another
	 * is less than or equal to a specified value.
	 */
	_export virtual bool isWithinDistance(const Geometry *geom,double cDistance);

	/** \brief
	 * Computes the centroid of this <code>Geometry</code>.
	 *
	 * The centroid is equal to the centroid of the set of component
	 * Geometries of highest dimension (since the lower-dimension geometries
	 * contribute zero "weight" to the centroid)
	 *
	 * @return a {@link Point} which is the centroid of this Geometry
	 */
	_export virtual Point* getCentroid() const;

	/// Computes the centroid of this Geometry as a Coordinate
	//
	/// Returns false if centroid cannot be computed (EMPTY geometry)
	///
	_export virtual bool getCentroid(Coordinate& ret) const;

	/** \brief
	 * Computes an interior point of this <code>Geometry</code>.
	 *
	 * An interior point is guaranteed to lie in the interior of the Geometry,
	 * if it possible to calculate such a point exactly. Otherwise,
	 * the point may lie on the boundary of the geometry.
	 *
	 * @return a Point which is in the interior of this Geometry, or
	 *         null if the geometry doesn't have an interior (empty)
	 */
	_export virtual Point* getInteriorPoint() const;

	/*
	 * \brief
	 * Notifies this Geometry that its Coordinates have been changed
	 * by an external party (using a CoordinateFilter, for example).
	 */
	_export virtual void geometryChanged();

	/**
	 * \brief
	 * Notifies this Geometry that its Coordinates have been changed
	 * by an external party.
	 */
	_export void geometryChangedAction();

protected:

	/// The bounding box of this Geometry
	mutable std::auto_ptr<Envelope> envelope;
	
	/// Returns true if the array contains any non-empty Geometrys.
	static bool hasNonEmptyElements(const std::vector<Geometry *>* geometries);

	/// Returns true if the CoordinateSequence contains any null elements.
	static bool hasNullElements(const CoordinateSequence* list);

	/// Returns true if the vector contains any null elements.
	static bool hasNullElements(const std::vector<Geometry *>* lrs);

//	static void reversePointOrder(CoordinateSequence* coordinates);
//	static Coordinate& minCoordinate(CoordinateSequence* coordinates);
//	static void scroll(CoordinateSequence* coordinates,Coordinate* firstCoordinate);
//	static int indexOf(Coordinate* coordinate,CoordinateSequence* coordinates);
//
	/** \brief
	 * Returns whether the two Geometrys are equal, from the point
	 * of view of the equalsExact method.
	 */
	_export virtual bool isEquivalentClass(const Geometry *other) const;

	static void checkNotGeometryCollection(const Geometry *g);
			// throw(IllegalArgumentException *);

	//virtual void checkEqualSRID(Geometry *other);

	//virtual void checkEqualPrecisionModel(Geometry *other);

	virtual Envelope::AutoPtr computeEnvelopeInternal() const=0; //Abstract

	virtual int compareToSameClass(const Geometry *geom) const=0; //Abstract

	int compare(std::vector<Coordinate> a, std::vector<Coordinate> b) const;

	int compare(std::vector<Geometry *> a, std::vector<Geometry *> b) const;

	bool equal(const Coordinate& a, const Coordinate& b,
			double tolerance) const;
	int SRID;

	/// @deprecated
	//Geometry* toInternalGeometry(const Geometry *g) const;

	/// @deprecated
	//Geometry* fromInternalGeometry(const Geometry *g) const;

	/// Polygon overrides to check for actual rectangle
	//virtual bool isRectangle() const { return false; } -- moved to public

	Geometry(const Geometry &geom);

	/** \brief
	 * Construct a geometry with the given GeometryFactory.
	 *
	 * Will keep a reference to the factory, so don't
	 * delete it until al Geometry objects referring to
	 * it are deleted.
	 *
	 * @param factory
	 */
	Geometry(const GeometryFactory *factory);

	const GeometryFactory *factory;

private:



	int getClassSortIndex() const;

	class GeometryChangedFilter : public GeometryComponentFilter
	{
	public:
		void filter_rw(Geometry* geom)
		{
			geom->geometryChangedAction();
		}
	};

	static GeometryChangedFilter geometryChangedFilter;

	/// The GeometryFactory used to create this Geometry
	//
	/// Externally owned
	///


	static const GeometryFactory* INTERNAL_GEOMETRY_FACTORY;

	void* userData;
};

/// \brief
/// Write the Well-known Binary representation of this Geometry
/// as an HEX string to the given output stream
///
std::ostream& operator<< (std::ostream& os, const Geometry& geom);

struct GEOS_DLL GeometryGreaterThen {
	bool operator()(const Geometry *first, const Geometry *second);
};


/// Return current GEOS version
std::string geosversion();

/**
 * \brief
 * Return the version of JTS this GEOS
 * release has been ported from.
 */
std::string jtsport();


} // namespace geos::geom
} // namespace geos

#ifdef GEOS_INLINE
# include <geos/geom/Geometry.inl>
#endif

#endif // ndef GEOS_GEOM_GEOMETRY_H

/**********************************************************************
 * $Log$
 * Revision 1.14  2006/07/08 00:33:55  strk
 *         * configure.in: incremented CAPI minor version, to avoid                        falling behind any future version from the 2.2. branch.
 *         * source/geom/Geometry.cpp, source/geom/GeometryFactory.cpp,
 *         source/geomgraph/EdgeRing.cpp,
 *         source/headers/geos/geom/Geometry.h,
 *         source/headers/geos/geom/GeometryFactory.h,
 *         source/headers/geos/geom/GeometryFactory.inl,
 *         source/headers/geos/geomgraph/EdgeRing.h:
 *         updated doxygen comments (sync with JTS head).
 *         * source/headers/geos/platform.h.in: include <inttypes.h>
 *         rather then <stdint.h>
 *
 * Revision 1.13  2006/06/12 10:10:39  strk
 * Fixed getGeometryN() to take size_t rather then int, changed unsigned int parameters to size_t.
 *
 * Revision 1.12  2006/05/18 08:56:50  strk
 *         * source/geom/Geometry.cpp,
 *         source/headers/geos/geom/Geometry.h: added
 *         covers() and isCoveredBy() predicates.
 *         * tests/unit/Makefile.am,
 *         tests/unit/geom/Geometry/coversTest.cpp:
 *         added test for covers() predicates.
 *
 * Revision 1.11  2006/05/04 15:49:39  strk
 * updated all Geometry::getDimension() methods to return Dimension::DimensionType (closes bug#93)
 *
 * Revision 1.10  2006/04/28 10:55:39  strk
 * Geometry constructors made protected, to ensure all constructions use GeometryFactory,
 * which has been made friend of all Geometry derivates. getNumPoints() changed to return
 * size_t.
 *
 * Revision 1.9  2006/04/11 09:31:47  strk
 * Added Geometry::AutoPtr typedef
 *
 * Revision 1.8  2006/04/10 18:15:09  strk
 * Changed Geometry::envelope member to be of type auto_ptr<Envelope>.
 * Changed computeEnvelopeInternal() signater to return auto_ptr<Envelope>
 *
 * Revision 1.7  2006/04/07 09:54:30  strk
 * Geometry::getNumGeometries() changed to return 'unsigned int'
 * rather then 'int'
 *
 * Revision 1.6  2006/03/31 16:53:53  strk
 * Added comment about possible NULL return from getCoordinate()
 *
 * Revision 1.5  2006/03/24 09:52:41  strk
 * USE_INLINE => GEOS_INLINE
 *
 * Revision 1.4  2006/03/23 15:10:29  strk
 * Dropped by-pointer TopologyException constructor, various small cleanups
 *
 * Revision 1.3  2006/03/23 12:12:01  strk
 * Fixes to allow build with -DGEOS_INLINE
 *
 * Revision 1.2  2006/03/20 12:03:25  strk
 * Added operator<< for Geometry, writing HEXWKB
 *
 * Revision 1.1  2006/03/09 16:46:49  strk
 * geos::geom namespace definition, first pass at headers split
 *
 **********************************************************************/
