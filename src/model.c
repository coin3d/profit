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
#include <profit/basic_funcs.h>
#include <config.h>

#include <profit/model.h>
#include <profit/state.h>
#include <profit/debug.h>
#include <profit/node.h>
#include <profit/nodeinfo.h>
#include <profit/vertex.h>
#include <profit/nodes/matrix.h>
#include <profit/nodes/vertex_palette.h>
#include <profit/nodes/vertex_with_color.h>
#include <profit/nodes/obsolete_vertex_with_color.h>
#include <profit/nodes/vertex_with_normal.h>
#include <profit/nodes/obsolete_vertex_with_normal.h>
#include <profit/nodes/vertex_with_normal_and_texture.h>
#include <profit/nodes/vertex_with_texture.h>
#include <profit/util/array.h>
#include <profit/util/bfile.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

/*
  NOTES:
    The traversal code:
      If it was hard to write, it should be hard to read.  I prefer cursing
      to recursing ;)  It was actually readable before I started adding
      extra traversal control based on the callback return value.
      One problem is the so-called "logical" hierarchy you will find in
      OpenFlight files with primary and ancillary nodes independent of the
      control-node-managed hierarchy level.  Anyway, I believe I have gotten
      it right now, as long as all the "ancillary" nodes are flagged as such.
      BTW, the traversal code is becoming readable again, due to some extra
      hook functions...

  TODO: (no particular order)
    - rewrite in-order traversal to a recursive function for readability
    - convert a lot of asserts into fault-management, and deal with errors
    - implement TRAV_UP to skip all remaining siblings
    - implement save callback with traversal management
    - move arrays into memory pool on load when pooling is done
    - handle memory-pool'ed models as "read-only" models
    - make sub-nodes of super-nodes (e.g. vertex palette) traversable
         (the closest thing to recursion one will find in the traversal code)

    - implement instance definition / instance referencing (macros_fix)
    - implement replication (macros_fix)

  MIGHT DO:
    - switching
    - animation
    - morphing

  NOT TODO:
    - external references
      (need it? do it yourself with a load_f / traverse_f method)
*/

/**************************************************************************/

static bool_t prf_vertextras_init( prf_model_t * model );
static bool_t prf_vertextras_exit( prf_model_t * model );

#define  PRF_POOL_BLOCK_SIZE  (32*1024)

/**************************************************************************/

prf_model_t *
prf_model_create(
    void )
{
    prf_model_t * model;
    model = (prf_model_t *)malloc( sizeof( prf_model_t ) );
    if ( model == NULL )
        prf_error( 9, "memory allocation error (returned NULL)" );
    prf_model_clear( model );
    return model;
} /* prf_model_create() */

/**************************************************************************/

void
prf_model_clear(
    prf_model_t * model )
{
    if ( model != NULL ) {
        model->header = NULL;
        model->mempool_id = 0;
        model->vertex_palette = NULL;
        model->vertextras = NULL;
        model->ofversion = 0;
    }
} /* prf_model_clear() */

/**************************************************************************/

static
int
destroy_cb(
    void * sysdata,
    void * userdata )
{
    prf_state_t * state;
    prf_model_t * model;
    prf_node_t * node;

    state = (prf_state_t *) sysdata;
    model = (prf_model_t *) userdata;

    node = state->node;
    if ( node->children != NULL )
        prf_array_free( node->children );

    if ( model->mempool_id == 0 ) {
        if ( node->data != NULL )
            free( node->data );
        free( node );
    }

    return PRF_TRAV_CONTINUE;
} /* destroy_cb() */

void
prf_model_destroy(
    prf_model_t * model )
{
    assert( model != NULL );

    if ( model->header != NULL ) {
        prf_cb_t traverseCB;
        prf_cb_set( traverseCB, destroy_cb, model );
        prf_model_traverse_df( model, traverseCB );
    }

    if ( model->mempool_id != 0 )
        pool_destroy( model->mempool_id );

    if ( model->vertextras != NULL )
        prf_vertextras_exit( model );

    free( model );
} /* prf_model_destroy() */

/**************************************************************************/

void
prf_model_poolmem(
    prf_model_t * model )
{
    assert( model != NULL );
    if ( model->mempool_id != 0 )
        return;
    assert( model->header == NULL ); /* must pool before loading */
    model->mempool_id = pool_create();
} /* prf_model_poolmem() */

/**************************************************************************/

static
int
dummy_cb(
    void * sysdata,
    void * userdata )
{
    return PRF_TRAV_CONTINUE;
} /* dummy_cb() */

/**************************************************************************/

