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

#include <profit/nodes/texture.h>
#include <profit/types.h>
#include <profit/model.h>
#include <profit/state.h>
#include <profit/nodeinfo.h>
#include <profit/node.h>
#include <profit/debug.h>
#include <profit/util/mempool.h>
#include <profit/util/bfile.h>
#include <profit/util/array.h>
#include <config.h>

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#ifdef HAVE_STRING_H
#include <string.h>
#endif

/**************************************************************************/

static prf_nodeinfo_t prf_texture_info = 
{
    64, PRF_ANCILLARY,
    "Texture Record",
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL
}; /* struct prf_texture_info */

/**************************************************************************/

typedef struct prf_texture_data node_data;

#define NODE_DATA_PAD 0

/**************************************************************************/

prf_node_t *
prf_texture_node_create(
    prf_model_t * model,
    int index,
    const char * filename )
{
    prf_node_t * node =
        prf_node_create_etc( model, sizeof( struct prf_texture_data ) );
    if ( node ) {
        struct prf_texture_data * data;
        node->opcode = 64;
        node->length = 4 + sizeof( struct prf_texture_data );
        data = (struct prf_texture_data *) node->data;
        data->pattern_index = index;
        data->x_location = 0;
        data->y_location = 0;
        strncpy( data->filename, filename, 200 );
    }
    return node;
} /* prf_texture_node_create() */

/**************************************************************************/

static bool_t
prf_texture_load_f(
    prf_node_t * node,
    prf_state_t * state,
    bfile_t * bfile)
{
    int pos = 4;

    assert( node != NULL && state != NULL && bfile != NULL );

    node->opcode = bf_get_uint16_be( bfile );
    if ( node->opcode != prf_texture_info.opcode ) {
        prf_error( 9, "tried to use texture load method on node of type %d.",
                   node->opcode );
        bf_rewind( bfile, 2 );
        return FALSE;
    }
    node->length = bf_get_uint16_be( bfile );

    if ( node->data == NULL && node->length > 4 ) {
        assert( state->model != NULL );
        if ( state->model->mempool_id == 0 )
            node->data =
                (uint8_t *) malloc( node->length - 4 + NODE_DATA_PAD );
        else
            node->data =
                (uint8_t *) pool_malloc( state->model->mempool_id,
                                         node->length - 4 + NODE_DATA_PAD );
        if ( node->data == NULL ) {
            prf_error( 9, "memory allocation problem (returned NULL)" );
            bf_rewind( bfile, 4 );
            return FALSE;
        }
    }
  
    do {
        node_data * data = (node_data *) node->data;
        if ( node->length < (pos + 200) ) break;
        bf_read( bfile, (unsigned char*) data->filename, 200 ); pos += 200;
        if ( node->length < (pos + 4) ) break;
        data->pattern_index = bf_get_int32_be( bfile ); pos += 4;
        if ( node->length < (pos + 4) ) break;
        data->x_location = bf_get_int32_be( bfile ); pos += 4;
        if ( node->length < (pos + 4) ) break;
        data->y_location = bf_get_int32_be( bfile ); pos += 4;
    } while ( FALSE );

    if ( pos < node->length )
        pos += bf_read( bfile, node->data + pos - 4 + NODE_DATA_PAD,
                        node->length - pos );
    return TRUE;
} /* prf_texture_load_f() */

/**************************************************************************/

static
bool_t
prf_texture_save_f(
    prf_node_t * node,
    prf_state_t * state,
    bfile_t * bfile )
{
    int pos;

    assert( node != NULL && state != NULL && bfile != NULL );

    if ( node->opcode != prf_texture_info.opcode ) {
        prf_error( 9, "tried using texture save method for node of type %d.",
                   node->opcode);
        return FALSE;
    }
    
    bf_put_uint16_be( bfile, node->opcode );
    bf_put_uint16_be( bfile, node->length );

    pos = 4;
    do {
        node_data * data;
        data = (node_data *) node->data;

        if ( node->length < (pos + 200) ) break;
        bf_write( bfile, (unsigned char *) data->filename, 200 ); pos += 200;
        if ( node->length < (pos + 4) ) break;
        bf_put_int32_be( bfile, data->pattern_index ); pos += 4;
        if ( node->length < (pos + 4) ) break;
        bf_put_int32_be( bfile, data->x_location ); pos += 4;
        if ( node->length < (pos + 4) ) break;
        bf_put_int32_be( bfile, data->y_location ); pos += 4;
    } while ( FALSE );

    if ( node->length > pos )
        pos += bf_write( bfile, node->data + pos - 4 + NODE_DATA_PAD,
                         node->length - pos);
  
    return TRUE;
} /* prf_texture_save_f() */

/**************************************************************************/

static
void
prf_texture_entry_f(
    prf_node_t * node,
    prf_state_t * state )
{
    assert( node != NULL && state != NULL );

    if ( node->opcode != prf_texture_info.opcode ) {
        prf_error( 9, "texture entry state method tried on node of type %d.",
                   node->opcode );
        return;
    }
    state->textures =
        (prf_node_t **) prf_array_append_ptr( state->textures, node );
} /* prf_texture_entry_f() */

/**************************************************************************/

void
prf_texture_init(
    void )
{
    prf_texture_info.load_f = prf_texture_load_f;
    prf_texture_info.save_f = prf_texture_save_f;
    prf_texture_info.entry_f = prf_texture_entry_f;
    prf_nodeinfo_set( &prf_texture_info );
} /* prf_texture_init() */

/**************************************************************************/
