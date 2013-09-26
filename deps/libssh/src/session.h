#ifndef LIBSSH2_SESSION_H
#define LIBSSH2_SESSION_H
/* Copyright (c) 2004-2007 Sara Golemon <sarag@libssh2.org>
 * Copyright (c) 2009-2010 by Daniel Stenberg
 * Copyright (c) 2010 Simon Josefsson <simon@josefsson.org>
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

/* Conveniance-macros to allow code like this;

   int rc = BLOCK_ADJUST(rc, session, session_startup(session, sock) );

   int rc = BLOCK_ADJUST_ERRNO(ptr, session, session_startup(session, sock) );

   The point of course being to make sure that while in non-blocking mode
   these always return no matter what the return code is, but in blocking mode
   it blocks if EAGAIN is the reason for the return from the underlying
   function.

*/
#define BLOCK_ADJUST(rc,sess,x) \
    do { \
       time_t entry_time = time (NULL); \
       do { \
          rc = x; \
          /* the order of the check below is important to properly deal with \
             the case when the 'sess' is freed */ \
          if((rc != LIBSSH2_ERROR_EAGAIN) || !sess->api_block_mode) \
              break; \
          rc = _libssh2_wait_socket(sess, entry_time);  \
       } while(!rc);   \
    } while(0)

/*
 * For functions that returns a pointer, we need to check if the API is
 * non-blocking and return immediately. If the pointer is non-NULL we return
 * immediately. If the API is blocking and we get a NULL we check the errno
 * and *only* if that is EAGAIN we loop and wait for socket action.
 */
#define BLOCK_ADJUST_ERRNO(ptr,sess,x) \
    do { \
       time_t entry_time = time (NULL); \
       int rc; \
       do { \
           ptr = x; \
           if(!sess->api_block_mode || \
              (ptr != NULL) || \
              (libssh2_session_last_errno(sess) != LIBSSH2_ERROR_EAGAIN) ) \
               break; \
           rc = _libssh2_wait_socket(sess, entry_time); \
        } while(!rc); \
    } while(0)


int _libssh2_wait_socket(LIBSSH2_SESSION *session, time_t entry_time);

/* this is the lib-internal set blocking function */
int _libssh2_session_set_blocking(LIBSSH2_SESSION * session, int blocking);

#endif /* LIBSSH2_SESSION_H */
