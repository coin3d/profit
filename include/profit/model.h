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

#ifndef PRF_MODEL_H
#define PRF_MODEL_H

#include <profit/basics.h>
#include <profit/profit.h>
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

PROFIT_API  prf_model_t * prf_model_create( void );
PROFIT_API  void          prf_model_clear( prf_model_t * model );
PROFIT_API  void          prf_model_destroy( prf_model_t * model );
PROFIT_API  prf_model_t * prf_model_clone( prf_model_t * model,
                                           bool_t mempool );

PROFIT_API  void    prf_model_poolmem( prf_model_t * model );

PROFIT_API  bool_t  prf_model_load( prf_model_t * model, bfile_t * bfile );
PROFIT_API  bool_t  prf_model_load_with_callback( prf_model_t * model,
                        bfile_t * bfile, prf_cb_t callback );
PROFIT_API  void    prf_model_dump( prf_model_t * model );
PROFIT_API  bool_t  prf_model_save( prf_model_t * model, bfile_t * bfile );
PROFIT_API  bool_t  prf_model_save_with_callback( prf_model_t * model,
                        bfile_t * bfile, prf_cb_t callback );

PROFIT_API  bool_t  prf_model_macros_fix( prf_model_t * model,
                        prf_cb_t callback );
PROFIT_API  bool_t  prf_model_vertex_palette_optimize( prf_model_t * model );

PROFIT_API  int     prf_model_traverse(prf_model_t * model,
                        prf_cb_t callback);

PROFIT_API  int     prf_model_traverse_io(prf_model_t * model,
                        prf_cb_t callback);
PROFIT_API  int     prf_model_traverse_df(prf_model_t * model,
                        prf_cb_t callback);

PROFIT_API  int     prf_model_traverse_io_(prf_node_t * root,
                        prf_state_t * state);

PROFIT_API  uint32_t  prf_model_add_vertex( prf_model_t * model,
                          prf_vertex_t * vertex );
PROFIT_API  bool_t    prf_model_vertex_palette_lookup( prf_model_t * model,
                          uint32_t offset, prf_vertex_t * vertex );
PROFIT_API  uint32_t  prf_model_vertex_palette_get_first_index(
                          prf_model_t * model );
PROFIT_API  uint32_t  prf_model_vertex_palette_get_next_index(
                          prf_model_t * model, uint32_t idx );

PROFIT_API  bool_t    prf_model_append_node( prf_model_t * model,
                          prf_node_t * node );

/**************************************************************************/

#ifdef __cplusplus
}; /* extern "C" */
#endif /* __cplusplus */

#endif /* ! PRF_MODEL_H */
