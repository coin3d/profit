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

#include <profit/util/bfile.h>
#include <profit/basics.h>
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/stat.h>
#include <ctype.h>
#ifdef HAVE_STRING_H
#include <string.h>
#endif

/*
  DEPENDENCIES
  - depends on WORDS_BIGENDIAN being defined [in <profit/config.h>] for
    bigendian architectures

  TODO soon
  - double buffer (n-buffer) reading for real peek support
  - double/n-buffer for real backup/rewind support
  - fill in more of the peek functions

  TODO the-day-after-tomorrow
  - implement seeking
  - optionally spawn read-ahead thread (on systems that support it)

  TODO when-I-have-nothing-else-better-to-do
  - add support for int64 and float128 datatypes
*/

/**************************************************************************/

struct bfile_s {
    uint16_t    flags;
    FILE *      file;
    size_t      size;
    size_t      pos;
    size_t      ipos; /* internal position */
    char *      filename;

    uint8_t *   buffer;
    uint32_t    buffer_size;
    int         (*progress_cb)( float, void *);
    void        *progress_data;
}; /* struct bfile_s */

/* flags */
#define BF_OPEN      0x0001
#define BF_EOF       0x0002
#define BF_READABLE  0x0100
#define BF_WRITABLE  0x0200
#define BF_MEMBUFFER 0x0400

/* defines */
#define BFILE_BLOCK_SIZE (4*1024)

static void bf_read_buffer( bfile_t * bfile );
static void bf_write_buffer( bfile_t * bfile );

/**************************************************************************/

static
bfile_t *
create_bfile_t(
    uint32_t buffer_size )
{
    bfile_t * bfile;
    bfile = (bfile_t *)malloc( sizeof( struct bfile_s ) );
    assert( bfile != NULL );
    bfile->flags = 0;
    bfile->file = NULL;
    bfile->size = 0;
    bfile->pos = 0;
    bfile->ipos = 0;
    bfile->filename = NULL;
    if ( buffer_size ) {
      bfile->buffer = (uint8_t *)malloc( buffer_size );
      assert( bfile->buffer != NULL );
      bfile->buffer_size = buffer_size;
    }
    else {
      bfile->buffer = NULL;
      bfile->buffer_size = 0;
    }
    bfile->progress_cb = NULL;
    bfile->progress_data = NULL;
    return bfile;
} /* create_bfile_t() */

static
void
destroy_bfile_t(
    bfile_t * bfile )
{
  if ( bfile != NULL ) {
    if ( bfile->filename != NULL ) {
      free( bfile->filename );
      bfile->filename = NULL;
    }
    if ( (bfile->buffer != NULL) && !(bfile->flags & BF_MEMBUFFER)) {
      free( bfile->buffer );
      bfile->buffer = NULL;
    }
    free( bfile );
  }
} /* destroy_bf_t() */

/**************************************************************************/

bfile_t  *
bf_create_m(
    const uint8_t * buffer,
    unsigned int len )
{
    bfile_t * bfile;
    bfile = create_bfile_t( 0 );
    bfile->buffer = (uint8_t*) buffer;
    bfile->buffer_size = bfile->size = len;
    bfile->flags |= BF_MEMBUFFER | BF_READABLE;
    bfile->ipos = len;
    return bfile;
}

bfile_t *
bf_create_r(
    const char * filename )
{
    bfile_t * bfile;
    FILE * file;
    struct stat buf;

    bfile = NULL;
    file = fopen( filename, "rb" );
    if ( file ) {
        bfile = create_bfile_t( BFILE_BLOCK_SIZE );
        bfile->file = file;
        bfile->filename = (char *)malloc( strlen( filename ) + 1 );
        assert( bfile->filename );
        strcpy( bfile->filename, filename );
        stat( filename, &buf );
        bfile->size = buf.st_size;
        bfile->flags |= BF_READABLE;
    }

    return bfile;
} /* bf_create_r() */

/**************************************************************************/

bfile_t *
bf_create_w(
    const char * filename )
{
    FILE * file;
    bfile_t * bfile;

    bfile = NULL;
    file = fopen( filename, "wb" );
    if ( file != NULL ) {
        bfile = create_bfile_t( BFILE_BLOCK_SIZE );
        bfile->file = file;
        bfile->filename = (char *)malloc( strlen( filename ) + 1 );
        assert( bfile->filename );
        strcpy( bfile->filename, filename );
        bfile->flags |= BF_WRITABLE;
    }

    return bfile;
} /* bf_create_w() */

