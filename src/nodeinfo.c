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

#include <profit/nodeinfo.h>
#include <profit/util/array.h>
#include <config.h>

#include <assert.h>
#include <stdlib.h>
#include <string.h>

/**************************************************************************/

static prf_nodeinfo_t **  nodetypes;
prf_nodeinfo_t  prf_nodeinfo_defaults[];

/**************************************************************************/

void
prf_nodeinfo_init(
    void )
{
    int idx;

    nodetypes = prf_array_init( 128, sizeof( prf_nodeinfo_t ) );
    assert( nodetypes != NULL );

    idx = 0;
    while ( prf_nodeinfo_defaults[ idx ].opcode != 0 ) {
        prf_nodeinfo_set( &prf_nodeinfo_defaults[ idx ] );
        idx++;
    }

    /* place functions for setting load_f, save_f, and state_f here... */

    prf_header_init();
    prf_group_init();
    prf_object_init();
    prf_face_init();

    prf_push_attribute_init();
    prf_pop_attribute_init();
    prf_push_extension_init();
    prf_pop_extension_init();
    prf_push_level_init();
    prf_pop_level_init();
    prf_push_subface_init();
    prf_pop_subface_init();

    prf_vertex_palette_init();
    prf_vertex_with_color_init();
    prf_obsolete_vertex_with_color_init();
    prf_vertex_with_normal_init();
    prf_obsolete_vertex_with_normal_init();
    prf_vertex_with_normal_and_texture_init();
    prf_vertex_with_texture_init();

    prf_vertex_list_init();
    prf_morph_vertex_list_init();

    prf_color_palette_init();
    prf_material_init();
    prf_texture_init();
    prf_material_palette_init(); /* obsolete */
    prf_instance_definition_init();
    prf_instance_reference_init();

    prf_matrix_init();

} /* prf_nodeinfo_init() */

/**************************************************************************/

void
prf_nodeinfo_exit(
    void )
{
    int i, count;
    count = prf_array_count( nodetypes );
    for ( i = 0; i < count; i++ ) {
        if ( nodetypes[i] != NULL ) {
            free( nodetypes[ i ]->name );
            free( nodetypes[ i ] );
        }
    }
    prf_array_free( nodetypes );
    nodetypes = NULL;
} /* prf_nodeinfo_exit() */

/**************************************************************************/

bool_t
prf_nodeinfo_set(
    const prf_nodeinfo_t * nodeinfo )
{
    int i;
    prf_nodeinfo_t * newnodeinfo;

    assert( nodeinfo->opcode > 0 );

    i = prf_array_count( nodetypes );
    while ( i <= nodeinfo->opcode ) {
        nodetypes = prf_array_append_ptr( nodetypes, NULL );
        i++;
    }

    if ( nodetypes[ nodeinfo->opcode ] != NULL ) {
        assert( nodetypes[ nodeinfo->opcode ] != nodeinfo );
        free( nodetypes[ nodeinfo->opcode ]->name );
        free( nodetypes[ nodeinfo->opcode ] );
    }

    newnodeinfo = malloc( sizeof( struct prf_nodeinfo_s ) );
    assert( newnodeinfo != NULL );
    nodetypes[ nodeinfo->opcode ] = newnodeinfo;
    newnodeinfo->opcode = nodeinfo->opcode;
    newnodeinfo->flags = nodeinfo->flags;
    newnodeinfo->load_f = nodeinfo->load_f;
    newnodeinfo->save_f = nodeinfo->save_f;
    newnodeinfo->entry_f = nodeinfo->entry_f;
    newnodeinfo->exit_f = nodeinfo->exit_f;
    newnodeinfo->traverse_f = nodeinfo->traverse_f;
    newnodeinfo->destroy_f = nodeinfo->destroy_f;
    newnodeinfo->clone_f = nodeinfo->clone_f;
    newnodeinfo->name = malloc( strlen( nodeinfo->name ) + 1 );
    assert( newnodeinfo->name != NULL );
    strcpy( newnodeinfo->name, nodeinfo->name );
    
    return TRUE;
} /* prf_nodeinfo_set() */

/**************************************************************************/

prf_nodeinfo_t *
prf_nodeinfo_get(
    uint16_t opcode )
{
    if ( opcode < prf_array_count( nodetypes ) )
        return nodetypes[ opcode ];
    return NULL;
} /* prf_nodeinfo_get() */

/**************************************************************************/

/* These are just default entries without hooks.  */

