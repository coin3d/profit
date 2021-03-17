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

#include <profit/util/array.h>
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <stddef.h>
#include <stdlib.h>
#include <assert.h>
#ifdef HAVE_STRING_H
#include <string.h>
#endif

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
prf_array_to_id( array * ptr )
{
  return (void *)(((char *)ptr)+sizeof(array));
}

static array *
id_to_array( void * ptr )
{
  return (array *) (((char *)ptr)-sizeof(array));
} /* id_to_array() */

/**************************************************************************/

void *
prf_array_init(int initsize, int elemsize)
{
  array *arr;

  arr = (array *)profit_malloc((size_t)initsize*elemsize+sizeof(array));
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
  array * arr;
  assert( id != NULL && index >= 0 );
  arr = id_to_array(id);
  assert( index < arr->numelem && arr->numelem >= 0 );
  if ( (arr->numelem - 1) > index ) {
    void ** ptr = (void **) id;
    assert( ((arr->numelem - index - 1) * sizeof(void *)) > 0 );
    memmove( ((char *) ptr) + (index * sizeof(void *)),
             ((char *) ptr) + ((index + 1) * sizeof(void *)),
             (arr->numelem - index - 1) * sizeof(void *));
  }
#ifndef NDEBUG
  else {
    assert( index == arr->numelem );
  }
#endif /* ! NDEBUG */
  arr->numelem--;
  return id;
} /* prf_array_remove() */

void *
prf_array_remove_fast(void *id, int index)
{
  void ** ptr = (void **) id;
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
  newarray = (array *)profit_realloc(oldarray,
                                     (size_t)newsize*elemsize + sizeof(array));
  
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

int 
prf_array_alloc_count(
    void * id )
{
    return id_to_array(id)->size;
} /* prf_array_alloc_count() */

int 
prf_array_elemsize(
    void * id )
{
    return id_to_array(id)->elemsize;
} /* prf_array_elemsize() */

/**************************************************************************/