/**************************************************************************/

void
bf_destroy(
    bfile_t * bfile )
{
    if ( bfile == NULL ) /* "destroy" bfiles that was never created */
        return;

    if ( (bfile->flags & BF_WRITABLE) != FALSE )
        /* write - must write unwritten data */
        bf_write_buffer( bfile );

    if ( bfile->file ) fclose( bfile->file );
    
    destroy_bfile_t( bfile );
} /* bf_destroy() */

/**************************************************************************/

void
bf_set_progress_cb(
    bfile_t * bfile,
    int (*func)( float, void * ),
    void * data )
{
    bfile->progress_cb = func;
    bfile->progress_data = data;
} /* bf_set_progress_cb() */

/**************************************************************************/

static
void
bf_read_buffer(
    bfile_t * bfile )
{
    unsigned int bytes;

    if ( bfile->progress_cb ) {
        bfile->progress_cb( (float)bfile->ipos / (float)bfile->size,
            bfile->progress_data);
    }

    if ( bfile->ipos == bfile->size )
        /* file completely read */
        return;

    bytes = 0;
    while ( (bytes < bfile->buffer_size) && (bfile->ipos < bfile->size) ) {
        int b;
        b = fread( bfile->buffer + bytes, 1, bfile->buffer_size - bytes,
                   bfile->file );
        if ( b > 0 ) {
            bytes += b;
            bfile->ipos += b;
        } else {
            fprintf( stderr, "Error: fread() returned 0.\n" );
        }
    }
} /* bf_read_buffer() */

/**************************************************************************/

static
void
bf_write_buffer(
    bfile_t * bfile )
{
    int written, towrite;

    assert( bfile != NULL );
    assert( bfile->file != NULL );
    assert( (bfile->flags & BF_WRITABLE) != FALSE );

    towrite = bfile->pos - bfile->ipos;
    written = 0;
    while ( written < towrite ) {
        int res, bufpos;
        bufpos = bfile->ipos % bfile->buffer_size;
        res = fwrite( bfile->buffer + bufpos + written, 1, 
                      towrite - written, bfile->file );
        if ( res > 0 ) {
            written += res;
        } else {
            fprintf( stderr, "Error: fwrite() returned 0.\n" );
        }
    }

    bfile->ipos += written;
} /* bf_write_buffer() */

/**************************************************************************/

int
bf_read(
    bfile_t * bfile,
    uint8_t * buffer,
    unsigned int len )
{
    unsigned int toread, hasread;

    assert( bfile != NULL );
    assert( (bfile->flags & BF_READABLE) != FALSE );

    hasread = 0;
    toread = PRF_MIN( len, bfile->size - bfile->pos ); /* crop read length */
    while ( hasread < toread ) {
        if ( bfile->pos < bfile->ipos ) {
            int bufoff;
            int cpylen;
            bufoff = bfile->pos % bfile->buffer_size;
            cpylen = PRF_MIN( bfile->buffer_size - bufoff, toread - hasread );
            memcpy( buffer + hasread, bfile->buffer + bufoff, cpylen );
            hasread += cpylen;
            bfile->pos += cpylen;
        } else {
            bf_read_buffer( bfile );
        }
    }

    return hasread;
} /* bf_read() */

/**************************************************************************/

int
bf_write(
    bfile_t * bfile,
    uint8_t * buffer,
    unsigned int len )
{
    unsigned int haswritten, towrite;

    assert( bfile != NULL );
    assert( (bfile->flags & BF_WRITABLE) != FALSE );

    haswritten = 0;
    towrite = len;
    while ( haswritten < towrite ) {
        if ( (bfile->pos - bfile->ipos) < bfile->buffer_size ) {
            int bufoff, cpylen;
            bufoff = bfile->pos % bfile->buffer_size;
            cpylen = PRF_MIN( (bfile->buffer_size - bufoff), (towrite - haswritten) );
            memcpy( bfile->buffer + bufoff, buffer + haswritten, cpylen );
            haswritten += cpylen;
            bfile->pos += cpylen;
        } else {
            bf_write_buffer( bfile );
        }
    }

    return haswritten;
} /* bf_write() */

/**************************************************************************/

