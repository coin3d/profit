/**************************************************************************\
 * 
 *  Copyright (C) 1998-1999 by Systems in Motion.  All rights reserved.
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

#ifndef PROFIT_TYPES_H
#define PROFIT_TYPES_H

#include <profit/basic_types.h>

#ifdef __cplusplus
extern "C" {
#endif /* __splusplus */

typedef  struct bfile_s              bfile_t;

typedef  struct prf_model_s          prf_model_t;
typedef  struct prf_state_s          prf_state_t;

typedef  struct prf_node_s           prf_node_t;
typedef  struct prf_nodeinfo_s       prf_nodeinfo_t;

typedef  struct prf_vertex_s         prf_vertex_t;
typedef  struct prf_material_s       prf_material_t;
typedef  struct prf_texture_s        prf_texture_t;

typedef  float32_t                   matrix4x4_f32_t[4][4];

#ifdef __cplusplus
}; /* extern "C" */
#endif /* __splusplus */

#endif /* ! PROFIT_TYPES_H */

/* $Id$ */

