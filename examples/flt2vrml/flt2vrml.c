/**************************************************************************
 *
 *  flt2vrml.c
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

#include <stdio.h>
#include <assert.h>

#include <profit/model.h>
#include <profit/node.h>
#include <profit/vertex.h>
#include <profit/state.h>
#include <profit/profit.h>
#include <profit/nodeinfo.h>
#include <profit/material.h>
#include <profit/texture.h>
#include <profit/nodes/face.h>
#include <profit/nodes/header.h>
#include <profit/nodes/vertex_list.h>
#include <profit/nodes/morph_vertex_list.h>
#include <profit/nodes/group.h>
#include <profit/nodes/vertex_palette.h>
#include <profit/util/bfile.h>
#include <profit/callback.h>

#include "array.h"
#include "bsptree.h"
#include <stdlib.h>
#include <math.h>
#include <float.h>
#include <string.h>

#define FLT2VRML_VERSION "0.2"

static void flt2vrml( prf_model_t * model, FILE * file );

int main(int argc, char ** argv )
{
  bfile_t * modelfile;
  prf_model_t * model;
  FILE * vrml;

  if ( argc == 1 || argc > 3 ) {
    fprintf( stderr, "Usage: %s <flt-file> [<filename.wrl>]\n", argv[0] );
    return -1;
  }

  prf_init();

  modelfile = bf_create_r( argv[1] );
  if ( ! modelfile ) {
    fprintf( stderr, "Error: Couldn't open file named \"%s\".\n", argv[1] );
    prf_exit();
    return -1;
  }

  model = prf_model_create();
  assert( model != NULL );
  prf_model_poolmem( model );

  if ( ! prf_model_load( model, modelfile ) ) {
    bf_destroy( modelfile );
    prf_model_destroy( model );
    prf_exit();
    fprintf( stderr, "Error: Couldn't load Open Flight file \"%s\".\n",
      argv[1] );
    return -1;
  }
  bf_destroy( modelfile );

  if ( argc == 3 ) {
    vrml = fopen( argv[2], "w" );
    if ( ! vrml ) {
      perror( "fopen()" );
      prf_model_destroy( model );
      prf_exit();
      return -1;
    }
  } else {
    vrml = stdout;
  }

  flt2vrml( model, vrml );

  if ( argc == 3 ) fclose( vrml );
  prf_model_destroy( model );
  prf_exit();
  return 0;
} /* main() */

/**************************************************************************/


#define PUSH_SUBFACE_OPCODE 19
#define POP_SUBFACE_OPCODE  20

#define DEF_OPCODE         62
#define REF_OPCODE         61
#define GROUP_OPCODE        2
#define FACE_OPCODE         5
#define VLIST_OPCODE       72
#define MORPH_VLIST_OPCODE 89
#define PUSH_OPCODE        10
#define POP_OPCODE         11
#define VERTEX_C_OPCODE    68
#define VERTEX_CN_OPCODE   69
#define VERTEX_CNUV_OPCODE 70
#define VERTEX_CUV_OPCODE  71
#define MATRIX_OPCODE      49


typedef struct
{
  void *vertex_colors; /* bsp tree */
  void *texcoords;     /* bsp tree */
  int *colorIdx; /* growable array */

  int *indices; /* growable array */
  int *lineIndices; /* growable array */
  int *texIndices;  /* growable array */
  void *bspTree;

  unsigned int packed_color;
  short texture_pattern_index;
  char *texture_pattern_filename;
  short material_index;
  unsigned short transparency;
  unsigned short lightmode;
  prf_material_t material;
  unsigned int subface_level;
} faceset;



static faceset *
faceset_create()
{
  faceset *fs = (faceset*) malloc(sizeof(faceset));
  fs->texture_pattern_filename = NULL;
  fs->vertex_colors = bsp_create(3);
  fs->texcoords = bsp_create(2);

  fs->indices = array_create_int(4);
  fs->lineIndices = array_create_int(4);
  fs->texIndices = array_create_int(4);
  fs->colorIdx = array_create_int(4);

  fs->bspTree = bsp_create(3);
  return fs;
}

