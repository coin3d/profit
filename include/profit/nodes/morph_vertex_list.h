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

#ifndef PRF_MORPH_VERTEX_LIST_NODE_H
#define PRF_MORPH_VERTEX_LIST_NODE_H

#include <profit/basics.h>
#include <profit/profit.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

struct prf_morph_vertex_list_data {
    struct {
        int32_t byte_offset_0;
        int32_t byte_offset_100;
    } morph_vertex[1];
}; /* struct prf_morph_vertex_list_data */

PROFIT_API  int prf_morph_vertex_list_get_count( prf_node_t * node );
PROFIT_API  uint32_t * prf_morph_vertex_list_get_list_ptr( prf_node_t * node );

#ifdef __cplusplus
}; /* extern "C" */
#endif /* __cplusplus */

#endif /* ! PRF_MORPH_VERTEX_LIST_NODE_H */
