/**************************************************************************
 *
 *  bsptree.c
 *
 *  this bsptree implementation is based on orthogonal sub-division.
 *  the number of dimensions are set dynamically upon creation.
 *  all dimensions have float precision.
 *
 *  a bsptree can also be used where you would normally use hashing.  just
 *  set up one dimension for each hash function you think you need to get
 *  as unique "locations" as possible.  insert and lookup will both have
 *  O(lg(n)) complexity.
 *
 **************************************************************************
 *
 *  This file is free software and comes without any warranty.
 *  You can redistribute it and / or modify it under the terms of the
 *  GNU General Public License, version 2, as published by the
 *  Free Software Foundation (see file named COPYING for details).
 *
 *  Copyright (C) 1999 by Systems in Motion.  All rights reserved.
 *
 **************************************************************************/

static const char * const rcsid =
    "$Id$";

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <stdarg.h>
#include <assert.h>
#include <float.h>

#include "array.h"
#include "bsptree.h"

/*
 * TODO
 * * bps_configure():
 *   - implement optional mempooling of sub-node structures
 *   - implement split strategy selection
 *   - implement numnodes configuration
 *   - implement maxdepth configuration
 */

/**************************************************************************/

typedef  struct _bsp_tree  bsp_tree;
typedef  struct _bsp_node  bsp_node;

struct _bsp_tree {
    float **       pointarrays;
    uint8_t        dimensions;
    uint8_t        maxdepth;
    uint16_t       maxnodepoints;
    uint16_t       flags;
    uint16_t       mempoolid;

    bsp_node *     top;
}; /* struct _bsp_tree */

struct _bsp_node {
    bsp_tree *     root;
    int *          idxarray;

    int16_t        splitdim;
    float          splitpos;
    bsp_node *     ge;
    bsp_node *     lt;
}; /* struct _bsp_node */

/**************************************************************************/

/* internal methods */

bsp_node * _bsp_create_node( bsp_tree * root );
void       _bsp_destroy_node( bsp_node * node );

int        _bsp_add_point( bsp_node * node, float * point, int dimensions,
               int maxnodepoints, int invdepth );
void       _bsp_find_split( int dimensions, float ** pointarrays,
               int * idxarray, int * splitdim, float * splitpos );
int        _bsp_remove_point( bsp_node * node, float * point, int dimensions );
int        _bsp_find_point( bsp_node * node, float * point, int dimensions );
void       _bsp_add_points( bsp_node * node, float * point, int dimensions,
               float radius, int ** idxarray );
int        _bsp_find_close( bsp_node * node, float * point, int dimensions );

/**************************************************************************/

void *
bsp_create(
    int dimensions )
{
    bsp_tree * tree;
    int i;

    assert( dimensions > 0 && dimensions < 128 );

    tree = malloc( sizeof( bsp_tree ) );
    assert( tree != NULL );

    tree->dimensions = dimensions;
    tree->flags = 0;
    tree->maxdepth = 64;
    tree->maxnodepoints = 64;
    tree->mempoolid = 0;

    tree->pointarrays = malloc( sizeof(float *) * (dimensions + 1) );
    assert( tree->pointarrays != NULL );

    for ( i = 0; i < dimensions; i++ ) {
        tree->pointarrays[i] = array_create_float( 32 );
        assert( tree->pointarrays[i] != NULL );
    }
    tree->pointarrays[tree->dimensions] = (float *) array_create_ptr( 32 );
    assert( tree->pointarrays[tree->dimensions] != NULL );

    tree->top = _bsp_create_node( tree );
    return (void *) tree;
} /* bsp_create() */

bsp_node *
_bsp_create_node(
    bsp_tree * root )
{
    bsp_node * node;
    node = (bsp_node *) malloc( sizeof(bsp_node) );
    assert( node != NULL );
    node->root = root;
    node->idxarray = array_create_int( 4 );
    assert( node->idxarray != NULL );
    node->splitdim = -1;
    node->splitpos = 0;
    node->ge = NULL;
    node->lt = NULL;
    return node;
} /* bsp_create_node() */

/*
 * to be used for setting up maxdepth, maxpointspernode, node splitting
 * strategy and similar things.
 */

