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

#ifndef PRF_HEADER_NODE_H
#define PRF_HEADER_NODE_H

#include <profit/basic_types.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* version IDs */
#define  PRF_FLIGHT_V11       11
#define  PRF_FLIGHT_V12       12
#define  PRF_OPENFLIGHT_V14   14
#define  PRF_OPENFLIGHT_V142  1420
#define  PRF_OPENFLIGHT_V151  1510
#define  PRF_OPENFLIGHT_V154  1540
#define  PRF_OPENFLIGHT_V156  1560

struct prf_header_data {
    char         id[ 8 ];
    int32_t      format_revision_level;
    int32_t      edit_revision_level;
    char         date_and_time[ 32 ];
    int16_t      next_group;
    int16_t      next_lod;
    int16_t      next_object;
    int16_t      next_face;
    int16_t      unit_multiplier_divisor;
    int8_t       vertex_coordinate_units;
    int8_t       texwhite;
    uint32_t     flags;
    int32_t      reserved1[ 6 ];
    int32_t      projection_type;
    int32_t      reserved2[ 7 ];
    int16_t      next_dof;
    int16_t      vertex_storage_type;
    int32_t      database_origin;
    float64_t    southwest_database_x;
    float64_t    southwest_database_y;
    float64_t    delta_x;
    float64_t    delta_y;
    int16_t      next_sound;
    int16_t      next_path;
    int32_t      reserved3[ 2 ];
    int16_t      next_clip;
    int16_t      next_text;
    int16_t      next_bsp;
    int16_t      next_switch;
    int32_t      reserved4;
    float64_t    southwest_corner_latitude;
    float64_t    southwest_corner_longitude;
    float64_t    northeast_corner_latitude;
    float64_t    northeast_corner_longitude;
    float64_t    origin_latitude;
    float64_t    origin_longitude;
    float64_t    lambert_upper_latitude;
    float64_t    lambert_lower_latitude;
    int16_t      next_light_source;
    int16_t      next_light_point;
    int16_t      next_road;
    int16_t      next_cat;
    int16_t      reserved5;
    int16_t      reserved6;
    int16_t      reserved7;
    int16_t      reserved8;
    int32_t      earth_ellipsoid_model;
    int16_t      next_adaptive;
    int16_t      next_curve;
    int16_t      reserved9;
}; /* struct prf_header_data */

void prf_header_init( void );

#ifdef __cplusplus
}; /* extern "C" */
#endif /* __cplusplus */

#endif /* ! PRF_HEADER_NODE_H */
