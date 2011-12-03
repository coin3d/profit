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

#ifndef PRF_STATE_H
#define PRF_STATE_H

#include <profit/basics.h>
#include <profit/profit.h>
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

PROFIT_API  prf_state_t *  prf_state_create( void );
PROFIT_API  void           prf_state_reset( prf_state_t * state );
PROFIT_API  prf_state_t *  prf_state_clone( prf_state_t * original );
PROFIT_API  void           prf_state_copy( prf_state_t * copy,
                               prf_state_t * original );
PROFIT_API  void           prf_state_destroy( prf_state_t * state );

PROFIT_API  void           prf_state_push( prf_state_t * state );
PROFIT_API  void           prf_state_pop( prf_state_t * state );

PROFIT_API  matrix4x4_f32_t *  prf_state_get_matrix( prf_state_t * state );
PROFIT_API  void               prf_state_matrix_mult_right(
                                   prf_state_t * state,
                                   matrix4x4_f32_t * matrix );
PROFIT_API  matrix4x4_f32_t *  prf_state_get_inverse_matrix(
                                   prf_state_t * state );

PROFIT_API  bool_t        prf_state_material_lookup( prf_state_t * state,
                              unsigned int index, prf_material_t * material );
PROFIT_API  bool_t        prf_state_texture_lookup( prf_state_t * state,
                              unsigned int index, prf_texture_t * texture );
PROFIT_API  prf_node_t *  prf_state_get_instance( prf_state_t * state, 
                              int16_t instance_definition_number );
PROFIT_API  prf_node_t *  prf_state_get_instance_from_node(
                              prf_state_t * state, prf_node_t * refnode);
PROFIT_API  uint32_t      prf_state_color_lookup( prf_state_t * state,
                              unsigned int index, int intensity );

#ifdef __cplusplus
}; /* extern "C" */
#endif /* __cplusplus */

#endif /* ! PRF_STATE_H */
