/* Copyright (c) 2004-2007 Sara Golemon <sarag@libssh2.org>
 * Copyright (c) 2009-2011 by Daniel Stenberg
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

#include "libssh2_priv.h"
#include <errno.h>
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#include <stdlib.h>
#include <fcntl.h>

#ifdef HAVE_GETTIMEOFDAY
#include <sys/time.h>
#endif
#ifdef HAVE_ALLOCA_H
#include <alloca.h>
#endif

#include "transport.h"
#include "session.h"
#include "channel.h"
#include "mac.h"
#include "misc.h"

/* libssh2_default_alloc
 */
static
LIBSSH2_ALLOC_FUNC(libssh2_default_alloc)
{
    (void) abstract;
    return malloc(count);
}

/* libssh2_default_free
 */
static
LIBSSH2_FREE_FUNC(libssh2_default_free)
{
    (void) abstract;
    free(ptr);
}

/* libssh2_default_realloc
 */
static
LIBSSH2_REALLOC_FUNC(libssh2_default_realloc)
{
    (void) abstract;
    return realloc(ptr, count);
}

/*
 * banner_receive
 *
 * Wait for a hello from the remote host
 * Allocate a buffer and store the banner in session->remote.banner
 * Returns: 0 on success, LIBSSH2_ERROR_EAGAIN if read would block, negative
 * on failure
 */
static int
banner_receive(LIBSSH2_SESSION * session)
{
    int ret;
    int banner_len;

    if (session->banner_TxRx_state == libssh2_NB_state_idle) {
        banner_len = 0;

        session->banner_TxRx_state = libssh2_NB_state_created;
    } else {
        banner_len = session->banner_TxRx_total_send;
    }

    while ((banner_len < (int) sizeof(session->banner_TxRx_banner)) &&
           ((banner_len == 0)
            || (session->banner_TxRx_banner[banner_len - 1] != '\n'))) {
        char c = '\0';

        /* no incoming block yet! */
        session->socket_block_directions &= ~LIBSSH2_SESSION_BLOCK_INBOUND;

        ret = LIBSSH2_RECV(session, &c, 1,
                            LIBSSH2_SOCKET_RECV_FLAGS(session));
        if (ret < 0) {
            if(session->api_block_mode || (ret != -EAGAIN))
                /* ignore EAGAIN when non-blocking */
                _libssh2_debug(session, LIBSSH2_TRACE_SOCKET,
                               "Error recving %d bytes: %d", 1, -ret);
        }
        else
            _libssh2_debug(session, LIBSSH2_TRACE_SOCKET,
                           "Recved %d bytes banner", ret);

        if (ret < 0) {
            if (ret == -EAGAIN) {
                session->socket_block_directions =
                    LIBSSH2_SESSION_BLOCK_INBOUND;
                session->banner_TxRx_total_send = banner_len;
                return LIBSSH2_ERROR_EAGAIN;
            }

            /* Some kinda error */
            session->banner_TxRx_state = libssh2_NB_state_idle;
            session->banner_TxRx_total_send = 0;
            return LIBSSH2_ERROR_SOCKET_RECV;
        }

        if (ret == 0) {
            session->socket_state = LIBSSH2_SOCKET_DISCONNECTED;
            return LIBSSH2_ERROR_SOCKET_DISCONNECT;
        }

        if (c == '\0') {
            /* NULLs are not allowed in SSH banners */
            session->banner_TxRx_state = libssh2_NB_state_idle;
            session->banner_TxRx_total_send = 0;
            return LIBSSH2_ERROR_BANNER_RECV;
        }

        session->banner_TxRx_banner[banner_len++] = c;
    }

    while (banner_len &&
           ((session->banner_TxRx_banner[banner_len - 1] == '\n') ||
            (session->banner_TxRx_banner[banner_len - 1] == '\r'))) {
        banner_len--;
    }

    /* From this point on, we are done here */
    session->banner_TxRx_state = libssh2_NB_state_idle;
    session->banner_TxRx_total_send = 0;

    if (!banner_len)
        return LIBSSH2_ERROR_BANNER_RECV;

    session->remote.banner = LIBSSH2_ALLOC(session, banner_len + 1);
    if (!session->remote.banner) {
        return _libssh2_error(session, LIBSSH2_ERROR_ALLOC,
                              "Error allocating space for remote banner");
    }
    memcpy(session->remote.banner, session->banner_TxRx_banner, banner_len);
    session->remote.banner[banner_len] = '\0';
    _libssh2_debug(session, LIBSSH2_TRACE_TRANS, "Received Banner: %s",
                   session->remote.banner);
    return LIBSSH2_ERROR_NONE;
}

/*
 * banner_send
 *
 * Send the default banner, or the one set via libssh2_setopt_string
 *
 * Returns LIBSSH2_ERROR_EAGAIN if it would block - and if it does so, you
 * should call this function again as soon as it is likely that more data can
 * be sent, and this function should then be called with the same argument set
 * (same data pointer and same data_len) until zero or failure is returned.
 */
static int
banner_send(LIBSSH2_SESSION * session)
{
    char *banner = (char *) LIBSSH2_SSH_DEFAULT_BANNER_WITH_CRLF;
    int banner_len = sizeof(LIBSSH2_SSH_DEFAULT_BANNER_WITH_CRLF) - 1;
    ssize_t ret;
#ifdef LIBSSH2DEBUG
    char banner_dup[256];
#endif

    if (session->banner_TxRx_state == libssh2_NB_state_idle) {
        if (session->local.banner) {
            /* setopt_string will have given us our \r\n characters */
            banner_len = strlen((char *) session->local.banner);
            banner = (char *) session->local.banner;
        }
#ifdef LIBSSH2DEBUG
        /* Hack and slash to avoid sending CRLF in debug output */
        if (banner_len < 256) {
            memcpy(banner_dup, banner, banner_len - 2);
            banner_dup[banner_len - 2] = '\0';
        } else {
            memcpy(banner_dup, banner, 255);
            banner[255] = '\0';
        }

        _libssh2_debug(session, LIBSSH2_TRACE_TRANS, "Sending Banner: %s",
                       banner_dup);
#endif

        session->banner_TxRx_state = libssh2_NB_state_created;
    }

    /* no outgoing block yet! */
    session->socket_block_directions &= ~LIBSSH2_SESSION_BLOCK_OUTBOUND;

    ret = LIBSSH2_SEND(session,
                        banner + session->banner_TxRx_total_send,
                        banner_len - session->banner_TxRx_total_send,
                        LIBSSH2_SOCKET_SEND_FLAGS(session));
    if (ret < 0)
        _libssh2_debug(session, LIBSSH2_TRACE_SOCKET,
                       "Error sending %d bytes: %d",
                       banner_len - session->banner_TxRx_total_send, -ret);
    else
        _libssh2_debug(session, LIBSSH2_TRACE_SOCKET,
                       "Sent %d/%d bytes at %p+%d", ret,
                       banner_len - session->banner_TxRx_total_send,
                       banner, session->banner_TxRx_total_send);

    if (ret != (banner_len - session->banner_TxRx_total_send)) {
        if (ret >= 0 || ret == -EAGAIN) {
            /* the whole packet could not be sent, save the what was */
            session->socket_block_directions =
                LIBSSH2_SESSION_BLOCK_OUTBOUND;
            if (ret > 0)
                session->banner_TxRx_total_send += ret;
            return LIBSSH2_ERROR_EAGAIN;
        }
        session->banner_TxRx_state = libssh2_NB_state_idle;
        session->banner_TxRx_total_send = 0;
        return LIBSSH2_ERROR_SOCKET_RECV;
    }

    /* Set the state back to idle */
    session->banner_TxRx_state = libssh2_NB_state_idle;
    session->banner_TxRx_total_send = 0;

    return 0;
}

