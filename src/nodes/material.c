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

#include <profit/nodes/material.h>
#include <profit/basics.h>
#include <profit/model.h>
#include <profit/state.h>
#include <profit/nodeinfo.h>
#include <profit/node.h>
#include <profit/debug.h>
#include <profit/util/mempool.h>
#include <profit/util/bfile.h>
#include <profit/util/array.h>
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

/**************************************************************************/

static prf_nodeinfo_t prf_material_info = {
  113, PRF_ANCILLARY,
  "Material Record",
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL
}; /* struct prf_material_info */

/**************************************************************************/

typedef  struct prf_material_data  node_data;

#define  NODE_DATA_PAD 0

/**************************************************************************/

prf_node_t *
prf_material_node_create(
  prf_model_t * model,
  int index )
{
  prf_node_t * node = prf_node_create_etc( model, 
					   sizeof( struct prf_material_data ) );
  if ( node ) {
    struct prf_material_data *data;
    node->opcode = 113;
    node->length = 4 + sizeof( struct prf_material_data );
    
    data = (struct prf_material_data *) node->data;

    data->material_index = index;
    sprintf( data->name, "prfmat_%d", index );
    data->ambient_red = 0.2f;
    data->ambient_green = 0.2f;
    data->ambient_blue = 0.2f;

    data->diffuse_red = 0.2f;
    data->diffuse_green = 0.2f;
    data->diffuse_blue = 0.2f;

    data->specular_red = 0.0f;
    data->specular_green = 0.0f;
    data->specular_blue = 0.0f;

    data->emissive_red = 0.2f;
    data->emissive_green = 0.2f;
    data->emissive_blue = 0.2f;

    data->shininess = 20.0f;
    data->alpha = 1.0f;
    data->spare = 0;
  }
  return node;
} /* prf_material_node_create() */

/**************************************************************************/

static
bool_t
prf_material_load_f(
    prf_node_t * node,
    prf_state_t * state,
    bfile_t * bfile )
{
    int pos = 4;

    assert( node != NULL && state != NULL && bfile != NULL );

    node->opcode = bf_get_uint16_be( bfile );
    if ( node->opcode != prf_material_info.opcode ) {
        prf_error( 9, "tried to use material load method on node of type %d.",
            node->opcode );
        bf_rewind( bfile, 2 );
        return FALSE;
    }
    node->length = bf_get_uint16_be( bfile );

    if ( node->data == NULL && node->length > 4 ) {
        assert( state->model != NULL );
        if ( state->model->mempool_id == 0 )
            node->data = (uint8_t *)malloc( node->length - 4 + NODE_DATA_PAD );
        else
            node->data = (uint8_t *)pool_malloc( state->model->mempool_id,
                node->length - 4 + NODE_DATA_PAD );
        if ( node->data == NULL ) {
            prf_error( 9, "memory allocation problem (returned NULL)" );
            bf_rewind( bfile, 4 );
            return FALSE;
        }
    }

    do {
        node_data * data = (node_data *) node->data;
        if ( node->length < (pos + 4) ) break;
        data->material_index = bf_get_uint32_be( bfile ); pos += 4;
        if ( node->length < (pos + 12) ) break;
        bf_read( bfile, (uint8_t *) data->name, 12 ); pos += 12;
        if ( node->length < (pos + 4) ) break;
        data->flags = bf_get_uint32_be( bfile ); pos += 4;

        if ( node->length < (pos + 4) ) break;
        data->ambient_red = bf_get_float32_be( bfile ); pos += 4;
        if ( node->length < (pos + 4) ) break;
        data->ambient_green = bf_get_float32_be( bfile ); pos += 4;
        if ( node->length < (pos + 4) ) break;
        data->ambient_blue = bf_get_float32_be( bfile ); pos += 4;

        if ( node->length < (pos + 4) ) break;
        data->diffuse_red = bf_get_float32_be( bfile ); pos += 4;
        if ( node->length < (pos + 4) ) break;
        data->diffuse_green = bf_get_float32_be( bfile ); pos += 4;
        if ( node->length < (pos + 4) ) break;
        data->diffuse_blue = bf_get_float32_be( bfile ); pos += 4;

        if ( node->length < (pos + 4) ) break;
        data->specular_red = bf_get_float32_be( bfile ); pos += 4;
        if ( node->length < (pos + 4) ) break;
        data->specular_green = bf_get_float32_be( bfile ); pos += 4;
        if ( node->length < (pos + 4) ) break;
        data->specular_blue = bf_get_float32_be( bfile ); pos += 4;

        if ( node->length < (pos + 4) ) break;
        data->emissive_red = bf_get_float32_be( bfile ); pos += 4;
        if ( node->length < (pos + 4) ) break;
        data->emissive_green = bf_get_float32_be( bfile ); pos += 4;
        if ( node->length < (pos + 4) ) break;
        data->emissive_blue = bf_get_float32_be( bfile ); pos += 4;

        if ( node->length < (pos + 4) ) break;
        data->shininess = bf_get_float32_be( bfile ); pos += 4;
        if ( node->length < (pos + 4) ) break;
        data->alpha = bf_get_float32_be( bfile ); pos += 4;
        if ( node->length < (pos + 4) ) break;
        data->spare = bf_get_uint32_be( bfile ); pos += 4;
    } while ( FALSE );

    if ( pos < node->length )
        pos += bf_read( bfile, node->data + pos - 4 + NODE_DATA_PAD,
            node->length - pos );

    return TRUE;
} /* prf_material_load_f() */

