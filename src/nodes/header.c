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

#include <profit/nodes/header.h>
#include <profit/types.h>
#include <profit/basic_funcs.h>
#include <profit/model.h>
#include <profit/state.h>
#include <profit/node.h>
#include <profit/nodeinfo.h>
#include <profit/debug.h>
#include <profit/util/bfile.h>
#include <profit/util/mempool.h>
#include <profit/util/array.h>
#include <config.h>

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

/**************************************************************************/

static prf_nodeinfo_t prf_header_info = {
    1, PRF_PRIMARY,
    "Header",
    NULL,
    NULL,
    NULL,
    NULL, /* exit_f */
    NULL, /* traverse_f */
    NULL, /* destroy_f */
    NULL
}; /* struct prf_header_info */

/**************************************************************************/


typedef  struct prf_header_data  node_data;
#define  NODE_DATA_SIZE          274
#define  NODE_DATA_PAD           (sizeof(node_data)-NODE_DATA_SIZE)

/**************************************************************************/

static
bool_t
prf_header_load_f(
    prf_node_t * node,
    prf_state_t * state,
    bfile_t * bfile )
{
    uint16_t pos;

    assert( node != NULL && state != NULL && bfile != NULL );

    node->opcode = bf_get_uint16_be( bfile );

    if ( node->opcode != prf_header_info.opcode ) {
        prf_error( 9, "tried header load method for node of type %d.",
            node->opcode );
        bf_rewind( bfile, 2 );
        return FALSE;
    }

    node->length = bf_get_uint16_be( bfile );

    pos = 4;

    if ( (node->length > 4) && (node->data == NULL) ) {
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
        node_data * data;
        data = (node_data *) node->data;
        if ( node->length < (pos + 8) ) break;
        bf_read( bfile, (uint8_t *) data->id, 8 ); pos += 8;
        if ( node->length < (pos + 4) ) break;
        data->format_revision_level = bf_get_int32_be( bfile ); pos += 4;
/*
        fprintf( stderr, "Open Flight revision level: %d\n",
            data->format_revision_level );
*/
        if ( node->length < (pos + 4) ) break;
        data->edit_revision_level = bf_get_int32_be( bfile ); pos += 4;
        if ( node->length < (pos + 32) ) break;
        bf_read( bfile, (uint8_t *) data->date_and_time, 32 ); pos += 32;
        if ( node->length < (pos + 2) ) break;
        data->next_group = bf_get_int16_be( bfile ); pos += 2;
        if ( node->length < (pos + 2) ) break;
        data->next_lod = bf_get_int16_be( bfile ); pos += 2;
        if ( node->length < (pos + 2) ) break;
        data->next_object = bf_get_int16_be( bfile ); pos += 2;
        if ( node->length < (pos + 2) ) break;
        data->next_face = bf_get_int16_be( bfile ); pos += 2;
        if ( node->length < (pos + 2) ) break;
        data->unit_multiplier_divisor = bf_get_int16_be( bfile ); pos += 2;
        if ( node->length < (pos + 1) ) break;
        data->vertex_coordinate_units = bf_get_int8( bfile ); pos += 1;
        if ( node->length < (pos + 1) ) break;
        data->texwhite = bf_get_int8( bfile ); pos += 1;
        if ( node->length < (pos + 4) ) break;
        data->flags = bf_get_uint32_be( bfile ); pos += 4;
        if ( node->length < (pos + 4) ) break;
        data->reserved1[0] = bf_get_int32_be( bfile ); pos += 4;
        if ( node->length < (pos + 4) ) break;
        data->reserved1[1] = bf_get_int32_be( bfile ); pos += 4;
        if ( node->length < (pos + 4) ) break;
        data->reserved1[2] = bf_get_int32_be( bfile ); pos += 4;
        if ( node->length < (pos + 4) ) break;
        data->reserved1[3] = bf_get_int32_be( bfile ); pos += 4;
        if ( node->length < (pos + 4) ) break;
        data->reserved1[4] = bf_get_int32_be( bfile ); pos += 4;
        if ( node->length < (pos + 4) ) break;
        data->reserved1[5] = bf_get_int32_be( bfile ); pos += 4;
        if ( node->length < (pos + 4) ) break;
        data->projection_type = bf_get_int32_be( bfile ); pos += 4;
        if ( node->length < (pos + 4) ) break;
        data->reserved2[0] = bf_get_int32_be( bfile ); pos += 4;
        if ( node->length < (pos + 4) ) break;
        data->reserved2[1] = bf_get_int32_be( bfile ); pos += 4;
        if ( node->length < (pos + 4) ) break;
        data->reserved2[2] = bf_get_int32_be( bfile ); pos += 4;
        if ( node->length < (pos + 4) ) break;
        data->reserved2[3] = bf_get_int32_be( bfile ); pos += 4;
        if ( node->length < (pos + 4) ) break;
        data->reserved2[4] = bf_get_int32_be( bfile ); pos += 4;
        if ( node->length < (pos + 4) ) break;
        data->reserved2[5] = bf_get_int32_be( bfile ); pos += 4;
        if ( node->length < (pos + 4) ) break;
        data->reserved2[6] = bf_get_int32_be( bfile ); pos += 4;
        if ( node->length < (pos + 2) ) break;
        data->next_dof = bf_get_int16_be( bfile ); pos += 2;
        if ( node->length < (pos + 2) ) break;
        data->vertex_storage_type = bf_get_int16_be( bfile ); pos += 2;
        if ( data->vertex_storage_type != 1 ) {
            prf_warn( 3, "vertex storage type (%d) may not be supported\n",
                data->vertex_storage_type );
            /* bf_rewind( bfile, pos ); return FALSE; */
        }
        if ( node->length < (pos + 4) ) break;
        data->database_origin = bf_get_int32_be( bfile ); pos += 4;
        if ( node->length < (pos + 8) ) break;
        prf_dblwrite( data->southwest_database_x, bf_get_float64_be( bfile ) );
            pos += 8;
        if ( node->length < (pos + 8) ) break;
        prf_dblwrite( data->southwest_database_y, bf_get_float64_be( bfile ) );
            pos += 8;
        if ( node->length < (pos + 8) ) break;
        prf_dblwrite( data->delta_x, bf_get_float64_be( bfile ) ); pos += 8;
        if ( node->length < (pos + 8) ) break;
        prf_dblwrite( data->delta_y, bf_get_float64_be( bfile ) ); pos += 8;
        if ( node->length < (pos + 2) ) break;
        data->next_sound = bf_get_int16_be( bfile ); pos += 2;
        if ( node->length < (pos + 2) ) break;
        data->next_path = bf_get_int16_be( bfile ); pos += 2;
        if ( node->length < (pos + 4) ) break;
        data->reserved3[0] = bf_get_int32_be( bfile ); pos += 4;
        if ( node->length < (pos + 4) ) break;
        data->reserved3[1] = bf_get_int32_be( bfile ); pos += 4;
        if ( node->length < (pos + 2) ) break;
        data->next_clip = bf_get_int16_be( bfile ); pos += 2;
        if ( node->length < (pos + 2) ) break;
        data->next_text = bf_get_int16_be( bfile ); pos += 2;
        if ( node->length < (pos + 2) ) break;
        data->next_bsp = bf_get_int16_be( bfile ); pos += 2;
        if ( node->length < (pos + 2) ) break;
        data->next_switch = bf_get_int16_be( bfile ); pos += 2;
        if ( node->length < (pos + 4) ) break;
        data->reserved4 = bf_get_int32_be( bfile ); pos += 4;
        if ( node->length < (pos + 8) ) break;
        prf_dblwrite( data->southwest_corner_latitude,
            bf_get_float64_be( bfile ) ); pos += 8;
        if ( node->length < (pos + 8) ) break;
        prf_dblwrite( data->southwest_corner_longitude,
            bf_get_float64_be( bfile )); pos += 8;
        if ( node->length < (pos + 8) ) break;
        prf_dblwrite( data->northeast_corner_latitude,
            bf_get_float64_be( bfile ) ); pos += 8;
        if ( node->length < (pos + 8) ) break;
        prf_dblwrite( data->northeast_corner_longitude,
            bf_get_float64_be( bfile )); pos += 8;
        if ( node->length < (pos + 8) ) break;
        prf_dblwrite( data->origin_latitude, bf_get_float64_be( bfile ) );
            pos += 8;
        if ( node->length < (pos + 8) ) break;
        prf_dblwrite( data->origin_longitude, bf_get_float64_be( bfile ) );
            pos += 8;
        if ( node->length < (pos + 8) ) break;
        prf_dblwrite( data->lambert_upper_latitude,
            bf_get_float64_be( bfile ) ); pos += 8;
        if ( node->length < (pos + 8) ) break;
        prf_dblwrite( data->lambert_lower_latitude,
            bf_get_float64_be( bfile ) ); pos += 8;
        if ( node->length < (pos + 2) ) break;
        data->next_light_source = bf_get_int16_be( bfile ); pos += 2;
        if ( node->length < (pos + 2) ) break;
        data->next_light_point = bf_get_int16_be( bfile ); pos += 2;
        if ( node->length < (pos + 2) ) break;
        data->next_road = bf_get_int16_be( bfile ); pos += 2;
        if ( node->length < (pos + 2) ) break;
        data->next_cat = bf_get_int16_be( bfile ); pos += 2;
        if ( node->length < (pos + 2) ) break;
        data->reserved5 = bf_get_int16_be( bfile ); pos += 2;
        if ( node->length < (pos + 2) ) break;
        data->reserved6 = bf_get_int16_be( bfile ); pos += 2;
        if ( node->length < (pos + 2) ) break;
        data->reserved7 = bf_get_int16_be( bfile ); pos += 2;
        if ( node->length < (pos + 2) ) break;
        data->reserved8 = bf_get_int16_be( bfile ); pos += 2;
        if ( node->length < (pos + 4) ) break;
        data->earth_ellipsoid_model = bf_get_int32_be( bfile ); pos += 4;
        if ( node->length < (pos + 2) ) break;
        data->next_adaptive = bf_get_int16_be( bfile ); pos += 2;
        if ( node->length < (pos + 2) ) break;
        data->next_curve = bf_get_int16_be( bfile ); pos += 2;
        if ( node->length < (pos + 2) ) break;
        data->reserved9 = bf_get_int16_be( bfile ); pos += 2;
    } while ( FALSE );

    if ( pos < node->length )
        pos += bf_read( bfile, node->data + pos - 4 + NODE_DATA_PAD,
            node->length - pos );

    return TRUE;
} /* prf_header_load_f() */

