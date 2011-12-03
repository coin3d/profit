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

#ifndef PRF_BFILE_H
#define PRF_BFILE_H

#include <profit/basics.h>
#include <profit/profit.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

PROFIT_API  bfile_t *     bf_create_r( const char * filename );
PROFIT_API  bfile_t *     bf_create_w( const char * filename );
PROFIT_API  bfile_t *     bf_create_m( const uint8_t * buffer, unsigned int len );
PROFIT_API  void          bf_destroy( bfile_t * bfile );

PROFIT_API  void          bf_set_progress_cb( bfile_t * bfile,
                              int (*func)( float, void * ), void *);

PROFIT_API  int           bf_read( bfile_t * bfile, uint8_t * buffer,
                              unsigned int len );
PROFIT_API  int           bf_write( bfile_t * bfile, uint8_t * buffer,
                              unsigned int len );
PROFIT_API  int           bf_rewind( bfile_t * bfile, unsigned int num_bytes );

PROFIT_API  int8_t        bf_get_int8( bfile_t * bfile );
PROFIT_API  uint8_t       bf_get_uint8( bfile_t * bfile );
PROFIT_API  int16_t       bf_get_int16_be( bfile_t * bfile );
PROFIT_API  uint16_t      bf_get_uint16_be( bfile_t * bfile );
PROFIT_API  int16_t       bf_get_int16_le( bfile_t * bfile );
PROFIT_API  uint16_t      bf_get_uint16_le( bfile_t * bfile );
PROFIT_API  int32_t       bf_get_int32_be( bfile_t * bfile );
PROFIT_API  uint32_t      bf_get_uint32_be( bfile_t * bfile );
PROFIT_API  int32_t       bf_get_int32_le( bfile_t * bfile );
PROFIT_API  uint32_t      bf_get_uint32_le( bfile_t * bfile );
PROFIT_API  float32_t     bf_get_float32_be( bfile_t * bfile );
PROFIT_API  float32_t     bf_get_float32_le( bfile_t * bfile );
PROFIT_API  float64_t     bf_get_float64_be( bfile_t * bfile );
PROFIT_API  float64_t     bf_get_float64_le( bfile_t * bfile );

PROFIT_API  void          bf_unget_uint32_be( bfile_t * bfile, uint32_t data );

/*
PROFIT_API  int8_t        bf_peek_int8( bfile_t * bfile );
PROFIT_API  uint8_t       bf_peek_uint8( bfile_t * bfile );
*/
PROFIT_API  int16_t       bf_peek_int16_be( bfile_t * bfile );
PROFIT_API  uint16_t      bf_peek_uint16_be( bfile_t * bfile );
PROFIT_API  int16_t       bf_peek_int16_le( bfile_t * bfile );
PROFIT_API  uint16_t      bf_peek_uint16_le( bfile_t * bfile );
/*
PROFIT_API  int32_t       bf_peek_int32_be( bfile_t * bfile );
PROFIT_API  uint32_t      bf_peek_uint32_be( bfile_t * bfile );
PROFIT_API  int32_t       bf_peek_int32_le( bfile_t * bfile );
PROFIT_API  uint32_t      bf_peek_uint32_le( bfile_t * bfile );
*/

PROFIT_API  bool_t        bf_put_int8( bfile_t * bfile, int8_t value );
PROFIT_API  bool_t        bf_put_uint8( bfile_t * bfile, uint8_t value );
PROFIT_API  bool_t        bf_put_int16_be( bfile_t * bfile, int16_t value );
PROFIT_API  bool_t        bf_put_uint16_be( bfile_t * bfile, uint16_t value );
PROFIT_API  bool_t        bf_put_int16_le( bfile_t * bfile, int16_t value );
PROFIT_API  bool_t        bf_put_uint16_le( bfile_t * bfile, uint16_t value );
PROFIT_API  bool_t        bf_put_int32_be( bfile_t * bfile, int32_t value );
PROFIT_API  bool_t        bf_put_uint32_be( bfile_t * bfile, uint32_t value );
PROFIT_API  bool_t        bf_put_int32_le( bfile_t * bfile, int32_t value );
PROFIT_API  bool_t        bf_put_uint32_le( bfile_t * bfile, uint32_t value );
PROFIT_API  bool_t        bf_put_float32_be( bfile_t * bfile, float32_t value );
PROFIT_API  bool_t        bf_put_float32_le( bfile_t * bfile, float32_t value );
PROFIT_API  bool_t        bf_put_float64_be( bfile_t * bfile, float64_t value );
PROFIT_API  bool_t        bf_put_float64_le( bfile_t * bfile, float64_t value );

PROFIT_API  bool_t        bf_is_readable( bfile_t * bfile );
PROFIT_API  bool_t        bf_is_writable( bfile_t * bfile );

PROFIT_API  bool_t        bf_at_eof( bfile_t * bfile );

PROFIT_API  const char *  bf_get_filename( bfile_t * bfile );
PROFIT_API  size_t        bf_get_position( bfile_t * bfile );
PROFIT_API  size_t        bf_get_length( bfile_t * bfile );
PROFIT_API  size_t        bf_get_remaining_length( bfile_t * bfile );

PROFIT_API  int           bf_hex_dump( bfile_t * bfile, FILE * file,
                              unsigned int num_bytes, unsigned int unit_size );

#ifdef __cplusplus
}; /* extern "C" */
#endif /* __cplusplus */

#endif /* ! PRF_BFILE_H */