/**************************************************************************/

static
bool_t
prf_material_save_f(
    prf_node_t * node,
    prf_state_t * state,
    bfile_t * bfile )
{
    int pos;

    assert( node != NULL && state != NULL && bfile != NULL );

    if ( node->opcode != prf_material_info.opcode ) {
        prf_error( 9, "tried using material save method for node of type %d.",
            node->opcode );
        return FALSE;
    }

    bf_put_uint16_be( bfile, node->opcode );
    bf_put_uint16_be( bfile, node->length );

    pos = 4;
    do {
        node_data * data;
        data = (node_data *) node->data;

        if ( node->length < (pos + 4) ) break;
        bf_put_uint32_be( bfile, data->material_index ); pos += 4;
        if ( node->length < (pos + 12) ) break;
        bf_write( bfile, (uint8_t *) data->name, 12 ); pos += 12;
        if ( node->length < (pos + 4) ) break;
        bf_put_uint32_be( bfile, data->flags ); pos += 4;

        if ( node->length < (pos + 4) ) break;
        bf_put_float32_be( bfile, data->ambient_red ); pos += 4;
        if ( node->length < (pos + 4) ) break;
        bf_put_float32_be( bfile, data->ambient_green ); pos += 4;
        if ( node->length < (pos + 4) ) break;
        bf_put_float32_be( bfile, data->ambient_blue ); pos += 4;

        if ( node->length < (pos + 4) ) break;
        bf_put_float32_be( bfile, data->diffuse_red ); pos += 4;
        if ( node->length < (pos + 4) ) break;
        bf_put_float32_be( bfile, data->diffuse_green ); pos += 4;
        if ( node->length < (pos + 4) ) break;
        bf_put_float32_be( bfile, data->diffuse_blue ); pos += 4;

        if ( node->length < (pos + 4) ) break;
        bf_put_float32_be( bfile, data->specular_red ); pos += 4;
        if ( node->length < (pos + 4) ) break;
        bf_put_float32_be( bfile, data->specular_green ); pos += 4;
        if ( node->length < (pos + 4) ) break;
        bf_put_float32_be( bfile, data->specular_blue ); pos += 4;

        if ( node->length < (pos + 4) ) break;
        bf_put_float32_be( bfile, data->emissive_red ); pos += 4;
        if ( node->length < (pos + 4) ) break;
        bf_put_float32_be( bfile, data->emissive_green ); pos += 4;
        if ( node->length < (pos + 4) ) break;
        bf_put_float32_be( bfile, data->emissive_blue ); pos += 4;

        if ( node->length < (pos + 4) ) break;
        bf_put_float32_be( bfile, data->shininess ); pos += 4;
        if ( node->length < (pos + 4) ) break;
        bf_put_float32_be( bfile, data->alpha ); pos += 4;
        if ( node->length < (pos + 4) ) break;
        bf_put_uint32_be( bfile, data->spare ); pos += 4;
    } while ( FALSE );

    if ( node->length > pos )
        pos += bf_write( bfile, node->data + pos - 4 + NODE_DATA_PAD,
            node->length - pos );

    return TRUE;
} /* prf_material_save_f() */

/**************************************************************************/

static
void
prf_material_entry_f(
    prf_node_t * node,
    prf_state_t * state )
{
    assert( node != NULL && state != NULL );

    if ( node->opcode != prf_material_info.opcode ) {
        prf_error( 9, "material entry state method tried on node of type %d.",
            node->opcode );
        return;
    }

    state->materials = (prf_node_t **)prf_array_append_ptr( state->materials, 
							    node );
} /* prf_material_entry_f() */

/**************************************************************************/

void
prf_material_init(
    void )
{
    prf_material_info.load_f = prf_material_load_f;
    prf_material_info.save_f = prf_material_save_f;
    prf_material_info.entry_f = prf_material_entry_f;
    prf_nodeinfo_set( &prf_material_info );
} /* prf_material_init() */

/**************************************************************************/
