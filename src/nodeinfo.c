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

#include <profit/nodeinfo.h>
#include <profit/util/array.h>
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <assert.h>
#include <stdlib.h>
#ifdef HAVE_STRING_H
#include <string.h>
#endif

/**************************************************************************/

/* declarations for node init functions - they are not in the node header
   files, since they aren't suitable for the exported API */

void prf_header_init( void );
void prf_group_init( void );
void prf_object_init( void );
void prf_face_init( void );

void prf_matrix_init( void );

void prf_vertex_palette_init( void );
void prf_color_palette_init( void );
void prf_material_init( void );
void prf_texture_init( void );
void prf_material_palette_init( void );

void prf_push_attribute_init( void );
void prf_pop_attribute_init( void );
void prf_push_extension_init( void );
void prf_pop_extension_init( void );
void prf_push_level_init( void );
void prf_pop_level_init( void );
void prf_push_subface_init( void );
void prf_pop_subface_init( void );

void prf_vertex_with_color_init( void );
void prf_obsolete_vertex_with_color_init( void );
void prf_vertex_with_normal_init( void );
void prf_obsolete_vertex_with_normal_init( void );
void prf_vertex_with_normal_and_texture_init( void );
void prf_vertex_with_texture_init( void );
void prf_vertex_list_init( void );
void prf_morph_vertex_list_init( void );
void prf_instance_definition_init( void );
void prf_instance_reference_init( void );
void prf_external_reference_init(void);
void prf_level_of_detail_init(void);

/**************************************************************************/

static prf_nodeinfo_t **  nodetypes;

