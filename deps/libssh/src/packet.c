/* Copyright (c) 2004-2007, Sara Golemon <sarag@libssh2.org>
 * Copyright (c) 2005,2006 Mikhail Gusarov
 * Copyright (c) 2009-2010 by Daniel Stenberg
 * Copyright (c) 2010 Simon Josefsson
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
#include <fcntl.h>

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#ifdef HAVE_SYS_TIME_H
#include <sys/time.h>
#endif

#ifdef HAVE_INTTYPES_H
#include <inttypes.h>
#endif

/* Needed for struct iovec on some platforms */
#ifdef HAVE_SYS_UIO_H
#include <sys/uio.h>
#endif

#include <sys/types.h>

#include "transport.h"
#include "channel.h"
#include "packet.h"

/*
 * libssh2_packet_queue_listener
 *
 * Queue a connection request for a listener
 */
static inline int
packet_queue_listener(LIBSSH2_SESSION * session, unsigned char *data,
                      unsigned long datalen,
                      packet_queue_listener_state_t *listen_state)
{
    /*
     * Look for a matching listener
     */
    /* 17 = packet_type(1) + channel(4) + reason(4) + descr(4) + lang(4) */
    unsigned long packet_len = 17 + (sizeof(FwdNotReq) - 1);
    unsigned char *p;
    LIBSSH2_LISTENER *listn = _libssh2_list_first(&session->listeners);
    char failure_code = SSH_OPEN_ADMINISTRATIVELY_PROHIBITED;
    int rc;

    (void) datalen;

    if (listen_state->state == libssh2_NB_state_idle) {
        unsigned char *s = data + (sizeof("forwarded-tcpip") - 1) + 5;
        listen_state->sender_channel = _libssh2_ntohu32(s);
        s += 4;

        listen_state->initial_window_size = _libssh2_ntohu32(s);
        s += 4;
        listen_state->packet_size = _libssh2_ntohu32(s);
        s += 4;

        listen_state->host_len = _libssh2_ntohu32(s);
        s += 4;
        listen_state->host = s;
        s += listen_state->host_len;
        listen_state->port = _libssh2_ntohu32(s);
        s += 4;

        listen_state->shost_len = _libssh2_ntohu32(s);
        s += 4;
        listen_state->shost = s;
        s += listen_state->shost_len;
        listen_state->sport = _libssh2_ntohu32(s);

        _libssh2_debug(session, LIBSSH2_TRACE_CONN,
                       "Remote received connection from %s:%ld to %s:%ld",
                       listen_state->shost, listen_state->sport,
                       listen_state->host, listen_state->port);

        listen_state->state = libssh2_NB_state_allocated;
    }

    if (listen_state->state != libssh2_NB_state_sent) {
        while (listn) {
            if ((listn->port == (int) listen_state->port) &&
                (strlen(listn->host) == listen_state->host_len) &&
                (memcmp (listn->host, listen_state->host,
                         listen_state->host_len) == 0)) {
                /* This is our listener */
                LIBSSH2_CHANNEL *channel = NULL;
                listen_state->channel = NULL;

                if (listen_state->state == libssh2_NB_state_allocated) {
                    if (listn->queue_maxsize &&
                        (listn->queue_maxsize <= listn->queue_size)) {
                        /* Queue is full */
                        failure_code = SSH_OPEN_RESOURCE_SHORTAGE;
                        _libssh2_debug(session, LIBSSH2_TRACE_CONN,
                                       "Listener queue full, ignoring");
                        listen_state->state = libssh2_NB_state_sent;
                        break;
                    }

                    channel = LIBSSH2_ALLOC(session, sizeof(LIBSSH2_CHANNEL));
                    if (!channel) {
                        _libssh2_error(session, LIBSSH2_ERROR_ALLOC,
                                       "Unable to allocate a channel for "
                                       "new connection");
                        failure_code = SSH_OPEN_RESOURCE_SHORTAGE;
                        listen_state->state = libssh2_NB_state_sent;
                        break;
                    }
                    listen_state->channel = channel;

                    memset(channel, 0, sizeof(LIBSSH2_CHANNEL));

                    channel->session = session;
                    channel->channel_type_len = sizeof("forwarded-tcpip") - 1;
                    channel->channel_type = LIBSSH2_ALLOC(session,
                                                          channel->
                                                          channel_type_len +
                                                          1);
                    if (!channel->channel_type) {
                        _libssh2_error(session, LIBSSH2_ERROR_ALLOC,
                                       "Unable to allocate a channel for new"
                                       " connection");
                        LIBSSH2_FREE(session, channel);
                        failure_code = SSH_OPEN_RESOURCE_SHORTAGE;
                        listen_state->state = libssh2_NB_state_sent;
                        break;
                    }
                    memcpy(channel->channel_type, "forwarded-tcpip",
                           channel->channel_type_len + 1);

                    channel->remote.id = listen_state->sender_channel;
                    channel->remote.window_size_initial =
                        LIBSSH2_CHANNEL_WINDOW_DEFAULT;
                    channel->remote.window_size =
                        LIBSSH2_CHANNEL_WINDOW_DEFAULT;
                    channel->remote.packet_size =
                        LIBSSH2_CHANNEL_PACKET_DEFAULT;

                    channel->local.id = _libssh2_channel_nextid(session);
                    channel->local.window_size_initial =
                        listen_state->initial_window_size;
                    channel->local.window_size =
                        listen_state->initial_window_size;
                    channel->local.packet_size = listen_state->packet_size;

                    _libssh2_debug(session, LIBSSH2_TRACE_CONN,
                                   "Connection queued: channel %lu/%lu "
                                   "win %lu/%lu packet %lu/%lu",
                                   channel->local.id, channel->remote.id,
                                   channel->local.window_size,
                                   channel->remote.window_size,
                                   channel->local.packet_size,
                                   channel->remote.packet_size);

                    p = listen_state->packet;
                    *(p++) = SSH_MSG_CHANNEL_OPEN_CONFIRMATION;
                    _libssh2_store_u32(&p, channel->remote.id);
                    _libssh2_store_u32(&p, channel->local.id);
                    _libssh2_store_u32(&p,
                                       channel->remote.window_size_initial);
                    _libssh2_store_u32(&p, channel->remote.packet_size);

                    listen_state->state = libssh2_NB_state_created;
                }

                if (listen_state->state == libssh2_NB_state_created) {
                    rc = _libssh2_transport_send(session, listen_state->packet,
                                                 17, NULL, 0);
                    if (rc == LIBSSH2_ERROR_EAGAIN)
                        return rc;
                    else if (rc) {
                        listen_state->state = libssh2_NB_state_idle;
                        return _libssh2_error(session, rc,
                                              "Unable to send channel "
                                              "open confirmation");
                    }

                    /* Link the channel into the end of the queue list */
                    _libssh2_list_add(&listn->queue,
                                      &listen_state->channel->node);
                    listn->queue_size++;

                    listen_state->state = libssh2_NB_state_idle;
                    return 0;
                }
            }

            listn = _libssh2_list_next(&listn->node);
        }

        listen_state->state = libssh2_NB_state_sent;
    }

    /* We're not listening to you */
    p = listen_state->packet;
    *(p++) = SSH_MSG_CHANNEL_OPEN_FAILURE;
    _libssh2_store_u32(&p, listen_state->sender_channel);
    _libssh2_store_u32(&p, failure_code);
    _libssh2_store_str(&p, FwdNotReq, sizeof(FwdNotReq) - 1);
    _libssh2_htonu32(p, 0);

    rc = _libssh2_transport_send(session, listen_state->packet,
                                 packet_len, NULL, 0);
    if (rc == LIBSSH2_ERROR_EAGAIN) {
        return rc;
    } else if (rc) {
        listen_state->state = libssh2_NB_state_idle;
        return _libssh2_error(session, rc, "Unable to send open failure");

    }
    listen_state->state = libssh2_NB_state_idle;
    return 0;
}

