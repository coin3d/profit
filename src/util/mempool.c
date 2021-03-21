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
#include <profit/util/mempool.h>
#include <profit/util/array.h>
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

/**************************************************************************/

#define NO_POOL                    (0)
#define POOL_BLOCK_SIZE      (16*1024)

typedef  struct pool_node_s *  pool_node_t;
typedef  struct pool_info_s *  pool_info_t;

struct pool_info_s {
    int block_size;
    pool_node_t * blocks;
}; /* struct pool_info_s */

struct pool_node_s {
    uint8_t * data;
    int data_size;
    int data_pos;
}; /* struct pool_node_s */

static pool_info_t * pools;

/**************************************************************************/

static
pool_info_t
new_pool(
    int block_size )
{
    pool_info_t pool;

    pool = (pool_info_t)malloc( sizeof( struct pool_info_s ) );
    assert( pool != NULL );
    pool->block_size = block_size;
    pool->blocks = (pool_node_t *)prf_array_init( 8, sizeof( pool_info_t ) );
    assert( pool->blocks != NULL );

    return pool;
} /* new_pool() */

/**************************************************************************/

void
pool_system_init(
    void )
{
    pools = (pool_info_t *)prf_array_init( 8, sizeof( pool_info_t ) );
    assert( pools != NULL );
    prf_array_append_ptr( pools, NULL ); /* reserve pools[0] */
} /* pool_system_init() */

void
pool_system_exit(
    void )
{
    pool_destroy_all();
    prf_array_free( pools );
} /* pool_system_exit() */

/**************************************************************************/

pool_t
pool_create(
    void )
{
    pool_t id;
    id = pool_create_sized( POOL_BLOCK_SIZE );
    return id;
} /* pool_create() */

pool_t
pool_create_sized(
    int block_size )
{
    pool_t pool_id, num_pools;
    num_pools = prf_array_count( pools );
    for ( pool_id = 1; pool_id < num_pools; pool_id++ ) {
        if ( pools[ pool_id ] == NULL ) { /* free slot */
            pools[ pool_id ] = new_pool( block_size );
            return pool_id;
        }
    }
    pools = (pool_info_t *)prf_array_append_ptr( pools, NULL );
    pools[ pool_id ] = new_pool( block_size );

    return pool_id;
} /* pool_create_sized() */

/**************************************************************************/

void
pool_destroy(
    pool_t pool_id )
{
    pool_t num_pools;
    pool_info_t pool;
    int num_blocks, i;

    num_pools = prf_array_count( pools );
    assert( (pool_id > 0) && (pool_id < num_pools));
    assert( pools[ pool_id ] != NULL );

    pool = pools[ pool_id ];
    num_blocks = prf_array_count( pool->blocks );
    for ( i = 0; i < num_blocks; i++ ) {
        free( pool->blocks[ i ]->data );
        free( pool->blocks[ i ] );
    }
    prf_array_free( pool->blocks );
    free( pool );
    pools[ pool_id ] = NULL;
} /* pool_destroy() */

/**************************************************************************/

void
pool_destroy_all(
    void )
{
    pool_t pool_id, num_pools;
    num_pools = prf_array_count( pools );
    for ( pool_id = 1; pool_id < num_pools; pool_id++ ) {
       if ( pools[ pool_id ] != NULL )
           pool_destroy( pool_id );
    }
} /* pool_destroy_all() */

/**************************************************************************/

void *
pool_malloc(
    pool_t pool_id,
    int size )
{
    int i, num_blocks;
    pool_info_t pool;
    pool_node_t newnode;
    void * ptr;

    assert( (pool_id > 0) && (pool_id < prf_array_count( pools )) );
    assert( pools[ pool_id ] != NULL );

    pool = pools[ pool_id ];
    size = (size + 7) & (~7); /* we're always aligning on eight bytes */

    num_blocks = prf_array_count( pool->blocks );
    for ( i = 0; i < num_blocks; i++ ) {
        if ( size <= (pool->blocks[i]->data_size -
                      pool->blocks[i]->data_pos) ) {
            ptr = pool->blocks[i]->data + pool->blocks[i]->data_pos;
            pool->blocks[i]->data_pos += size;
            return ptr;
        }
    }
    /* need new block */

    newnode = (pool_node_t)malloc( sizeof( struct pool_node_s ) );
    assert( newnode != NULL );
    newnode->data_size = PRF_MAX( pool->block_size, size );
    newnode->data_pos = size;
    newnode->data = (uint8_t *)malloc( newnode->data_size );
    assert( newnode->data != NULL );
    pool->blocks = (pool_node_t *)prf_array_append_ptr(pool->blocks, newnode );
    return newnode->data;
} /* pool_allocate() */

/**************************************************************************/