/*
 * session_nonblock() sets the given socket to either blocking or
 * non-blocking mode based on the 'nonblock' boolean argument. This function
 * is copied from the libcurl sources with permission.
 */
static int
session_nonblock(libssh2_socket_t sockfd,   /* operate on this */
                 int nonblock /* TRUE or FALSE */ )
{
#undef SETBLOCK
#define SETBLOCK 0
#ifdef HAVE_O_NONBLOCK
    /* most recent unix versions */
    int flags;

    flags = fcntl(sockfd, F_GETFL, 0);
    if (nonblock)
        return fcntl(sockfd, F_SETFL, flags | O_NONBLOCK);
    else
        return fcntl(sockfd, F_SETFL, flags & (~O_NONBLOCK));
#undef SETBLOCK
#define SETBLOCK 1
#endif

#if defined(HAVE_FIONBIO) && (SETBLOCK == 0)
    /* older unix versions and VMS*/
    int flags;

    flags = nonblock;
    return ioctl(sockfd, FIONBIO, &flags);
#undef SETBLOCK
#define SETBLOCK 2
#endif

#if defined(HAVE_IOCTLSOCKET) && (SETBLOCK == 0)
    /* Windows? */
    unsigned long flags;
    flags = nonblock;

    return ioctlsocket(sockfd, FIONBIO, &flags);
#undef SETBLOCK
#define SETBLOCK 3
#endif

#if defined(HAVE_IOCTLSOCKET_CASE) && (SETBLOCK == 0)
    /* presumably for Amiga */
    return IoctlSocket(sockfd, FIONBIO, (long) nonblock);
#undef SETBLOCK
#define SETBLOCK 4
#endif

#if defined(HAVE_SO_NONBLOCK) && (SETBLOCK == 0)
    /* BeOS */
    long b = nonblock ? 1 : 0;
    return setsockopt(sockfd, SOL_SOCKET, SO_NONBLOCK, &b, sizeof(b));
#undef SETBLOCK
#define SETBLOCK 5
#endif

#ifdef HAVE_DISABLED_NONBLOCKING
    return 0;                   /* returns success */
#undef SETBLOCK
#define SETBLOCK 6
#endif

#if (SETBLOCK == 0)
#error "no non-blocking method was found/used/set"
#endif
}

/*
 * get_socket_nonblocking()
 *
 * gets the given blocking or non-blocking state of the socket.
 */
static int
get_socket_nonblocking(int sockfd)
{                               /* operate on this */
#undef GETBLOCK
#define GETBLOCK 0
#ifdef HAVE_O_NONBLOCK
    /* most recent unix versions */
    int flags;

    if ((flags = fcntl(sockfd, F_GETFL, 0)) == -1) {
        /* Assume blocking on error */
        return 1;
    }
    return (flags & O_NONBLOCK);
#undef GETBLOCK
#define GETBLOCK 1
#endif

#if defined(WSAEWOULDBLOCK) && (GETBLOCK == 0)
    /* Windows? */
    unsigned int option_value;
    socklen_t option_len = sizeof(option_value);

    if (getsockopt
        (sockfd, SOL_SOCKET, SO_ERROR, (void *) &option_value, &option_len)) {
        /* Assume blocking on error */
        return 1;
    }
    return (int) option_value;
#undef GETBLOCK
#define GETBLOCK 2
#endif

#if defined(HAVE_SO_NONBLOCK) && (GETBLOCK == 0)
    /* BeOS */
    long b;
    if (getsockopt(sockfd, SOL_SOCKET, SO_NONBLOCK, &b, sizeof(b))) {
        /* Assume blocking on error */
        return 1;
    }
    return (int) b;
#undef GETBLOCK
#define GETBLOCK 5
#endif

#if defined(SO_STATE) && defined( __VMS ) && (GETBLOCK == 0)

    /* VMS TCP/IP Services */

    size_t sockstat = 0;
    int    callstat = 0;
    size_t size = sizeof( int );

    callstat = getsockopt(sockfd, SOL_SOCKET, SO_STATE,
                                  (char *)&sockstat, &size);
    if ( callstat == -1 ) return(0);
    if ( (sockstat&SS_NBIO) )return(1);
    return(0);

#undef GETBLOCK
#define GETBLOCK 6
#endif

#ifdef HAVE_DISABLED_NONBLOCKING
    return 1;                   /* returns blocking */
#undef GETBLOCK
#define GETBLOCK 7
#endif

#if (GETBLOCK == 0)
#error "no non-blocking method was found/used/get"
#endif
}

/* libssh2_session_banner_set
 * Set the local banner to use in the server handshake.
 */
LIBSSH2_API int
libssh2_session_banner_set(LIBSSH2_SESSION * session, const char *banner)
{
    size_t banner_len = banner ? strlen(banner) : 0;

    if (session->local.banner) {
        LIBSSH2_FREE(session, session->local.banner);
        session->local.banner = NULL;
    }

    if (!banner_len)
        return 0;

    session->local.banner = LIBSSH2_ALLOC(session, banner_len + 3);
    if (!session->local.banner) {
        return _libssh2_error(session, LIBSSH2_ERROR_ALLOC,
                              "Unable to allocate memory for local banner");
    }

    memcpy(session->local.banner, banner, banner_len);

    /* first zero terminate like this so that the debug output is nice */
    session->local.banner[banner_len] = '\0';
    _libssh2_debug(session, LIBSSH2_TRACE_TRANS, "Setting local Banner: %s",
                   session->local.banner);
    session->local.banner[banner_len++] = '\r';
    session->local.banner[banner_len++] = '\n';
    session->local.banner[banner_len] = '\0';

    return 0;
}

/* libssh2_banner_set
 * Set the local banner. DEPRECATED VERSION
 */
LIBSSH2_API int
libssh2_banner_set(LIBSSH2_SESSION * session, const char *banner)
{
    return libssh2_session_banner_set(session, banner);
}

/*
 * libssh2_session_init_ex
 *
 * Allocate and initialize a libssh2 session structure. Allows for malloc
 * callbacks in case the calling program has its own memory manager It's
 * allowable (but unadvisable) to define some but not all of the malloc
 * callbacks An additional pointer value may be optionally passed to be sent
 * to the callbacks (so they know who's asking)
 */
