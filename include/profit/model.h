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

#ifndef PROFIT_MODEL_H
#define PROFIT_MODEL_H

#include <profit/types.h>

#include <profit/callback.h>
#include <profit/util/mempool.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**************************************************************************/

struct prf_model_s {
    prf_node_t *                 header;
    pool_t                       mempool_id;
    prf_node_t *                 vertex_palette;
    struct prf_vertex_pool_s **  vertextras;
    uint16_t                     ofversion;
}; /* struct prf_model_s */

/**************************************************************************/

prf_model_t * prf_model_create( void );
void          prf_model_clear( prf_model_t * model );
void          prf_model_destroy( prf_model_t * model );
prf_model_t * prf_model_clone( prf_model_t * model, bool_t mempool );

void          prf_model_poolmem( prf_model_t * model );

bool_t        prf_model_load( prf_model_t * model, bfile_t * bfile );
bool_t        prf_model_load_with_callback( prf_model_t * model,
                  bfile_t * bfile, prf_cb_t callback );
void          prf_model_dump( prf_model_t * model );
bool_t        prf_model_save( prf_model_t * model, bfile_t * bfile );
bool_t        prf_model_save_with_callback( prf_model_t * model,
                  bfile_t * bfile, prf_cb_t callback );

bool_t        prf_model_macros_fix( prf_model_t * model, prf_cb_t callback );
bool_t        prf_model_vertex_palette_optimize( prf_model_t * model );

int           prf_model_traverse_io( prf_model_t * model, prf_cb_t callback );
int           prf_model_traverse_df( prf_model_t * model, prf_cb_t callback );
int           prf_model_traverse_bf( prf_model_t * model, prf_cb_t callback );
int           prf_model_traverse_io_(prf_node_t * root, prf_state_t * state);

uint32_t      prf_model_add_vertex( prf_model_t * model,
                  prf_vertex_t * vertex );
bool_t        prf_model_vertex_palette_lookup( prf_model_t * model,
                  uint32_t offset, prf_vertex_t * vertex );
uint32_t      prf_model_vertex_palette_get_first_index( prf_model_t * model );
uint32_t      prf_model_vertex_palette_get_next_index( prf_model_t * model,
                  uint32_t idx );

bool_t        prf_model_append_node( prf_model_t * model, prf_node_t * node );

/**************************************************************************/

#ifdef __cplusplus
}; /* extern "C" */
#endif /* __cplusplus */

#endif /* ! PROFIT_MODEL_H */

/* $Id$ */