#if 0
bool_t
bsp_configure(
    void * handle,
    ... )
{
    bsp_tree * tree;
    tree = (bsp_tree *) handle;
    assert( tree != NULL );

    assert( 0 && "not implemented yet" );

    return TRUE;
} /* bsp_configure() */
#endif /* 0 */

void
bsp_destroy(
    void * handle )
{
    bsp_tree * tree;
    int i;

    tree = (bsp_tree *) handle;
    assert( tree != NULL );
    for ( i = 0; i <= tree->dimensions; i++ )
        array_destroy( tree->pointarrays[i] );
    free( tree->pointarrays );

    if ( tree->top != NULL )
        _bsp_destroy_node( tree->top );
    free( tree );
} /* bsp_destroy() */

void
_bsp_destroy_node(
    bsp_node * node )
{
    assert( node != NULL );
    if ( node->ge != NULL ) _bsp_destroy_node( node->ge );
    if ( node->lt != NULL ) _bsp_destroy_node( node->lt );
    if ( node->idxarray != NULL ) array_destroy( node->idxarray );
    free( node );
} /* _bsp_destroy_node() */

void
bsp_clear(
    void * handle,
    int approxpoints )
{
    bsp_tree * tree;
    int i;

    tree = (bsp_tree *) handle;
    assert( tree != NULL );

    for ( i = 0; i < tree->dimensions; i++ ) {
        array_destroy( tree->pointarrays[i] );
        tree->pointarrays[i] = array_create_float( approxpoints );
        assert( tree->pointarrays[i] != NULL );
    }
    tree->pointarrays[tree->dimensions] =
        (float *) array_create_ptr( approxpoints );
    assert( tree->pointarrays[i] != NULL );

    if ( tree->top != NULL ) _bsp_destroy_node( tree->top );
} /* bsp_clear() */

/**************************************************************************/

int
bsp_get_num_points(
    void * handle )
{
    bsp_tree * tree;
    tree = (bsp_tree *) handle;
    assert( tree != NULL );
    return array_count( tree->pointarrays[0] );
} /* bsp_get_num_points() */

int
bsp_get_dimensions(
    void * handle )
{
    bsp_tree * tree;
    tree = (bsp_tree *) handle;
    assert( tree != NULL );
    return tree->dimensions;
} /* bsp_get_dimensions() */

void
bsp_get_boundaries(
    void * handle,
    float * boundaries )
{
    bsp_tree * tree;
    int i, j, cnt;
    tree = (bsp_tree *) handle;
    assert( tree != NULL );
    assert( tree->pointarrays != NULL );
    assert( tree->pointarrays[0] != NULL );
    if ( (cnt = array_count( tree->pointarrays[0] )) > 0 ) {
        for ( j = 0; j < tree->dimensions; j++ ) {
            boundaries[j*2] = tree->pointarrays[j][0];
            boundaries[j*2+1] = tree->pointarrays[j][0];
        }
        for ( j = 0; j < tree->dimensions; j++ ) {
            int min, max;
            min = j*2;
            max = j*2+1;
            for ( i = 1; i < cnt; i++ ) {
                /* MIN() */
                if ( boundaries[min] > tree->pointarrays[j][i] )
                    boundaries[min] = tree->pointarrays[j][i];
                /* MAX() */
                else if ( boundaries[max] < tree->pointarrays[j][i] )
                    boundaries[max] = tree->pointarrays[j][i];
            }
        }
    }
} /* bsp_get_boundaries() */

float **
bsp_get_point_arrays(
    void * handle )
{
    bsp_tree * tree;
    tree = (bsp_tree *) handle;
    assert( tree != NULL );
    return tree->pointarrays;
} /* bsp_get_point_arrays() */

void **
bsp_get_user_data_array(
    void * handle )
{
    bsp_tree * tree;
    tree = (bsp_tree *) handle;
    assert( tree != NULL );
    return (void **) tree->pointarrays[tree->dimensions];
} /* bsp_get_user_data_array() */

/**************************************************************************/

