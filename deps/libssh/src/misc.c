/* Copyright (c) 2004-2007 Sara Golemon <sarag@libssh2.org>
 * Copyright (c) 2009-2010 by Daniel Stenberg
 * Copyright (c) 2010  Simon Josefsson
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms,
 * with or without modification, are permitted provided
 * that the following conditions are met:
 *
 *   Redistributions of source code must retain the above
 *   copyright notice, this list of conditions and the
 *   following disclaimer.
 *
 *   Redistributions in binary form must reproduce the above
 *   copyright notice, this list of conditions and the following
 *   disclaimer in the documentation and/or other materials
 *   provided with the distribution.
 *
 *   Neither the name of the copyright holder nor the names
 *   of any other contributors may be used to endorse or
 *   promote products derived from this software without
 *   specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND
 * CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
 * USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
 * OF SUCH DAMAGE.
 */

#include "libssh2_priv.h"
#include "misc.h"

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#ifdef HAVE_SYS_TIME_H
#include <sys/time.h>
#endif

#include <stdio.h>
#include <errno.h>

int _libssh2_error(LIBSSH2_SESSION* session, int errcode, const char* errmsg)
{
    session->err_msg = errmsg;
    session->err_code = errcode;
#ifdef LIBSSH2DEBUG
    if((errcode == LIBSSH2_ERROR_EAGAIN) && !session->api_block_mode)
        /* if this is EAGAIN and we're in non-blocking mode, don't generate
           a debug output for this */
        return errcode;
    _libssh2_debug(session, LIBSSH2_TRACE_ERROR, "%d - %s", session->err_code,
                   session->err_msg);
#endif

    return errcode;
}

#ifdef WIN32
static int wsa2errno(void)
{
    switch (WSAGetLastError()) {
    case WSAEWOULDBLOCK:
        return EAGAIN;

    case WSAENOTSOCK:
        return EBADF;

    case WSAEINTR:
        return EINTR;

    default:
        /* It is most important to ensure errno does not stay at EAGAIN
         * when a different error occurs so just set errno to a generic
         * error */
        return EIO;
    }
}
#endif

/* _libssh2_recv
 *
 * Replacement for the standard recv, return -errno on failure.
 */
ssize_t
_libssh2_recv(libssh2_socket_t sock, void *buffer, size_t length, int flags, void **abstract)
{
    ssize_t rc = recv(sock, buffer, length, flags);
#ifdef WIN32
    if (rc < 0 )
        return -wsa2errno();
#elif defined(__VMS)
    if (rc < 0 ){
        if ( errno == EWOULDBLOCK )
            return -EAGAIN;
        else
            return -errno;
    }
#else
    if (rc < 0 ){
        /* Sometimes the first recv() function call sets errno to ENOENT on
           Solaris and HP-UX */
        if ( errno == ENOENT )
            return -EAGAIN;
        else
            return -errno;
    }
#endif
    return rc;
}

/* _libssh2_send
 *
 * Replacement for the standard send, return -errno on failure.
 */
ssize_t
_libssh2_send(libssh2_socket_t sock, const void *buffer, size_t length,
              int flags, void **abstract)
{
    ssize_t rc = send(sock, buffer, length, flags);
#ifdef WIN32
    if (rc < 0 )
        return -wsa2errno();
#elif defined(__VMS)
    if (rc < 0 ) {
        if ( errno == EWOULDBLOCK )
            return -EAGAIN;
        else
            return -errno;
    }
#else
    if (rc < 0 )
        return -errno;
#endif
    return rc;
}

/* libssh2_ntohu32
 */
unsigned int
_libssh2_ntohu32(const unsigned char *buf)
{
    return (buf[0] << 24) | (buf[1] << 16) | (buf[2] << 8) | buf[3];
}


/* _libssh2_ntohu64
 */