/* These are just default entries without hooks.  */
prf_nodeinfo_t prf_nodeinfo_defaults[] = {
    {   1, PRF_PRIMARY,                      "Header",
           NULL, NULL, NULL, NULL, NULL, NULL, NULL },
    {   2, PRF_PRIMARY,                      "Group",
           NULL, NULL, NULL, NULL, NULL, NULL, NULL },
    {   3, PRF_PRIMARY | PRF_OBSOLETE,       "Level of Detail (obsolete)",
           NULL, NULL, NULL, NULL, NULL, NULL, NULL },
    {   4, PRF_PRIMARY,                      "Object",
           NULL, NULL, NULL, NULL, NULL, NULL, NULL },
    {   5, PRF_PRIMARY,                      "Face",
           NULL, NULL, NULL, NULL, NULL, NULL, NULL },
    {   6, PRF_VERTEX | PRF_OBSOLETE,        "Vertex with ID (obsolete)",
           NULL, NULL, NULL, NULL, NULL, NULL, NULL },
    {   7, PRF_VERTEX | PRF_OBSOLETE,        "Short Vertex (obsolete)",
           NULL, NULL, NULL, NULL, NULL, NULL, NULL },
    {   8, PRF_VERTEX | PRF_OBSOLETE,        "Vertex with Color (obsolete)",
           NULL, NULL, NULL, NULL, NULL, NULL, NULL },
    {   9, PRF_VERTEX | PRF_OBSOLETE,
           "Vertex with Color and Normal (obsolete)",
           NULL, NULL, NULL, NULL, NULL, NULL, NULL },
    {  10, PRF_CONTROL | PRF_PUSH_NODE,      "Push Level",
           NULL, NULL, NULL, NULL, NULL, NULL, NULL },
    {  11, PRF_CONTROL | PRF_POP_NODE,       "Pop Level",
           NULL, NULL, NULL, NULL, NULL, NULL, NULL },
    {  12, PRF_ANCILLARY | PRF_TRANSFORM | PRF_OBSOLETE,
           "Translate (obsolete)",
           NULL, NULL, NULL, NULL, NULL, NULL, NULL },
    {  13, PRF_PRIMARY | PRF_OBSOLETE,       "Degree of Freedom (obsolete)",
           NULL, NULL, NULL, NULL, NULL, NULL, NULL },
    {  14, PRF_PRIMARY,                      "Degree of Freedom",
           NULL, NULL, NULL, NULL, NULL, NULL, NULL },
    {  16, PRF_CONTROL | PRF_REFERENCE | PRF_OBSOLETE,
           "Instance Reference (obsolete)",
           NULL, NULL, NULL, NULL, NULL, NULL, NULL },
    {  17, PRF_CONTROL | PRF_DEFINITION | PRF_OBSOLETE,
           "Instance Definition (obsolete)",
           NULL, NULL, NULL, NULL, NULL, NULL, NULL },
    {  19, PRF_CONTROL | PRF_PUSH_NODE,      "Push Subface",
           NULL, NULL, NULL, NULL, NULL, NULL, NULL },
    {  20, PRF_CONTROL | PRF_POP_NODE,       "Pop Subface",
           NULL, NULL, NULL, NULL, NULL, NULL, NULL },
    {  21, PRF_CONTROL | PRF_PUSH_NODE,      "Push Extension",
           NULL, NULL, NULL, NULL, NULL, NULL, NULL },
    {  22, PRF_CONTROL | PRF_POP_NODE,       "Pop Extension",
           NULL, NULL, NULL, NULL, NULL, NULL, NULL },
    {  23, PRF_CONTINUATION,                 "Continuation", /* New in 15.7 */
           NULL, NULL, NULL, NULL, NULL, NULL, NULL },
    {  31, PRF_ANCILLARY,                    "Text Comment",
           NULL, NULL, NULL, NULL, NULL, NULL, NULL },
    {  32, PRF_ANCILLARY,                    "Color Palette",
           NULL, NULL, NULL, NULL, NULL, NULL, NULL },
    {  33, PRF_ANCILLARY,                    "Long ID",
           NULL, NULL, NULL, NULL, NULL, NULL, NULL },
    {  40, PRF_ANCILLARY | PRF_TRANSFORM | PRF_OBSOLETE,
           "Translate (obsolete)",
           NULL, NULL, NULL, NULL, NULL, NULL, NULL },
    {  41, PRF_ANCILLARY | PRF_TRANSFORM | PRF_OBSOLETE,
           "Rotate about Point (obsolete)",
           NULL, NULL, NULL, NULL, NULL, NULL, NULL },
    {  42, PRF_ANCILLARY | PRF_TRANSFORM | PRF_OBSOLETE,
           "Rotate about Edge (obsolete)",
           NULL, NULL, NULL, NULL, NULL, NULL, NULL },
    {  43, PRF_ANCILLARY | PRF_TRANSFORM | PRF_OBSOLETE,
           "Scale (obsolete)",
           NULL, NULL, NULL, NULL, NULL, NULL, NULL },
    {  44, PRF_ANCILLARY | PRF_TRANSFORM | PRF_OBSOLETE,
           "Translate (obsolete)",
           NULL, NULL, NULL, NULL, NULL, NULL, NULL },
    {  45, PRF_ANCILLARY | PRF_TRANSFORM | PRF_OBSOLETE,
           "Scale (Nonuniform) (obsolete)",
           NULL, NULL, NULL, NULL, NULL, NULL, NULL },
    {  46, PRF_ANCILLARY | PRF_TRANSFORM | PRF_OBSOLETE,
           "Rotate about Point (obsolete)",
           NULL, NULL, NULL, NULL, NULL, NULL, NULL },
    {  47, PRF_ANCILLARY | PRF_TRANSFORM | PRF_OBSOLETE,
           "Rotate and/or Scale to Point (obsolete)",
           NULL, NULL, NULL, NULL, NULL, NULL, NULL },
    {  48, PRF_ANCILLARY | PRF_TRANSFORM | PRF_OBSOLETE,
           "Put Transform (obsolete)",
           NULL, NULL, NULL, NULL, NULL, NULL, NULL },
    {  49, PRF_ANCILLARY | PRF_TRANSFORM,    "Transformation Matrix",
           NULL, NULL, NULL, NULL, NULL, NULL, NULL },
    {  50, PRF_ANCILLARY,                    "Vector",
           NULL, NULL, NULL, NULL, NULL, NULL, NULL },
    {  51, PRF_ANCILLARY | PRF_OBSOLETE,     "Bounding Box (obsolete)",
           NULL, NULL, NULL, NULL, NULL, NULL, NULL },
    {  52, PRF_ANCILLARY,                    "MultiTexture", /* New in 15.7 */
           NULL, NULL, NULL, NULL, NULL, NULL, NULL },
    {  53, PRF_ANCILLARY,                    "UV List", /* New in 15.7 */
           NULL, NULL, NULL, NULL, NULL, NULL, NULL },
    {  55, PRF_PRIMARY,                      "Binary Separating Plane",
           NULL, NULL, NULL, NULL, NULL, NULL, NULL },
    {  60, PRF_ANCILLARY | PRF_REFERENCE,    "Replicate",
           NULL, NULL, NULL, NULL, NULL, NULL, NULL },
    {  61, PRF_CONTROL | PRF_REFERENCE,      "Instance Reference",
           NULL, NULL, NULL, NULL, NULL, NULL, NULL },
    {  62, PRF_CONTROL | PRF_DEFINITION,     "Instance Definition",
           NULL, NULL, NULL, NULL, NULL, NULL, NULL },
    {  63, PRF_PRIMARY | PRF_REFERENCE,      "External Reference",
           NULL, NULL, NULL, NULL, NULL, NULL, NULL },
    {  64, PRF_ANCILLARY,                    "Texture Palette",
           NULL, NULL, NULL, NULL, NULL, NULL, NULL },
    {  65, PRF_ANCILLARY | PRF_OBSOLETE,     "Eyepoint Palette (obsolete)",
           NULL, NULL, NULL, NULL, NULL, NULL, NULL },
    {  66, PRF_ANCILLARY | PRF_OBSOLETE,     "Material Palette (obsolete)",
           NULL, NULL, NULL, NULL, NULL, NULL, NULL },
    {  67, PRF_ANCILLARY,                    "Vertex Palette",
           NULL, NULL, NULL, NULL, NULL, NULL, NULL },
    {  68, PRF_VERTEX,                       "Vertex with Color",
           NULL, NULL, NULL, NULL, NULL, NULL, NULL },
    {  69, PRF_VERTEX,                       "Vertex with Color and Normal",
           NULL, NULL, NULL, NULL, NULL, NULL, NULL },
    {  70, PRF_VERTEX,                       "Vertex w/ Color, Normal and UV",
           NULL, NULL, NULL, NULL, NULL, NULL, NULL },
    {  71, PRF_VERTEX,                       "Vertex with Color and UV",
           NULL, NULL, NULL, NULL, NULL, NULL, NULL },
    {  72, PRF_PRIMARY,                      "Vertex List",
           NULL, NULL, NULL, NULL, NULL, NULL, NULL },
    {  73, PRF_PRIMARY,                      "Level of Detail",
           NULL, NULL, NULL, NULL, NULL, NULL, NULL },
    {  74, PRF_ANCILLARY,                    "Bounding Box",
           NULL, NULL, NULL, NULL, NULL, NULL, NULL },
    {  76, PRF_ANCILLARY | PRF_TRANSFORM,    "Rotate about Edge",
           NULL, NULL, NULL, NULL, NULL, NULL, NULL },
    {  77, PRF_ANCILLARY | PRF_TRANSFORM | PRF_OBSOLETE,
           "Scale (obsolete)",
           NULL, NULL, NULL, NULL, NULL, NULL, NULL },
    {  78, PRF_ANCILLARY | PRF_TRANSFORM,    "Translate",
           NULL, NULL, NULL, NULL, NULL, NULL, NULL },
    {  79, PRF_ANCILLARY | PRF_TRANSFORM,    "Scale (Nonuniform)",
           NULL, NULL, NULL, NULL, NULL, NULL, NULL },
    {  80, PRF_ANCILLARY | PRF_TRANSFORM,    "Rotate about Point",
           NULL, NULL, NULL, NULL, NULL, NULL, NULL },
    {  81, PRF_ANCILLARY | PRF_TRANSFORM,    "Rotate and/or Scale to Point",
           NULL, NULL, NULL, NULL, NULL, NULL, NULL },
    {  82, PRF_ANCILLARY | PRF_TRANSFORM,    "Put Transform",
           NULL, NULL, NULL, NULL, NULL, NULL, NULL },
    {  83, PRF_ANCILLARY,                    "Eyepoint / Trackplane Palette",
           NULL, NULL, NULL, NULL, NULL, NULL, NULL },
    {  84, PRF_PRIMARY,                      "Mesh", /* New in 15.7.0 */
           NULL, NULL, NULL, NULL, NULL, NULL, NULL },
    {  85, PRF_PRIMARY,                      "Local Vertex Pool", /* New in 15.7.0 */
           NULL, NULL, NULL, NULL, NULL, NULL, NULL }, 
    {  86, PRF_PRIMARY,                      "Mesh Primitive", /* New in 15.7.0 */
           NULL, NULL, NULL, NULL, NULL, NULL, NULL },
    {  87, PRF_PRIMARY,                      "Road Segment",
           NULL, NULL, NULL, NULL, NULL, NULL, NULL },
    {  88, PRF_ANCILLARY,                    "Road Zone",
           NULL, NULL, NULL, NULL, NULL, NULL, NULL },
    {  89, PRF_PRIMARY,                      "Morph Vertex List",
           NULL, NULL, NULL, NULL, NULL, NULL, NULL },
    {  90, PRF_ANCILLARY,                    "Behaviour (Linkage) Palette",
           NULL, NULL, NULL, NULL, NULL, NULL, NULL },
    {  91, PRF_PRIMARY,                      "Sound",
           NULL, NULL, NULL, NULL, NULL, NULL, NULL },
    {  92, PRF_PRIMARY,                      "Road Path",
           NULL, NULL, NULL, NULL, NULL, NULL, NULL },
    {  93, PRF_ANCILLARY,                    "Sound Palette",
           NULL, NULL, NULL, NULL, NULL, NULL, NULL },
    {  94, PRF_ANCILLARY | PRF_TRANSFORM,    "General Matrix",
           NULL, NULL, NULL, NULL, NULL, NULL, NULL },
    {  95, PRF_PRIMARY,                      "Text",
           NULL, NULL, NULL, NULL, NULL, NULL, NULL },
    {  96, PRF_PRIMARY,                      "Switch",
           NULL, NULL, NULL, NULL, NULL, NULL, NULL },
    {  97, PRF_ANCILLARY,                    "Line Style",
           NULL, NULL, NULL, NULL, NULL, NULL, NULL },
    {  98, PRF_PRIMARY,                      "Clip Region",
           NULL, NULL, NULL, NULL, NULL, NULL, NULL },
    { 100, PRF_PRIMARY,                      "Extension",
           NULL, NULL, NULL, NULL, NULL, NULL, NULL },
    { 101, PRF_PRIMARY,                      "Light Source",
           NULL, NULL, NULL, NULL, NULL, NULL, NULL },
    { 102, PRF_ANCILLARY,                    "Light Source Palette",
           NULL, NULL, NULL, NULL, NULL, NULL, NULL },
    { 103, PRF_RESERVED,                     "Reserved",
           NULL, NULL, NULL, NULL, NULL, NULL, NULL },
    { 104, PRF_RESERVED,                     "Reserved",
           NULL, NULL, NULL, NULL, NULL, NULL, NULL },
    { 105, PRF_ANCILLARY,                    "Bounding Sphere",
           NULL, NULL, NULL, NULL, NULL, NULL, NULL },
    { 106, PRF_ANCILLARY,                    "Bounding Cylinder",
           NULL, NULL, NULL, NULL, NULL, NULL, NULL },
    { 107, PRF_RESERVED,                     "Reserved",
           NULL, NULL, NULL, NULL, NULL, NULL, NULL },
    { 108, PRF_ANCILLARY,                    "Bounding Volume Center",
           NULL, NULL, NULL, NULL, NULL, NULL, NULL },
    { 109, PRF_ANCILLARY,                    "Bounding Volume Orientation",
           NULL, NULL, NULL, NULL, NULL, NULL, NULL },
    { 110, PRF_ANCILLARY | PRF_OBSOLETE,
           "Histogram Bounding Volume (obsolete)",
           NULL, NULL, NULL, NULL, NULL, NULL, NULL },
    { 111, PRF_PRIMARY,                      "Light Point",
           NULL, NULL, NULL, NULL, NULL, NULL, NULL },
    { 112, PRF_ANCILLARY,                    "Texture Mapping Palette",
           NULL, NULL, NULL, NULL, NULL, NULL, NULL },
    { 113, PRF_ANCILLARY,                    "Material Palette",
           NULL, NULL, NULL, NULL, NULL, NULL, NULL },
    { 114, PRF_ANCILLARY,                    "Color Name Palette",
           NULL, NULL, NULL, NULL, NULL, NULL, NULL },
    { 115, PRF_PRIMARY,                      "Continuously Adaptive Terrain",
           NULL, NULL, NULL, NULL, NULL, NULL, NULL },
    { 116, PRF_ANCILLARY,                    "CAT Data Header",
           NULL, NULL, NULL, NULL, NULL, NULL, NULL },
    { 117, PRF_RESERVED,                     "Reserved",
           NULL, NULL, NULL, NULL, NULL, NULL, NULL },
    { 118, PRF_RESERVED,                     "Reserved",
           NULL, NULL, NULL, NULL, NULL, NULL, NULL },
    { 119, PRF_RESERVED,                     "Reserved",
           NULL, NULL, NULL, NULL, NULL, NULL, NULL },
    { 120, PRF_RESERVED,                     "Reserved",
           NULL, NULL, NULL, NULL, NULL, NULL, NULL },
    { 121, PRF_RESERVED,                     "Reserved",
           NULL, NULL, NULL, NULL, NULL, NULL, NULL },
    { 122, PRF_CONTROL | PRF_PUSH_NODE,      "Push Attribute",
           NULL, NULL, NULL, NULL, NULL, NULL, NULL },
    { 123, PRF_CONTROL | PRF_POP_NODE,       "Pop Attribute",
           NULL, NULL, NULL, NULL, NULL, NULL, NULL },
    { 124, PRF_RESERVED,                     "Reserved",
           NULL, NULL, NULL, NULL, NULL, NULL, NULL },
    { 125, 0,                                "Adaptive Attribute",
           NULL, NULL, NULL, NULL, NULL, NULL, NULL },
    { 126, PRF_PRIMARY,                      "Curve Bead",
           NULL, NULL, NULL, NULL, NULL, NULL, NULL },
    { 127, PRF_PRIMARY,                      "Road Construction",
           NULL, NULL, NULL, NULL, NULL, NULL, NULL }
}; /* struct prf_nodeinfo_defaults[] */

