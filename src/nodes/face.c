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

#include <profit/nodes/face.h>
#include <profit/model.h>
#include <profit/node.h>
#include <profit/nodeinfo.h>
#include <profit/state.h>
#include <profit/debug.h>
#include <profit/util/bfile.h>
#include <profit/util/mempool.h>
#include <config.h>

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

/**************************************************************************/

static prf_nodeinfo_t prf_face_info = {
    5, PRF_PRIMARY,
    "Face",
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL
}; /* struct prf_face_info */

/**************************************************************************/


typedef  struct prf_face_data  node_data;
#define  NODE_DATA_SIZE        76
#define  NODE_DATA_PAD         (sizeof(node_data)-NODE_DATA_SIZE)

/**************************************************************************/

void
prf_face_get_material(
    prf_node_t * node,
    prf_state_t * state,
    prf_material_t * material )
{
    node_data * data;
    prf_model_t * model;

    assert( node != NULL && state != NULL && material != NULL );
    prf_error( 9, "using prf_face_get_material() - not implemented" );

    data = (node_data *) node->data;
    model = state->model;

    if ( (data->material_index & 0x8000) == 0 ) {
        /* non-negative material index */
        prf_state_material_lookup( state, data->material_index, material );
        if ( data->light_mode == PRF_FACE_LIGHT_MODE_FACE_COLOR ) {
            if ( data->flags & PRF_FACE_FLAGS_NO_COLOR ){
                prf_warn( 6, "light mode = face color: face NO_COLOR" );
            } else if ( (data->flags & PRF_FACE_FLAGS_PACKED_COLOR) != 0 ) {
                float a, b, g, r;
                a = data->packed_color_primary >> 24;
                b = (data->packed_color_primary >> 16) & 0xff;
                g = (data->packed_color_primary >> 8) & 0xff;
                r = data->packed_color_primary & 0xff;
            } else {
                prf_state_color_lookup( state, data->color_name_index, 0 );
            }
        } else if ( data->light_mode == PRF_FACE_LIGHT_MODE_VERTEX_COLORS ) {
        } else if ( data->light_mode == 
                    PRF_FACE_LIGHT_MODE_FACE_COLOR_AND_NORMALS ) {
        } else if ( data->light_mode ==
                    PRF_FACE_LIGHT_MODE_VERTEX_COLORS_AND_NORMALS ) {
        } else {
            prf_warn( 9, "unknown face (0x%p) light mode (%d)", node,
                data->light_mode );
        }

    } else {
#if 0 /* not impemented */
        float r, g, b, a;
        /* no material - create default one */
        if ( data->flags & PRF_FACE_FLAGS_NO_COLOR ) {
        }
#endif /* 0 - not implemented yet */
    }
    /* deal with state->object_transparency & data->transparency */
} /* face_get_material() */

/**************************************************************************/