libssh2_uint64_t
_libssh2_ntohu64(const unsigned char *buf)
{
    unsigned long msl, lsl;

    msl = ((libssh2_uint64_t)buf[0] << 24) | ((libssh2_uint64_t)buf[1] << 16)
        | ((libssh2_uint64_t)buf[2] << 8) | (libssh2_uint64_t)buf[3];
    lsl = ((libssh2_uint64_t)buf[4] << 24) | ((libssh2_uint64_t)buf[5] << 16)
        | ((libssh2_uint64_t)buf[6] << 8) | (libssh2_uint64_t)buf[7];

    return ((libssh2_uint64_t)msl <<32) | lsl;
}

/* _libssh2_htonu32
 */
void
_libssh2_htonu32(unsigned char *buf, uint32_t value)
{
    buf[0] = (value >> 24) & 0xFF;
    buf[1] = (value >> 16) & 0xFF;
    buf[2] = (value >> 8) & 0xFF;
    buf[3] = value & 0xFF;
}

/* _libssh2_store_u32
 */
void _libssh2_store_u32(unsigned char **buf, uint32_t value)
{
    _libssh2_htonu32(*buf, value);
    *buf += sizeof(uint32_t);
}

/* _libssh2_store_str
 */
void _libssh2_store_str(unsigned char **buf, const char *str, size_t len)
{
    _libssh2_store_u32(buf, (uint32_t)len);
    if(len) {
        memcpy(*buf, str, len);
        *buf += len;
    }
}

/* Base64 Conversion */

static const char base64_table[] =
{
    'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M',
    'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
    'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm',
    'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '+', '/', '\0'
};

static const char base64_pad = '=';

static const short base64_reverse_table[256] = {
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 62, -1, -1, -1, 63,
    52, 53, 54, 55, 56, 57, 58, 59, 60, 61, -1, -1, -1, -1, -1, -1,
    -1, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14,
    15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, -1, -1, -1, -1, -1,
    -1, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
    41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1
};

/* libssh2_base64_decode
 *
 * Decode a base64 chunk and store it into a newly alloc'd buffer
 */
LIBSSH2_API int
libssh2_base64_decode(LIBSSH2_SESSION *session, char **data,
                      unsigned int *datalen, const char *src,
                      unsigned int src_len)
{
    unsigned char *s, *d;
    short v;
    int i = 0, len = 0;

    *data = LIBSSH2_ALLOC(session, (3 * src_len / 4) + 1);
    d = (unsigned char *) *data;
    if (!d) {
        return _libssh2_error(session, LIBSSH2_ERROR_ALLOC,
                              "Unable to allocate memory for base64 decoding");
    }

    for(s = (unsigned char *) src; ((char *) s) < (src + src_len); s++) {
        if ((v = base64_reverse_table[*s]) < 0)
            continue;
        switch (i % 4) {
        case 0:
            d[len] = v << 2;
            break;
        case 1:
            d[len++] |= v >> 4;
            d[len] = v << 4;
            break;
        case 2:
            d[len++] |= v >> 2;
            d[len] = v << 6;
            break;
        case 3:
            d[len++] |= v;
            break;
        }
        i++;
    }
    if ((i % 4) == 1) {
        /* Invalid -- We have a byte which belongs exclusively to a partial
           octet */
        LIBSSH2_FREE(session, *data);
        return _libssh2_error(session, LIBSSH2_ERROR_INVAL, "Invalid base64");
    }

    *datalen = len;
    return 0;
}

/* ---- Base64 Encoding/Decoding Table --- */
static const char table64[]=
  "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

/*
 * _libssh2_base64_encode()
 *
 * Returns the length of the newly created base64 string. The third argument
 * is a pointer to an allocated area holding the base64 data. If something
 * went wrong, 0 is returned.
 *
 */
