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

#ifndef PRF_NODE_H
#define PRF_NODE_H

#include <profit/types.h>
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

prf_node_t *  prf_node_create( void );
prf_node_t *  prf_node_create_etc( prf_model_t *model, uint16_t datasize );
void          prf_node_clear( prf_node_t * node );
void          prf_node_destroy( prf_node_t * node );
prf_node_t *  prf_node_clone( prf_node_t * node, prf_model_t * source,
              prf_model_t * target );

bool_t        prf_node_is_header( prf_node_t * node );
bool_t        prf_node_is_face( prf_node_t * node );
bool_t        prf_node_is_transformation( prf_node_t * node );
bool_t        prf_node_is_vertex( prf_node_t * node );

#ifdef __cplusplus
}; /* extern "C" */
#endif /* __cplusplus */

#endif /* ! PRF_NODE_H */