int
bf_rewind(
    bfile_t * bfile,
    unsigned int num_bytes )
{
    unsigned int prevbuf, newbuf;
    assert( bfile != NULL );
    prevbuf = bfile->pos / bfile->buffer_size;
    if ( num_bytes > bfile->pos ) {
        num_bytes = bfile->pos;
        bfile->pos = 0;
        fprintf( stderr, "warning: tried rewinding past beginning of file.\n" );
    } else {
        bfile->pos -= num_bytes;
    }
    newbuf = bfile->pos / bfile->buffer_size;
    if ( newbuf < prevbuf ) { /* rewinded past block boundary */
        /* refresh "preloaded" buffer with old buffer */
        bfile->ipos = newbuf * bfile->buffer_size;
        fseek( bfile->file, bfile->ipos, SEEK_SET );
        bf_read_buffer( bfile );
    }
    return num_bytes;
} /* bf_rewind() */

/**************************************************************************/

int8_t
bf_get_int8(
    bfile_t * bfile )
{
    int8_t val;
    int bufpos;

    assert( bfile != NULL );
    assert( (bfile->flags & BF_READABLE) != FALSE );

    if ( bf_at_eof( bfile ) != FALSE )
        return 0;

    if ( (bfile->ipos - bfile->pos) < 1 ) 
        bf_read_buffer( bfile );

    bufpos = bfile->pos % bfile->buffer_size;
    val =((int8_t *)(bfile->buffer))[bufpos];
    bfile->pos++;
    return val;
} /* bf_get_int8() */

/**************************************************************************/

uint8_t
bf_get_uint8(
    bfile_t * bfile )
{
    uint8_t val;
    uint8_t * buffer;
    int bufpos;

    assert( bfile != NULL );
    assert( (bfile->flags & BF_READABLE) != FALSE );

    if ( bf_at_eof( bfile ) != FALSE )
        return 0;

    if ( (bfile->ipos - bfile->pos) < 1 ) 
        bf_read_buffer( bfile );

    buffer = bfile->buffer;
    bufpos = bfile->pos % bfile->buffer_size;
    val = buffer[bufpos];
    bfile->pos++;
    return val;
} /* bf_get_uint8() */

/**************************************************************************/

int16_t
bf_get_int16_be(
    bfile_t * bfile )
{
    int16_t val;
    val = bf_get_int8( bfile ) << 8;
    val |= bf_get_uint8( bfile );
    return val;
} /* bf_get_int16_be() */

/**************************************************************************/

uint16_t
bf_get_uint16_be(
    bfile_t * bfile )
{
    uint16_t val;
    val = bf_get_uint8( bfile ) << 8;
    val |= bf_get_uint8( bfile );
    return val;
} /* bf_get_uint16_be() */

/**************************************************************************/

int16_t
bf_get_int16_le(
    bfile_t * bfile )
{
    int16_t val;
    val = bf_get_uint8( bfile );
    val |= bf_get_int8( bfile ) << 8;
    return val;
} /* bf_get_int16_le() */

/**************************************************************************/

uint16_t
bf_get_uint16_le(
    bfile_t * bfile )
{
    uint16_t val;
    val = bf_get_uint8( bfile );
    val |= bf_get_uint8( bfile ) << 8;
    return val;
} /* bf_get_uint16_le() */

/**************************************************************************/

int32_t
bf_get_int32_be(
    bfile_t * bfile )
{
    int32_t val;
    val = bf_get_int8( bfile ) << 24;
    val |= bf_get_uint8( bfile ) << 16;
    val |= bf_get_uint8( bfile ) << 8;
    val |= bf_get_uint8( bfile );
    return val;
} /* bf_get_int32_be() */

/**************************************************************************/

uint32_t
bf_get_uint32_be(
    bfile_t * bfile )
{
    uint32_t val;
    val = bf_get_uint8( bfile ) << 24;
    val |= bf_get_uint8( bfile ) << 16;
    val |= bf_get_uint8( bfile ) << 8;
    val |= bf_get_uint8( bfile );
    return val;
} /* bf_get_uint32_be() */

/**************************************************************************/

int32_t
bf_get_int32_le(
    bfile_t * bfile )
{
    int32_t val;
    val = bf_get_uint8( bfile );
    val |= bf_get_uint8( bfile ) << 8;
    val |= bf_get_uint8( bfile ) << 16;
    val |= bf_get_int8( bfile ) << 24;
    return val;
} /* bf_get_int32_le() */

/**************************************************************************/

