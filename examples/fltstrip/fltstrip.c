/**************************************************************************
 * fltstrip.c
 * 
 * This example file public domain.
 **************************************************************************/

/* $Id$ */

#include <profit/basics.h>
#include <profit/profit.h>
#include <profit/model.h>
#include <profit/nodeinfo.h>
#include <profit/util/bfile.h>

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

/**************************************************************************/

int
dont_save(
    prf_node_t * node,
    prf_state_t * state,
    bfile_t * bfile )
{
    /* hooked to nodes that shouldn't be saved */
    return PRF_TRAV_CONTINUE;
} /* dont_save() */

/**************************************************************************/

int
main(
    int argc,
    char ** argv )
{
    prf_model_t * model;
    bfile_t * bfile;
    int i;

    if ( argc <= 3 ) {
        printf( "Usage: %s <inputfile> <outputfile> <opcode> [<opcode> ...]\n",
            argv[0] );
        return -1;
    }
    
    bfile = bf_create_r( argv[1] );
    if ( ! bfile ) {
        printf( "Error: Could not open '%s' for loading.\n", argv[1] );
        return -1;
    }

    prf_init();

    model = prf_model_create();
    assert( model != NULL );
    prf_model_poolmem( model );

    if ( ! prf_model_load( model, bfile ) ) {
        printf( "Error: Couldn't parse model.\n" );
        bf_destroy( bfile );
        prf_exit();
        return -1;
    }
    bf_destroy( bfile );

    for ( i = 3; i < argc; i++ ) {
        prf_nodeinfo_t * info;
        info = prf_nodeinfo_get( atoi( argv[i] ) );
        if ( ! info ) {
            fprintf( stderr, "Warning: couldn't find nodeinfo for '%s'\n",
                argv[i] );
        } else {
            info->save_f = dont_save;
        }
    }

    bfile = bf_create_w( argv[2] );
    if ( ! bfile ) {
        printf( "Error: Could not open '%s' for writing.\n", argv[2] );
        prf_model_destroy( model );
        prf_exit();
        return -1;
    }
    prf_model_save( model, bfile );

    bf_destroy( bfile );
    prf_model_destroy( model );

    prf_exit();

    return 0;
} /* main() */