bool_t
prf_model_load_with_callback(
    prf_model_t * model,
    bfile_t * bfile,
    prf_cb_t callback )
{
    /* no recursing */
    prf_node_t *** stack;
    prf_state_t * state;
    int level;

    assert( model != NULL && bfile != NULL );
    assert( model->header == NULL );

    stack = (prf_node_t ***)prf_array_init( 8, sizeof( prf_node_t ** ) );
    assert( stack != NULL );
    stack[0] = (prf_node_t **)prf_array_init( 8, sizeof( prf_node_t * ) );
    assert( stack[0] != NULL );
    level = 0;
    state = prf_state_create();
    assert( state != NULL );
    state->model = model;

    while ( ! bf_at_eof( bfile ) ) {
        prf_node_t * node;
        prf_nodeinfo_t * info;

        if ( bf_get_remaining_length( bfile ) < 4 ) {
            prf_error( 6, "would reach EOF before reading node header." );
            goto error;
        }

        if ( model->mempool_id == 0 ) {
            node = prf_node_create();
            if ( node == NULL ) {
                prf_error( 6, "could not allocate memory for new node." );
                goto error;
            }
        } else {
            node = (prf_node_t *)pool_malloc( model->mempool_id, 
					      sizeof( prf_node_t ) );
            if ( node == NULL ) {
                prf_error( 6, "could not allocate memory for new node." );
                goto error;
            }
            prf_node_clear( node );
            node->flags |= PRF_NODE_MEMPOOLED;
        }

        node->opcode = bf_peek_uint16_be( bfile );

        if ( node->opcode == 0x0b00 ) { /* fix for moronic MultiGen bug */
          uint32_t nodedata, temp1, temp2;
          nodedata = bf_get_uint32_be( bfile );
          temp1 = nodedata & 0xff00ff00;
          temp2 = nodedata & 0x00ff00ff;
          nodedata = (temp1 >> 8) | (temp2 << 8);
          bf_unget_uint32_be( bfile, nodedata );

          node->opcode = bf_peek_uint16_be( bfile );
        }

        info = prf_nodeinfo_get( node->opcode );
        if ( (info != NULL) && (info->load_f != NULL) ) {
            state->node = node;
            if ( ! (*(info->load_f))( node, state, bfile ) )
                goto error;
        } else { /* unsupported node */
            node->opcode = bf_get_uint16_be( bfile );
            node->length = bf_get_uint16_be( bfile );
            if ( node->length > (bf_get_remaining_length( bfile ) + 4) )
                goto error;
            if ( node->length > 4 ) {
                if ( model->mempool_id == 0 ) {
                    node->data = (uint8_t *)malloc( node->length - 4 );
                } else {
                    node->data = (uint8_t *)pool_malloc( model->mempool_id,
                        node->length - 4 );
                    node->flags |= PRF_NODE_MEMPOOLED;
                }
                assert( node->data != NULL );
                bf_read( bfile, node->data, node->length - 4 );
            }
        }
        if ( info != 0 )
            prf_debug( 1, "node loaded: \"%s\"", info->name );
        else
            prf_debug( 1, "node loaded: opcode %d, length %d", node->opcode,
                node->length );

        /* insert the node in the model */
        if ( ((info->flags & PRF_PUSH_NODE) != 0) ||
             ((info->flags & PRF_ANCILLARY) != 0) ) {
            int cnt;
            cnt = prf_array_count( stack[level] );
            if ( (stack[level])[cnt-1]->children == NULL ) {
                stack = (prf_node_t ***)prf_array_set_count( stack, level+1 );
                stack = (prf_node_t ***)prf_array_append_ptr( stack, NULL );
                stack[level+1] = 
		  (prf_node_t **)prf_array_init( 8, sizeof( prf_node_t * ) );
            }
            if ( (info->flags & PRF_PUSH_NODE) != 0 )
                level++;
        }

        if ( (info->flags & PRF_ANCILLARY) != 0 ) {
            int cnt, anodes, i;
            assert( stack[level+1] != NULL );
            stack[level+1] = 
	      (prf_node_t **)prf_array_append_ptr( stack[level+1], node );
            assert( level >= 0 );
            cnt = prf_array_count( stack[level] );
            stack[level][cnt-1]->children = (prf_node_t **) stack[level+1];
            anodes = prf_array_count( stack[level+1] );
            for ( i = 0; i < anodes; i++ )
                stack[level+1][i]->parent = stack[level][cnt-1];
        } else {
            assert( stack[level] != NULL );
            stack[level] = 
	      (prf_node_t **)prf_array_append_ptr( stack[level], node );
        }

        prf_cb_call( callback, node );

        if ( (info->flags & PRF_POP_NODE) != 0 ) {
            int cnt, children, i;
            assert( level > 0 );
            cnt = prf_array_count( stack[level-1] );
            stack[level-1][cnt-1]->children = (prf_node_t **) stack[level];
            children = prf_array_count( stack[level] );
            for ( i = 0; i < children; i++ )
                stack[level][i]->parent = stack[level-1][cnt-1];
            level--;
        }
    } /* while ( ! bf_at_eof( bfile ) ) */

    if ( level > 0 )
        prf_warn( 6, "loading model left state on level %d.", level );

    prf_state_destroy( state );
    if ( prf_array_count( stack[0] ) != 1 ) {
        prf_error( 8, "model got multiple %d root nodes",
            prf_array_count( stack[0] ) );
        return FALSE;
    }

    assert( prf_array_count( stack ) > 0 &&
        stack[0] != NULL && prf_array_count( stack[0] ) > 0 );
    model->header = stack[0][0];
    prf_array_free( stack[0] );
    prf_array_free( stack );
    return TRUE;

error:
    do {
        uint16_t opcode;
        uint16_t length;
        prf_nodeinfo_t * info;
        opcode = bf_get_uint16_be( bfile );
        length = bf_get_uint16_be( bfile );
        bf_rewind( bfile, 4 );
        info = prf_nodeinfo_get( opcode );
        prf_error( 9, "could not load model database." );
        if ( info != NULL ) {
            prf_error( 6, "  failed while loading node \"%s\" (%d), length: %d",
                info->name, opcode, length );
        } else {
            prf_error( 6,
                "  failed while loading node with opcode %d, length: %d",
                opcode, length );
        }
        prf_error( 6, "  file: \"%s\", position: %d (of %d)",
            bf_get_filename( bfile ), bf_get_position( bfile ),
            bf_get_length( bfile ) );

        prf_debug( 6, "dumping node data" );
        bf_hex_dump( bfile, stderr, length, 4 );
    } while ( FALSE );
    return FALSE;
} /* prf_model_load_with_callback() */

bool_t
prf_model_load(
    prf_model_t * model,
    bfile_t * bfile )
{
    prf_cb_t callback;

    prf_cb_set( callback, dummy_cb, NULL );
    return prf_model_load_with_callback( model, bfile, callback );
} /* prf_model_load() */

/**************************************************************************/

