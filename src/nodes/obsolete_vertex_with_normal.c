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

/* FIXME: 990504 larsa - because of lack of documentation
  Did not find docs for this node type, which means data except the coordinate
  of the vertex is unavailable.
*/

#include <profit/nodes/obsolete_vertex_with_normal.h>

#include <profit/basic_funcs.h>
#include <profit/node.h>
#include <profit/vertex.h>
#include <profit/nodeinfo.h>
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

static const prf_nodeinfo_t prf_obsolete_vertex_with_normal_info;

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

void
prf_obsolete_vertex_with_normal_init(
    void )
{
    prf_nodeinfo_set( &prf_obsolete_vertex_with_normal_info );
} /* prf_obsolete_vertex_with_normal_inin() */

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
            node->data = malloc( node->length - 4 );
        else
            node->data = pool_malloc( state->model->mempool_id,
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

static const prf_nodeinfo_t prf_obsolete_vertex_with_normal_info = {
    9, PRF_VERTEX | PRF_OBSOLETE,
    "Vertex with Color and Normal (obsolete)",
    prf_obsolete_vertex_with_normal_load_f,
    prf_obsolete_vertex_with_normal_save_f,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL
}; /* struct prf_obsolete_vertex_with_normal_info */

/**************************************************************************/

/* $Id$ */

