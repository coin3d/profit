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

#include <profit/node.h>
#include <profit/nodeinfo.h>
#include <profit/model.h>
#include <profit/debug.h>
#include <config.h>
#include <profit/util/array.h>
#include <profit/util/mempool.h>

#include <stdlib.h>
#include <assert.h>

/**************************************************************************/

prf_node_t *
prf_node_create()
{
  prf_node_t * node;
  node = malloc( sizeof( struct prf_node_s ) );
  prf_node_clear( node );
  return node;
} /* prf_node_create() */

prf_node_t *
prf_node_create_etc(prf_model_t *model,
		    uint16_t datasize)
{
  prf_node_t *node = NULL;
  if (model->mempool_id == 0) {
    node = prf_node_create();
    if (datasize) node->data = malloc(datasize);
  }
  else {
    node = pool_malloc(model->mempool_id, sizeof( prf_node_t ) );
    prf_node_clear(node);
    if (datasize) node->data = pool_malloc(model->mempool_id, datasize);
  }
  
  if (node == NULL || (datasize > 0 && node->data == NULL)) {
    prf_error( 9, "memory allocation failure (returned NULL)" );
    return NULL;
  }
  else {
    node->length = 4 + datasize;
    return node;
  }
}

/**************************************************************************/

void
prf_node_clear(
    prf_node_t * node )
{
    if ( node != NULL ) {
        node->opcode = 0;
        node->length = 4;
        node->flags = 0;
        node->data = NULL;
        node->children = NULL;
        node->parent = NULL;
        node->userdata = NULL;
    }
} /* prf_node_clear() */

/**************************************************************************/

void
prf_node_destroy(
    prf_node_t * node )
{
    if ( node != NULL ) {
        if ( node->data )
            free( node->data );
        if ( node->children )
            prf_array_free( node->children );
        free( node );
    }
} /* prf_node_destroy() */

/**************************************************************************/

prf_node_t *
prf_node_clone(
    prf_node_t * node,
    prf_model_t * source,
    prf_model_t * target )
{
    prf_nodeinfo_t * info;

    assert( node != NULL && target != NULL );

    info = prf_nodeinfo_get( node->opcode );
    if ( info != NULL && info->clone_f != NULL ) {
        return (*(info->clone_f))( node, source, target );
    } else {
        prf_node_t * newnode;
        if ( target->mempool_id == 0 ) {
            newnode = prf_node_create();
        } else {
            newnode = pool_malloc( target->mempool_id, sizeof( prf_node_t ) );
            prf_node_clear( newnode );
        }
        if ( newnode == NULL ) {
            prf_error( 9, "memory allocation failure (returned NULL)" );
            return NULL;
        }
        newnode->opcode = node->opcode;
        newnode->length = node->length;
        if ( target->mempool_id != 0 )
            newnode->flags |= PRF_NODE_MEMPOOLED;
        if ( (node->length > 4) && (target->mempool_id == 0) )
            newnode->data = malloc( node->length - 4 );
        else if ( node->length > 4 )
            newnode->data = pool_malloc( target->mempool_id, node->length - 4 );
        if ( (node->length > 4) && (newnode->data == NULL) ) {
            prf_error( 9, "memory allocation failure (returned NULL)" );
            if ( target->mempool_id == 0 )
                free( newnode );
            return NULL;
        }
        memcpy( newnode->data, node->data, node->length - 4 );
        return newnode;
    }
} /* prf_node_clone() */

/**************************************************************************/

/* $Id$ */

