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

#include <profit/messages.h>
#include <profit/util/array.h>
#include <config.h>

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

/**************************************************************************/

struct prf_msg_handler_s {
    int level;
    void (*func)( int, int, const char *, void * );
    void * userdata;
}; /* struct prf_msg_handler_s */

typedef struct prf_msg_handler_s prf_msg_handler_t;

static prf_msg_handler_t ** prf_handlers[6];

/**************************************************************************/

void
prf_messages_init(
    void )
{
    prf_handlers[ PRF_MSG_INFO ] =
        array_init( 4, sizeof( prf_msg_handler_t * ) );
    prf_handlers[ PRF_MSG_DEBUG ] =
        array_init( 4, sizeof( prf_msg_handler_t * ) );
    prf_handlers[ PRF_MSG_WARNING ] =
        array_init( 4, sizeof( prf_msg_handler_t * ) );
    prf_handlers[ PRF_MSG_ERROR ] =
        array_init( 4, sizeof( prf_msg_handler_t * ) );
    prf_handlers[ PRF_MSG_FATAL_ERROR ] =
        array_init( 4, sizeof( prf_msg_handler_t * ) );
} /* messages_init() */

/**************************************************************************/

void
prf_messages_exit(
    void )
{
    int i, count;

    count = array_count( prf_handlers[ PRF_MSG_INFO ] );
    for ( i = 0; i < count; i++ ) {
        if ( prf_handlers[ PRF_MSG_INFO ][ i ] != NULL ) {
            free( prf_handlers[ PRF_MSG_INFO ][ i ] );
        }
    }
    array_free( prf_handlers[ PRF_MSG_INFO ] );

    count = array_count( prf_handlers[ PRF_MSG_DEBUG ] );
    for ( i = 0; i < count; i++ ) {
        if ( prf_handlers[ PRF_MSG_DEBUG ][ i ] != NULL ) {
            free( prf_handlers[ PRF_MSG_DEBUG ][ i ] );
        }
    }
    array_free( prf_handlers[ PRF_MSG_DEBUG ] );

    count = array_count( prf_handlers[ PRF_MSG_WARNING ] );
    for ( i = 0; i < count; i++ ) {
        if ( prf_handlers[ PRF_MSG_WARNING ][ i ] != NULL ) {
            free( prf_handlers[ PRF_MSG_WARNING ][ i ] );
        }
    }
    array_free( prf_handlers[ PRF_MSG_WARNING ] );

    count = array_count( prf_handlers[ PRF_MSG_ERROR ] );
    for ( i = 0; i < count; i++ ) {
        if ( prf_handlers[ PRF_MSG_ERROR ][ i ] != NULL ) {
            free( prf_handlers[ PRF_MSG_ERROR ][ i ] );
        }
    }
    array_free( prf_handlers[ PRF_MSG_ERROR ] );

    count = array_count( prf_handlers[ PRF_MSG_FATAL_ERROR ] );
    for ( i = 0; i < count; i++ ) {
        if ( prf_handlers[ PRF_MSG_FATAL_ERROR ][ i ] != NULL ) {
            free( prf_handlers[ PRF_MSG_FATAL_ERROR ][ i ] );
        }
    }
    array_free( prf_handlers[ PRF_MSG_FATAL_ERROR ] );
} /* messages_exit() */

/**************************************************************************/

void
prf_messages_post_va(
    int type,
    int level,
    va_list args )
{
    bool_t present;
    int i, count;
    char string[ 256 ];
    assert( type > 0 && type < 6 );
    /* check that handler is present */
    present = FALSE;
    count = array_count( prf_handlers[type] );
    for ( i = 0; i < count; i++ ) {
        if ( prf_handlers[type][i] != NULL &&
             prf_handlers[type][i]->level <= level ) {
            present = TRUE;
        }
    }
    if ( present == FALSE ) {
        return;
    }
    /* build string */
    do {
        char * format;
        format = va_arg( args, char * );
        vsprintf( string, format, args );
    } while ( FALSE );

    /* call handlers with string */
    count = array_count( prf_handlers[ type ] );
    for ( i = 0; i < count; i++ ) {
        if ( prf_handlers[type][i] != NULL &&
             prf_handlers[type][i]->level <= level ) {
            (*(prf_handlers[type][i]->func))( type, level, string,
                 prf_handlers[type][i]->userdata );
        }
    }
} /* prf_messages_post_va() */

void
prf_messages_post(
    int type,
    int level,
    ... )
{
    va_list args;
    va_start( args, level );
    prf_messages_post_va( type, level, args );
    va_end( args );
} /* prf_message_post() */

/**************************************************************************/

void
prf_messages_add_handler(
    int type,
    int level,
    void (*func)( int, int, const char *, void * ),
    void * data )
{
    prf_msg_handler_t * handler;
    int i, count;
    assert( type > 0 && type < 6 );
    handler = malloc( sizeof( prf_msg_handler_t ) );
    assert( handler != NULL );
    handler->level = level;
    handler->func = func;
    handler->userdata = data;
    count = array_count( prf_handlers[type] );
    for ( i = 0; i < count; i++ ) {
        if ( prf_handlers[type][i] == NULL ) {
            prf_handlers[type][i] = handler;
            return;
        }
    }
    prf_handlers[type] = array_append_ptr( prf_handlers[type], handler );
} /* prf_messages_add_handler() */

/**************************************************************************/

void
prf_messages_del_handler(
    int type,
    int level,
    void (*func)( int, int, const char *, void * ),
    void * data )
{
    int i, count;
    assert( type > 0 && type < 6 );
    count = array_count( prf_handlers[type] );
    for ( i = 0; i < count; i++ ) {
        if ( (prf_handlers[type][i] != NULL) &&
             (prf_handlers[type][i]->func == func) &&
             (prf_handlers[type][i]->userdata == data) &&
             (prf_handlers[type][i]->level == level) ) {
            free( prf_handlers[type][i] );
            prf_handlers[type][i] = NULL;
        }
    }
    
} /* prf_messages_del_handler() */

/**************************************************************************/

/* $Id$ */

