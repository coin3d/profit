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

/* FIXME: 990504 larsa - because of lack of documentation
  Did not find docs for this node type, which means data except the coordinate
  of the vertex is unavailable.
*/

#include <profit/nodes/obsolete_vertex_with_color.h>

#include <profit/node.h>
#include <profit/nodeinfo.h>
#include <profit/vertex.h>
#include <profit/state.h>
#include <profit/model.h>
#include <profit/debug.h>
#include <profit/util/mempool.h>
#include <profit/util/bfile.h>
#include <config.h>

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

/**************************************************************************/

static prf_nodeinfo_t prf_obsolete_vertex_with_color_info = {
    8, PRF_VERTEX | PRF_OBSOLETE,
    "Vertex with Color (obsolete)",
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL
}; /* struct prf_obsolete_vertex_with_color_info */

/**************************************************************************/


typedef  struct prf_obsolete_vertex_with_color_data  node_data;

#define  NODE_DATA_PAD  0

/**************************************************************************/

void
prf_obsolete_vertex_with_color_fill_vertex(
    prf_node_t * node,
    prf_vertex_t * vertex )
{
    bool_t complete = FALSE;
    int pos = 4;

    assert( node != NULL && vertex != NULL );
    do {
        node_data * data = (node_data *) node->data;
        vertex->x = (float64_t) data->x; pos += 4;
        vertex->y = (float64_t) data->y; pos += 4;
        vertex->z = (float64_t) data->z; pos += 4;
        complete = TRUE;
    } while ( FALSE );
#if 0 /* FIXME: 990504 larsa (need docs) */
    if ( ! complete )
        prf_warn( 1, "stores only %d bytes in vertex with color", pos );
    /* padding can't be dealt with, but should be notified, though  */
    if ( pos < node->length )
        prf_warn( 2, "vertex with color is larger than %d bytes (%d)",
            pos, node->length );
#endif /* 0 - need doc */
} /* prf_obsolete_vertex_with_color_fill_vertex() */

/**************************************************************************/

static
bool_t
prf_obsolete_vertex_with_color_load_f(
    prf_node_t * node,
    prf_state_t * state,
    bfile_t * bfile )
{
    int pos = 4;

    assert( node != NULL && state != NULL && bfile != NULL );

    node->opcode = bf_get_uint16_be( bfile );
    if ( node->opcode != prf_obsolete_vertex_with_color_info.opcode ) {
        prf_error( 9,
            "tried vertex with color (obs) load method for node of type %d.",
            node->opcode );
        bf_rewind( bfile, 2 );
        return FALSE;
    }

    node->length = bf_get_uint16_be( bfile );
    if ( node->length > 4 && node->data == NULL ) { /* not preallocated */
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

    if ( pos < node->length )
        pos += bf_read( bfile, node->data + pos - 4 + NODE_DATA_PAD,
            node->length - pos );

    return TRUE;
} /* prf_obsolete_vertex_with_color_load_f() */

/**************************************************************************/

static
bool_t
prf_obsolete_vertex_with_color_save_f(
    prf_node_t * node,
    prf_state_t * state,
    bfile_t * bfile )
{
    int pos = 4;
    assert( node != NULL && state != NULL && bfile != NULL );

    if ( node->opcode != prf_obsolete_vertex_with_color_info.opcode ) {
        prf_error( 9,
            "tried vertex with color (obs) save method for node of type %d.",
            node->opcode );
        return FALSE;
    }

    bf_put_uint16_be( bfile, node->opcode );
    bf_put_uint16_be( bfile, node->length );

    do {
        node_data * data;
        data = (node_data *) node->data;
        assert( data != NULL );
        bf_put_uint32_be( bfile, data->x ); pos += 4;
        bf_put_uint32_be( bfile, data->y ); pos += 4;
        bf_put_uint32_be( bfile, data->z ); pos += 4;
    } while ( FALSE );

    if ( node->length > pos )
        pos += bf_write( bfile, node->data + pos - 4 + NODE_DATA_PAD,
            node->length - pos );

    return TRUE;
} /* prf_obsolete_vertex_with_color_save_f() */

/**************************************************************************/

void
prf_obsolete_vertex_with_color_init(
    void )
{
    prf_obsolete_vertex_with_color_info.load_f =
        prf_obsolete_vertex_with_color_load_f;
    prf_obsolete_vertex_with_color_info.save_f =
        prf_obsolete_vertex_with_color_save_f;
    prf_nodeinfo_set( &prf_obsolete_vertex_with_color_info );
} /* prf_obsolete_vertex_with_color_inin() */

/**************************************************************************/