uint32_t
bf_get_uint32_le(
    bfile_t * bfile )
{
    uint32_t val;
    val = bf_get_uint8( bfile );
    val |= bf_get_uint8( bfile ) << 8;
    val |= bf_get_uint8( bfile ) << 16;
    val |= bf_get_uint8( bfile ) << 24;
    return val;
} /* bf_get_uint32_le() */

/**************************************************************************/

float32_t
bf_get_float32_be(
    bfile_t * bfile )
{
    union {
        float32_t  real;
        uint8_t    bytes[4];
    } data;

#ifndef WORDS_BIGENDIAN
    data.bytes[3] = bf_get_uint8( bfile );
    data.bytes[2] = bf_get_uint8( bfile );
    data.bytes[1] = bf_get_uint8( bfile );
    data.bytes[0] = bf_get_uint8( bfile );
#else /* WORDS_BIGENDIAN */
    data.bytes[0] = bf_get_uint8( bfile );
    data.bytes[1] = bf_get_uint8( bfile );
    data.bytes[2] = bf_get_uint8( bfile );
    data.bytes[3] = bf_get_uint8( bfile );
#endif /* WORDS_BIGENDIAN */

    return data.real;
} /* bf_get_float32_be() */

/**************************************************************************/

float32_t
bf_get_float32_le(
    bfile_t * bfile )
{
    union {
        float32_t  real;
        uint8_t    bytes[4];
    } data;

#ifndef WORDS_BIGENDIAN
    data.bytes[0] = bf_get_uint8( bfile );
    data.bytes[1] = bf_get_uint8( bfile );
    data.bytes[2] = bf_get_uint8( bfile );
    data.bytes[3] = bf_get_uint8( bfile );
#else /* WORDS_BIGENDIAN */
    data.bytes[3] = bf_get_uint8( bfile );
    data.bytes[2] = bf_get_uint8( bfile );
    data.bytes[1] = bf_get_uint8( bfile );
    data.bytes[0] = bf_get_uint8( bfile );
#endif /* WORDS_BIGENDIAN */

    return data.real;
} /* bf_get_float32_le() */

/**************************************************************************/

float64_t
bf_get_float64_be(
    bfile_t * bfile )
{
    union {
        float64_t real;
        uint8_t   bytes[8];
    } data;

#ifndef WORDS_BIGENDIAN
    data.bytes[7] = bf_get_uint8( bfile );
    data.bytes[6] = bf_get_uint8( bfile );
    data.bytes[5] = bf_get_uint8( bfile );
    data.bytes[4] = bf_get_uint8( bfile );
    data.bytes[3] = bf_get_uint8( bfile );
    data.bytes[2] = bf_get_uint8( bfile );
    data.bytes[1] = bf_get_uint8( bfile );
    data.bytes[0] = bf_get_uint8( bfile );
#else /* WORDS_BIGENDIAN */
    data.bytes[0] = bf_get_uint8( bfile );
    data.bytes[1] = bf_get_uint8( bfile );
    data.bytes[2] = bf_get_uint8( bfile );
    data.bytes[3] = bf_get_uint8( bfile );
    data.bytes[4] = bf_get_uint8( bfile );
    data.bytes[5] = bf_get_uint8( bfile );
    data.bytes[6] = bf_get_uint8( bfile );
    data.bytes[7] = bf_get_uint8( bfile );
#endif /* WORDS_BIGENDIAN */

    return data.real;
} /* bf_get_float64_be() */

/**************************************************************************/

float64_t
bf_get_float64_le(
    bfile_t * bfile )
{
    union {
        float64_t real;
        uint8_t   bytes[8];
    } data;

#ifndef WORDS_BIGENDIAN
    data.bytes[0] = bf_get_uint8( bfile );
    data.bytes[1] = bf_get_uint8( bfile );
    data.bytes[2] = bf_get_uint8( bfile );
    data.bytes[3] = bf_get_uint8( bfile );
    data.bytes[4] = bf_get_uint8( bfile );
    data.bytes[5] = bf_get_uint8( bfile );
    data.bytes[6] = bf_get_uint8( bfile );
    data.bytes[7] = bf_get_uint8( bfile );
#else /* WORDS_BIGENDIAN */
    data.bytes[7] = bf_get_uint8( bfile );
    data.bytes[6] = bf_get_uint8( bfile );
    data.bytes[5] = bf_get_uint8( bfile );
    data.bytes[4] = bf_get_uint8( bfile );
    data.bytes[3] = bf_get_uint8( bfile );
    data.bytes[2] = bf_get_uint8( bfile );
    data.bytes[1] = bf_get_uint8( bfile );
    data.bytes[0] = bf_get_uint8( bfile );
#endif /* WORDS_BIGENDIAN */

    return data.real;
} /* bf_get_float64_le() */

