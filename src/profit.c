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

#include <profit/profit.h>
#include <profit/types.h>
#include <profit/nodeinfo.h>
#include <profit/messages.h>
#include <profit/util/mempool.h>
#include <config.h>

#include <stdio.h>
#include <assert.h>

/**************************************************************************/

static
void
profit_debug(
    int type,
    int level,
    const char * string,
    void * userdata )
{
    fprintf( stderr, "DEBUG (%d): %s\n", level, string );
} /* profit_debug() */

static
void
profit_warning(
    int type,
    int level,
    const char * string,
    void * userdata )
{
    fprintf( stderr, "WARNING (%d): %s\n", level, string );
} /* profit_warning() */

static
void
profit_error(
    int type,
    int level,
    const char * string,
    void * userdata )
{
    fprintf( stderr, "ERROR (%d): %s\n", level, string );
} /* profit_error() */

/**************************************************************************/

#ifndef NDEBUG

static
bool_t
prf_endianness_verification(
    void )
{
    union { int32_t i; char c[4]; } u;
    u.c[0] = 1; u.c[1] = 2; u.c[2] = 3; u.c[3] = 4;
#ifndef WORDS_BIGENDIAN
    if ( u.i != 0x04030201 ) return FALSE;
#else /* WORDS_BIGENDIAN */
    if ( u.i != 0x01020304 ) return FALSE;
#endif /* WORDS_BIGENDIAN */
    return TRUE;
} /* prf_endianness_verification() */

#endif /* !NDEBUG */

bool_t
prf_init(
    void )
{
    assert( prf_endianness_verification() && "invalid endianness setting" );
    prf_messages_init();
#ifndef NDEBUG
    /* configure these for level-of-verboseness during debugging */
    prf_messages_add_handler( PRF_MSG_DEBUG,  3, profit_debug, NULL );
    prf_messages_add_handler( PRF_MSG_WARNING, 3, profit_warning, NULL );
#endif /* ! NDEBUG */
    prf_messages_add_handler( PRF_MSG_ERROR, 1, profit_error, NULL );
    pool_system_init();
    prf_nodeinfo_init();
    return TRUE;
} /* prf_init() */

/**************************************************************************/

bool_t
prf_exit(
    void )
{
    prf_nodeinfo_exit();
    pool_system_exit();
    prf_messages_exit();
    return TRUE;
} /* prf_exit() */

/**************************************************************************/

bool_t
prf_openflight_check_file(
    const char * filename )
{
    FILE * fp;
    char magic[2];
    int tmp;
    if ( (fp = fopen( filename, "rb" )) == NULL ) return FALSE;
    tmp = fread( magic, 1, 2, fp );
    fclose( fp );
    if ( tmp != 2 || magic[0] != 0 || magic[1] != 1 ) return FALSE;
    return TRUE;
} /* prf_openflight_ckeck_file() */

/**************************************************************************/

/* $Id$ */

