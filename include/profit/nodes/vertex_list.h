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

#ifndef PROFIT_VERTEX_LIST_H
#define PROFIT_VERTEX_LIST_H

#include <profit/types.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

struct prf_vertex_list_data {
    uint32_t   offset[1]; /* byte offset into vertex palette */
}; /* struct prf_vertex_list_data */

void        prf_vertex_list_init( void );
prf_node_t *prf_vertex_list_create(prf_model_t *model, int numvertices);
int         prf_vertex_list_get_count( prf_node_t * node );
uint32_t *  prf_vertex_list_get_list_ptr( prf_node_t * node );
void        prf_vertex_list_set_list( prf_node_t * node, int count,
                uint32_t * ptr );

#ifdef __cplusplus
}; /* extern "C" */
#endif /* __cplusplus */

#endif /* ! PROFIT_VERTEX_LIST_H */

/* $Id$ */

