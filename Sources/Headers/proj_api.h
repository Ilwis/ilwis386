/******************************************************************************
 * $Id: proj_api.h 1631 2009-09-24 02:26:05Z warmerdam $
 *
 * Project:  PROJ.4
 * Purpose:  Public (application) include file for PROJ.4 API, and constants.
 * Author:   Frank Warmerdam, <warmerdam@pobox.com>
 *
 ******************************************************************************
 * Copyright (c) 2001, Frank Warmerdam <warmerdam@pobox.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 *****************************************************************************/

/* General projections header file */
#ifndef PROJ_API_H
#define PROJ_API_H

/* standard inclusions */
#include <math.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Try to update this every version! */
#define PJ_VERSION 470

extern char const pj_release[]; /* global release id string */

#define RAD_TO_DEG	57.29577951308232
#define DEG_TO_RAD	.0174532925199432958


extern int pj_errno;	/* global error return code */

#if !defined(PROJECTS_H)
    typedef struct { double u, v; } projUV;
    typedef void *projPJ;
    #define projXY projUV
    #define projLP projUV
#else
    typedef PJ *projPJ;
#   define projXY	XY
#   define projLP       LP
#endif

/* procedure prototypes */

projXY pj_fwd(projLP, projPJ);
projLP pj_inv(projXY, projPJ);

int __declspec(dllexport) pj_transform( projPJ src, projPJ dst, long point_count, int point_offset,
                  double *x, double *y, double *z );
int __declspec(dllexport) pj_datum_transform( projPJ src, projPJ dst, long point_count, int point_offset,
                        double *x, double *y, double *z );
int __declspec(dllexport) pj_geocentric_to_geodetic( double a, double es,
                               long point_count, int point_offset,
                               double *x, double *y, double *z );
int __declspec(dllexport) pj_geodetic_to_geocentric( double a, double es,
                               long point_count, int point_offset,
                               double *x, double *y, double *z );
int __declspec(dllexport) pj_compare_datums( projPJ srcdefn, projPJ dstdefn );
int __declspec(dllexport) pj_apply_gridshift( const char *, int, 
                        long point_count, int point_offset,
                        double *x, double *y, double *z );
void __declspec(dllexport) pj_deallocate_grids(void);
int __declspec(dllexport) pj_is_latlong(projPJ);
int __declspec(dllexport) pj_is_geocent(projPJ);
void __declspec(dllexport) pj_pr_list(projPJ);
void __declspec(dllexport) pj_free(projPJ);
void __declspec(dllexport) pj_set_finder( const char *(*)(const char *) );
void __declspec(dllexport) pj_set_searchpath ( int count, const char **path );
projPJ __declspec(dllexport) pj_init(int, char **);
projPJ __declspec(dllexport) pj_init_plus(const char *);
__declspec(dllexport) char *pj_get_def(projPJ, int);
projPJ __declspec(dllexport) pj_latlong_from_proj( projPJ );
__declspec(dllexport) void *pj_malloc(size_t);
void __declspec(dllexport) pj_dalloc(void *);
__declspec(dllexport) char * pj_strerrno(int);
__declspec(dllexport) int * pj_get_errno_ref(void);
__declspec(dllexport) const char *pj_get_release(void);
void __declspec(dllexport) pj_acquire_lock(void);
void __declspec(dllexport) pj_release_lock(void);
void __declspec(dllexport) pj_cleanup_lock(void);

#ifdef __cplusplus
}
#endif

#endif /* ndef PROJ_API_H */

