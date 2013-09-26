/* Copyright (c) 2004-2007, Sara Golemon <sarag@libssh2.org>
 * Copyright (c) 2010-2012 by Daniel Stenberg
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
#include "libssh2_publickey.h"
#include "channel.h"
#include "session.h"

#define LIBSSH2_PUBLICKEY_VERSION               2

/* Numericised response codes -- Not IETF, just local representation */
#define LIBSSH2_PUBLICKEY_RESPONSE_STATUS       0
#define LIBSSH2_PUBLICKEY_RESPONSE_VERSION      1
#define LIBSSH2_PUBLICKEY_RESPONSE_PUBLICKEY    2

typedef struct _LIBSSH2_PUBLICKEY_CODE_LIST
{
    int code;
    const char *name;
    int name_len;
} LIBSSH2_PUBLICKEY_CODE_LIST;

static const LIBSSH2_PUBLICKEY_CODE_LIST publickey_response_codes[] =
{
    {LIBSSH2_PUBLICKEY_RESPONSE_STATUS, "status", sizeof("status") - 1},
    {LIBSSH2_PUBLICKEY_RESPONSE_VERSION, "version", sizeof("version") - 1},
    {LIBSSH2_PUBLICKEY_RESPONSE_PUBLICKEY, "publickey",
     sizeof("publickey") - 1} ,
    {0, NULL, 0}
};

/* PUBLICKEY status codes -- IETF defined */
#define LIBSSH2_PUBLICKEY_SUCCESS               0
#define LIBSSH2_PUBLICKEY_ACCESS_DENIED         1
#define LIBSSH2_PUBLICKEY_STORAGE_EXCEEDED      2
#define LIBSSH2_PUBLICKEY_VERSION_NOT_SUPPORTED 3
#define LIBSSH2_PUBLICKEY_KEY_NOT_FOUND         4
#define LIBSSH2_PUBLICKEY_KEY_NOT_SUPPORTED     5
#define LIBSSH2_PUBLICKEY_KEY_ALREADY_PRESENT   6
#define LIBSSH2_PUBLICKEY_GENERAL_FAILURE       7
#define LIBSSH2_PUBLICKEY_REQUEST_NOT_SUPPORTED 8

#define LIBSSH2_PUBLICKEY_STATUS_CODE_MAX       8

static const LIBSSH2_PUBLICKEY_CODE_LIST publickey_status_codes[] = {
    {LIBSSH2_PUBLICKEY_SUCCESS, "success", sizeof("success") - 1} ,
    {LIBSSH2_PUBLICKEY_ACCESS_DENIED, "access denied",
     sizeof("access denied") - 1},
    {LIBSSH2_PUBLICKEY_STORAGE_EXCEEDED, "storage exceeded",
     sizeof("storage exceeded") - 1} ,
    {LIBSSH2_PUBLICKEY_VERSION_NOT_SUPPORTED, "version not supported",
     sizeof("version not supported") - 1} ,
    {LIBSSH2_PUBLICKEY_KEY_NOT_FOUND, "key not found",
     sizeof("key not found") - 1},
    {LIBSSH2_PUBLICKEY_KEY_NOT_SUPPORTED, "key not supported",
     sizeof("key not supported") - 1},
    {LIBSSH2_PUBLICKEY_KEY_ALREADY_PRESENT, "key already present",
     sizeof("key already present") - 1},
    {LIBSSH2_PUBLICKEY_GENERAL_FAILURE, "general failure",
     sizeof("general failure") - 1},
    {LIBSSH2_PUBLICKEY_REQUEST_NOT_SUPPORTED, "request not supported",
     sizeof("request not supported") - 1},
    {0, NULL, 0}
};

/*
 * publickey_status_error
 *
 * Format an error message from a status code
 */
static void
publickey_status_error(const LIBSSH2_PUBLICKEY *pkey,
                       LIBSSH2_SESSION *session, int status)
{
    const char *msg;

    /* GENERAL_FAILURE got remapped between version 1 and 2 */
    if (status == 6 && pkey && pkey->version == 1) {
        status = 7;
    }

    if (status < 0 || status > LIBSSH2_PUBLICKEY_STATUS_CODE_MAX) {
        msg = "unknown";
    } else {
        msg = publickey_status_codes[status].name;
    }

    _libssh2_error(session, LIBSSH2_ERROR_PUBLICKEY_PROTOCOL, msg);
}

/*
 * publickey_packet_receive
 *
 * Read a packet from the subsystem
 */