static
int
save_cb(
    void * sysdata,
    void * userdata )
{
    prf_state_t * state;
    bfile_t * bfile;
    prf_node_t * node;
    prf_nodeinfo_t * info;

    state = (prf_state_t *) sysdata;
    bfile = (bfile_t *) userdata;
    node = state->node;

    if ( (node->flags & PRF_NODE_DELETED) != 0 || /* deleted nodes */
         (node->flags & PRF_NODE_VIRTUAL) != 0 )  /* "fake" helper-nodes */
        return PRF_TRAV_SIBLING;

    info = prf_nodeinfo_get( node->opcode );
    if ( (info != NULL) && (info->save_f != NULL) ) {
        (*(info->save_f))( node, state, bfile );
    } else {
        bf_put_uint16_be( bfile, node->opcode );
        bf_put_uint16_be( bfile, node->length );
        if ( node->length > 4 )
            bf_write( bfile, node->data, node->length - 4 );
    }

    return PRF_TRAV_CONTINUE;
} /* save_cb() */

bool_t
prf_model_save(
    prf_model_t * model,
    bfile_t * bfile )
{
    prf_cb_t saveCB;

    assert( model != NULL );
    assert( model->header != NULL );
    assert( bfile != NULL );
    assert( bf_is_writable( bfile ) == TRUE );

    prf_cb_set( saveCB, save_cb, bfile );
    prf_model_traverse_io( model, saveCB );

    return TRUE;
} /* prf_model_save() */

bool_t
prf_model_save_with_callback(
    prf_model_t * model,
    bfile_t * bfile,
    prf_cb_t callback )
{
    assert( 0 && "not implemented yet" ); /* FIXME: larsa 290499 */
} /* prf_model_save_with_callback() */

/**************************************************************************/

static
int
dump_cb(
    void * sysdata,
    void * userdata )
{
    prf_state_t * state;
    prf_nodeinfo_t * nodeinfo;
    static char dump_indent[] = "                                                                                                                                                                                                ";

    state = (prf_state_t *) sysdata;

    nodeinfo = prf_nodeinfo_get( state->node->opcode );
    assert( nodeinfo != NULL );

    dump_indent[ PRF_ABS( 2 * state->push_level ) ] = '\0';

    printf( "%sopcode %d, name \"%s\", size %d\n", dump_indent,
            nodeinfo->opcode, nodeinfo->name,
            state->node->length );

    if ( state->node->opcode == 67 )
        printf( "    => vertex palette size: %d\n",
            ((uint32_t *)(state->node->data))[0] );

    dump_indent[ PRF_ABS( 2 * state->push_level ) ] = ' ';

    return PRF_TRAV_CONTINUE;
} /* dump_cb() */

void
prf_model_dump(
    prf_model_t * model )
{
    prf_cb_t traverseCB;

    assert( model != NULL );
    assert( model->header != NULL );

    prf_cb_set( traverseCB, dump_cb, NULL );
    prf_model_traverse_io( model, traverseCB );

} /* prf_model_dump() */

/**************************************************************************/

bool_t
prf_model_macros_fix(
    prf_model_t * model,
    prf_cb_t callback )
{
    assert( model != NULL );

/* FIXME: 19990504 larsa: not implemented
   add replications
   add internal instance references */

    return TRUE;
} /* prf_model_macros_fix() */

/**************************************************************************/


static
int
vt_tag_cb(
    void * sysdata,
    void * userdata )
{
    prf_state_t * state = (prf_state_t *) sysdata;
    uint8_t * rlt = (uint8_t *) userdata;
    prf_node_t * node = state->node;

    if ( node->flags & PRF_NODE_DELETED )
        return PRF_TRAV_SIBLING;

    switch ( node->opcode ) {
    case 72: /* vertex list */
    case 89: /* morph vertex list */
        do {
            int i, num;
            uint32_t * data;
            data = (uint32_t *) node->data;
            num = (node->length - 4) / 4;
            for ( i = 0; i < num; i++ )
                ((uint32_t *)(rlt + data[i]))[0] += 1;
        } while ( FALSE );
        break;
    default:
        break;
    }
    return PRF_TRAV_CONTINUE;
} /* vt_tag_cb() */

static
int
vt_fix_cb(
    void * sysdata,
    void * userdata )
{
    prf_state_t * state = (prf_state_t *) sysdata;
    uint8_t * rlt = (uint8_t *) userdata;
    prf_node_t * node = state->node;

    if ( node->flags & PRF_NODE_DELETED )
        return PRF_TRAV_SIBLING;

    switch ( node->opcode ) {
    case 72: /* vertex list */
    case 89: /* morph vertex list */
        do {
            int i, num;
            uint32_t * data;
            data = (uint32_t *) node->data;
            num = (node->length - 4) / 4;
            for ( i = 0; i < num; i++ )
                data[i] = ((uint32_t *)(rlt + data[i]))[0];
        } while ( FALSE );
        break;
    default:
        break;
    }
    return PRF_TRAV_CONTINUE;
} /* vt_fix_cb() */

