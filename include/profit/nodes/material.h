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

#ifndef PRF_MATERIAL_NODE_H
#define PRF_MATERIAL_NODE_H

#include <profit/basics.h>
#include <profit/profit.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

struct prf_material_data {
    uint32_t    material_index;
    char        name[ 12 ];
    uint32_t    flags;
    float32_t   ambient_red;
    float32_t   ambient_green;
    float32_t   ambient_blue;
    float32_t   diffuse_red;
    float32_t   diffuse_green;
    float32_t   diffuse_blue;
    float32_t   specular_red;
    float32_t   specular_green;
    float32_t   specular_blue;
    float32_t   emissive_red;
    float32_t   emissive_green;
    float32_t   emissive_blue;
    float32_t   shininess;
    float32_t   alpha;
    uint32_t    spare;
}; /* struct prf_material_data */

PROFIT_API  prf_node_t * prf_material_node_create( prf_model_t * model,
                             int index );

#define  PRF_MATERIAL_FLAGS_MATERIALS_USED   0x80000000

#ifdef __cplusplus
}; /* extern "C" */
#endif /* __cplusplus */

#endif /* ! PRF_MATERIAL_NODE_H */
