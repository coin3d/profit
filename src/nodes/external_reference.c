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

#include <profit/nodes/external_reference.h>
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

static prf_nodeinfo_t prf_external_reference_info = {
    63, PRF_PRIMARY|PRF_REFERENCE,
    "External Reference",
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL
}; /* struct prf_external_reference_info */

typedef struct prf_external_reference_data node_data;
#define  NODE_DATA_SIZE          208
#define  NODE_DATA_PAD           (sizeof(node_data)-NODE_DATA_SIZE)

static
bool_t
prf_external_reference_load_f(prf_node_t * node,
                              prf_state_t * state,
                              bfile_t * bfile)
{
  int pos;

  assert(node != NULL && state != NULL && bfile != NULL);
  
  node->opcode = bf_get_uint16_be(bfile);
  if (node->opcode != prf_external_reference_info.opcode) {
    prf_error(9,
              "tried external reference node load method on node of type %d.",
              node->opcode);
    bf_rewind(bfile, 2);
    return FALSE;
  }

  node->length = bf_get_uint16_be(bfile);
  if (node->length < NODE_DATA_SIZE + 4) {
    prf_error(9, "external reference node too short (%d bytes)", node->length);
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
    bf_read(bfile, data->filename, 200); pos += 200;
    data->reserved1 = bf_get_int8(bfile); pos += 1;
    data->reserved2 = bf_get_int8(bfile); pos += 1;
    data->reserved3[0] = bf_get_int16_be(bfile); pos += 2;
    data->reserved3[1] = bf_get_int16_be(bfile); pos += 2;
    data->flags = bf_get_int32_be(bfile); pos += 4;
  } while (FALSE);

  if (node->length > pos)
    pos += bf_read(bfile, node->data + pos - 4 + NODE_DATA_PAD,
                   node->length - pos);

  return TRUE;
} /* prf_external_reference_load_f() */

/**************************************************************************/

static bool_t
prf_external_reference_save_f(prf_node_t * node,
                              prf_state_t * state,
                              bfile_t * bfile)
{
  int pos;

  assert(node != NULL && bfile != NULL);

  if (node->opcode != prf_external_reference_info.opcode) {
    prf_error(9, "tried external reference save method on node of type %d.",
              node->opcode);
    return FALSE;
  }

  bf_put_uint16_be(bfile, node->opcode);
  bf_put_uint16_be(bfile, node->length);

  pos = 4;
  do {
    node_data * data;
    data = (node_data *) node->data;

    bf_write(bfile, data->filename, 200); pos += 200;
    bf_put_int8(bfile, data->reserved1); pos += 1;
    bf_put_int8(bfile, data->reserved2); pos += 1;
    bf_put_int16_be(bfile, data->reserved3[0]); pos += 2;
    bf_put_int16_be(bfile, data->reserved3[1]); pos += 2;
    bf_put_int32_be(bfile, data->flags); pos += 4;
  } while (FALSE);

  if (node->length > pos)
    pos += bf_write(bfile, node->data + pos - 4 + NODE_DATA_PAD,
                    node->length - pos);

  return TRUE;
} /* prf_external_reference_save_f() */

/**************************************************************************/

static void
prf_external_reference_entry_f(prf_node_t * node,
                               prf_state_t * state)
{
  assert((node != NULL) && (state != NULL));

  if (node->opcode != prf_external_reference_info.opcode) {
    prf_error(9, "external reference entry state method on node of type %d.",
              node->opcode);
    return;
  }
  /* do nothing ? */
} /* prf_external_reference_entry_f() */

/**************************************************************************/

void
prf_external_reference_init(void)
{
  prf_external_reference_info.load_f = prf_external_reference_load_f;
  prf_external_reference_info.save_f = prf_external_reference_save_f;
  prf_external_reference_info.entry_f = prf_external_reference_entry_f;
  prf_nodeinfo_set(&prf_external_reference_info);
} /* prf_external_reference_init() */
