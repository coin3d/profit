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

#ifndef PRF_VERTEX_PALETTE_NODE_H
#define PRF_VERTEX_PALETTE_NODE_H

#include <profit/types.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

struct prf_vertex_palette_data {
    int32_t  length;
}; /* struct prf_vertex_palette_data */

void prf_vertex_palette_init( void );

typedef  struct prf_vertex_pool_s  prf_vertex_pool_t;

struct prf_vertex_pool_s {
    uint8_t * data;
    uint32_t  data_size;
    uint32_t  position;
}; /* struct prf_vertex_pool_s */

#ifdef __cplusplus
}; /* extern "C" */
#endif /* __cplusplus */

#endif /* ! PRF_VERTEX_PALETTE_NODE_H */
