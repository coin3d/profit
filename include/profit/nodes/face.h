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

#ifndef PRF_FACE_NODE_H
#define PRF_FACE_NODE_H

#include <profit/types.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

struct prf_face_data {
  char      ascii_id[ 8 ];                /* 12 */
  int32_t   ir_color_code;                /* 16 */
  int16_t   relative_priority;            /* 18 */
  int8_t    draw_type;                    /* 19 */
  int8_t    texture_white;                /* 20 */
  uint16_t  color_name_index;             /* 22 */
  uint16_t  alternate_color_name_index;   /* 24 */
  int8_t    reserved1;                    /* 25 */
  int8_t    billboard_flags;              /* 26 */
  int16_t   detail_texture_pattern_index; /* 28 */
  int16_t   texture_pattern_index;        /* 30 */
  int16_t   material_index;               /* 32 */
  int16_t   surface_material_code;        /* 34 */
  int16_t   feature_id;                   /* 36 */
  int32_t   ir_material_code;             /* 40 */
  uint16_t  transparency;                 /* 42 */
  uint8_t   lod_generation_control;       /* 43 */
  uint8_t   line_style_index;             /* 44 */
  uint32_t  flags;                        /* 48 */
  uint8_t   light_mode;                   /* 49 */
  uint8_t   reserved2;                    /* 50 */
  uint16_t  reserved3;                    /* 52 */
  uint32_t  reserved4;                    /* 56 */
  uint32_t  packed_color_primary;         /* 60 */
  uint32_t  packed_color_alternate;       /* 64 */
  int16_t   texture_mapping_index;
  int16_t   reserved5;
  uint32_t  primary_color_index;
  uint32_t  alternate_color_index;
  int16_t   reserved6;
  int16_t   reserved7;
}; /* struct prf_face_data */

void prf_face_init( void );
void prf_face_get_material( prf_node_t * node, prf_state_t * state,
         prf_material_t * material );

#define  PRF_FACE_DRAW_TYPE_SOLID                       0x00
#define  PRF_FACE_DRAW_TYPE_SOLID_NO_BFCULLING          0x01
#define  PRF_FACE_DRAW_TYPE_WIREFRAME                   0x02
#define  PRF_FACE_DRAW_TYPE_WIREFRAME_AND_CLOSE         0x03
#define  PRF_FACE_DRAW_TYPE_WIREFRAME_ALTERNATE         0x04
#define  PRF_FACE_DRAW_TYPE_OMNIDIRECTIONAL_LIGHT       0x08
#define  PRF_FACE_DRAW_TYPE_UNIDIRECTIONAL_LIGHT        0x09
#define  PRF_FACE_DRAW_TYPE_BIDIRECTIONAL_LIGHT         0x0a

#define  PRF_FACE_TEMPLATE_FIXED_NO_ALPHA               0x00
#define  PRF_FACE_TEMPLATE_FIXED_ALPHA                  0x01
#define  PRF_FACE_TEMPLATE_AXIAL_ROTATE                 0x02
#define  PRF_FACE_TEMPLATE_POINT_ROTATE                 0x04

#define  PRF_FACE_FLAGS_TERRAIN                         0x80000000
#define  PRF_FACE_FLAGS_NO_COLOR                        0x40000000
#define  PRF_FACE_FLAGS_NO_ALTERNATE_COLOR              0x20000000
#define  PRF_FACE_FLAGS_PACKED_COLOR                    0x10000000
#define  PRF_FACE_FLAGS_TERRAIN_CULTURE_CUTOUT          0x08000000
#define  PRF_FACE_FLAGS_HIDDEN                          0x04000000

#define  PRF_FACE_LIGHT_MODE_FACE_COLOR                 0x00
#define  PRF_FACE_LIGHT_MODE_VERTEX_COLORS              0x01
#define  PRF_FACE_LIGHT_MODE_FACE_COLOR_AND_NORMALS     0x02
#define  PRF_FACE_LIGHT_MODE_VERTEX_COLORS_AND_NORMALS  0x03

#ifdef __cplusplus
}; /* extern "C" */
#endif /* __cplusplus */

#endif /* ! PRF_FACE_NODE_H */
