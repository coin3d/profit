/**************************************************************************\
 * 
 *  $Id$
 *
 *  This file is free software and comes without any warranty.
 *  You can redistribute it and / or modify it under the terms of the
 *  GNU General Public License, version 2, as published by the
 *  Free Software Foundation (see file named COPYING for details).
 *
 *  Copyright (C) 1998-1999 by Systems in Motion.  All rights reserved.
 *
\**************************************************************************/

static const char * const rcsid =
    "$Id$";

#include <config.h>
#include <stddef.h>
#include <stdlib.h>
#include <assert.h>

#include "array.h"

/**************************************************************************/

typedef  struct array_s  array_t;

struct array_s {
  unsigned int size;         /* number of allocated elements */
  unsigned int elemsize : 4; /* size of a element (1-15 bytes) */
  unsigned int numelem : 28; /* number of elements in array */
}; /* struct array_s */

/**************************************************************************/

#define _HANDLE( array ) \
    ((array_t *) (((char *) array) - sizeof(array_t)))

#define _ARRAY( handle ) \
    ((void *) (((char *) handle) + sizeof(array_t)))

/**************************************************************************/

#define ARRAY_CREATE( name, type )                                         \
type *                                                                     \
array_create_##name(                                                       \
    int initsize )                                                         \
{                                                                          \
    return (type *) array_create( initsize, sizeof( type ) );              \
} /* array_create_##name() */

#define ARRAY_APPEND( name, type )                                         \
type *                                                                     \
_array_append_##name(                                                      \
    type * array,                                                          \
    type item)                                                             \
{                                                                          \
    array_t * handle;                                                      \
    handle = _HANDLE(array);                                               \
    if ( handle->numelem >= handle->size ) {                               \
        array_set_size( array, handle->size << 1 );                        \
        handle = _HANDLE(array);                                           \
    }                                                                      \
    array[handle->numelem] = item;                                         \
    handle->numelem++;                                                     \
    return array;                                                          \
} /* array_append_##name() */

#define ARRAY_INSERT( name, type )                                         \
type *                                                                     \
_array_insert_##name(                                                      \
    type * array,                                                          \
    int idx,                                                               \
    type item )                                                            \
{                                                                          \
    array_t * handle;                                                      \
    array_append_##name( array, item );                                    \
    handle = _HANDLE(array);                                               \
    assert( idx >= 0 && idx < handle->numelem );                           \
    if ( idx < handle->numelem - 1 )                                       \
        memmove( &array[idx+1], &array[idx],                               \
                 (handle->numelem-1-idx) * handle->elemsize );             \
    array[idx] = item;                                                     \
    return array;                                                          \
} /* array_insert_##name() */

#define ARRAY_REMOVE( name, type )                                         \
type *                                                                     \
_array_remove_##name(                                                      \
    type * array,                                                          \
    int idx )                                                              \
{                                                                          \
    array_t * handle;                                                      \
    handle = _HANDLE(array);                                               \
    if ( idx < handle->numelem-1 )                                         \
        memmove( &array[idx], &array[idx+1],                               \
                 (handle->numelem-1-idx) * handle->elemsize );             \
    handle->numelem--;                                                     \
    return array;                                                          \
} /* array_remove_##name() */                                              \
                                                                           \
type *                                                                     \
_array_remove_##name##_fast(                                               \
    type * array,                                                          \
    int idx )                                                              \
{                                                                          \
    array_t * handle;                                                      \
    handle = _HANDLE(array);                                               \
    handle->numelem -= 1;                                                  \
    array[idx] = array[handle->numelem];                                   \
    return array;                                                          \
} /* array_remove_fast() */

#define ARRAY_FIND( name, type )                                           \
int                                                                        \
array_find_##name(                                                         \
    type * array,                                                          \
    type item )                                                            \
{                                                                          \
    int i, n;                                                              \
    n = _HANDLE(array)->numelem;                                           \
    for ( i = 0; i < n; i++ )                                              \
        if ( array[i] == item ) return i;                                  \
    return -1;                                                             \
} /* array_find_##name() */

#define ARRAY_IMPLEMENT( name, type )                                      \
ARRAY_CREATE( name, type )                                                 \
ARRAY_FIND( name, type )                                                   \
ARRAY_APPEND( name, type )                                                 \
ARRAY_INSERT( name, type )                                                 \
ARRAY_REMOVE( name, type )

/**************************************************************************/

void *
array_create(
    int initsize,
    int elemsize )
{
    array_t * handle;
    handle = malloc( sizeof(array_t) + elemsize * initsize );
    assert( handle != NULL );
    handle->size = initsize;
    handle->elemsize = elemsize;
    handle->numelem = 0;
    return _ARRAY(handle);
} /* array_create() */

void 
array_destroy(
    void * array )
{
    assert( array != NULL && _HANDLE(array) != NULL );
    free( _HANDLE(array) );
} /* array_destroy() */

/**************************************************************************/

ARRAY_IMPLEMENT( int, int )
ARRAY_IMPLEMENT( ptr, void * )
ARRAY_IMPLEMENT( float, float )

/**************************************************************************/

int 
array_count(
    void * array )
{
    return _HANDLE(array)->numelem;
} /* array_count() */

void *
_array_set_count(
    void * array,
    int newcount )
{
    _HANDLE(array)->numelem = newcount;
    return array;
} /* array_set_count() */

void *
_array_set_size(
    void * array,
    int newsize )
{
    array_t * oldhandle, * newhandle; 
    int elemsize, numelem;
    oldhandle = _HANDLE(array);
    newhandle = realloc( oldhandle,
                         sizeof(array_t) + newsize * oldhandle->elemsize );
    newhandle->size = newsize;
    return _ARRAY(newhandle);
} /* _array_set_size() */

void *
_array_fit(
    void * array )
{
    return _array_set_size( array, _HANDLE(array)->numelem );
} /* _array_fit() */

/**************************************************************************/

#ifndef NDEBUG

int 
array_alloc_count(
    void * array )
{
    return _HANDLE(array)->size;
} /* array_alloc_count() */

int 
array_elemsize(
    void * array )
{
    return _HANDLE(array)->elemsize;
} /* array_elemsize() */

#endif /* ! NDEBUG */

/**************************************************************************/
