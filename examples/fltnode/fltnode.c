/**************************************************************************
 * fltnode.c
 *
 * This example file is public domain.
 **************************************************************************/

/* $Id$ */

#include <profit/profit.h>
#include <profit/model.h>
#include <profit/nodeinfo.h>
#include <profit/node.h>
#include <profit/callback.h>
#include <profit/util/bfile.h>

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

/**************************************************************************/

long position;

int
callback(
    void * sys,
    void * user )
{
    prf_node_t * node;
    prf_nodeinfo_t * info;
    bfile_t * file;
    long fpos;

    node = (prf_node_t *) sys;
    file = (bfile_t *) user;

    fpos = bf_get_position( file );
    if ( fpos > position ) {
        int pos;
        info = prf_nodeinfo_get( node->opcode );
        assert( info != NULL );
        printf( "enclosing node: \"%s\" (opcode=%d)\n", info->name,
           info->opcode );
        pos = fpos - node->length;
        printf( "    position: %d (+%d) [node size %d]\n",
            pos, position - pos, node->length );
        exit( 0 ); /* model load can't be broken with PRF_TRAV_EXIT */
    }
    return PRF_TRAV_CONTINUE;
} /* callback() */

/**************************************************************************/

int
main(
    int argc,
    char ** argv )
{
    prf_model_t * model;
    bfile_t * file;
    prf_cb_t loadCB;

    if ( argc != 3 ) {
        printf( "Usage: %s <pos> <inputfile>\n", argv[0] );
        return -1;
    }

    position = atoi( argv[1] );

    prf_init();

    model = prf_model_create();
    prf_model_poolmem( model );

    file = bf_create_r( argv[2] );
    if ( ! file ) {
        printf( "Error: Could not open '%s' for loading.\n", argv[2] );
        prf_exit();
        return -1;
    }

    prf_cb_set( loadCB, callback, file );

    if ( ! prf_model_load_with_callback( model, file, loadCB ) ) {
        printf( "Error: Couldn't parse model.\n" );
        prf_model_destroy( model );
        bf_destroy( file );
        prf_exit();
        return -1;
    }
    bf_destroy( file );

    prf_model_destroy( model );
    prf_exit();

    return 0;
} /* main() */

