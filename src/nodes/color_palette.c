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

#include <profit/nodes/color_palette.h>
#include <profit/basic_funcs.h>
#include <profit/types.h>
#include <profit/model.h>
#include <profit/state.h>
#include <profit/node.h>
#include <profit/nodeinfo.h>
#include <profit/debug.h>
#include <profit/util/mempool.h>
#include <profit/util/bfile.h>
#include <config.h>

#include <stdlib.h>
#include <assert.h>
#include <stdio.h>

/**************************************************************************/

static prf_nodeinfo_t prf_color_palette_info = {
    32, PRF_ANCILLARY,
    "Color Palette",
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL
}; /* struct prf_color_palette_info */

/**************************************************************************/


typedef struct prf_color_palette_data node_data;

#define NODE_DATA_PAD 0

/**************************************************************************/

static
bool_t
prf_color_palette_load_f(
    prf_node_t * node,
    prf_state_t * state,
    bfile_t * bfile )
{
    int pos;

    assert( node != NULL && state != NULL && bfile != NULL );

    node->opcode = bf_get_uint16_be( bfile );
    if ( node->opcode != prf_color_palette_info.opcode ) {
        prf_error( 9, "tried color palette loader for other node type" );
        bf_rewind( bfile, 2 );
        return FALSE;
    }

    node->length = bf_get_uint16_be( bfile );

    if ( node->data == NULL ) { /* not pre-allocated */
        assert( state->model != NULL );
        if ( state->model->mempool_id == 0 )
            node->data = (uint8_t *)malloc( node->length - 4 + NODE_DATA_PAD );
        else
            node->data = (uint8_t *)pool_malloc( state->model->mempool_id,
                node->length - 4 + NODE_DATA_PAD );
        if ( node->data == NULL ) {
            prf_error( 6, "malloc returned NULL" );
            bf_rewind( bfile, 4 );
            return FALSE;
        }
    }

    pos = 4;
    do {
        int i;
        uint8_t * ptr;
        node_data * data;
        data = (node_data *) node->data;
        if ( node->length < (pos + 128) ) break;
        bf_read( bfile, (uint8_t *) data->reserved1, 128 ); pos += 128;
        for ( i = 0; i < 1024 && (pos + 4) <= node->length; i++ ) {
            data->brightest_rgb[i] = bf_get_uint32_be( bfile ); pos += 4;
#if 0
	    fprintf(stderr, "palette load (%d): 0x%x\n",
		    i, data->brightest_rgb[i]);
#endif
        }

        if ( node->length < (pos + 4) ) break;
        data->num_color_names = bf_get_uint32_be( bfile ); pos += 4;
        ptr = node->data + sizeof( node_data );

        i = 0;
        while ( i < data->num_color_names ) {
            struct prf_color_name_data * cdata;
            cdata = (struct prf_color_name_data *) ptr;
            cdata->length = bf_get_uint16_be( bfile );
            cdata->reserved1 = bf_get_uint16_be( bfile );
            cdata->index = bf_get_uint16_be( bfile );
            cdata->reserved2 = bf_get_uint16_be( bfile );
            bf_read( bfile, (uint8_t *) cdata->name, cdata->length - 8 );
            pos += cdata->length;
            ptr += cdata->length;
            i++;
        }
    } while ( FALSE );

    if ( pos < node->length )
        pos += bf_read( bfile, node->data + pos - 4 + NODE_DATA_PAD,
            node->length - pos );

    return TRUE;
} /* prf_color_palette_load_f() */

/**************************************************************************/