bool_t
prf_model_vertex_palette_optimize(
    prf_model_t * model )
{
    prf_cb_t tag_cb, fix_cb;

    uint32_t length = 0;
    uint8_t * rlt = NULL;

    assert( model != NULL && model->vertex_palette != NULL );

    do { /* create vertex list pool */
        length = ((uint32_t *)(model->vertex_palette->data))[0];
        if ( model->vertextras != NULL ) {
            int i, count;
            count = prf_array_count( model->vertextras );
            for ( i = 0; i < count; i++ )
                length += model->vertextras[i]->position;
        }
        rlt = (uint8_t *)malloc( length );
        if ( rlt == NULL ) {
            prf_error( 9, "memory allocation error (malloc returned NULL)" );
            return FALSE;
        }
        memset( rlt, 0, length );
    } while ( FALSE );

    do { /* traverse and tag vertices */
        prf_cb_func( tag_cb, vt_tag_cb );
        prf_cb_data( tag_cb, rlt );
        prf_model_traverse_io( model, tag_cb );
    } while ( FALSE );

    do { /* compact vertex list and create reverse lookup table */
        uint32_t offset = 0;
        uint32_t datasize = 0;
        uint32_t orig_length = 0;
        uint8_t * ptr = NULL, * origdata = NULL;
        uint8_t * buffer = (uint8_t *)malloc( length );

        if ( buffer == NULL ) {
            prf_error( 9, "memory allocation error (malloc returned NULL)" );
            free( rlt );
            return FALSE;
        }

        origdata = model->vertex_palette->data;
        datasize = ((uint32_t *)(origdata))[0];
       
        /* node for node, check if tagged, and update move offset */
        ptr = origdata + 8;
        while ( ((unsigned int) (ptr - origdata)) < datasize ) {
            uint16_t * uint16ptr = (uint16_t *) ptr;
            if ( ((uint32_t *)(rlt+(ptr-origdata)))[0] != 0 ) {
                /* vertex is in use, move vertex into new buffer */
                memmove( buffer + (ptr-origdata) - offset, ptr, uint16ptr[1] );
                /* set reverse lookup index in rtl table */
                ((uint32_t *)(rlt+(ptr-origdata)))[0] = (ptr-origdata) - offset;
            } else {
                /* vertex not in use => accumulate offset */
                offset += uint16ptr[1]; /* add node length to offset */
            }
            ptr += uint16ptr[1];
        }
        orig_length = ((uint32_t *)(origdata))[0];
        if ( model->vertextras != NULL ) {
            int num, i;
            num = prf_array_count( model->vertextras );
            for ( i = 0; i < num; i++ ) {
                unsigned int pos = 0;
                prf_vertex_pool_t * pool = model->vertextras[i];
                while ( pos < pool->position ) {
                     uint16_t * uint16ptr = (uint16_t *) (pool->data + pos);
                     if ( ((uint32_t *)(rlt + orig_length + pos))[0] != 0 ) {
                         /* vertex in use, move vertex to buffer */
                         memmove( buffer + (orig_length + pos) - offset,
                             uint16ptr, uint16ptr[1] );
                         /* create reverse lookup */
                         ((uint32_t *)(rlt + (orig_length+pos)))[0] =
                                (orig_length+pos) - offset;
                     } else {
                         /* vertex not in use, accumulate offset */
                         offset += uint16ptr[1];
                     }
                     pos += uint16ptr[1];
                }
                orig_length += pool->position;
            }
        }

        /* move buffer back into model with vertextras */
        if ( model->vertextras != NULL ) {
            int i, count = prf_array_count( model->vertextras );
            for ( i = 0; i < count; i++ ) {
                free( model->vertextras[i]->data );
                free( model->vertextras[i] );
            }
            prf_array_free( model->vertextras );
            model->vertextras = NULL;
        }

        if ( model->mempool_id == 0 ) {
            free( model->vertex_palette->data );
            model->vertex_palette->data = 
	      (uint8_t *)realloc( buffer, orig_length-offset );
        } else {
            if ( ((uint32_t *)model->vertex_palette->data)[0] >=
                 (orig_length - offset) ) {
                memcpy( model->vertex_palette->data, buffer,
                    orig_length - offset );
            } else {
                /* some memory is wasted here until memory pool is freed */
	      model->vertex_palette->data = (uint8_t *)
                    pool_malloc( model->mempool_id, orig_length - offset );
                memcpy( model->vertex_palette->data, buffer,
                    orig_length - offset );
            }
            free( buffer );
        }
        ((uint32_t *)model->vertex_palette->data)[0] = orig_length - offset;
    } while ( FALSE );

    do { /* traverse and fix vertex list offsets */
        prf_cb_func( fix_cb, vt_fix_cb );
        prf_cb_data( fix_cb, rlt );
        prf_model_traverse_io( model, fix_cb );
        free( rlt );
    } while ( FALSE );

    return TRUE;
} /* prf_model_vertex_list_optimize() */

/**************************************************************************/

/*
  io: in-order traversal:
      the normal way to traverse a tree - parents, then children
*/

/*
  traverse_io()

  when traversing a node, the root's entry_f and exit_f will be called by
  the new traverse fuunction, and should not be called from the calling
  function.
*/

int
prf_model_traverse_io_(
    prf_node_t * root,
    prf_state_t * state )
{
    /* only losers recurse ;) */
    prf_node_t *** stack = NULL;
    prf_node_t ** array = NULL;
    prf_node_t * node = NULL;
    int * child = NULL, * limit = NULL;
    int level = 0, traverse = PRF_TRAV_CONTINUE;
    prf_nodeinfo_t * info = NULL;
    int count, i;

    assert( root != NULL && state != NULL );

    stack = (prf_node_t ***)prf_array_init( 8, sizeof( prf_node_t ** ) );
    assert( stack != NULL );
    array = (prf_node_t **)prf_array_init( 1, sizeof( prf_node_t * ) );
    assert( array != NULL );
    array = (prf_node_t **)prf_array_append_ptr( array, root );
    stack = (prf_node_t ***)prf_array_append_ptr( stack, array );
    child = (int *)prf_array_init( 4, sizeof( int ) );
    assert( child != NULL );
    child = (int *)prf_array_append_int( child, 0 );
    limit = (int *)prf_array_init( 4, sizeof( int ) );
    assert( limit != NULL );
    limit = 
      (int *)prf_array_append_int( limit, prf_array_count(stack[0]));

    while ( child[0] < limit[0] ) {

        node = stack[level][child[level]];
        info = prf_nodeinfo_get( node->opcode );
        state->node = node;

        /* traverse */
        if ( (node->flags & PRF_NODE_DELETED) != 0 ) {
            traverse = PRF_TRAV_SIBLING;
        } else if ( info == NULL ) { /* nothing known about node */
            if ( state->callback.func != NULL )
                traverse = prf_cb_call( state->callback, state );
            else
                traverse = PRF_TRAV_CONTINUE;
        } else if ( info->traverse_f == NULL ) { /* traverse normally */
            if ( info->entry_f != NULL )
                (*(info->entry_f))( node, state );
            if ( state->callback.func != NULL )
                traverse = prf_cb_call( state->callback, state );
            else
                traverse = PRF_TRAV_CONTINUE;
            if ( info->exit_f != NULL )
                (*(info->exit_f))( node, state );
        } else { /* special-purpose traversal */
            traverse = (*(info->traverse_f))( node, state );
        }

        /* handle traversal control */
        if ( traverse == PRF_TRAV_EXIT )
            break;

        if ( traverse == PRF_TRAV_CONTINUE ) {
            /* visit children */
            if ( stack[level][child[level]]->children != NULL ) {
                stack = (prf_node_t ***)prf_array_append_ptr( stack,
                    stack[level][child[level]]->children );
                child = (int *)prf_array_append_int( child, 0 );
                limit = (int *)prf_array_append_int( limit,
                    prf_array_count( stack[level+1] ) );
                prf_state_push( state );
                level++;
                state->physical_level++;
                continue;
            }
        }

        if ( traverse == PRF_TRAV_POP ) {
            prf_nodeinfo_t * info;
            do {
                child[level]++;
                info = prf_nodeinfo_get( stack[level][child[level]]->opcode );
            } while ( (info == NULL) || ((info->flags & PRF_POP_NODE) == 0) );
            continue;
        }

        child[ level ]++;

        while ( (level > 0) && (child[level] >= limit[level]) ) {
            stack = 
	      (prf_node_t ***)prf_array_set_count(stack,
						  prf_array_count(stack)-1);
            child = (int *)prf_array_set_count(child,prf_array_count(child)-1);
            limit = (int *)prf_array_set_count(limit,prf_array_count(limit)-1);
            level--;
            child[ level ]++;
            prf_state_pop( state );
            state->physical_level--;
        }
    }

    count = prf_array_count( stack );
    for ( i = 0; i < count; i++ )
        prf_array_free( stack[i] );
    prf_array_free( stack );
    prf_array_free( child );
    prf_array_free( limit );

    return traverse;
} /* prf_model_traverse_io_() */