static int
publickey_packet_receive(LIBSSH2_PUBLICKEY * pkey,
                         unsigned char **data, size_t *data_len)
{
    LIBSSH2_CHANNEL *channel = pkey->channel;
    LIBSSH2_SESSION *session = channel->session;
    unsigned char buffer[4];
    int rc;

    if (pkey->receive_state == libssh2_NB_state_idle) {
        rc = _libssh2_channel_read(channel, 0, (char *) buffer, 4);
        if (rc == LIBSSH2_ERROR_EAGAIN) {
            return rc;
        } else if (rc != 4) {
            return _libssh2_error(session, LIBSSH2_ERROR_PUBLICKEY_PROTOCOL,
                                  "Invalid response from publickey subsystem");
        }

        pkey->receive_packet_len = _libssh2_ntohu32(buffer);
        pkey->receive_packet =
            LIBSSH2_ALLOC(session, pkey->receive_packet_len);
        if (!pkey->receive_packet) {
            return _libssh2_error(session, LIBSSH2_ERROR_ALLOC,
                                  "Unable to allocate publickey response "
                                  "buffer");
        }

        pkey->receive_state = libssh2_NB_state_sent;
    }

    if (pkey->receive_state == libssh2_NB_state_sent) {
        rc = _libssh2_channel_read(channel, 0, (char *) pkey->receive_packet,
                                   pkey->receive_packet_len);
        if (rc == LIBSSH2_ERROR_EAGAIN) {
            return rc;
        } else if (rc != (int)pkey->receive_packet_len) {
            LIBSSH2_FREE(session, pkey->receive_packet);
            pkey->receive_packet = NULL;
            pkey->receive_state = libssh2_NB_state_idle;
            return _libssh2_error(session, LIBSSH2_ERROR_SOCKET_TIMEOUT,
                                  "Timeout waiting for publickey subsystem "
                                  "response packet");
        }

        *data = pkey->receive_packet;
        *data_len = pkey->receive_packet_len;
    }

    pkey->receive_state = libssh2_NB_state_idle;

    return 0;
}

/* publickey_response_id
 *
 * Translate a string response name to a numeric code
 * Data will be incremented by 4 + response_len on success only
 */
static int
publickey_response_id(unsigned char **pdata, size_t data_len)
{
    size_t response_len;
    unsigned char *data = *pdata;
    const LIBSSH2_PUBLICKEY_CODE_LIST *codes = publickey_response_codes;

    if (data_len < 4) {
        /* Malformed response */
        return -1;
    }
    response_len = _libssh2_ntohu32(data);
    data += 4;
    data_len -= 4;
    if (data_len < response_len) {
        /* Malformed response */
        return -1;
    }

    while (codes->name) {
        if ((unsigned long)codes->name_len == response_len &&
            strncmp(codes->name, (char *) data, response_len) == 0) {
            *pdata = data + response_len;
            return codes->code;
        }
        codes++;
    }

    return -1;
}

/* publickey_response_success
 *
 * Generic helper routine to wait for success response and nothing else
 */
static int
publickey_response_success(LIBSSH2_PUBLICKEY * pkey)
{
    LIBSSH2_SESSION *session = pkey->channel->session;
    unsigned char *data, *s;
    size_t data_len;
    int response;

    while (1) {
        int rc = publickey_packet_receive(pkey, &data, &data_len);
        if (rc == LIBSSH2_ERROR_EAGAIN) {
            return rc;
        } else if (rc) {
            return _libssh2_error(session, LIBSSH2_ERROR_SOCKET_TIMEOUT,
                                  "Timeout waiting for response from "
                                  "publickey subsystem");
        }

        s = data;
        response = publickey_response_id(&s, data_len);

        switch (response) {
        case LIBSSH2_PUBLICKEY_RESPONSE_STATUS:
            /* Error, or processing complete */
        {
            unsigned long status = _libssh2_ntohu32(s);

            LIBSSH2_FREE(session, data);

            if (status == LIBSSH2_PUBLICKEY_SUCCESS)
                return 0;

            publickey_status_error(pkey, session, status);
            return -1;
        }
        default:
            LIBSSH2_FREE(session, data);
            if (response < 0) {
                return _libssh2_error(session,
                                      LIBSSH2_ERROR_PUBLICKEY_PROTOCOL,
                                      "Invalid publickey subsystem response");
            }
            /* Unknown/Unexpected */
            _libssh2_error(session, LIBSSH2_ERROR_PUBLICKEY_PROTOCOL,
                           "Unexpected publickey subsystem response");
            data = NULL;
        }
    }
    /* never reached, but include `return` to silence compiler warnings */
    return -1;
}

/* *****************
 * Publickey API *
 ***************** */

/*
 * publickey_init
 *
 * Startup the publickey subsystem
 */