/**************************************************************************/

void
bf_unget_uint32_be(
    bfile_t * bfile,
    uint32_t data )
{
    unsigned char * buffer = bfile->buffer;
    int bufpos = bfile->pos % bfile->buffer_size;
    bufpos -= 4;
    assert( bufpos >= 0 );
    buffer[bufpos] = (data >> 24) & 0xff;
    buffer[bufpos+1] = (data >> 16) & 0xff;
    buffer[bufpos+2] = (data >> 8) & 0xff;
    buffer[bufpos+3] = data & 0xff ;
    bfile->pos -= 4;
}

/* peek functions */

int16_t
bf_peek_int16_be(
    bfile_t * bfile )
{
    int16_t value;
    int bufoff;
    int8_t * buffer;

    assert( bfile != NULL );
    assert( (bfile->flags & BF_READABLE) != FALSE );

    bufoff = bfile->pos % bfile->buffer_size;
    if ( (bufoff == 0) && (bfile->pos == bfile->ipos) )
        bf_read_buffer( bfile );

    if ( bfile->pos > (bfile->ipos - 2) ) {
        fprintf( stderr,
            "bfile warning: could not peek on next int16.\n" );
        return 0;
    }

    buffer = (int8_t *) bfile->buffer;
    value = buffer[bufoff] << 8;
    bufoff = (bufoff + 1) % bfile->buffer_size;
    value = value | bfile->buffer[bufoff];

    return value;
} /* bf_peek_int16_be() */


uint16_t
bf_peek_uint16_be(
    bfile_t * bfile )
{
    uint16_t value;
    int bufoff;

    assert( bfile != NULL );
    assert( (bfile->flags & BF_READABLE) != FALSE );

    bufoff = bfile->pos % bfile->buffer_size;
    if ( (bufoff == 0) && (bfile->pos == bfile->ipos) )
        bf_read_buffer( bfile );

    if ( bfile->pos > (bfile->ipos - 2) ) {
        fprintf( stderr,
            "bfile warning: could not peek on next uint16.\n" );
        return 0;
    }

    bufoff = bfile->pos % bfile->buffer_size;
    value = bfile->buffer[bufoff] << 8;
    bufoff = (bufoff + 1) % bfile->buffer_size;
    value = value | bfile->buffer[bufoff];

    return value;
} /* bf_peek_uint16_be() */

int16_t
bf_peek_int16_le(
    bfile_t * bfile )
{
    int16_t value;
    int8_t * buffer;
    int bufoff;

    assert( bfile != NULL );
    assert( (bfile->flags & BF_READABLE) != FALSE );

    bufoff = bfile->pos % bfile->buffer_size;
    if ( (bufoff == 0) && (bfile->pos == bfile->ipos) )
        bf_read_buffer( bfile );

    if ( bfile->pos > (bfile->ipos - 2) ) {
        fprintf( stderr,
            "bfile warning: could not peek on next int16.\n" );
        return 0;
    }

    bufoff = bfile->pos % bfile->buffer_size;
    buffer = (int8_t *) bfile->buffer;
    value = bfile->buffer[bufoff];
    bufoff = (bufoff + 1) % bfile->buffer_size;
    value = value | (buffer[bufoff] << 8);

    return value;
} /* bf_peek_int16_le() */

uint16_t
bf_peek_uint16_le(
    bfile_t * bfile )
{
    uint16_t value;
    int bufoff;

    assert( bfile != NULL );
    assert( (bfile->flags & BF_READABLE) != FALSE );

    bufoff = bfile->pos % bfile->buffer_size;
    if ( (bufoff == 0) && (bfile->pos == bfile->ipos) )
        bf_read_buffer( bfile );

    if ( bfile->pos > (bfile->ipos - 2) ) {
        fprintf( stderr,
            "bfile warning: could not peek on next uint16.\n" );
        return 0;
    }

    bufoff = bfile->pos % bfile->buffer_size;
    value = bfile->buffer[bufoff];
    bufoff = (bufoff + 1) % bfile->buffer_size;
    value = value | (bfile->buffer[bufoff] << 8);

    return value;
} /* bf_peek_uint16_le() */