/*
 * packet_x11_open
 *
 * Accept a forwarded X11 connection
 */
static inline int
packet_x11_open(LIBSSH2_SESSION * session, unsigned char *data,
                unsigned long datalen,
                packet_x11_open_state_t *x11open_state)
{
    int failure_code = SSH_OPEN_CONNECT_FAILED;
    /* 17 = packet_type(1) + channel(4) + reason(4) + descr(4) + lang(4) */
    unsigned long packet_len = 17 + (sizeof(X11FwdUnAvil) - 1);
    unsigned char *p;
    LIBSSH2_CHANNEL *channel = x11open_state->channel;
    int rc;

    (void) datalen;

    if (x11open_state->state == libssh2_NB_state_idle) {
        unsigned char *s = data + (sizeof("x11") - 1) + 5;
        x11open_state->sender_channel = _libssh2_ntohu32(s);
        s += 4;
        x11open_state->initial_window_size = _libssh2_ntohu32(s);
        s += 4;
        x11open_state->packet_size = _libssh2_ntohu32(s);
        s += 4;
        x11open_state->shost_len = _libssh2_ntohu32(s);
        s += 4;
        x11open_state->shost = s;
        s += x11open_state->shost_len;
        x11open_state->sport = _libssh2_ntohu32(s);

        _libssh2_debug(session, LIBSSH2_TRACE_CONN,
                       "X11 Connection Received from %s:%ld on channel %lu",
                       x11open_state->shost, x11open_state->sport,
                       x11open_state->sender_channel);

        x11open_state->state = libssh2_NB_state_allocated;
    }

    if (session->x11) {
        if (x11open_state->state == libssh2_NB_state_allocated) {
            channel = LIBSSH2_ALLOC(session, sizeof(LIBSSH2_CHANNEL));
            if (!channel) {
                _libssh2_error(session, LIBSSH2_ERROR_ALLOC,
                               "allocate a channel for new connection");
                failure_code = SSH_OPEN_RESOURCE_SHORTAGE;
                goto x11_exit;
            }
            memset(channel, 0, sizeof(LIBSSH2_CHANNEL));

            channel->session = session;
            channel->channel_type_len = sizeof("x11") - 1;
            channel->channel_type = LIBSSH2_ALLOC(session,
                                                  channel->channel_type_len +
                                                  1);
            if (!channel->channel_type) {
                _libssh2_error(session, LIBSSH2_ERROR_ALLOC,
                               "allocate a channel for new connection");
                LIBSSH2_FREE(session, channel);
                failure_code = SSH_OPEN_RESOURCE_SHORTAGE;
                goto x11_exit;
            }
            memcpy(channel->channel_type, "x11",
                   channel->channel_type_len + 1);

            channel->remote.id = x11open_state->sender_channel;
            channel->remote.window_size_initial =
                LIBSSH2_CHANNEL_WINDOW_DEFAULT;
            channel->remote.window_size = LIBSSH2_CHANNEL_WINDOW_DEFAULT;
            channel->remote.packet_size = LIBSSH2_CHANNEL_PACKET_DEFAULT;

            channel->local.id = _libssh2_channel_nextid(session);
            channel->local.window_size_initial =
                x11open_state->initial_window_size;
            channel->local.window_size = x11open_state->initial_window_size;
            channel->local.packet_size = x11open_state->packet_size;

            _libssh2_debug(session, LIBSSH2_TRACE_CONN,
                           "X11 Connection established: channel %lu/%lu "
                           "win %lu/%lu packet %lu/%lu",
                           channel->local.id, channel->remote.id,
                           channel->local.window_size,
                           channel->remote.window_size,
                           channel->local.packet_size,
                           channel->remote.packet_size);
            p = x11open_state->packet;
            *(p++) = SSH_MSG_CHANNEL_OPEN_CONFIRMATION;
            _libssh2_store_u32(&p, channel->remote.id);
            _libssh2_store_u32(&p, channel->local.id);
            _libssh2_store_u32(&p, channel->remote.window_size_initial);
            _libssh2_store_u32(&p, channel->remote.packet_size);

            x11open_state->state = libssh2_NB_state_created;
        }

        if (x11open_state->state == libssh2_NB_state_created) {
            rc = _libssh2_transport_send(session, x11open_state->packet, 17,
                                         NULL, 0);
            if (rc == LIBSSH2_ERROR_EAGAIN) {
                return rc;
            } else if (rc) {
                x11open_state->state = libssh2_NB_state_idle;
                return _libssh2_error(session, LIBSSH2_ERROR_SOCKET_SEND,
                                      "Unable to send channel open "
                                      "confirmation");
            }

            /* Link the channel into the session */
            _libssh2_list_add(&session->channels, &channel->node);

            /*
             * Pass control to the callback, they may turn right around and
             * free the channel, or actually use it
             */
            LIBSSH2_X11_OPEN(channel, (char *)x11open_state->shost,
                             x11open_state->sport);

            x11open_state->state = libssh2_NB_state_idle;
            return 0;
        }
    }
    else
        failure_code = SSH_OPEN_RESOURCE_SHORTAGE;
    /* fall-trough */
  x11_exit:
    p = x11open_state->packet;
    *(p++) = SSH_MSG_CHANNEL_OPEN_FAILURE;
    _libssh2_store_u32(&p, x11open_state->sender_channel);
    _libssh2_store_u32(&p, failure_code);
    _libssh2_store_str(&p, X11FwdUnAvil, sizeof(X11FwdUnAvil) - 1);
    _libssh2_htonu32(p, 0);

    rc = _libssh2_transport_send(session, x11open_state->packet, packet_len,
                                 NULL, 0);
    if (rc == LIBSSH2_ERROR_EAGAIN) {
        return rc;
    } else if (rc) {
        x11open_state->state = libssh2_NB_state_idle;
        return _libssh2_error(session, rc, "Unable to send open failure");
    }
    x11open_state->state = libssh2_NB_state_idle;
    return 0;
}

