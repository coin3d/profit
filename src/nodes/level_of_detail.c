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

#include <profit/nodes/level_of_detail.h>
#include <profit/basics.h>
#include <profit/nodeinfo.h>
#include <profit/node.h>
#include <profit/model.h>
#include <profit/state.h>
#include <profit/debug.h>
#include <profit/util/bfile.h>
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

static prf_nodeinfo_t prf_level_of_detail_info = {
    73, PRF_PRIMARY,
    "Level-Of-Detail",
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL
}; /* struct prf_level_of_detail_info */

typedef struct prf_level_of_detail_data node_data;
#define  NODE_DATA_SIZE          68
#define  NODE_DATA_PAD           (sizeof(node_data)-NODE_DATA_SIZE)

static bool_t
prf_level_of_detail_load_f(prf_node_t * node,
                           prf_state_t * state,
                           bfile_t * bfile)
{
  int pos;

  assert(node != NULL && state != NULL && bfile != NULL);
  
  node->opcode = bf_get_uint16_be(bfile);
  if (node->opcode != prf_level_of_detail_info.opcode) {
    prf_error(9,
              "tried level of detail node load method on node of type %d.",
              node->opcode);
    bf_rewind(bfile, 2);
    return FALSE;
  }

  node->length = bf_get_uint16_be(bfile);
  if (node->length < NODE_DATA_SIZE + 4) {
    prf_error(9, "level of detail node too short (%d bytes)", node->length);
    bf_rewind(bfile, 4);
    return FALSE;
  }

  if (node->length > 4 && node->data == NULL) {
    assert(state->model != NULL);
    if (state->model->mempool_id == 0) {
      node->data = (uint8_t *)malloc(node->length - 4 + NODE_DATA_PAD);
    }
    else {
      node->data = (uint8_t *)pool_malloc(state->model->mempool_id,
                                          node->length - 4 + NODE_DATA_PAD);
    }
    if (node->data == NULL) {
      prf_error(9, "memory allocation problem (returned NULL)");
      bf_rewind(bfile, 4);
      return FALSE;
    }
  }

  pos = 4;
  do {
    node_data * data;
    data = (node_data *) node->data;
    bf_read(bfile, data->id, 8); pos += 8;
    data->reserved = bf_get_int32_be(bfile); pos += 4;
    data->switch_in_distance = bf_get_float64_be(bfile); pos += 8;
    data->switch_out_distance = bf_get_float64_be(bfile); pos += 8;
    data->special_effect_id1 = bf_get_int16_be(bfile); pos += 2;
    data->special_effect_id2 = bf_get_int16_be(bfile); pos += 2;
    data->flags = bf_get_int32_be(bfile); pos += 4;
    data->center_x = bf_get_float64_be(bfile); pos += 8;
    data->center_y = bf_get_float64_be(bfile); pos += 8;
    data->center_z = bf_get_float64_be(bfile); pos += 8;
    data->transition_range = bf_get_float64_be(bfile); pos += 8;
  } while (FALSE);

  if (node->length > pos) {
    pos += bf_read(bfile, node->data + pos - 4 + NODE_DATA_PAD,
                   node->length - pos);
  }
  return TRUE;
} /* prf_level_of_detail_load_f() */

/**************************************************************************/

static bool_t
prf_level_of_detail_save_f(prf_node_t * node,
                           prf_state_t * state,
                           bfile_t * bfile)
{
  int pos;

  assert(node != NULL && bfile != NULL);

  if (node->opcode != prf_level_of_detail_info.opcode) {
    prf_error(9, "tried level of detail save method on node of type %d.",
              node->opcode);
    return FALSE;
  }

  bf_put_uint16_be(bfile, node->opcode);
  bf_put_uint16_be(bfile, node->length);

  pos = 4;
  do {
    node_data * data;
    data = (node_data *) node->data;

    bf_write(bfile, data->id, 8); pos += 8;
    bf_put_int32_be(bfile, data->reserved); pos += 4;
    bf_put_float64_be(bfile, data->switch_in_distance); pos += 8;
    bf_put_float64_be(bfile, data->switch_out_distance); pos += 8;
    bf_put_int16_be(bfile, data->special_effect_id1); pos += 2;
    bf_put_int16_be(bfile, data->special_effect_id2); pos += 2;
    bf_put_int32_be(bfile, data->flags); pos += 4;
    bf_put_float64_be(bfile, data->center_x); pos += 8;
    bf_put_float64_be(bfile, data->center_y); pos += 8;
    bf_put_float64_be(bfile, data->center_z); pos += 8;
    bf_put_float64_be(bfile, data->transition_range); pos += 8;

  } while (FALSE);
  
  if (node->length > pos) {
    pos += bf_write(bfile, node->data + pos - 4 + NODE_DATA_PAD,
                    node->length - pos);
  }
  return TRUE;
} /* prf_level_of_detail_save_f() */

/**************************************************************************/

static void
prf_level_of_detail_entry_f(prf_node_t * node,
                            prf_state_t * state)
{
  assert((node != NULL) && (state != NULL));

  if (node->opcode != prf_level_of_detail_info.opcode) {
    prf_error(9, "level of detail entry state method on node of type %d.",
              node->opcode);
    return;
  }
  /* do nothing ? */
} /* prf_level_of_detail_entry_f() */

/**************************************************************************/

void
prf_level_of_detail_init(void)
{
  prf_level_of_detail_info.load_f = prf_level_of_detail_load_f;
  prf_level_of_detail_info.save_f = prf_level_of_detail_save_f;
  prf_level_of_detail_info.entry_f = prf_level_of_detail_entry_f;
  prf_nodeinfo_set(&prf_level_of_detail_info);
} /* prf_level_of_detail_init() */