/**************************************************************************/

bool_t
bf_put_int8(
    bfile_t * bfile,
    int8_t value )
{
    int8_t * buffer;
    int bufpos;

    assert( bfile != NULL );
    assert( (bfile->flags & BF_WRITABLE) != FALSE );

    if ( (bfile->pos - bfile->ipos) >= bfile->buffer_size ) 
        bf_write_buffer( bfile );

    buffer = (int8_t *) bfile->buffer;
    bufpos = bfile->pos % bfile->buffer_size;
    buffer[bufpos] = (value & 0xff);
    bfile->pos++;
    return TRUE;
} /* bf_put_int8() */

bool_t
bf_put_uint8(
    bfile_t * bfile,
    uint8_t value )
{
    uint8_t * buffer;
    int bufpos;

    assert( bfile != NULL );
    assert( (bfile->flags & BF_WRITABLE) != FALSE );

    if ( (bfile->pos - bfile->ipos) >= bfile->buffer_size ) 
        bf_write_buffer( bfile );

    buffer = bfile->buffer;
    bufpos = bfile->pos % bfile->buffer_size;
    buffer[bufpos] = value & 0xff;
    bfile->pos++;
    return TRUE;
} /* bf_put_uint8() */

/**************************************************************************/

bool_t
bf_put_int16_be(
    bfile_t * bfile,
    int16_t value )
{
    bf_put_int8( bfile, (int8_t) ((value >> 8) & 0xff) );
    bf_put_uint8( bfile, (uint8_t) (value & 0xff) );
    return TRUE;
} /* bf_put_int16_be() */

bool_t
bf_put_uint16_be(
    bfile_t * bfile,
    uint16_t value )
{
    bf_put_uint8( bfile, (uint8_t) ((value >> 8) & 0xff) );
    bf_put_uint8( bfile, (uint8_t) (value & 0xff) );
    return TRUE;
} /* bf_put_uint16_be() */

bool_t
bf_put_int16_le(
    bfile_t * bfile,
    int16_t value )
{
    bf_put_uint8( bfile, (uint8_t) (value & 0xff) );
    bf_put_int8( bfile, (int8_t) ((value >> 8) & 0xff) );
    return TRUE;
} /* bf_put_int16_le */

bool_t
bf_put_uint16_le(
    bfile_t * bfile,
    uint16_t value )
{
    bf_put_uint8( bfile, (uint8_t) (value & 0xff) );
    bf_put_uint8( bfile, (uint8_t) ((value >> 8) & 0xff) );
    return TRUE;
} /* bf_put_uint16_le() */

/**************************************************************************/

bool_t
bf_put_int32_be(
    bfile_t * bfile,
    int32_t value )
{
    bf_put_int8( bfile, (int8_t) ((value >> 24) & 0xff) );
    bf_put_uint8( bfile, (uint8_t) ((value >> 16) & 0xff) );
    bf_put_uint8( bfile, (uint8_t) ((value >> 8) & 0xff) );
    bf_put_uint8( bfile, (uint8_t) (value & 0xff) );
    return TRUE;
} /* bf_put_int32_be() */

bool_t
bf_put_uint32_be(
    bfile_t * bfile,
    uint32_t value )
{
    bf_put_uint8( bfile, (uint8_t) ((value >> 24) & 0xff) );
    bf_put_uint8( bfile, (uint8_t) ((value >> 16) & 0xff) );
    bf_put_uint8( bfile, (uint8_t) ((value >> 8) & 0xff) );
    bf_put_uint8( bfile, (uint8_t) (value & 0xff) );
    return TRUE;
} /* bf_put_uint32_be() */

bool_t
bf_put_int32_le(
    bfile_t * bfile,
    int32_t value )
{
    bf_put_uint8( bfile, (uint8_t) (value & 0xff) );
    bf_put_uint8( bfile, (uint8_t) ((value >> 8) & 0xff) );
    bf_put_uint8( bfile, (uint8_t) ((value >> 16) & 0xff) );
    bf_put_int8( bfile, (int8_t) ((value >> 24) & 0xff) );
    return TRUE;
} /* bf_put_int32_le() */