/*
 * _libssh2_packet_add
 *
 * Create a new packet and attach it to the brigade. Called from the transport
 * layer when it has received a packet.
 *
 * The input pointer 'data' is pointing to allocated data that this function
 * is asked to deal with so on failure OR success, it must be freed fine.
 *
 * This function will always be called with 'datalen' greater than zero.
 */
int
_libssh2_packet_add(LIBSSH2_SESSION * session, unsigned char *data,
                    size_t datalen, int macstate)
{
    int rc = 0;
    char *message=NULL;
    char *language=NULL;
    size_t message_len=0;
    size_t language_len=0;
    LIBSSH2_CHANNEL *channelp = NULL;
    size_t data_head = 0;
    unsigned char msg = data[0];

    switch(session->packAdd_state) {
    case libssh2_NB_state_idle:
        _libssh2_debug(session, LIBSSH2_TRACE_TRANS,
                       "Packet type %d received, length=%d",
                       (int) msg, (int) datalen);

        if ((macstate == LIBSSH2_MAC_INVALID) &&
            (!session->macerror ||
             LIBSSH2_MACERROR(session, (char *) data, datalen))) {
            /* Bad MAC input, but no callback set or non-zero return from the
               callback */

            LIBSSH2_FREE(session, data);
            return _libssh2_error(session, LIBSSH2_ERROR_INVALID_MAC,
                                  "Invalid MAC received");
        }
        session->packAdd_state = libssh2_NB_state_allocated;
        break;
    case libssh2_NB_state_jump1:
        goto libssh2_packet_add_jump_point1;
    case libssh2_NB_state_jump2:
        goto libssh2_packet_add_jump_point2;
    case libssh2_NB_state_jump3:
        goto libssh2_packet_add_jump_point3;
    case libssh2_NB_state_jump4:
        goto libssh2_packet_add_jump_point4;
    case libssh2_NB_state_jump5:
        goto libssh2_packet_add_jump_point5;
    default: /* nothing to do */
        break;
    }

    if (session->packAdd_state == libssh2_NB_state_allocated) {
        /* A couple exceptions to the packet adding rule: */
        switch (msg) {

            /*
              byte      SSH_MSG_DISCONNECT
              uint32    reason code
              string    description in ISO-10646 UTF-8 encoding [RFC3629]
              string    language tag [RFC3066]
            */

        case SSH_MSG_DISCONNECT:
            if(datalen >= 5) {
                size_t reason = _libssh2_ntohu32(data + 1);

                if(datalen >= 9) {
                    message_len = _libssh2_ntohu32(data + 5);

                    if(message_len < datalen-13) {
                        /* 9 = packet_type(1) + reason(4) + message_len(4) */
                        message = (char *) data + 9;

                        language_len = _libssh2_ntohu32(data + 9 + message_len);
                        language = (char *) data + 9 + message_len + 4;

                        if(language_len > (datalen-13-message_len)) {
                            /* bad input, clear info */
                            language = message = NULL;
                            language_len = message_len = 0;
                        }
                    }
                    else
                        /* bad size, clear it */
                        message_len=0;
                }
                if (session->ssh_msg_disconnect) {
                    LIBSSH2_DISCONNECT(session, reason, message,
                                       message_len, language, language_len);
                }
                _libssh2_debug(session, LIBSSH2_TRACE_TRANS,
                               "Disconnect(%d): %s(%s)", reason,
                               message, language);
            }

            LIBSSH2_FREE(session, data);
            session->socket_state = LIBSSH2_SOCKET_DISCONNECTED;
            session->packAdd_state = libssh2_NB_state_idle;
            return _libssh2_error(session, LIBSSH2_ERROR_SOCKET_DISCONNECT,
                                  "socket disconnect");
            /*
              byte      SSH_MSG_IGNORE
              string    data
            */

        case SSH_MSG_IGNORE:
            if (datalen >= 2) {
                if (session->ssh_msg_ignore) {
                    LIBSSH2_IGNORE(session, (char *) data + 1, datalen - 1);
                }
            } else if (session->ssh_msg_ignore) {
                LIBSSH2_IGNORE(session, "", 0);
            }
            LIBSSH2_FREE(session, data);
            session->packAdd_state = libssh2_NB_state_idle;
            return 0;

            /*
              byte      SSH_MSG_DEBUG
              boolean   always_display
              string    message in ISO-10646 UTF-8 encoding [RFC3629]
              string    language tag [RFC3066]
            */

        case SSH_MSG_DEBUG:
            if(datalen >= 2) {
                int always_display= data[1];

                if(datalen >= 6) {
                    message_len = _libssh2_ntohu32(data + 2);

                    if(message_len <= (datalen - 10)) {
                        /* 6 = packet_type(1) + display(1) + message_len(4) */
                        message = (char *) data + 6;
                        language_len = _libssh2_ntohu32(data + 6 + message_len);

                        if(language_len <= (datalen - 10 - message_len))
                            language = (char *) data + 10 + message_len;
                    }
                }

                if (session->ssh_msg_debug) {
                    LIBSSH2_DEBUG(session, always_display, message,
                                  message_len, language, language_len);
                }
            }
            /*
             * _libssh2_debug will actually truncate this for us so
             * that it's not an inordinate about of data
             */
            _libssh2_debug(session, LIBSSH2_TRACE_TRANS,
                           "Debug Packet: %s", message);
            LIBSSH2_FREE(session, data);
            session->packAdd_state = libssh2_NB_state_idle;
            return 0;

            /*
              byte      SSH_MSG_GLOBAL_REQUEST
              string    request name in US-ASCII only
              boolean   want reply
              ....      request-specific data follows
            */

        case SSH_MSG_GLOBAL_REQUEST:
            if(datalen >= 5) {
                uint32_t len =0;
                unsigned char want_reply=0;
                len = _libssh2_ntohu32(data + 1);
                if(datalen >= (6 + len)) {
                    want_reply = data[5 + len];
                    _libssh2_debug(session,
                                   LIBSSH2_TRACE_CONN,
                                   "Received global request type %.*s (wr %X)",
                                   len, data + 5, want_reply);
                }


                if (want_reply) {
                    unsigned char packet = SSH_MSG_REQUEST_FAILURE;
                  libssh2_packet_add_jump_point5:
                    session->packAdd_state = libssh2_NB_state_jump5;
                    rc = _libssh2_transport_send(session, &packet, 1, NULL, 0);
                    if (rc == LIBSSH2_ERROR_EAGAIN)
                        return rc;
                }
            }
            LIBSSH2_FREE(session, data);
            session->packAdd_state = libssh2_NB_state_idle;
            return 0;

            /*
              byte      SSH_MSG_CHANNEL_EXTENDED_DATA
              uint32    recipient channel
              uint32    data_type_code
              string    data
            */

        case SSH_MSG_CHANNEL_EXTENDED_DATA:
            /* streamid(4) */
            data_head += 4;

            /* fall-through */

            /*
              byte      SSH_MSG_CHANNEL_DATA
              uint32    recipient channel
              string    data
            */

        case SSH_MSG_CHANNEL_DATA:
            /* packet_type(1) + channelno(4) + datalen(4) */
            data_head += 9;

            if(datalen >= data_head)
                channelp =
                    _libssh2_channel_locate(session,
                                            _libssh2_ntohu32(data + 1));

            if (!channelp) {
                _libssh2_error(session, LIBSSH2_ERROR_CHANNEL_UNKNOWN,
                               "Packet received for unknown channel");
                LIBSSH2_FREE(session, data);
                session->packAdd_state = libssh2_NB_state_idle;
                return 0;
            }
#ifdef LIBSSH2DEBUG
            {
                uint32_t stream_id = 0;
                if (msg == SSH_MSG_CHANNEL_EXTENDED_DATA)
                    stream_id = _libssh2_ntohu32(data + 5);

                _libssh2_debug(session, LIBSSH2_TRACE_CONN,
                               "%d bytes packet_add() for %lu/%lu/%lu",
                               (int) (datalen - data_head),
                               channelp->local.id,
                               channelp->remote.id,
                               stream_id);
            }
#endif
            if ((channelp->remote.extended_data_ignore_mode ==
                 LIBSSH2_CHANNEL_EXTENDED_DATA_IGNORE) &&
                (msg == SSH_MSG_CHANNEL_EXTENDED_DATA)) {
                /* Pretend we didn't receive this */
                LIBSSH2_FREE(session, data);

                _libssh2_debug(session, LIBSSH2_TRACE_CONN,
                               "Ignoring extended data and refunding %d bytes",
                               (int) (datalen - 13));
                session->packAdd_channelp = channelp;

                /* Adjust the window based on the block we just freed */
              libssh2_packet_add_jump_point1:
                session->packAdd_state = libssh2_NB_state_jump1;
                rc = _libssh2_channel_receive_window_adjust(session->
                                                            packAdd_channelp,
                                                            datalen - 13,
                                                            1, NULL);
                if (rc == LIBSSH2_ERROR_EAGAIN)
                    return rc;

                session->packAdd_state = libssh2_NB_state_idle;
                return 0;
            }

            /*
             * REMEMBER! remote means remote as source of data,
             * NOT remote window!
             */
            if (channelp->remote.packet_size < (datalen - data_head)) {
                /*
                 * Spec says we MAY ignore bytes sent beyond
                 * packet_size
                 */
                _libssh2_error(session,
                               LIBSSH2_ERROR_CHANNEL_PACKET_EXCEEDED,
                               "Packet contains more data than we offered"
                               " to receive, truncating");
                datalen = channelp->remote.packet_size + data_head;
            }
            if (channelp->remote.window_size <= 0) {
                /*
                 * Spec says we MAY ignore bytes sent beyond
                 * window_size
                 */
                _libssh2_error(session,
                               LIBSSH2_ERROR_CHANNEL_WINDOW_EXCEEDED,
                               "The current receive window is full,"
                               " data ignored");
                LIBSSH2_FREE(session, data);
                session->packAdd_state = libssh2_NB_state_idle;
                return 0;
            }
            /* Reset EOF status */
            channelp->remote.eof = 0;

            if ((datalen - data_head) > channelp->remote.window_size) {
                _libssh2_error(session,
                               LIBSSH2_ERROR_CHANNEL_WINDOW_EXCEEDED,
                               "Remote sent more data than current "
                               "window allows, truncating");
                datalen = channelp->remote.window_size + data_head;
                channelp->remote.window_size = 0;
            }
            else
                /* Now that we've received it, shrink our window */
                channelp->remote.window_size -= datalen - data_head;

            break;

            /*
              byte      SSH_MSG_CHANNEL_EOF
              uint32    recipient channel
            */

        case SSH_MSG_CHANNEL_EOF:
            if(datalen >= 5)
                channelp =
                    _libssh2_channel_locate(session,
                                            _libssh2_ntohu32(data + 1));
            if (!channelp)
                /* We may have freed already, just quietly ignore this... */
                ;
            else {
                _libssh2_debug(session,
                               LIBSSH2_TRACE_CONN,
                               "EOF received for channel %lu/%lu",
                               channelp->local.id,
                               channelp->remote.id);
                channelp->remote.eof = 1;
            }
            LIBSSH2_FREE(session, data);
            session->packAdd_state = libssh2_NB_state_idle;
            return 0;

            /*
              byte      SSH_MSG_CHANNEL_REQUEST
              uint32    recipient channel
              string    request type in US-ASCII characters only
              boolean   want reply
              ....      type-specific data follows
            */

        case SSH_MSG_CHANNEL_REQUEST:
            if(datalen >= 9) {
                uint32_t channel = _libssh2_ntohu32(data + 1);
                uint32_t len = _libssh2_ntohu32(data + 5);
                unsigned char want_reply = 1;

                if(len < (datalen - 10))
                    want_reply = data[9 + len];

                _libssh2_debug(session,
                               LIBSSH2_TRACE_CONN,
                               "Channel %d received request type %.*s (wr %X)",
                               channel, len, data + 9, want_reply);

                if (len == sizeof("exit-status") - 1
                    && !memcmp("exit-status", data + 9,
                               sizeof("exit-status") - 1)) {

                    /* we've got "exit-status" packet. Set the session value */
                    if(datalen >= 20)
                        channelp =
                            _libssh2_channel_locate(session, channel);

                    if (channelp) {
                        channelp->exit_status =
                            _libssh2_ntohu32(data + 9 + sizeof("exit-status"));
                        _libssh2_debug(session, LIBSSH2_TRACE_CONN,
                                       "Exit status %lu received for "
                                       "channel %lu/%lu",
                                       channelp->exit_status,
                                       channelp->local.id,
                                       channelp->remote.id);
                    }

                }
                else if (len == sizeof("exit-signal") - 1
                         && !memcmp("exit-signal", data + 9,
                                    sizeof("exit-signal") - 1)) {
                    /* command terminated due to signal */
                    if(datalen >= 20)
                        channelp = _libssh2_channel_locate(session, channel);

                    if (channelp) {
                        /* set signal name (without SIG prefix) */
                        uint32_t namelen =
                            _libssh2_ntohu32(data + 9 + sizeof("exit-signal"));
                        channelp->exit_signal =
                            LIBSSH2_ALLOC(session, namelen + 1);
                        if (!channelp->exit_signal)
                            rc = _libssh2_error(session, LIBSSH2_ERROR_ALLOC,
                                                "memory for signal name");
                        else {
                            memcpy(channelp->exit_signal,
                                   data + 13 + sizeof("exit_signal"), namelen);
                            channelp->exit_signal[namelen] = '\0';
                            /* TODO: save error message and language tag */
                            _libssh2_debug(session, LIBSSH2_TRACE_CONN,
                                           "Exit signal %s received for "
                                           "channel %lu/%lu",
                                           channelp->exit_signal,
                                           channelp->local.id,
                                           channelp->remote.id);
                        }
                    }
                }


                if (want_reply) {
                    unsigned char packet[5];
                  libssh2_packet_add_jump_point4:
                    session->packAdd_state = libssh2_NB_state_jump4;
                    packet[0] = SSH_MSG_CHANNEL_FAILURE;
                    memcpy(&packet[1], data+1, 4);
                    rc = _libssh2_transport_send(session, packet, 5, NULL, 0);
                    if (rc == LIBSSH2_ERROR_EAGAIN)
                        return rc;
                }
            }
            LIBSSH2_FREE(session, data);
            session->packAdd_state = libssh2_NB_state_idle;
            return rc;

            /*
              byte      SSH_MSG_CHANNEL_CLOSE
              uint32    recipient channel
            */

        case SSH_MSG_CHANNEL_CLOSE:
            if(datalen >= 5)
                channelp =
                    _libssh2_channel_locate(session,
                                            _libssh2_ntohu32(data + 1));
            if (!channelp) {
                /* We may have freed already, just quietly ignore this... */
                LIBSSH2_FREE(session, data);
                session->packAdd_state = libssh2_NB_state_idle;
                return 0;
            }
            _libssh2_debug(session, LIBSSH2_TRACE_CONN,
                           "Close received for channel %lu/%lu",
                           channelp->local.id,
                           channelp->remote.id);

            channelp->remote.close = 1;
            channelp->remote.eof = 1;

            LIBSSH2_FREE(session, data);
            session->packAdd_state = libssh2_NB_state_idle;
            return 0;

            /*
              byte      SSH_MSG_CHANNEL_OPEN
              string    "session"
              uint32    sender channel
              uint32    initial window size
              uint32    maximum packet size
            */

        case SSH_MSG_CHANNEL_OPEN:
            if(datalen < 17)
                ;
            else if ((datalen >= (sizeof("forwarded-tcpip") + 4)) &&
                     ((sizeof("forwarded-tcpip") - 1) ==
                      _libssh2_ntohu32(data + 1))
                     &&
                     (memcmp(data + 5, "forwarded-tcpip",
                             sizeof("forwarded-tcpip") - 1) == 0)) {

                /* init the state struct */
                memset(&session->packAdd_Qlstn_state, 0,
                       sizeof(session->packAdd_Qlstn_state));

              libssh2_packet_add_jump_point2:
                session->packAdd_state = libssh2_NB_state_jump2;
                rc = packet_queue_listener(session, data, datalen,
                                           &session->packAdd_Qlstn_state);
            }
            else if ((datalen >= (sizeof("x11") + 4)) &&
                     ((sizeof("x11") - 1) == _libssh2_ntohu32(data + 1)) &&
                     (memcmp(data + 5, "x11", sizeof("x11") - 1) == 0)) {

                /* init the state struct */
                memset(&session->packAdd_x11open_state, 0,
                       sizeof(session->packAdd_x11open_state));

              libssh2_packet_add_jump_point3:
                session->packAdd_state = libssh2_NB_state_jump3;
                rc = packet_x11_open(session, data, datalen,
                                     &session->packAdd_x11open_state);
            }
            if (rc == LIBSSH2_ERROR_EAGAIN)
                return rc;

            LIBSSH2_FREE(session, data);
            session->packAdd_state = libssh2_NB_state_idle;
            return rc;

            /*
              byte      SSH_MSG_CHANNEL_WINDOW_ADJUST
              uint32    recipient channel
              uint32    bytes to add
            */
        case SSH_MSG_CHANNEL_WINDOW_ADJUST:
            if(datalen < 9)
                ;
            else {
                uint32_t bytestoadd = _libssh2_ntohu32(data + 5);
                channelp =
                    _libssh2_channel_locate(session,
                                            _libssh2_ntohu32(data + 1));
                if(channelp) {
                    channelp->local.window_size += bytestoadd;

                    _libssh2_debug(session, LIBSSH2_TRACE_CONN,
                                   "Window adjust for channel %lu/%lu, "
                                   "adding %lu bytes, new window_size=%lu",
                                   channelp->local.id,
                                   channelp->remote.id,
                                   bytestoadd,
                                   channelp->local.window_size);
                }
            }
            LIBSSH2_FREE(session, data);
            session->packAdd_state = libssh2_NB_state_idle;
            return 0;
        default:
            break;
        }

        session->packAdd_state = libssh2_NB_state_sent;
    }

    if (session->packAdd_state == libssh2_NB_state_sent) {
        LIBSSH2_PACKET *packetp =
            LIBSSH2_ALLOC(session, sizeof(LIBSSH2_PACKET));
        if (!packetp) {
            _libssh2_debug(session, LIBSSH2_ERROR_ALLOC,
                           "memory for packet");
            session->packAdd_state = libssh2_NB_state_idle;
            return LIBSSH2_ERROR_ALLOC;
        }
        packetp->data = data;
        packetp->data_len = datalen;
        packetp->data_head = data_head;

        _libssh2_list_add(&session->packets, &packetp->node);

        session->packAdd_state = libssh2_NB_state_sent1;
    }

    if ((msg == SSH_MSG_KEXINIT &&
         !(session->state & LIBSSH2_STATE_EXCHANGING_KEYS)) ||
        (session->packAdd_state == libssh2_NB_state_sent2)) {
        if (session->packAdd_state == libssh2_NB_state_sent1) {
            /*
             * Remote wants new keys
             * Well, it's already in the brigade,
             * let's just call back into ourselves
             */
            _libssh2_debug(session, LIBSSH2_TRACE_TRANS, "Renegotiating Keys");

            session->packAdd_state = libssh2_NB_state_sent2;
        }

        /*
         * The KEXINIT message has been added to the queue.  The packAdd and
         * readPack states need to be reset because _libssh2_kex_exchange
         * (eventually) calls upon _libssh2_transport_read to read the rest of
         * the key exchange conversation.
         */
        session->readPack_state = libssh2_NB_state_idle;
        session->packet.total_num = 0;
        session->packAdd_state = libssh2_NB_state_idle;
        session->fullpacket_state = libssh2_NB_state_idle;

        memset(&session->startup_key_state, 0, sizeof(key_exchange_state_t));

        /*
         * If there was a key reexchange failure, let's just hope we didn't
         * send NEWKEYS yet, otherwise remote will drop us like a rock
         */
        rc = _libssh2_kex_exchange(session, 1, &session->startup_key_state);
        if (rc == LIBSSH2_ERROR_EAGAIN)
            return rc;
    }

    session->packAdd_state = libssh2_NB_state_idle;
    return 0;
}

