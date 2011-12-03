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

#include <profit/nodes/pop_subface.h>
#include <profit/basics.h>
#include <profit/state.h>
#include <profit/nodeinfo.h>
#include <profit/debug.h>
#include <profit/node.h>
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <assert.h>

/**************************************************************************/

static prf_nodeinfo_t prf_pop_subface_info = {
    20, PRF_CONTROL | PRF_POP_NODE,
    "Pop Subface",
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL
}; /* struct prf_pop_subface_info */

/**************************************************************************/

static
void
prf_pop_subface_exit_f(
    prf_node_t * node,
    prf_state_t * state )
{
    assert( node != NULL && state != NULL );
    if ( node->opcode != prf_pop_subface_info.opcode ) {
        prf_error( 9, "tried pop subface exit state method on node of type %d.",
            node->opcode );
        return;
    }
    state->subface_level--;
} /* prf_pop_subface_exit_f() */

/**************************************************************************/

void
prf_pop_subface_init(
    void )
{
    prf_pop_subface_info.exit_f = prf_pop_subface_exit_f;
    prf_nodeinfo_set( &prf_pop_subface_info );
} /* prf_pop_subface_init() */

/**************************************************************************/
