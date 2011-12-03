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

#ifndef PRF_NODE_H
#define PRF_NODE_H

#include <profit/basics.h>
#include <profit/profit.h>
#include <profit/util/mempool.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

struct prf_node_s {
    uint16_t       opcode;
    uint16_t       length;
    uint32_t       flags;
    uint8_t *      data;
    prf_node_t **  children; /* see <profit/util/array.h> */
    prf_node_t *   parent;
    void *         userdata; /* too hook up application-specific data */
}; /* struct prf_node_s */

#define PRF_NODE_DELETED     0x00000001
#define PRF_NODE_TAGGED      0x00000002
#define PRF_NODE_MEMPOOLED   0x00000100

/* not in use yet: */
#define PRF_NODE_PERFORMED   0x00008000 /* references and replications */
#define PRF_NODE_VIRTUAL     0x00004000 /* added for references/replications */

PROFIT_API  prf_node_t *  prf_node_create( void );
PROFIT_API  prf_node_t *  prf_node_create_etc( prf_model_t * model,
                              uint16_t datasize );
PROFIT_API  void          prf_node_clear( prf_node_t * node );
PROFIT_API  void          prf_node_destroy( prf_node_t * node );
PROFIT_API  prf_node_t *  prf_node_clone( prf_node_t * node,
                              prf_model_t * source, prf_model_t * target );

PROFIT_API  bool_t        prf_node_is_header( prf_node_t * node );
PROFIT_API  bool_t        prf_node_is_face( prf_node_t * node );
PROFIT_API  bool_t        prf_node_is_transformation( prf_node_t * node );
PROFIT_API  bool_t        prf_node_is_vertex( prf_node_t * node );

#ifdef __cplusplus
}; /* extern "C" */
#endif /* __cplusplus */

#endif /* ! PRF_NODE_H */