void
bsp_get_point(
    void * handle,
    int idx,
    float * point )
{
    bsp_tree * tree;
    int i;

    tree = (bsp_tree *) handle;
    assert( tree != NULL && point != NULL );

    assert( idx >= 0 && idx < array_count(tree->pointarrays[0]) );

    for ( i = 0; i < tree->dimensions; i++ )
        point[i] = tree->pointarrays[i][idx];
} /* bsp_get_point() */

void *
bsp_get_user_data(
    void * handle,
    int idx )
{
    bsp_tree * tree;
    tree = (bsp_tree *) handle;
    assert( tree != NULL );
    assert( idx >= 0 && idx < array_count( tree->pointarrays[0] ) );
    return ((void **)(tree->pointarrays[tree->dimensions]))[idx];
} /* bsp_get_user_data() */

void
bsp_set_user_data(
    void * handle,
    int idx,
    void * userdata )
{
    bsp_tree * tree;
    tree = (bsp_tree *) handle;
    assert( tree != NULL );
    assert( idx >= 0 && idx < array_count(tree->pointarrays[0]) );
    ((void **)(tree->pointarrays[tree->dimensions]))[idx] = userdata;
} /* bsp_set_user_data() */

int
bsp_add_point(
    void * handle,
    float * point )
{
    bsp_tree * tree;

    tree = (bsp_tree *) handle;
    assert( tree != NULL );

    return _bsp_add_point( tree->top, point, tree->dimensions,
                           tree->maxnodepoints, tree->maxdepth );
} /* bsp_add_point() */

/* the recursive traverse */
int
_bsp_add_point(
    bsp_node * node,
    float * point,
    int dimensions,
    int maxnodepoints,
    int invdepth )
{
    int cnt, i, j;
    float ** pointarrays;

    assert( invdepth != 0 );

    if ( node->idxarray == NULL ) {
        /* node has been split */
        bsp_node * the_node;
        if ( point[node->splitdim] >= node->splitpos )
            return _bsp_add_point( node->ge, point, dimensions, maxnodepoints,
                invdepth - 1 );
        else
            return _bsp_add_point( node->lt, point, dimensions, maxnodepoints,
                invdepth - 1 );
    }

    cnt = array_count( node->idxarray );
    pointarrays = node->root->pointarrays;

    for ( i = 0; i < cnt; i++ ) {
        for ( j = 0; j < dimensions; j++ ) {
            if ( pointarrays[j][node->idxarray[i]] != point[j] )
                break;
        }
	if (j == dimensions) break;
    }
    if ( i < cnt ) {
        /* found point */
        return node->idxarray[i];
    } else if ( cnt != maxnodepoints ) {
        /* didn't find point - node got space for point */
        int idx = array_count( pointarrays[0] );
        array_append_int( node->idxarray, idx );
        for ( i = 0; i < dimensions; i++ )
            array_append_float( pointarrays[i], point[i] );
        pointarrays[dimensions] =
        (float *) _array_append_ptr( (void **) pointarrays[dimensions], NULL );
        return idx;
    }

    /* didn't find point - and node must be split */
    /*
    fprintf( stderr, "splitting node - level = %d\n", 64 - invdepth );
    */
    do {
        int dim, i, cnt;
        float pos;
        float ** pointarrays;

        pointarrays = node->root->pointarrays;
        _bsp_find_split( dimensions, pointarrays, node->idxarray,
            &dim, &pos );

        node->ge = _bsp_create_node( node->root );
        assert( node->ge != NULL );
        node->lt = _bsp_create_node( node->root );
        assert( node->lt != NULL );

        cnt = array_count( node->idxarray );
        for ( i = 0; i < cnt; i++ ) {
            if ( pointarrays[dim][node->idxarray[i]] >= pos )
                array_append_int( node->ge->idxarray, node->idxarray[i] );
            else
                array_append_int( node->lt->idxarray, node->idxarray[i] );
        }
        array_destroy( node->idxarray );
        node->idxarray = NULL;
        node->splitdim = dim;
        node->splitpos = pos;

        return _bsp_add_point(node, point, dimensions, maxnodepoints, invdepth);
    } while ( FALSE );
} /* _bsp_add_point() */