prf_nodeinfo_t prf_nodeinfo_defaults[] = {
    {   1, PRF_PRIMARY,                      "Header" },
    {   2, PRF_PRIMARY,                      "Group" },
    {   3, PRF_PRIMARY | PRF_OBSOLETE,       "Level of Detail (obsolete)" },
    {   4, PRF_PRIMARY,                      "Object" },
    {   5, PRF_PRIMARY,                      "Face" },
    {   6, PRF_VERTEX | PRF_OBSOLETE,        "Vertex with ID (obsolete)" },
    {   7, PRF_VERTEX | PRF_OBSOLETE,        "Short Vertex (obsolete)" },
    {   8, PRF_VERTEX | PRF_OBSOLETE,        "Vertex with Color (obsolete)" },
    {   9, PRF_VERTEX | PRF_OBSOLETE,
           "Vertex with Color and Normal (obsolete)" },
    {  10, PRF_CONTROL | PRF_PUSH_NODE,      "Push Level" },
    {  11, PRF_CONTROL | PRF_POP_NODE,       "Pop Level" },
    {  12, PRF_ANCILLARY | PRF_TRANSFORM | PRF_OBSOLETE,
           "Translate (obsolete)" },
    {  13, PRF_PRIMARY | PRF_OBSOLETE,       "Degree of Freedom (obsolete)" },
    {  14, PRF_PRIMARY,                      "Degree of Freedom" },
    {  16, PRF_CONTROL | PRF_REFERENCE | PRF_OBSOLETE,
           "Instance Reference (obsolete)" },
    {  17, PRF_CONTROL | PRF_DEFINITION | PRF_OBSOLETE,
           "Instance Definition (obsolete)" },
    {  19, PRF_CONTROL | PRF_PUSH_NODE,      "Push Subface" },
    {  20, PRF_CONTROL | PRF_POP_NODE,       "Pop Subface" },
    {  21, PRF_CONTROL | PRF_PUSH_NODE,      "Push Extension" },
    {  22, PRF_CONTROL | PRF_POP_NODE,       "Pop Extension" },
    {  31, PRF_ANCILLARY,                    "Text Comment" },
    {  32, PRF_ANCILLARY,                    "Color Palette" },
    {  33, PRF_ANCILLARY,                    "Long ID" },
    {  40, PRF_ANCILLARY | PRF_TRANSFORM | PRF_OBSOLETE,
           "Translate (obsolete)" },
    {  41, PRF_ANCILLARY | PRF_TRANSFORM | PRF_OBSOLETE,
           "Rotate about Point (obsolete)" },
    {  42, PRF_ANCILLARY | PRF_TRANSFORM | PRF_OBSOLETE,
           "Rotate about Edge (obsolete)" },
    {  43, PRF_ANCILLARY | PRF_TRANSFORM | PRF_OBSOLETE,
           "Scale (obsolete)" },
    {  44, PRF_ANCILLARY | PRF_TRANSFORM | PRF_OBSOLETE,
           "Translate (obsolete)" },
    {  45, PRF_ANCILLARY | PRF_TRANSFORM | PRF_OBSOLETE,
           "Scale (Nonuniform) (obsolete)" },
    {  46, PRF_ANCILLARY | PRF_TRANSFORM | PRF_OBSOLETE,
           "Rotate about Point (obsolete)" },
    {  47, PRF_ANCILLARY | PRF_TRANSFORM | PRF_OBSOLETE,
           "Rotate and/or Scale to Point (obsolete)" },
    {  48, PRF_ANCILLARY | PRF_TRANSFORM | PRF_OBSOLETE,
           "Put Transform (obsolete)" },
    {  49, PRF_ANCILLARY | PRF_TRANSFORM,    "Transformation Matrix" },
    {  50, PRF_ANCILLARY,                    "Vector" },
    {  51, PRF_ANCILLARY | PRF_OBSOLETE,     "Bounding Box (obsolete)" },
    {  55, PRF_PRIMARY,                      "Binary Separate Plane" },
    {  60, PRF_ANCILLARY | PRF_REFERENCE,    "Replicate" },
    {  61, PRF_CONTROL | PRF_REFERENCE,      "Instance Reference" },
    {  62, PRF_CONTROL | PRF_DEFINITION,     "Instance Definition" },
    {  63, PRF_PRIMARY | PRF_REFERENCE,      "External Reference" },
    {  64, PRF_ANCILLARY,                    "Texture Palette" },
    {  65, PRF_ANCILLARY | PRF_OBSOLETE,     "Eyepoint Palette (obsolete)" },
    {  66, PRF_ANCILLARY | PRF_OBSOLETE,     "Material Palette (obsolete)" },
    {  67, PRF_ANCILLARY,                    "Vertex Palette" },
    {  68, PRF_VERTEX,                       "Vertex with Color" },
    {  69, PRF_VERTEX,                       "Vertex with Color and Normal" },
    {  70, PRF_VERTEX,                       "Vertex w/ Color, Normal and UV" },
    {  71, PRF_VERTEX,                       "Vertex with Color and UV" },
    {  72, PRF_PRIMARY,                      "Vertex List" },
    {  73, PRF_PRIMARY,                      "Level of Detail" },
    {  74, PRF_ANCILLARY,                    "Bounding Box" },
    {  76, PRF_ANCILLARY | PRF_TRANSFORM,    "Rotate about Edge" },
    {  77, PRF_ANCILLARY | PRF_TRANSFORM | PRF_OBSOLETE,
           "Scale (obsolete)" },
    {  78, PRF_ANCILLARY | PRF_TRANSFORM,    "Translate" },
    {  79, PRF_ANCILLARY | PRF_TRANSFORM,    "Scale (Nonuniform)" },
    {  80, PRF_ANCILLARY | PRF_TRANSFORM,    "Rotate about Point" },
    {  81, PRF_ANCILLARY | PRF_TRANSFORM,    "Rotate and/or Scale to Point" },
    {  82, PRF_ANCILLARY | PRF_TRANSFORM,    "Put Transform" },
    {  83, PRF_ANCILLARY,                    "Eyepoint / Trackplane Palette" },
    {  84, PRF_RESERVED,                     "Reserved" },
    {  85, PRF_RESERVED,                     "Reserved" },
    {  86, PRF_RESERVED,                     "Reserved" },
    {  87, PRF_PRIMARY,                      "Road Segment" },
    {  88, PRF_ANCILLARY,                    "Road Zone" },
    {  89, PRF_PRIMARY,                      "Morph Vertex List" },
    {  90, PRF_ANCILLARY,                    "Behaviour (Linkage) Palette" },
    {  91, PRF_PRIMARY,                      "Sound" },
    {  92, PRF_PRIMARY,                      "Road Path" },
    {  93, PRF_ANCILLARY,                    "Sound Palette" },
    {  94, PRF_ANCILLARY | PRF_TRANSFORM,    "General Matrix" },
    {  95, PRF_PRIMARY,                      "Text" },
    {  96, PRF_PRIMARY,                      "Switch" },
    {  97, PRF_ANCILLARY,                    "Line Style" },
    {  98, PRF_PRIMARY,                      "Clip Region" },
    { 100, PRF_PRIMARY,                      "Extension" },
    { 101, PRF_PRIMARY,                      "Light Source" },
    { 102, PRF_ANCILLARY,                    "Light Source Palette" },
    { 103, PRF_RESERVED,                     "Reserved" },
    { 104, PRF_RESERVED,                     "Reserved" },
    { 105, PRF_ANCILLARY,                    "Bounding Sphere" },
    { 106, PRF_ANCILLARY,                    "Bounding Cylinder" },
    { 107, PRF_RESERVED,                     "Reserved" },
    { 108, PRF_ANCILLARY,                    "Bounding Volume Center" },
    { 109, PRF_ANCILLARY,                    "Bounding Volume Orientation" },
    { 110, PRF_ANCILLARY | PRF_OBSOLETE,
           "Histogram Bounding Volume (obsolete)" },
    { 111, PRF_PRIMARY,                      "Light Point" },
    { 112, PRF_ANCILLARY,                    "Texture Mapping Palette" },
    { 113, PRF_ANCILLARY,                    "Material Palette" },
    { 114, PRF_ANCILLARY,                    "Color Name Palette" },
    { 115, PRF_PRIMARY,                      "Continuously Adaptive Terrain" },
    { 116, PRF_ANCILLARY,                    "CAT Data Header" },
    { 117, PRF_RESERVED,                     "Reserved" },
    { 118, PRF_RESERVED,                     "Reserved" },
    { 119, PRF_RESERVED,                     "Reserved" },
    { 120, PRF_RESERVED,                     "Reserved" },
    { 121, PRF_RESERVED,                     "Reserved" },
    { 122, PRF_CONTROL | PRF_PUSH_NODE,      "Push Attribute" },
    { 123, PRF_CONTROL | PRF_POP_NODE,       "Pop Attribute" },
    { 124, PRF_RESERVED,                     "Reserved" },
    { 125, 0,                                "Adaptive Attribute" },
    { 126, PRF_PRIMARY,                      "Curve Bead" },
    { 127, PRF_PRIMARY,                      "Road Construction" },
    {   0, 0,                                NULL }, /* terminator */
}; /* struct prf_nodeinfo_defaults[] */

/****************************************************************************/

/* $Id$ */

