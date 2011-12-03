/**************************************************************************\
 * Copyright (c) Kongsberg Oil & Gas Technologies AS
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 * 
 * Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 * 
 * Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 * 
 * Neither the name of the copyright holder nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
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

#if defined(__sgi) || defined(__hpux)  || defined(__sun)
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
