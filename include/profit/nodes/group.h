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