static LIBSSH2_PUBLICKEY *publickey_init(LIBSSH2_SESSION *session)
{
    int response;
    int rc;

    if (session->pkeyInit_state == libssh2_NB_state_idle) {
        session->pkeyInit_data = NULL;
        session->pkeyInit_pkey = NULL;
        session->pkeyInit_channel = NULL;

        _libssh2_debug(session, LIBSSH2_TRACE_PUBLICKEY,
                       "Initializing publickey subsystem");

        session->pkeyInit_state = libssh2_NB_state_allocated;
    }

    if (session->pkeyInit_state == libssh2_NB_state_allocated) {

        session->pkeyInit_channel =
            _libssh2_channel_open(session, "session",
                                  sizeof("session") - 1,
                                  LIBSSH2_CHANNEL_WINDOW_DEFAULT,
                                  LIBSSH2_CHANNEL_PACKET_DEFAULT, NULL,
                                  0);
        if (!session->pkeyInit_channel) {
            if (libssh2_session_last_errno(session) == LIBSSH2_ERROR_EAGAIN)
                /* The error state is already set, so leave it */
                return NULL;
            _libssh2_error(session, LIBSSH2_ERROR_CHANNEL_FAILURE,
                           "Unable to startup channel");
            goto err_exit;
        }

        session->pkeyInit_state = libssh2_NB_state_sent;
    }

    if (session->pkeyInit_state == libssh2_NB_state_sent) {
        rc = _libssh2_channel_process_startup(session->pkeyInit_channel,
                                              "subsystem",
                                              sizeof("subsystem") - 1,
                                              "publickey",
                                              sizeof("publickey") - 1);
        if (rc == LIBSSH2_ERROR_EAGAIN) {
            _libssh2_error(session, LIBSSH2_ERROR_EAGAIN,
                           "Would block starting publickey subsystem");
            return NULL;
        } else if (rc) {
            _libssh2_error(session, LIBSSH2_ERROR_CHANNEL_FAILURE,
                           "Unable to request publickey subsystem");
            goto err_exit;
        }

        session->pkeyInit_state = libssh2_NB_state_sent1;
    }

    if (session->pkeyInit_state == libssh2_NB_state_sent1) {
        unsigned char *s;
        rc = _libssh2_channel_extended_data(session->pkeyInit_channel,
                                            LIBSSH2_CHANNEL_EXTENDED_DATA_IGNORE);
        if (rc == LIBSSH2_ERROR_EAGAIN) {
            _libssh2_error(session, LIBSSH2_ERROR_EAGAIN,
                           "Would block starting publickey subsystem");
            return NULL;
        }

        session->pkeyInit_pkey =
            LIBSSH2_ALLOC(session, sizeof(LIBSSH2_PUBLICKEY));
        if (!session->pkeyInit_pkey) {
            _libssh2_error(session, LIBSSH2_ERROR_ALLOC,
                           "Unable to allocate a new publickey structure");
            goto err_exit;
        }
        memset(session->pkeyInit_pkey, 0, sizeof(LIBSSH2_PUBLICKEY));
        session->pkeyInit_pkey->channel = session->pkeyInit_channel;
        session->pkeyInit_pkey->version = 0;

        s = session->pkeyInit_buffer;
        _libssh2_htonu32(s, 4 + (sizeof("version") - 1) + 4);
        s += 4;
        _libssh2_htonu32(s, sizeof("version") - 1);
        s += 4;
        memcpy(s, "version", sizeof("version") - 1);
        s += sizeof("version") - 1;
        _libssh2_htonu32(s, LIBSSH2_PUBLICKEY_VERSION);

        session->pkeyInit_buffer_sent = 0;

        _libssh2_debug(session, LIBSSH2_TRACE_PUBLICKEY,
                       "Sending publickey advertising version %d support",
                       (int) LIBSSH2_PUBLICKEY_VERSION);

        session->pkeyInit_state = libssh2_NB_state_sent2;
    }

    if (session->pkeyInit_state == libssh2_NB_state_sent2) {
        rc = _libssh2_channel_write(session->pkeyInit_channel, 0,
                                    session->pkeyInit_buffer,
                                    19 - session->pkeyInit_buffer_sent);
        if (rc == LIBSSH2_ERROR_EAGAIN) {
            _libssh2_error(session, LIBSSH2_ERROR_EAGAIN,
                           "Would block sending publickey version packet");
            return NULL;
        } else if (rc < 0) {
            _libssh2_error(session, rc,
                           "Unable to send publickey version packet");
            goto err_exit;
        }
        session->pkeyInit_buffer_sent += rc;
        if(session->pkeyInit_buffer_sent < 19) {
            _libssh2_error(session, LIBSSH2_ERROR_EAGAIN,
                           "Need to be called again to complete this");
            return NULL;
        }

        session->pkeyInit_state = libssh2_NB_state_sent3;
    }

    if (session->pkeyInit_state == libssh2_NB_state_sent3) {
        while (1) {
            unsigned char *s;
            rc = publickey_packet_receive(session->pkeyInit_pkey,
                                          &session->pkeyInit_data,
                                          &session->pkeyInit_data_len);
            if (rc == LIBSSH2_ERROR_EAGAIN) {
                _libssh2_error(session, LIBSSH2_ERROR_EAGAIN,
                               "Would block waiting for response from "
                               "publickey subsystem");
                return NULL;
            } else if (rc) {
                _libssh2_error(session, LIBSSH2_ERROR_SOCKET_TIMEOUT,
                               "Timeout waiting for response from "
                               "publickey subsystem");
                goto err_exit;
            }

            s = session->pkeyInit_data;
            if ((response =
                 publickey_response_id(&s, session->pkeyInit_data_len)) < 0) {
                _libssh2_error(session, LIBSSH2_ERROR_PUBLICKEY_PROTOCOL,
                               "Invalid publickey subsystem response code");
                goto err_exit;
            }

            switch (response) {
            case LIBSSH2_PUBLICKEY_RESPONSE_STATUS:
                /* Error */
            {
                unsigned long status, descr_len, lang_len;

                status = _libssh2_ntohu32(s);
                s += 4;
                descr_len = _libssh2_ntohu32(s);
                s += 4;
                /* description starts here */
                s += descr_len;
                lang_len = _libssh2_ntohu32(s);
                s += 4;
                /* lang starts here */
                s += lang_len;

                if (s >
                    session->pkeyInit_data + session->pkeyInit_data_len) {
                    _libssh2_error(session,
                                   LIBSSH2_ERROR_PUBLICKEY_PROTOCOL,
                                   "Malformed publickey subsystem packet");
                    goto err_exit;
                }

                publickey_status_error(NULL, session, status);

                goto err_exit;
            }

            case LIBSSH2_PUBLICKEY_RESPONSE_VERSION:
                /* What we want */
                session->pkeyInit_pkey->version = _libssh2_ntohu32(s);
                if (session->pkeyInit_pkey->version >
                    LIBSSH2_PUBLICKEY_VERSION) {
                    _libssh2_debug(session, LIBSSH2_TRACE_PUBLICKEY,
                                   "Truncate remote publickey version from %lu",
                                   session->pkeyInit_pkey->version);
                    session->pkeyInit_pkey->version =
                        LIBSSH2_PUBLICKEY_VERSION;
                }
                _libssh2_debug(session, LIBSSH2_TRACE_PUBLICKEY,
                               "Enabling publickey subsystem version %lu",
                               session->pkeyInit_pkey->version);
                LIBSSH2_FREE(session, session->pkeyInit_data);
                session->pkeyInit_data = NULL;
                session->pkeyInit_state = libssh2_NB_state_idle;
                return session->pkeyInit_pkey;

            default:
                /* Unknown/Unexpected */
                _libssh2_error(session, LIBSSH2_ERROR_PUBLICKEY_PROTOCOL,
                               "Unexpected publickey subsystem response, "
                               "ignoring");
                LIBSSH2_FREE(session, session->pkeyInit_data);
                session->pkeyInit_data = NULL;
            }
        }
    }

    /* Never reached except by direct goto */
  err_exit:
    session->pkeyInit_state = libssh2_NB_state_sent4;
    if (session->pkeyInit_channel) {
        rc = _libssh2_channel_close(session->pkeyInit_channel);
        if (rc == LIBSSH2_ERROR_EAGAIN) {
            _libssh2_error(session, LIBSSH2_ERROR_EAGAIN,
                           "Would block closing channel");
            return NULL;
        }
    }
    if (session->pkeyInit_pkey) {
        LIBSSH2_FREE(session, session->pkeyInit_pkey);
        session->pkeyInit_pkey = NULL;
    }
    if (session->pkeyInit_data) {
        LIBSSH2_FREE(session, session->pkeyInit_data);
        session->pkeyInit_data = NULL;
    }
    session->pkeyInit_state = libssh2_NB_state_idle;
    return NULL;
}