/*
 * _libssh2_packet_ask
 *
 * Scan the brigade for a matching packet type, optionally poll the socket for
 * a packet first
 */
int
_libssh2_packet_ask(LIBSSH2_SESSION * session, unsigned char packet_type,
                    unsigned char **data, size_t *data_len,
                    int match_ofs, const unsigned char *match_buf,
                    size_t match_len)
{
    LIBSSH2_PACKET *packet = _libssh2_list_first(&session->packets);

    _libssh2_debug(session, LIBSSH2_TRACE_TRANS,
                   "Looking for packet of type: %d", (int) packet_type);

    while (packet) {
        if (packet->data[0] == packet_type
            && (packet->data_len >= (match_ofs + match_len))
            && (!match_buf ||
                (memcmp(packet->data + match_ofs, match_buf,
                        match_len) == 0))) {
            *data = packet->data;
            *data_len = packet->data_len;

            /* unlink struct from session->packets */
            _libssh2_list_remove(&packet->node);

            LIBSSH2_FREE(session, packet);

            return 0;
        }
        packet = _libssh2_list_next(&packet->node);
    }
    return -1;
}

/*
 * libssh2_packet_askv
 *
 * Scan for any of a list of packet types in the brigade, optionally poll the
 * socket for a packet first
 */
int
_libssh2_packet_askv(LIBSSH2_SESSION * session,
                     const unsigned char *packet_types,
                     unsigned char **data, size_t *data_len,
                     int match_ofs,
                     const unsigned char *match_buf,
                     size_t match_len)
{
    int i, packet_types_len = strlen((char *) packet_types);

    for(i = 0; i < packet_types_len; i++) {
        if (0 == _libssh2_packet_ask(session, packet_types[i], data,
                                     data_len, match_ofs,
                                     match_buf, match_len)) {
            return 0;
        }
    }

    return -1;
}