/**************************************************************************/

static
bool_t
prf_header_save_f(
    prf_node_t * node,
    prf_state_t * state,
    bfile_t * bfile )
{
    int pos;
    assert( node != NULL && state != NULL && bfile != NULL );

    if ( node->opcode != prf_header_info.opcode ) {
        prf_error( 9, "tried header save method on node of type %d.",
            node->opcode );
        return FALSE;
    }

    bf_put_uint16_be( bfile, node->opcode );
    bf_put_uint16_be( bfile, node->length );

    pos = 4;
    do {
        node_data * data;
        data = (node_data *) node->data;
        if ( node->length < (pos + 8) ) break;
        bf_write( bfile, (uint8_t *) data->id, 8 ); pos += 8;
        if ( node->length < (pos + 4) ) break;
        bf_put_int32_be( bfile, data->format_revision_level ); pos += 4;
        if ( node->length < (pos + 4) ) break;
        bf_put_int32_be( bfile, data->edit_revision_level ); pos += 4;
        if ( node->length < (pos + 32) ) break;
        bf_write( bfile, (uint8_t *) data->date_and_time, 32 ); pos += 32;
        if ( node->length < (pos + 2) ) break;
        bf_put_int16_be( bfile, data->next_group ); pos += 2;
        if ( node->length < (pos + 2) ) break;
        bf_put_int16_be( bfile, data->next_lod ); pos += 2;
        if ( node->length < (pos + 2) ) break;
        bf_put_int16_be( bfile, data->next_object ); pos += 2;
        if ( node->length < (pos + 2) ) break;
        bf_put_int16_be( bfile, data->next_face ); pos += 2;
        if ( node->length < (pos + 2) ) break;
        bf_put_int16_be( bfile, data->unit_multiplier_divisor ); pos += 2;
        if ( node->length < (pos + 1) ) break;
        bf_put_int8( bfile, data->vertex_coordinate_units ); pos += 1;
        if ( node->length < (pos + 1) ) break;
        bf_put_int8( bfile, data->texwhite ); pos += 1;
        if ( node->length < (pos + 4) ) break;
        bf_put_uint32_be( bfile, data->flags ); pos += 4;
        if ( node->length < (pos + 4) ) break;
        bf_put_int32_be( bfile, data->reserved1[0] ); pos += 4;
        if ( node->length < (pos + 4) ) break;
        bf_put_int32_be( bfile, data->reserved1[1] ); pos += 4;
        if ( node->length < (pos + 4) ) break;
        bf_put_int32_be( bfile, data->reserved1[2] ); pos += 4;
        if ( node->length < (pos + 4) ) break;
        bf_put_int32_be( bfile, data->reserved1[3] ); pos += 4;
        if ( node->length < (pos + 4) ) break;
        bf_put_int32_be( bfile, data->reserved1[4] ); pos += 4;
        if ( node->length < (pos + 4) ) break;
        bf_put_int32_be( bfile, data->reserved1[5] ); pos += 4;
        if ( node->length < (pos + 4) ) break;
        bf_put_int32_be( bfile, data->projection_type ); pos += 4;
        if ( node->length < (pos + 4) ) break;
        bf_put_int32_be( bfile, data->reserved2[0] ); pos += 4;
        if ( node->length < (pos + 4) ) break;
        bf_put_int32_be( bfile, data->reserved2[1] ); pos += 4;
        if ( node->length < (pos + 4) ) break;
        bf_put_int32_be( bfile, data->reserved2[2] ); pos += 4;
        if ( node->length < (pos + 4) ) break;
        bf_put_int32_be( bfile, data->reserved2[3] ); pos += 4;
        if ( node->length < (pos + 4) ) break;
        bf_put_int32_be( bfile, data->reserved2[4] ); pos += 4;
        if ( node->length < (pos + 4) ) break;
        bf_put_int32_be( bfile, data->reserved2[5] ); pos += 4;
        if ( node->length < (pos + 4) ) break;
        bf_put_int32_be( bfile, data->reserved2[6] ); pos += 4;
        if ( node->length < (pos + 2) ) break;
        bf_put_int16_be( bfile, data->next_dof ); pos += 2;
        if ( node->length < (pos + 2) ) break;
        bf_put_int16_be( bfile, data->vertex_storage_type ); pos += 2;
        if ( node->length < (pos + 4) ) break;
        bf_put_int32_be( bfile, data->database_origin ); pos += 4;
        if ( node->length < (pos + 8) ) break;
        bf_put_float64_be( bfile, prf_dblread( data->southwest_database_x ) );
            pos += 8;
        if ( node->length < (pos + 8) ) break;
        bf_put_float64_be( bfile, prf_dblread( data->southwest_database_y ) );
            pos += 8;
        if ( node->length < (pos + 8) ) break;
        bf_put_float64_be( bfile, prf_dblread( data->delta_x ) ); pos += 8;
        if ( node->length < (pos + 8) ) break;
        bf_put_float64_be( bfile, prf_dblread( data->delta_y ) ); pos += 8;
        if ( node->length < (pos + 2) ) break;
        bf_put_int16_be( bfile, data->next_sound ); pos += 2;
        if ( node->length < (pos + 2) ) break;
        bf_put_int16_be( bfile, data->next_path ); pos += 2;
        if ( node->length < (pos + 4) ) break;
        bf_put_int32_be( bfile, data->reserved3[0] ); pos += 4;
        if ( node->length < (pos + 4) ) break;
        bf_put_int32_be( bfile, data->reserved3[1] ); pos += 4;
        if ( node->length < (pos + 2) ) break;
        bf_put_int16_be( bfile, data->next_clip ); pos += 2;
        if ( node->length < (pos + 2) ) break;
        bf_put_int16_be( bfile, data->next_text ); pos += 2;
        if ( node->length < (pos + 2) ) break;
        bf_put_int16_be( bfile, data->next_bsp ); pos += 2;
        if ( node->length < (pos + 2) ) break;
        bf_put_int16_be( bfile, data->next_switch ); pos += 2;
        if ( node->length < (pos + 4) ) break;
        bf_put_int32_be( bfile, data->reserved4 ); pos += 4;
        if ( node->length < (pos + 8) ) break;
        bf_put_float64_be( bfile, prf_dblread( data->southwest_corner_latitude ) );
            pos += 8;
        if ( node->length < (pos + 8) ) break;
        bf_put_float64_be( bfile, prf_dblread( data->southwest_corner_longitude ) );
            pos += 8;
        if ( node->length < (pos + 8) ) break;
        bf_put_float64_be( bfile, prf_dblread( data->northeast_corner_latitude ) );
            pos += 8;
        if ( node->length < (pos + 8) ) break;
        bf_put_float64_be( bfile, prf_dblread( data->northeast_corner_longitude ) );
            pos += 8;
        if ( node->length < (pos + 8) ) break;
        bf_put_float64_be( bfile, prf_dblread( data->origin_latitude ) );
            pos += 8;
        if ( node->length < (pos + 8) ) break;
        bf_put_float64_be( bfile, prf_dblread( data->origin_longitude ) );
            pos += 8;
        if ( node->length < (pos + 8) ) break;
        bf_put_float64_be( bfile, prf_dblread( data->lambert_upper_latitude ) );
            pos += 8;
        if ( node->length < (pos + 8) ) break;
        bf_put_float64_be( bfile, prf_dblread( data->lambert_lower_latitude ) );
            pos += 8;
        if ( node->length < (pos + 2) ) break;
        bf_put_int16_be( bfile, data->next_light_source ); pos += 2;
        if ( node->length < (pos + 2) ) break;
        bf_put_int16_be( bfile, data->next_light_point ); pos += 2;
        if ( node->length < (pos + 2) ) break;
        bf_put_int16_be( bfile, data->next_road ); pos += 2;
        if ( node->length < (pos + 2) ) break;
        bf_put_int16_be( bfile, data->next_cat ); pos += 2;
        if ( node->length < (pos + 2) ) break;
        bf_put_int16_be( bfile, data->reserved5 ); pos += 2;
        if ( node->length < (pos + 2) ) break;
        bf_put_int16_be( bfile, data->reserved6 ); pos += 2;
        if ( node->length < (pos + 2) ) break;
        bf_put_int16_be( bfile, data->reserved7 ); pos += 2;
        if ( node->length < (pos + 2) ) break;
        bf_put_int16_be( bfile, data->reserved8 ); pos += 2;
        if ( node->length < (pos + 4) ) break;
        bf_put_int32_be( bfile, data->earth_ellipsoid_model ); pos += 4;
        if ( node->length < (pos + 2) ) break;
        bf_put_int16_be( bfile, data->next_adaptive ); pos += 2;
        if ( node->length < (pos + 2) ) break;
        bf_put_int16_be( bfile, data->next_curve ); pos += 2;
        if ( node->length < (pos + 2) ) break;
        bf_put_int16_be( bfile, data->reserved9 ); pos += 2;
    } while ( FALSE );

    if ( pos < node->length )
        pos += bf_write( bfile, node->data + pos - 4 + NODE_DATA_PAD,
            node->length - pos );

    return TRUE;
} /* prf_header_save_f() */