LIBSSH2_API LIBSSH2_SESSION *
libssh2_session_init_ex(LIBSSH2_ALLOC_FUNC((*my_alloc)),
                        LIBSSH2_FREE_FUNC((*my_free)),
                        LIBSSH2_REALLOC_FUNC((*my_realloc)), void *abstract)
{
    LIBSSH2_ALLOC_FUNC((*local_alloc)) = libssh2_default_alloc;
    LIBSSH2_FREE_FUNC((*local_free)) = libssh2_default_free;
    LIBSSH2_REALLOC_FUNC((*local_realloc)) = libssh2_default_realloc;
    LIBSSH2_SESSION *session;

    if (my_alloc) {
        local_alloc = my_alloc;
    }
    if (my_free) {
        local_free = my_free;
    }
    if (my_realloc) {
        local_realloc = my_realloc;
    }

    session = local_alloc(sizeof(LIBSSH2_SESSION), &abstract);
    if (session) {
        memset(session, 0, sizeof(LIBSSH2_SESSION));
        session->alloc = local_alloc;
        session->free = local_free;
        session->realloc = local_realloc;
        session->send = _libssh2_send;
        session->recv = _libssh2_recv;
        session->abstract = abstract;
        session->api_timeout = 0; /* timeout-free API by default */
        session->api_block_mode = 1; /* blocking API by default */
        _libssh2_debug(session, LIBSSH2_TRACE_TRANS,
                       "New session resource allocated");
        _libssh2_init_if_needed ();
    }
    return session;
}

/*
 * libssh2_session_callback_set
 *
 * Set (or reset) a callback function
 * Returns the prior address
 *
 * FIXME: this function relies on that we can typecast function pointers
 * to void pointers, which isn't allowed in ISO C!
 */
LIBSSH2_API void *
libssh2_session_callback_set(LIBSSH2_SESSION * session,
                             int cbtype, void *callback)
{
    void *oldcb;

    switch (cbtype) {
    case LIBSSH2_CALLBACK_IGNORE:
        oldcb = session->ssh_msg_ignore;
        session->ssh_msg_ignore = callback;
        return oldcb;

    case LIBSSH2_CALLBACK_DEBUG:
        oldcb = session->ssh_msg_debug;
        session->ssh_msg_debug = callback;
        return oldcb;

    case LIBSSH2_CALLBACK_DISCONNECT:
        oldcb = session->ssh_msg_disconnect;
        session->ssh_msg_disconnect = callback;
        return oldcb;

    case LIBSSH2_CALLBACK_MACERROR:
        oldcb = session->macerror;
        session->macerror = callback;
        return oldcb;

    case LIBSSH2_CALLBACK_X11:
        oldcb = session->x11;
        session->x11 = callback;
        return oldcb;

    case LIBSSH2_CALLBACK_SEND:
        oldcb = session->send;
        session->send = callback;
        return oldcb;

    case LIBSSH2_CALLBACK_RECV:
        oldcb = session->recv;
        session->recv = callback;
        return oldcb;
    }
    _libssh2_debug(session, LIBSSH2_TRACE_TRANS, "Setting Callback %d", cbtype);

    return NULL;
}

/*
 * _libssh2_wait_socket()
 *
 * Utility function that waits for action on the socket. Returns 0 when ready
 * to run again or error on timeout.
 */
int _libssh2_wait_socket(LIBSSH2_SESSION *session, time_t start_time)
{
    int rc;
    int seconds_to_next;
    int dir;
    int has_timeout;
    long ms_to_next = 0;
    long elapsed_ms;

    /* since libssh2 often sets EAGAIN internally before this function is
       called, we can decrease some amount of confusion in user programs by
       resetting the error code in this function to reduce the risk of EAGAIN
       being stored as error when a blocking function has returned */
    session->err_code = LIBSSH2_ERROR_NONE;

    rc = libssh2_keepalive_send (session, &seconds_to_next);
    if (rc < 0)
        return rc;

    ms_to_next = seconds_to_next * 1000;

    /* figure out what to wait for */
    dir = libssh2_session_block_directions(session);

    if(!dir) {
        _libssh2_debug(session, LIBSSH2_TRACE_SOCKET,
                       "Nothing to wait for in wait_socket");
        /* To avoid that we hang below just because there's nothing set to
           wait for, we timeout on 1 second to also avoid busy-looping
           during this condition */
        ms_to_next = 1000;
    }

    if (session->api_timeout > 0 &&
        (seconds_to_next == 0 ||
         seconds_to_next > session->api_timeout)) {
        time_t now = time (NULL);
        elapsed_ms = (long)(1000*difftime(start_time, now));
        if (elapsed_ms > session->api_timeout) {
            session->err_code = LIBSSH2_ERROR_TIMEOUT;
            return LIBSSH2_ERROR_TIMEOUT;
        }
        ms_to_next = (session->api_timeout - elapsed_ms);
        has_timeout = 1;
    }
    else if (ms_to_next > 0) {
        has_timeout = 1;
    }
    else
        has_timeout = 0;

#ifdef HAVE_POLL
    {
        struct pollfd sockets[1];

        sockets[0].fd = session->socket_fd;
        sockets[0].events = 0;
        sockets[0].revents = 0;

        if(dir & LIBSSH2_SESSION_BLOCK_INBOUND)
            sockets[0].events |= POLLIN;

        if(dir & LIBSSH2_SESSION_BLOCK_OUTBOUND)
            sockets[0].events |= POLLOUT;

        rc = poll(sockets, 1, has_timeout?ms_to_next: -1);
    }
#else
    {
        fd_set rfd;
        fd_set wfd;
        fd_set *writefd = NULL;
        fd_set *readfd = NULL;
        struct timeval tv;

        tv.tv_sec = ms_to_next / 1000;
        tv.tv_usec = (ms_to_next - tv.tv_sec*1000) * 1000;

        if(dir & LIBSSH2_SESSION_BLOCK_INBOUND) {
            FD_ZERO(&rfd);
            FD_SET(session->socket_fd, &rfd);
            readfd = &rfd;
        }

        if(dir & LIBSSH2_SESSION_BLOCK_OUTBOUND) {
            FD_ZERO(&wfd);
            FD_SET(session->socket_fd, &wfd);
            writefd = &wfd;
        }

        rc = select(session->socket_fd + 1, readfd, writefd, NULL,
                    has_timeout ? &tv : NULL);
    }
#endif
    if(rc <= 0) {
        /* timeout (or error), bail out with a timeout error */
        session->err_code = LIBSSH2_ERROR_TIMEOUT;
        return LIBSSH2_ERROR_TIMEOUT;
    }

    return 0; /* ready to try again */
}

