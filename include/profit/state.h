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

#ifndef PRF_STATE_H
#define PRF_STATE_H

#include <profit/types.h>
#include <profit/callback.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

struct prf_state_s {
    prf_model_t *       model;

    prf_node_t *        node;

    prf_node_t *        header;
    prf_node_t *        vertex_palette;
    prf_node_t *        color_palette;
    prf_node_t *        material_palette;
    prf_node_t **       materials;
    prf_node_t **       textures;
    prf_node_t **       instances;
    matrix4x4_f32_t **  matrix;
    matrix4x4_f32_t *   inv_matrix;
    bool_t              inv_dirty;

    int                 traversal_method;
    prf_cb_t            callback;

    uint32_t            object_flags;
    uint16_t            object_transparency;

/* private */
    uint16_t            push_level;
    uint16_t            subface_level;
    uint16_t            attribute_level;
    uint16_t            extension_level;

    uint16_t            state_push_level;
    uint16_t            physical_level;
}; /* struct prf_state_s */

#define  PRF_TRAV_IN_ORDER         (1)
#define  PRF_TRAV_DEPTH_FIRST      (2)
#define  PRF_TRAV_BREADTH_FIRST    (3)

prf_state_t *      prf_state_create( void );
void               prf_state_reset( prf_state_t * state );
prf_state_t *      prf_state_clone( prf_state_t * original );
void               prf_state_copy( prf_state_t * copy, prf_state_t * original );
void               prf_state_destroy( prf_state_t * state );

void               prf_state_push( prf_state_t * state );
void               prf_state_pop( prf_state_t * state );

matrix4x4_f32_t *  prf_state_get_matrix( prf_state_t * state );
void               prf_state_matrix_mult_right( prf_state_t * state,
                       matrix4x4_f32_t * matrix );
matrix4x4_f32_t *  prf_state_get_inverse_matrix( prf_state_t * state );

bool_t             prf_state_material_lookup( prf_state_t * state,
                       unsigned int index, prf_material_t * material );
bool_t             prf_state_texture_lookup( prf_state_t * state,
                       unsigned int index, prf_texture_t * texture );
prf_node_t *       prf_state_get_instance(
                       prf_state_t * state, 
                       int16_t instance_definition_number );
prf_node_t *       prf_state_get_instance_from_node(
                       prf_state_t * state,
                       prf_node_t * refnode);
uint32_t           prf_state_color_lookup( prf_state_t * state,
                       unsigned int index, int intensity );

#ifdef __cplusplus
}; /* extern "C" */
#endif /* __cplusplus */

#endif /* ! PRF_STATE_H */