/*
 * libssh2_publickey_init
 *
 * Startup the publickey subsystem
 */
LIBSSH2_API LIBSSH2_PUBLICKEY *
libssh2_publickey_init(LIBSSH2_SESSION *session)
{
    LIBSSH2_PUBLICKEY *ptr;

    BLOCK_ADJUST_ERRNO(ptr, session,
                       publickey_init(session));
    return ptr;
}



/*
 * libssh2_publickey_add_ex
 *
 * Add a new public key entry
 */
LIBSSH2_API int
libssh2_publickey_add_ex(LIBSSH2_PUBLICKEY *pkey, const unsigned char *name,
                         unsigned long name_len, const unsigned char *blob,
                         unsigned long blob_len, char overwrite,
                         unsigned long num_attrs,
                         const libssh2_publickey_attribute attrs[])
{
    LIBSSH2_CHANNEL *channel;
    LIBSSH2_SESSION *session;
    /*  19 = packet_len(4) + add_len(4) + "add"(3) + name_len(4) + {name}
        blob_len(4) + {blob} */
    unsigned long i, packet_len = 19 + name_len + blob_len;
    unsigned char *comment = NULL;
    unsigned long comment_len = 0;
    int rc;

    if(!pkey)
        return LIBSSH2_ERROR_BAD_USE;

    channel = pkey->channel;
    session = channel->session;

    if (pkey->add_state == libssh2_NB_state_idle) {
        pkey->add_packet = NULL;

        _libssh2_debug(session, LIBSSH2_TRACE_PUBLICKEY, "Adding %s publickey",
                       name);

        if (pkey->version == 1) {
            for(i = 0; i < num_attrs; i++) {
                /* Search for a comment attribute */
                if (attrs[i].name_len == (sizeof("comment") - 1) &&
                    strncmp(attrs[i].name, "comment",
                            sizeof("comment") - 1) == 0) {
                    comment = (unsigned char *) attrs[i].value;
                    comment_len = attrs[i].value_len;
                    break;
                }
            }
            packet_len += 4 + comment_len;
        } else {
            packet_len += 5;    /* overwrite(1) + attribute_count(4) */
            for(i = 0; i < num_attrs; i++) {
                packet_len += 9 + attrs[i].name_len + attrs[i].value_len;
                /* name_len(4) + value_len(4) + mandatory(1) */
            }
        }

        pkey->add_packet = LIBSSH2_ALLOC(session, packet_len);
        if (!pkey->add_packet) {
            return _libssh2_error(session, LIBSSH2_ERROR_ALLOC,
                                  "Unable to allocate memory for "
                                  "publickey \"add\" packet");
        }

        pkey->add_s = pkey->add_packet;
        _libssh2_htonu32(pkey->add_s, packet_len - 4);
        pkey->add_s += 4;
        _libssh2_htonu32(pkey->add_s, sizeof("add") - 1);
        pkey->add_s += 4;
        memcpy(pkey->add_s, "add", sizeof("add") - 1);
        pkey->add_s += sizeof("add") - 1;
        if (pkey->version == 1) {
            _libssh2_htonu32(pkey->add_s, comment_len);
            pkey->add_s += 4;
            if (comment) {
                memcpy(pkey->add_s, comment, comment_len);
                pkey->add_s += comment_len;
            }

            _libssh2_htonu32(pkey->add_s, name_len);
            pkey->add_s += 4;
            memcpy(pkey->add_s, name, name_len);
            pkey->add_s += name_len;
            _libssh2_htonu32(pkey->add_s, blob_len);
            pkey->add_s += 4;
            memcpy(pkey->add_s, blob, blob_len);
            pkey->add_s += blob_len;
        } else {
            /* Version == 2 */

            _libssh2_htonu32(pkey->add_s, name_len);
            pkey->add_s += 4;
            memcpy(pkey->add_s, name, name_len);
            pkey->add_s += name_len;
            _libssh2_htonu32(pkey->add_s, blob_len);
            pkey->add_s += 4;
            memcpy(pkey->add_s, blob, blob_len);
            pkey->add_s += blob_len;
            *(pkey->add_s++) = overwrite ? 0x01 : 0;
            _libssh2_htonu32(pkey->add_s, num_attrs);
            pkey->add_s += 4;
            for(i = 0; i < num_attrs; i++) {
                _libssh2_htonu32(pkey->add_s, attrs[i].name_len);
                pkey->add_s += 4;
                memcpy(pkey->add_s, attrs[i].name, attrs[i].name_len);
                pkey->add_s += attrs[i].name_len;
                _libssh2_htonu32(pkey->add_s, attrs[i].value_len);
                pkey->add_s += 4;
                memcpy(pkey->add_s, attrs[i].value, attrs[i].value_len);
                pkey->add_s += attrs[i].value_len;
                *(pkey->add_s++) = attrs[i].mandatory ? 0x01 : 0;
            }
        }

        _libssh2_debug(session, LIBSSH2_TRACE_PUBLICKEY,
                       "Sending publickey \"add\" packet: "
                       "type=%s blob_len=%ld num_attrs=%ld",
                       name, blob_len, num_attrs);

        pkey->add_state = libssh2_NB_state_created;
    }

    if (pkey->add_state == libssh2_NB_state_created) {
        rc = _libssh2_channel_write(channel, 0, pkey->add_packet,
                                    (pkey->add_s - pkey->add_packet));
        if (rc == LIBSSH2_ERROR_EAGAIN) {
            return rc;
        } else if ((pkey->add_s - pkey->add_packet) != rc) {
            LIBSSH2_FREE(session, pkey->add_packet);
            pkey->add_packet = NULL;
            return _libssh2_error(session, LIBSSH2_ERROR_SOCKET_SEND,
                                  "Unable to send publickey add packet");
        }
        LIBSSH2_FREE(session, pkey->add_packet);
        pkey->add_packet = NULL;

        pkey->add_state = libssh2_NB_state_sent;
    }

    rc = publickey_response_success(pkey);
    if (rc == LIBSSH2_ERROR_EAGAIN) {
        return rc;
    }

    pkey->add_state = libssh2_NB_state_idle;

    return rc;
}

