/**************************************************************************
 *
 *  bsptree - the generalized alternative to quad-, and octtree datatypes.
 *
 **************************************************************************/

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

#ifndef _SIM_BSP_TREE_H_
#define _SIM_BSP_TREE_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <profit/basics.h>

/*
 * array arguments that are to be returned are of the type pointer to pointer
 * to growable arrays (the type found in array.h).
 */

/**************************************************************************/

void *  bsp_create( int dimensions );
/*
bool_t  bsp_configure( void * handle, ... );
*/

void    bsp_destroy( void * handle );

void    bsp_clear( void * handle, int approxpoints );

int     bsp_get_num_points( void * handle );
int     bsp_get_dimensions( void * handle );
void    bsp_get_boundaries( void * handle, float * boundaries );
float ** bsp_get_point_arrays( void * handle );
void ** bsp_get_user_data_array( void * handle );

void    bsp_get_point( void * handle, int idx, float * point );
void *  bsp_get_user_data( void * handle, int idx );
void    bsp_set_user_data( void * handle, int idx, void * user );

int     bsp_add_point( void * handle, float * pos );

int     bsp_remove_point( void * handle, float * pos );
void    bsp_remove_point_by_idx( void * handle, int idx );

int     bsp_find_point( void * handle, float * point );
void    bsp_find_points( void * handle, float * point, float radius,
            int ** idxarray );

int     bsp_find_closest( void * handle, float * point );
int     bsp_find_closest_points( void * handle, float * point,
            float radius, int ** pointarray );

/**************************************************************************/

#define BSP_CONF_MAX_DEPTH                   0x64707468  /* 'dpth' */
#define BSP_CONF_MAX_NODE_POINTS             0x706e7473  /* 'pnts' */
#define BSP_CONF_MEMORY_POOLING              0x706f6f6c  /* 'pool' */
#define BSP_CONF_SPLIT_POSITION_STRATEGY     0x73706f73  /* 'spos' */
#define BSP_CONF_SPLIT_DIMENSION_STRATEGY    0x7364696d  /* 'sdim' */

/**************************************************************************/

#ifdef __cplusplus
}; /* extern "C" */
#endif /* __cplusplus */

#endif /* ! _SIM_BSP_TREE_H_ */
