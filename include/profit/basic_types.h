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

#ifndef PROFIT_BASIC_TYPES_H
#define PROFIT_BASIC_TYPES_H

#include <sys/types.h>

#ifdef __sgi
#define HAVE_INTTYPES
#define HAVE_UINTTYPES
#endif

#ifdef __BIT_TYPES_DEFINED__
#define HAVE_INTTYPES
#endif /* __BIT_TYPES_DEFINES__ */

#ifndef HAVE_INTTYPES
#define HAVE_INTTYPES
typedef  signed char       int8_t;
typedef  signed short      int16_t;
typedef  signed int        int32_t;
#endif /* ! HAVE_INTTYPES */

#ifndef HAVE_UINTTYPES
#define HAVE_UINTTYPES
typedef  unsigned char     uint8_t;
typedef  unsigned short    uint16_t;
typedef  unsigned int      uint32_t;
#endif /* ! HAVE_UINTTYPES */

typedef  float             float32_t;
typedef  double            float64_t;

typedef  int               bool_t;

#ifndef FALSE
#define FALSE              (0)
#define TRUE               (!FALSE)
#endif /* ! FALSE */

#ifndef NULL
#define NULL               (0)
#endif /* ! NULL */

#endif /* ! PROFIT_BASIC_TYPES_H */

/* $Id$ */