/* libssh2_publickey_remove_ex
 * Remove an existing publickey so that authentication can no longer be
 * performed using it
 */
LIBSSH2_API int
libssh2_publickey_remove_ex(LIBSSH2_PUBLICKEY * pkey,
                            const unsigned char *name, unsigned long name_len,
                            const unsigned char *blob, unsigned long blob_len)
{
    LIBSSH2_CHANNEL *channel;
    LIBSSH2_SESSION *session;
    /* 22 = packet_len(4) + remove_len(4) + "remove"(6) + name_len(4) + {name}
       + blob_len(4) + {blob} */
    unsigned long packet_len = 22 + name_len + blob_len;
    int rc;

    if(!pkey)
        return LIBSSH2_ERROR_BAD_USE;

    channel = pkey->channel;
    session = channel->session;

    if (pkey->remove_state == libssh2_NB_state_idle) {
        pkey->remove_packet = NULL;

        pkey->remove_packet = LIBSSH2_ALLOC(session, packet_len);
        if (!pkey->remove_packet) {
            return _libssh2_error(session, LIBSSH2_ERROR_ALLOC,
                                  "Unable to allocate memory for "
                                  "publickey \"remove\" packet");
        }

        pkey->remove_s = pkey->remove_packet;
        _libssh2_htonu32(pkey->remove_s, packet_len - 4);
        pkey->remove_s += 4;
        _libssh2_htonu32(pkey->remove_s, sizeof("remove") - 1);
        pkey->remove_s += 4;
        memcpy(pkey->remove_s, "remove", sizeof("remove") - 1);
        pkey->remove_s += sizeof("remove") - 1;
        _libssh2_htonu32(pkey->remove_s, name_len);
        pkey->remove_s += 4;
        memcpy(pkey->remove_s, name, name_len);
        pkey->remove_s += name_len;
        _libssh2_htonu32(pkey->remove_s, blob_len);
        pkey->remove_s += 4;
        memcpy(pkey->remove_s, blob, blob_len);
        pkey->remove_s += blob_len;

        _libssh2_debug(session, LIBSSH2_TRACE_PUBLICKEY,
                       "Sending publickey \"remove\" packet: "
                       "type=%s blob_len=%ld",
                       name, blob_len);

        pkey->remove_state = libssh2_NB_state_created;
    }

    if (pkey->remove_state == libssh2_NB_state_created) {
        rc = _libssh2_channel_write(channel, 0, pkey->remove_packet,
                                    (pkey->remove_s - pkey->remove_packet));
        if (rc == LIBSSH2_ERROR_EAGAIN) {
            return rc;
        } else if ((pkey->remove_s - pkey->remove_packet) != rc) {
            LIBSSH2_FREE(session, pkey->remove_packet);
            pkey->remove_packet = NULL;
            pkey->remove_state = libssh2_NB_state_idle;
            return _libssh2_error(session, LIBSSH2_ERROR_SOCKET_SEND,
                                  "Unable to send publickey remove packet");
        }
        LIBSSH2_FREE(session, pkey->remove_packet);
        pkey->remove_packet = NULL;

        pkey->remove_state = libssh2_NB_state_sent;
    }

    rc = publickey_response_success(pkey);
    if (rc == LIBSSH2_ERROR_EAGAIN) {
        return rc;
    }

    pkey->remove_state = libssh2_NB_state_idle;

    return rc;
}

