/**************************************************************************
 *
 *  $Id$
 *
 *  bsptree - the generalized alternative to quad-, and octtree datatypes.
 *
 *  This file is free software and comes without any warranty.
 *  You can redistribute it and / or modify it under the terms of the
 *  GNU General Public License, version 2, as published by the
 *  Free Software Foundation (see file named COPYING for details).
 *
 *  Copyright (C) 1999 by Systems in Motion.  All rights reserved.
 *
 **************************************************************************/

#ifndef _SIM_BSP_TREE_H_
#define _SIM_BSP_TREE_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <profit/basic_types.h>

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
