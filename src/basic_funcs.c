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

#include <profit/basic_funcs.h>
#include <config.h>

/**************************************************************************/

#if defined(__sgi) || defined(__hpux)

float64_t
_prf_read_double(
    float64_t * ptr )
{
    union {
        uint32_t data[2];
        float64_t real;
    } u;
    uint32_t * dptr;
    dptr = (uint32_t *) ptr;
    u.data[0] = dptr[0];
    u.data[1] = dptr[1];
    return u.real;
} /* _read_double() */

void
_prf_write_double(
    float64_t * ptr,
    float64_t fA )
{
    union {
        uint32_t data[2];
        float64_t real;
    } u;
    uint32_t * dptr;
    dptr = (uint32_t *) ptr;
    u.real = fA;
    dptr[0] = u.data[0];
    dptr[1] = u.data[1];
} /* _write_double() */

#endif /* __sgi || __hpux*/

/**************************************************************************/

/* $Id$ */

