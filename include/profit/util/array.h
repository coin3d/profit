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

#ifndef PRF_ARRAY_H
#define PRF_ARRAY_H

#include <profit/basics.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

PROFIT_API  void * prf_array_init( int initsize, int elemsize );
PROFIT_API  void prf_array_free( void * id );

PROFIT_API  int prf_array_count( void * id );
  
PROFIT_API  int prf_array_find_int( void * id, int elem );
PROFIT_API  int prf_array_find_ptr( void * id, void * elem ); 

PROFIT_API  void * prf_array_append_int( void * id, int elem );
PROFIT_API  void * prf_array_append_int( void * id, int elem );
PROFIT_API  void * prf_array_append_ptr( void * id, void * elem );
PROFIT_API  void * prf_array_insert_ptr( void * id, int index, void * elem );
PROFIT_API  void * prf_array_remove( void * id, int index );
PROFIT_API  void * prf_array_remove_fast( void * id, int index );
PROFIT_API  void * prf_array_set_count( void * id, int newcount );
PROFIT_API  void * prf_array_set_size( void * id, int newsize );
PROFIT_API  void * prf_array_fit( void * id );

PROFIT_API  int prf_array_alloc_count( void * id );
PROFIT_API  int prf_array_elemsize( void * id );

#ifdef __cplusplus
}; /* extern "C" */
#endif /* __cplusplus */

#endif /* ! PRF_ARRAY_H */ 
