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

#include <profit/nodes/replicate.h>
#include <config.h>

/**************************************************************************/

static const nodeinfo_t replicate_info = {
    60, ANCILLARY | REFERENCE,
    "Replicate",
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL
}; /* struct replicate_info */

/**************************************************************************/

#if 0 /* not in use */

static
bool_t
replicate_load_f(
    node_t * node,
    state_t * state,
    bfile_t * bfile )
{
} /* replicate_load_f() */

/**************************************************************************/

static
bool_t
replicate_save_f(
    node_t * node,
    state_t * state,
    bfile_t * bfile )
{
} /* replicate_save_f() */

#endif /* 0 - not in use */

/**************************************************************************/

void
replicate_init(
    void )
{
    set_nodeinfo( &replicate_info );
} /* replicate_init() */

/**************************************************************************/
