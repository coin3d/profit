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

#ifndef BFILE_H
#define BFILE_H

#include <profit/types.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

bfile_t *     bf_create_r( const char * filename );
bfile_t *     bf_create_w( const char * filename );
bfile_t *     bf_create_m( const uint8_t *buffer, int len );
void          bf_destroy( bfile_t * bfile );

void          bf_set_progress_cb( bfile_t * bfile,
                  int (*func)( float, void * ), void *);

int           bf_read( bfile_t * bfile, uint8_t * buffer, int len );
int           bf_write( bfile_t * bfile, uint8_t * buffer, int len );
int           bf_rewind( bfile_t * bfile, uint32_t num_bytes );

int8_t        bf_get_int8( bfile_t * bfile );
uint8_t       bf_get_uint8( bfile_t * bfile );
int16_t       bf_get_int16_be( bfile_t * bfile );
uint16_t      bf_get_uint16_be( bfile_t * bfile );
int16_t       bf_get_int16_le( bfile_t * bfile );
uint16_t      bf_get_uint16_le( bfile_t * bfile );
int32_t       bf_get_int32_be( bfile_t * bfile );
uint32_t      bf_get_uint32_be( bfile_t * bfile );
int32_t       bf_get_int32_le( bfile_t * bfile );
uint32_t      bf_get_uint32_le( bfile_t * bfile );
float32_t     bf_get_float32_be( bfile_t * bfile );
float32_t     bf_get_float32_le( bfile_t * bfile );
float64_t     bf_get_float64_be( bfile_t * bfile );
float64_t     bf_get_float64_le( bfile_t * bfile );

void          bf_unget_uint32_be( bfile_t * bfile, uint32_t data );

/*
int8_t        bf_peek_int8( bfile_t * bfile );
uint8_t       bf_peek_uint8( bfile_t * bfile );
*/
int16_t       bf_peek_int16_be( bfile_t * bfile );
uint16_t      bf_peek_uint16_be( bfile_t * bfile );
int16_t       bf_peek_int16_le( bfile_t * bfile );
uint16_t      bf_peek_uint16_le( bfile_t * bfile );
/*
int32_t       bf_peek_int32_be( bfile_t * bfile );
uint32_t      bf_peek_uint32_be( bfile_t * bfile );
int32_t       bf_peek_int32_le( bfile_t * bfile );
uint32_t      bf_peek_uint32_le( bfile_t * bfile );
*/

bool_t        bf_put_int8( bfile_t * bfile, int8_t value );
bool_t        bf_put_uint8( bfile_t * bfile, uint8_t value );
bool_t        bf_put_int16_be( bfile_t * bfile, int16_t value );
bool_t        bf_put_uint16_be( bfile_t * bfile, uint16_t value );
bool_t        bf_put_int16_le( bfile_t * bfile, int16_t value );
bool_t        bf_put_uint16_le( bfile_t * bfile, uint16_t value );
bool_t        bf_put_int32_be( bfile_t * bfile, int32_t value );
bool_t        bf_put_uint32_be( bfile_t * bfile, uint32_t value );
bool_t        bf_put_int32_le( bfile_t * bfile, int32_t value );
bool_t        bf_put_uint32_le( bfile_t * bfile, uint32_t value );
bool_t        bf_put_float32_be( bfile_t * bfile, float32_t value );
bool_t        bf_put_float32_le( bfile_t * bfile, float32_t value );
bool_t        bf_put_float64_be( bfile_t * bfile, float64_t value );
bool_t        bf_put_float64_le( bfile_t * bfile, float64_t value );

bool_t        bf_is_readable( bfile_t * bfile );
bool_t        bf_is_writable( bfile_t * bfile );

bool_t        bf_at_eof( bfile_t * bfile );

const char *  bf_get_filename( bfile_t * bfile );
size_t        bf_get_position( bfile_t * bfile );
size_t        bf_get_length( bfile_t * bfile );
size_t        bf_get_remaining_length( bfile_t * bfile );

int           bf_hex_dump( bfile_t * bfile, FILE * file, uint32_t num_bytes,
                  int unit_size );

#ifdef __cplusplus
}; /* extern "C" */
#endif /* __cplusplus */

#endif /* ! BFILE_H */

/* $Id$ */