bool_t
bf_put_uint32_le(
    bfile_t * bfile,
    uint32_t value )
{
    bf_put_uint8( bfile, (uint8_t) (value & 0xff) );
    bf_put_uint8( bfile, (uint8_t) ((value >> 8) & 0xff) );
    bf_put_uint8( bfile, (uint8_t) ((value >> 16) & 0xff) );
    bf_put_uint8( bfile, (uint8_t) ((value >> 24) & 0xff) );
    return TRUE;
} /* bf_put_uint32_le() */

/**************************************************************************/

bool_t
bf_put_float32_be(
    bfile_t * bfile,
    float32_t value )
{
    union {
        float32_t real;
        uint8_t   bytes[4];
    } data;

    assert( bfile != NULL );
    data.real = value;

#ifndef WORDS_BIGENDIAN
    bf_put_uint8( bfile, data.bytes[3] );
    bf_put_uint8( bfile, data.bytes[2] );
    bf_put_uint8( bfile, data.bytes[1] );
    bf_put_uint8( bfile, data.bytes[0] );
#else /* WORDS_BIGENDIAN */
    bf_put_uint8( bfile, data.bytes[0] );
    bf_put_uint8( bfile, data.bytes[1] );
    bf_put_uint8( bfile, data.bytes[2] );
    bf_put_uint8( bfile, data.bytes[3] );
#endif /* WORDS_BIGENDIAN */

    return TRUE;
} /* bf_put_float32_be() */

bool_t
bf_put_float32_le(
    bfile_t * bfile,
    float32_t value )
{
    union {
        float32_t real;
        uint8_t   bytes[4];
    } data;

    assert( bfile != NULL );
    data.real = value;

#ifndef WORDS_BIGENDIAN
    bf_put_uint8( bfile, data.bytes[0] );
    bf_put_uint8( bfile, data.bytes[1] );
    bf_put_uint8( bfile, data.bytes[2] );
    bf_put_uint8( bfile, data.bytes[3] );
#else /* WORDS_BIGENDIAN */
    bf_put_uint8( bfile, data.bytes[3] );
    bf_put_uint8( bfile, data.bytes[2] );
    bf_put_uint8( bfile, data.bytes[1] );
    bf_put_uint8( bfile, data.bytes[0] );
#endif /* WORDS_BIGENDIAN */

    return TRUE;
} /* bf_put_float32_le() */


bool_t
bf_put_float64_be(
    bfile_t * bfile,
    float64_t value )
{
    union {
        float64_t real;
        uint8_t   bytes[8];
    } data;

    assert( bfile != NULL );
    data.real = value;

#ifndef WORDS_BIGENDIAN
    bf_put_uint8( bfile, data.bytes[7] );
    bf_put_uint8( bfile, data.bytes[6] );
    bf_put_uint8( bfile, data.bytes[5] );
    bf_put_uint8( bfile, data.bytes[4] );
    bf_put_uint8( bfile, data.bytes[3] );
    bf_put_uint8( bfile, data.bytes[2] );
    bf_put_uint8( bfile, data.bytes[1] );
    bf_put_uint8( bfile, data.bytes[0] );
#else /* WORDS_BIGENDIAN */
    bf_put_uint8( bfile, data.bytes[0] );
    bf_put_uint8( bfile, data.bytes[1] );
    bf_put_uint8( bfile, data.bytes[2] );
    bf_put_uint8( bfile, data.bytes[3] );
    bf_put_uint8( bfile, data.bytes[4] );
    bf_put_uint8( bfile, data.bytes[5] );
    bf_put_uint8( bfile, data.bytes[6] );
    bf_put_uint8( bfile, data.bytes[7] );
#endif /* WORDS_BIGENDIAN */

    return TRUE;
} /* bf_put_float64_be() */


bool_t
bf_put_float64_le(
    bfile_t * bfile,
    float64_t value )
{
    union {
        float64_t real;
        uint8_t   bytes[8];
    } data;

    assert( bfile != NULL );
    data.real = value;

#ifndef WORDS_BIGENDIAN
    bf_put_uint8( bfile, data.bytes[0] );
    bf_put_uint8( bfile, data.bytes[1] );
    bf_put_uint8( bfile, data.bytes[2] );
    bf_put_uint8( bfile, data.bytes[3] );
    bf_put_uint8( bfile, data.bytes[4] );
    bf_put_uint8( bfile, data.bytes[5] );
    bf_put_uint8( bfile, data.bytes[6] );
    bf_put_uint8( bfile, data.bytes[7] );
#else /* WORDS_BIGENDIAN */
    bf_put_uint8( bfile, data.bytes[7] );
    bf_put_uint8( bfile, data.bytes[6] );
    bf_put_uint8( bfile, data.bytes[5] );
    bf_put_uint8( bfile, data.bytes[4] );
    bf_put_uint8( bfile, data.bytes[3] );
    bf_put_uint8( bfile, data.bytes[2] );
    bf_put_uint8( bfile, data.bytes[1] );
    bf_put_uint8( bfile, data.bytes[0] );
#endif /* WORDS_BIGENDIAN */

    return TRUE;
} /* bf_put_float64_le() */