int
prf_model_traverse_io(
    prf_model_t * model,
    prf_cb_t callback )
{
    prf_state_t * state = NULL;
    int traverse;

    assert( model != NULL && model->header != NULL );

    state = prf_state_create();
    assert( state != NULL );
    state->model = model;
    state->traversal_method = PRF_TRAV_IN_ORDER;
    state->callback = callback;

    traverse = prf_model_traverse_io_( model->header, state );

    prf_state_destroy( state );

    return traverse;
} /* prf_model_traverse_io() */

/**************************************************************************/

/* warning: the callback is called from two places inside the while loop */

/*
  df: depth-first:
      visit parents after children (e.g. clean-up traversal)
*/

int
prf_model_traverse_df_(
    prf_node_t * root,
    prf_state_t * state )
{
    return PRF_TRAV_CONTINUE;
} /* prf_model_traverse_df_() */

int
prf_model_traverse_df( /* depth-first traversal */
    prf_model_t * model,
    prf_cb_t callback )
{
    prf_node_t *** stack, * node;
    prf_node_t ** array;
    int * child, * limit, level, traverse;
    prf_state_t * state;
    prf_nodeinfo_t * info;
    /* only losers recurse ;) */

    assert( model != NULL );
    assert( model->header != NULL );

    state = prf_state_create();
    state->model = model;
    stack = (prf_node_t ***)prf_array_init( 8, sizeof( prf_node_t ** ) );
    assert( stack != NULL );
    array = (prf_node_t **)prf_array_init( 1, sizeof( prf_node_t * ) );
    assert( array != NULL );
    array = (prf_node_t **)prf_array_append_ptr( array, model->header );
    stack = (prf_node_t ***)prf_array_append_ptr( stack, array );
    child = (int *)prf_array_init( 4, sizeof( int ) );
    assert( child != NULL );
    child = (int *)prf_array_append_int( child, 0 );
    limit = (int *)prf_array_init( 4, sizeof( int ) );
    assert( limit != NULL );
    limit = (int *)prf_array_append_int( limit, prf_array_count(stack[0]));
    traverse = PRF_TRAV_CONTINUE;

    level = 0;
    while ( child[0] < limit[0] ) {
        node = stack[level][child[level]];
        info = prf_nodeinfo_get( node->opcode );

        if ( (info != NULL) && (info->entry_f != NULL) )
            (*info->entry_f)(node, state);

        while ( node->children != NULL ) {
            stack = (prf_node_t ***)prf_array_append_ptr(stack,node->children);
            child = (int *)prf_array_append_int( child, 0 );
            limit = (int *)prf_array_append_int(limit, prf_array_count(stack[level+1]));
            prf_state_push( state );
            level++;
            state->physical_level++;
            node = stack[level][child[level]];
            info = prf_nodeinfo_get( node->opcode );
            if ( info->entry_f != NULL )
                (*info->entry_f)(node, state);
        }

        state->node = node;
        traverse = prf_cb_call( callback, state );

        if ( traverse == PRF_TRAV_EXIT )
            break;

        if ( traverse == PRF_TRAV_POP ) {
            prf_nodeinfo_t * info;
            info = prf_nodeinfo_get( stack[level][child[level]]->opcode );
            do {
                if ( info->exit_f != NULL )
                    (*(info->exit_f))( stack[level][child[level]], state );
                
                child[level]++;
                info = prf_nodeinfo_get( stack[level][child[level]]->opcode );
                if ( info->entry_f != NULL )
                    (*(info->entry_f))( stack[level][child[level]], state );
            } while ( (info->flags & PRF_POP_NODE) == 0 );
            continue;
        }

        child[level]++;

        if ( (level > 0) && (child[level] >= limit[level]) ) {
            stack = (prf_node_t ***)prf_array_set_count( stack, prf_array_count( stack ) - 1 );
            child = (int *)prf_array_set_count( child, prf_array_count( child ) - 1 );
            limit = (int *)prf_array_set_count( limit, prf_array_count( limit ) - 1 );
            prf_state_pop( state );
            level--;
            state->physical_level--;

            node = stack[level][child[level]];
            info = prf_nodeinfo_get( node->opcode );

            state->node = node;
            traverse = prf_cb_call( callback, state );

            if ( traverse == PRF_TRAV_EXIT )
                break;

            child[level]++;
        }
    }

    prf_state_destroy( state );
    prf_array_free( stack );
    prf_array_free( array );
    prf_array_free( child );
    prf_array_free( limit );
    return traverse;
} /* prf_model_traverse_df() */

