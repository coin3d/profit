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

/* $Id$ */

#include <profit/types.h>
#include <profit/model.h>
#include <profit/nodeinfo.h>
#include <profit/node.h>
#include <profit/state.h>
#include <profit/debug.h>
#include <profit/nodes/object.h>
#include <profit/util/mempool.h>
#include <profit/util/bfile.h>
#include <config.h>

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

/**************************************************************************/

static const prf_nodeinfo_t prf_object_info;

typedef  struct prf_object_data  node_data;
#define  NODE_DATA_SIZE          24
#define  NODE_DATA_PAD           (sizeof(node_data)-NODE_DATA_SIZE)

/**************************************************************************/

void
prf_object_init(
    void )
{
    prf_nodeinfo_set( &prf_object_info );
} /* prf_object_init() */

/**************************************************************************/

static
bool_t
prf_object_load_f(
    prf_node_t * node,
    prf_state_t * state,
    bfile_t * bfile )
{
    int pos;

    assert( node != NULL && state != NULL && bfile != NULL );

    node->opcode = bf_get_uint16_be( bfile );
    if ( node->opcode != prf_object_info.opcode ) {
        prf_error( 9, "tried object load method on node of type %d.",
            node->opcode );
        bf_rewind( bfile, 2 );
        return FALSE;
    }

    node->length = bf_get_uint16_be( bfile );
    if ( node->length < NODE_DATA_SIZE + 4 ) {
        prf_error( 6, "object node of unsupported length (%d bytes short).",
            node->length );
        bf_rewind( bfile, 4 );
        return FALSE;
    }

    if ( node->length > 4 && node->data == NULL ) { /* not preallocated */
        assert( state->model != NULL );
        if ( state->model->mempool_id == 0 )
            node->data = malloc( node->length - 4 + NODE_DATA_PAD );
        else
            node->data = pool_malloc( state->model->mempool_id,
                node->length - 4 + NODE_DATA_PAD );
        if ( node->data == NULL ) {
            prf_error( 9, "memory allocation problem (returned NULL)" );
            bf_rewind( bfile, 4 );
            return FALSE;
        }
    }

    pos = 4;
    do {
        node_data * data;
        data = (node_data *) node->data;
        bf_read( bfile, (uint8_t *) data->ascii_id, 8 ); pos += 8;
        data->flags = bf_get_uint32_be( bfile ); pos += 4;
        data->relative_priority = bf_get_int16_be( bfile ); pos += 2;
        data->transparency = bf_get_uint16_be( bfile ); pos += 2;
        data->special_effect_id1 = bf_get_int16_be( bfile ); pos += 2;
        data->special_effect_id2 = bf_get_int16_be( bfile ); pos += 2;
        data->significance = bf_get_int16_be( bfile ); pos += 2;
        if ( node->length < (pos + 2) ) break;
        data->spare = bf_get_int16_be( bfile ); pos += 2;
    } while ( FALSE );

    if ( node->length > pos )
        pos += bf_read( bfile, node->data + pos - 4 + NODE_DATA_PAD,
            node->length - pos );

    return TRUE;
} /* prf_object_load_f() */

/**************************************************************************/

static
bool_t
prf_object_save_f(
    prf_node_t * node,
    prf_state_t * state,
    bfile_t * bfile )
{
    int pos;

    assert( node != NULL && state != NULL && bfile != NULL );

    if ( node->opcode != prf_object_info.opcode ) {
        prf_error( 9, "tried object save method on node of type %d.",
            node->opcode );
        return FALSE;
    }

    bf_put_uint16_be( bfile, node->opcode );
    bf_put_uint16_be( bfile, node->length );

    pos = 4;
    do {
        node_data * data;
        data = (node_data *) node->data;
        bf_write( bfile, (uint8_t *) data->ascii_id, 8 ); pos += 8;
        bf_put_uint32_be( bfile, data->flags ); pos += 4;
        bf_put_int16_be( bfile, data->relative_priority ); pos += 2;
        bf_put_uint16_be( bfile, data->transparency ); pos += 2;
        bf_put_int16_be( bfile, data->special_effect_id1 ); pos += 2;
        bf_put_int16_be( bfile, data->special_effect_id2 ); pos += 2;
        bf_put_int16_be( bfile, data->significance ); pos += 2;
        if ( node->length < (pos + 2) ) break;
        bf_put_int16_be( bfile, data->spare ); pos += 2;
    } while ( FALSE );

    if ( node->length > pos )
        pos += bf_write( bfile, node->data + pos - 4 + NODE_DATA_PAD,
            node->length - pos );

    return FALSE;
} /* prf_object_save_f() */

/**************************************************************************/

static
void
prf_object_entry_f(
    prf_node_t * node,
    prf_state_t * state )
{
    assert( node != NULL && state != NULL );
    state->object_transparency = ((node_data *)node->data)->transparency;
    state->object_flags = ((node_data *)node->data)->flags;
} /* prf_object_entry_f() */

static
void
prf_object_exit_f(
    prf_node_t * node,
    prf_state_t * state )
{
  assert( node != NULL && state != NULL );
  state->object_transparency = 0;
} /* prf_object_exit_f() */


/**************************************************************************/

static const prf_nodeinfo_t prf_object_info = {
    4, PRF_PRIMARY,
    "Object",
    prf_object_load_f,
    prf_object_save_f,
    prf_object_entry_f,
    prf_object_exit_f,
    NULL,
    NULL,
    NULL
}; /* struct prf_object_info */

/**************************************************************************/

/* $Id$ */

