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

#ifndef PRF_MESSAGES_H
#define PRF_MESSAGES_H

#include <stdarg.h>

#include <profit/basics.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

PROFIT_API  void prf_messages_init( void );
PROFIT_API  void prf_messages_exit( void );

PROFIT_API  void prf_messages_post( int type, int level, ... );
PROFIT_API  void prf_messages_post_va( int type, int level, va_list args );

PROFIT_API  void prf_messages_add_handler( int type, int level,
                void (*func)( int, int, const char *, void * ), void * );

PROFIT_API  void prf_messages_del_handler( int type, int level,
                void (*func)(int, int, const char *, void *), void * );

#define PRF_MSG_INFO           1
#define PRF_MSG_DEBUG          2
#define PRF_MSG_WARNING        3
#define PRF_MSG_ERROR          4
#define PRF_MSG_FATAL_ERROR    5

#ifdef __cplusplus
}; /* extern "C" */
#endif /* __cplusplus */

#endif /* ! PRF_MESSAGES_H */
