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

#include <profit/basic_funcs.h>
#include <profit/vertex.h>
#include <profit/node.h>
#include <profit/nodes/vertex_with_color.h>
#include <profit/nodes/obsolete_vertex_with_color.h>
#include <profit/nodes/vertex_with_normal.h>
#include <profit/nodes/vertex_with_texture.h>
#include <profit/nodes/obsolete_vertex_with_normal.h>
#include <profit/nodes/vertex_with_normal_and_texture.h>
#include <config.h>

#include <stdio.h>

/**************************************************************************/

void
prf_vertex_clear(
    prf_vertex_t * vertex )
{
    if ( vertex != NULL ) {
        prf_dblwrite( vertex->x, 0.0f );
        prf_dblwrite( vertex->y, 0.0f );
        prf_dblwrite( vertex->z, 0.0f );
        vertex->color_name_index = 0;
        vertex->flags = 0;
        vertex->normal[0] = 0.0f;
        vertex->normal[1] = 0.0f;
        vertex->normal[2] = 0.0f;
        vertex->texture[0] = 0.0f;
        vertex->texture[1] = 0.0f;
        vertex->packed_color = 0;
        vertex->color_index = 0;
        vertex->has_normal = FALSE;
        vertex->has_texture = FALSE;
    }
} /* prf_vertex_clear() */

/**************************************************************************/

bool_t
prf_vertex_lookup(
    prf_node_t * node,
    prf_vertex_t * vertex )
{
    switch ( node->opcode ) {
    case 8:
        prf_obsolete_vertex_with_color_fill_vertex( node, vertex );
        break;
    case 9:
        prf_obsolete_vertex_with_normal_fill_vertex( node, vertex );
        break;
    case 68:
        prf_vertex_with_color_fill_vertex( node, vertex );
        break;
    case 69:
        prf_vertex_with_normal_fill_vertex( node, vertex );
        break;
    case 70:
        prf_vertex_with_normal_and_texture_fill_vertex( node, vertex );
        break;
    case 71:
        prf_vertex_with_texture_fill_vertex( node, vertex );
        break;
    default:
        return FALSE;
    }
    return TRUE;
} /* prf_vertex_lookup() */

/**************************************************************************/