/*
 * _libssh2_packet_require
 *
 * Loops _libssh2_transport_read() until the packet requested is available
 * SSH_DISCONNECT or a SOCKET_DISCONNECTED will cause a bailout
 *
 * Returns negative on error
 * Returns 0 when it has taken care of the requested packet.
 */
int
_libssh2_packet_require(LIBSSH2_SESSION * session, unsigned char packet_type,
                        unsigned char **data, size_t *data_len,
                        int match_ofs,
                        const unsigned char *match_buf,
                        size_t match_len,
                        packet_require_state_t *state)
{
    if (state->start == 0) {
        if (_libssh2_packet_ask(session, packet_type, data, data_len,
                                match_ofs, match_buf,
                                match_len) == 0) {
            /* A packet was available in the packet brigade */
            return 0;
        }

        state->start = time(NULL);
    }

    while (session->socket_state == LIBSSH2_SOCKET_CONNECTED) {
        int ret = _libssh2_transport_read(session);
        if (ret == LIBSSH2_ERROR_EAGAIN)
            return ret;
        else if (ret < 0) {
            state->start = 0;
            /* an error which is not just because of blocking */
            return ret;
        } else if (ret == packet_type) {
            /* Be lazy, let packet_ask pull it out of the brigade */
            ret = _libssh2_packet_ask(session, packet_type, data, data_len,
                                      match_ofs, match_buf, match_len);
            state->start = 0;
            return ret;
        } else if (ret == 0) {
            /* nothing available, wait until data arrives or we time out */
            long left = LIBSSH2_READ_TIMEOUT - (long)(time(NULL) -
                                                      state->start);

            if (left <= 0) {
                state->start = 0;
                return LIBSSH2_ERROR_TIMEOUT;
            }
            return -1; /* no packet available yet */
        }
    }

    /* Only reached if the socket died */
    return LIBSSH2_ERROR_SOCKET_DISCONNECT;
}