static
bool_t
prf_color_palette_save_f(
    prf_node_t * node,
    prf_state_t * state,
    bfile_t * bfile )
{
    int pos;

    assert( node != NULL && state != NULL && bfile != NULL );

    if ( node->opcode != prf_color_palette_info.opcode ) {
        prf_error( 3, "tried color palette save method on other node" );
        return FALSE;
    }

    bf_put_uint16_be( bfile, node->opcode );
    bf_put_uint16_be( bfile, node->length );

    pos = 4;
    do {
        node_data * data;
        int num, i;
        uint8_t * ptr;
        if ( node->length < (pos + 128) ) break;
        data = (node_data *) node->data;
        bf_write( bfile, (uint8_t *) data->reserved1, 128 ); pos += 128;
        num = PRF_MIN( 1024, ((node->length - 132) / 4) );
        for ( i = 0; i < num; i++ ) {
            bf_put_uint32_be( bfile, data->brightest_rgb[i] ); pos += 4;
        }
        if ( node->length < (pos + 4) ) break;
        bf_put_uint32_be( bfile, data->num_color_names ); pos += 4;
        ptr = node->data + pos - 4;

        for ( i = 0; i < data->num_color_names; i++ ) {
            struct prf_color_name_data * cdata;
            cdata = (struct prf_color_name_data *) ptr;
            if ( node->length < (pos + cdata->length) ) break;
            bf_put_uint16_be( bfile, cdata->length );
            bf_put_uint16_be( bfile, cdata->reserved1 );
            bf_put_uint16_be( bfile, cdata->index );
            bf_put_uint16_be( bfile, cdata->reserved2 );
            bf_write( bfile, (uint8_t *) cdata->name, cdata->length - 8 );
            pos += cdata->length;
            ptr += cdata->length;
        }

    } while ( FALSE );

    if ( pos < node->length )
        pos += bf_write( bfile, node->data + pos - 4 + NODE_DATA_PAD,
            node->length - pos );

    return TRUE;
} /* prf_color_palette_save_f() */

/**************************************************************************/

static
void
prf_color_palette_entry_f(
    prf_node_t * node,
    prf_state_t * state )
{
    assert( node != NULL && state != NULL );

    if ( node->opcode != prf_color_palette_info.opcode ) {
        prf_error( 9,
            "color palette entry state method tried on node of type %d.",
            node->opcode );
        return;
    }
    if ( state->color_palette != NULL && state->color_palette != node ) {
        prf_error( 9,
            "color palette entry: a color palette is already registered" );
        return;
    }

    state->color_palette = node;
} /* prf_color_palette_entry_f() */

/**************************************************************************/

void
prf_color_palette_init(
    void )
{
  prf_color_palette_info.load_f=prf_color_palette_load_f;
  prf_color_palette_info.save_f=prf_color_palette_save_f;
  prf_color_palette_info.entry_f=prf_color_palette_entry_f;

  prf_nodeinfo_set( &prf_color_palette_info );
} /* prf_color_palette_info() */

/**************************************************************************/

/* default ARGB color palette values */