size_t _libssh2_base64_encode(LIBSSH2_SESSION *session,
                              const char *inp, size_t insize, char **outptr)
{
  unsigned char ibuf[3];
  unsigned char obuf[4];
  int i;
  int inputparts;
  char *output;
  char *base64data;
  const char *indata = inp;

  *outptr = NULL; /* set to NULL in case of failure before we reach the end */

  if(0 == insize)
    insize = strlen(indata);

  base64data = output = LIBSSH2_ALLOC(session, insize*4/3+4);
  if(NULL == output)
    return 0;

  while(insize > 0) {
    for (i = inputparts = 0; i < 3; i++) {
      if(insize > 0) {
        inputparts++;
        ibuf[i] = *indata;
        indata++;
        insize--;
      }
      else
        ibuf[i] = 0;
    }

    obuf[0] = (unsigned char)  ((ibuf[0] & 0xFC) >> 2);
    obuf[1] = (unsigned char) (((ibuf[0] & 0x03) << 4) | \
                               ((ibuf[1] & 0xF0) >> 4));
    obuf[2] = (unsigned char) (((ibuf[1] & 0x0F) << 2) | \
                               ((ibuf[2] & 0xC0) >> 6));
    obuf[3] = (unsigned char)   (ibuf[2] & 0x3F);

    switch(inputparts) {
    case 1: /* only one byte read */
      snprintf(output, 5, "%c%c==",
               table64[obuf[0]],
               table64[obuf[1]]);
      break;
    case 2: /* two bytes read */
      snprintf(output, 5, "%c%c%c=",
               table64[obuf[0]],
               table64[obuf[1]],
               table64[obuf[2]]);
      break;
    default:
      snprintf(output, 5, "%c%c%c%c",
               table64[obuf[0]],
               table64[obuf[1]],
               table64[obuf[2]],
               table64[obuf[3]] );
      break;
    }
    output += 4;
  }
  *output=0;
  *outptr = base64data; /* make it return the actual data memory */

  return strlen(base64data); /* return the length of the new data */
}
/* ---- End of Base64 Encoding ---- */

LIBSSH2_API void
libssh2_free(LIBSSH2_SESSION *session, void *ptr)
{
    LIBSSH2_FREE(session, ptr);
}

#ifdef LIBSSH2DEBUG
LIBSSH2_API int
libssh2_trace(LIBSSH2_SESSION * session, int bitmask)
{
    session->showmask = bitmask;
    return 0;
}

LIBSSH2_API int
libssh2_trace_sethandler(LIBSSH2_SESSION *session, void* handler_context,
                         libssh2_trace_handler_func callback)
{
    session->tracehandler = callback;
    session->tracehandler_context = handler_context;
    return 0;
}

void
_libssh2_debug(LIBSSH2_SESSION * session, int context, const char *format, ...)
{
    char buffer[1536];
    int len, msglen, buflen = sizeof(buffer);
    va_list vargs;
    struct timeval now;
    static int firstsec;
    static const char *const contexts[] = {
        "Unknown",
        "Transport",
        "Key Ex",
        "Userauth",
        "Conn",
        "SCP",
        "SFTP",
        "Failure Event",
        "Publickey",
        "Socket",
    };
    const char* contexttext = contexts[0];
    unsigned int contextindex;

    if (!(session->showmask & context)) {
        /* no such output asked for */
        return;
    }

    /* Find the first matching context string for this message */
    for (contextindex = 0; contextindex < ARRAY_SIZE(contexts);
         contextindex++) {
        if ((context & (1 << contextindex)) != 0) {
            contexttext = contexts[contextindex];
            break;
        }
    }

    _libssh2_gettimeofday(&now, NULL);
    if(!firstsec) {
        firstsec = now.tv_sec;
    }
    now.tv_sec -= firstsec;

    len = snprintf(buffer, buflen, "[libssh2] %d.%06d %s: ",
                   (int)now.tv_sec, (int)now.tv_usec, contexttext);

    if (len >= buflen)
        msglen = buflen - 1;
    else {
        buflen -= len;
        msglen = len;
        va_start(vargs, format);
        len = vsnprintf(buffer + msglen, buflen, format, vargs);
        va_end(vargs);
        msglen += len < buflen ? len : buflen - 1;
    }

    if (session->tracehandler)
        (session->tracehandler)(session, session->tracehandler_context, buffer,
                                msglen);
    else
        fprintf(stderr, "%s\n", buffer);
}

