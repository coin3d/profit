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

#ifndef PROFIT_COLOR_PALETTE_H
#define PROFIT_COLOR_PALETTE_H

#include <profit/types.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

struct prf_color_palette_data {
    char       reserved1[ 128 ];
    uint32_t   brightest_rgb[ 1024 ]; /* intensity 127 */
    uint32_t   num_color_names; /* if we are lucky... */
}; /* struct prf_color_palette_data */

struct prf_color_name_data {
    uint16_t   length;
    uint16_t   reserved1;
    uint16_t   index;
    uint16_t   reserved2;
    char       name[ 1 ];
}; /* struct prf_color_name_data */

void prf_color_palette_init( void );

extern uint32_t prf_color_palette_default_values[];

#ifdef __cplusplus
}; /* extern "C" */
#endif /* __cplusplus */

#endif /* ! PROFIT_COLOR_PALETTE_H */

/* $Id$ */