static int
session_startup(LIBSSH2_SESSION *session, libssh2_socket_t sock)
{
    int rc;

    if (session->startup_state == libssh2_NB_state_idle) {
        _libssh2_debug(session, LIBSSH2_TRACE_TRANS,
                       "session_startup for socket %d", sock);
        if (LIBSSH2_INVALID_SOCKET == sock) {
            /* Did we forget something? */
            return _libssh2_error(session, LIBSSH2_ERROR_BAD_SOCKET,
                                  "Bad socket provided");
        }
        session->socket_fd = sock;

        session->socket_prev_blockstate =
            !get_socket_nonblocking(session->socket_fd);

        if (session->socket_prev_blockstate) {
            /* If in blocking state chang to non-blocking */
            session_nonblock(session->socket_fd, 1);
        }

        session->startup_state = libssh2_NB_state_created;
    }

    if (session->startup_state == libssh2_NB_state_created) {
        rc = banner_send(session);
        if (rc) {
            return _libssh2_error(session, rc,
                                  "Failed sending banner");
        }
        session->startup_state = libssh2_NB_state_sent;
        session->banner_TxRx_state = libssh2_NB_state_idle;
    }

    if (session->startup_state == libssh2_NB_state_sent) {
        do {
            rc = banner_receive(session);
            if (rc)
                return _libssh2_error(session, rc,
                                      "Failed getting banner");
        } while(strncmp("SSH-", (char *)session->remote.banner, 4));

        session->startup_state = libssh2_NB_state_sent1;
    }

    if (session->startup_state == libssh2_NB_state_sent1) {
        rc = _libssh2_kex_exchange(session, 0, &session->startup_key_state);
        if (rc)
            return _libssh2_error(session, rc,
                                  "Unable to exchange encryption keys");

        session->startup_state = libssh2_NB_state_sent2;
    }

    if (session->startup_state == libssh2_NB_state_sent2) {
        _libssh2_debug(session, LIBSSH2_TRACE_TRANS,
                       "Requesting userauth service");

        /* Request the userauth service */
        session->startup_service[0] = SSH_MSG_SERVICE_REQUEST;
        _libssh2_htonu32(session->startup_service + 1,
                         sizeof("ssh-userauth") - 1);
        memcpy(session->startup_service + 5, "ssh-userauth",
               sizeof("ssh-userauth") - 1);

        session->startup_state = libssh2_NB_state_sent3;
    }

    if (session->startup_state == libssh2_NB_state_sent3) {
        rc = _libssh2_transport_send(session, session->startup_service,
                                     sizeof("ssh-userauth") + 5 - 1,
                                     NULL, 0);
        if (rc) {
            return _libssh2_error(session, rc,
                                  "Unable to ask for ssh-userauth service");
        }

        session->startup_state = libssh2_NB_state_sent4;
    }

    if (session->startup_state == libssh2_NB_state_sent4) {
        rc = _libssh2_packet_require(session, SSH_MSG_SERVICE_ACCEPT,
                                     &session->startup_data,
                                     &session->startup_data_len, 0, NULL, 0,
                                     &session->startup_req_state);
        if (rc)
            return rc;

        session->startup_service_length =
            _libssh2_ntohu32(session->startup_data + 1);

        if ((session->startup_service_length != (sizeof("ssh-userauth") - 1))
            || strncmp("ssh-userauth", (char *) session->startup_data + 5,
                       session->startup_service_length)) {
            LIBSSH2_FREE(session, session->startup_data);
            session->startup_data = NULL;
            return _libssh2_error(session, LIBSSH2_ERROR_PROTO,
                                  "Invalid response received from server");
        }
        LIBSSH2_FREE(session, session->startup_data);
        session->startup_data = NULL;

        session->startup_state = libssh2_NB_state_idle;

        return 0;
    }

    /* just for safety return some error */
    return LIBSSH2_ERROR_INVAL;
}

/*
 * libssh2_session_handshake()
 *
 * session: LIBSSH2_SESSION struct allocated and owned by the calling program
 * sock:    *must* be populated with an opened and connected socket.
 *
 * Returns: 0 on success, or non-zero on failure
 */
LIBSSH2_API int
libssh2_session_handshake(LIBSSH2_SESSION *session, libssh2_socket_t sock)
{
    int rc;

    BLOCK_ADJUST(rc, session, session_startup(session, sock) );

    return rc;
}

/*
 * libssh2_session_startup()
 *
 * DEPRECATED. Use libssh2_session_handshake() instead! This function is not
 * portable enough.
 *
 * session: LIBSSH2_SESSION struct allocated and owned by the calling program
 * sock:    *must* be populated with an opened and connected socket.
 *
 * Returns: 0 on success, or non-zero on failure
 */
LIBSSH2_API int
libssh2_session_startup(LIBSSH2_SESSION *session, int sock)
{
    return libssh2_session_handshake(session, (libssh2_socket_t) sock);
}

/*
 * libssh2_session_free
 *
 * Frees the memory allocated to the session
 * Also closes and frees any channels attached to this session
 */