static void
faceset_delete(faceset *fs)
{
  assert(fs);
  bsp_destroy(fs->vertex_colors);
  bsp_destroy(fs->texcoords);
  bsp_destroy(fs->bspTree);

  array_destroy(fs->indices);
  array_destroy(fs->lineIndices);
  array_destroy(fs->texIndices);
  array_destroy(fs->colorIdx);
  free(fs);
}

/* global variables are cool :-> */

int *indexarray; /* growable array */
float *vertexarray; /* growable array */
int *colorarray; /* growable array */
float *texcoordarray; /* growable array */

void **facesets; /* growable array */
prf_model_t *currmodel;
void *materialLookup; /* bsp tree */
uint16_t ofversion;

static int profitCB(void *mystate, void *ud);
static void do_convert(prf_model_t *model, FILE *file);
static void convertFaceset(faceset *data, FILE *file);

static void
flt2vrml(prf_model_t * model,
  FILE * file )
{
  fprintf( file, "#VRML V1.0 ascii\n" );
  fprintf( file, "#[generated by flt2vrml v" FLT2VRML_VERSION ", " );
  fprintf( file, "http://www.sim.no]\n");
  fprintf( file, "\nSeparator {\n" );

  indexarray = array_create_int(4);
  vertexarray = array_create_float(4);
  colorarray = array_create_int(4);
  texcoordarray = array_create_float(4);

  facesets = array_create_ptr(4);
  currmodel = model;
  materialLookup = bsp_create(3);

  do_convert(model, file);

  fprintf( file, "}\n");
  array_destroy(indexarray);
  array_destroy(vertexarray);
  array_destroy(colorarray);
  array_destroy(texcoordarray);
  array_destroy(facesets);
  bsp_destroy(materialLookup);
} /* flt2vrml() */


static void
do_convert(prf_model_t *model, FILE *file)
{
  int i;
  struct prf_cb_s cb;

  struct prf_header_data *header =
    (struct prf_header_data*) model->header->data;
  ofversion = header->format_revision_level;

  prf_cb_set(cb, profitCB, NULL);
  prf_model_traverse_io(model, cb);

  for (i = 0; i < array_count(facesets); i++) {
    convertFaceset((faceset *)facesets[i], file);
    faceset_delete((faceset *)facesets[i]);
    facesets[i] = NULL;
  }
}

static void
print_index_array(FILE *file, int *idx)
{
  int i, n = array_count(idx);

  for (i = 0; i < n; i++) {
    if (i < n-1) fprintf(file,"%d,", idx[i]);
    else fprintf(file,"%d\n", idx[i]);

    if ((i & 0xf) == 0xf) fprintf(file,"\n      ");
  }
}

static void
set_default_material(prf_material_t *mat)
{
  mat->diffuse_red = 1.0f;
  mat->diffuse_green = 1.0f;
  mat->diffuse_blue = 1.0f;
  mat->specular_red = 1.0f;
  mat->specular_green = 1.0f;
  mat->specular_blue = 1.0f;
  mat->ambient_red = 0.2f;
  mat->ambient_green = 0.2f;
  mat->ambient_blue = 0.2f;
  mat->emissive_red = 0.0f;
  mat->emissive_green = 0.0f;
  mat->emissive_blue = 0.0f;
  mat->shininess = 25.6f;
  mat->alpha = 1.0f;
}

