/**************************************************************************\
 * 
 *  $Id$
 *
 *  not just an array - a growable array
 *
 *  This file is free software and comes without any warranty.
 *  You can redistribute it and / or modify it under the terms of the
 *  GNU General Public License, version 2, as published by the
 *  Free Software Foundation (see file named COPYING for details).
 *
 *  Copyright (C) 1999 by Systems in Motion.  All rights reserved.
 *
\**************************************************************************/

#ifndef _SIM_ARRAY_H_
#define _SIM_ARRAY_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*
 *  DISCLAIMER
 *
 *  This module is a hack and uses a lot of dirty tricks for your convenience.
 *  Use this module at your own risk.
 *
 *  Be aware of the following:
 *  - lots of the methods are #define macros which evaluate the array
 *    argument multiple times (don't use incrementors in those arguments).
 *  - the array handle might change during use so don't keep duplicates
 *    around - use only one active handle for each array
 *  - lots of the prototyped methods in this header is only eye-candy
 *    designed to guide the user.  the methods might not exist for real -
 *    pre-processor macros might override them further down and direct the
 *    calls to the "real" methods.
 *
 */

/**************************************************************************/

void *  array_create( int initsize, int elemsize );
void    array_destroy( void * array );

int     array_count( void * array );

int *   array_create_int( int initsize );
int     array_find_int( int * array, int item );
void    array_append_int( int * array, int item );
void    array_insert_int( int * array, int idx, int item );
void    array_remove_int( int * array, int idx );
void    array_remove_int_fast( int * array, int idx );

void ** array_create_ptr( int initsize );
int     array_find_ptr( void ** array, void * item );
void    array_append_ptr( void ** array, void * item );
void    array_insert_ptr( void ** array, int idx, void * item );
void    array_remove_ptr( void ** array, int idx );
void    array_remove_ptr_fast( void ** array, int idx );

float * array_create_float( int initsize );
int     array_find_float( float * array, float item );
void    array_append_float( float * array, float item );
void    array_insert_float( float * array, int idx, float item );
void    array_remove_float( float * array, int idx );
void    array_remove_float_fast( float * array, int idx );

void    array_set_count( void * array, int count );
void    array_set_size( void * array, int newsize );
void    array_fit( void * array );

#ifndef NDEBUG
int     array_alloc_count( void * array );
int     array_elemsize( void * array );
#endif /* ! NDEBUG */

/**************************************************************************/
/* STOP HERE - the below mess will only confuse you :) */

#define ARRAY_REAL_PROTOS( name, type )                                    \
int     _array_find_##name( type * array, type item );                     \
type *  _array_append_##name( type * array, type item );                   \
type *  _array_insert_##name( type * array, int idx, type item );          \
type *  _array_remove_##name( type * array, int idx );                     \
type *  _array_remove_##name##_fast( type * array, int idx );

ARRAY_REAL_PROTOS( int, int )
ARRAY_REAL_PROTOS( ptr, void * )
ARRAY_REAL_PROTOS( float, float )

/* the magic (or just the hack?) begins here */
#define array_append_int( ARRAY, ITEM )                                    \
    ARRAY = _array_append_int( ARRAY, ITEM )
#define array_insert_int( ARRAY, IDX, ITEM )                               \
    ARRAY = _array_insert_int( ARRAY, IDX, ITEM )
#define array_remove_int( ARRAY, IDX )                                     \
    ARRAY = _array_remove_int( ARRAY, IDX )
#define array_remove_int_fast( ARRAY, IDX )                                \
    ARRAY = _array_remove_int_fast( ARRAY, IDX )

#define array_append_ptr( ARRAY, ITEM )                                    \
    ARRAY = _array_append_ptr( ARRAY, ITEM )
#define array_insert_ptr( ARRAY, IDX, ITEM )                               \
    ARRAY = _array_insert_ptr( ARRAY, IDX, ITEM )
#define array_remove_ptr( ARRAY, IDX )                                     \
    ARRAY = _array_remove_ptr( ARRAY, IDX )
#define array_remove_ptr_fast( ARRAY, IDX )                                \
    ARRAY = _array_remove_ptr_fast( ARRAY, IDX )

#define array_append_float( ARRAY, ITEM )                                  \
    ARRAY = _array_append_float( ARRAY, ITEM )
#define array_insert_float( ARRAY, IDX, ITEM )                             \
    ARRAY = _array_insert_float( ARRAY, IDX, ITEM )
#define array_remove_float( ARRAY, IDX )                                   \
    ARRAY = _array_remove_float( ARRAY, IDX )
#define array_remove_float_fast( ARRAY, IDX )                              \
    ARRAY = _array_remove_float_fast( ARRAY, IDX )

void *  _array_set_count( void * array, int newcount );
#define array_set_count( ARRAY, CNT )                                      \
    ARRAY = _array_set_count( ARRAY, CNT )

void *  _array_set_size( void * array, int newsize );
#define array_set_size( ARRAY, SIZE )                                      \
    ARRAY = _array_set_size( ARRAY, SIZE )

void *  _array_fit( void * array );
#define array_fit( ARRAY )                                                 \
    ARRAY = _array_fit( ARRAY )

/**************************************************************************/

#ifdef __cplusplus
}; /* extern "C" */
#endif /* __cplusplus */

#endif /* ! _SIM_ARRAY_H_ */ 
