/**************************************************************************\
 * 
 *  Copyright (C) 1998-1999 by Systems in Motion.  All rights reserved.
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

#ifndef PROFIT_BASIC_FUNCS_H
#define PROFIT_BASIC_FUNCS_H

#include <profit/basic_types.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

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
float64_t _prf_read_double( float64_t * fA );
void _prf_write_double( float64_t * fD, float64_t fA );
#else /* !__sgi && !__hpux*/
#define prf_dblcpy( fD, fA ) (fD) = (fA)
#define prf_dblread( fA ) (fA)
#define prf_dblwrite( fD, fA ) (fD) = (fA)
#endif /* ! __sgi */

#ifdef __cplusplus
}; /* extern "C" */
#endif /* __cplusplus */

#endif /* ! PROFIT_BASIC_FUNCS_H */

/* $Id$ */

