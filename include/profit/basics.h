/**************************************************************************\
 * 
 *  Copyright (C) 1998-2001 by Systems in Motion.  All rights reserved.
 *
 *  This file is part of the profit library.
 *
 *  This library (profit) is free software and comes without any warranty.
 *  You can redistribute it and / or modify it under the terms of the
 *  GNU General Public License, version 2, as published by the
 *  Free Software Foundation (see file named COPYING for details).
 *
 *  If you want to use profit in applications not covered by licenses
 *  compatible with the GNU GPL, you can contact SIM to acquire a
 *  Professional Edition license for profit.
 *
 *  Systems in Motion AS, Prof. Brochs gate 6, N-7030 Trondheim, NORWAY
 *  http://www.sim.no/ sales@sim.no Voice: +47 22114160 Fax: +47 67172912
 *
\**************************************************************************/

/* $Id$ */

#ifndef PRF_BASICS_H
#define PRF_BASICS_H

#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#ifdef PROFIT_MAKE_DLL
#define PROFIT_API __declspec(dllexport)
#else /* ! PROFIT_MAKE_DLL */
#ifdef PROFIT_DLL
#define PROFIT_API __declspec(dllimport)
#else /* ! PROFIT_DLL */
#define PROFIT_API
#endif /* ! PROFIT_DLL */
#endif /* ! PPROFIT_MAKE_DLL */

#if defined(__sgi) || defined(__hpux) 
#define HAVE_INTTYPES
#define HAVE_UINTTYPES
#endif

#if defined (__APPLE__)
#define HAVE_INTTYPES
#endif

#ifdef __BIT_TYPES_DEFINED__
#define HAVE_INTTYPES
#endif /* __BIT_TYPES_DEFINES__ */

#ifndef HAVE_INTTYPES
#define HAVE_INTTYPES
typedef  signed char            int8_t;
typedef  signed short           int16_t;
typedef  signed int             int32_t;
#endif /* ! HAVE_INTTYPES */

#ifndef HAVE_UINTTYPES
#define HAVE_UINTTYPES
typedef  unsigned char          uint8_t;
typedef  unsigned short         uint16_t;
typedef  unsigned int           uint32_t;
#endif /* ! HAVE_UINTTYPES */

typedef  float                  float32_t;
typedef  double                 float64_t;

typedef  int                    bool_t;

typedef  struct bfile_s         bfile_t;

typedef  struct prf_model_s     prf_model_t;
typedef  struct prf_state_s     prf_state_t;

typedef  struct prf_node_s      prf_node_t;
typedef  struct prf_nodeinfo_s  prf_nodeinfo_t;

typedef  struct prf_vertex_s    prf_vertex_t;
typedef  struct prf_material_s  prf_material_t;
typedef  struct prf_texture_s   prf_texture_t;

typedef  float32_t              matrix4x4_f32_t[4][4];

#ifndef FALSE
#define FALSE                   (0)
#define TRUE                    (!FALSE)
#endif /* ! FALSE */

#ifndef NULL
#define NULL                    (0)
#endif /* ! NULL */

#define PRF_MAX( arg1, arg2 ) (((arg1) < (arg2)) ? (arg2) : (arg1))
#define PRF_MIN( arg1, arg2 ) (((arg1) < (arg2)) ? (arg1) : (arg2))
#define PRF_ABS( arg ) (((arg) < 0) ? (0-(arg)) : (arg))

/* prf_dblcpy( d, a ) copies a double from a to d.  IRIX has problems with
   unaligned double reads, so this is necessary to avoid dumping under IRIX */

#if defined(__sgi) || defined(__hpux)
#define prf_dblcpy( fD, fA ) \
    do {                          \
        int *ptrD, *ptrA;         \
        ptrD = (int *) &(fD);     \
        ptrA = (int *) &(fA);     \
        *ptrD++ = *ptrA++;        \
        *ptrD = *ptrA;            \
    } while ( FALSE )
#define prf_dblread( fA ) _prf_read_double( &(fA) )
#define prf_dblwrite( fD, fA ) _prf_write_double( &(fD), fA )
PROFIT_API  float64_t _prf_read_double( float64_t * fA );
PROFIT_API  void _prf_write_double( float64_t * fD, float64_t fA );
#else /* !__sgi && !__hpux*/
#define prf_dblcpy( fD, fA ) (fD) = (fA)
#define prf_dblread( fA ) (fA)
#define prf_dblwrite( fD, fA ) (fD) = (fA)
#endif /* ! __sgi */

#ifdef __cplusplus
}; /* extern "C" */
#endif /* __cplusplus */

#endif /* ! PRF_BASICS_H */
