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

#ifndef CALLBACK_H
#define CALLBACK_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**************************************************************************
   cb_call() will call the callback with the system data as the first
   argument and the user data as the second argument.  The hierarchy
   traversal can be controlled to some degree by using various callback
   return values.
 **************************************************************************/

typedef  struct prf_cb_s  prf_cb_t;

struct prf_cb_s {
    int (*func)( void * sysdata, void * userdata );
    void * data;
};

#define  PRF_TRAV_EXIT           (0)
#define  PRF_TRAV_CONTINUE       (1)
#define  PRF_TRAV_SIBLING        (2)
/* trav_sibling only means something for in-order traversal,
   and causes the node's children to be skipped. */
#define  PRF_TRAV_POP            (3)
/* trav_pop means that nodes between this node and the next pop will
   not be traversed (but the pop will) - not implemented yet -
   can be used to avoid traversing extensions and attributes  */
#define  PRF_TRAV_UP             (4)
/* trav_up is used to skip the rest of the siblings to this node, and go
   back up to the node's parent.  */
#define  PRF_TRAV_TRAVERSE       (5)
/* trav_traverse is used to traverse into the internals of container
   nodes. */

#define prf_cb_func( cb, function )            \
    cb.func = function

#define prf_cb_data( cb, userdata )            \
    cb.data = userdata

#define prf_cb_set( cb, function, userdata )   \
    prf_cb_func( cb, function );               \
    prf_cb_data( cb, userdata )

#define prf_cb_call( cb, sysdata )             \
    (*cb.func)( sysdata, cb.data )

#ifdef __cplusplus
}; /* extern "C" */
#endif /* __cplusplus */

#endif /* ! CALLBACK_H */

/* $Id$ */