static int
session_free(LIBSSH2_SESSION *session)
{
    int rc;
    LIBSSH2_PACKET *pkg;
    LIBSSH2_CHANNEL *ch;
    LIBSSH2_LISTENER *l;
    int packets_left = 0;

    if (session->free_state == libssh2_NB_state_idle) {
        _libssh2_debug(session, LIBSSH2_TRACE_TRANS, "Freeing session resource",
                       session->remote.banner);

        session->free_state = libssh2_NB_state_created;
    }

    if (session->free_state == libssh2_NB_state_created) {
        while ((ch = _libssh2_list_first(&session->channels))) {

            rc = _libssh2_channel_free(ch);
            if (rc == LIBSSH2_ERROR_EAGAIN)
                return rc;
        }

        session->free_state = libssh2_NB_state_sent;
    }

    if (session->free_state == libssh2_NB_state_sent) {
        while ((l = _libssh2_list_first(&session->listeners))) {
            rc = _libssh2_channel_forward_cancel(l);
            if (rc == LIBSSH2_ERROR_EAGAIN)
                return rc;
        }

        session->free_state = libssh2_NB_state_sent1;
    }

    if (session->state & LIBSSH2_STATE_NEWKEYS) {
        /* hostkey */
        if (session->hostkey && session->hostkey->dtor) {
            session->hostkey->dtor(session, &session->server_hostkey_abstract);
        }

        /* Client to Server */
        /* crypt */
        if (session->local.crypt && session->local.crypt->dtor) {
            session->local.crypt->dtor(session,
                                       &session->local.crypt_abstract);
        }
        /* comp */
        if (session->local.comp && session->local.comp->dtor) {
            session->local.comp->dtor(session, 1,
                                      &session->local.comp_abstract);
        }
        /* mac */
        if (session->local.mac && session->local.mac->dtor) {
            session->local.mac->dtor(session, &session->local.mac_abstract);
        }

        /* Server to Client */
        /* crypt */
        if (session->remote.crypt && session->remote.crypt->dtor) {
            session->remote.crypt->dtor(session,
                                        &session->remote.crypt_abstract);
        }
        /* comp */
        if (session->remote.comp && session->remote.comp->dtor) {
            session->remote.comp->dtor(session, 0,
                                       &session->remote.comp_abstract);
        }
        /* mac */
        if (session->remote.mac && session->remote.mac->dtor) {
            session->remote.mac->dtor(session, &session->remote.mac_abstract);
        }

        /* session_id */
        if (session->session_id) {
            LIBSSH2_FREE(session, session->session_id);
        }
    }

    /* Free banner(s) */
    if (session->remote.banner) {
        LIBSSH2_FREE(session, session->remote.banner);
    }
    if (session->local.banner) {
        LIBSSH2_FREE(session, session->local.banner);
    }

    /* Free preference(s) */
    if (session->kex_prefs) {
        LIBSSH2_FREE(session, session->kex_prefs);
    }
    if (session->hostkey_prefs) {
        LIBSSH2_FREE(session, session->hostkey_prefs);
    }

    if (session->local.kexinit) {
        LIBSSH2_FREE(session, session->local.kexinit);
    }
    if (session->local.crypt_prefs) {
        LIBSSH2_FREE(session, session->local.crypt_prefs);
    }
    if (session->local.mac_prefs) {
        LIBSSH2_FREE(session, session->local.mac_prefs);
    }
    if (session->local.comp_prefs) {
        LIBSSH2_FREE(session, session->local.comp_prefs);
    }
    if (session->local.lang_prefs) {
        LIBSSH2_FREE(session, session->local.lang_prefs);
    }

    if (session->remote.kexinit) {
        LIBSSH2_FREE(session, session->remote.kexinit);
    }
    if (session->remote.crypt_prefs) {
        LIBSSH2_FREE(session, session->remote.crypt_prefs);
    }
    if (session->remote.mac_prefs) {
        LIBSSH2_FREE(session, session->remote.mac_prefs);
    }
    if (session->remote.comp_prefs) {
        LIBSSH2_FREE(session, session->remote.comp_prefs);
    }
    if (session->remote.lang_prefs) {
        LIBSSH2_FREE(session, session->remote.lang_prefs);
    }

    /*
     * Make sure all memory used in the state variables are free
     */
    if (session->kexinit_data) {
        LIBSSH2_FREE(session, session->kexinit_data);
    }
    if (session->startup_data) {
        LIBSSH2_FREE(session, session->startup_data);
    }
    if (session->userauth_list_data) {
        LIBSSH2_FREE(session, session->userauth_list_data);
    }
    if (session->userauth_pswd_data) {
        LIBSSH2_FREE(session, session->userauth_pswd_data);
    }
    if (session->userauth_pswd_newpw) {
        LIBSSH2_FREE(session, session->userauth_pswd_newpw);
    }
    if (session->userauth_host_packet) {
        LIBSSH2_FREE(session, session->userauth_host_packet);
    }
    if (session->userauth_host_method) {
        LIBSSH2_FREE(session, session->userauth_host_method);
    }
    if (session->userauth_host_data) {
        LIBSSH2_FREE(session, session->userauth_host_data);
    }
    if (session->userauth_pblc_data) {
        LIBSSH2_FREE(session, session->userauth_pblc_data);
    }
    if (session->userauth_pblc_packet) {
        LIBSSH2_FREE(session, session->userauth_pblc_packet);
    }
    if (session->userauth_pblc_method) {
        LIBSSH2_FREE(session, session->userauth_pblc_method);
    }
    if (session->userauth_kybd_data) {
        LIBSSH2_FREE(session, session->userauth_kybd_data);
    }
    if (session->userauth_kybd_packet) {
        LIBSSH2_FREE(session, session->userauth_kybd_packet);
    }
    if (session->userauth_kybd_auth_instruction) {
        LIBSSH2_FREE(session, session->userauth_kybd_auth_instruction);
    }
    if (session->open_packet) {
        LIBSSH2_FREE(session, session->open_packet);
    }
    if (session->open_data) {
        LIBSSH2_FREE(session, session->open_data);
    }
    if (session->direct_message) {
        LIBSSH2_FREE(session, session->direct_message);
    }
    if (session->fwdLstn_packet) {
        LIBSSH2_FREE(session, session->fwdLstn_packet);
    }
    if (session->pkeyInit_data) {
        LIBSSH2_FREE(session, session->pkeyInit_data);
    }
    if (session->scpRecv_command) {
        LIBSSH2_FREE(session, session->scpRecv_command);
    }
    if (session->scpSend_command) {
        LIBSSH2_FREE(session, session->scpSend_command);
    }

    /* Cleanup all remaining packets */
    while ((pkg = _libssh2_list_first(&session->packets))) {
        packets_left++;
        _libssh2_debug(session, LIBSSH2_TRACE_TRANS,
            "packet left with id %d", pkg->data[0]);
        /* unlink the node */
        _libssh2_list_remove(&pkg->node);

        /* free */
        LIBSSH2_FREE(session, pkg->data);
        LIBSSH2_FREE(session, pkg);
    }
    _libssh2_debug(session, LIBSSH2_TRACE_TRANS,
         "Extra packets left %d", packets_left);

    if(session->socket_prev_blockstate)
        /* if the socket was previously blocking, put it back so */
        session_nonblock(session->socket_fd, 0);

    if (session->server_hostkey) {
        LIBSSH2_FREE(session, session->server_hostkey);
    }

    LIBSSH2_FREE(session, session);

    return 0;
}

/*
 * libssh2_session_free
 *
 * Frees the memory allocated to the session
 * Also closes and frees any channels attached to this session
 */
LIBSSH2_API int
libssh2_session_free(LIBSSH2_SESSION * session)
{
    int rc;

    BLOCK_ADJUST(rc, session, session_free(session) );

    return rc;
}

/*
 * libssh2_session_disconnect_ex
 */
static int
session_disconnect(LIBSSH2_SESSION *session, int reason,
                   const char *description,
                   const char *lang)
{
    unsigned char *s;
    unsigned long descr_len = 0, lang_len = 0;
    int rc;

    if (session->disconnect_state == libssh2_NB_state_idle) {
        _libssh2_debug(session, LIBSSH2_TRACE_TRANS,
                       "Disconnecting: reason=%d, desc=%s, lang=%s", reason,
                       description, lang);
        if (description)
            descr_len = strlen(description);

        if (lang)
            lang_len = strlen(lang);

        if(descr_len > 256)
            return _libssh2_error(session, LIBSSH2_ERROR_INVAL,
                                  "too long description");

        /* 13 = packet_type(1) + reason code(4) + descr_len(4) + lang_len(4) */
        session->disconnect_data_len = descr_len + lang_len + 13;

        s = session->disconnect_data;

        *(s++) = SSH_MSG_DISCONNECT;
        _libssh2_store_u32(&s, reason);
        _libssh2_store_str(&s, description, descr_len);
        /* store length only, lang is sent separately */
        _libssh2_store_u32(&s, lang_len);

        session->disconnect_state = libssh2_NB_state_created;
    }

    rc = _libssh2_transport_send(session, session->disconnect_data,
                                 session->disconnect_data_len,
                                 (unsigned char *)lang, lang_len);
    if (rc == LIBSSH2_ERROR_EAGAIN)
        return rc;

    session->disconnect_state = libssh2_NB_state_idle;

    return 0;
}