/****************************************************************************/

void
prf_nodeinfo_init(
    void )
{
    int idx;

    nodetypes = (prf_nodeinfo_t **) prf_array_init(128, sizeof(prf_nodeinfo_t));
    assert(nodetypes != NULL);

    idx = 0;
    while ( idx < (sizeof(prf_nodeinfo_defaults) / sizeof(prf_nodeinfo_defaults[0])) ) {
      /* FIXME: why is commented out code here? 20030425 mortene. */
      /* prf_nodeinfo_defaults[idx].opcode != 0 */
        prf_nodeinfo_set(&prf_nodeinfo_defaults[idx]);
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

    prf_external_reference_init();
    prf_level_of_detail_init();

} /* prf_nodeinfo_init() */

/**************************************************************************/

void
prf_nodeinfo_exit(
    void )
{
    int i, count;
    count = prf_array_count(nodetypes);
    for ( i = 0; i < count; i++ ) {
        if ( nodetypes[i] != NULL ) {
            free(nodetypes[i]->name);
            free(nodetypes[i]);
        }
    }
    prf_array_free(nodetypes);
    nodetypes = NULL;
} /* prf_nodeinfo_exit() */

/**************************************************************************/

bool_t
prf_nodeinfo_set(const prf_nodeinfo_t * nodeinfo)
{
    int i;
    prf_nodeinfo_t * newnodeinfo;

    assert(nodeinfo->opcode > 0);

    i = prf_array_count(nodetypes);
    while ( i <= nodeinfo->opcode ) {
        nodetypes = (prf_nodeinfo_t **) prf_array_append_ptr(nodetypes, NULL);
        i++;
    }

    if ( nodetypes[nodeinfo->opcode ] != NULL ) {
        assert(nodetypes[nodeinfo->opcode] != nodeinfo);
        free(nodetypes[nodeinfo->opcode]->name);
        free(nodetypes[nodeinfo->opcode]);
    }

    newnodeinfo = (prf_nodeinfo_t *) malloc(sizeof(struct prf_nodeinfo_s));
    assert(newnodeinfo != NULL);
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
    newnodeinfo->name = (char *) malloc(strlen(nodeinfo->name) + 1);
    assert(newnodeinfo->name != NULL);
    strcpy(newnodeinfo->name, nodeinfo->name);
    
    return TRUE;
} /* prf_nodeinfo_set() */

/**************************************************************************/

prf_nodeinfo_t *
prf_nodeinfo_get(
    uint16_t opcode )
{
    if ( opcode < prf_array_count(nodetypes) )
        return nodetypes[opcode];
    return NULL;
} /* prf_nodeinfo_get() */

/**************************************************************************/
