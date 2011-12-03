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

#ifndef PRF_NODEINFO_H
#define PRF_NODEINFO_H

#include <profit/basics.h>
#include <profit/profit.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

PROFIT_API  void              prf_nodeinfo_init( void );
PROFIT_API  void              prf_nodeinfo_exit( void );

PROFIT_API  prf_nodeinfo_t *  prf_nodeinfo_get( uint16_t opcode );
PROFIT_API  bool_t            prf_nodeinfo_set(
                                  const prf_nodeinfo_t * nodeinfo );

struct prf_nodeinfo_s {
    uint16_t opcode;
    uint16_t flags;
    char * name;
    bool_t        (*load_f)(     prf_node_t * node, prf_state_t * state,
                                     bfile_t * bfile );
    bool_t        (*save_f)(     prf_node_t * node, prf_state_t * state,
                                     bfile_t * bfile );
    void          (*entry_f)(    prf_node_t * node, prf_state_t * state );
    void          (*exit_f)(     prf_node_t * node, prf_state_t * state );
    int           (*traverse_f)( prf_node_t * node, prf_state_t * state );
    void          (*destroy_f)(  prf_node_t * node, prf_model_t * model );
    prf_node_t *  (*clone_f)(    prf_node_t * node, prf_model_t * model,
                                     prf_model_t * dest );
}; /* struct prf_nodeinfo_s */

/* flags */
#define PRF_PUSH_NODE    0x0001
#define PRF_POP_NODE     0x0002
#define PRF_VERTEX       0x0010
#define PRF_TRANSFORM    0x0020
#define PRF_DEFINITION   0x0040
#define PRF_REFERENCE    0x0080
#define PRF_PRIMARY      0x0100
#define PRF_ANCILLARY    0x0200
#define PRF_CONTROL      0x0400
#define PRF_CONTINUATION 0x0800
#define PRF_RESERVED     0x4000
#define PRF_OBSOLETE     0x8000

#ifdef __cplusplus
}; /* extern "C" */
#endif /* __cplusplus */

#endif /* ! PRF_NODEINFO_H */