/**************************************************************************/

static
void
prf_header_entry_f(
    prf_node_t * node,
    prf_state_t * state )
{
    assert( node != NULL && state != NULL );

    if ( node->opcode != prf_header_info.opcode ) {
        prf_error( 9, "header entry state method tried on node of type %d.",
            node->opcode );
        return;
    }

    if ( state->header != NULL && state->header != node ) {
        prf_error( 9, "header entry: a header node is already registered" );
        return;
    }


    state->header = node;
    state->materials = (prf_node_t **)prf_array_set_count( state->materials,0);

    if ( state->model == NULL ) {
        prf_error( 9, "model is " );
    } else {
        node_data * data;
        data = (node_data *) node->data;
        state->model->ofversion = data->format_revision_level;
    }

} /* prf_header_entry_f() */

/**************************************************************************/

static
prf_node_t *
prf_header_clone_f(
    prf_node_t * orig,
    prf_model_t * source,
    prf_model_t * target )
{
    prf_node_t * clone;

    assert( orig != NULL && source != NULL && target != NULL );

    if ( target->mempool_id == 0 ) {
        clone = prf_node_create();
    } else {
        clone = (prf_node_t *)pool_malloc( target->mempool_id, 
					   sizeof( prf_node_t ) );
        prf_node_clear( clone );
    }
    if ( clone == NULL ) {
        prf_error( 9, "memory allocation failure (returned NULL)" );
        return NULL;
    }
    if ( target->mempool_id == 0 )
        clone->flags |= PRF_NODE_MEMPOOLED;

    clone->opcode = orig->opcode;
    clone->length = orig->length;
    assert( orig->data != NULL && orig->length > 4 );

    if ( target->mempool_id == 0 )
        clone->data = (uint8_t *)malloc( orig->length - 4 + NODE_DATA_PAD );
    else
        clone->data = (uint8_t *)pool_malloc( target->mempool_id,
            orig->length - 4 + NODE_DATA_PAD );
    if ( clone->data == NULL ) {
        prf_error( 9, "memory allocation failure (returned NULL)" );
        prf_node_destroy( clone );
        return NULL;
    }
    memcpy( clone->data, orig->data, orig->length - 4 + NODE_DATA_PAD );
 /* clone->flags */
    return clone;
} /* prf_header_clone_f() */

/**************************************************************************/

void
prf_header_init(
    void )
{
  prf_header_info.load_f=prf_header_load_f;
  prf_header_info.save_f=prf_header_save_f;
  prf_header_info.entry_f=prf_header_entry_f;
  prf_header_info.clone_f=prf_header_clone_f;
  prf_nodeinfo_set( &prf_header_info );
} /* header_init() */

/**************************************************************************/

/* $Id$ */