/**************************************************************************/

/*
  bf: breadth-first: (not implemented)
      traverse graph, level by level (e.g. find match closest to top)
*/

int
prf_model_traverse_bf_(
    prf_node_t * node,
    prf_state_t * state,
    prf_node_t *** this_level,
    prf_state_t ** this_level_states,
    prf_node_t *** next_level,
    prf_state_t ** next_level_states )
{
    return PRF_TRAV_CONTINUE;
} /* prf_model_traverse_bf_() */

int
prf_model_traverse_bf(
    prf_model_t * model,
    prf_cb_t callback )
{
    prf_node_t *** this_level;
    prf_node_t *** next_level;
    prf_state_t ** this_level_states;
    prf_state_t ** next_level_states;
    prf_state_t * state; /* working state */
    prf_node_t ** array;
    int traverse, i, j;
    /* only *real* losers recurse on this one */

    state = prf_state_create();
    state->model = model;

    assert( model != NULL && model->header != NULL );

    this_level = (prf_node_t ***)prf_array_init( 8, sizeof( prf_node_t ** ) );
    this_level_states = (prf_state_t **)prf_array_init(8,sizeof(prf_state_t*));
    next_level = (prf_node_t ***)prf_array_init( 8, sizeof( prf_node_t ** ) );
    next_level_states = (prf_state_t **)prf_array_init(8,sizeof(prf_state_t*));

    array = (prf_node_t **)prf_array_init( 1, sizeof( prf_node_t * ) );
    assert( array != NULL );
    array = (prf_node_t **)prf_array_append_ptr( array, model->header );
    this_level = (prf_node_t ***)prf_array_append_ptr( this_level, array );
    this_level_states = 
      (prf_state_t **)prf_array_append_ptr( this_level_states,
					    prf_state_clone( state ) );

    traverse = PRF_TRAV_CONTINUE;

    while ( prf_array_count( this_level ) > 0 ) {
        prf_node_t *** temp_nodes;
        prf_state_t ** temp_states;
        
        for ( i = 0; i < prf_array_count( this_level ); i++ ) {
            prf_node_t ** children;

            children = this_level[i];
            prf_state_copy( state, this_level_states[i] );
            for ( j = 0; j < prf_array_count( children ); j++ ) {
                prf_nodeinfo_t * info;
                state->node = children[j];
                info = prf_nodeinfo_get( state->node->opcode );
                assert( info != NULL );
                if ( info->entry_f != NULL )
                    (*(info->entry_f))( state->node, state );
                traverse = prf_cb_call( callback, state );

                if ( traverse == PRF_TRAV_EXIT )
                    break;

                if ( (children[j]->children != NULL) &&
                     (traverse != PRF_TRAV_SIBLING) ) {
                    prf_state_t * clone;
                    clone = prf_state_clone( state );
                    prf_state_push( clone );
                    clone->physical_level++;
                    next_level = (prf_node_t ***)prf_array_append_ptr( 
                        next_level, children[j]->children );
                    next_level_states = (prf_state_t **)prf_array_append_ptr(
                        next_level_states, clone );
                }
                if ( info->exit_f != NULL )
                    (*(info->exit_f))( state->node, state );
            }
            if ( traverse == PRF_TRAV_EXIT )
                break;
        }

        if ( traverse == PRF_TRAV_EXIT )
            break;

        for ( i = 0; i < prf_array_count( this_level_states ); i++ ) {
            prf_state_destroy( this_level_states[i] );
            this_level_states[i] = NULL;
        }
        /* save and rotate the growable arrays */
        temp_nodes = this_level;
        temp_states = this_level_states;
        this_level = next_level;
        this_level_states = next_level_states;
        next_level = temp_nodes;
        next_level = (prf_node_t ***)prf_array_set_count( next_level, 0 );
        next_level_states = temp_states;
        next_level_states = 
	  (prf_state_t **)prf_array_set_count( next_level_states, 0 );
    }

    prf_state_destroy( state );
    prf_array_free( this_level );
    for ( i = 0; i < prf_array_count( this_level_states ); i++ )
        if ( this_level_states[i] != NULL )
            prf_state_destroy( this_level_states[i] );
    prf_array_free( this_level_states );
    prf_array_free( next_level );
    for ( i = 0; i < prf_array_count( next_level_states ); i++ )
        if ( next_level_states[i] != NULL )
            prf_state_destroy( next_level_states[i] );
    prf_array_free( next_level_states );
    return traverse;
} /* prf_model_traverse_bf() */

/**************************************************************************/

struct prf_clone_info_s {
    prf_model_t * model;
    prf_node_t ** stack;
};

static
int
clone_cb(
    void * sysdata,
    void * userdata )
{
    prf_state_t * state;
    struct prf_clone_info_s * clone_info;

    state = (prf_state_t *) sysdata;
    clone_info = (struct prf_clone_info_s *) userdata;

    if ( state->node->opcode == 1 ) {
        prf_node_t * node;
        assert( clone_info->model->header == NULL &&
                state->physical_level == 0 );
        node = prf_node_clone( state->node, state->model, clone_info->model );
        clone_info->model->header = node;
        clone_info->stack = 
	  (prf_node_t **)prf_array_append_ptr( clone_info->stack, node );
    } else {
        prf_node_t * node;
        assert( clone_info->model->header != NULL &&
                clone_info->stack != NULL );
        node = prf_node_clone( state->node, state->model, clone_info->model );
        node->parent = clone_info->stack[state->physical_level-1];
        if ( clone_info->stack[state->physical_level-1]->children == NULL )
            clone_info->stack[state->physical_level-1]->children =
                (prf_node_t **)prf_array_init( 4, sizeof( prf_node_t * ) );
        clone_info->stack[state->physical_level-1]->children =
            (prf_node_t **)prf_array_append_ptr(
                clone_info->stack[state->physical_level-1]->children, node );
        prf_array_set_count( clone_info->stack, state->physical_level );
        clone_info->stack = 
	  (prf_node_t **)prf_array_append_ptr( clone_info->stack, node );
    }

    return PRF_TRAV_CONTINUE;
} /* clone_cb() */

