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

/*
  The vertex palette node slurps up all the vertices in one go.

  A consequence is that the vertices are hidden from the profit tree.
  - might make an option of traversing inside the palette later...
  Data must be read word by word to translate data to native endianness.
*/

#include <profit/types.h>
#include <profit/nodeinfo.h>
#include <profit/node.h>
#include <profit/vertex.h>
#include <profit/model.h>
#include <profit/state.h>
#include <profit/debug.h>
#include <profit/nodes/vertex_palette.h>
#include <profit/nodes/vertex_with_color.h>
#include <profit/nodes/obsolete_vertex_with_color.h>
#include <profit/nodes/vertex_with_normal.h>
#include <profit/nodes/obsolete_vertex_with_normal.h>
#include <profit/nodes/vertex_with_normal_and_texture.h>
#include <profit/nodes/vertex_with_texture.h>
#include <profit/util/bfile.h>
#include <profit/util/array.h>
#include <config.h>

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

/**************************************************************************/

static const prf_nodeinfo_t prf_vertex_palette_info;
typedef struct prf_vertex_palette_data node_data;

/**************************************************************************/

void
prf_vertex_palette_init(
    void )
{
    prf_nodeinfo_set( &prf_vertex_palette_info );
} /* prf_vertex_palette_init() */

/**************************************************************************/

static
bool_t
prf_vertex_palette_load_f(
    prf_node_t * node,
    prf_state_t * state,
    bfile_t * bfile )
{
    int32_t length;
    int curptr;
    prf_model_t fakemodel;
    node_data * vpdata;

    assert( node != NULL && state != NULL && bfile != NULL );
    
    fakemodel.header = NULL;
    fakemodel.mempool_id = 0;

    node->opcode = bf_get_uint16_be( bfile );
    if ( node->opcode != prf_vertex_palette_info.opcode ) {
        prf_error( 9, "tried vertex palette load method on node of type %d.",
            node->opcode );
        bf_rewind( bfile, 2 );
        return FALSE;
    }

    node->length = bf_get_int16_be( bfile );
    length = bf_get_int32_be( bfile );

    if ( length > 4 && node->data == NULL ) {
        assert( state->model != NULL );
        if ( state->model->mempool_id != 0 ) {
            node->data = pool_malloc( state->model->mempool_id, length );
        } else {
            node->data = malloc( length );
        }
        if ( node->data == NULL ) {
            prf_error( 9, "memory allocation problem (returned NULL)" );
            bf_rewind( bfile, 8 );
            return FALSE;
        }
    }
    vpdata = (node_data *) node->data;
    vpdata->length = length;

    /* four bytes are skipped */
    curptr = 8;
    while ( curptr < length ) {
        /* do it the hard - but general way */
        uint16_t opcode;
        prf_nodeinfo_t * subnodeinfo;
        prf_node_t subnode;

        opcode = bf_peek_uint16_be( bfile );
        subnodeinfo = prf_nodeinfo_get( opcode );
        if ( subnodeinfo != NULL && subnodeinfo->load_f != NULL ) {
            uint32_t oldpos, newpos;
            uint16_t * uint16ptr;

            oldpos = bf_get_position( bfile );
            subnode.data = node->data + curptr + 4;
            (*(subnodeinfo->load_f))( &subnode, state, bfile );
            newpos = bf_get_position( bfile );
            assert( (newpos - oldpos) == subnode.length );
            uint16ptr = (uint16_t *) (node->data + curptr);
            uint16ptr[0] = subnode.opcode;
            uint16ptr[1] = subnode.length;
            if ( (subnode.length > 4) &&
                (subnode.data != (node->data + curptr + 4)) ) {
                assert( subnode.data != NULL );
                memcpy( node->data + curptr + 4, subnode.data,
                    subnode.length - 4 );
            }
            curptr += subnode.length;
        } else {
            uint16_t nodelength;
            uint16_t * uint16ptr;
            opcode = bf_get_uint16_be( bfile ); /* opcode */
            nodelength = bf_get_uint16_be( bfile );
            uint16ptr = (uint16_t *) (node->data + curptr);
            uint16ptr[0] = opcode;
            uint16ptr[1] = nodelength;
            if ( nodelength > 4 )
                bf_read( bfile, node->data + curptr + 4, nodelength - 4 );
            curptr += nodelength;
        }
    }

    return TRUE;
} /* prf_vertex_palette_load_f() */

/**************************************************************************/

