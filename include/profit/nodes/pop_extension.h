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

#ifndef PRF_POP_EXTENSION_NODE_H
#define PRF_POP_EXTENSION_NODE_H

#include <profit/basic_types.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

struct prf_pop_extension_data {
    char reserved[ 18 ];              /* wouldn't want it any other way... */
    uint16_t vertex_reference_index;  /* -1 (!!!) if not vertex extension */
}; /* struct prf_pop_extension_data */

void prf_pop_extension_init( void );

#ifdef __cplusplus
}; /* extern "C" */
#endif /* __cplusplus */

#endif /* ! PRF_POP_EXTENSION_NODE_H */
