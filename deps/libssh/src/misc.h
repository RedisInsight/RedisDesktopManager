#ifndef __LIBSSH2_MISC_H
#define __LIBSSH2_MISC_H
/* Copyright (c) 2009-2011 by Daniel Stenberg
 *
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

struct list_head {
    struct list_node *last;
    struct list_node *first;
};

struct list_node {
    struct list_node *next;
    struct list_node *prev;
    struct list_head *head;
};

int _libssh2_error(LIBSSH2_SESSION* session, int errcode, const char* errmsg);

void _libssh2_list_init(struct list_head *head);

/* add a node last in the list */
void _libssh2_list_add(struct list_head *head,
                       struct list_node *entry);

/* return the "first" node in the list this head points to */
void *_libssh2_list_first(struct list_head *head);

/* return the next node in the list */
void *_libssh2_list_next(struct list_node *node);

/* return the prev node in the list */
void *_libssh2_list_prev(struct list_node *node);

/* remove this node from the list */
void _libssh2_list_remove(struct list_node *entry);

size_t _libssh2_base64_encode(struct _LIBSSH2_SESSION *session,
                              const char *inp, size_t insize, char **outptr);

unsigned int _libssh2_ntohu32(const unsigned char *buf);
libssh2_uint64_t _libssh2_ntohu64(const unsigned char *buf);
void _libssh2_htonu32(unsigned char *buf, uint32_t val);
void _libssh2_store_u32(unsigned char **buf, uint32_t value);
void _libssh2_store_str(unsigned char **buf, const char *str, size_t len);

#if defined(LIBSSH2_WIN32) && !defined(__MINGW32__) && !defined(__CYGWIN__)
/* provide a private one */
#undef HAVE_GETTIMEOFDAY
int __cdecl _libssh2_gettimeofday(struct timeval *tp, void *tzp);
#define HAVE_LIBSSH2_GETTIMEOFDAY
#define LIBSSH2_GETTIMEOFDAY_WIN32 /* enable the win32 implementation */
#else
#ifdef HAVE_GETTIMEOFDAY
#define _libssh2_gettimeofday(x,y) gettimeofday(x,y)
#define HAVE_LIBSSH2_GETTIMEOFDAY
#endif
#endif

#endif /* _LIBSSH2_MISC_H */