void
_bsp_find_split(
    int dimensions,
    float ** pointarrays,
    int * idxarray,
    int * splitdim,
    float * splitpos )
{
    int d, i, cnt, maxdim;
    float * stats;
    float maxfact;

    stats = malloc( sizeof(float) * 5 * dimensions );
    assert( stats != NULL );

    cnt = array_count( idxarray );
    for ( d = 0; d < dimensions; d++ ) {
        float mean, variance, deviation, min, max;
        float pos, diff;
        mean = min = max = pointarrays[d][idxarray[0]];
        for ( i = 1; i < cnt; i++ ) {
            pos = pointarrays[d][idxarray[i]];
            mean += pos;
            if ( pos < min ) min = pos;
            else if ( pos > max ) max = pos;
        }
        mean = mean / (float) cnt;
        variance = 0;
        for ( i = 0; i < cnt; i++ ) {
            diff = pointarrays[d][idxarray[i]] - mean;
            variance += diff * diff;
        }
        deviation = sqrt( variance );

        stats[d*5] = mean;
        stats[d*5+1] = variance;
        stats[d*5+2] = deviation;
        stats[d*5+3] = min;
        stats[d*5+4] = max;
    }

    
    maxdim = 0;
    maxfact = 0.0;
    for ( d = 0; d < dimensions; d++ ) {
        float range, factor;
        range = stats[d*5+4] - stats[d*5+3];
        factor = stats[d*5+2] / range;
        if ( factor > maxfact ) {
            maxdim = d;
            maxfact = factor;
        }
    }

    *splitdim = maxdim;
    *splitpos = stats[maxdim*5]; /* geometric mean */

#if 0
* code for analyzing BSP splits */
    do {
        static int filenum = 0;
        char filename[80];
        FILE * file;
        int i;
        float coord[3], size, minrange;

        sprintf( filename, "split-%03d.wrl", ++filenum );
        file = fopen( filename, "w" );
        if ( ! file ) {
            perror( "fopen()" );
            return;
        }

        fprintf( file, "#VRML V1.0 ascii\n" );
        fprintf( file, "\n" );
        fprintf( file, "Separator {\n" );
        fprintf( file, "  Coordinate3 {\n" );
        fprintf( file, "    point [\n" );

        minrange = stats[4] - stats[3];
        for ( i = 0; i < dimensions; i++ ) {
            coord[i] = stats[maxdim*5]; /* set everything to geometric mean */
            if ( (stats[i*5+4] - stats[i*5+3]) < minrange )
                minrange = stats[i*5+4] - stats[i*5+3];
        }
        size = minrange / 50.0f;

        /* mean & min, min */
        for ( i = 0; i < dimensions; i++ )
            if ( i != maxdim )
                coord[i] = stats[maxdim*5 + 3];
        fprintf( file, "      %f %f %f,\n", coord[0], coord[1], coord[2] );

        /* mean & max, min */
        if ( maxdim == 0 ) coord[1] = stats[maxdim*5 + 4];
        else               coord[0] = stats[maxdim*5 + 4];
        fprintf( file, "      %f %f %f,\n", coord[0], coord[1], coord[2] );

        /* mean & max, max */
        for ( i = 0; i < dimensions; i++ )
            if ( i != maxdim )
                coord[i] = stats[maxdim*5 + 4];
        fprintf( file, "      %f %f %f,\n", coord[0], coord[1], coord[2] );

        /* mean & min, max */
        if ( maxdim == 0 ) coord[1] = stats[maxdim*5 + 3];
        else               coord[0] = stats[maxdim*5 + 3];
        fprintf( file, "      %f %f %f ]\n", coord[0], coord[1], coord[2] );

        fprintf( file, "  }\n" );
        fprintf( file, "  Material {\n" );
        fprintf( file, "    diffuseColor 0.8 0.3 0.3\n" );
        fprintf( file, "    transparency 0.5\n" );
        fprintf( file, "  }\n" );

        fprintf( file, "  IndexedFaceSet {\n" );
        fprintf( file, "    coordIndex [\n" );
        fprintf( file, "      0, 1, 2, 3, -1\n" );
        fprintf( file, "    ]\n" );
        fprintf( file, "  }\n" );

        for ( i = 0; i < array_count( idxarray ); i++ ) {
            float x, y, z;
            x = pointarrays[0][idxarray[i]];
            y = pointarrays[1][idxarray[i]];
            z = pointarrays[2][idxarray[i]];

            fprintf( file, "  Separator { # point %d\n", i + 1 );
            fprintf( file, "    Translation {\n" );
            fprintf( file, "      translation %f %f %f\n", x, y, z );
            fprintf( file, "    }\n" );
            fprintf( file, "    Cube {\n" );
            fprintf( file, "      width %f\n", size );
            fprintf( file, "      height %f\n", size );
            fprintf( file, "      depth %f\n", size );
            fprintf( file, "    }\n" );
            fprintf( file, "  }\n" );
        }

        fprintf( file, "}\n\n" );
        fclose( file );
    } while ( FALSE );
#endif

    free( stats );
} /* _bsp_find_split() */

