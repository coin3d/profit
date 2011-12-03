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
        (prf_msg_handler_t **)prf_array_init( 4, sizeof( prf_msg_handler_t *));
    prf_handlers[ PRF_MSG_DEBUG ] =
        (prf_msg_handler_t **)prf_array_init( 4, sizeof( prf_msg_handler_t *));
    prf_handlers[ PRF_MSG_WARNING ] =
        (prf_msg_handler_t **)prf_array_init( 4, sizeof( prf_msg_handler_t *));
    prf_handlers[ PRF_MSG_ERROR ] =
        (prf_msg_handler_t **)prf_array_init( 4, sizeof( prf_msg_handler_t *));
    prf_handlers[ PRF_MSG_FATAL_ERROR ] =
        (prf_msg_handler_t **)prf_array_init( 4, sizeof( prf_msg_handler_t *));
} /* messages_init() */

/**************************************************************************/

void
prf_messages_exit(
    void )
{
    int i, count;

    count = prf_array_count( prf_handlers[ PRF_MSG_INFO ] );
    for ( i = 0; i < count; i++ ) {
        if ( prf_handlers[ PRF_MSG_INFO ][ i ] != NULL ) {
            free( prf_handlers[ PRF_MSG_INFO ][ i ] );
        }
    }
    prf_array_free( prf_handlers[ PRF_MSG_INFO ] );

    count = prf_array_count( prf_handlers[ PRF_MSG_DEBUG ] );
    for ( i = 0; i < count; i++ ) {
        if ( prf_handlers[ PRF_MSG_DEBUG ][ i ] != NULL ) {
            free( prf_handlers[ PRF_MSG_DEBUG ][ i ] );
        }
    }
    prf_array_free( prf_handlers[ PRF_MSG_DEBUG ] );

    count = prf_array_count( prf_handlers[ PRF_MSG_WARNING ] );
    for ( i = 0; i < count; i++ ) {
        if ( prf_handlers[ PRF_MSG_WARNING ][ i ] != NULL ) {
            free( prf_handlers[ PRF_MSG_WARNING ][ i ] );
        }
    }
    prf_array_free( prf_handlers[ PRF_MSG_WARNING ] );

    count = prf_array_count( prf_handlers[ PRF_MSG_ERROR ] );
    for ( i = 0; i < count; i++ ) {
        if ( prf_handlers[ PRF_MSG_ERROR ][ i ] != NULL ) {
            free( prf_handlers[ PRF_MSG_ERROR ][ i ] );
        }
    }
    prf_array_free( prf_handlers[ PRF_MSG_ERROR ] );

    count = prf_array_count( prf_handlers[ PRF_MSG_FATAL_ERROR ] );
    for ( i = 0; i < count; i++ ) {
        if ( prf_handlers[ PRF_MSG_FATAL_ERROR ][ i ] != NULL ) {
            free( prf_handlers[ PRF_MSG_FATAL_ERROR ][ i ] );
        }
    }
    prf_array_free( prf_handlers[ PRF_MSG_FATAL_ERROR ] );
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
    count = prf_array_count( prf_handlers[type] );
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
    count = prf_array_count( prf_handlers[ type ] );
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
    int i = 0, count;
    assert( type > 0 && type < 6 );
    handler = (prf_msg_handler_t *)malloc( sizeof( prf_msg_handler_t ) );
    assert( handler != NULL );
    handler->level = level;
    handler->func = func;
    handler->userdata = data;
    count = prf_array_count( prf_handlers[type] );
    for ( i = 0; i < count; i++ ) {
        if ( prf_handlers[type][i] == NULL ) {
            prf_handlers[type][i] = handler;
            return;
        }
    }
    prf_handlers[type] = 
      (prf_msg_handler_t **)prf_array_append_ptr( prf_handlers[type], handler);
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
    count = prf_array_count( prf_handlers[type] );
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