/*
 * libssh2_session_disconnect_ex
 */
LIBSSH2_API int
libssh2_session_disconnect_ex(LIBSSH2_SESSION *session, int reason,
                              const char *desc, const char *lang)
{
    int rc;

    BLOCK_ADJUST(rc, session,
                 session_disconnect(session, reason, desc, lang));

    return rc;
}

/* libssh2_session_methods
 *
 * Return the currently active methods for method_type
 *
 * NOTE: Currently lang_cs and lang_sc are ALWAYS set to empty string
 * regardless of actual negotiation Strings should NOT be freed
 */
LIBSSH2_API const char *
libssh2_session_methods(LIBSSH2_SESSION * session, int method_type)
{
    /* All methods have char *name as their first element */
    const LIBSSH2_KEX_METHOD *method = NULL;

    switch (method_type) {
    case LIBSSH2_METHOD_KEX:
        method = session->kex;
        break;

    case LIBSSH2_METHOD_HOSTKEY:
        method = (LIBSSH2_KEX_METHOD *) session->hostkey;
        break;

    case LIBSSH2_METHOD_CRYPT_CS:
        method = (LIBSSH2_KEX_METHOD *) session->local.crypt;
        break;

    case LIBSSH2_METHOD_CRYPT_SC:
        method = (LIBSSH2_KEX_METHOD *) session->remote.crypt;
        break;

    case LIBSSH2_METHOD_MAC_CS:
        method = (LIBSSH2_KEX_METHOD *) session->local.mac;
        break;

    case LIBSSH2_METHOD_MAC_SC:
        method = (LIBSSH2_KEX_METHOD *) session->remote.mac;
        break;

    case LIBSSH2_METHOD_COMP_CS:
        method = (LIBSSH2_KEX_METHOD *) session->local.comp;
        break;

    case LIBSSH2_METHOD_COMP_SC:
        method = (LIBSSH2_KEX_METHOD *) session->remote.comp;
        break;

    case LIBSSH2_METHOD_LANG_CS:
        return "";

    case LIBSSH2_METHOD_LANG_SC:
        return "";

    default:
        _libssh2_error(session, LIBSSH2_ERROR_INVAL,
                       "Invalid parameter specified for method_type");
        return NULL;
    }

    if (!method) {
        _libssh2_error(session, LIBSSH2_ERROR_METHOD_NONE,
                       "No method negotiated");
        return NULL;
    }

    return method->name;
}

/* libssh2_session_abstract
 * Retrieve a pointer to the abstract property
 */
LIBSSH2_API void **
libssh2_session_abstract(LIBSSH2_SESSION * session)
{
    return &session->abstract;
}

/* libssh2_session_last_error
 *
 * Returns error code and populates an error string into errmsg If want_buf is
 * non-zero then the string placed into errmsg must be freed by the calling
 * program. Otherwise it is assumed to be owned by libssh2
 */
LIBSSH2_API int
libssh2_session_last_error(LIBSSH2_SESSION * session, char **errmsg,
                           int *errmsg_len, int want_buf)
{
    size_t msglen = 0;

    /* No error to report */
    if (!session->err_code) {
        if (errmsg) {
            if (want_buf) {
                *errmsg = LIBSSH2_ALLOC(session, 1);
                if (*errmsg) {
                    **errmsg = 0;
                }
            } else {
                *errmsg = (char *) "";
            }
        }
        if (errmsg_len) {
            *errmsg_len = 0;
        }
        return 0;
    }

    if (errmsg) {
        const char *error = session->err_msg ? session->err_msg : "";

        msglen = strlen(error);

        if (want_buf) {
            /* Make a copy so the calling program can own it */
            *errmsg = LIBSSH2_ALLOC(session, msglen + 1);
            if (*errmsg) {
                memcpy(*errmsg, error, msglen);
                (*errmsg)[msglen] = 0;
            }
        }
        else
            *errmsg = (char *)error;
    }

    if (errmsg_len) {
        *errmsg_len = msglen;
    }

    return session->err_code;
}

/* libssh2_session_last_errno
 *
 * Returns error code
 */
LIBSSH2_API int
libssh2_session_last_errno(LIBSSH2_SESSION * session)
{
    return session->err_code;
}

/* libssh2_session_flag
 *
 * Set/Get session flags
 *
 * Return error code.
 */
LIBSSH2_API int
libssh2_session_flag(LIBSSH2_SESSION * session, int flag, int value)
{
    switch(flag) {
    case LIBSSH2_FLAG_SIGPIPE:
        session->flag.sigpipe = value;
        break;
    case LIBSSH2_FLAG_COMPRESS:
        session->flag.compress = value;
        break;
    default:
        /* unknown flag */
        return LIBSSH2_ERROR_INVAL;
    }

    return LIBSSH2_ERROR_NONE;
}

/* _libssh2_session_set_blocking
 *
 * Set a session's blocking mode on or off, return the previous status when
 * this function is called. Note this function does not alter the state of the
 * actual socket involved.
 */
int
_libssh2_session_set_blocking(LIBSSH2_SESSION *session, int blocking)
{
    int bl = session->api_block_mode;
    _libssh2_debug(session, LIBSSH2_TRACE_CONN,
                   "Setting blocking mode %s", blocking?"ON":"OFF");
    session->api_block_mode = blocking;

    return bl;
}

/* libssh2_session_set_blocking
 *
 * Set a channel's blocking mode on or off, similar to a socket's
 * fcntl(fd, F_SETFL, O_NONBLOCK); type command
 */
LIBSSH2_API void
libssh2_session_set_blocking(LIBSSH2_SESSION * session, int blocking)
{
    (void) _libssh2_session_set_blocking(session, blocking);
}

/* libssh2_session_get_blocking
 *
 * Returns a session's blocking mode on or off
 */
LIBSSH2_API int
libssh2_session_get_blocking(LIBSSH2_SESSION * session)
{
    return session->api_block_mode;
}


/* libssh2_session_set_timeout
 *
 * Set a session's timeout (in msec) for blocking mode,
 * or 0 to disable timeouts.
 */
LIBSSH2_API void
libssh2_session_set_timeout(LIBSSH2_SESSION * session, long timeout)
{
    session->api_timeout = timeout;
}

/* libssh2_session_get_timeout
 *
 * Returns a session's timeout, or 0 if disabled
 */
LIBSSH2_API long
libssh2_session_get_timeout(LIBSSH2_SESSION * session)
{
    return session->api_timeout;
}

/*
 * libssh2_poll_channel_read
 *
 * Returns 0 if no data is waiting on channel,
 * non-0 if data is available
 */