int
bsp_remove_point(
    void * handle,
    float * point )
{
    bsp_tree * tree;
    tree = (bsp_tree *) handle;
    assert( handle != NULL );
    return _bsp_remove_point( tree->top, point, tree->dimensions );
} /* bsp_remove_point() */

int
_bsp_remove_point(
    bsp_node * node,
    float * point,
    int dimensions )
{
    
    if ( node->ge == NULL ) {
        int d, i, cnt;
        float ** points;
        cnt = array_count( node->idxarray );
        points = node->root->pointarrays;
        for ( i = 0; i < cnt; i++ ) {
            for ( d = 0; d < dimensions; d++ ) {
                if ( point[d] != points[d][node->idxarray[i]] )
                    break;
            }
            if ( d == dimensions ) {
                int oldidx = node->idxarray[i];
                array_remove_int_fast( node->idxarray, i );
                return oldidx;
            }
        }
        return -1;
    }

    if ( point[node->splitdim] >= node->splitpos )
        return _bsp_remove_point( node->ge, point, dimensions );
    else
        return _bsp_remove_point( node->lt, point, dimensions );
} /* _bsp_remove_point() */

void
bsp_remove_point_by_idx(
    void * handle,
    int idx )
{
    bsp_tree * tree;
    float * point;
    int i;
    tree = (bsp_tree *) handle;
    assert( tree != NULL );
    point = malloc( sizeof(float) * tree->dimensions );
    assert( point != NULL );
    for ( i = 0; i < tree->dimensions; i++ ) {
        point[i] = tree->pointarrays[i][idx];
    }
    bsp_remove_point( handle, point );
    free( point );
} /* bsp_remove_point_by_idx() */

int
bsp_find_point(
    void * handle,
    float * point )
{
    bsp_tree * tree;
    tree = (bsp_tree *) handle;
    assert( tree != NULL );
    return _bsp_find_point( tree->top, point, tree->dimensions );
} /* bsp_find_point() */

int
_bsp_find_point(
    bsp_node * node,
    float * point,
    int dimensions )
{
    if ( node->ge == NULL ) {
        int d, i, cnt;
        float ** points;
        cnt = array_count( node->idxarray );
        points = node->root->pointarrays;
        for ( i = 0; i < cnt; i++ ) {
            for ( d = 0; d < dimensions; d++ ) {
                if ( point[d] != points[d][node->idxarray[i]] )
                    break;
            }
            if ( d == dimensions )
                return node->idxarray[i];
        }
        return -1;
    }
    if ( point[node->splitdim] >= node->splitpos )
        return _bsp_find_point( node->ge, point, dimensions );
    else
        return _bsp_find_point( node->lt, point, dimensions );
} /* _bsp_find_point() */

void
bsp_find_points(
    void * handle,
    float * point,
    float radius,
    int ** indexarray )
{
    bsp_tree * tree;
    tree = (bsp_tree *) handle;
    assert( tree != NULL );

    _bsp_add_points( tree->top, point, tree->dimensions, radius, indexarray );
} /* bsp_find_points() */

