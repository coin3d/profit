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

#include <profit/state.h>
#include <profit/types.h>
#include <profit/node.h>
#include <profit/material.h>
#include <profit/texture.h>
#include <profit/debug.h>
#include <profit/nodes/material_node.h>
#include <profit/nodes/texture_node.h>
#include <profit/nodes/material_palette.h>
#include <profit/nodes/color_palette.h>
#include <profit/nodes/instance_definition.h>
#include <profit/nodes/instance_reference.h>
#include <profit/util/array.h>
#include <config.h>

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

/*
  NOTES:

  TODO:
    - implement childnum
    - do lazy deallocation on pop, and reuse un-deallocated memory on push
    - use pushlevel instead of matrix array counts
*/

/**************************************************************************/

prf_state_t *
prf_state_create(
    void )
{
    prf_state_t * state;
    state = malloc( sizeof( prf_state_t ) );
    if ( state != NULL ) {
        matrix4x4_f32_t * m;
        state->materials = prf_array_init( 4, sizeof( prf_node_t * ) );
        if ( state->materials == NULL ) {
            free( state );
            return NULL;
        }
	state->textures = prf_array_init( 4, sizeof( prf_node_t * ) );
	if ( state->textures == NULL ) {
	    prf_array_free( state->materials );
            free( state );
            return NULL;
	} 

	state->instances = prf_array_init( 4, sizeof( prf_node_t * ) );
	if ( state->instances == NULL ) {
	  prf_array_free( state->materials );
	  prf_array_free( state->textures );
	  free( state );
	}
        state->matrix = prf_array_init( 14, sizeof( matrix4x4_f32_t *) );
        if ( state->matrix == NULL ) {
            prf_array_free( state->materials );
            prf_array_free( state->textures );
            prf_array_free( state->instances );
            free( state );
            return NULL;
        }

        m = malloc( sizeof( matrix4x4_f32_t )  );
        if ( m == NULL ) {
            prf_array_free( state->materials );
            prf_array_free( state->textures );
            prf_array_free( state->instances );
            prf_array_free( state->matrix );
            free( state );
            return NULL;
        }
        state->matrix = prf_array_append_ptr( state->matrix, m );
        state->inv_matrix = malloc( sizeof( matrix4x4_f32_t ) );
        if ( state->inv_matrix == NULL ) {
            free( m );
            prf_array_free( state->materials );
            prf_array_free( state->textures );
            prf_array_free( state->instances );
            prf_array_free( state->matrix );
            free( state );
            return NULL;
        }
        prf_state_reset( state );
    }
    return state;
} /* prf_state_create() */

/**************************************************************************/

void
prf_state_reset(
    prf_state_t * state )
{
    matrix4x4_f32_t * m;
    state->model = NULL;
    state->node = NULL;

    state->header = 0;
    state->vertex_palette = NULL;
    state->color_palette = NULL;
    state->material_palette = NULL;
    state->object_transparency = 0;
    state->object_flags = 0;
    prf_array_set_count( state->materials, 0 );
    prf_array_set_count( state->textures, 0 );
    prf_array_set_count( state->instances, 0 );
    m = state->matrix[0];
    (*m)[0][0]=1.0f; (*m)[0][1]=0.0f; (*m)[0][2]=0.0f; (*m)[0][3]=0.0f;
    (*m)[1][0]=0.0f; (*m)[1][1]=1.0f; (*m)[1][2]=0.0f; (*m)[1][3]=0.0f;
    (*m)[2][0]=0.0f; (*m)[2][1]=0.0f; (*m)[2][2]=1.0f; (*m)[2][3]=0.0f;
    (*m)[3][0]=0.0f; (*m)[3][1]=0.0f; (*m)[3][2]=0.0f; (*m)[3][3]=1.0f;
    state->inv_dirty = TRUE;

    state->push_level = 0;
    state->subface_level = 0;
    state->attribute_level = 0;
    state->extension_level = 0;

    state->state_push_level = 0;
    state->physical_level = 0;
} /* prf_state_reset() */

/**************************************************************************/

void
prf_state_destroy(
    prf_state_t * state )
{
    int cnt, i;
    assert( state != NULL );
    cnt = prf_array_count( state->matrix );
    for ( i = 0; i < cnt; i++ )
        free( state->matrix[i] );
    free( state->inv_matrix );
    prf_array_free( state->materials );
    prf_array_free( state->textures );
    prf_array_free( state->instances );
    prf_array_free( state->matrix );
    free( state );
} /* prf_state_destroy() */

/**************************************************************************/

