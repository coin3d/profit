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

#ifndef PRF_GROUP_NODE_H
#define PRF_GROUP_NODE_H

#include <profit/basics.h>
#include <profit/profit.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

struct prf_group_data {
    char     ascii_id[ 8 ];
    int16_t  relative_priority;
    int16_t  reserved1;
    uint32_t flags;
    int16_t  special_effect_id1;
    int16_t  special_effect_id2;
    int16_t  significance;
    int8_t   layer_code;
    int8_t   reserved2;
    int32_t  reserved3;
}; /* struct prf_group_data */

#define PRF_GROUP_FLAG_FORWARD_ANIMATION (1<<30)
#define PRF_GROUP_FLAG_SWING_ANIMATION (1<<29)
#define PRF_GROUP_FLAG_ANIMATION (PRF_GROUP_FLAG_FORWARD_ANIMATION|PRF_GROUP_FLAG_SWING_ANIMATION)

#ifdef __cplusplus
}; /* extern "C" */
#endif /* __cplusplus */

#endif /* ! PRF_GROUP_NODE_H */