void
_bsp_add_points(
    bsp_node * node,
    float * point,
    int dimensions,
    float radius,
    int ** idxarray )
{
    if ( node->ge == NULL ) {
        int d, i, cnt;
        float sqrad;
        float ** points;
        sqrad = radius * radius;
        points = node->root->pointarrays;
        cnt = array_count( node->idxarray );
        for ( i = 0; i < cnt; i++ ) {
            float dist = 0.0;
            for ( d = 0; d < dimensions; d++ ) {
                float diff;
                diff = point[d] - points[d][node->idxarray[i]];
                dist += diff * diff;
            }
            if ( dist <= sqrad )
                array_append_int( idxarray[0], node->idxarray[i] );
        }
    } else {
        float diff;
        diff = point[node->splitdim] - node->splitpos;
        if ( diff < 0.0f ) diff = 0.0f - diff;
        if ( diff <= radius ) {
            _bsp_add_points( node->ge, point, dimensions, radius, idxarray );
            _bsp_add_points( node->lt, point, dimensions, radius, idxarray );
        } else if ( point[node->splitdim] >= node->splitpos ) {
            _bsp_add_points( node->ge, point, dimensions, radius, idxarray );
        } else {
            _bsp_add_points( node->lt, point, dimensions, radius, idxarray );
        }
    }
} /* _bsp_add_points() */

int
bsp_find_closest(
    void * handle,
    float * point )
{
    int close, closest, i, d, cnt;
    bsp_tree * tree;
    float radius, bestradius;
    int * indexarray;

    tree = (bsp_tree *) handle;
    assert( tree != NULL );

    close = _bsp_find_close( tree->top, point, tree->dimensions );
    if ( close == -1 ) return -1;

    radius = 0.0f;
    for ( d = 0; d < tree->dimensions; d++ ) {
        float diff;
        diff = point[d] - tree->pointarrays[d][close];
        radius += diff * diff;
    }
    radius = sqrt( radius );

    indexarray = array_create_int( 8 );
    bsp_find_points( tree, point, radius, &indexarray );

    cnt = array_count( indexarray );
    closest = -1;
    bestradius = FLT_MAX;
    for ( i = 0; i < cnt; i++ ) {
        radius = 0.0f;
        for ( d = 0; d < tree->dimensions; d++ ) {
            float diff;
            diff = point[d] - tree->pointarrays[d][indexarray[i]];
            radius += diff * diff;
        }
        if ( radius < bestradius ) {
            closest = indexarray[i];
            bestradius = radius;
        }
    }

    array_destroy( indexarray );
    return closest;
} /* bsp_find_closest() */

int
_bsp_find_close(
    bsp_node * node,
    float * point,
    int dimensions )
{
    if ( node->ge == NULL ) {
        int d, i, best, cnt;
        float ** points;
        float bestdist;
        cnt = array_count( node->idxarray );
        points = node->root->pointarrays;
        best = -1;
        bestdist = FLT_MAX;
        for ( i = 0; i < cnt; i++ ) {
            float diff, dist;
            dist = 0.0f;
            for ( d = 0; d < dimensions; d++ ) {
                diff = point[d] - points[d][node->idxarray[i]];
                dist += diff * diff;
            }
            if ( dist < bestdist ) {
                bestdist = dist;
                best = i;
            }
        }
        return node->idxarray[best];
    }
    if ( point[node->splitdim] >= node->splitpos )
        return _bsp_find_close( node->ge, point, dimensions );
    else
        return _bsp_find_close( node->lt, point, dimensions );
} /* _bsp_find_close() */

int
bsp_find_closest_points(
    void * handle,
    float * point,
    float radius,
    int ** pointarray )
{
    bsp_tree * tree;
    int d, i, cnt, closest;
    float bestradius;

    tree = (bsp_tree *) handle;
    assert( tree != NULL );

    bsp_find_points( tree, point, radius, pointarray );

    cnt = array_count( pointarray[0] );
    closest = -1;
    bestradius = FLT_MAX;
    for ( i = 0; i < cnt; i++ ) {
        radius = 0.0f;
        for ( d = 0; d < tree->dimensions; d++ ) {
            float diff;
            diff = point[d] - tree->pointarrays[d][pointarray[0][i]];
            radius += diff * diff;
        }
        if ( radius < bestradius ) {
            closest = pointarray[0][i];
            bestradius = radius;
        }
    }

    return closest;
} /* bsp_find_closest_points() */

/**************************************************************************/
