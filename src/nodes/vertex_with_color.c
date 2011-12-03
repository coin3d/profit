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

#include <profit/nodes/vertex_with_color.h>
#include <profit/basics.h>
#include <profit/node.h>
#include <profit/nodeinfo.h>
#include <profit/vertex.h>
#include <profit/model.h>
#include <profit/state.h>
#include <profit/debug.h>
#include <profit/util/bfile.h>
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

/**************************************************************************/

static prf_nodeinfo_t prf_vertex_with_color_info = {
    68, PRF_VERTEX,
    "Vertex with Color",
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL
}; /* struct prf_vertex_with_color_info */

/**************************************************************************/


typedef  struct prf_vertex_with_color_data  node_data;
#define  NODE_DATA_SIZE 36
#define  NODE_DATA_PAD 0

/**************************************************************************/

void
prf_vertex_with_color_fill_vertex(
    prf_node_t * node,
    prf_vertex_t * vertex )
{
    int pos = 4;
    bool_t complete = FALSE;

    assert( node != NULL && vertex != NULL );

    if ( node->opcode != prf_vertex_with_color_info.opcode ) {
        prf_error( 9, "fill vertex with color from node of type %d",
            node->opcode );
        return;
    }

    do {
        node_data * data = (node_data *) node->data;
        if ( node->length < (pos + 2) ) break;
        vertex->color_name_index = data->color_name_index; pos += 2;
        if ( node->length < (pos + 2) ) break;
        vertex->flags = data->flags; pos += 2;
        if ( node->length < (pos + 8) ) break;
        prf_dblcpy( vertex->x, data->x ); pos += 8;
        if ( node->length < (pos + 8) ) break;
        prf_dblcpy( vertex->y, data->y ); pos += 8;
        if ( node->length < (pos + 8) ) break;
        prf_dblcpy( vertex->z, data->z ); pos += 8;
        if ( node->length < (pos + 4) ) break;
        vertex->packed_color = data->packed_color; pos += 4;
        complete = TRUE;
        if ( node->length < (pos + 4) ) break;
        vertex->color_index = data->color_index;  pos += 4;
        complete = TRUE;
    } while ( FALSE );

    if ( ! complete )
        prf_warn( 1, "stores only %d bytes in vertex with color", pos );
    /* padding can't be dealt with, but should be notified, though  */
    if ( pos < node->length )
        prf_warn( 1, "vertex with color is larger than %d bytes (%d)",
            pos, node->length );

} /* prf_vertex_with_color_fill_vertex() */

/**************************************************************************/

static
bool_t
prf_vertex_with_color_load_f(
    prf_node_t * node,
    prf_state_t * state,
    bfile_t * bfile )
{
    uint32_t pos = 4;

    assert( node != NULL && state != NULL && bfile != NULL );

    node->opcode = bf_get_uint16_be( bfile );
    if ( node->opcode != prf_vertex_with_color_info.opcode ) {
        prf_error( 9, "tried vertex with color load method for node of type %d.",
            node->opcode );
        bf_rewind( bfile, 2 );
        return FALSE;
    }

    node->length = bf_get_uint16_be( bfile );
    if ( node->length < 32 ) {
        prf_error( 6, "vertex (%d) so short (%d bytes) it is not supported.",
            node->opcode, node->length );
        bf_rewind( bfile, 4 );
        return FALSE;
    }

    if ( node->length > 4 && node->data == NULL ) { /* not preallocated */
        assert( state->model != NULL );
        if ( state->model->mempool_id == 0 )
            node->data = (uint8_t *)malloc( node->length + NODE_DATA_PAD );
        else
            node->data = (uint8_t *)pool_malloc( state->model->mempool_id,
                node->length + NODE_DATA_PAD );
        if ( node->data == NULL ) {
            prf_error( 9, "memory allocation problem (returned NULL)" );
            bf_rewind( bfile, 8 );
            return FALSE;
        }
    }

    do {
        node_data * data = (node_data *) node->data;
        data->color_name_index = bf_get_uint16_be( bfile ); pos += 2;
        data->flags = bf_get_uint16_be( bfile ); pos += 2;
        prf_dblwrite( data->x, bf_get_float64_be( bfile )); pos += 8;
        prf_dblwrite( data->y, bf_get_float64_be( bfile )); pos += 8;
        prf_dblwrite( data->z, bf_get_float64_be( bfile )); pos += 8;
        if ( node->length < (pos + 4) ) break;
        data->packed_color = bf_get_uint32_be( bfile ); pos += 4;
        if ( node->length < (pos + 4) ) break;
        data->color_index = bf_get_uint32_be( bfile ); pos += 4;
    } while ( FALSE );

    if ( pos < node->length ) /* padding */
        pos += bf_read( bfile, node->data + pos - 4 + NODE_DATA_PAD,
            node->length - pos );

    return TRUE;
} /* prf_vertex_with_color_load_f() */

/**************************************************************************/

static
bool_t
prf_vertex_with_color_save_f(
    prf_node_t * node,
    prf_state_t * state,
    bfile_t * bfile )
{
    int pos = 4;

    assert( node != NULL && state != NULL && bfile != NULL );

    if ( node->opcode != prf_vertex_with_color_info.opcode ) {
        prf_error( 9, "tried vertex with color save method on node of type %d.",
            node->opcode );
        return FALSE;
    }

    bf_put_uint16_be( bfile, node->opcode );
    bf_put_uint16_be( bfile, node->length );

    do {
        node_data * data = (node_data *) node->data;
        assert( data != NULL );
        bf_put_uint16_be( bfile, data->color_name_index ); pos += 2;
        bf_put_uint16_be( bfile, data->flags ); pos += 2;
        bf_put_float64_be( bfile, prf_dblread( data->x ) ); pos += 8;
        bf_put_float64_be( bfile, prf_dblread( data->y ) ); pos += 8;
        bf_put_float64_be( bfile, prf_dblread( data->z ) ); pos += 8;
        if ( node->length < (pos + 4) ) break;
        bf_put_uint32_be( bfile, data->packed_color ); pos += 4;
        if ( node->length < (pos + 4) ) break;
        bf_put_uint32_be( bfile, data->color_index ); pos += 4;
    } while ( FALSE );

    if ( pos < node->length ) /* padding */
        pos += bf_write( bfile, node->data + pos - 4, node->length - pos );

    return TRUE;
} /* prf_vertex_with_color_save_f() */

/**************************************************************************/

void
prf_vertex_with_color_init(
    void )
{
    prf_vertex_with_color_info.load_f = prf_vertex_with_color_load_f;
    prf_vertex_with_color_info.save_f = prf_vertex_with_color_save_f;
    prf_nodeinfo_set( &prf_vertex_with_color_info );
} /* prf_vertex_with_color_init() */

/**************************************************************************/