/*
 * _libssh2_packet_burn
 *
 * Loops _libssh2_transport_read() until any packet is available and promptly
 * discards it.
 * Used during KEX exchange to discard badly guessed KEX_INIT packets
 */
int
_libssh2_packet_burn(LIBSSH2_SESSION * session,
                     libssh2_nonblocking_states * state)
{
    unsigned char *data;
    size_t data_len;
    unsigned char all_packets[255];
    int i;
    int ret;

    if (*state == libssh2_NB_state_idle) {
        for(i = 1; i < 256; i++) {
            all_packets[i - 1] = i;
        }

        if (_libssh2_packet_askv(session, all_packets, &data, &data_len, 0,
                                 NULL, 0) == 0) {
            i = data[0];
            /* A packet was available in the packet brigade, burn it */
            LIBSSH2_FREE(session, data);
            return i;
        }

        _libssh2_debug(session, LIBSSH2_TRACE_TRANS,
                       "Blocking until packet becomes available to burn");
        *state = libssh2_NB_state_created;
    }

    while (session->socket_state == LIBSSH2_SOCKET_CONNECTED) {
        ret = _libssh2_transport_read(session);
        if (ret == LIBSSH2_ERROR_EAGAIN) {
            return ret;
        } else if (ret < 0) {
            *state = libssh2_NB_state_idle;
            return ret;
        } else if (ret == 0) {
            /* FIXME: this might busyloop */
            continue;
        }

        /* Be lazy, let packet_ask pull it out of the brigade */
        if (0 ==
            _libssh2_packet_ask(session, ret, &data, &data_len, 0, NULL, 0)) {
            /* Smoke 'em if you got 'em */
            LIBSSH2_FREE(session, data);
            *state = libssh2_NB_state_idle;
            return ret;
        }
    }

    /* Only reached if the socket died */
    return LIBSSH2_ERROR_SOCKET_DISCONNECT;
}

