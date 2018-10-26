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

#include <profit/basics.h>
#include <profit/node.h>
#include <profit/nodeinfo.h>
#include <profit/model.h>
#include <profit/state.h>
#include <profit/debug.h>
#include <profit/nodes/instance_definition.h>
#include <profit/util/bfile.h>
#include <profit/util/array.h>
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

static prf_nodeinfo_t prf_instance_definition_info = {
    62, PRF_CONTROL | PRF_DEFINITION,
    "Instance Definition",
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL
}; /* struct instance_definition_info */

/**************************************************************************/

typedef struct prf_instance_definition_data node_data;

/**************************************************************************/

static bool_t
prf_instance_definition_load_f(
    prf_node_t * node,
    prf_state_t * state,
    bfile_t * bfile )
{
    node_data * data;
    uint32_t pos = 4;

    assert( node != NULL && state != NULL && bfile != NULL );

    node->opcode = bf_get_uint16_be( bfile );
    if ( node->opcode != prf_instance_definition_info.opcode ) {
        prf_error( 9, "tried instance definition load method for "
                   "node of type %d.", node->opcode );
        bf_rewind( bfile, 2 );
        return FALSE;
    }

    node->length = bf_get_uint16_be( bfile );
    assert(node->length >= 8);

    if ( node->data == NULL && node->length > 4 ) {
        assert( state->model != NULL );
        if ( state->model->mempool_id == 0 )
            node->data = (uint8_t *) malloc( node->length - 4);
        else
            node->data = (uint8_t *) pool_malloc( state->model->mempool_id,
                                                  node->length - 4 );
        if ( node->data == NULL ) {
            prf_error( 9, "memory allocation problem (returned NULL)" );
            bf_rewind( bfile, 4 );
            return FALSE;
        }
    }


    data = (node_data *) node->data;
    data->spare = bf_get_int16_be( bfile ); pos += 2;
    data->instance_definition_number = bf_get_int16_be( bfile ); pos += 2;

    if ( pos < node->length ) /* padding */
        pos += bf_read( bfile, node->data + pos - 4, node->length - pos );
    return TRUE;
} /* instance_definition_load_f() */

/**************************************************************************/

static bool_t
prf_instance_definition_save_f(
    prf_node_t * node,
    prf_state_t * state,
    bfile_t * bfile)
{
    node_data * data;
    int pos = 4;

    assert( node != NULL && state != NULL && bfile != NULL );

    if ( node->opcode != prf_instance_definition_info.opcode) {
        prf_error( 9, "instance definition save method on node of type %d.",
                   node->opcode );
        return FALSE;
    }

    bf_put_uint16_be( bfile, node->opcode );
    bf_put_uint16_be( bfile, node->length );

    data = (node_data *) node->data;
    assert( data != NULL );
    bf_put_int16_be( bfile, data->spare ); pos += 2;
    bf_put_int16_be( bfile, data->instance_definition_number ); pos += 2;

    if ( pos < node->length ) /* padding */
        pos += bf_write( bfile, node->data + pos - 4, node->length - pos );
    return TRUE;
} /* instance_definition_save_f() */

/**************************************************************************/

static void
prf_instance_definition_entry_f(
    prf_node_t * node,
    prf_state_t * state )
{
    int i, n;
    assert( node != NULL && state != NULL );

    if ( node->opcode != prf_instance_definition_info.opcode ) {
        prf_error( 9, "instance definition entry state method tried "
                   "on node of type %d.", node->opcode );
        return;
    }
    n = prf_array_count( state->instances );
    for ( i = 0; i < n; i++ )
        if (state->instances[i] == node) break;
    if ( i == n )
        state->instances =
           (prf_node_t **) prf_array_append_ptr( state->instances, node );
} /* prf_instance_definition_entry_f() */

/**************************************************************************/

void
prf_instance_definition_init()
{
    prf_instance_definition_info.load_f = prf_instance_definition_load_f;
    prf_instance_definition_info.save_f = prf_instance_definition_save_f;
    prf_instance_definition_info.entry_f = prf_instance_definition_entry_f;
    prf_nodeinfo_set( &prf_instance_definition_info );
} /* instance_definition_init() */

/**************************************************************************/
