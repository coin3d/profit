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

#ifndef PRF_VERTEX_WITH_NORMAL_AND_TEXTURE_NODE_H
#define PRF_VERTEX_WITH_NORMAL_AND_TEXTURE_NODE_H

#include <profit/types.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

struct prf_vertex_with_normal_and_texture_data {
    float64_t  x;
    float64_t  y;
    float64_t  z;
    uint16_t  color_name_index;
    uint16_t  flags;
    float32_t  normal[ 3 ];
    float32_t  texture[ 2 ];
    uint32_t   packed_color;
    uint32_t   color_index;
}; /* struct prf_vertex_with_normal_and_texture_data */

void prf_vertex_with_normal_and_texture_init( void );
void prf_vertex_with_normal_and_texture_fill_vertex( prf_node_t * node,
         prf_vertex_t * vertex );

#ifdef __cplusplus
}; /* extern "C" */
#endif /* __cplusplus */

#endif /* ! PRF_VERTEX_WITH_NORMAL_AND_TEXTURE_NODE_H */