prf_model_t *
prf_model_clone(
    prf_model_t * model,
    bool_t use_mempooling )
{
    prf_model_t * clone;
    prf_state_t * state;
    struct prf_clone_info_s * clone_info;

    assert( model != NULL && model->header != NULL );

    clone = prf_model_create();
    state = prf_state_create();
    state->model = model;
    clone_info = 
      (struct prf_clone_info_s *)malloc( sizeof( struct prf_clone_info_s ) );
    clone_info->model = clone;
    clone_info->stack = (prf_node_t **)prf_array_init(8, sizeof(prf_node_t *));

    if ( use_mempooling != FALSE )
        prf_model_poolmem( clone );

    prf_cb_set( state->callback, clone_cb, clone_info );

    prf_model_traverse_io_( model->header, state );
    
    prf_array_free( clone_info->stack );
    free( clone_info );
    prf_state_destroy( state );

    return clone;
} /* prf_model_clone() */

/**************************************************************************/

static
bool_t
prf_vertextras_init(
    prf_model_t * model )
{
    prf_vertex_pool_t * first = NULL;

    assert( model != NULL );

    model->vertextras = (struct prf_vertex_pool_s **)
      prf_array_init( 4, sizeof( prf_vertex_pool_t * ) );
    assert( model->vertextras != NULL );
    first = (prf_vertex_pool_t *)malloc( sizeof( prf_vertex_pool_t ) );
    assert( first != NULL );
    model->vertextras = (struct prf_vertex_pool_s **)
      prf_array_append_ptr( model->vertextras, first );
    model->vertextras[0]->data = (uint8_t *)malloc( PRF_POOL_BLOCK_SIZE );
    assert( model->vertextras[0]->data != NULL );
    model->vertextras[0]->data_size = PRF_POOL_BLOCK_SIZE;
    model->vertextras[0]->position = 0;
    return TRUE;
} /* prf_vertextras_init() */

static
bool_t
prf_vertextras_exit(
    prf_model_t * model )
{
    int i, count;
    assert( model != NULL );

    if ( model->vertextras == NULL )
        return TRUE;

    count = prf_array_count( model->vertextras );
    for ( i = 0; i < count; i++ ) {
        free( model->vertextras[i]->data );
        free( model->vertextras[i] );
    }
    prf_array_free( model->vertextras );

    return TRUE;
} /* prf_vertextras_exit() */

/**************************************************************************/

/* returns byte offset to vertex in palette */

uint32_t
prf_model_add_vertex( 
    prf_model_t * model,
    prf_vertex_t * vertex )
{
    uint8_t * ptr8;
    uint16_t * ptr16;

    uint32_t offset;
    prf_vertex_pool_t * pool = NULL;
    uint16_t opcode;
    uint16_t length;
    int i, count;

    assert( model != NULL && vertex != NULL );
    assert( model->vertex_palette != NULL );

    if ( model->vertextras == NULL ) {
        if ( prf_vertextras_init( model ) == FALSE )
            return FALSE;
    }
    if ( vertex->has_normal != FALSE ) {
        if ( vertex->has_texture != FALSE ) {
            opcode = 70;
            length = 4+3*8+4+5*4+4+4;
        } else {
            opcode = 69;
            length = 4+3*8+4+3*4+4+4;
        }
    } else {
        if ( vertex->has_texture != FALSE ) {
            opcode = 71;
            length = 4+3*8+4+2*4+4+4;
        } else {
            opcode = 68;
            length = 4+3*8+4+4+4;
        }
    }

    offset = ((uint32_t *) (model->vertex_palette->data))[0];
    count = prf_array_count( model->vertextras );
    for ( i = 0; i < count; i++ ) {
        pool = model->vertextras[ i ];
        offset += pool->position;
    }

    if ( pool->data_size - pool->position < length ) {
        pool = (prf_vertex_pool_t *)malloc( sizeof( prf_vertex_pool_t ) );
        assert( pool != NULL );
        pool->data = (uint8_t *)malloc( PRF_POOL_BLOCK_SIZE );
        assert( pool->data != NULL );
        pool->data_size = PRF_POOL_BLOCK_SIZE;
        pool->position = 0;
        model->vertextras = (struct prf_vertex_pool_s **)
          prf_array_append_ptr( model->vertextras, pool);
    }

    ptr8 = pool->data + pool->position;
    ptr16 = (uint16_t *) ptr8;
    pool->position += length;

    ptr16[0] = opcode;
    ptr16[1] = length;

    switch ( opcode ) {
    case 68: /* vertex_with_color */
        do {
            struct prf_vertex_with_color_data * data;
            data = (struct prf_vertex_with_color_data *) (ptr8 + 4);
            prf_dblwrite( data->x, vertex->x );
            prf_dblwrite( data->y, vertex->y );
            prf_dblwrite( data->z, vertex->z );
            data->color_name_index = vertex->color_name_index;
            data->flags = vertex->flags;
            data->packed_color = vertex->packed_color;
            data->color_index = vertex->color_index;
        } while ( FALSE );
        break;
    case 69: /* vertex_with_normal */
        do {
            struct prf_vertex_with_normal_data * data;
            data = (struct prf_vertex_with_normal_data *) (ptr8 + 4);
            prf_dblwrite( data->x, vertex->x );
            prf_dblwrite( data->y, vertex->y );
            prf_dblwrite( data->z, vertex->z );
            data->color_name_index = vertex->color_name_index;
            data->flags = vertex->flags;
            data->normal[0] = vertex->normal[0];
            data->normal[1] = vertex->normal[1];
            data->normal[2] = vertex->normal[2];
            data->packed_color = vertex->packed_color;
            data->color_index = vertex->color_index;
        } while ( FALSE );
        break;
    case 70: /* vertex_with_normal_and_texture */
        do {
            struct prf_vertex_with_normal_and_texture_data * data;
            data = (struct prf_vertex_with_normal_and_texture_data *) (ptr8+4);
            prf_dblwrite( data->x, vertex->x );
            prf_dblwrite( data->y, vertex->y );
            prf_dblwrite( data->z, vertex->z );
            data->color_name_index = vertex->color_name_index;
            data->flags = vertex->flags;
            data->normal[0] = vertex->normal[0];
            data->normal[1] = vertex->normal[1];
            data->normal[2] = vertex->normal[2];
            data->texture[0] = vertex->texture[0];
            data->texture[1] = vertex->texture[1];
            data->packed_color = vertex->packed_color;
            data->color_index = vertex->color_index;
        } while ( FALSE );
        break;
    case 71: /* vertex_with_texture */
        do {
            struct prf_vertex_with_texture_data * data;
            data = (struct prf_vertex_with_texture_data *) (ptr8 + 4);
            prf_dblwrite( data->x, vertex->x );
            prf_dblwrite( data->y, vertex->y );
            prf_dblwrite( data->z, vertex->z );
            data->color_name_index = vertex->color_name_index;
            data->flags = vertex->flags;
            data->texture[0] = vertex->texture[0];
            data->texture[1] = vertex->texture[1];
            data->packed_color = vertex->packed_color;
            data->color_index = vertex->color_index;
        } while ( FALSE );
        break;
    default:
        assert( 0 && "invalid opcode - how did this happen?" );
        break;
    }
    
    return offset;
} /* prf_model_add_vertex() */

