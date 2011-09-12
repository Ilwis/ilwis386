/************************************************************************
 *
 * $Id: geos_c.h.in 2975 2010-04-17 21:55:53Z strk $
 *
 * C-Wrapper for GEOS library
 *
 * Copyright (C) 2005 Refractions Research Inc.
 *
 * This is free software; you can redistribute and/or modify it under
 * the terms of the GNU Lesser General Public Licence as published
 * by the Free Software Foundation. 
 * See the COPYING file for more information.
 *
 * Author: Sandro Santilli <strk@refractions.net>
 *
 ***********************************************************************
 *
 * GENERAL NOTES:
 *
 *	- Remember to call initGEOS() before any use of this library's
 *	  functions, and call finishGEOS() when done.
 *
 *	- Currently you have to explicitly GEOSGeom_destroy() all
 *	  GEOSGeom objects to avoid memory leaks, and to GEOSFree()
 *	  all returned char * (unless const). 
 *
 ***********************************************************************/

#ifndef GEOS_C_H_INCLUDED
#define GEOS_C_H_INCLUDED

#ifndef __cplusplus
# include <stddef.h> /* for size_t definition */
#else
# include <cstddef>
using std::size_t;
#endif

#ifdef __cplusplus
extern "C" {
#endif

/************************************************************************
 *
 * Version
 *
 ***********************************************************************/

/*
 * Following 'ifdef' hack fixes problem with generating geos_c.h on Windows,
 * when building with Visual C++ compiler.
 *
 */
#if defined(_MSC_VER)
#include <geos/version.h>
#define GEOS_CAPI_VERSION_MAJOR 1
#define GEOS_CAPI_VERSION_MINOR 6
#define GEOS_CAPI_VERSION_PATCH 2
#define GEOS_CAPI_VERSION "3.2.2-CAPI-1.6.2"
#else
#ifndef GEOS_VERSION_MAJOR
#define GEOS_VERSION_MAJOR 3
#endif
#ifndef GEOS_VERSION_MINOR
#define GEOS_VERSION_MINOR 2
#endif
#ifndef GEOS_VERSION_PATCH
#define GEOS_VERSION_PATCH 2
#endif
#ifndef GEOS_VERSION
#define GEOS_VERSION "3.2.2"
#endif
#ifndef GEOS_JTS_PORT
#define GEOS_JTS_PORT "1.10.0"
#endif

#define GEOS_CAPI_VERSION_MAJOR 1
#define GEOS_CAPI_VERSION_MINOR 6
#define GEOS_CAPI_VERSION_PATCH 2
#define GEOS_CAPI_VERSION "3.2.2-CAPI-1.6.2"
#endif

#define GEOS_CAPI_FIRST_INTERFACE GEOS_CAPI_VERSION_MAJOR 
#define GEOS_CAPI_LAST_INTERFACE (GEOS_CAPI_VERSION_MAJOR+GEOS_CAPI_VERSION_MINOR)

/************************************************************************
 *
 * (Abstract) type definitions
 *
 ************************************************************************/

typedef void (*GEOSMessageHandler)(const char *fmt, ...);

/* When we're included by geos_c.cpp, those are #defined to the original
 * JTS definitions via preprocessor. We don't touch them to allow the
 * compiler to cross-check the declarations. However, for all "normal" 
 * C-API users, we need to define them as "opaque" struct pointers, as 
 * those clients don't have access to the original C++ headers, by design.
 */
#ifndef GEOSGeometry
typedef struct GEOSGeom_t GEOSGeometry;
typedef struct GEOSPrepGeom_t GEOSPreparedGeometry;
typedef struct GEOSCoordSeq_t GEOSCoordSequence;
typedef struct GEOSSTRtree_t GEOSSTRtree;
#endif

/* Those are compatibility definitions for source compatibility
 * with GEOS 2.X clients relying on that type.
 */
typedef GEOSGeometry* GEOSGeom;
typedef GEOSCoordSequence* GEOSCoordSeq;

/* Supported geometry types
 * This was renamed from GEOSGeomTypeId in GEOS 2.2.X, which might
 * break compatibility, this issue is still under investigation.
 */

enum GEOSGeomTypes {
    GEOS_POINT,
    GEOS_LINESTRING,
    GEOS_LINEARRING,
    GEOS_POLYGON,
    GEOS_MULTIPOINT,
    GEOS_MULTILINESTRING,
    GEOS_MULTIPOLYGON,
    GEOS_GEOMETRYCOLLECTION
};

/* Byte oders exposed via the c api */
enum GEOSByteOrders {
    GEOS_WKB_XDR = 0, /* Big Endian */
    GEOS_WKB_NDR = 1 /* Little Endian */
};

typedef struct GEOSContextHandle_HS *GEOSContextHandle_t;

typedef void (*GEOSQueryCallback)(void *item, void *userdata);

/************************************************************************
 *
 * Initialization, cleanup, version
 *
 ***********************************************************************/

#include <geos/export.h>

extern void __declspec(dllexport) initGEOS(GEOSMessageHandler notice_function,
	GEOSMessageHandler error_function);
extern void __declspec(dllexport) finishGEOS(void);

extern GEOSContextHandle_t __declspec(dllexport) initGEOS_r(
                                    GEOSMessageHandler notice_function,
                                    GEOSMessageHandler error_function);
extern void __declspec(dllexport) finishGEOS_r(GEOSContextHandle_t handle);

extern const char __declspec(dllexport) *GEOSversion();


/************************************************************************
 *
 * NOTE - These functions are DEPRECATED.  Please use the new Reader and
 * writer APIS!
 *
 ***********************************************************************/

extern GEOSGeometry __declspec(dllexport) *GEOSGeomFromWKT(const char *wkt);
extern char __declspec(dllexport) *GEOSGeomToWKT(const GEOSGeometry* g);

extern GEOSGeometry __declspec(dllexport) *GEOSGeomFromWKT_r(GEOSContextHandle_t handle,
                                                const char *wkt);
extern char __declspec(dllexport) *GEOSGeomToWKT_r(GEOSContextHandle_t handle,
                                      const GEOSGeometry* g);

/*
 * Specify whether output WKB should be 2d or 3d.
 * Return previously set number of dimensions.
 */
extern int __declspec(dllexport) GEOS_getWKBOutputDims();
extern int __declspec(dllexport) GEOS_setWKBOutputDims(int newDims);

extern int __declspec(dllexport) GEOS_getWKBOutputDims_r(GEOSContextHandle_t handle);
extern int __declspec(dllexport) GEOS_setWKBOutputDims_r(GEOSContextHandle_t handle,
                                            int newDims);

/*
 * Specify whether the WKB byte order is big or little endian. 
 * The return value is the previous byte order.
 */
extern int __declspec(dllexport) GEOS_getWKBByteOrder();
extern int __declspec(dllexport) GEOS_setWKBByteOrder(int byteOrder);

extern GEOSGeometry __declspec(dllexport) *GEOSGeomFromWKB_buf(const unsigned char *wkb, size_t size);
extern unsigned char __declspec(dllexport) *GEOSGeomToWKB_buf(const GEOSGeometry* g, size_t *size);

extern GEOSGeometry __declspec(dllexport) *GEOSGeomFromHEX_buf(const unsigned char *hex, size_t size);
extern unsigned char __declspec(dllexport) *GEOSGeomToHEX_buf(const GEOSGeometry* g, size_t *size);

extern int __declspec(dllexport) GEOS_getWKBByteOrder_r(GEOSContextHandle_t handle);
extern int __declspec(dllexport) GEOS_setWKBByteOrder_r(GEOSContextHandle_t handle,
                                           int byteOrder);

extern GEOSGeometry __declspec(dllexport) *GEOSGeomFromWKB_buf_r(GEOSContextHandle_t handle,
                                                    const unsigned char *wkb,
                                                    size_t size);
extern unsigned char __declspec(dllexport) *GEOSGeomToWKB_buf_r(GEOSContextHandle_t handle,
                                                   const GEOSGeometry* g,
                                                   size_t *size);

extern GEOSGeometry __declspec(dllexport) *GEOSGeomFromHEX_buf_r(GEOSContextHandle_t handle,
                                                    const unsigned char *hex,
                                                    size_t size);
extern unsigned char __declspec(dllexport) *GEOSGeomToHEX_buf_r(GEOSContextHandle_t handle,
                                                   const GEOSGeometry* g,
                                                   size_t *size);

/************************************************************************
 *
 * Coordinate Sequence functions
 *
 ***********************************************************************/

/*
 * Create a Coordinate sequence with ``size'' coordinates
 * of ``dims'' dimensions.
 * Return NULL on exception.
 */
extern GEOSCoordSequence __declspec(dllexport) *GEOSCoordSeq_create(unsigned int size, unsigned int dims);

extern GEOSCoordSequence __declspec(dllexport) *GEOSCoordSeq_create_r(
                                                GEOSContextHandle_t handle,
                                                unsigned int size,
                                                unsigned int dims);

/*
 * Clone a Coordinate Sequence.
 * Return NULL on exception.
 */
extern GEOSCoordSequence __declspec(dllexport) *GEOSCoordSeq_clone(const GEOSCoordSequence* s);

extern GEOSCoordSequence __declspec(dllexport) *GEOSCoordSeq_clone_r(
                                                GEOSContextHandle_t handle,
                                                const GEOSCoordSequence* s);

/*
 * Destroy a Coordinate Sequence.
 */
extern void __declspec(dllexport) GEOSCoordSeq_destroy(GEOSCoordSequence* s);

extern void __declspec(dllexport) GEOSCoordSeq_destroy_r(GEOSContextHandle_t handle,
                                            GEOSCoordSequence* s);

/*
 * Set ordinate values in a Coordinate Sequence.
 * Return 0 on exception.
 */
extern int __declspec(dllexport) GEOSCoordSeq_setX(GEOSCoordSequence* s,
	unsigned int idx, double val);
extern int __declspec(dllexport) GEOSCoordSeq_setY(GEOSCoordSequence* s,
	unsigned int idx, double val);
extern int __declspec(dllexport) GEOSCoordSeq_setZ(GEOSCoordSequence* s,
	unsigned int idx, double val);
extern int __declspec(dllexport) GEOSCoordSeq_setOrdinate(GEOSCoordSequence* s,
	unsigned int idx, unsigned int dim, double val);

extern int __declspec(dllexport) GEOSCoordSeq_setX_r(GEOSContextHandle_t handle,
                                        GEOSCoordSequence* s, unsigned int idx,
                                        double val);
extern int __declspec(dllexport) GEOSCoordSeq_setY_r(GEOSContextHandle_t handle,
                                        GEOSCoordSequence* s, unsigned int idx,
                                        double val);
extern int __declspec(dllexport) GEOSCoordSeq_setZ_r(GEOSContextHandle_t handle,
                                        GEOSCoordSequence* s, unsigned int idx,
                                        double val);
extern int __declspec(dllexport) GEOSCoordSeq_setOrdinate_r(GEOSContextHandle_t handle,
                                               GEOSCoordSequence* s,
                                               unsigned int idx,
                                               unsigned int dim, double val);

/*
 * Get ordinate values from a Coordinate Sequence.
 * Return 0 on exception.
 */
extern int __declspec(dllexport) GEOSCoordSeq_getX(const GEOSCoordSequence* s,
	unsigned int idx, double *val);
extern int __declspec(dllexport) GEOSCoordSeq_getY(const GEOSCoordSequence* s,
	unsigned int idx, double *val);
extern int __declspec(dllexport) GEOSCoordSeq_getZ(const GEOSCoordSequence* s,
	unsigned int idx, double *val);
extern int __declspec(dllexport) GEOSCoordSeq_getOrdinate(const GEOSCoordSequence* s,
	unsigned int idx, unsigned int dim, double *val);

extern int __declspec(dllexport) GEOSCoordSeq_getX_r(GEOSContextHandle_t handle,
                                        const GEOSCoordSequence* s,
                                        unsigned int idx, double *val);
extern int __declspec(dllexport) GEOSCoordSeq_getY_r(GEOSContextHandle_t handle,
                                        const GEOSCoordSequence* s,
                                        unsigned int idx, double *val);
extern int __declspec(dllexport) GEOSCoordSeq_getZ_r(GEOSContextHandle_t handle,
                                        const GEOSCoordSequence* s,
                                        unsigned int idx, double *val);
extern int __declspec(dllexport) GEOSCoordSeq_getOrdinate_r(GEOSContextHandle_t handle,
                                               const GEOSCoordSequence* s,
                                               unsigned int idx,
                                               unsigned int dim, double *val);
/*
 * Get size and dimensions info from a Coordinate Sequence.
 * Return 0 on exception.
 */
extern int __declspec(dllexport) GEOSCoordSeq_getSize(const GEOSCoordSequence* s,
	unsigned int *size);
extern int __declspec(dllexport) GEOSCoordSeq_getDimensions(const GEOSCoordSequence* s,
	unsigned int *dims);

extern int __declspec(dllexport) GEOSCoordSeq_getSize_r(GEOSContextHandle_t handle,
                                           const GEOSCoordSequence* s,
                                           unsigned int *size);
extern int __declspec(dllexport) GEOSCoordSeq_getDimensions_r(GEOSContextHandle_t handle,
                                                 const GEOSCoordSequence* s,
                                                 unsigned int *dims);

/************************************************************************
 *
 *  Linearref functions -- there are more, but these two are probably
 *  sufficient for most purposes
 *
 ***********************************************************************/

/* 
 * GEOSGeometry ownership is retained by caller
 */


extern double __declspec(dllexport) GEOSProject(const GEOSGeometry *g,
                                   const GEOSGeometry* p);
extern double __declspec(dllexport) GEOSProject_r(GEOSContextHandle_t handle,
                                     const GEOSGeometry *g,
                                     const GEOSGeometry *p);

extern GEOSGeometry __declspec(dllexport) *GEOSInterpolate(const GEOSGeometry *g,
                                              double d);
extern GEOSGeometry __declspec(dllexport) *GEOSInterpolate_r(GEOSContextHandle_t handle,
                                                const GEOSGeometry *g,
                                                double d);

extern double __declspec(dllexport) GEOSProjectNormalized(const GEOSGeometry *g,
                                             const GEOSGeometry* p);
extern double __declspec(dllexport) GEOSProjectNormalized_r(GEOSContextHandle_t handle,
                                               const GEOSGeometry *g,
                                               const GEOSGeometry *p);

extern GEOSGeometry __declspec(dllexport) *GEOSInterpolateNormalized(const GEOSGeometry *g,
                                                        double d);
extern GEOSGeometry __declspec(dllexport) *GEOSInterpolateNormalized_r(
                                                GEOSContextHandle_t handle,
                                                const GEOSGeometry *g,
                                                double d);

/************************************************************************
 *
 * Buffer related functions
 *
 ***********************************************************************/

enum GEOSBufCapStyles {
	GEOSBUF_CAP_ROUND=1,
	GEOSBUF_CAP_FLAT=2,
	GEOSBUF_CAP_SQUARE=3
};

enum GEOSBufJoinStyles {
	GEOSBUF_JOIN_ROUND=1,
	GEOSBUF_JOIN_MITRE=2,
	GEOSBUF_JOIN_BEVEL=3
};

/* These functions return NULL on exception. */
extern GEOSGeometry __declspec(dllexport) *GEOSBuffer(const GEOSGeometry* g1,
	double width, int quadsegs);
extern GEOSGeometry __declspec(dllexport) *GEOSBuffer_r(GEOSContextHandle_t handle,
                                           const GEOSGeometry* g1,
                                           double width, int quadsegs);

/* These functions return NULL on exception. */
extern GEOSGeometry __declspec(dllexport) *GEOSBufferWithStyle(const GEOSGeometry* g1,
	double width, int quadsegs, int endCapStyle, int joinStyle,
	double mitreLimit);
extern GEOSGeometry __declspec(dllexport) *GEOSBufferWithStyle_r(GEOSContextHandle_t handle,
	const GEOSGeometry* g1, double width, int quadsegs, int endCapStyle,
	int joinStyle, double mitreLimit);

/* These functions return NULL on exception. Only LINESTRINGs are accepted. */
extern GEOSGeometry __declspec(dllexport) *GEOSSingleSidedBuffer(const GEOSGeometry* g1,
	double width, int quadsegs, int joinStyle, double mitreLimit,
	int leftSide);
extern GEOSGeometry __declspec(dllexport) *GEOSSingleSidedBuffer_r(
	GEOSContextHandle_t handle,
	const GEOSGeometry* g1, double width, int quadsegs, 
	int joinStyle, double mitreLimit, int leftSide);


/************************************************************************
 *
 * Geometry Constructors.
 * GEOSCoordSequence* arguments will become ownership of the returned object.
 * All functions return NULL on exception.
 *
 ***********************************************************************/

extern GEOSGeometry __declspec(dllexport) *GEOSGeom_createPoint(GEOSCoordSequence* s);
extern GEOSGeometry __declspec(dllexport) *GEOSGeom_createLinearRing(GEOSCoordSequence* s);
extern GEOSGeometry __declspec(dllexport) *GEOSGeom_createLineString(GEOSCoordSequence* s);

extern GEOSGeometry __declspec(dllexport) *GEOSGeom_createPoint_r(
                                       GEOSContextHandle_t handle,
                                       GEOSCoordSequence* s);
extern GEOSGeometry __declspec(dllexport) *GEOSGeom_createLinearRing_r(
                                       GEOSContextHandle_t handle,
                                       GEOSCoordSequence* s);
extern GEOSGeometry __declspec(dllexport) *GEOSGeom_createLineString_r(
                                       GEOSContextHandle_t handle,
                                       GEOSCoordSequence* s);

/*
 * Second argument is an array of GEOSGeometry* objects.
 * The caller remains owner of the array, but pointed-to
 * objects become ownership of the returned GEOSGeometry.
 */
extern GEOSGeometry __declspec(dllexport) *GEOSGeom_createPolygon(GEOSGeometry* shell,
	GEOSGeometry** holes, unsigned int nholes);
extern GEOSGeometry __declspec(dllexport) *GEOSGeom_createCollection(int type,
	GEOSGeometry* *geoms, unsigned int ngeoms);

extern GEOSGeometry __declspec(dllexport) *GEOSGeom_createPolygon_r(
                                       GEOSContextHandle_t handle,
                                       GEOSGeometry* shell,
                                       GEOSGeometry** holes,
                                       unsigned int nholes);
extern GEOSGeometry __declspec(dllexport) *GEOSGeom_createCollection_r(
                                       GEOSContextHandle_t handle, int type,
                                       GEOSGeometry* *geoms,
                                       unsigned int ngeoms);

extern GEOSGeometry __declspec(dllexport) *GEOSGeom_clone(const GEOSGeometry* g);

extern GEOSGeometry __declspec(dllexport) *GEOSGeom_clone_r(GEOSContextHandle_t handle,
                                               const GEOSGeometry* g);

/************************************************************************
 *
 * Memory management
 *
 ***********************************************************************/

extern void __declspec(dllexport) GEOSGeom_destroy(GEOSGeometry* g);

extern void __declspec(dllexport) GEOSGeom_destroy_r(GEOSContextHandle_t handle,
                                        GEOSGeometry* g);

/************************************************************************
 *
 * Topology operations - return NULL on exception.
 *
 ***********************************************************************/

extern GEOSGeometry __declspec(dllexport) *GEOSEnvelope(const GEOSGeometry* g1);
extern GEOSGeometry __declspec(dllexport) *GEOSIntersection(const GEOSGeometry* g1, const GEOSGeometry* g2);
extern GEOSGeometry __declspec(dllexport) *GEOSConvexHull(const GEOSGeometry* g1);
extern GEOSGeometry __declspec(dllexport) *GEOSDifference(const GEOSGeometry* g1, const GEOSGeometry* g2);
extern GEOSGeometry __declspec(dllexport) *GEOSSymDifference(const GEOSGeometry* g1,
	const GEOSGeometry* g2);
extern GEOSGeometry __declspec(dllexport) *GEOSBoundary(const GEOSGeometry* g1);
extern GEOSGeometry __declspec(dllexport) *GEOSUnion(const GEOSGeometry* g1, const GEOSGeometry* g2);
extern GEOSGeometry __declspec(dllexport) *GEOSUnionCascaded(const GEOSGeometry* g1);

extern GEOSGeometry __declspec(dllexport) *GEOSPointOnSurface(const GEOSGeometry* g1);
extern GEOSGeometry __declspec(dllexport) *GEOSGetCentroid(const GEOSGeometry* g);
extern char __declspec(dllexport) *GEOSRelate(const GEOSGeometry* g1, const GEOSGeometry* g2);

extern GEOSGeometry __declspec(dllexport) *GEOSEnvelope_r(GEOSContextHandle_t handle,
                                             const GEOSGeometry* g1);
extern GEOSGeometry __declspec(dllexport) *GEOSIntersection_r(GEOSContextHandle_t handle,
                                                 const GEOSGeometry* g1,
                                                 const GEOSGeometry* g2);
extern GEOSGeometry __declspec(dllexport) *GEOSConvexHull_r(GEOSContextHandle_t handle,
                                               const GEOSGeometry* g1);
extern GEOSGeometry __declspec(dllexport) *GEOSDifference_r(GEOSContextHandle_t handle,
                                               const GEOSGeometry* g1,
                                               const GEOSGeometry* g2);
extern GEOSGeometry __declspec(dllexport) *GEOSSymDifference_r(GEOSContextHandle_t handle,
                                                  const GEOSGeometry* g1,
                                                  const GEOSGeometry* g2);
extern GEOSGeometry __declspec(dllexport) *GEOSBoundary_r(GEOSContextHandle_t handle,
                                             const GEOSGeometry* g1);
extern GEOSGeometry __declspec(dllexport) *GEOSUnion_r(GEOSContextHandle_t handle,
                                          const GEOSGeometry* g1,
                                          const GEOSGeometry* g2);
extern GEOSGeometry __declspec(dllexport) *GEOSUnionCascaded_r(GEOSContextHandle_t handle, const GEOSGeometry* g1);
extern GEOSGeometry __declspec(dllexport) *GEOSPointOnSurface_r(GEOSContextHandle_t handle,
                                                   const GEOSGeometry* g1);
extern GEOSGeometry __declspec(dllexport) *GEOSGetCentroid_r(GEOSContextHandle_t handle,
                                                const GEOSGeometry* g);
extern char __declspec(dllexport) *GEOSRelate_r(GEOSContextHandle_t handle,
                                   const GEOSGeometry* g1,
                                   const GEOSGeometry* g2);

/*
 * all arguments remain ownership of the caller
 * (both Geometries and pointers)
 */
extern GEOSGeometry __declspec(dllexport) *GEOSPolygonize(const GEOSGeometry * const geoms[], unsigned int ngeoms);
extern GEOSGeometry __declspec(dllexport) *GEOSPolygonizer_getCutEdges(const GEOSGeometry * const geoms[], unsigned int ngeoms);

extern GEOSGeometry __declspec(dllexport) *GEOSLineMerge(const GEOSGeometry* g);
extern GEOSGeometry __declspec(dllexport) *GEOSSimplify(const GEOSGeometry* g1, double tolerance);
extern GEOSGeometry __declspec(dllexport) *GEOSTopologyPreserveSimplify(const GEOSGeometry* g1,
	double tolerance);

extern GEOSGeometry __declspec(dllexport) *GEOSPolygonize_r(GEOSContextHandle_t handle,
                              const GEOSGeometry *const geoms[],
                              unsigned int ngeoms);
extern GEOSGeometry __declspec(dllexport) *GEOSPolygonizer_getCutEdges_r(
                              GEOSContextHandle_t handle,
                              const GEOSGeometry * const geoms[],
                              unsigned int ngeoms);

extern GEOSGeometry __declspec(dllexport) *GEOSLineMerge_r(GEOSContextHandle_t handle,
                                              const GEOSGeometry* g);
extern GEOSGeometry __declspec(dllexport) *GEOSSimplify_r(GEOSContextHandle_t handle,
                                             const GEOSGeometry* g1,
                                             double tolerance);
extern GEOSGeometry __declspec(dllexport) *GEOSTopologyPreserveSimplify_r(
                              GEOSContextHandle_t handle,
                              const GEOSGeometry* g1, double tolerance);

/************************************************************************
 *
 *  Binary predicates - return 2 on exception, 1 on true, 0 on false
 *
 ***********************************************************************/

extern char __declspec(dllexport) GEOSRelatePattern(const GEOSGeometry* g1, const GEOSGeometry* g2,
	const char *pat);
extern char __declspec(dllexport) GEOSDisjoint(const GEOSGeometry* g1, const GEOSGeometry* g2);
extern char __declspec(dllexport) GEOSTouches(const GEOSGeometry* g1, const GEOSGeometry* g2);
extern char __declspec(dllexport) GEOSIntersects(const GEOSGeometry* g1, const GEOSGeometry* g2);
extern char __declspec(dllexport) GEOSCrosses(const GEOSGeometry* g1, const GEOSGeometry* g2);
extern char __declspec(dllexport) GEOSWithin(const GEOSGeometry* g1, const GEOSGeometry* g2);
extern char __declspec(dllexport) GEOSContains(const GEOSGeometry* g1, const GEOSGeometry* g2);
extern char __declspec(dllexport) GEOSOverlaps(const GEOSGeometry* g1, const GEOSGeometry* g2);
extern char __declspec(dllexport) GEOSEquals(const GEOSGeometry* g1, const GEOSGeometry* g2);
extern char __declspec(dllexport) GEOSEqualsExact(const GEOSGeometry* g1, const GEOSGeometry* g2, double tolerance);

extern char __declspec(dllexport) GEOSRelatePattern_r(GEOSContextHandle_t handle,
                                         const GEOSGeometry* g1,
                                         const GEOSGeometry* g2,
                                         const char *pat);
extern char __declspec(dllexport) GEOSDisjoint_r(GEOSContextHandle_t handle,
                                    const GEOSGeometry* g1,
                                    const GEOSGeometry* g2);
extern char __declspec(dllexport) GEOSTouches_r(GEOSContextHandle_t handle,
                                   const GEOSGeometry* g1,
                                   const GEOSGeometry* g2);
extern char __declspec(dllexport) GEOSIntersects_r(GEOSContextHandle_t handle,
                                      const GEOSGeometry* g1,
                                      const GEOSGeometry* g2);
extern char __declspec(dllexport) GEOSCrosses_r(GEOSContextHandle_t handle,
                                   const GEOSGeometry* g1,
                                   const GEOSGeometry* g2);
extern char __declspec(dllexport) GEOSWithin_r(GEOSContextHandle_t handle,
                                  const GEOSGeometry* g1,
                                  const GEOSGeometry* g2);
extern char __declspec(dllexport) GEOSContains_r(GEOSContextHandle_t handle,
                                    const GEOSGeometry* g1,
                                    const GEOSGeometry* g2);
extern char __declspec(dllexport) GEOSOverlaps_r(GEOSContextHandle_t handle,
                                    const GEOSGeometry* g1,
                                    const GEOSGeometry* g2);
extern char __declspec(dllexport) GEOSEquals_r(GEOSContextHandle_t handle,
                                  const GEOSGeometry* g1,
                                  const GEOSGeometry* g2);
extern char __declspec(dllexport) GEOSEqualsExact_r(GEOSContextHandle_t handle,
                                       const GEOSGeometry* g1,
                                       const GEOSGeometry* g2,
                                       double tolerance);

/************************************************************************
 *
 *  Prepared Geometry Binary predicates - return 2 on exception, 1 on true, 0 on false
 *
 ***********************************************************************/

/* 
 * GEOSGeometry ownership is retained by caller
 */
extern const GEOSPreparedGeometry __declspec(dllexport) *GEOSPrepare(const GEOSGeometry* g);

extern void __declspec(dllexport) GEOSPreparedGeom_destroy(const GEOSPreparedGeometry* g);

extern char __declspec(dllexport) GEOSPreparedContains(const GEOSPreparedGeometry* pg1, const GEOSGeometry* g2);
extern char __declspec(dllexport) GEOSPreparedContainsProperly(const GEOSPreparedGeometry* pg1, const GEOSGeometry* g2);
extern char __declspec(dllexport) GEOSPreparedCovers(const GEOSPreparedGeometry* pg1, const GEOSGeometry* g2);
extern char __declspec(dllexport) GEOSPreparedIntersects(const GEOSPreparedGeometry* pg1, const GEOSGeometry* g2);

/* 
 * GEOSGeometry ownership is retained by caller
 */
extern const GEOSPreparedGeometry __declspec(dllexport) *GEOSPrepare_r(
                                            GEOSContextHandle_t handle,
                                            const GEOSGeometry* g);

extern void __declspec(dllexport) GEOSPreparedGeom_destroy_r(GEOSContextHandle_t handle,
                                                const GEOSPreparedGeometry* g);

extern char __declspec(dllexport) GEOSPreparedContains_r(GEOSContextHandle_t handle,
                                            const GEOSPreparedGeometry* pg1,
                                            const GEOSGeometry* g2);
extern char __declspec(dllexport) GEOSPreparedContainsProperly_r(GEOSContextHandle_t handle,
                                         const GEOSPreparedGeometry* pg1,
                                         const GEOSGeometry* g2);
extern char __declspec(dllexport) GEOSPreparedCovers_r(GEOSContextHandle_t handle,
                                          const GEOSPreparedGeometry* pg1,
                                          const GEOSGeometry* g2);
extern char __declspec(dllexport) GEOSPreparedIntersects_r(GEOSContextHandle_t handle,
                                              const GEOSPreparedGeometry* pg1,
                                              const GEOSGeometry* g2);

/************************************************************************
 *
 *  STRtree functions
 *
 ***********************************************************************/

/* 
 * GEOSGeometry ownership is retained by caller
 */

extern GEOSSTRtree __declspec(dllexport) *GEOSSTRtree_create(size_t nodeCapacity);
extern void __declspec(dllexport) GEOSSTRtree_insert(GEOSSTRtree *tree,
                                        const GEOSGeometry *g,
                                        void *item);
extern void __declspec(dllexport) GEOSSTRtree_query(GEOSSTRtree *tree,
                                       const GEOSGeometry *g,
                                       GEOSQueryCallback callback,
                                       void *userdata);
extern void __declspec(dllexport) GEOSSTRtree_iterate(GEOSSTRtree *tree,
                                       GEOSQueryCallback callback,
                                       void *userdata);
extern char __declspec(dllexport) GEOSSTRtree_remove(GEOSSTRtree *tree,
                                        const GEOSGeometry *g,
                                        void *item);
extern void __declspec(dllexport) GEOSSTRtree_destroy(GEOSSTRtree *tree);


extern GEOSSTRtree __declspec(dllexport) *GEOSSTRtree_create_r(
                                    GEOSContextHandle_t handle,
                                    size_t nodeCapacity);
extern void __declspec(dllexport) GEOSSTRtree_insert_r(GEOSContextHandle_t handle,
                                          GEOSSTRtree *tree,
                                          const GEOSGeometry *g,
                                          void *item);
extern void __declspec(dllexport) GEOSSTRtree_query_r(GEOSContextHandle_t handle,
                                         GEOSSTRtree *tree,
                                         const GEOSGeometry *g,
                                         GEOSQueryCallback callback,
                                         void *userdata);
extern void __declspec(dllexport) GEOSSTRtree_iterate_r(GEOSContextHandle_t handle,
                                       GEOSSTRtree *tree,
                                       GEOSQueryCallback callback,
                                       void *userdata);
extern char __declspec(dllexport) GEOSSTRtree_remove_r(GEOSContextHandle_t handle,
                                          GEOSSTRtree *tree,
                                          const GEOSGeometry *g,
                                          void *item);
extern void __declspec(dllexport) GEOSSTRtree_destroy_r(GEOSContextHandle_t handle,
                                           GEOSSTRtree *tree);


/************************************************************************
 *
 *  Unary predicate - return 2 on exception, 1 on true, 0 on false
 *
 ***********************************************************************/

extern char __declspec(dllexport) GEOSisEmpty(const GEOSGeometry* g1);
extern char __declspec(dllexport) GEOSisValid(const GEOSGeometry* g1);
extern char __declspec(dllexport) *GEOSisValidReason(const GEOSGeometry *g1);
extern char __declspec(dllexport) GEOSisSimple(const GEOSGeometry* g1);
extern char __declspec(dllexport) GEOSisRing(const GEOSGeometry* g1);
extern char __declspec(dllexport) GEOSHasZ(const GEOSGeometry* g1);

extern char __declspec(dllexport) GEOSisEmpty_r(GEOSContextHandle_t handle,
                                   const GEOSGeometry* g1);
extern char __declspec(dllexport) GEOSisValid_r(GEOSContextHandle_t handle,
                                   const GEOSGeometry* g1);
extern char __declspec(dllexport) *GEOSisValidReason_r(GEOSContextHandle_t handle,
                                         const GEOSGeometry* g1);
extern char __declspec(dllexport) GEOSisSimple_r(GEOSContextHandle_t handle,
                                    const GEOSGeometry* g1);
extern char __declspec(dllexport) GEOSisRing_r(GEOSContextHandle_t handle,
                                  const GEOSGeometry* g1);
extern char __declspec(dllexport) GEOSHasZ_r(GEOSContextHandle_t handle,
                                const GEOSGeometry* g1);

/************************************************************************
 *
 *  Geometry info
 *
 ***********************************************************************/

/* Return NULL on exception, result must be freed by caller. */
extern char __declspec(dllexport) *GEOSGeomType(const GEOSGeometry* g1);

extern char __declspec(dllexport) *GEOSGeomType_r(GEOSContextHandle_t handle,
                                     const GEOSGeometry* g1);

/* Return -1 on exception */
extern int __declspec(dllexport) GEOSGeomTypeId(const GEOSGeometry* g1);

extern int __declspec(dllexport) GEOSGeomTypeId_r(GEOSContextHandle_t handle,
                                     const GEOSGeometry* g1);

/* Return 0 on exception */
extern int __declspec(dllexport) GEOSGetSRID(const GEOSGeometry* g);
extern int __declspec(dllexport) GEOSGetSRID_r(GEOSContextHandle_t handle,
                                  const GEOSGeometry* g);

extern void __declspec(dllexport) GEOSSetSRID(GEOSGeometry* g, int SRID);
extern void __declspec(dllexport) GEOSSetSRID_r(GEOSContextHandle_t handle,
                                   GEOSGeometry* g, int SRID);

/* May be called on all geometries in GEOS 3.x, returns -1 on error and 1
 * for non-multi geometries. Older GEOS versions only accept 
 * GeometryCollections or Multi* geometries here, and are likely to crash
 * when feeded simple geometries, so beware if you need compatibility with
 * old GEOS versions.
 */
extern int __declspec(dllexport) GEOSGetNumGeometries(const GEOSGeometry* g);

extern int __declspec(dllexport) GEOSGetNumGeometries_r(GEOSContextHandle_t handle,
                                           const GEOSGeometry* g);

/*
 * Return NULL on exception, Geometry must be a Collection.
 * Returned object is a pointer to internal storage:
 * it must NOT be destroyed directly.
 */
extern const GEOSGeometry __declspec(dllexport) *GEOSGetGeometryN(const GEOSGeometry* g, int n);

extern const GEOSGeometry __declspec(dllexport) *GEOSGetGeometryN_r(
                                    GEOSContextHandle_t handle,
                                    const GEOSGeometry* g, int n);

/* Return -1 on exception */
extern int __declspec(dllexport) GEOSNormalize(GEOSGeometry* g1);

extern int __declspec(dllexport) GEOSNormalize_r(GEOSContextHandle_t handle,
                                    GEOSGeometry* g1);

/* Return -1 on exception */
extern int __declspec(dllexport) GEOSGetNumInteriorRings(const GEOSGeometry* g1);

extern int __declspec(dllexport) GEOSGetNumInteriorRings_r(GEOSContextHandle_t handle,
                                              const GEOSGeometry* g1);

/*
 * Return NULL on exception, Geometry must be a Polygon.
 * Returned object is a pointer to internal storage:
 * it must NOT be destroyed directly.
 */
extern const GEOSGeometry __declspec(dllexport) *GEOSGetInteriorRingN(const GEOSGeometry* g, int n);

extern const GEOSGeometry __declspec(dllexport) *GEOSGetInteriorRingN_r(
                                    GEOSContextHandle_t handle,
                                    const GEOSGeometry* g, int n);

/*
 * Return NULL on exception, Geometry must be a Polygon.
 * Returned object is a pointer to internal storage:
 * it must NOT be destroyed directly.
 */
extern const GEOSGeometry __declspec(dllexport) *GEOSGetExteriorRing(const GEOSGeometry* g);

extern const GEOSGeometry __declspec(dllexport) *GEOSGetExteriorRing_r(
                                    GEOSContextHandle_t handle,
                                    const GEOSGeometry* g);

/* Return -1 on exception */
extern int __declspec(dllexport) GEOSGetNumCoordinates(const GEOSGeometry* g1);

extern int __declspec(dllexport) GEOSGetNumCoordinates_r(GEOSContextHandle_t handle,
                                            const GEOSGeometry* g1);

/*
 * Return NULL on exception.
 * Geometry must be a LineString, LinearRing or Point.
 */
extern const GEOSCoordSequence __declspec(dllexport) *GEOSGeom_getCoordSeq(const GEOSGeometry* g);

extern const GEOSCoordSequence __declspec(dllexport) *GEOSGeom_getCoordSeq_r(
                                         GEOSContextHandle_t handle,
                                         const GEOSGeometry* g);

/*
 * Return 0 on exception (or empty geometry)
 * See also GEOSCoordSeq_getDimensions 
 */
extern int __declspec(dllexport) GEOSGeom_getDimensions(const GEOSGeometry* g);

extern int __declspec(dllexport) GEOSGeom_getDimensions_r(GEOSContextHandle_t handle,
                                             const GEOSGeometry* g);

/************************************************************************
 *
 *  Misc functions 
 *
 ***********************************************************************/

/* Return 0 on exception, 1 otherwise */
extern int __declspec(dllexport) GEOSArea(const GEOSGeometry* g1, double *area);
extern int __declspec(dllexport) GEOSLength(const GEOSGeometry* g1, double *length);
extern int __declspec(dllexport) GEOSDistance(const GEOSGeometry* g1, const GEOSGeometry* g2,
	double *dist);
extern int __declspec(dllexport) GEOSHausdorffDistance(const GEOSGeometry *g1,
        const GEOSGeometry *g2, double *dist);
extern int __declspec(dllexport) GEOSHausdorffDistanceDensify(const GEOSGeometry *g1,
        const GEOSGeometry *g2, double densifyFrac, double *dist);
extern int __declspec(dllexport) GEOSArea_r(GEOSContextHandle_t handle,
                               const GEOSGeometry* g1, double *area);
extern int __declspec(dllexport) GEOSLength_r(GEOSContextHandle_t handle,
                                 const GEOSGeometry* g1, double *length);
extern int __declspec(dllexport) GEOSDistance_r(GEOSContextHandle_t handle,
                                   const GEOSGeometry* g1,
                                   const GEOSGeometry* g2, double *dist);
extern int __declspec(dllexport) GEOSHausdorffDistance_r(GEOSContextHandle_t handle,
                                   const GEOSGeometry *g1,
                                   const GEOSGeometry *g2,
                                   double *dist);
extern int __declspec(dllexport) GEOSHausdorffDistanceDensify_r(GEOSContextHandle_t handle,
                                   const GEOSGeometry *g1,
                                   const GEOSGeometry *g2,
                                   double densifyFrac, double *dist);


/************************************************************************
 *
 * Reader and Writer APIs
 *
 ***********************************************************************/

typedef struct GEOSWKTReader_t GEOSWKTReader;
typedef struct GEOSWKTWriter_t GEOSWKTWriter;
typedef struct GEOSWKBReader_t GEOSWKBReader;
typedef struct GEOSWKBWriter_t GEOSWKBWriter;


/* WKT Reader */
extern GEOSWKTReader __declspec(dllexport) *GEOSWKTReader_create();
extern void __declspec(dllexport) GEOSWKTReader_destroy(GEOSWKTReader* reader);
extern GEOSGeometry __declspec(dllexport) *GEOSWKTReader_read(GEOSWKTReader* reader, const char *wkt);

extern GEOSWKTReader __declspec(dllexport) *GEOSWKTReader_create_r(
                                             GEOSContextHandle_t handle);
extern void __declspec(dllexport) GEOSWKTReader_destroy_r(GEOSContextHandle_t handle,
                                             GEOSWKTReader* reader);
extern GEOSGeometry __declspec(dllexport) *GEOSWKTReader_read_r(GEOSContextHandle_t handle,
                                                   GEOSWKTReader* reader,
                                                   const char *wkt);

/* WKT Writer */
extern GEOSWKTWriter __declspec(dllexport) *GEOSWKTWriter_create();
extern void __declspec(dllexport) GEOSWKTWriter_destroy(GEOSWKTWriter* writer);
extern char __declspec(dllexport) *GEOSWKTWriter_write(GEOSWKTWriter* reader, const GEOSGeometry* g);

extern GEOSWKTWriter __declspec(dllexport) *GEOSWKTWriter_create_r(
                                             GEOSContextHandle_t handle);
extern void __declspec(dllexport) GEOSWKTWriter_destroy_r(GEOSContextHandle_t handle,
                                             GEOSWKTWriter* writer);
extern char __declspec(dllexport) *GEOSWKTWriter_write_r(GEOSContextHandle_t handle,
                                            GEOSWKTWriter* reader,
                                            const GEOSGeometry* g);

/* WKB Reader */
extern GEOSWKBReader __declspec(dllexport) *GEOSWKBReader_create();
extern void __declspec(dllexport) GEOSWKBReader_destroy(GEOSWKBReader* reader);
extern GEOSGeometry __declspec(dllexport) *GEOSWKBReader_read(GEOSWKBReader* reader, const unsigned char *wkb, size_t size);
extern GEOSGeometry __declspec(dllexport) *GEOSWKBReader_readHEX(GEOSWKBReader* reader, const unsigned char *hex, size_t size);

extern GEOSWKBReader __declspec(dllexport) *GEOSWKBReader_create_r(
                                             GEOSContextHandle_t handle);
extern void __declspec(dllexport) GEOSWKBReader_destroy_r(GEOSContextHandle_t handle,
                                             GEOSWKBReader* reader);
extern GEOSGeometry __declspec(dllexport) *GEOSWKBReader_read_r(GEOSContextHandle_t handle,
                                                   GEOSWKBReader* reader,
                                                   const unsigned char *wkb,
                                                   size_t size);
extern GEOSGeometry __declspec(dllexport) *GEOSWKBReader_readHEX_r(
                                            GEOSContextHandle_t handle,
                                            GEOSWKBReader* reader,
                                            const unsigned char *hex,
                                            size_t size);

/* WKB Writer */
extern GEOSWKBWriter __declspec(dllexport) *GEOSWKBWriter_create();
extern void __declspec(dllexport) GEOSWKBWriter_destroy(GEOSWKBWriter* writer);

extern GEOSWKBWriter __declspec(dllexport) *GEOSWKBWriter_create_r(
                                             GEOSContextHandle_t handle);
extern void __declspec(dllexport) GEOSWKBWriter_destroy_r(GEOSContextHandle_t handle,
                                             GEOSWKBWriter* writer);

/* The caller owns the results for these two methods! */
extern unsigned char __declspec(dllexport) *GEOSWKBWriter_write(GEOSWKBWriter* writer, const GEOSGeometry* g, size_t *size);
extern unsigned char __declspec(dllexport) *GEOSWKBWriter_writeHEX(GEOSWKBWriter* writer, const GEOSGeometry* g, size_t *size);

extern unsigned char __declspec(dllexport) *GEOSWKBWriter_write_r(
                                             GEOSContextHandle_t handle,
                                             GEOSWKBWriter* writer,
                                             const GEOSGeometry* g,
                                             size_t *size);
extern unsigned char __declspec(dllexport) *GEOSWKBWriter_writeHEX_r(
                                             GEOSContextHandle_t handle,
                                             GEOSWKBWriter* writer,
                                             const GEOSGeometry* g,
                                             size_t *size);

/* 
 * Specify whether output WKB should be 2d or 3d.
 * Return previously set number of dimensions.
 */
extern int __declspec(dllexport) GEOSWKBWriter_getOutputDimension(const GEOSWKBWriter* writer);
extern void __declspec(dllexport) GEOSWKBWriter_setOutputDimension(GEOSWKBWriter* writer, int newDimension);

extern int __declspec(dllexport) GEOSWKBWriter_getOutputDimension_r(
                                  GEOSContextHandle_t handle,
                                  const GEOSWKBWriter* writer);
extern void __declspec(dllexport) GEOSWKBWriter_setOutputDimension_r(
                                   GEOSContextHandle_t handle,
                                   GEOSWKBWriter* writer, int newDimension);

/*
 * Specify whether the WKB byte order is big or little endian. 
 * The return value is the previous byte order.
 */
extern int __declspec(dllexport) GEOSWKBWriter_getByteOrder(const GEOSWKBWriter* writer);
extern void __declspec(dllexport) GEOSWKBWriter_setByteOrder(GEOSWKBWriter* writer, int byteOrder);

extern int __declspec(dllexport) GEOSWKBWriter_getByteOrder_r(GEOSContextHandle_t handle,
                                                 const GEOSWKBWriter* writer);
extern void __declspec(dllexport) GEOSWKBWriter_setByteOrder_r(GEOSContextHandle_t handle,
                                                  GEOSWKBWriter* writer,
                                                  int byteOrder);

/*
 * Specify whether SRID values should be output. 
 */
extern char __declspec(dllexport) GEOSWKBWriter_getIncludeSRID(const GEOSWKBWriter* writer);
extern void __declspec(dllexport) GEOSWKBWriter_setIncludeSRID(GEOSWKBWriter* writer, const char writeSRID);

extern char __declspec(dllexport) GEOSWKBWriter_getIncludeSRID_r(GEOSContextHandle_t handle,
                                   const GEOSWKBWriter* writer);
extern void __declspec(dllexport) GEOSWKBWriter_setIncludeSRID_r(GEOSContextHandle_t handle,
                                   GEOSWKBWriter* writer, const char writeSRID);


/*
 * Free buffers returned by stuff like GEOSWKBWriter_write(), 
 * GEOSWKBWriter_writeHEX() and GEOSWKTWriter_write(). 
 */
extern void __declspec(dllexport) GEOSFree( void *buffer );
extern void __declspec(dllexport) GEOSFree_r( GEOSContextHandle_t handle, void *buffer );

#ifdef __cplusplus
} // extern "C"
#endif

#endif /* #ifndef GEOS_C_H_INCLUDED */
