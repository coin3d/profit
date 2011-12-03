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

#ifndef PRF_HEADER_NODE_H
#define PRF_HEADER_NODE_H

#include <profit/basics.h>
#include <profit/profit.h>

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

#ifdef __cplusplus
}; /* extern "C" */
#endif /* __cplusplus */

#endif /* ! PRF_HEADER_NODE_H */