LIBSSH2_API int
libssh2_poll_channel_read(LIBSSH2_CHANNEL *channel, int extended)
{
    LIBSSH2_SESSION *session;
    LIBSSH2_PACKET *packet;

    if(!channel)
        return LIBSSH2_ERROR_BAD_USE;

    session = channel->session;
    packet = _libssh2_list_first(&session->packets);

    while (packet) {
        if ( channel->local.id == _libssh2_ntohu32(packet->data + 1)) {
            if ( extended == 1 &&
                 (packet->data[0] == SSH_MSG_CHANNEL_EXTENDED_DATA
                  || packet->data[0] == SSH_MSG_CHANNEL_DATA )) {
                return 1;
            } else if ( extended == 0 &&
                        packet->data[0] == SSH_MSG_CHANNEL_DATA) {
                return 1;
            }
            /* else - no data of any type is ready to be read */
        }
        packet = _libssh2_list_next(&packet->node);
    }

    return 0;
}

/*
 * poll_channel_write
 *
 * Returns 0 if writing to channel would block,
 * non-0 if data can be written without blocking
 */
static inline int
poll_channel_write(LIBSSH2_CHANNEL * channel)
{
    return channel->local.window_size ? 1 : 0;
}

/* poll_listener_queued
 *
 * Returns 0 if no connections are waiting to be accepted
 * non-0 if one or more connections are available
 */
static inline int
poll_listener_queued(LIBSSH2_LISTENER * listener)
{
    return _libssh2_list_first(&listener->queue) ? 1 : 0;
}

/*
 * libssh2_poll
 *
 * Poll sockets, channels, and listeners for activity
 */
