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

#include <profit/util/array.h>
#include <config.h>
#include <stddef.h>
#include <stdlib.h>
#include <assert.h>

/* fixme */
#define profit_malloc malloc
#define profit_free free
#define profit_realloc realloc

/**************************************************************************/

typedef  struct _array  array;

struct _array
{
  unsigned int size;         /* number of allocated elements */
  unsigned int elemsize : 4; /* size of a element (1-15 bytes) */
  unsigned int numelem : 28; /* number of elements in array */
}; /* struct _array */

/**************************************************************************/

static void *
prf_array_to_id( array *ptr )
{
  return (void *)(((char*)ptr)+sizeof(array));
}

static array*
id_to_array( void *ptr )
{
  return (array*) (((char*)ptr)-sizeof(array));
} /* id_to_array() */

/**************************************************************************/

void *
prf_array_init(int initsize, int elemsize)
{
  array *arr;

  arr = profit_malloc(initsize*elemsize+sizeof(array));
  assert( arr != NULL );
  
  arr->size = initsize;
  arr->elemsize = elemsize;
  arr->numelem = 0;
  
  return prf_array_to_id(arr);
} /* prf_array_init() */

/**************************************************************************/

void 
prf_array_free(void *id)
{
  assert( id != NULL );
  assert( id_to_array(id) != NULL );
  profit_free(id_to_array(id));
} /* prf_array_free() */

int 
prf_array_count(void *id)
{
  return id_to_array(id)->numelem;
} /* prf_array_count() */

int 
prf_array_find_int(void *id, int elem)
{
  int i,n;
  int *ptr;

  n = id_to_array(id)->numelem;
  ptr = (int*)id;

  for (i = 0; i < n; i++) if (ptr[i] == elem) return i;
  return -1;
} /* prf_array_find_int() */

int 
prf_array_find_ptr(void *id, void *elem)
{
  int i,n;
  void **ptr;

  n = id_to_array(id)->numelem;
  ptr = (void**)id;

  for (i = 0; i < n; i++) if (ptr[i] == elem) return i;
  return -1;
} /* prf_array_find_ptr() */

void *
prf_array_append_ptr(void *id, void *elem)
{
  void **ptr;
  array *arr; 
  
  arr = id_to_array(id);
  if (arr->numelem >= arr->size) {
    id = prf_array_set_size(id, arr->size<<1);
    arr = id_to_array(id);
  }
  ptr = (void**)id;
  ptr[arr->numelem++] = elem;
  return id;
} /* prf_array_append_ptr() */

void *
prf_array_insert_ptr(void *id, int idx, void *elem)
{
  int i, n;
  void **ptr;

  n = id_to_array(id)->numelem;

  /* make room for one more */
  ptr = (void**)prf_array_append_ptr(id, elem);

  if (idx < n) {
    for (i = n; i > idx; i--) {
      ptr[i] = ptr[i-1]; 
    }
    ptr[idx] = elem;
  }
  return ptr;
} /* prf_array_insert_ptr() */

void *
prf_array_append_int(void *id, int elem)
{
  int *ptr;
  array *arr; 
  
  arr = id_to_array(id);
  if (arr->numelem >= arr->size) {
    id = prf_array_set_size(id, arr->size<<1);
    arr = id_to_array(id);
  }
  ptr = (int*)id;
  ptr[arr->numelem++] = elem;
  return id;
} /* prf_array_append_int() */

void *
prf_array_insert_int(void *id, int idx, int elem)
{
  int i, n;
  int *ptr;

  n = id_to_array(id)->numelem;

  /* make room for one more */
  ptr = (int*)prf_array_append_int(id, elem);

  if (idx < n) {
    for (i = n; i > idx; i--) {
      ptr[i] = ptr[i-1]; 
    }
    ptr[idx] = elem;
  }
  return ptr;
} /* prf_array_insert_int() */

void *
prf_array_remove(void *id, int index)
{
  int i, n;
  array *arr; 
  void **ptr;

  /* FIXME: use memmove instead */
  arr = id_to_array(id);
  n = arr->numelem-1;
  ptr = (void**)id;

  for (i = index; i < n; i++)
    ptr[i] = ptr[i+1];
 
  arr->numelem--;
  return id;
} /* prf_array_remove() */

void *
prf_array_remove_fast(void *id, int index)
{
  void **ptr;

  ptr = (void**)id;

  ptr[index] = ptr[--id_to_array(id)->numelem];
  return id;
} /* prf_array_remove_fast() */

void *
prf_array_set_count(void *id, int newcount)
{
  id_to_array(id)->numelem = newcount;
  return id;
} /* prf_array_set_count() */

void *
prf_array_set_size(void *id, int newsize)
{
  array *oldarray;
  array *newarray; 
  int elemsize;
  int numelem;

  oldarray = id_to_array(id);
  elemsize = oldarray->elemsize;
  numelem = oldarray->numelem;
  newarray = profit_realloc(oldarray, 
			    newsize*elemsize + sizeof(array));
  
  newarray->size = newsize;
  newarray->elemsize = elemsize;
  newarray->numelem = numelem;
  
  return prf_array_to_id(newarray);
} /* prf_array_set_size() */

void *
prf_array_fit(void *id)
{
  return prf_array_set_size(id, id_to_array(id)->numelem);
} /* prf_array_fit() */

#ifndef NDEBUG

int 
prf_array_alloc_count(void *id)
{
  return id_to_array(id)->size;
} /* prf_array_alloc_count() */

int 
prf_array_elemsize(void *id)
{
  return id_to_array(id)->elemsize;
} /* prf_array_elemsize() */

#endif /* ! NDEBUG */

/**************************************************************************/

/* $Id$ */

