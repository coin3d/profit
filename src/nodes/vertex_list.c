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

#include <profit/types.h>
#include <profit/node.h>
#include <profit/nodeinfo.h>
#include <profit/model.h>
#include <profit/state.h>
#include <profit/debug.h>
#include <profit/nodes/vertex_list.h>
#include <profit/util/bfile.h>
#include <config.h>

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

/**************************************************************************/

static prf_nodeinfo_t prf_vertex_list_info = {
    72, PRF_PRIMARY,
    "Vertex List",
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL
}; /* struct prf_vertex_list_info */

/**************************************************************************/

int
prf_vertex_list_get_count(
    prf_node_t * node )
{
    assert( node != NULL );

    if ( node->opcode != prf_vertex_list_info.opcode ) {
        prf_error( 9, "tried vertex_list_get_count() on node of type %d.",
            node->opcode );
        return 0;
    }

    return ((node->length - 4) / 4);
} /* prf_vertex_list_get_count() */

/**************************************************************************/

uint32_t *
prf_vertex_list_get_list_ptr(
    prf_node_t * node )
{
    assert( node != NULL );

    if ( node->opcode != prf_vertex_list_info.opcode ) {
        prf_error( 9, "tried vertex_list_get_list_ptr() on node of type %d.",
            node->opcode );
        return NULL;
    }

    return (uint32_t *) node->data;
} /* prf_vertex_list_get_list_ptr() */

/**************************************************************************/

void
prf_vertex_list_set_list(
    prf_node_t * node,
    int count,
    uint32_t * ptr )
{
    assert( node != NULL && ptr != NULL );
    assert( count > 2 && count <= ((node->length - 4) / 4) );

    if ( node->opcode != prf_vertex_list_info.opcode ) {
        prf_error( 9, "tried vertex_list_set_list() on node of type %d.",
            node->opcode );
        return;
    }

    node->length = (count * 4) + 4;
    memcpy( node->data, ptr, count * 4 );
} /* prf_vertex_list_set_list() */

/**************************************************************************/

prf_node_t *
prf_vertex_list_node_create(
  prf_model_t * model,
  int numvertices )
{
  prf_node_t *node = prf_node_create_etc(model, numvertices*4);
  if (node) {
    node->opcode = 72;
    node->length = 4 + numvertices * 4;
  }
  return node;
} /* prf_vertex_list_node_create() */

/**************************************************************************/

static
bool_t
prf_vertex_list_load_f(
    prf_node_t * node,
    prf_state_t * state,
    bfile_t * bfile )
{
    int num, count;
    uint32_t * ptr;

    assert( node != NULL && state != NULL && bfile != NULL );

    node->opcode = bf_get_uint16_be( bfile );
    if ( node->opcode != prf_vertex_list_info.opcode ) {
        prf_error( 9, "tried vertex list load method on node of type %d.",
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

    ptr = (uint32_t *) node->data;
    num = 0;
    count = (node->length - 4) / 4;
    while ( num < count ) {
        ptr[num] = bf_get_uint32_be( bfile );
        num++;
    }

    return TRUE;
} /* prf_vertex_list_load_f() */

/**************************************************************************/

static
bool_t
prf_vertex_list_save_f(
    prf_node_t * node,
    prf_state_t * state,
    bfile_t * bfile )
{
    uint32_t * ptr;
    int num, count;

    assert( node != NULL && state != NULL && bfile != NULL );

    if ( node->opcode != prf_vertex_list_info.opcode ) {
        prf_error( 9, "tried vertex list save method on node of type %d.",
            node->opcode );
        return FALSE;
    }

    bf_put_uint16_be( bfile, node->opcode );
    bf_put_uint16_be( bfile, node->length );

    ptr = (uint32_t *) node->data;
    num = 0;
    count = (node->length - 4) / 4;
    while ( num < count ) {
        bf_put_uint32_be( bfile, ptr[num] );
        num++;
    }

    return TRUE;
} /* prf_vertex_list_save_f() */

/**************************************************************************/

void
prf_vertex_list_init(
    void )
{
    prf_vertex_list_info.load_f = prf_vertex_list_load_f;
    prf_vertex_list_info.save_f = prf_vertex_list_save_f;
    prf_nodeinfo_set( &prf_vertex_list_info );
} /* prf_vertex_list_init() */

/**************************************************************************/