static
bool_t
prf_vertex_palette_save_f(
    prf_node_t * node,
    prf_state_t * state,
    bfile_t * bfile )
{
    node_data * vpdata;
    int32_t length;
    prf_node_t * subnode;
    int32_t curptr;

    assert( node != NULL && state != NULL && bfile != NULL );

    vpdata = (node_data *) node->data;
    length = vpdata->length;

    if ( state->model->vertextras != NULL ) {
        int i, count;
        count = prf_array_count( state->model->vertextras );
        for ( i = 0; i < count; i++ ) 
            length += state->model->vertextras[i]->position;
    }

    bf_put_uint16_be( bfile, node->opcode );
    bf_put_int16_be( bfile, node->length );
    bf_put_int32_be( bfile, length );

    subnode = prf_node_create();
    curptr = 8;
    while ( curptr < vpdata->length ) {
        uint16_t * uint16ptr;
        prf_nodeinfo_t * nodeinfo;

        uint16ptr = (uint16_t *) (node->data + curptr);
        subnode->opcode = uint16ptr[0];
        subnode->length = uint16ptr[1];
        subnode->data = node->data + curptr + 4;
        if ( subnode->length == 4 )
            subnode->data = NULL;
        nodeinfo = prf_nodeinfo_get( subnode->opcode );
        if ( nodeinfo != NULL && nodeinfo->save_f != NULL ) {
            (*(nodeinfo->save_f))( subnode, state, bfile );
        } else {
            prf_debug( 6, "saving node of type %d", node->opcode );
            bf_put_uint16_be( bfile, subnode->opcode );
            bf_put_uint16_be( bfile, subnode->length );
            if ( subnode->length > 4 )
                bf_write( bfile, subnode->data, subnode->length - 4 );
        }
        curptr += subnode->length;
    }

    if ( state->model->vertextras != NULL ) {
        int i, count;
        count = prf_array_count( state->model->vertextras );
        for ( i = 0; i < count; i++ ) {
            curptr = 0;
            while ( curptr < state->model->vertextras[i]->position ) {
                uint16_t * uint16ptr;
		prf_nodeinfo_t * nodeinfo;

		uint16ptr = (uint16_t *)
                    (state->model->vertextras[i]->data + curptr);
		subnode->opcode = uint16ptr[0];
		subnode->length = uint16ptr[1];
                subnode->data = state->model->vertextras[i]->data + curptr + 4;
                if ( subnode->length == 4 )
                    subnode->data = NULL;
		nodeinfo = prf_nodeinfo_get( subnode->opcode );
                if ( nodeinfo != NULL && nodeinfo->save_f != NULL ) {
                    (*(nodeinfo->save_f))( subnode, state, bfile );
                } else {
                    prf_debug( 6, "saving extra node of type %d", node->opcode );
                    bf_put_uint16_be( bfile, subnode->opcode );
                    bf_put_uint16_be( bfile, subnode->length );
                    if ( subnode->length > 4 )
                        bf_write( bfile, subnode->data, subnode->length - 4 );
                }
                curptr += subnode->length;
            }
        }
    }

    subnode->data = NULL;
    prf_node_destroy( subnode );

    return TRUE;
} /* prf_vertex_palette_save_f() */

/**************************************************************************/

static
void
prf_vertex_palette_entry_f(
    prf_node_t * node,
    prf_state_t * state )
{
    assert( node != NULL && state != NULL );

    assert( (state->vertex_palette == NULL) ||
            (state->vertex_palette == node) );

    if ( node->opcode != prf_vertex_palette_info.opcode ) {
        prf_error( 9,
            "tried vertex palette entry state method for node of type %d.",
            node->opcode );
        return;
    }

    state->vertex_palette = node;
    state->model->vertex_palette = node;
} /* prf_vertex_palette_entry_f() */

/**************************************************************************/

#if 0 /* 990505 larsa: not in use */

