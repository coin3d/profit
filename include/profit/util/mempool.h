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

#ifndef PRF_MEMPOOL_H
#define PRF_MEMPOOL_H

#include <profit/basics.h>
#include <profit/profit.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef  uint16_t  pool_t;

PROFIT_API  void    pool_system_init( void );
PROFIT_API  void    pool_system_exit( void );

PROFIT_API  pool_t  pool_create( void );
PROFIT_API  pool_t  pool_create_sized( int block_size );

PROFIT_API  void    pool_destroy( pool_t pool_id );
PROFIT_API  void    pool_destroy_all( void );

PROFIT_API  void *  pool_malloc( pool_t pool_id, int size );

#ifdef __cplusplus
}; /* extern "C" */
#endif /* __cplusplus */

#endif /* ! PRF_MEMPOOL_H */