/*
 * _libssh2_packet_requirev
 *
 * Loops _libssh2_transport_read() until one of a list of packet types
 * requested is available. SSH_DISCONNECT or a SOCKET_DISCONNECTED will cause
 * a bailout. packet_types is a null terminated list of packet_type numbers
 */

int
_libssh2_packet_requirev(LIBSSH2_SESSION *session,
                         const unsigned char *packet_types,
                         unsigned char **data, size_t *data_len,
                         int match_ofs,
                         const unsigned char *match_buf, size_t match_len,
                         packet_requirev_state_t * state)
{
    if (_libssh2_packet_askv(session, packet_types, data, data_len, match_ofs,
                             match_buf, match_len) == 0) {
        /* One of the packets listed was available in the packet brigade */
        state->start = 0;
        return 0;
    }

    if (state->start == 0) {
        state->start = time(NULL);
    }

    while (session->socket_state != LIBSSH2_SOCKET_DISCONNECTED) {
        int ret = _libssh2_transport_read(session);
        if ((ret < 0) && (ret != LIBSSH2_ERROR_EAGAIN)) {
            state->start = 0;
            return ret;
        }
        if (ret <= 0) {
            long left = LIBSSH2_READ_TIMEOUT -
                (long)(time(NULL) - state->start);

            if (left <= 0) {
                state->start = 0;
                return LIBSSH2_ERROR_TIMEOUT;
            }
            else if (ret == LIBSSH2_ERROR_EAGAIN) {
                return ret;
            }
        }

        if (strchr((char *) packet_types, ret)) {
            /* Be lazy, let packet_ask pull it out of the brigade */
            return _libssh2_packet_askv(session, packet_types, data,
                                        data_len, match_ofs, match_buf,
                                        match_len);
        }
    }

    /* Only reached if the socket died */
    state->start = 0;
    return LIBSSH2_ERROR_SOCKET_DISCONNECT;
}