/**************************************************************************/

bool_t
bf_is_readable(
    bfile_t * bfile )
{
    assert( bfile != NULL );
    return (bfile->flags & BF_READABLE) ? TRUE : FALSE;
} /* bf_is_readable() */

bool_t
bf_is_writable(
    bfile_t * bfile )
{
    assert( bfile != NULL );
    return (bfile->flags & BF_WRITABLE) ? TRUE : FALSE;
} /* bf_is_writable() */

/**************************************************************************/

bool_t
bf_at_eof(
    bfile_t * bfile )
{
    assert( bfile != NULL );
    return ( bfile->pos == bfile->size ) ? TRUE : FALSE;
} /* bf_at_eof() */

/**************************************************************************/

size_t
bf_get_position(
    bfile_t * bfile )
{
    assert( bfile != NULL );
    return bfile->pos;
} /* bf_get_position() */

/**************************************************************************/

size_t
bf_get_length(
    bfile_t * bfile )
{
    assert( bfile != NULL );
    return bfile->size;
} /* bf_get_length() */

/**************************************************************************/

size_t
bf_get_remaining_length(
    bfile_t * bfile )
{
    assert( bfile != NULL );
    assert( bfile->size >= bfile->pos );
    return bfile->size - bfile->pos;
} /* bf_get_remaining_length() */

/**************************************************************************/

const char *
bf_get_filename(
    bfile_t * bfile )
{
    assert( bfile != NULL );
    return bfile->filename;
} /* bf_get_filename() */

/**************************************************************************/

int
bf_hex_dump(
    bfile_t * bfile,
    FILE * file,
    unsigned int num_bytes,
    unsigned int unit_size )
{
    char line[ 256 ];
    char ascii[ 256 ];
    char buffer[ 256 ];
    char format[ 256 ];
    bool_t eof;

    unsigned int i, j;
    unsigned int bytes, bytes_per_line;

    unit_size = 3;

    line[0] = '\0';
    eof = FALSE;

    if ( bf_get_remaining_length( bfile ) < num_bytes ) {
        num_bytes = bf_get_remaining_length( bfile );
        eof = TRUE;
    }

    bytes_per_line = 16;
    bytes_per_line -= (bytes_per_line%unit_size);

    bytes = 0;
    while ( bytes < num_bytes ) {
        sprintf( line, "0x%06x:  ", bf_get_position( bfile ) );
        i = 0;
        while ( (i < bytes_per_line) && ((bytes + i) < num_bytes) ) {
            uint32_t word;
            uint32_t mask;
            word = 0; mask = 0;
            for ( j = 0; j < unit_size && (bytes + i) < num_bytes; j++ ) {
                word <<= 8;
                mask <<= 8; mask += 0xff;
                word += bf_get_uint8( bfile );
                ascii[i] = '.';
                if ( isprint( word & 0xff ) )
                    ascii[i] = word & 0xff;
                i++;
            }
            sprintf( format, "%%0%dx ", j * 2 );
            sprintf( buffer, format, (word & mask) );
            strcat( line, buffer );
        }
        if ( i < bytes_per_line ) {
            unsigned int pad;
            pad = ((bytes_per_line - i + unit_size) / unit_size) - 1 +
                  2 * (bytes_per_line - i);
            for ( j = 0; j < pad; j++ ) 
                strcat( line, " " );
        }
        ascii[i] = '\0';
        strcat( line, " \"" );
        strcat( line, ascii );
        strcat( line, "\"" );
        fprintf( file, "%s\n", line );
        bytes += i;
    }
    if ( eof != FALSE )
        fprintf( file, "0x%06x:  [reached end of file]\n",
            bf_get_position( bfile ) );
    
    return num_bytes;
} /* bf_hex_dump() */

/**************************************************************************/