static void
convertFaceset(faceset *data, FILE *file)
{
  int dotex;
  int i, n,icnt, vcnt, lcnt, tcnt;
  float r,g,b;
  float tmp[3];
  float trans;
  int color_per_vertex = 0;

  icnt = array_count(data->indices);
  vcnt = bsp_get_num_points(data->bspTree);
  lcnt = array_count(data->lineIndices);
  tcnt = array_count(data->texIndices);
  dotex = data->texture_pattern_index >= 0 &&
    data->texture_pattern_filename != NULL;

  if (vcnt == 0) return;

  fprintf(file,"  Separator {\n");

#define bsptree data->bspTree
#define texcoords data->texcoords

  if (dotex) {
    fprintf(file,
            "  Texture2 {\n"
            "    filename \"%s\"\n"
            "  }\n", data->texture_pattern_filename);
  }

  r = (data->packed_color & 0xff) / 255.0f;
  g = (data->packed_color & 0xff00) / (256.0f*255.0f);
  b = (data->packed_color & 0xff0000) / (65536*255.0f);

  if (data->lightmode == 1 || data->lightmode == 3) {
    if (bsp_get_num_points(data->vertex_colors) > 1) {
      color_per_vertex = 1;
    }
    else {
      bsp_get_point(data->vertex_colors, 0, tmp);
      r = tmp[0]; g = tmp[1]; b = tmp[2];
    }
  }

  if (color_per_vertex) {
    fprintf(file,
            "  Material {\n"
            "    ambientColor %g %g %g\n"
            "    diffuseColor [\n",
            data->material.ambient_red,
            data->material.ambient_green,
            data->material.ambient_blue);

    n = bsp_get_num_points(data->vertex_colors);
    for (i = 0; i < n; i++) {
      bsp_get_point(data->vertex_colors, i, tmp);
      if (i < n-1)
        fprintf(file,"      %g %g %g,\n", tmp[0], tmp[1], tmp[2]);
      else
        fprintf(file,"      %g %g %g\n", tmp[0], tmp[1], tmp[2]);
    }
    fprintf(file,"    ]\n");
  }
  else {
    fprintf(file,
            "  Material {\n"
            "    diffuseColor %g %g %g\n"
            "    ambientColor %g %g %g\n",
            data->material.diffuse_red * r,
            data->material.diffuse_green * g,
            data->material.diffuse_blue * b,
            data->material.ambient_red * r,
            data->material.ambient_green * g,
            data->material.ambient_blue * b);
  }

  trans = 1.0f - (data->material.alpha *
                  (1.0f - (data->transparency / 65535.0f)));

  if (trans < 1.0f/255.0f)
    trans = 0.0f;
  else if (trans > 1.0f)
    trans = 1.0f;

  fprintf(file,
          "    specularColor %g %g %g\n"
          "    emissiveColor %g %g %g\n"
          "    shininess %g\n"
          "    transparency %g\n"
          "  }\n",
          data->material.specular_red,
          data->material.specular_green,
          data->material.specular_blue,
          data->material.emissive_red,
          data->material.emissive_green,
          data->material.emissive_blue,
          data->material.shininess / 128.0f,
          trans);

  if (vcnt) {
    n = vcnt;
    fprintf(file,
            "  Coordinate3 {\n"
            "    point [\n");
    for (i = 0; i < vcnt; i++) {
      bsp_get_point(bsptree, i, tmp);
      if (i < n-1)
        fprintf(file, "      %g %g %g,\n", tmp[0], tmp[2], -tmp[1]);
      else
        fprintf(file, "      %g %g %g\n", tmp[0], tmp[2], -tmp[1]);
    }
    fprintf(file,
            "    ]\n"
            "  }\n");

    n = bsp_get_num_points(texcoords);
    if (n) {
      fprintf(file,
              "  TextureCoordinate2 {\n"
              "    point [\n");
      for (i = 0; i < n; i++) {
        bsp_get_point(texcoords, i, tmp);
        if (i < n-1)
          fprintf(file,"      %g %g,\n", tmp[0], tmp[1]);
        else
          fprintf(file,"      %g %g\n", tmp[0], tmp[1]);
      }
      fprintf(file,
              "    ]\n"
              "  }\n");
    }

    if (icnt) {
      if (color_per_vertex) {
        fprintf(file,
                "  MaterialBinding {\n"
                "    value PER_VERTEX_INDEXED\n"
                "  }\n");
      }

      fprintf(file,
              "  IndexedFaceSet {\n");

      if (color_per_vertex) {
        fprintf(file,"    materialIndex [\n      ");
        print_index_array(file, data->colorIdx);
        fprintf(file,"    ]\n}\n");
      }

      fprintf(file,
              "    coordIndex [\n      ");
      print_index_array(file, data->indices);
      fprintf(file,"    ]\n");

      if (tcnt) {
        fprintf(file,"    textureCoordIndex [\n      ");
        print_index_array(file, data->texIndices);
        fprintf(file,"    ]\n");
      }
      fprintf(file,"  }\n");
    }
  }
  fprintf(file,"  }\n"); /* end of Separator { */

#undef bsptree
#undef texcoords
}