static
bool_t
prf_face_load_f(
    prf_node_t * node,
    prf_state_t * state,
    bfile_t * bfile )
{
    int pos;

    assert( node != NULL && state != NULL && bfile != NULL );
 
    node->opcode = bf_get_uint16_be( bfile );
    assert( node->opcode == prf_face_info.opcode );

    node->length = bf_get_uint16_be( bfile );
    if ( node->length < 44 ) {
        prf_error( 3, "face node too short (only %d bytes)", node->length );
        bf_rewind( bfile, 4 );
        return FALSE;
    }

    if ( node->length > 4 && node->data == NULL ) { /* not pre-allocated */
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

    pos = 4;
    do {
        node_data * data;
        data = (node_data *) node->data;

        bf_read( bfile, (uint8_t *) data->ascii_id, 8 ); pos += 8;
        data->ir_color_code = bf_get_int32_be( bfile ); pos += 4;
        data->relative_priority = bf_get_int16_be( bfile ); pos += 2;
        data->draw_type = bf_get_int8( bfile ); pos += 1;
        data->texture_white = bf_get_int8( bfile ); pos += 1;
        data->color_name_index = bf_get_uint16_be( bfile ); pos += 2;
        data->alternate_color_name_index = bf_get_uint16_be( bfile ); pos += 2;
        data->reserved1 = bf_get_int8( bfile ); pos += 1;
        data->billboard_flags = bf_get_int8( bfile ); pos += 1;
        data->detail_texture_pattern_index = bf_get_int16_be( bfile ); pos += 2;
        data->texture_pattern_index = bf_get_int16_be( bfile ); pos += 2;
        data->material_index = bf_get_int16_be( bfile ); pos += 2;
        data->surface_material_code = bf_get_int16_be( bfile ); pos += 2;
        data->feature_id = bf_get_int16_be( bfile ); pos += 2;
        data->ir_material_code = bf_get_int32_be( bfile ); pos += 4;
        data->transparency = bf_get_uint16_be( bfile ); pos += 2;
        data->lod_generation_control = bf_get_uint8( bfile ); pos += 1;
        data->line_style_index = bf_get_uint8( bfile ); pos += 1;
        if ( node->length < (pos + 4) ) break;
        data->flags = bf_get_uint32_be( bfile ); pos += 4;
        if ( node->length < (pos + 1) ) break;
        data->light_mode = bf_get_uint8( bfile ); pos += 1;
        if ( node->length < (pos + 1) ) break;
        data->reserved2 = bf_get_uint8( bfile ); pos += 1;
        if ( node->length < (pos + 2) ) break;
        data->reserved3 = bf_get_uint16_be( bfile ); pos += 2;
        if ( node->length < (pos + 4) ) break;
        data->reserved4 = bf_get_uint32_be( bfile ); pos += 4;
        if ( node->length < (pos + 4) ) break;
        data->packed_color_primary = bf_get_uint32_be( bfile ); pos += 4;
        if ( node->length < (pos + 4) ) break;
        data->packed_color_alternate = bf_get_uint32_be( bfile ); pos += 4;
        if ( node->length < (pos + 2) ) break;
        data->texture_mapping_index = bf_get_int16_be( bfile ); pos += 2;
        if ( node->length < (pos + 2) ) break;
        data->reserved5 = bf_get_int16_be( bfile ); pos += 2;
        if ( node->length < (pos + 4) ) break;
        data->primary_color_index = bf_get_uint32_be( bfile ); pos += 4;
        if ( node->length < (pos + 4) ) break;
        data->alternate_color_index = bf_get_uint32_be( bfile ); pos += 4;
        if ( node->length < (pos + 2) ) break;
        data->reserved6 = bf_get_int16_be( bfile ); pos += 2;
        if ( node->length < (pos + 2) ) break;
        data->reserved7 = bf_get_int16_be( bfile ); pos += 2;
    } while ( FALSE );

    if ( node->length > pos )
        pos += bf_read( bfile, node->data + pos - 4 + NODE_DATA_PAD,
            node->length - pos );

    return TRUE;
} /* prf_face_load_f() */

/**************************************************************************/

static
bool_t
prf_face_save_f(
    prf_node_t * node,
    prf_state_t * state,
    bfile_t * bfile )
{
    int pos;

    assert( node != NULL && state != NULL && bfile != NULL );

    if ( node->opcode != prf_face_info.opcode ) {
        prf_error( 6, "tried face node save method for other node type" );
        return FALSE;
    }

    bf_put_uint16_be( bfile, node->opcode );
    bf_put_uint16_be( bfile, node->length );

    pos = 4;
    do {
        node_data * data;
        data = (node_data *) node->data;
        bf_write( bfile, (uint8_t *) data->ascii_id, 8 ); pos += 8;
        bf_put_int32_be( bfile, data->ir_color_code ); pos += 4;
        bf_put_int16_be( bfile, data->relative_priority ); pos += 2;
        bf_put_int8( bfile, data->draw_type ); pos += 1;
        bf_put_int8( bfile, data->texture_white ); pos += 1;
        bf_put_uint16_be( bfile, data->color_name_index ); pos += 2;
        bf_put_uint16_be( bfile, data->alternate_color_name_index ); pos += 2;
        bf_put_int8( bfile, data->reserved1 ); pos += 1;
        bf_put_int8( bfile, data->billboard_flags ); pos += 1;
        bf_put_int16_be( bfile, data->detail_texture_pattern_index ); pos += 2;
        bf_put_int16_be( bfile, data->texture_pattern_index ); pos += 2;
        bf_put_int16_be( bfile, data->material_index ); pos += 2;
        bf_put_int16_be( bfile, data->surface_material_code ); pos += 2;
        bf_put_int16_be( bfile, data->feature_id ); pos += 2;
        bf_put_int32_be( bfile, data->ir_material_code ); pos += 4;
        bf_put_uint16_be( bfile, data->transparency ); pos += 2;
        bf_put_uint8( bfile, data->lod_generation_control ); pos += 1;
        bf_put_uint8( bfile, data->line_style_index ); pos += 1;
        if ( node->length < (pos + 4) ) break;
        bf_put_uint32_be( bfile, data->flags ); pos += 4;
        if ( node->length < (pos + 1) ) break;
        bf_put_uint8( bfile, data->light_mode ); pos += 1;
        if ( node->length < (pos + 1) ) break;
        bf_put_uint8( bfile, data->reserved2 ); pos += 1;
        if ( node->length < (pos + 2) ) break;
        bf_put_uint16_be( bfile, data->reserved3 ); pos += 2;
        if ( node->length < (pos + 4) ) break;
        bf_put_uint32_be( bfile, data->reserved4 ); pos += 4;
        if ( node->length < (pos + 4) ) break;
        bf_put_uint32_be( bfile, data->packed_color_primary ); pos += 4;
        if ( node->length < (pos + 4) ) break;
        bf_put_uint32_be( bfile, data->packed_color_alternate ); pos += 4;
        if ( node->length < (pos + 2) ) break;
        bf_put_int16_be( bfile, data->texture_mapping_index ); pos += 2;
        if ( node->length < (pos + 2) ) break;
        bf_put_int16_be( bfile, data->reserved5 ); pos += 2;
        if ( node->length < (pos + 4) ) break;
        bf_put_uint32_be( bfile, data->primary_color_index ); pos += 4;
        if ( node->length < (pos + 4) ) break;
        bf_put_uint32_be( bfile, data->alternate_color_index ); pos += 4;
        if ( node->length < (pos + 2) ) break;
        bf_put_int16_be( bfile, data->reserved6 ); pos += 2;
        if ( node->length < (pos + 2) ) break;
        bf_put_int16_be( bfile, data->reserved7 ); pos += 2;
    } while ( FALSE );

    if ( node->length > pos )
        pos += bf_write( bfile, node->data + pos - 4 + NODE_DATA_PAD,
            node->length - pos );

    return TRUE;
} /* prf_face_save_f() */

/**************************************************************************/

/*
static
void
prf_face_entry_f(
    prf_node_t * node,
    prf_state_t * state )
{
    assert( node != NULL && state != NULL );
} */ /* prf_face_entry_f() */

/**************************************************************************/

void
prf_face_init(
    void )
{
    prf_face_info.load_f = prf_face_load_f;
    prf_face_info.save_f = prf_face_save_f;
    prf_nodeinfo_set( &prf_face_info );
} /* prf_face_init() */

/**************************************************************************/
