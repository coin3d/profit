/**************************************************************************\
 * 
 *  Copyright (C) 1998-2001 by Systems in Motion.  All rights reserved.
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

#ifndef PRF_ARRAY_H
#define PRF_ARRAY_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

  void *prf_array_init(int initsize, int elemsize);
  void prf_array_free(void *id);

  int prf_array_count(void *id);
  
  int prf_array_find_int(void *id, int elem);
  int prf_array_find_ptr(void *id, void *elem); 

  void *prf_array_append_int(void *id, int elem);
  void *prf_array_append_int(void *id, int elem);
  void *prf_array_append_ptr(void *id, void *elem);
  void *prf_array_insert_ptr(void *id, int index, void *elem);
  void *prf_array_remove(void *id, int index);
  void *prf_array_remove_fast(void *id, int index);
  void *prf_array_set_count(void *id, int newcount);
  void *prf_array_set_size(void *id, int newsize);
  void *prf_array_fit(void *id);

#ifndef NDEBUG
  int prf_array_alloc_count(void *id);
  int prf_array_elemsize(void *id);
#endif /* ! NDEBUG */

#ifdef __cplusplus
}; /* extern "C" */
#endif /* __cplusplus */

#endif /* ! PRF_ARRAY_H */ 
