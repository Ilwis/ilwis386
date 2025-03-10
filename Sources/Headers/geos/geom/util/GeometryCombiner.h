/**********************************************************************
 * $Id: GeometryCombiner.h 2253 2009-01-23 23:58:18Z mloskot $
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
#ifndef GEOS_GEOM_UTIL_GEOMETRYCOMBINER_H
#define GEOS_GEOM_UTIL_GEOMETRYCOMBINER_H

#include <vector>

// Forward declarations
namespace geos {
    namespace geom {
        class Geometry;
        class GeometryFactory;
    }
}

namespace geos {
namespace geom { // geos.geom
namespace util { // geos.geom.util

/**
 * Combines {@link Geometry}s
 * to produce a {@link GeometryCollection} of the most appropriate type.
 * Input geometries which are already collections
 * will have their elements extracted first.
 * No validation of the result geometry is performed.
 * (The only case where invalidity is possible is where {@link Polygonal} geometries
 * are combined and result in a self-intersection).
 * 
 * @see GeometryFactory#buildGeometry
 */
class GeometryCombiner 
{
public:
    /**
     * Combines a collection of geometries.
     * 
     * @param geoms the geometries to combine
     * @return the combined geometry
     */
    static Geometry* combine(std::vector<Geometry*> const& geoms);

    /**
     * Combines two geometries.
     * 
     * @param g0 a geometry to combine
     * @param g1 a geometry to combine
     * @return the combined geometry
     */
    static Geometry* combine(Geometry* g0, Geometry* g1);

    /**
     * Combines three geometries.
     * 
     * @param g0 a geometry to combine
     * @param g1 a geometry to combine
     * @param g2 a geometry to combine
     * @return the combined geometry
     */
    static Geometry* combine(Geometry* g0, Geometry* g1, Geometry* g2);

private:
    GeometryFactory const* geomFactory;
    bool skipEmpty;
    std::vector<Geometry*> const& inputGeoms;

public:
    /**
     * Creates a new combiner for a collection of geometries
     * 
     * @param geoms the geometries to combine
     */
    GeometryCombiner(std::vector<Geometry*> const& geoms);

    /**
     * Extracts the GeometryFactory used by the geometries in a collection
     * 
     * @param geoms
     * @return a GeometryFactory
     */
    static GeometryFactory const* extractFactory(std::vector<Geometry*> const& geoms);

    /**
     * Computes the combination of the input geometries
     * to produce the most appropriate {@link Geometry} or {@link GeometryCollection}
     * 
     * @return a Geometry which is the combination of the inputs
     */
    Geometry* combine();

private:
    void extractElements(Geometry* geom, std::vector<Geometry*>& elems);
};

} // namespace geos.geom.util
} // namespace geos.geom
} // namespace geos

#endif

/**********************************************************************
 * $Log$
 *
 **********************************************************************/
