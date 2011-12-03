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
#include <profit/state.h>
#include <profit/model.h>
#include <profit/nodeinfo.h>
#include <profit/debug.h>
#include <profit/nodes/group.h>
#include <profit/util/mempool.h>
#include <profit/util/bfile.h>
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

/**************************************************************************/

static prf_nodeinfo_t prf_group_info = {
    2, PRF_PRIMARY,
    "Group",
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL
}; /* struct prf_group_info */

/**************************************************************************/


typedef  struct prf_group_data  node_data;
#define  NODE_DATA_SIZE         28
#define  NODE_DATA_PAD          (sizeof(node_data)-NODE_DATA_SIZE)

/**************************************************************************/

static
bool_t
prf_group_load_f(
    prf_node_t * node,
    prf_state_t * state,
    bfile_t * bfile )
{
    int pos;

    assert( node != NULL && state != NULL && bfile != NULL );

    node->opcode = bf_get_uint16_be( bfile );
    if ( node->opcode != prf_group_info.opcode ) {
        prf_error( 9, "tried group load method for node of type %d.",
            node->opcode );
        bf_rewind( bfile, 2 );
        return FALSE;
    }

    node->length = bf_get_uint16_be( bfile );
    if ( node->length < 28 ) {
        prf_error( 3, "no support for so short (%d bytes) group nodes.",
            node->length );
        bf_rewind( bfile, 4 );
        return FALSE;
    }

    if ( node->length > 4 && node->data == NULL ) {
        assert( state->model != NULL );
        if ( state->model->mempool_id == 0 )
             node->data = (uint8_t *)malloc( node->length-4+NODE_DATA_PAD );
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
        data->relative_priority = bf_get_int16_be( bfile ); pos += 2;
        data->reserved1 = bf_get_int16_be( bfile ); pos += 2;
        data->flags = bf_get_uint32_be( bfile ); pos += 4;
        data->special_effect_id1 = bf_get_int16_be( bfile ); pos += 2;
        data->special_effect_id2 = bf_get_int16_be( bfile ); pos += 2;
        data->significance = bf_get_int16_be( bfile ); pos += 2;
        data->layer_code = bf_get_int8( bfile ); pos += 1;
        data->reserved2 = bf_get_int8( bfile ); pos += 1;
        if ( node->length < (pos + 4) ) break;
        data->reserved3 = bf_get_int32_be( bfile ); pos += 4;
    } while ( FALSE );

    if ( node->length > pos )
        pos += bf_read( bfile, node->data + pos - 4 + NODE_DATA_PAD,
            node->length - pos );

    return TRUE;
} /* prf_group_load_f() */

/**************************************************************************/

static
bool_t
prf_group_save_f(
    prf_node_t * node,
    prf_state_t * state,
    bfile_t * bfile )
{
    int pos;

    assert( node != NULL && state != NULL && bfile != NULL );

    if ( node->opcode != prf_group_info.opcode ) {
        prf_error( 3, "tried group save method on node of type %d.",
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
        bf_put_int16_be( bfile, data->relative_priority ); pos += 2;
        bf_put_int16_be( bfile, data->reserved1 ); pos += 2;
        bf_put_uint32_be( bfile, data->flags ); pos += 4;
        bf_put_int16_be( bfile, data->special_effect_id1 ); pos += 2;
        bf_put_int16_be( bfile, data->special_effect_id2 ); pos += 2;
        bf_put_int16_be( bfile, data->significance ); pos += 2;
        bf_put_int8( bfile, data->layer_code ); pos += 1;
        bf_put_int8( bfile, data->reserved2 ); pos += 1;
        if ( node->length < (pos + 4) ) break;
        bf_put_int32_be( bfile, data->reserved3 ); pos += 4;
    } while ( FALSE );

    if ( node->length > pos )
        pos += bf_write( bfile, node->data + pos - 4 + NODE_DATA_PAD,
            node->length - pos );

    return TRUE;
} /* prf_group_save_f() */

/**************************************************************************/

#if 0 /* not in use */

static
void
prf_group_entry_f(
    prf_node_t * node,
    prf_state_t * state )
{
} /* prf_group_entry_f() */

#endif /* 0 - not in use */

/**************************************************************************/

void
prf_group_init(
    void )
{
    prf_group_info.load_f = prf_group_load_f;
    prf_group_info.save_f = prf_group_save_f;
    prf_nodeinfo_set( &prf_group_info );
} /* prf_group_init() */

/**************************************************************************/