uint32_t prf_color_palette_default_values[ 1024 ] = {
    0xffffffff, 0xff000000, 0xffff0000, 0xff00ff00,
    0xff0000ff, 0xffffff00, 0xffff00ff, 0xff00ffff,
    0xff007fff, 0xff7fff00, 0xff00ff7f, 0xffff007f,
    0xffff7fff, 0xff7fffff, 0xffffff7f, 0xff007f7f,
    0xff7f7f00, 0xff7f007f, 0xff00407f, 0xff7f4000,
    0xff40007f, 0xff407f00, 0xff7f0040, 0xff007f40,
    0xff20de5a, 0xff2051de, 0xffafce16, 0xffce1638,
    0xffcf1616, 0xff16afce, 0xff1616ce, 0xffee9290,
    0xff340000, 0xff4f0000, 0xff6b0000, 0xff860000,
    0xffa10000, 0xffbc0000, 0xffd70000, 0xfff30000,
    0xfffe0f0f, 0xffff2a2a, 0xffff4545, 0xffff6060,
    0xfffe7c7c, 0xffff9797, 0xffffb2b2, 0xffffcdcd,
    0xffc1a3a3, 0xffc59f9f, 0xffc99b9b, 0xffcd9797,
    0xffd19393, 0xffd49090, 0xffd88c8c, 0xffdc8888,
    0xffe08484, 0xffe48080, 0xffe87c7c, 0xffeb7979,
    0xffef7575, 0xfff37171, 0xfff76d6d, 0xfffb6969,
    0xff34000a, 0xff4f000f, 0xff6b0015, 0xff86001a,
    0xffa10020, 0xffbc0025, 0xffd7002b, 0xfff30030,
    0xfffe0f3f, 0xffff2a55, 0xffff456a, 0xffff6080,
    0xfffe7c96, 0xffff97ac, 0xffffb2c1, 0xffffcdd7,
    0xffc1a3a9, 0xffc59fa7, 0xffc99ba4, 0xffcd97a2,
    0xffd193a0, 0xffd4909d, 0xffd88c9b, 0xffdc8899,
    0xffe08496, 0xffe48094, 0xffe87c92, 0xffeb7990,
    0xffef758d, 0xfff3718b, 0xfff76d89, 0xfffb6986,
    0xff340015, 0xff4f001f, 0xff6b002a, 0xff860035,
    0xffa10040, 0xffbc004b, 0xffd70056, 0xfff30061,
    0xfffe0f6f, 0xffff2a7f, 0xffff458f, 0xffff60a0,
    0xfffe7cb0, 0xffff97c0, 0xffffb2d1, 0xffffcde1,
    0xffc1a3af, 0xffc59fae, 0xffc99bad, 0xffcd97ad,
    0xffd193ac, 0xffd490ab, 0xffd88caa, 0xffdc88aa,
    0xffe084a9, 0xffe480a8, 0xffe87ca7, 0xffeb79a7,
    0xffef75a6, 0xfff371a5, 0xfff76da4, 0xfffb69a3,
    0xff34001f, 0xff4f002f, 0xff6b0040, 0xff860050,
    0xffa10060, 0xffbc0071, 0xffd70081, 0xfff30091,
    0xfffe0f9f, 0xffff2aaa, 0xffff45b4, 0xffff60bf,
    0xfffe7cca, 0xffff97d5, 0xffffb2e0, 0xffffcdeb,
    0xffc1a3b5, 0xffc59fb6, 0xffc99bb7, 0xffcd97b7,
    0xffd193b8, 0xffd490b9, 0xffd88cba, 0xffdc88ba,
    0xffe084bb, 0xffe480bc, 0xffe87cbd, 0xffeb79bd,
    0xffef75be, 0xfff371bf, 0xfff76dc0, 0xfffb69c1,
    0xff34002a, 0xff4f003f, 0xff6b0055, 0xff86006b,
    0xffa10081, 0xffbc0096, 0xffd700ac, 0xfff300c2,
    0xfffe0fcf, 0xffff2ad4, 0xffff45d9, 0xffff60df,
    0xfffe7ce4, 0xffff97ea, 0xffffb2ef, 0xffffcdf5,
    0xffc1a3bb, 0xffc59fbd, 0xffc99bc0, 0xffcd97c2,
    0xffd193c4, 0xffd490c7, 0xffd88cc9, 0xffdc88cb,
    0xffe084ce, 0xffe480d0, 0xffe87cd2, 0xffeb79d4,
    0xffef75d7, 0xfff371d9, 0xfff76ddb, 0xfffb69de,
    0xff340034, 0xff4f004f, 0xff6b006b, 0xff860086,
    0xffa100a1, 0xffbc00bc, 0xffd700d7, 0xfff300f3,
    0xfffe0ffe, 0xffff2aff, 0xffff45ff, 0xffff60ff,
    0xfffe7cfe, 0xffff97ff, 0xffffb2ff, 0xffffcdff,
    0xffc1a3c1, 0xffc59fc5, 0xffc99bc9, 0xffcd97cd,
    0xffd193d1, 0xffd490d4, 0xffd88cd8, 0xffdc88dc,
    0xffe084e0, 0xffe480e4, 0xffe87ce8, 0xffeb79eb,
    0xffef75ef, 0xfff371f3, 0xfff76df7, 0xfffb69fb,
    0xff2a0034, 0xff3f004f, 0xff55006b, 0xff6b0086,
    0xff8100a1, 0xff9600bc, 0xffac00d7, 0xffc200f3,
    0xffcf0ffe, 0xffd42aff, 0xffd945ff, 0xffdf60ff,
    0xffe47cfe, 0xffea97ff, 0xffefb2ff, 0xfff5cdff,
    0xffbba3c1, 0xffbd9fc5, 0xffc09bc9, 0xffc297cd,
    0xffc493d1, 0xffc790d4, 0xffc98cd8, 0xffcb88dc,
    0xffce84e0, 0xffd080e4, 0xffd27ce8, 0xffd479eb,
    0xffd775ef, 0xffd971f3, 0xffdb6df7, 0xffde69fb,
    0xff1f0034, 0xff2f004f, 0xff40006b, 0xff500086,
    0xff6000a1, 0xff7100bc, 0xff8100d7, 0xff9100f3,
    0xff9f0ffe, 0xffaa2aff, 0xffb445ff, 0xffbf60ff,
    0xffca7cfe, 0xffd597ff, 0xffe0b2ff, 0xffebcdff,
    0xffb5a3c1, 0xffb69fc5, 0xffb79bc9, 0xffb797cd,
    0xffb893d1, 0xffb990d4, 0xffba8cd8, 0xffba88dc,
    0xffbb84e0, 0xffbc80e4, 0xffbd7ce8, 0xffbd79eb,
    0xffbe75ef, 0xffbf71f3, 0xffc06df7, 0xffc169fb,
    0xff150034, 0xff1f004f, 0xff2a006b, 0xff350086,
    0xff4000a1, 0xff4b00bc, 0xff5600d7, 0xff6100f3,
    0xff6f0ffe, 0xff7f2aff, 0xff8f45ff, 0xffa060ff,
    0xffb07cfe, 0xffc097ff, 0xffd1b2ff, 0xffe1cdff,
    0xffafa3c1, 0xffae9fc5, 0xffad9bc9, 0xffad97cd,
    0xffac93d1, 0xffab90d4, 0xffaa8cd8, 0xffaa88dc,
    0xffa984e0, 0xffa880e4, 0xffa77ce8, 0xffa779eb,
    0xffa675ef, 0xffa571f3, 0xffa46df7, 0xffa369fb,
    0xff0a0034, 0xff0f004f, 0xff15006b, 0xff1a0086,
    0xff2000a1, 0xff2500bc, 0xff2b00d7, 0xff3000f3,
    0xff3f0ffe, 0xff552aff, 0xff6a45ff, 0xff8060ff,
    0xff967cfe, 0xffac97ff, 0xffc1b2ff, 0xffd7cdff,
    0xffa9a3c1, 0xffa79fc5, 0xffa49bc9, 0xffa297cd,
    0xffa093d1, 0xff9d90d4, 0xff9b8cd8, 0xff9988dc,
    0xff9684e0, 0xff9480e4, 0xff927ce8, 0xff9079eb,
    0xff8d75ef, 0xff8b71f3, 0xff896df7, 0xff8669fb,
    0xff000034, 0xff00004f, 0xff00006b, 0xff000086,
    0xff0000a1, 0xff0000bc, 0xff0000d7, 0xff0000f3,
    0xff0f0ffe, 0xff2a2aff, 0xff4545ff, 0xff6060ff,
    0xff7c7cfe, 0xff9797ff, 0xffb2b2ff, 0xffcdcdff,
    0xffa3a3c1, 0xff9f9fc5, 0xff9b9bc9, 0xff9797cd,
    0xff9393d1, 0xff9090d4, 0xff8c8cd8, 0xff8888dc,
    0xff8484e0, 0xff8080e4, 0xff7c7ce8, 0xff7979eb,
    0xff7575ef, 0xff7171f3, 0xff6d6df7, 0xff6969fb,
    0xff000a34, 0xff000f4f, 0xff00156b, 0xff001a86,
    0xff0020a1, 0xff0025bc, 0xff002bd7, 0xff0030f3,
    0xff0f3ffe, 0xff2a55ff, 0xff456aff, 0xff6080ff,
    0xff7c96fe, 0xff97acff, 0xffb2c1ff, 0xffcdd7ff,
    0xffa3a9c1, 0xff9fa7c5, 0xff9ba4c9, 0xff97a2cd,
    0xff93a0d1, 0xff909dd4, 0xff8c9bd8, 0xff8899dc,
    0xff8496e0, 0xff8094e4, 0xff7c92e8, 0xff7990eb,
    0xff758def, 0xff718bf3, 0xff6d89f7, 0xff6986fb,
    0xff001534, 0xff001f4f, 0xff002a6b, 0xff003586,
    0xff0040a1, 0xff004bbc, 0xff0056d7, 0xff0061f3,
    0xff0f6ffe, 0xff2a7fff, 0xff458fff, 0xff60a0ff,
    0xff7cb0fe, 0xff97c0ff, 0xffb2d1ff, 0xffcde1ff,
    0xffa3afc1, 0xff9faec5, 0xff9badc9, 0xff97adcd,
    0xff93acd1, 0xff90abd4, 0xff8caad8, 0xff88aadc,
    0xff84a9e0, 0xff80a8e4, 0xff7ca7e8, 0xff79a7eb,
    0xff75a6ef, 0xff71a5f3, 0xff6da4f7, 0xff69a3fb,
    0xff001f34, 0xff002f4f, 0xff00406b, 0xff005086,
    0xff0060a1, 0xff0071bc, 0xff0081d7, 0xff0091f3,
    0xff0f9ffe, 0xff2aaaff, 0xff45b4ff, 0xff60bfff,
    0xff7ccafe, 0xff97d5ff, 0xffb2e0ff, 0xffcdebff,
    0xffa3b5c1, 0xff9fb6c5, 0xff9bb7c9, 0xff97b7cd,
    0xff93b8d1, 0xff90b9d4, 0xff8cbad8, 0xff88badc,
    0xff84bbe0, 0xff80bce4, 0xff7cbde8, 0xff79bdeb,
    0xff75beef, 0xff71bff3, 0xff6dc0f7, 0xff69c1fb,
    0xff002a34, 0xff003f4f, 0xff00556b, 0xff006b86,
    0xff0081a1, 0xff0096bc, 0xff00acd7, 0xff00c2f3,
    0xff0fcffe, 0xff2ad4ff, 0xff45d9ff, 0xff60dfff,
    0xff7ce4fe, 0xff97eaff, 0xffb2efff, 0xffcdf5ff,
    0xffa3bbc1, 0xff9fbdc5, 0xff9bc0c9, 0xff97c2cd,
    0xff93c4d1, 0xff90c7d4, 0xff8cc9d8, 0xff88cbdc,
    0xff84cee0, 0xff80d0e4, 0xff7cd2e8, 0xff79d4eb,
    0xff75d7ef, 0xff71d9f3, 0xff6ddbf7, 0xff69defb,
    0xff003434, 0xff004f4f, 0xff006b6b, 0xff008686,
    0xff00a1a1, 0xff00bcbc, 0xff00d7d7, 0xff00f3f3,
    0xff0ffefe, 0xff2affff, 0xff45ffff, 0xff60ffff,
    0xff7cfefe, 0xff97ffff, 0xffb2ffff, 0xffcdffff,
    0xffa3c1c1, 0xff9fc5c5, 0xff9bc9c9, 0xff97cdcd,
    0xff93d1d1, 0xff90d4d4, 0xff8cd8d8, 0xff88dcdc,
    0xff84e0e0, 0xff80e4e4, 0xff7ce8e8, 0xff79ebeb,
    0xff75efef, 0xff71f3f3, 0xff6df7f7, 0xff69fbfb,
    0xff00342a, 0xff004f3f, 0xff006b55, 0xff00866b,
    0xff00a181, 0xff00bc96, 0xff00d7ac, 0xff00f3c2,
    0xff0ffecf, 0xff2affd4, 0xff45ffd9, 0xff60ffdf,
    0xff7cfee4, 0xff97ffea, 0xffb2ffef, 0xffcdfff5,
    0xffa3c1bb, 0xff9fc5bd, 0xff9bc9c0, 0xff97cdc2,
    0xff93d1c4, 0xff90d4c7, 0xff8cd8c9, 0xff88dccb,
    0xff84e0ce, 0xff80e4d0, 0xff7ce8d2, 0xff79ebd4,
    0xff75efd7, 0xff71f3d9, 0xff6df7db, 0xff69fbde,
    0xff00341f, 0xff004f2f, 0xff006b40, 0xff008650,
    0xff00a160, 0xff00bc71, 0xff00d781, 0xff00f391,
    0xff0ffe9f, 0xff2affaa, 0xff45ffb4, 0xff60ffbf,
    0xff7cfeca, 0xff97ffd5, 0xffb2ffe0, 0xffcdffeb,
    0xffa3c1b5, 0xff9fc5b6, 0xff9bc9b7, 0xff97cdb7,
    0xff93d1b8, 0xff90d4b9, 0xff8cd8ba, 0xff88dcba,
    0xff84e0bb, 0xff80e4bc, 0xff7ce8bd, 0xff79ebbd,
    0xff75efbe, 0xff71f3bf, 0xff6df7c0, 0xff69fbc1,
    0xff003415, 0xff004f1f, 0xff006b2a, 0xff008635,
    0xff00a140, 0xff00bc4b, 0xff00d756, 0xff00f361,
    0xff0ffe6f, 0xff2aff7f, 0xff45ff8f, 0xff60ffa0,
    0xff7cfeb0, 0xff97ffc0, 0xffb2ffd1, 0xffcdffe1,
    0xffa3c1af, 0xff9fc5ae, 0xff9bc9ad, 0xff97cdad,
    0xff93d1ac, 0xff90d4ab, 0xff8cd8aa, 0xff88dcaa,
    0xff84e0a9, 0xff80e4a8, 0xff7ce8a7, 0xff79eba7,
    0xff75efa6, 0xff71f3a5, 0xff6df7a4, 0xff69fba3,
    0xff00340a, 0xff004f0f, 0xff006b15, 0xff00861a,
    0xff00a120, 0xff00bc25, 0xff00d72b, 0xff00f330,
    0xff0ffe3f, 0xff2aff55, 0xff45ff6a, 0xff60ff80,
    0xff7cfe96, 0xff97ffac, 0xffb2ffc1, 0xffcdffd7,
    0xffa3c1a9, 0xff9fc5a7, 0xff9bc9a4, 0xff97cda2,
    0xff93d1a0, 0xff90d49d, 0xff8cd89b, 0xff88dc99,
    0xff84e096, 0xff80e494, 0xff7ce892, 0xff79eb90,
    0xff75ef8d, 0xff71f38b, 0xff6df789, 0xff69fb86,
    0xff003400, 0xff004f00, 0xff006b00, 0xff008600,
    0xff00a100, 0xff00bc00, 0xff00d700, 0xff00f300,
    0xff0ffe0f, 0xff2aff2a, 0xff45ff45, 0xff60ff60,
    0xff7cfe7c, 0xff97ff97, 0xffb2ffb2, 0xffcdffcd,
    0xffa3c1a3, 0xff9fc59f, 0xff9bc99b, 0xff97cd97,
    0xff93d193, 0xff90d490, 0xff8cd88c, 0xff88dc88,
    0xff84e084, 0xff80e480, 0xff7ce87c, 0xff79eb79,
    0xff75ef75, 0xff71f371, 0xff6df76d, 0xff69fb69,
    0xff0a3400, 0xff0f4f00, 0xff156b00, 0xff1a8600,
    0xff20a100, 0xff25bc00, 0xff2bd700, 0xff30f300,
    0xff3ffe0f, 0xff55ff2a, 0xff6aff45, 0xff80ff60,
    0xff96fe7c, 0xffacff97, 0xffc1ffb2, 0xffd7ffcd,
    0xffa9c1a3, 0xffa7c59f, 0xffa4c99b, 0xffa2cd97,
    0xffa0d193, 0xff9dd490, 0xff9bd88c, 0xff99dc88,
    0xff96e084, 0xff94e480, 0xff92e87c, 0xff90eb79,
    0xff8def75, 0xff8bf371, 0xff89f76d, 0xff86fb69,
    0xff153400, 0xff1f4f00, 0xff2a6b00, 0xff358600,
    0xff40a100, 0xff4bbc00, 0xff56d700, 0xff61f300,
    0xff6ffe0f, 0xff7fff2a, 0xff8fff45, 0xffa0ff60,
    0xffb0fe7c, 0xffc0ff97, 0xffd1ffb2, 0xffe1ffcd,
    0xffafc1a3, 0xffaec59f, 0xffadc99b, 0xffadcd97,
    0xffacd193, 0xffabd490, 0xffaad88c, 0xffaadc88,
    0xffa9e084, 0xffa8e480, 0xffa7e87c, 0xffa7eb79,
    0xffa6ef75, 0xffa5f371, 0xffa4f76d, 0xffa3fb69,
    0xff1f3400, 0xff2f4f00, 0xff406b00, 0xff508600,
    0xff60a100, 0xff71bc00, 0xff81d700, 0xff91f300,
    0xff9ffe0f, 0xffaaff2a, 0xffb4ff45, 0xffbfff60,
    0xffcafe7c, 0xffd5ff97, 0xffe0ffb2, 0xffebffcd,
    0xffb5c1a3, 0xffb6c59f, 0xffb7c99b, 0xffb7cd97,
    0xffb8d193, 0xffb9d490, 0xffbad88c, 0xffbadc88,
    0xffbbe084, 0xffbce480, 0xffbde87c, 0xffbdeb79,
    0xffbeef75, 0xffbff371, 0xffc0f76d, 0xffc1fb69,
    0xff2a3400, 0xff3f4f00, 0xff556b00, 0xff6b8600,
    0xff81a100, 0xff96bc00, 0xffacd700, 0xffc2f300,
    0xffcffe0f, 0xffd4ff2a, 0xffd9ff45, 0xffdfff60,
    0xffe4fe7c, 0xffeaff97, 0xffefffb2, 0xfff5ffcd,
    0xffbbc1a3, 0xffbdc59f, 0xffc0c99b, 0xffc2cd97,
    0xffc4d193, 0xffc7d490, 0xffc9d88c, 0xffcbdc88,
    0xffcee084, 0xffd0e480, 0xffd2e87c, 0xffd4eb79,
    0xffd7ef75, 0xffd9f371, 0xffdbf76d, 0xffdefb69,
    0xff343400, 0xff4f4f00, 0xff6b6b00, 0xff868600,
    0xffa1a100, 0xffbcbc00, 0xffd7d700, 0xfff3f300,
    0xfffefe0f, 0xffffff2a, 0xffffff45, 0xffffff60,
    0xfffefe7c, 0xffffff97, 0xffffffb2, 0xffffffcd,
    0xffc1c1a3, 0xffc5c59f, 0xffc9c99b, 0xffcdcd97,
    0xffd1d193, 0xffd4d490, 0xffd8d88c, 0xffdcdc88,
    0xffe0e084, 0xffe4e480, 0xffe8e87c, 0xffebeb79,
    0xffefef75, 0xfff3f371, 0xfff7f76d, 0xfffbfb69,
    0xff342a00, 0xff4f3f00, 0xff6b5500, 0xff866b00,
    0xffa18100, 0xffbc9600, 0xffd7ac00, 0xfff3c200,
    0xfffecf0f, 0xffffd42a, 0xffffd945, 0xffffdf60,
    0xfffee47c, 0xffffea97, 0xffffefb2, 0xfffff5cd,
    0xffc1bba3, 0xffc5bd9f, 0xffc9c09b, 0xffcdc297,
    0xffd1c493, 0xffd4c790, 0xffd8c98c, 0xffdccb88,
    0xffe0ce84, 0xffe4d080, 0xffe8d27c, 0xffebd479,
    0xffefd775, 0xfff3d971, 0xfff7db6d, 0xfffbde69,
    0xff341f00, 0xff4f2f00, 0xff6b4000, 0xff865000,
    0xffa16000, 0xffbc7100, 0xffd78100, 0xfff39100,
    0xfffe9f0f, 0xffffaa2a, 0xffffb445, 0xffffbf60,
    0xfffeca7c, 0xffffd597, 0xffffe0b2, 0xffffebcd,
    0xffc1b5a3, 0xffc5b69f, 0xffc9b79b, 0xffcdb797,
    0xffd1b893, 0xffd4b990, 0xffd8ba8c, 0xffdcba88,
    0xffe0bb84, 0xffe4bc80, 0xffe8bd7c, 0xffebbd79,
    0xffefbe75, 0xfff3bf71, 0xfff7c06d, 0xfffbc169,
    0xff341500, 0xff4f1f00, 0xff6b2a00, 0xff863500,
    0xffa14000, 0xffbc4b00, 0xffd75600, 0xfff36100,
    0xfffe6f0f, 0xffff7f2a, 0xffff8f45, 0xffffa060,
    0xfffeb07c, 0xffffc097, 0xffffd1b2, 0xffffe1cd,
    0xffc1afa3, 0xffc5ae9f, 0xffc9ad9b, 0xffcdad97,
    0xffd1ac93, 0xffd4ab90, 0xffd8aa8c, 0xffdcaa88,
    0xffe0a984, 0xffe4a880, 0xffe8a77c, 0xffeba779,
    0xffefa675, 0xfff3a571, 0xfff7a46d, 0xfffba369,
    0xff340a00, 0xff4f0f00, 0xff6b1500, 0xff861a00,
    0xffa12000, 0xffbc2500, 0xffd72b00, 0xfff33000,
    0xfffe3f0f, 0xffff552a, 0xffff6a45, 0xffff8060,
    0xfffe967c, 0xffffac97, 0xffffc1b2, 0xffffd7cd,
    0xffc1a9a3, 0xffc5a79f, 0xffc9a49b, 0xffcda297,
    0xffd1a093, 0xffd49d90, 0xffd89b8c, 0xffdc9988,
    0xffe09684, 0xffe49480, 0xffe8927c, 0xffeb9079,
    0xffef8d75, 0xfff38b71, 0xfff7896d, 0xfffb8669,
    0xff340000, 0xff4f0000, 0xff6b0000, 0xff860000,
    0xffa10000, 0xffbc0000, 0xffd70000, 0xfff30000,
    0xfffe0f0f, 0xffff2a2a, 0xffff4545, 0xffff6060,
    0xfffe7c7c, 0xffff9797, 0xffffb2b2, 0xffffcdcd,
    0xffc1a3a3, 0xffc59f9f, 0xffc99b9b, 0xffcd9797,
    0xffd19393, 0xffd49090, 0xffd88c8c, 0xffdc8888,
    0xffe08484, 0xffe48080, 0xffe87c7c, 0xffeb7979,
    0xffef7575, 0xfff37171, 0xfff76d6d, 0xfffb6969
}; /* prf_color_palette_default_values[] */

/**************************************************************************/

/* $Id$ */

