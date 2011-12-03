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

/* FIXME: 990504 larsa - because of lack of documentation
  Did not find docs for this node type, which means data except the coordinate
  of the vertex is unavailable.
*/

#include <profit/nodes/obsolete_vertex_with_normal.h>
#include <profit/basics.h>
#include <profit/node.h>
#include <profit/vertex.h>
#include <profit/nodeinfo.h>
#include <profit/state.h>
#include <profit/model.h>
#include <profit/debug.h>
#include <profit/util/mempool.h>
#include <profit/util/bfile.h>
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

/**************************************************************************/

static prf_nodeinfo_t prf_obsolete_vertex_with_normal_info = {
    9, PRF_VERTEX | PRF_OBSOLETE,
    "Vertex with Color and Normal (obsolete)",
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL
}; /* struct prf_obsolete_vertex_with_normal_info */

/**************************************************************************/


typedef  struct prf_obsolete_vertex_with_normal_data  node_data;

#define  NODE_DATA_PAD  0

/**************************************************************************/

void
prf_obsolete_vertex_with_normal_fill_vertex(
    prf_node_t * node,
    prf_vertex_t * vertex )
{
    int pos = 4;
    bool_t complete = FALSE;

    assert( node != NULL && vertex != NULL );

    do {
        node_data * data = (node_data *) node->data;
        prf_dblwrite( vertex->x, (float64_t) data->x ); pos += 8;
        prf_dblwrite( vertex->y, (float64_t) data->y ); pos += 8;
        prf_dblwrite( vertex->z, (float64_t) data->z ); pos += 8;
        complete = TRUE;
    } while ( FALSE );

} /* prf_obsolete_vertex_with_normal_fill_vertex() */

/**************************************************************************/

static
bool_t
prf_obsolete_vertex_with_normal_load_f(
    prf_node_t * node,
    prf_state_t * state,
    bfile_t * bfile )
{
    int pos = 4;

    assert( node != NULL && state != NULL && bfile != NULL );

    node->opcode = bf_get_uint16_be( bfile );
    if ( node->opcode != prf_obsolete_vertex_with_normal_info.opcode ) {
        prf_error( 9,
            "tried vertex with normal (obs) load method for node of type %d.",
            node->opcode );
        bf_rewind( bfile, 2 );
        return FALSE;
    }
    node->length = bf_get_uint16_be( bfile );

    if ( node->length > 4 && node->data == NULL ) {
        assert( state->model != NULL );
        if ( state->model->mempool_id == 0 )
            node->data = (uint8_t *)malloc( node->length - 4 );
        else
            node->data = (uint8_t *)pool_malloc( state->model->mempool_id,
                node->length - 4 );
        if ( node->data == NULL ) {
            prf_error( 9, "memory allocation problem (returned NULL)" );
            bf_rewind( bfile, 4 );
            return FALSE;
        }
    }

    do {
        node_data * data;
        data = (node_data *) node->data;

        if ( node->length < (pos + 4) ) break;
        data->x = bf_get_int32_be( bfile ); pos += 4;
        if ( node->length < (pos + 4) ) break;
        data->y = bf_get_int32_be( bfile ); pos += 4;
        if ( node->length < (pos + 4) ) break;
        data->z = bf_get_int32_be( bfile ); pos += 4;
    } while ( FALSE );

    if ( node->length > pos )
        pos += bf_read( bfile, node->data + pos - 4 + NODE_DATA_PAD,
            node->length - pos );

    return TRUE;
} /* prf_obsolete_vertex_with_normal_load_f() */

/**************************************************************************/

static
bool_t
prf_obsolete_vertex_with_normal_save_f(
    prf_node_t * node,
    prf_state_t * state,
    bfile_t * bfile )
{
    int pos;

    assert( node != NULL && state != NULL && bfile != NULL );

    if ( node->opcode != prf_obsolete_vertex_with_normal_info.opcode ) {
        prf_error( 9,
            "tried vertex with normal (obs) save method for node of type %d.",
            node->opcode );
        return FALSE;
    }

    bf_put_uint16_be( bfile, node->opcode );
    bf_put_uint16_be( bfile, node->length );

    pos = 4;
    do {
        node_data * data;
        assert( node->data != NULL );
        data = (node_data *) node->data;
        bf_put_uint32_be( bfile, data->x ); pos += 4;
        bf_put_uint32_be( bfile, data->y ); pos += 4;
        bf_put_uint32_be( bfile, data->z ); pos += 4;
    } while ( FALSE );

    if ( node->length > pos )
        pos += bf_write( bfile, node->data + pos - 4 + NODE_DATA_PAD,
            node->length - pos );

    return TRUE;
} /* prf_obsolete_vertex_with_normal_save_f() */

/**************************************************************************/

void
prf_obsolete_vertex_with_normal_init(
    void )
{
    prf_obsolete_vertex_with_normal_info.load_f =
        prf_obsolete_vertex_with_normal_load_f;
    prf_obsolete_vertex_with_normal_info.save_f =
        prf_obsolete_vertex_with_normal_save_f;
    prf_nodeinfo_set( &prf_obsolete_vertex_with_normal_info );
} /* prf_obsolete_vertex_with_normal_inin() */

/**************************************************************************/