prf_state_t *
prf_state_clone(
    prf_state_t * original )
{
    prf_state_t * clone;
    int cnt, i;

    assert( original != NULL );

    clone = prf_state_create();
    if ( clone == NULL )
        return NULL;
    clone->model = original->model;
    clone->node = original->node;
    clone->header = original->header;
    clone->vertex_palette = original->vertex_palette;
    clone->object_transparency = original->object_transparency;
    clone->object_flags = original->object_flags;
    clone->color_palette = original->color_palette;
    clone->material_palette = original->material_palette;
    cnt = prf_array_count( original->materials );
    for ( i = 0; i < cnt; i++ )
        clone->materials =
            prf_array_append_ptr( clone->materials, original->materials[i] );
    cnt = prf_array_count( original->textures );
    for ( i = 0; i < cnt; i++ )
        clone->textures = 
            prf_array_append_ptr( clone->textures, original->textures[i] );

    cnt = prf_array_count( original->instances );
    for ( i = 0; i < cnt; i++ )
        clone->instances = 
            prf_array_append_ptr( clone->instances, original->instances[i] );

    for ( i = 0; i <= original->state_push_level; i++ ) {
        if ( i >= prf_array_count( clone->matrix ) ) {
            matrix4x4_f32_t * m;
            m = malloc( sizeof( matrix4x4_f32_t ) );
            if ( m == NULL ) {
                prf_state_destroy( clone );
                return NULL;
            }
            clone->matrix = prf_array_append_ptr( clone->matrix, m );
        }
        memcpy( clone->matrix[i], original->matrix[i],
            sizeof( matrix4x4_f32_t ) );
    }

    clone->inv_dirty = original->inv_dirty;
    if ( clone->inv_dirty == FALSE )
        memcpy( clone->inv_matrix, original->inv_matrix,
            sizeof( matrix4x4_f32_t ) );

    clone->push_level = original->push_level;
    clone->subface_level = original->subface_level;
    clone->extension_level = original->extension_level;
    clone->attribute_level = original->attribute_level;

    clone->state_push_level = original->state_push_level;
    clone->physical_level = original->physical_level;

    return clone;
} /* prf_state_clone() */

/**************************************************************************/

void
prf_state_copy(
    prf_state_t * copy,
    prf_state_t * original )
{
    int cnt, i;
    assert( copy != NULL && original != NULL );
    copy->model = original->model;
    copy->node = original->node;
    copy->object_transparency = original->object_transparency;
    copy->object_flags = original->object_flags;
    copy->header = original->header;
    copy->vertex_palette = original->vertex_palette;
    copy->color_palette = original->color_palette;
    copy->material_palette = original->material_palette;
    copy->inv_dirty = original->inv_dirty;

    cnt = prf_array_count( original->materials );
    copy->materials = prf_array_set_count( copy->materials, 0 );
    for ( i = 0; i < cnt; i++ ) {
        copy->materials =
            prf_array_append_ptr( copy->materials, original->materials[i] );
    }

    cnt = prf_array_count( original->textures );
    copy->textures = prf_array_set_count( copy->textures, 0 );
    for ( i = 0; i < cnt; i++ ) {
        copy->textures =
            prf_array_append_ptr( copy->textures, original->textures[i] );
    }

    cnt = prf_array_count( original->instances );
    copy->instances = prf_array_set_count( copy->instances, 0 );
    for ( i = 0; i < cnt; i++ ) {
        copy->instances =
            prf_array_append_ptr( copy->instances, original->instances[i] );
    }
    


    cnt = prf_array_count( original->matrix );
    
    for ( i = 0; i <= original->state_push_level; i++ ) {
        if ( i >= prf_array_count( copy->matrix ) ) {
            matrix4x4_f32_t * m;
            m = malloc( sizeof( matrix4x4_f32_t ) );
            if ( m == NULL ) {
                prf_error( 9,
                    "memory allocation failure (malloc returned NULL)" );
                return;
            }
            copy->matrix = prf_array_append_ptr( copy->matrix, m );
        }
        memcpy( copy->matrix[i], original->matrix[i],
            sizeof( matrix4x4_f32_t ) );
    }

    if ( copy->inv_dirty == FALSE )
        memcpy( copy->inv_matrix, original->inv_matrix,
            sizeof( matrix4x4_f32_t ) );

    copy->push_level = original->push_level;
    copy->subface_level = original->subface_level;
    copy->extension_level = original->extension_level;
    copy->attribute_level = original->attribute_level;

    copy->state_push_level = original->state_push_level;
    copy->physical_level = original->physical_level;

} /* prf_state_copy() */

/**************************************************************************/

void
prf_state_push(
    prf_state_t * state )
{
    assert( state != NULL );

    state->state_push_level++;
    prf_debug( 1, "state pushed to level %d", state->state_push_level );
    if ( prf_array_count( state->matrix ) <= state->state_push_level ) {
        matrix4x4_f32_t * m;
        m = malloc( sizeof( matrix4x4_f32_t ) );
        if ( m == NULL ) {
            prf_state_reset( state );
            return;
        }
        state->matrix = prf_array_append_ptr( state->matrix, m );
    }
    memcpy( state->matrix[ state->state_push_level ],
        state->matrix[ state->state_push_level - 1 ], sizeof( matrix4x4_f32_t ) );
} /* prf_state_push() */