static void
packed_to_vec(unsigned int packed_color, float *vec)
{
  vec[0] = (packed_color & 0xff) / 255.0f;
  vec[1] = (packed_color & 0xff00) / (256.0f*255.0f);
  vec[2] = (packed_color & 0xff0000) / (65536.0f*255.0f);
}

static unsigned int
my_palette_lookup(prf_state_t * state, unsigned int name)
{
  unsigned int packed_color;
  float intensity;
  float r,g,b;

  if (ofversion < 1500) {
    packed_color = prf_state_color_lookup(state,
                   (name & 0x1ff),
                   name >> 9);
    intensity = (name>>9) / 127.0f;
  }
  else {
    packed_color = prf_state_color_lookup(state,
                   name >> 7,
                   name & 0x7f);
    intensity = (float)(name&0x7f) / 127.0f;
  }

  if ((packed_color & 0xff000000) == 0xff000000) {
    r = ((packed_color & 0xff) / 255.0f)*intensity;
    g = ((packed_color & 0xff00) / (256.0f*255.0f))*intensity;
    b = ((packed_color & 0xff0000) / (65536.0f*255.0f))*intensity;
  }
  else if ((packed_color & 0xff000000) == 0x10000000) {
    unsigned int t = packed_color & 0x7fff;
    r = (float)(t & 0x1f) / 32.0f;
    t >>= 5;
    g = (float)(t & 0x1f) / 32.0f;
    t >>= 5;
    b = (float)(t & 0x1f) / 32.0f;
  }
  else {
    unsigned int t = packed_color & 0x7fff;
    r = (float)(t & 0x1f) / 32.0f;
    t >>= 5;
    g = (float)(t & 0x1f) / 32.0f;
    t >>= 5;
    b = (float)(t & 0x1f) / 32.0f;
  }
  return (int)(r*255) +
    ((int)((b*255.0f))<<16) +
    ((int)((g*255.0f))<<8);
}

static void
mult_matrix_vec(float *m, float *v)
{
  float W;
  float src[3];
  float (*matrix)[4];

  matrix = (float (*)[4]) m;

  src[0] = v[0];
  src[1] = v[1];
  src[2] = v[2];

  W =
    src[0]*matrix[3][0]+
    src[1]*matrix[3][1]+
    src[2]*matrix[3][2]+
    matrix[3][3];

  if (fabs(W) > FLT_EPSILON) W = 1.0f / W;
  else W = 1.0f;

  v[0] =
    (src[0]*matrix[0][0]+
     src[1]*matrix[0][1]+
     src[2]*matrix[0][2]+
     matrix[0][3])*W;
  v[1] =
    (src[0]*matrix[1][0]+
     src[1]*matrix[1][1]+
     src[2]*matrix[1][2]+
     matrix[1][3])*W;
  v[2] =
    (src[0]*matrix[2][0]+
     src[1]*matrix[2][1]+
     src[2]*matrix[2][2]+
     matrix[2][3])*W;
}