/* libssh2_publickey_list_fetch
 * Fetch a list of supported public key from a server
 */
LIBSSH2_API int
libssh2_publickey_list_fetch(LIBSSH2_PUBLICKEY * pkey, unsigned long *num_keys,
                             libssh2_publickey_list ** pkey_list)
{
    LIBSSH2_CHANNEL *channel;
    LIBSSH2_SESSION *session;
    libssh2_publickey_list *list = NULL;
    unsigned long buffer_len = 12, keys = 0, max_keys = 0, i;
    /* 12 = packet_len(4) + list_len(4) + "list"(4) */
    int response;
    int rc;

    if(!pkey)
        return LIBSSH2_ERROR_BAD_USE;

    channel = pkey->channel;
    session = channel->session;

    if (pkey->listFetch_state == libssh2_NB_state_idle) {
        pkey->listFetch_data = NULL;

        pkey->listFetch_s = pkey->listFetch_buffer;
        _libssh2_htonu32(pkey->listFetch_s, buffer_len - 4);
        pkey->listFetch_s += 4;
        _libssh2_htonu32(pkey->listFetch_s, sizeof("list") - 1);
        pkey->listFetch_s += 4;
        memcpy(pkey->listFetch_s, "list", sizeof("list") - 1);
        pkey->listFetch_s += sizeof("list") - 1;

        _libssh2_debug(session, LIBSSH2_TRACE_PUBLICKEY,
                       "Sending publickey \"list\" packet");

        pkey->listFetch_state = libssh2_NB_state_created;
    }

    if (pkey->listFetch_state == libssh2_NB_state_created) {
        rc = _libssh2_channel_write(channel, 0,
                                    pkey->listFetch_buffer,
                                    (pkey->listFetch_s -
                                     pkey->listFetch_buffer));
        if (rc == LIBSSH2_ERROR_EAGAIN) {
            return rc;
        } else if ((pkey->listFetch_s - pkey->listFetch_buffer) != rc) {
            pkey->listFetch_state = libssh2_NB_state_idle;
            return _libssh2_error(session, LIBSSH2_ERROR_SOCKET_SEND,
                                  "Unable to send publickey list packet");
        }

        pkey->listFetch_state = libssh2_NB_state_sent;
    }

    while (1) {
        rc = publickey_packet_receive(pkey, &pkey->listFetch_data,
                                      &pkey->listFetch_data_len);
        if (rc == LIBSSH2_ERROR_EAGAIN) {
            return rc;
        } else if (rc) {
            _libssh2_error(session, LIBSSH2_ERROR_SOCKET_TIMEOUT,
                           "Timeout waiting for response from "
                           "publickey subsystem");
            goto err_exit;
        }

        pkey->listFetch_s = pkey->listFetch_data;
        if ((response =
             publickey_response_id(&pkey->listFetch_s,
                                   pkey->listFetch_data_len)) < 0) {
            _libssh2_error(session, LIBSSH2_ERROR_PUBLICKEY_PROTOCOL,
                           "Invalid publickey subsystem response code");
            goto err_exit;
        }

        switch (response) {
        case LIBSSH2_PUBLICKEY_RESPONSE_STATUS:
            /* Error, or processing complete */
        {
            unsigned long status, descr_len, lang_len;

            status = _libssh2_ntohu32(pkey->listFetch_s);
            pkey->listFetch_s += 4;
            descr_len = _libssh2_ntohu32(pkey->listFetch_s);
            pkey->listFetch_s += 4;
            /* description starts at pkey->listFetch_s */
            pkey->listFetch_s += descr_len;
            lang_len = _libssh2_ntohu32(pkey->listFetch_s);
            pkey->listFetch_s += 4;
            /* lang starts at pkey->listFetch_s */
            pkey->listFetch_s += lang_len;

            if (pkey->listFetch_s >
                pkey->listFetch_data + pkey->listFetch_data_len) {
                _libssh2_error(session, LIBSSH2_ERROR_PUBLICKEY_PROTOCOL,
                               "Malformed publickey subsystem packet");
                goto err_exit;
            }

            if (status == LIBSSH2_PUBLICKEY_SUCCESS) {
                LIBSSH2_FREE(session, pkey->listFetch_data);
                pkey->listFetch_data = NULL;
                *pkey_list = list;
                *num_keys = keys;
                pkey->listFetch_state = libssh2_NB_state_idle;
                return 0;
            }

            publickey_status_error(pkey, session, status);
            goto err_exit;
        }
        case LIBSSH2_PUBLICKEY_RESPONSE_PUBLICKEY:
            /* What we want */
            if (keys >= max_keys) {
                libssh2_publickey_list *newlist;
                /* Grow the key list if necessary */
                max_keys += 8;
                newlist =
                    LIBSSH2_REALLOC(session, list,
                                    (max_keys +
                                     1) * sizeof(libssh2_publickey_list));
                if (!newlist) {
                    _libssh2_error(session, LIBSSH2_ERROR_ALLOC,
                                   "Unable to allocate memory for "
                                   "publickey list");
                    goto err_exit;
                }
                list = newlist;
            }
            if (pkey->version == 1) {
                unsigned long comment_len;

                comment_len = _libssh2_ntohu32(pkey->listFetch_s);
                pkey->listFetch_s += 4;
                if (comment_len) {
                    list[keys].num_attrs = 1;
                    list[keys].attrs =
                        LIBSSH2_ALLOC(session,
                                      sizeof(libssh2_publickey_attribute));
                    if (!list[keys].attrs) {
                        _libssh2_error(session, LIBSSH2_ERROR_ALLOC,
                                       "Unable to allocate memory for "
                                       "publickey attributes");
                        goto err_exit;
                    }
                    list[keys].attrs[0].name = "comment";
                    list[keys].attrs[0].name_len = sizeof("comment") - 1;
                    list[keys].attrs[0].value = (char *) pkey->listFetch_s;
                    list[keys].attrs[0].value_len = comment_len;
                    list[keys].attrs[0].mandatory = 0;

                    pkey->listFetch_s += comment_len;
                } else {
                    list[keys].num_attrs = 0;
                    list[keys].attrs = NULL;
                }
                list[keys].name_len = _libssh2_ntohu32(pkey->listFetch_s);
                pkey->listFetch_s += 4;
                list[keys].name = pkey->listFetch_s;
                pkey->listFetch_s += list[keys].name_len;
                list[keys].blob_len = _libssh2_ntohu32(pkey->listFetch_s);
                pkey->listFetch_s += 4;
                list[keys].blob = pkey->listFetch_s;
                pkey->listFetch_s += list[keys].blob_len;
            } else {
                /* Version == 2 */
                list[keys].name_len = _libssh2_ntohu32(pkey->listFetch_s);
                pkey->listFetch_s += 4;
                list[keys].name = pkey->listFetch_s;
                pkey->listFetch_s += list[keys].name_len;
                list[keys].blob_len = _libssh2_ntohu32(pkey->listFetch_s);
                pkey->listFetch_s += 4;
                list[keys].blob = pkey->listFetch_s;
                pkey->listFetch_s += list[keys].blob_len;
                list[keys].num_attrs = _libssh2_ntohu32(pkey->listFetch_s);
                pkey->listFetch_s += 4;
                if (list[keys].num_attrs) {
                    list[keys].attrs =
                        LIBSSH2_ALLOC(session,
                                      list[keys].num_attrs *
                                      sizeof(libssh2_publickey_attribute));
                    if (!list[keys].attrs) {
                        _libssh2_error(session, LIBSSH2_ERROR_ALLOC,
                                       "Unable to allocate memory for "
                                       "publickey attributes");
                        goto err_exit;
                    }
                    for(i = 0; i < list[keys].num_attrs; i++) {
                        list[keys].attrs[i].name_len =
                            _libssh2_ntohu32(pkey->listFetch_s);
                        pkey->listFetch_s += 4;
                        list[keys].attrs[i].name = (char *) pkey->listFetch_s;
                        pkey->listFetch_s += list[keys].attrs[i].name_len;
                        list[keys].attrs[i].value_len =
                            _libssh2_ntohu32(pkey->listFetch_s);
                        pkey->listFetch_s += 4;
                        list[keys].attrs[i].value = (char *) pkey->listFetch_s;
                        pkey->listFetch_s += list[keys].attrs[i].value_len;

                        /* actually an ignored value */
                        list[keys].attrs[i].mandatory = 0;
                    }
                } else {
                    list[keys].attrs = NULL;
                }
            }
            /* To be FREEd in libssh2_publickey_list_free() */
            list[keys].packet = pkey->listFetch_data;
            keys++;

            list[keys].packet = NULL;   /* Terminate the list */
            pkey->listFetch_data = NULL;
            break;
        default:
            /* Unknown/Unexpected */
            _libssh2_error(session, LIBSSH2_ERROR_PUBLICKEY_PROTOCOL,
                           "Unexpected publickey subsystem response");
            LIBSSH2_FREE(session, pkey->listFetch_data);
            pkey->listFetch_data = NULL;
        }
    }

    /* Only reached via explicit goto */
  err_exit:
    if (pkey->listFetch_data) {
        LIBSSH2_FREE(session, pkey->listFetch_data);
        pkey->listFetch_data = NULL;
    }
    if (list) {
        libssh2_publickey_list_free(pkey, list);
    }
    pkey->listFetch_state = libssh2_NB_state_idle;
    return -1;
}