#else
LIBSSH2_API int
libssh2_trace(LIBSSH2_SESSION * session, int bitmask)
{
    (void) session;
    (void) bitmask;
    return 0;
}

LIBSSH2_API int
libssh2_trace_sethandler(LIBSSH2_SESSION *session, void* handler_context,
                         libssh2_trace_handler_func callback)
{
    (void) session;
    (void) handler_context;
    (void) callback;
    return 0;
}
#endif

/* init the list head */
void _libssh2_list_init(struct list_head *head)
{
    head->first = head->last = NULL;
}

/* add a node to the list */
void _libssh2_list_add(struct list_head *head,
                       struct list_node *entry)
{
    /* store a pointer to the head */
    entry->head = head;

    /* we add this entry at the "top" so it has no next */
    entry->next = NULL;

    /* make our prev point to what the head thinks is last */
    entry->prev = head->last;

    /* and make head's last be us now */
    head->last = entry;

    /* make sure our 'prev' node points to us next */
    if(entry->prev)
        entry->prev->next = entry;
    else
        head->first = entry;
}

/* return the "first" node in the list this head points to */
void *_libssh2_list_first(struct list_head *head)
{
    return head->first;
}

/* return the next node in the list */
void *_libssh2_list_next(struct list_node *node)
{
    return node->next;
}

/* return the prev node in the list */
void *_libssh2_list_prev(struct list_node *node)
{
    return node->prev;
}

/* remove this node from the list */
void _libssh2_list_remove(struct list_node *entry)
{
    if(entry->prev)
        entry->prev->next = entry->next;
    else
        entry->head->first = entry->next;

    if(entry->next)
        entry->next->prev = entry->prev;
    else
        entry->head->last = entry->prev;
}

#if 0
/* insert a node before the given 'after' entry */
void _libssh2_list_insert(struct list_node *after, /* insert before this */
                          struct list_node *entry)
{
    /* 'after' is next to 'entry' */
    bentry->next = after;

    /* entry's prev is then made to be the prev after current has */
    entry->prev = after->prev;

    /* the node that is now before 'entry' was previously before 'after'
       and must be made to point to 'entry' correctly */
    if(entry->prev)
        entry->prev->next = entry;
    else
      /* there was no node before this, so we make sure we point the head
         pointer to this node */
      after->head->first = entry;

    /* after's prev entry points back to entry */
    after->prev = entry;

    /* after's next entry is still the same as before */

    /* entry's head is the same as after's */
    entry->head = after->head;
}

#endif

/* this define is defined in misc.h for the correct platforms */
#ifdef LIBSSH2_GETTIMEOFDAY_WIN32
/*
 * gettimeofday
 * Implementation according to:
 * The Open Group Base Specifications Issue 6
 * IEEE Std 1003.1, 2004 Edition
 */

/*
 *  THIS SOFTWARE IS NOT COPYRIGHTED
 *
 *  This source code is offered for use in the public domain. You may
 *  use, modify or distribute it freely.
 *
 *  This code is distributed in the hope that it will be useful but
 *  WITHOUT ANY WARRANTY. ALL WARRANTIES, EXPRESS OR IMPLIED ARE HEREBY
 *  DISCLAIMED. This includes but is not limited to warranties of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 *  Contributed by:
 *  Danny Smith <dannysmith@users.sourceforge.net>
 */

/* Offset between 1/1/1601 and 1/1/1970 in 100 nanosec units */
#define _W32_FT_OFFSET (116444736000000000)

int __cdecl _libssh2_gettimeofday(struct timeval *tp, void *tzp)
 {
  union {
    unsigned __int64 ns100; /*time since 1 Jan 1601 in 100ns units */
    FILETIME ft;
  }  _now;

  if(tp)
    {
      GetSystemTimeAsFileTime (&_now.ft);
      tp->tv_usec=(long)((_now.ns100 / 10) % 1000000 );
      tp->tv_sec= (long)((_now.ns100 - _W32_FT_OFFSET) / 10000000);
    }
  /* Always return 0 as per Open Group Base Specifications Issue 6.
     Do not set errno on error.  */
  return 0;
}


#endif
