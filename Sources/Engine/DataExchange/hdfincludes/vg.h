/****************************************************************************
 * NCSA HDF                                                                 *
 * Software Development Group                                               *
 * National Center for Supercomputing Applications                          *
 * University of Illinois at Urbana-Champaign                               *
 * 605 E. Springfield, Champaign IL 61820                                   *
 *                                                                          *
 * For conditions of distribution and use, see the accompanying             *
 * hdf/COPYING file.                                                        *
 *                                                                          *
 ****************************************************************************/

/* $Id: vg.h,v 1.53 1998/03/30 18:29:40 smitchel Exp $ */

/*****************************************************************************
*
* vg.h
*
* Part of HDF VSet interface
*
* defines symbols and structures used in all v*.c files
*
* NOTES:
* This include file depends on the basic HDF *.h files hdfi.h and hdf.h.
* An 'S' in the comment means that that data field is saved in the HDF file.
*
******************************************************************************/

#ifndef _VG_H
#define _VG_H

/* --------------  H D F    V S E T   tags  ---------------------------- */

/* In what versions were these valid? */
#define OLD_VGDESCTAG   61820   /* tag for a vgroup d */
#define OLD_VSDESCTAG   61821   /* tag for a vdata descriptor */
#define OLD_VSDATATAG   61822   /* tag for actual raw data of a vdata */

/* re-define what new ones are. Don't know why there are two sets but
   made them conform to what was in 'htags.h' */
#define NEW_VGDESCTAG    DFTAG_VG
#define NEW_VSDESCTAG    DFTAG_VH
#define NEW_VSDATATAG    DFTAG_VS

#define VGDESCTAG       NEW_VGDESCTAG
#define VSDESCTAG       NEW_VSDESCTAG
#define VSDATATAG       NEW_VSDATATAG

#define _HDF_VSPACK 0
#define _HDF_VSUNPACK 1
#define VG_ATTR_SET 0x00000001  /* this vgroup has attribute(s) */
#define VS_ATTR_SET 0x00000001  /* this vdata has attribute(s) */
#define _HDF_VDATA -1		/* used in vdata attr interface
                           findex to represent the entire vdata  */
#define _HDF_ENTIRE_VDATA _HDF_VDATA /* if users have already  used
                            _HDF_ENTIRE_VDATA since 4.1b1 was out */

/* Size of the atom hash table */
#define VATOM_HASH_SIZE 256

/* .................................................................. */
#define VSET_NEW_VERSION	4  /* if attr or other new features are set */
#define VSET_VERSION		3  /* DO NOT CHANGE!! */
#define VSET_OLD_VERSION	2  /* All version <= 2 use old type mappings */
#define VSET_OLD_TYPES		VSET_OLD_VERSION  /* For backward compatibility */

/* Only include the library header if the VSET_INTERFACE macro is defined */
#ifdef VSET_INTERFACE
#include "Engine\DataExchange\hdfincludes\vgint.h"          /* Library VSet information header */
#endif /* VSET_INTERFACE */

#endif                          /* _VG_H */