static float *
get_coords_3d(float *arr, int idx, float *dst)
{
  dst[0] = arr[idx*3];
  dst[1] = arr[idx*3+1];
  dst[2] = arr[idx*3+2];
  return dst;
}

static float *
get_coords_2d(float *arr, int idx, float *dst)
{
  dst[0] = arr[idx*2];
  dst[1] = arr[idx*2+1];
  dst[2] = arr[idx*2+2];
  return dst;
}

static int
profitCB(void *mystate, void *ud)
{
  static int is_in_face = 0;
  static int current_index = -1;
  static int color_per_vertex = 0;
  prf_nodeinfo_t *info;
  int i,n;
  float tmp[3];
  prf_node_t **children;
  struct prf_vertex_s vertex;

  prf_state_t * state = (prf_state_t *)mystate;

  if (state->node->opcode == GROUP_OPCODE) {
    struct prf_group_data *data = (struct prf_group_data*) state->node->data;
    if (data->flags & PRF_GROUP_FLAG_ANIMATION) {
      assert(state->node->children && array_count(state->node->children) > 2);
      prf_state_push(state);
      prf_model_traverse_io_(state->node->children[1], state);
      prf_state_pop(state);
      return PRF_TRAV_SIBLING;
    }
  }
  else if (state->node->opcode == DEF_OPCODE) {
    return PRF_TRAV_SIBLING;
  }
  else if (state->node->opcode == REF_OPCODE) {
    prf_node_t *node = prf_state_get_instance_from_node(state, state->node);
    if (node) {
      int i, n = array_count(node->children);
      prf_state_push(state);
      for (i = 0; i < n; i++) {
	prf_model_traverse_io_(node->children[i], state);
      }
      prf_state_pop(state);
    }
    else {
      int n = array_count(state->instances);
    }
    assert(node);
    return PRF_TRAV_CONTINUE;
  }

  info = prf_nodeinfo_get(state->node->opcode);

  if (state->node->opcode == FACE_OPCODE) {
    struct prf_face_data *face;
    unsigned int packed_color;
    unsigned short lightmode;
    int idx;

    vertexarray=(float *)_array_set_count(vertexarray, 0);
    texcoordarray=(float *)_array_set_count(texcoordarray, 0);
    colorarray=(int *)_array_set_count(colorarray, 0);

    face = (struct prf_face_data*) state->node->data;

    if (!(face->flags & PRF_FACE_FLAGS_HIDDEN)) {
      short texture_pattern_index;
      short material_index;
      unsigned short transparency;
      is_in_face = 1;
      color_per_vertex = 0;
      if (face->light_mode == 1 || face->light_mode == 3) {
        color_per_vertex = 1;
      }

      packed_color = face->color_name_index;

      if (face->flags & 0x50000000) {
        packed_color = face->packed_color_primary;
      }
      else {
        if (ofversion < 1500) {
          packed_color = my_palette_lookup(state, face->color_name_index);
        }
        else {
          packed_color = my_palette_lookup(state, face->primary_color_index);
        }
      }
      texture_pattern_index = face->texture_pattern_index;
      material_index = face->material_index;

      transparency = (unsigned short)
        (((int32_t)(face->transparency) *
          (int32_t)(state->object_transparency)) >> 16);

      lightmode = face->light_mode;

      tmp[0] = (float) packed_color;
      tmp[1] = (float) texture_pattern_index +
        (float) material_index * 65536.0f;
      tmp[2] = (float) transparency +
        (float) lightmode * 65536.0f +
        (float) state->subface_level * 65536.0f * 256.0f;

      idx = bsp_add_point(materialLookup, tmp);
      current_index = idx;

      if (idx >= array_count(facesets)) {
        faceset *data = faceset_create();

        data->packed_color = packed_color;
        data->texture_pattern_index = texture_pattern_index;
        data->texture_pattern_filename = NULL;
        data->subface_level = state->subface_level;

        if (texture_pattern_index >= 0) {
          struct prf_texture_s tex;

          if (prf_state_texture_lookup(state, texture_pattern_index, &tex)) {

            if (strlen(tex.filename)) {
              data->texture_pattern_filename =
                (char *)malloc(strlen(tex.filename)+1);
              strcpy(data->texture_pattern_filename, tex.filename);
            }
          }
        }
         data->material_index = material_index;
         data->transparency = transparency;
         data->lightmode = lightmode;

         if (prf_state_material_lookup(state, material_index,
                                       &data->material)) {
             // FIXME: implement proper action
         }
         else {
           set_default_material(&data->material);
         }

         array_append_ptr(facesets, data);
      }

      children = state->node->children;
      if (children) {
        int insub = 0;
        n = array_count(children);

        prf_state_push(state);
        for (i = 0; i < n; i++) {
          if (children[i]->opcode == PUSH_SUBFACE_OPCODE) insub = 1;
          if (!insub) prf_model_traverse_io_(children[i], state);
          if (children[i]->opcode == POP_SUBFACE_OPCODE) insub = 0;
        }
        prf_state_pop(state);

        prf_state_push(state);
        insub = 0;
        for (i = 0; i < n; i++) {
          if (children[i]->opcode == PUSH_SUBFACE_OPCODE) insub = 1;
          if (insub) prf_model_traverse_io_(children[i], state);
          if (children[i]->opcode == POP_SUBFACE_OPCODE) insub = 0;
        }
        prf_state_pop(state);
      }
    }
    return PRF_TRAV_SIBLING; /* finished traversing face */
  }

  if (is_in_face && state->node->opcode == VLIST_OPCODE) {
    uint32_t *ptr;
    n = prf_vertex_list_get_count(state->node);
    ptr = prf_vertex_list_get_list_ptr(state->node);
    for (i = 0; i < n; i++) {
      prf_model_vertex_palette_lookup((prf_model_t*)currmodel,
                                      ptr[i], &vertex);

      array_append_float(vertexarray, vertex.x);
      array_append_float(vertexarray, vertex.y);
      array_append_float(vertexarray, vertex.z);

      if (vertex.has_texture) {
        array_append_float(texcoordarray, vertex.texture[0]);
        array_append_float(texcoordarray, vertex.texture[1]);
      }
      else {
        array_append_float(texcoordarray, 0.0f);
        array_append_float(texcoordarray, 0.0f);
      }
      if (color_per_vertex) {
        if (vertex.flags & 0x2000) {
          array_append_int(colorarray, ((faceset*)facesets[current_index])->packed_color);
        }
        else if (vertex.flags & 0x1000) {
          array_append_int(colorarray, vertex.packed_color);
        }
        else {
          array_append_int(colorarray, my_palette_lookup(state, vertex.color_name_index));
        }
      }
    }
  }
  else if (is_in_face && state->node->opcode == MORPH_VLIST_OPCODE) {
    uint32_t *ptr;
    n = prf_morph_vertex_list_get_count(state->node) * 2;
    ptr = prf_morph_vertex_list_get_list_ptr(state->node);
    for (i = 0; i < n; i += 2) {
      prf_model_vertex_palette_lookup((prf_model_t*)currmodel,
                                       ptr[i], &vertex);

      array_append_float(vertexarray, vertex.x);
      array_append_float(vertexarray, vertex.y);
      array_append_float(vertexarray, vertex.z);

      if (vertex.has_texture) {
        array_append_float(texcoordarray, vertex.texture[0]);
        array_append_float(texcoordarray, vertex.texture[1]);
      }
      else {
        array_append_float(texcoordarray, 0.0f);
        array_append_float(texcoordarray, 0.0f);
      }
      if (color_per_vertex) {
        if (vertex.flags & 0x2000) {
          array_append_int(colorarray, ((faceset*)facesets[current_index])->packed_color);
        }
        else if (vertex.flags & 0x1000) {
          array_append_int(colorarray, vertex.packed_color);
        }
        else {
          array_append_int(colorarray,my_palette_lookup(state, vertex.color_name_index));
        }
      }
    }
  }

  else if (is_in_face && prf_vertex_lookup(state->node, &vertex)) {
    array_append_float(vertexarray, vertex.x);
    array_append_float(vertexarray, vertex.y);
    array_append_float(vertexarray, vertex.z);

    array_append_float(texcoordarray, 0.0f);
    array_append_float(texcoordarray, 0.0f);
    if (color_per_vertex) {
      if (vertex.flags & 0x2000) {
        array_append_int(colorarray, ((faceset*)facesets[current_index])->packed_color);
      }
      else if (vertex.flags & 0x1000) {
        array_append_int(colorarray, vertex.packed_color);
      }
      else {
        array_append_int(colorarray, my_palette_lookup(state, vertex.color_name_index));
      }
    }
  }

  else if (is_in_face && state->node->opcode == POP_OPCODE) {
    float *mat;
    is_in_face = 0;

    mat = (float*) prf_state_get_matrix(state);

    if (array_count(vertexarray) == 1*3) {
      /* points not supported yet */
    }
    else if (array_count(vertexarray) == 2*3) {
#define idx ((faceset*)facesets[current_index])->lineIndices
#define bsp ((faceset*)facesets[current_index])->bspTree
#define colbsp ((faceset*)facesets[current_index])->vertex_colors
#define colidx ((faceset*)facesets[current_index])->colorIdx

      get_coords_3d(vertexarray, 0, tmp);
      mult_matrix_vec(mat, tmp);
      array_append_int(idx, bsp_add_point(bsp, tmp));
      get_coords_3d(vertexarray, 1, tmp);
      mult_matrix_vec(mat, tmp);
      array_append_int(idx, bsp_add_point(bsp, tmp));
      array_append_int(idx, -1);

      if (color_per_vertex) {
        packed_to_vec(colorarray[0], tmp);
        array_append_int(colidx, bsp_add_point(colbsp, tmp));
        packed_to_vec(colorarray[1], tmp);
        array_append_int(colidx, bsp_add_point(colbsp, tmp));
        array_append_int(colidx, -1);
      }
#undef idx
#undef bsp
#undef colbsp
#undef colidx
    }
    else if (array_count(vertexarray) >= 3*3) {

      int dotex = ((faceset*)facesets[current_index])->texture_pattern_index >= 0;

      if (color_per_vertex) {
        assert(array_count(colorarray) == array_count(vertexarray)/3);
      }
      if (dotex) {
        /*
        assert(array_count(texcoordarray) == array_count(vertexarray));
        */
      }
#define idx ((faceset*)facesets[current_index])->indices
#define tidx ((faceset*)facesets[current_index])->texIndices
#define bsp ((faceset*)facesets[current_index])->bspTree
#define colbsp ((faceset*)facesets[current_index])->vertex_colors
#define colidx ((faceset*)facesets[current_index])->colorIdx
#define texcoords ((faceset*)facesets[current_index])->texcoords

      n = array_count(vertexarray) / 3;
      for (i = 0; i < n; i++) {
        get_coords_3d(vertexarray, i ,tmp);
        mult_matrix_vec(mat, tmp);
        array_append_int(idx, bsp_add_point(bsp, tmp));
        if (dotex) {
          get_coords_2d(texcoordarray, i, tmp);
          array_append_int(tidx, bsp_add_point(texcoords, tmp));

        }
        if (color_per_vertex) {
          packed_to_vec(colorarray[i], tmp);
          array_append_int(colidx, bsp_add_point(colbsp, tmp));
        }
      }
      array_append_int(idx, -1);
      if (dotex) array_append_int(tidx, -1);
      if (color_per_vertex) array_append_int(colidx, -1);
    }
#undef idx
#undef tidx
#undef bsp
#undef colbsp
#undef colidx
#undef texcoords
  }
  return PRF_TRAV_CONTINUE;
}
