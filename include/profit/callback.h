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

#ifndef PRF_CALLBACK_H
#define PRF_CALLBACK_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**************************************************************************
   cb_call() will call the callback with the system data as the first
   argument and the user data as the second argument.  The hierarchy
   traversal can be controlled to some degree by using various callback
   return values.
 **************************************************************************/

typedef  struct prf_cb_s  prf_cb_t;

struct prf_cb_s {
    int (*func)( void * sysdata, void * userdata );
    void * data;
};

#define  PRF_TRAV_EXIT           (0)
#define  PRF_TRAV_CONTINUE       (1)
#define  PRF_TRAV_SIBLING        (2)
/* trav_sibling only means something for in-order traversal,
   and causes the node's children to be skipped. */
#define  PRF_TRAV_POP            (3)
/* trav_pop means that nodes between this node and the next pop will
   not be traversed (but the pop will) - not implemented yet -
   can be used to avoid traversing extensions and attributes  */
#define  PRF_TRAV_UP             (4)
/* trav_up is used to skip the rest of the siblings to this node, and go
   back up to the node's parent.  */
#define  PRF_TRAV_TRAVERSE       (5)
/* trav_traverse is used to traverse into the internals of container
   nodes. */

#define prf_cb_func( cb, function )            \
    cb.func = function

#define prf_cb_data( cb, userdata )            \
    cb.data = userdata

#define prf_cb_set( cb, function, userdata )   \
    prf_cb_func( cb, function );               \
    prf_cb_data( cb, userdata )

#define prf_cb_call( cb, sysdata )             \
    (*cb.func)( sysdata, cb.data )

#ifdef __cplusplus
}; /* extern "C" */
#endif /* __cplusplus */

#endif /* ! PRF_CALLBACK_H */
