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

#include <profit/nodes/vertex_with_normal.h>
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

static prf_nodeinfo_t prf_vertex_with_normal_info = {
    69, PRF_VERTEX,
    "Vertex with Color and Normal",
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL
}; /* struct prf_vertex_with_normal_info */

/**************************************************************************/


typedef  struct prf_vertex_with_normal_data  node_data;
#define  NODE_DATA_SIZE                      48
#define  NODE_DATA_PAD                       0

/**************************************************************************/

void
prf_vertex_with_normal_fill_vertex(
    prf_node_t * node,
    prf_vertex_t * vertex )
{
    int pos;
    bool_t complete;

    assert( node != NULL && vertex != NULL );

    complete = FALSE;
    if ( node->opcode != prf_vertex_with_normal_info.opcode ) {
        prf_error( 9, "vertex with normal: fill from illegal node type (%d).",
            node->opcode );
        return;
    }

    pos = 4;
    do {
        node_data * data;
        data = (node_data *) node->data;
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
        vertex->normal[0] = data->normal[0]; pos += 4;
        if ( node->length < (pos + 4) ) break;
        vertex->normal[1] = data->normal[1]; pos += 4;
        if ( node->length < (pos + 4) ) break;
        vertex->normal[2] = data->normal[2]; pos += 4;
        vertex->has_normal = TRUE;
        if ( node->length < (pos + 4) ) break;
        vertex->packed_color = data->packed_color; pos += 4;
        complete = TRUE;
        if ( node->length < (pos + 4) ) break;
        vertex->color_index = data->color_index; pos += 4;
        complete = TRUE;
    } while ( FALSE );
    if ( ! complete )
        prf_warn( 1, "stores only %d bytes in vertex with normal", pos );
    /* padding can't be dealt with, but should be notified, though  */
    if ( pos < node->length )
        prf_warn( 1, "vertex with normal is larger than %d bytes (%d)",
            pos, node->length );
} /* prf_vertex_with_normal_fill_vertex() */

/**************************************************************************/

static
bool_t
prf_vertex_with_normal_load_f(
    prf_node_t * node,
    prf_state_t * state,
    bfile_t * bfile )
{
    int pos;

    assert( node != NULL && state != NULL && bfile != NULL );

    node->opcode = bf_get_uint16_be( bfile );
    if ( node->opcode != prf_vertex_with_normal_info.opcode ) {
        prf_error( 9,
            "tried vertex with normal load method for node of type %d",
            node->opcode );
        bf_rewind( bfile, 2 );
        return FALSE;
    }

    node->length = bf_get_uint16_be( bfile );
    if ( node->length < 32 ) {
        prf_error( 6,
            "vertex with normal node to short (%d bytes, not supported).",
            node->length );
        bf_rewind( bfile, 4 );
    }

    if ( node->length > 4 && node->data == NULL ) { /* not preallocated */
        assert( state->model != NULL );
        if ( state->model->mempool_id == 0 )
            node->data = (uint8_t *)malloc( node->length - 4 + NODE_DATA_PAD );
        else
            node->data = (uint8_t *)pool_malloc( state->model->mempool_id,
                node->length - 4 + NODE_DATA_PAD );
        if ( node->data == NULL ) {
            prf_error( 9, "memory allocation problem (returned NULL)" );
            bf_rewind( bfile, 8 );
            return FALSE;
        }
    }

    pos = 4;
    do {
        node_data * data;
        data = (node_data *) node->data;
        assert( data != NULL );
        data->color_name_index = bf_get_uint16_be( bfile ); pos += 2;
        data->flags = bf_get_uint16_be( bfile ); pos += 2;
        prf_dblwrite( data->x, bf_get_float64_be( bfile ) ); pos += 8;
        prf_dblwrite( data->y, bf_get_float64_be( bfile ) ); pos += 8;
        prf_dblwrite( data->z, bf_get_float64_be( bfile ) ); pos += 8;
        if ( node->length < (pos + 4) ) break;
        data->normal[0] = bf_get_float32_be( bfile ); pos += 4;
        if ( node->length < (pos + 4) ) break;
        data->normal[1] = bf_get_float32_be( bfile ); pos += 4;
        if ( node->length < (pos + 4) ) break;
        data->normal[2] = bf_get_float32_be( bfile ); pos += 4;
        if ( node->length < (pos + 4) ) break;
        data->packed_color = bf_get_uint32_be( bfile ); pos += 4;

	/*
	fprintf(stderr,"vertex: %x %x %x\n", 
		data->flags,
		data->color_name_index,
		data->packed_color);
	*/

        if ( node->length < (pos + 4) ) break;
        data->color_index = bf_get_uint32_be( bfile ); pos += 4;

    } while ( FALSE );

    if ( pos < node->length )
        pos += bf_read( bfile, node->data + pos - 4 + NODE_DATA_PAD,
            node->length - pos );

    return TRUE;
} /* prf_vertex_with_normal_load_f() */

/**************************************************************************/

static
bool_t
prf_vertex_with_normal_save_f(
    prf_node_t * node,
    prf_state_t * state,
    bfile_t * bfile )
{
    int pos;

    assert( node != NULL && state != NULL && bfile != NULL );

    if ( node->opcode != prf_vertex_with_normal_info.opcode ) {
        prf_error( 9, "tried vertex with normal save method on node of type %d",
            node->opcode );
        return FALSE;
    }

    bf_put_uint16_be( bfile, node->opcode );
    bf_put_uint16_be( bfile, node->length );

    pos = 4;
    do {
        node_data * data;
        data = (node_data *) node->data;
        assert( data != NULL );
        bf_put_uint16_be( bfile, data->color_name_index ); pos += 2;
        bf_put_uint16_be( bfile, data->flags ); pos += 2;
        bf_put_float64_be( bfile, prf_dblread( data->x ) ); pos += 8;
        bf_put_float64_be( bfile, prf_dblread( data->y ) ); pos += 8;
        bf_put_float64_be( bfile, prf_dblread( data->z ) ); pos += 8;
        if ( node->length < (pos + 4) ) break;
        bf_put_float32_be( bfile, data->normal[0] ); pos += 4;
        if ( node->length < (pos + 4) ) break;
        bf_put_float32_be( bfile, data->normal[1] ); pos += 4;
        if ( node->length < (pos + 4) ) break;
        bf_put_float32_be( bfile, data->normal[2] ); pos += 4;
        if ( node->length < (pos + 4) ) break;
        bf_put_uint32_be( bfile, data->packed_color ); pos += 4;
        if ( node->length < (pos + 4) ) break;
        bf_put_uint32_be( bfile, data->color_index ); pos += 4;
    } while ( FALSE );

    if ( pos < node->length )
        pos += bf_write( bfile, node->data + pos - 4 + NODE_DATA_PAD,
            node->length - pos );

    return TRUE;
} /* prf_vertex_with_normal_save_f() */

/**************************************************************************/

void
prf_vertex_with_normal_init(
    void )
{
    prf_vertex_with_normal_info.load_f = prf_vertex_with_normal_load_f;
    prf_vertex_with_normal_info.save_f = prf_vertex_with_normal_save_f;
    prf_nodeinfo_set( &prf_vertex_with_normal_info );
} /* prf_vertex_with_normal_init() */

/**************************************************************************/
