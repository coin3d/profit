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

#ifndef PRF_NODEINFO_H
#define PRF_NODEINFO_H

#include <profit/types.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

void              prf_nodeinfo_init( void );
void              prf_nodeinfo_exit( void );

prf_nodeinfo_t *  prf_nodeinfo_get( uint16_t opcode );
bool_t            prf_nodeinfo_set( const prf_nodeinfo_t * nodeinfo );

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
#define PRF_RESERVED     0x4000
#define PRF_OBSOLETE     0x8000

#ifdef __cplusplus
}; /* extern "C" */
#endif /* __cplusplus */

#endif /* ! PRF_NODEINFO_H */