/**************************************************************************/

void
prf_state_pop(
    prf_state_t * state )
{
    assert( state != NULL );

    if ( state->inv_dirty == FALSE) {
        if ( state->state_push_level > 1 ) {
            matrix4x4_f32_t * m1, * m2;
            m1 = state->matrix[state->state_push_level-1];
            m2 = state->matrix[state->state_push_level];
            if ( memcmp( m1, m2, sizeof( matrix4x4_f32_t ) ) != 0 )
                state->inv_dirty = TRUE;
        } else {
            state->inv_dirty = TRUE;
        }
    }
    state->state_push_level--;
    prf_debug( 1, "state popped to level %d", state->state_push_level );
} /* prf_state_pop() */

/**************************************************************************/

void
prf_state_matrix_mult_right(
    prf_state_t * state,
    matrix4x4_f32_t * m )
{
    matrix4x4_f32_t copy;
    matrix4x4_f32_t * matrix;
    int i, j, n;

    assert( state != NULL && m != NULL );

    matrix = state->matrix[ state->state_push_level ];
    memcpy( &copy, matrix, sizeof( matrix4x4_f32_t ) );

    /* right-multiplication of matrix */
    for ( i = 0; i < 4; i++ ) {
        for ( j = 0; j < 4; j++ ) {
            (*matrix)[i][j] = 0.0f;
            for ( n = 0; n < 4; n++ )
                (*matrix)[i][j] += copy[i][n] * (*m)[n][j];
        }
    }
    state->inv_dirty = TRUE;

} /* prf_state_set_matrix() */

/**************************************************************************/

matrix4x4_f32_t *
prf_state_get_matrix(
    prf_state_t * state )
{
    assert( state != NULL );
    return state->matrix[ state->state_push_level ];
} /* prf_state_get_matrix() */

/**************************************************************************/

void
prf_state_dump_matrix(
    prf_state_t * state )
{
    matrix4x4_f32_t * m;
    m = state->matrix[ state->state_push_level ];
    assert( FALSE ); /* FIXME: print matrix - larsa 290499 */
} /* state_dump_matrix() */

/**************************************************************************/

/* inline matrix inversion, taken from Graphics Gems */

matrix4x4_f32_t *
prf_state_get_inverse_matrix(
    prf_state_t * state )
{
    assert( state != NULL );

    if ( state->inv_dirty != FALSE ) {
        float32_t max, pivot, s, q, (*a)[4];
        int i, j, k, p[4];

        a = (float32_t (*)[4]) state->inv_matrix;
        memcpy( state->inv_matrix, state->matrix[ state->state_push_level ],
            sizeof( matrix4x4_f32_t ) ); 

        for ( k = 0; k < 4; k++ ) {
            max = 0.0f;
            p[k] = 0;
            for ( i = k; i < 4; i++ ) {
                s = 0.0;
                for ( j = k; j < 4; j++ )
                    s += fabs( a[i][j] );
                q = fabs( a[i][k] ) / s;
                if ( q > max ) {
                    max = q;
                    p[k] = i;
                }
            }
            if ( max == 0.0f )
                return NULL;
            if ( p[k] != k ) {
                for ( j = 0; j < 4; j++ ) {
                    float32_t h;
                    h = a[k][j];
                    a[k][j] = a[p[k]][j];
                    a[p[k]][j] = h;
                }
            }
            pivot = a[k][k];
            for ( j = 0; j < 4; j++ ) {
                if ( j != k ) {
                    a[k][j] = - a[k][j] / pivot;
                    for ( i = 0; i < 4; i++ )
                        if ( i != k )
                            a[i][j] += a[i][k] * a[k][j];
                }
            }
            for ( i = 0; i < 4; i++ )
                a[i][k] /= pivot;
            a[k][k] = 1 / pivot;
        }
        for ( k = 4 - 2; k >= 0; k-- ) {
            if ( p[k] != k ) {
                for ( i = 0; i < 4; i++ ) {
                    float32_t h;
                    h = a[i][k];
                    a[i][k] = a[i][p[k]];
                    a[i][p[k]] = h;
                }
            }
        }
        state->inv_dirty = FALSE;
    }

    return state->inv_matrix;
} /* prf_state_get_inverse_matrix() */
    
/**************************************************************************/