/* libssh2_publickey_list_free
 * Free a previously fetched list of public keys
 */
LIBSSH2_API void
libssh2_publickey_list_free(LIBSSH2_PUBLICKEY * pkey,
                            libssh2_publickey_list * pkey_list)
{
    LIBSSH2_SESSION *session;
    libssh2_publickey_list *p = pkey_list;

    if(!pkey || !p)
        return;

    session = pkey->channel->session;

    while (p->packet) {
        if (p->attrs) {
            LIBSSH2_FREE(session, p->attrs);
        }
        LIBSSH2_FREE(session, p->packet);
        p++;
    }

    LIBSSH2_FREE(session, pkey_list);
}

/* libssh2_publickey_shutdown
 * Shutdown the publickey subsystem
 */
LIBSSH2_API int
libssh2_publickey_shutdown(LIBSSH2_PUBLICKEY *pkey)
{
    LIBSSH2_SESSION *session;
    int rc;

    if(!pkey)
        return LIBSSH2_ERROR_BAD_USE;

    session = pkey->channel->session;

    /*
     * Make sure all memory used in the state variables are free
     */
    if (pkey->receive_packet) {
        LIBSSH2_FREE(session, pkey->receive_packet);
        pkey->receive_packet = NULL;
    }
    if (pkey->add_packet) {
        LIBSSH2_FREE(session, pkey->add_packet);
        pkey->add_packet = NULL;
    }
    if (pkey->remove_packet) {
        LIBSSH2_FREE(session, pkey->remove_packet);
        pkey->remove_packet = NULL;
    }
    if (pkey->listFetch_data) {
        LIBSSH2_FREE(session, pkey->listFetch_data);
        pkey->listFetch_data = NULL;
    }

    rc = _libssh2_channel_free(pkey->channel);
    if (rc == LIBSSH2_ERROR_EAGAIN)
        return rc;

    LIBSSH2_FREE(session, pkey);
    return 0;
}