static
int
prf_vertex_palette_traverse_f(
    prf_node_t * node,
    prf_state_t * state )
{
    uint8_t * data;
    uint32_t length;
    int traverse = PRF_TRAV_CONTINUE;

    assert( node != NULL && state != NULL );

    prf_vertex_palette_entry_f( node, state );
    
    if ( state->callback.func != NULL )
        cb_call( state->callback, state );

    length = ((uint32_t *)node->data)[0];
    data = node->data + 8;
    while ( (data - node->data) < length ) {
        prf_node_t fakenode;
        prf_nodeinfo_t * info;

        prf_node_clear( &fakenode );
        fakenode.opcode = ((uint16_t *)data)[0];
        fakenode.length = ((uint16_t *)data)[1];
        fakenode.data = data + 4;
        fakenode.flags |= PRF_NODE_MEMPOOLED;
        state->node = &fakenode;

        info = prf_nodeinfo_get( fakenode.opcode );
        if ( info != NULL && info->traverse_f != NULL ) {
            (*info->traverse_f)( &fakenode, state );
        } else {
            if ( (info != NULL) && (info->entry_f != NULL) )
                (*info->entry_f)( &fakenode, state );

            if ( state->callback.func != NULL )
                traverse = cb_call( state->callback, state );

            if ( (info != NULL) && (info->exit_f != NULL) )
                (*info->exit_f)( &fakenode, state );

            if ( traverse == PRF_TRAV_EXIT || traverse == PRF_TRAV_UP ||
                 traverse == PRF_TRAV_POP )
                return traverse;

        }

        data += fakenode.length;
    }

    if ( state->model->vertextras != NULL ) {
        int i, count;
        prf_vertex_pool_t * pool;
        count = prf_array_count( state->model->vertextras );
        for ( i = 0; i < count; i++ ) {
            pool = state->model->vertextras[i];
            data = pool->data;
            while ( (data - pool->data) < pool->position ) {
                prf_node_t fakenode;
                prf_nodeinfo_t * info;
                prf_node_clear( &fakenode );
                fakenode.opcode = ((uint16_t *)data)[0];
                fakenode.length = ((uint16_t *)data)[1];
                fakenode.data = data + 4;
                fakenode.flags |= PRF_NODE_MEMPOOLED;
                
                state->node = &fakenode;
                info = prf_nodeinfo_get( fakenode.opcode );

                if ( info != NULL && info->traverse_f != NULL ) {
                    (*info->traverse_f)( &fakenode, state );
                } else {
                    if ( (info != NULL) && (info->entry_f != NULL) )
                        (*info->entry_f)( &fakenode, state );

                    if ( state->callback.func != NULL )
                        traverse = cb_call( state->callback, state );

                    if ( (info != NULL) && (info->exit_f != NULL) )
                        (*info->exit_f)( &fakenode, state );

                    if ( traverse == PRF_TRAV_EXIT || traverse == PRF_TRAV_UP ||
                         traverse == PRF_TRAV_POP )
                        return traverse;

                }
                data += fakenode.length;
            }
        }
    }

 /* prf_vertex_palette_exit_f( node, state ); */

    return traverse;
} /* vertex_palette_traverse_f() */

#endif /* 0 - not in use */

/**************************************************************************/

static
prf_node_t *
prf_vertex_palette_clone_f(
    prf_node_t * orig,
    prf_model_t * source,
    prf_model_t * target )
{
    prf_node_t * clone;
    uint32_t length;
    uint8_t * data;

    assert( orig != NULL && source != NULL && target != NULL );

    if ( target->mempool_id == 0 ) {
        clone = prf_node_create();
    } else {
        clone = pool_malloc( target->mempool_id, sizeof( prf_node_t ) );
        prf_node_clear( clone );
    }
    if ( clone == NULL ) {
        prf_error( 9, "memory allocation problem (returned NULL)" );
        return NULL;
    }
    clone->opcode = orig->opcode;
    clone->length = orig->length;
    if ( target->mempool_id != 0 )
        clone->flags |= PRF_NODE_MEMPOOLED;
    assert( orig->data != NULL );

    length = ((uint32_t *) orig->data)[0];
    if ( source->vertextras != NULL ) {
        int i, count;
        count = prf_array_count( source->vertextras );
        for ( i = 0; i < count; i++ )
            length += source->vertextras[i]->position;
    }
    if ( target->mempool_id == 0 ) 
        clone->data = malloc( length );
    else
        clone->data = pool_malloc( target->mempool_id, length );
    if ( clone->data == NULL ) {
        prf_error( 9, "memory allocation problems (returned NULL)" );
        if ( (clone->flags & PRF_NODE_MEMPOOLED) == 0 )
            prf_node_destroy( clone );
        return NULL;
    }
    data = clone->data;
    ((uint32_t *)data)[0] = length;
    data += 8;
    length = ((uint32_t *)orig->data)[0] - 8;
    memcpy( data, orig->data + 8, length );
    data += length;
    if ( source->vertextras != NULL ) {
        int i, count;
        count = prf_array_count( source->vertextras );
        for ( i = 0; i < count; i++ ) {
            memcpy( data, source->vertextras[i]->data,
                source->vertextras[i]->position );
            data += source->vertextras[i]->position;
        }
    }
    if ( target->vertextras != NULL )
        prf_debug( 6, "vertex palette - clone inserted into model with extras" );
    return clone;
} /* prf_vertex_palette_clone_f() */

/**************************************************************************/

static const prf_nodeinfo_t prf_vertex_palette_info = {
    67, PRF_ANCILLARY,
    "Vertex Palette",
    /* load_f */      prf_vertex_palette_load_f,
    /* save_f */      prf_vertex_palette_save_f,
    /* entry_f */     prf_vertex_palette_entry_f,
    /* exit_f */      NULL,
    /* traverse_f */  NULL, /* prf_vertex_palette_traverse_f, */
    /* destroy_f */   NULL,
    /* clone_f */     prf_vertex_palette_clone_f
}; /* struct prf_vertex_palette_info */

/**************************************************************************/

/* $Id$ */

