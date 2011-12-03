/**************************************************************************\
 * Copyright (c) Kongsberg Oil & Gas Technologies AS
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 * 
 * Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 * 
 * Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 * 
 * Neither the name of the copyright holder nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
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
  node = (prf_node_t *)malloc( sizeof( struct prf_node_s ) );
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
    if (datasize) node->data = (uint8_t *)malloc(datasize);
  }
  else {
    node = (prf_node_t *)pool_malloc(model->mempool_id, sizeof( prf_node_t ) );
    prf_node_clear(node);
    if (datasize) 
      node->data = (uint8_t *)pool_malloc(model->mempool_id, datasize);
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
            newnode = (prf_node_t *)pool_malloc( target->mempool_id, 
						 sizeof( prf_node_t ) );
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
            newnode->data = (uint8_t *)malloc( node->length - 4 );
        else if ( node->length > 4 )
            newnode->data = (uint8_t *)pool_malloc( target->mempool_id, 
						    node->length - 4 );
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
