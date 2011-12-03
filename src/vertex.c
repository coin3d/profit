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

#include <profit/basics.h>
#include <profit/vertex.h>
#include <profit/node.h>
#include <profit/nodes/vertex_with_color.h>
#include <profit/nodes/obsolete_vertex_with_color.h>
#include <profit/nodes/vertex_with_normal.h>
#include <profit/nodes/vertex_with_texture.h>
#include <profit/nodes/obsolete_vertex_with_normal.h>
#include <profit/nodes/vertex_with_normal_and_texture.h>
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

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
