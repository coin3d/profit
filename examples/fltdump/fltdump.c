/**************************************************************************
 * fltdump.c
 *
 * This example file is public domain.
 **************************************************************************/

/* $Id$ */

#include <profit/profit.h>
#include <profit/model.h>
#include <profit/util/bfile.h>

#include <stdio.h>

/**************************************************************************/

int
main(
    int argc,
    char ** argv )
{
    prf_model_t * model;
    bfile_t * file;

    if ( argc != 2 ) {
        printf( "Usage: %s <inputfile>\n", argv[0] );
        return -1;
    }

    prf_init();

    model = prf_model_create();
    prf_model_poolmem( model );

    file = bf_create_r( argv[1] );
    if ( ! file ) {
        printf( "Error: Could not open '%s' for loading.\n", argv[1] );
        prf_exit();
        return -1;
    }

    if ( ! prf_model_load( model, file ) ) {
        printf( "Error: Couldn't parse model.\n" );
        prf_model_destroy( model );
        bf_destroy( file );
        prf_exit();
        return -1;
    }
    bf_destroy( file );

    prf_model_dump( model );

    prf_model_destroy( model );
    prf_exit();
    return 0;
} /* main() */