LIBSSH2_API int
libssh2_poll(LIBSSH2_POLLFD * fds, unsigned int nfds, long timeout)
{
    long timeout_remaining;
    unsigned int i, active_fds;
#ifdef HAVE_POLL
    LIBSSH2_SESSION *session = NULL;
#ifdef HAVE_ALLOCA
    struct pollfd *sockets = alloca(sizeof(struct pollfd) * nfds);
#else
    struct pollfd sockets[256];

    if (nfds > 256)
        /* systems without alloca use a fixed-size array, this can be fixed if
           we really want to, at least if the compiler is a C99 capable one */
        return -1;
#endif
    /* Setup sockets for polling */
    for(i = 0; i < nfds; i++) {
        fds[i].revents = 0;
        switch (fds[i].type) {
        case LIBSSH2_POLLFD_SOCKET:
            sockets[i].fd = fds[i].fd.socket;
            sockets[i].events = fds[i].events;
            sockets[i].revents = 0;
            break;

        case LIBSSH2_POLLFD_CHANNEL:
            sockets[i].fd = fds[i].fd.channel->session->socket_fd;
            sockets[i].events = POLLIN;
            sockets[i].revents = 0;
            if (!session)
                session = fds[i].fd.channel->session;
            break;

        case LIBSSH2_POLLFD_LISTENER:
            sockets[i].fd = fds[i].fd.listener->session->socket_fd;
            sockets[i].events = POLLIN;
            sockets[i].revents = 0;
            if (!session)
                session = fds[i].fd.listener->session;
            break;

        default:
            if (session)
                _libssh2_error(session, LIBSSH2_ERROR_INVALID_POLL_TYPE,
                               "Invalid descriptor passed to libssh2_poll()");
            return -1;
        }
    }
#elif defined(HAVE_SELECT)
    LIBSSH2_SESSION *session = NULL;
    libssh2_socket_t maxfd = 0;
    fd_set rfds, wfds;
    struct timeval tv;

    FD_ZERO(&rfds);
    FD_ZERO(&wfds);
    for(i = 0; i < nfds; i++) {
        fds[i].revents = 0;
        switch (fds[i].type) {
        case LIBSSH2_POLLFD_SOCKET:
            if (fds[i].events & LIBSSH2_POLLFD_POLLIN) {
                FD_SET(fds[i].fd.socket, &rfds);
                if (fds[i].fd.socket > maxfd)
                    maxfd = fds[i].fd.socket;
            }
            if (fds[i].events & LIBSSH2_POLLFD_POLLOUT) {
                FD_SET(fds[i].fd.socket, &wfds);
                if (fds[i].fd.socket > maxfd)
                    maxfd = fds[i].fd.socket;
            }
            break;

        case LIBSSH2_POLLFD_CHANNEL:
            FD_SET(fds[i].fd.channel->session->socket_fd, &rfds);
            if (fds[i].fd.channel->session->socket_fd > maxfd)
                maxfd = fds[i].fd.channel->session->socket_fd;
            if (!session)
                session = fds[i].fd.channel->session;
            break;

        case LIBSSH2_POLLFD_LISTENER:
            FD_SET(fds[i].fd.listener->session->socket_fd, &rfds);
            if (fds[i].fd.listener->session->socket_fd > maxfd)
                maxfd = fds[i].fd.listener->session->socket_fd;
            if (!session)
                session = fds[i].fd.listener->session;
            break;

        default:
            if (session)
                _libssh2_error(session, LIBSSH2_ERROR_INVALID_POLL_TYPE,
                               "Invalid descriptor passed to libssh2_poll()");
            return -1;
        }
    }
#else
    /* No select() or poll()
     * no sockets sturcture to setup
     */

    timeout = 0;
#endif /* HAVE_POLL or HAVE_SELECT */

    timeout_remaining = timeout;
    do {
#if defined(HAVE_POLL) || defined(HAVE_SELECT)
        int sysret;
#endif

        active_fds = 0;

        for(i = 0; i < nfds; i++) {
            if (fds[i].events != fds[i].revents) {
                switch (fds[i].type) {
                case LIBSSH2_POLLFD_CHANNEL:
                    if ((fds[i].events & LIBSSH2_POLLFD_POLLIN) &&
                        /* Want to be ready for read */
                        ((fds[i].revents & LIBSSH2_POLLFD_POLLIN) == 0)) {
                        /* Not yet known to be ready for read */
                        fds[i].revents |=
                            libssh2_poll_channel_read(fds[i].fd.channel,
                                                      0) ?
                            LIBSSH2_POLLFD_POLLIN : 0;
                    }
                    if ((fds[i].events & LIBSSH2_POLLFD_POLLEXT) &&
                        /* Want to be ready for extended read */
                        ((fds[i].revents & LIBSSH2_POLLFD_POLLEXT) == 0)) {
                        /* Not yet known to be ready for extended read */
                        fds[i].revents |=
                            libssh2_poll_channel_read(fds[i].fd.channel,
                                                      1) ?
                            LIBSSH2_POLLFD_POLLEXT : 0;
                    }
                    if ((fds[i].events & LIBSSH2_POLLFD_POLLOUT) &&
                        /* Want to be ready for write */
                        ((fds[i].revents & LIBSSH2_POLLFD_POLLOUT) == 0)) {
                        /* Not yet known to be ready for write */
                        fds[i].revents |=
                            poll_channel_write(fds[i].fd. channel) ?
                            LIBSSH2_POLLFD_POLLOUT : 0;
                    }
                    if (fds[i].fd.channel->remote.close
                        || fds[i].fd.channel->local.close) {
                        fds[i].revents |= LIBSSH2_POLLFD_CHANNEL_CLOSED;
                    }
                    if (fds[i].fd.channel->session->socket_state ==
                        LIBSSH2_SOCKET_DISCONNECTED) {
                        fds[i].revents |=
                            LIBSSH2_POLLFD_CHANNEL_CLOSED |
                            LIBSSH2_POLLFD_SESSION_CLOSED;
                    }
                    break;

                case LIBSSH2_POLLFD_LISTENER:
                    if ((fds[i].events & LIBSSH2_POLLFD_POLLIN) &&
                        /* Want a connection */
                        ((fds[i].revents & LIBSSH2_POLLFD_POLLIN) == 0)) {
                        /* No connections known of yet */
                        fds[i].revents |=
                            poll_listener_queued(fds[i].fd. listener) ?
                            LIBSSH2_POLLFD_POLLIN : 0;
                    }
                    if (fds[i].fd.listener->session->socket_state ==
                        LIBSSH2_SOCKET_DISCONNECTED) {
                        fds[i].revents |=
                            LIBSSH2_POLLFD_LISTENER_CLOSED |
                            LIBSSH2_POLLFD_SESSION_CLOSED;
                    }
                    break;
                }
            }
            if (fds[i].revents) {
                active_fds++;
            }
        }

        if (active_fds) {
            /* Don't block on the sockets if we have channels/listeners which
               are ready */
            timeout_remaining = 0;
        }
#ifdef HAVE_POLL

#ifdef HAVE_LIBSSH2_GETTIMEOFDAY
        {
            struct timeval tv_begin, tv_end;

            _libssh2_gettimeofday((struct timeval *) &tv_begin, NULL);
            sysret = poll(sockets, nfds, timeout_remaining);
            _libssh2_gettimeofday((struct timeval *) &tv_end, NULL);
            timeout_remaining -= (tv_end.tv_sec - tv_begin.tv_sec) * 1000;
            timeout_remaining -= (tv_end.tv_usec - tv_begin.tv_usec) / 1000;
        }
#else
        /* If the platform doesn't support gettimeofday,
         * then just make the call non-blocking and walk away
         */
        sysret = poll(sockets, nfds, timeout_remaining);
        timeout_remaining = 0;
#endif /* HAVE_GETTIMEOFDAY */

        if (sysret > 0) {
            for(i = 0; i < nfds; i++) {
                switch (fds[i].type) {
                case LIBSSH2_POLLFD_SOCKET:
                    fds[i].revents = sockets[i].revents;
                    sockets[i].revents = 0; /* In case we loop again, be nice */
                    if (fds[i].revents) {
                        active_fds++;
                    }
                    break;
                case LIBSSH2_POLLFD_CHANNEL:
                    if (sockets[i].events & POLLIN) {
                        /* Spin session until no data available */
                        while (_libssh2_transport_read(fds[i].fd.channel->session)
                               > 0);
                    }
                    if (sockets[i].revents & POLLHUP) {
                        fds[i].revents |=
                            LIBSSH2_POLLFD_CHANNEL_CLOSED |
                            LIBSSH2_POLLFD_SESSION_CLOSED;
                    }
                    sockets[i].revents = 0;
                    break;
                case LIBSSH2_POLLFD_LISTENER:
                    if (sockets[i].events & POLLIN) {
                        /* Spin session until no data available */
                        while (_libssh2_transport_read(fds[i].fd.listener->session)
                               > 0);
                    }
                    if (sockets[i].revents & POLLHUP) {
                        fds[i].revents |=
                            LIBSSH2_POLLFD_LISTENER_CLOSED |
                            LIBSSH2_POLLFD_SESSION_CLOSED;
                    }
                    sockets[i].revents = 0;
                    break;
                }
            }
        }
#elif defined(HAVE_SELECT)
        tv.tv_sec = timeout_remaining / 1000;
        tv.tv_usec = (timeout_remaining % 1000) * 1000;
#ifdef HAVE_LIBSSH2_GETTIMEOFDAY
        {
            struct timeval tv_begin, tv_end;

            _libssh2_gettimeofday((struct timeval *) &tv_begin, NULL);
            sysret = select(maxfd+1, &rfds, &wfds, NULL, &tv);
            _libssh2_gettimeofday((struct timeval *) &tv_end, NULL);

            timeout_remaining -= (tv_end.tv_sec - tv_begin.tv_sec) * 1000;
            timeout_remaining -= (tv_end.tv_usec - tv_begin.tv_usec) / 1000;
        }
#else
        /* If the platform doesn't support gettimeofday,
         * then just make the call non-blocking and walk away
         */
        sysret = select(maxfd+1, &rfds, &wfds, NULL, &tv);
        timeout_remaining = 0;
#endif

        if (sysret > 0) {
            for(i = 0; i < nfds; i++) {
                switch (fds[i].type) {
                case LIBSSH2_POLLFD_SOCKET:
                    if (FD_ISSET(fds[i].fd.socket, &rfds)) {
                        fds[i].revents |= LIBSSH2_POLLFD_POLLIN;
                    }
                    if (FD_ISSET(fds[i].fd.socket, &wfds)) {
                        fds[i].revents |= LIBSSH2_POLLFD_POLLOUT;
                    }
                    if (fds[i].revents) {
                        active_fds++;
                    }
                    break;

                case LIBSSH2_POLLFD_CHANNEL:
                    if (FD_ISSET(fds[i].fd.channel->session->socket_fd, &rfds)) {
                        /* Spin session until no data available */
                        while (_libssh2_transport_read(fds[i].fd.channel->session)
                               > 0);
                    }
                    break;

                case LIBSSH2_POLLFD_LISTENER:
                    if (FD_ISSET
                        (fds[i].fd.listener->session->socket_fd, &rfds)) {
                        /* Spin session until no data available */
                        while (_libssh2_transport_read(fds[i].fd.listener->session)
                               > 0);
                    }
                    break;
                }
            }
        }
#endif /* else no select() or poll() -- timeout (and by extension
        * timeout_remaining) will be equal to 0 */
    } while ((timeout_remaining > 0) && !active_fds);

    return active_fds;
}

/*
 * libssh2_session_block_directions
 *
 * Get blocked direction when a function returns LIBSSH2_ERROR_EAGAIN
 * Returns LIBSSH2_SOCKET_BLOCK_INBOUND if recv() blocked
 * or LIBSSH2_SOCKET_BLOCK_OUTBOUND if send() blocked
 */
LIBSSH2_API int
libssh2_session_block_directions(LIBSSH2_SESSION *session)
{
    return session->socket_block_directions;
}

/* libssh2_session_banner_get
 * Get the remote banner (server ID string)
 */

LIBSSH2_API const char *
libssh2_session_banner_get(LIBSSH2_SESSION *session)
{
    /* to avoid a coredump when session is NULL */
    if (NULL == session)
        return NULL;

    if (NULL==session->remote.banner)
        return NULL;

    return (const char *) session->remote.banner;
}