/**************************************************************************/

bool_t
prf_model_vertex_palette_lookup(
    prf_model_t * model,
    uint32_t offset,
    prf_vertex_t * vertex )
{
    prf_node_t * vertex_palette;
    struct prf_vertex_palette_data * vpdata;
    uint16_t * uint16ptr;
    prf_node_t node;
    
    assert( model != NULL && vertex != NULL );

    if ( model->vertex_palette == NULL ) {
        prf_debug( 3, "model contains no vertex palette too look up in" );
        return FALSE;
    }

    vertex_palette = model->vertex_palette;
    vpdata = (struct prf_vertex_palette_data *) vertex_palette->data;

    uint16ptr = NULL;
    if ( offset >= (uint32_t) vpdata->length ) {
        int i, count;
        offset -= vpdata->length;
        if ( model->vertextras == NULL ) {
            prf_debug( 3, "vertex palette lookup over vertex palette bounds" );
            return FALSE;
        }
        count = prf_array_count( model->vertextras );
        i = 0;
        for ( ; i < count && offset >= model->vertextras[i]->position; i++ )
            offset -= model->vertextras[i]->position;
        if ( i >= count ) {
            prf_debug( 3, "vertex palette lookup over vertex palette bounds" );
            return FALSE;
        }
        uint16ptr = (uint16_t *) (model->vertextras[i]->data + offset);
    } else {
        uint16ptr = (uint16_t *) (vertex_palette->data + offset);
    }

    prf_vertex_clear( vertex );
    prf_node_clear( &node );
    node.opcode = uint16ptr[0];
    node.length = uint16ptr[1];
    node.data = ((uint8_t *) (uint16ptr)) + 4;

    switch( node.opcode ) {
    case 8:
        prf_obsolete_vertex_with_color_fill_vertex( &node, vertex );
        break;
    case 9:
        prf_obsolete_vertex_with_normal_fill_vertex( &node, vertex );
        break;
    case 68:
        prf_vertex_with_color_fill_vertex( &node, vertex );
        break;
    case 69:
        prf_vertex_with_normal_fill_vertex( &node, vertex );
        break;
    case 70:
        prf_vertex_with_normal_and_texture_fill_vertex( &node, vertex );
        break;
    case 71:
        prf_vertex_with_texture_fill_vertex( &node, vertex );
        break;
    default:
        prf_debug( 6, "vertex lookup on unknown node type (%d)", node.opcode );
        return FALSE;
        break;
    }

    return TRUE;
} /* prf_model_vertex_palette_lookup() */

/**************************************************************************/

uint32_t
prf_model_vertex_palette_get_first_index(
    prf_model_t * model )
{
    assert( model != NULL );
    if ( ! model->vertex_palette )
        return 0;
    return 8;
} /* prf_vertex_palette_get_first_index() */

/**************************************************************************/

uint32_t
prf_model_vertex_palette_get_next_index(
    prf_model_t * model,
    uint32_t idx )
{
    uint32_t length;
    assert( model != NULL && idx >= 8 && ! (idx & 3) );
    assert( model->vertex_palette != NULL );

    length = ((uint32_t *)(model->vertex_palette->data))[0];
    assert( idx < length );
    idx = idx + ((uint16_t *)(model->vertex_palette->data + idx))[1];
    if ( idx >= length ) return 0;
    return idx;
} /* prf_vertex_palette_get_next_index() */

/**************************************************************************/

bool_t
prf_model_append_node(
    prf_model_t * model,
    prf_node_t * node )
{
    assert( model != NULL && node != NULL );

    if ( model->header != NULL && node->opcode != 1 ) {
        prf_node_t * node;
        prf_nodeinfo_t * info;
        int count;

        node = model->header;
        while ( node->children != NULL ) {
            if ( (count = prf_array_count( node->children )) > 0 )
                node = node->children[ count - 1 ];
        }
        info = prf_nodeinfo_get( node->opcode );
        while ( info != NULL &&
                (info->flags & (PRF_POP_NODE | PRF_ANCILLARY)) != 0 ) {
            node = node->parent;
            info = prf_nodeinfo_get( node->opcode );
        }

        
    } else if ( model->header == NULL && node->opcode == 1 ) {
        model->header = node;
    } else {
        prf_error( 9, "append node - node doesn't fit model" );
        return FALSE;
    }
    return TRUE;
} /* prf_model_append_node() */

/**************************************************************************/
