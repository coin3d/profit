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

#ifndef PRF_VERTEX_H
#define PRF_VERTEX_H

#include <profit/types.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

struct prf_vertex_s {
    float64_t  x;
    float64_t  y;
    float64_t  z;
    uint16_t   color_name_index;
    uint16_t   flags;
    float32_t  normal[ 3 ];
    float32_t  texture[ 2 ];
    uint32_t   packed_color;
    uint32_t   color_index;
    bool_t     has_normal;
    bool_t     has_texture;
}; /* struct prf_vertex_s */

void     prf_vertex_clear( prf_vertex_t * vertex );
bool_t   prf_vertex_lookup( prf_node_t * node, prf_vertex_t * vertex );

#define  PRF_VERTEX_FLAGS_START_HARD_EDGE    0x8000
#define  PRF_VERTEX_FLAGS_NORMAL_FROZEN      0x4000
#define  PRF_VERTEX_FLAGS_NO_COLOR           0x2000
#define  PRF_VERTEX_FLAGS_PACKED_COLOR       0x1000

#ifdef __cplusplus
}; /* extern "C" */
#endif /* __cplusplus */

#endif /* ! PRF_VERTEX_H */