bool_t
prf_state_material_lookup( 
    prf_state_t * state,
    int index,
    prf_material_t * material )
{
    int count, i;
    assert( state != NULL && material != NULL );
    if ( state->material_palette != NULL ) { /* old-style material palette */
        struct prf_material_palette_data * original;
        original = (struct prf_material_palette_data *)
            state->material_palette->data;
        count = state->material_palette->length /
                sizeof( struct prf_material_palette_data );
        if ( index < 0 || index >= count )
            return FALSE;
        original += index;
        material->material_index = index;
        memcpy( material->name, original->name, 12 );
        material->flags = original->flags;
        material->ambient_red   = original->ambient_red;
        material->ambient_green = original->ambient_green;
        material->ambient_blue  = original->ambient_blue;
        material->diffuse_red   = original->diffuse_red;
        material->diffuse_green = original->diffuse_green;
        material->diffuse_blue  = original->diffuse_blue;
        material->specular_red   = original->specular_red;
        material->specular_green = original->specular_green;
        material->specular_blue  = original->specular_blue;
        material->emissive_red   = original->emissive_red;
        material->emissive_green = original->emissive_green;
        material->emissive_blue  = original->emissive_blue;
        material->shininess = original->shininess;
        material->alpha = original->alpha;
        return TRUE;
    } else { /* new material record node set */
        count = prf_array_count( state->materials );
        for ( i = 0; i < count; i++ ) {
            prf_node_t * node;
            prf_material_t * mat;
            node = state->materials[i];
            mat = (prf_material_t *) node->data;
            if ( mat->material_index == index ) {
             /* fprintf(stderr,"mat shininess: %f\n", mat->shininess); */
	        memcpy( material, mat, sizeof( prf_material_t ) );
	        return TRUE;
            }
        }
    }

    return FALSE;
} /* prf_state_material_lookup() */

bool_t
prf_state_texture_lookup(prf_state_t * state,
			 int index,
			 prf_texture_t * texture)
{
  int count, i;
  assert( state != NULL && texture != NULL );
  count = prf_array_count( state->textures );
  for (i = 0; i < count; i++) {
    prf_node_t *node;
    prf_texture_t *tex;
    node = state->textures[i];
    tex = (prf_texture_t *) node->data;
    if (tex->pattern_index == index) {
      memcpy(texture, tex, sizeof( prf_texture_t ) );
      return TRUE;
    }
  }
  return FALSE;
} /* prf_state_texture_lookup() */

prf_node_t *
prf_state_get_instance(prf_state_t * state, 
		       int16_t number)
{
  int i, n;

  n = prf_array_count(state->instances);
  for (i = 0; i < n; i++) {
    struct prf_instance_definition_data *data = 
      (struct prf_instance_definition_data*) state->instances[i]->data;
    if (data->instance_definition_number == number)
      return state->instances[i];
  }
  return NULL;
} 

prf_node_t *
prf_state_get_instance_from_node(prf_state_t *state,
				 prf_node_t *node)
{
  struct prf_instance_reference_data *data = 
    (struct prf_instance_reference_data*) node->data;

  return prf_state_get_instance(state, data->instance_definition_number);
}

/**************************************************************************/

static void
extract_components(uint32_t col, int *a, int *b, int *g, int *r)
{
    *r = col & 0xff;
    col >>= 8;
    *g = col & 0xff;
    col >>= 8;
    *b = col & 0xff;
    *a = col >> 8;
}

uint32_t
prf_state_color_lookup(
    prf_state_t * state,
    int index,
    int intensity )
{
    uint32_t color;

    assert( state != NULL );
    assert( index >= 0 && index < 1024 );

    color = 0;

    if ( /*(intensity != 0) &&*/
         (state->color_palette != NULL) &&
         (index < ((state->color_palette->length - 132) / 4)) ) {
        struct prf_color_palette_data * data;
        data = (struct prf_color_palette_data *) state->color_palette->data;
        color = data->brightest_rgb[index];
    }
    else {
      color = prf_color_palette_default_values[index];
    }

    if ((color & 0xff000000) != 0xff000000) {
      color = prf_color_palette_default_values[index];
    }

    /*
    if ( (intensity == 0) || (color == 0)) {
        color = prf_color_palette_default_values[index];
    }
    else if (0) {
      int b,g,r;
      int a0,a1,r0,r1,b0,b1,g0,g1;
      uint32_t defcol = prf_color_palette_default_values[index];
 
      extract_components(defcol, &a0, &b0, &g0, &r0);
      extract_components(color, &a1, &b1, &g1, &r1);

      a0 = 255 - a1;
      
      b = (b0 * a0 + b1 * a1) >> 8;
      g = (g0 * a0 + g1 * a1) >> 8;
      r = (r0 * a0 + r1 * a1) >> 8;
     
      color = 0xff000000 + (b<<16) + (g<<8) + r;
      }*/
    return color;
} /* prf_state_color_lookup() */

/**************************************************************************/

/* $Id$ */

