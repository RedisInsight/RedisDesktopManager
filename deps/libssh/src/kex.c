/* Copyright (c) 2004-2007, Sara Golemon <sarag@libssh2.org>
 * Copyright (c) 2010, Daniel Stenberg <daniel@haxx.se>
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

#include "transport.h"
#include "comp.h"
#include "mac.h"

/* TODO: Switch this to an inline and handle alloc() failures */
/* Helper macro called from kex_method_diffie_hellman_group1_sha1_key_exchange */
#define LIBSSH2_KEX_METHOD_DIFFIE_HELLMAN_SHA1_HASH(value, reqlen, version) \
    {                                                                   \
        libssh2_sha1_ctx hash;                                          \
        unsigned long len = 0;                                          \
        if (!(value)) {                                                 \
            value = LIBSSH2_ALLOC(session, reqlen + SHA_DIGEST_LENGTH); \
        }                                                               \
        if (value)                                                      \
            while (len < (unsigned long)reqlen) {                       \
                libssh2_sha1_init(&hash);                               \
                libssh2_sha1_update(hash, exchange_state->k_value,      \
                                    exchange_state->k_value_len);       \
                libssh2_sha1_update(hash, exchange_state->h_sig_comp,   \
                                    SHA_DIGEST_LENGTH);                 \
                if (len > 0) {                                          \
                    libssh2_sha1_update(hash, value, len);              \
                }    else {                                             \
                    libssh2_sha1_update(hash, (version), 1);            \
                    libssh2_sha1_update(hash, session->session_id,      \
                                        session->session_id_len);       \
                }                                                       \
                libssh2_sha1_final(hash, (value) + len);                \
                len += SHA_DIGEST_LENGTH;                               \
            }                                                           \
    }

/*
 * diffie_hellman_sha1
 *
 * Diffie Hellman Key Exchange, Group Agnostic
 */
static int diffie_hellman_sha1(LIBSSH2_SESSION *session,
                               _libssh2_bn *g,
                               _libssh2_bn *p,
                               int group_order,
                               unsigned char packet_type_init,
                               unsigned char packet_type_reply,
                               unsigned char *midhash,
                               unsigned long midhash_len,
                               kmdhgGPsha1kex_state_t *exchange_state)
{
    int ret = 0;
    int rc;

    if (exchange_state->state == libssh2_NB_state_idle) {
        /* Setup initial values */
        exchange_state->e_packet = NULL;
        exchange_state->s_packet = NULL;
        exchange_state->k_value = NULL;
        exchange_state->ctx = _libssh2_bn_ctx_new();
        exchange_state->x = _libssh2_bn_init(); /* Random from client */
        exchange_state->e = _libssh2_bn_init(); /* g^x mod p */
        exchange_state->f = _libssh2_bn_init(); /* g^(Random from server) mod p */
        exchange_state->k = _libssh2_bn_init(); /* The shared secret: f^x mod p */

        /* Zero the whole thing out */
        memset(&exchange_state->req_state, 0, sizeof(packet_require_state_t));

        /* Generate x and e */
        _libssh2_bn_rand(exchange_state->x, group_order, 0, -1);
        _libssh2_bn_mod_exp(exchange_state->e, g, exchange_state->x, p,
                            exchange_state->ctx);

        /* Send KEX init */
        /* packet_type(1) + String Length(4) + leading 0(1) */
        exchange_state->e_packet_len =
            _libssh2_bn_bytes(exchange_state->e) + 6;
        if (_libssh2_bn_bits(exchange_state->e) % 8) {
            /* Leading 00 not needed */
            exchange_state->e_packet_len--;
        }

        exchange_state->e_packet =
            LIBSSH2_ALLOC(session, exchange_state->e_packet_len);
        if (!exchange_state->e_packet) {
            ret = _libssh2_error(session, LIBSSH2_ERROR_ALLOC,
                                 "Out of memory error");
            goto clean_exit;
        }
        exchange_state->e_packet[0] = packet_type_init;
        _libssh2_htonu32(exchange_state->e_packet + 1,
                         exchange_state->e_packet_len - 5);
        if (_libssh2_bn_bits(exchange_state->e) % 8) {
            _libssh2_bn_to_bin(exchange_state->e,
                               exchange_state->e_packet + 5);
        } else {
            exchange_state->e_packet[5] = 0;
            _libssh2_bn_to_bin(exchange_state->e,
                               exchange_state->e_packet + 6);
        }

        _libssh2_debug(session, LIBSSH2_TRACE_KEX, "Sending KEX packet %d",
                       (int) packet_type_init);
        exchange_state->state = libssh2_NB_state_created;
    }

    if (exchange_state->state == libssh2_NB_state_created) {
        rc = _libssh2_transport_send(session, exchange_state->e_packet,
                                     exchange_state->e_packet_len,
                                     NULL, 0);
        if (rc == LIBSSH2_ERROR_EAGAIN) {
            return rc;
        } else if (rc) {
            ret = _libssh2_error(session, rc,
                                 "Unable to send KEX init message");
            goto clean_exit;
        }
        exchange_state->state = libssh2_NB_state_sent;
    }

    if (exchange_state->state == libssh2_NB_state_sent) {
        if (session->burn_optimistic_kexinit) {
            /* The first KEX packet to come along will be the guess initially
             * sent by the server.  That guess turned out to be wrong so we
             * need to silently ignore it */
            int burn_type;

            _libssh2_debug(session, LIBSSH2_TRACE_KEX,
                           "Waiting for badly guessed KEX packet (to be ignored)");
            burn_type =
                _libssh2_packet_burn(session, &exchange_state->burn_state);
            if (burn_type == LIBSSH2_ERROR_EAGAIN) {
                return burn_type;
            } else if (burn_type <= 0) {
                /* Failed to receive a packet */
                ret = burn_type;
                goto clean_exit;
            }
            session->burn_optimistic_kexinit = 0;

            _libssh2_debug(session, LIBSSH2_TRACE_KEX,
                           "Burnt packet of type: %02x",
                           (unsigned int) burn_type);
        }

        exchange_state->state = libssh2_NB_state_sent1;
    }

    if (exchange_state->state == libssh2_NB_state_sent1) {
        /* Wait for KEX reply */
        rc = _libssh2_packet_require(session, packet_type_reply,
                                     &exchange_state->s_packet,
                                     &exchange_state->s_packet_len, 0, NULL,
                                     0, &exchange_state->req_state);
        if (rc == LIBSSH2_ERROR_EAGAIN) {
            return rc;
        }
        if (rc) {
            ret = _libssh2_error(session, LIBSSH2_ERROR_TIMEOUT,
                                 "Timed out waiting for KEX reply");
            goto clean_exit;
        }

        /* Parse KEXDH_REPLY */
        exchange_state->s = exchange_state->s_packet + 1;

        session->server_hostkey_len = _libssh2_ntohu32(exchange_state->s);
        exchange_state->s += 4;
        session->server_hostkey =
            LIBSSH2_ALLOC(session, session->server_hostkey_len);
        if (!session->server_hostkey) {
            ret = _libssh2_error(session, LIBSSH2_ERROR_ALLOC,
                                 "Unable to allocate memory for a copy "
                                 "of the host key");
            goto clean_exit;
        }
        memcpy(session->server_hostkey, exchange_state->s,
               session->server_hostkey_len);
        exchange_state->s += session->server_hostkey_len;

#if LIBSSH2_MD5
        {
            libssh2_md5_ctx fingerprint_ctx;

            if (libssh2_md5_init(&fingerprint_ctx)) {
                libssh2_md5_update(fingerprint_ctx, session->server_hostkey,
                                   session->server_hostkey_len);
                libssh2_md5_final(fingerprint_ctx, session->server_hostkey_md5);
                session->server_hostkey_md5_valid = TRUE;
            }
            else {
                session->server_hostkey_md5_valid = FALSE;
            }
        }
#ifdef LIBSSH2DEBUG
        {
            char fingerprint[50], *fprint = fingerprint;
            int i;
            for(i = 0; i < 16; i++, fprint += 3) {
                snprintf(fprint, 4, "%02x:", session->server_hostkey_md5[i]);
            }
            *(--fprint) = '\0';
            _libssh2_debug(session, LIBSSH2_TRACE_KEX,
                           "Server's MD5 Fingerprint: %s", fingerprint);
        }
#endif /* LIBSSH2DEBUG */
#endif /* ! LIBSSH2_MD5 */

        {
            libssh2_sha1_ctx fingerprint_ctx;

            libssh2_sha1_init(&fingerprint_ctx);
            libssh2_sha1_update(fingerprint_ctx, session->server_hostkey,
                                session->server_hostkey_len);
            libssh2_sha1_final(fingerprint_ctx, session->server_hostkey_sha1);
        }
#ifdef LIBSSH2DEBUG
        {
            char fingerprint[64], *fprint = fingerprint;
            int i;

            for(i = 0; i < 20; i++, fprint += 3) {
                snprintf(fprint, 4, "%02x:", session->server_hostkey_sha1[i]);
            }
            *(--fprint) = '\0';
            _libssh2_debug(session, LIBSSH2_TRACE_KEX,
                           "Server's SHA1 Fingerprint: %s", fingerprint);
        }
#endif /* LIBSSH2DEBUG */

        if (session->hostkey->init(session, session->server_hostkey,
                                   session->server_hostkey_len,
                                   &session->server_hostkey_abstract)) {
            ret = _libssh2_error(session, LIBSSH2_ERROR_HOSTKEY_INIT,
                                 "Unable to initialize hostkey importer");
            goto clean_exit;
        }

        exchange_state->f_value_len = _libssh2_ntohu32(exchange_state->s);
        exchange_state->s += 4;
        exchange_state->f_value = exchange_state->s;
        exchange_state->s += exchange_state->f_value_len;
        _libssh2_bn_from_bin(exchange_state->f, exchange_state->f_value_len,
                             exchange_state->f_value);

        exchange_state->h_sig_len = _libssh2_ntohu32(exchange_state->s);
        exchange_state->s += 4;
        exchange_state->h_sig = exchange_state->s;

        /* Compute the shared secret */
        _libssh2_bn_mod_exp(exchange_state->k, exchange_state->f,
                            exchange_state->x, p, exchange_state->ctx);
        exchange_state->k_value_len = _libssh2_bn_bytes(exchange_state->k) + 5;
        if (_libssh2_bn_bits(exchange_state->k) % 8) {
            /* don't need leading 00 */
            exchange_state->k_value_len--;
        }
        exchange_state->k_value =
            LIBSSH2_ALLOC(session, exchange_state->k_value_len);
        if (!exchange_state->k_value) {
            ret = _libssh2_error(session, LIBSSH2_ERROR_ALLOC,
                                 "Unable to allocate buffer for K");
            goto clean_exit;
        }
        _libssh2_htonu32(exchange_state->k_value,
                         exchange_state->k_value_len - 4);
        if (_libssh2_bn_bits(exchange_state->k) % 8) {
            _libssh2_bn_to_bin(exchange_state->k, exchange_state->k_value + 4);
        } else {
            exchange_state->k_value[4] = 0;
            _libssh2_bn_to_bin(exchange_state->k, exchange_state->k_value + 5);
        }

        libssh2_sha1_init(&exchange_state->exchange_hash);
        if (session->local.banner) {
            _libssh2_htonu32(exchange_state->h_sig_comp,
                             strlen((char *) session->local.banner) - 2);
            libssh2_sha1_update(exchange_state->exchange_hash,
                                exchange_state->h_sig_comp, 4);
            libssh2_sha1_update(exchange_state->exchange_hash,
                                (char *) session->local.banner,
                                strlen((char *) session->local.banner) - 2);
        } else {
            _libssh2_htonu32(exchange_state->h_sig_comp,
                             sizeof(LIBSSH2_SSH_DEFAULT_BANNER) - 1);
            libssh2_sha1_update(exchange_state->exchange_hash,
                                exchange_state->h_sig_comp, 4);
            libssh2_sha1_update(exchange_state->exchange_hash,
                                LIBSSH2_SSH_DEFAULT_BANNER,
                                sizeof(LIBSSH2_SSH_DEFAULT_BANNER) - 1);
        }

        _libssh2_htonu32(exchange_state->h_sig_comp,
                         strlen((char *) session->remote.banner));
        libssh2_sha1_update(exchange_state->exchange_hash,
                            exchange_state->h_sig_comp, 4);
        libssh2_sha1_update(exchange_state->exchange_hash,
                            session->remote.banner,
                            strlen((char *) session->remote.banner));

        _libssh2_htonu32(exchange_state->h_sig_comp,
                         session->local.kexinit_len);
        libssh2_sha1_update(exchange_state->exchange_hash,
                            exchange_state->h_sig_comp, 4);
        libssh2_sha1_update(exchange_state->exchange_hash,
                            session->local.kexinit,
                            session->local.kexinit_len);

        _libssh2_htonu32(exchange_state->h_sig_comp,
                         session->remote.kexinit_len);
        libssh2_sha1_update(exchange_state->exchange_hash,
                            exchange_state->h_sig_comp, 4);
        libssh2_sha1_update(exchange_state->exchange_hash,
                            session->remote.kexinit,
                            session->remote.kexinit_len);

        _libssh2_htonu32(exchange_state->h_sig_comp,
                         session->server_hostkey_len);
        libssh2_sha1_update(exchange_state->exchange_hash,
                            exchange_state->h_sig_comp, 4);
        libssh2_sha1_update(exchange_state->exchange_hash,
                            session->server_hostkey,
                            session->server_hostkey_len);

        if (packet_type_init == SSH_MSG_KEX_DH_GEX_INIT) {
            /* diffie-hellman-group-exchange hashes additional fields */
#ifdef LIBSSH2_DH_GEX_NEW
            _libssh2_htonu32(exchange_state->h_sig_comp,
                             LIBSSH2_DH_GEX_MINGROUP);
            _libssh2_htonu32(exchange_state->h_sig_comp + 4,
                             LIBSSH2_DH_GEX_OPTGROUP);
            _libssh2_htonu32(exchange_state->h_sig_comp + 8,
                             LIBSSH2_DH_GEX_MAXGROUP);
            libssh2_sha1_update(exchange_state->exchange_hash,
                                exchange_state->h_sig_comp, 12);
#else
            _libssh2_htonu32(exchange_state->h_sig_comp,
                             LIBSSH2_DH_GEX_OPTGROUP);
            libssh2_sha1_update(exchange_state->exchange_hash,
                                exchange_state->h_sig_comp, 4);
#endif
        }

        if (midhash) {
            libssh2_sha1_update(exchange_state->exchange_hash, midhash,
                                midhash_len);
        }

        libssh2_sha1_update(exchange_state->exchange_hash,
                            exchange_state->e_packet + 1,
                            exchange_state->e_packet_len - 1);

        _libssh2_htonu32(exchange_state->h_sig_comp,
                         exchange_state->f_value_len);
        libssh2_sha1_update(exchange_state->exchange_hash,
                            exchange_state->h_sig_comp, 4);
        libssh2_sha1_update(exchange_state->exchange_hash,
                            exchange_state->f_value,
                            exchange_state->f_value_len);

        libssh2_sha1_update(exchange_state->exchange_hash,
                            exchange_state->k_value,
                            exchange_state->k_value_len);

        libssh2_sha1_final(exchange_state->exchange_hash,
                           exchange_state->h_sig_comp);

        if (session->hostkey->
            sig_verify(session, exchange_state->h_sig,
                       exchange_state->h_sig_len, exchange_state->h_sig_comp,
                       20, &session->server_hostkey_abstract)) {
            ret = _libssh2_error(session, LIBSSH2_ERROR_HOSTKEY_SIGN,
                                 "Unable to verify hostkey signature");
            goto clean_exit;
        }

        _libssh2_debug(session, LIBSSH2_TRACE_KEX, "Sending NEWKEYS message");
        exchange_state->c = SSH_MSG_NEWKEYS;

        exchange_state->state = libssh2_NB_state_sent2;
    }

    if (exchange_state->state == libssh2_NB_state_sent2) {
        rc = _libssh2_transport_send(session, &exchange_state->c, 1, NULL, 0);
        if (rc == LIBSSH2_ERROR_EAGAIN) {
            return rc;
        } else if (rc) {
            ret = _libssh2_error(session, rc, "Unable to send NEWKEYS message");
            goto clean_exit;
        }

        exchange_state->state = libssh2_NB_state_sent3;
    }

    if (exchange_state->state == libssh2_NB_state_sent3) {
        rc = _libssh2_packet_require(session, SSH_MSG_NEWKEYS,
                                     &exchange_state->tmp,
                                     &exchange_state->tmp_len, 0, NULL, 0,
                                     &exchange_state->req_state);
        if (rc == LIBSSH2_ERROR_EAGAIN) {
            return rc;
        } else if (rc) {
            ret = _libssh2_error(session, rc, "Timed out waiting for NEWKEYS");
            goto clean_exit;
        }
        /* The first key exchange has been performed,
           switch to active crypt/comp/mac mode */
        session->state |= LIBSSH2_STATE_NEWKEYS;
        _libssh2_debug(session, LIBSSH2_TRACE_KEX, "Received NEWKEYS message");

        /* This will actually end up being just packet_type(1)
           for this packet type anyway */
        LIBSSH2_FREE(session, exchange_state->tmp);

        if (!session->session_id) {
            session->session_id = LIBSSH2_ALLOC(session, SHA_DIGEST_LENGTH);
            if (!session->session_id) {
                ret = _libssh2_error(session, LIBSSH2_ERROR_ALLOC,
                                     "Unable to allocate buffer for SHA digest");
                goto clean_exit;
            }
            memcpy(session->session_id, exchange_state->h_sig_comp,
                   SHA_DIGEST_LENGTH);
            session->session_id_len = SHA_DIGEST_LENGTH;
            _libssh2_debug(session, LIBSSH2_TRACE_KEX, "session_id calculated");
        }

        /* Cleanup any existing cipher */
        if (session->local.crypt->dtor) {
            session->local.crypt->dtor(session,
                                       &session->local.crypt_abstract);
        }

        /* Calculate IV/Secret/Key for each direction */
        if (session->local.crypt->init) {
            unsigned char *iv = NULL, *secret = NULL;
            int free_iv = 0, free_secret = 0;

            LIBSSH2_KEX_METHOD_DIFFIE_HELLMAN_SHA1_HASH(iv,
                                                        session->local.crypt->
                                                        iv_len, "A");
            if (!iv) {
                ret = -1;
                goto clean_exit;
            }
            LIBSSH2_KEX_METHOD_DIFFIE_HELLMAN_SHA1_HASH(secret,
                                                        session->local.crypt->
                                                        secret_len, "C");
            if (!secret) {
                LIBSSH2_FREE(session, iv);
                ret = LIBSSH2_ERROR_KEX_FAILURE;
                goto clean_exit;
            }
            if (session->local.crypt->
                init(session, session->local.crypt, iv, &free_iv, secret,
                     &free_secret, 1, &session->local.crypt_abstract)) {
                LIBSSH2_FREE(session, iv);
                LIBSSH2_FREE(session, secret);
                ret = LIBSSH2_ERROR_KEX_FAILURE;
                goto clean_exit;
            }

            if (free_iv) {
                memset(iv, 0, session->local.crypt->iv_len);
                LIBSSH2_FREE(session, iv);
            }

            if (free_secret) {
                memset(secret, 0, session->local.crypt->secret_len);
                LIBSSH2_FREE(session, secret);
            }
        }
        _libssh2_debug(session, LIBSSH2_TRACE_KEX,
                       "Client to Server IV and Key calculated");

        if (session->remote.crypt->dtor) {
            /* Cleanup any existing cipher */
            session->remote.crypt->dtor(session,
                                        &session->remote.crypt_abstract);
        }

        if (session->remote.crypt->init) {
            unsigned char *iv = NULL, *secret = NULL;
            int free_iv = 0, free_secret = 0;

            LIBSSH2_KEX_METHOD_DIFFIE_HELLMAN_SHA1_HASH(iv,
                                                        session->remote.crypt->
                                                        iv_len, "B");
            if (!iv) {
                ret = LIBSSH2_ERROR_KEX_FAILURE;
                goto clean_exit;
            }
            LIBSSH2_KEX_METHOD_DIFFIE_HELLMAN_SHA1_HASH(secret,
                                                        session->remote.crypt->
                                                        secret_len, "D");
            if (!secret) {
                LIBSSH2_FREE(session, iv);
                ret = LIBSSH2_ERROR_KEX_FAILURE;
                goto clean_exit;
            }
            if (session->remote.crypt->
                init(session, session->remote.crypt, iv, &free_iv, secret,
                     &free_secret, 0, &session->remote.crypt_abstract)) {
                LIBSSH2_FREE(session, iv);
                LIBSSH2_FREE(session, secret);
                ret = LIBSSH2_ERROR_KEX_FAILURE;
                goto clean_exit;
            }

            if (free_iv) {
                memset(iv, 0, session->remote.crypt->iv_len);
                LIBSSH2_FREE(session, iv);
            }

            if (free_secret) {
                memset(secret, 0, session->remote.crypt->secret_len);
                LIBSSH2_FREE(session, secret);
            }
        }
        _libssh2_debug(session, LIBSSH2_TRACE_KEX,
                       "Server to Client IV and Key calculated");

        if (session->local.mac->dtor) {
            session->local.mac->dtor(session, &session->local.mac_abstract);
        }

        if (session->local.mac->init) {
            unsigned char *key = NULL;
            int free_key = 0;

            LIBSSH2_KEX_METHOD_DIFFIE_HELLMAN_SHA1_HASH(key,
                                                        session->local.mac->
                                                        key_len, "E");
            if (!key) {
                ret = LIBSSH2_ERROR_KEX_FAILURE;
                goto clean_exit;
            }
            session->local.mac->init(session, key, &free_key,
                                     &session->local.mac_abstract);

            if (free_key) {
                memset(key, 0, session->local.mac->key_len);
                LIBSSH2_FREE(session, key);
            }
        }
        _libssh2_debug(session, LIBSSH2_TRACE_KEX,
                       "Client to Server HMAC Key calculated");

        if (session->remote.mac->dtor) {
            session->remote.mac->dtor(session, &session->remote.mac_abstract);
        }

        if (session->remote.mac->init) {
            unsigned char *key = NULL;
            int free_key = 0;

            LIBSSH2_KEX_METHOD_DIFFIE_HELLMAN_SHA1_HASH(key,
                                                        session->remote.mac->
                                                        key_len, "F");
            if (!key) {
                ret = LIBSSH2_ERROR_KEX_FAILURE;
                goto clean_exit;
            }
            session->remote.mac->init(session, key, &free_key,
                                      &session->remote.mac_abstract);

            if (free_key) {
                memset(key, 0, session->remote.mac->key_len);
                LIBSSH2_FREE(session, key);
            }
        }
        _libssh2_debug(session, LIBSSH2_TRACE_KEX,
                       "Server to Client HMAC Key calculated");

        /* Initialize compression for each direction */

        /* Cleanup any existing compression */
        if (session->local.comp && session->local.comp->dtor) {
            session->local.comp->dtor(session, 1,
                                      &session->local.comp_abstract);
        }

        if (session->local.comp && session->local.comp->init) {
            if (session->local.comp->init(session, 1,
                                          &session->local.comp_abstract)) {
                ret = LIBSSH2_ERROR_KEX_FAILURE;
                goto clean_exit;
            }
        }
        _libssh2_debug(session, LIBSSH2_TRACE_KEX,
                       "Client to Server compression initialized");

        if (session->remote.comp && session->remote.comp->dtor) {
            session->remote.comp->dtor(session, 0,
                                       &session->remote.comp_abstract);
        }

        if (session->remote.comp && session->remote.comp->init) {
            if (session->remote.comp->init(session, 0,
                                           &session->remote.comp_abstract)) {
                ret = LIBSSH2_ERROR_KEX_FAILURE;
                goto clean_exit;
            }
        }
        _libssh2_debug(session, LIBSSH2_TRACE_KEX,
                       "Server to Client compression initialized");

    }

  clean_exit:
    _libssh2_bn_free(exchange_state->x);
    exchange_state->x = NULL;
    _libssh2_bn_free(exchange_state->e);
    exchange_state->e = NULL;
    _libssh2_bn_free(exchange_state->f);
    exchange_state->f = NULL;
    _libssh2_bn_free(exchange_state->k);
    exchange_state->k = NULL;
    _libssh2_bn_ctx_free(exchange_state->ctx);
    exchange_state->ctx = NULL;

    if (exchange_state->e_packet) {
        LIBSSH2_FREE(session, exchange_state->e_packet);
        exchange_state->e_packet = NULL;
    }

    if (exchange_state->s_packet) {
        LIBSSH2_FREE(session, exchange_state->s_packet);
        exchange_state->s_packet = NULL;
    }

    if (exchange_state->k_value) {
        LIBSSH2_FREE(session, exchange_state->k_value);
        exchange_state->k_value = NULL;
    }

    exchange_state->state = libssh2_NB_state_idle;

    return ret;
}



/* kex_method_diffie_hellman_group1_sha1_key_exchange
 * Diffie-Hellman Group1 (Actually Group2) Key Exchange using SHA1
 */
static int
kex_method_diffie_hellman_group1_sha1_key_exchange(LIBSSH2_SESSION *session,
                                                   key_exchange_state_low_t
                                                   * key_state)
{
    static const unsigned char p_value[128] = {
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
        0xC9, 0x0F, 0xDA, 0xA2, 0x21, 0x68, 0xC2, 0x34,
        0xC4, 0xC6, 0x62, 0x8B, 0x80, 0xDC, 0x1C, 0xD1,
        0x29, 0x02, 0x4E, 0x08, 0x8A, 0x67, 0xCC, 0x74,
        0x02, 0x0B, 0xBE, 0xA6, 0x3B, 0x13, 0x9B, 0x22,
        0x51, 0x4A, 0x08, 0x79, 0x8E, 0x34, 0x04, 0xDD,
        0xEF, 0x95, 0x19, 0xB3, 0xCD, 0x3A, 0x43, 0x1B,
        0x30, 0x2B, 0x0A, 0x6D, 0xF2, 0x5F, 0x14, 0x37,
        0x4F, 0xE1, 0x35, 0x6D, 0x6D, 0x51, 0xC2, 0x45,
        0xE4, 0x85, 0xB5, 0x76, 0x62, 0x5E, 0x7E, 0xC6,
        0xF4, 0x4C, 0x42, 0xE9, 0xA6, 0x37, 0xED, 0x6B,
        0x0B, 0xFF, 0x5C, 0xB6, 0xF4, 0x06, 0xB7, 0xED,
        0xEE, 0x38, 0x6B, 0xFB, 0x5A, 0x89, 0x9F, 0xA5,
        0xAE, 0x9F, 0x24, 0x11, 0x7C, 0x4B, 0x1F, 0xE6,
        0x49, 0x28, 0x66, 0x51, 0xEC, 0xE6, 0x53, 0x81,
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
    };

    int ret;

    if (key_state->state == libssh2_NB_state_idle) {
        /* g == 2 */
        key_state->p = _libssh2_bn_init();      /* SSH2 defined value (p_value) */
        key_state->g = _libssh2_bn_init();      /* SSH2 defined value (2) */

        /* Initialize P and G */
        _libssh2_bn_set_word(key_state->g, 2);
        _libssh2_bn_from_bin(key_state->p, 128, p_value);

        _libssh2_debug(session, LIBSSH2_TRACE_KEX,
                       "Initiating Diffie-Hellman Group1 Key Exchange");

        key_state->state = libssh2_NB_state_created;
    }
    ret = diffie_hellman_sha1(session, key_state->g, key_state->p, 128,
                              SSH_MSG_KEXDH_INIT, SSH_MSG_KEXDH_REPLY,
                              NULL, 0, &key_state->exchange_state);
    if (ret == LIBSSH2_ERROR_EAGAIN) {
        return ret;
    }

    _libssh2_bn_free(key_state->p);
    key_state->p = NULL;
    _libssh2_bn_free(key_state->g);
    key_state->g = NULL;
    key_state->state = libssh2_NB_state_idle;

    return ret;
}



/* kex_method_diffie_hellman_group14_sha1_key_exchange
 * Diffie-Hellman Group14 Key Exchange using SHA1
 */
static int
kex_method_diffie_hellman_group14_sha1_key_exchange(LIBSSH2_SESSION *session,
                                                    key_exchange_state_low_t
                                                    * key_state)
{
    static const unsigned char p_value[256] = {
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
        0xC9, 0x0F, 0xDA, 0xA2, 0x21, 0x68, 0xC2, 0x34,
        0xC4, 0xC6, 0x62, 0x8B, 0x80, 0xDC, 0x1C, 0xD1,
        0x29, 0x02, 0x4E, 0x08, 0x8A, 0x67, 0xCC, 0x74,
        0x02, 0x0B, 0xBE, 0xA6, 0x3B, 0x13, 0x9B, 0x22,
        0x51, 0x4A, 0x08, 0x79, 0x8E, 0x34, 0x04, 0xDD,
        0xEF, 0x95, 0x19, 0xB3, 0xCD, 0x3A, 0x43, 0x1B,
        0x30, 0x2B, 0x0A, 0x6D, 0xF2, 0x5F, 0x14, 0x37,
        0x4F, 0xE1, 0x35, 0x6D, 0x6D, 0x51, 0xC2, 0x45,
        0xE4, 0x85, 0xB5, 0x76, 0x62, 0x5E, 0x7E, 0xC6,
        0xF4, 0x4C, 0x42, 0xE9, 0xA6, 0x37, 0xED, 0x6B,
        0x0B, 0xFF, 0x5C, 0xB6, 0xF4, 0x06, 0xB7, 0xED,
        0xEE, 0x38, 0x6B, 0xFB, 0x5A, 0x89, 0x9F, 0xA5,
        0xAE, 0x9F, 0x24, 0x11, 0x7C, 0x4B, 0x1F, 0xE6,
        0x49, 0x28, 0x66, 0x51, 0xEC, 0xE4, 0x5B, 0x3D,
        0xC2, 0x00, 0x7C, 0xB8, 0xA1, 0x63, 0xBF, 0x05,
        0x98, 0xDA, 0x48, 0x36, 0x1C, 0x55, 0xD3, 0x9A,
        0x69, 0x16, 0x3F, 0xA8, 0xFD, 0x24, 0xCF, 0x5F,
        0x83, 0x65, 0x5D, 0x23, 0xDC, 0xA3, 0xAD, 0x96,
        0x1C, 0x62, 0xF3, 0x56, 0x20, 0x85, 0x52, 0xBB,
        0x9E, 0xD5, 0x29, 0x07, 0x70, 0x96, 0x96, 0x6D,
        0x67, 0x0C, 0x35, 0x4E, 0x4A, 0xBC, 0x98, 0x04,
        0xF1, 0x74, 0x6C, 0x08, 0xCA, 0x18, 0x21, 0x7C,
        0x32, 0x90, 0x5E, 0x46, 0x2E, 0x36, 0xCE, 0x3B,
        0xE3, 0x9E, 0x77, 0x2C, 0x18, 0x0E, 0x86, 0x03,
        0x9B, 0x27, 0x83, 0xA2, 0xEC, 0x07, 0xA2, 0x8F,
        0xB5, 0xC5, 0x5D, 0xF0, 0x6F, 0x4C, 0x52, 0xC9,
        0xDE, 0x2B, 0xCB, 0xF6, 0x95, 0x58, 0x17, 0x18,
        0x39, 0x95, 0x49, 0x7C, 0xEA, 0x95, 0x6A, 0xE5,
        0x15, 0xD2, 0x26, 0x18, 0x98, 0xFA, 0x05, 0x10,
        0x15, 0x72, 0x8E, 0x5A, 0x8A, 0xAC, 0xAA, 0x68,
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
    };
    int ret;

    if (key_state->state == libssh2_NB_state_idle) {
        key_state->p = _libssh2_bn_init();      /* SSH2 defined value (p_value) */
        key_state->g = _libssh2_bn_init();      /* SSH2 defined value (2) */

        /* g == 2 */
        /* Initialize P and G */
        _libssh2_bn_set_word(key_state->g, 2);
        _libssh2_bn_from_bin(key_state->p, 256, p_value);

        _libssh2_debug(session, LIBSSH2_TRACE_KEX,
                       "Initiating Diffie-Hellman Group14 Key Exchange");

        key_state->state = libssh2_NB_state_created;
    }
    ret = diffie_hellman_sha1(session, key_state->g, key_state->p,
                              256, SSH_MSG_KEXDH_INIT, SSH_MSG_KEXDH_REPLY,
                              NULL, 0, &key_state->exchange_state);
    if (ret == LIBSSH2_ERROR_EAGAIN) {
        return ret;
    }

    key_state->state = libssh2_NB_state_idle;
    _libssh2_bn_free(key_state->p);
    key_state->p = NULL;
    _libssh2_bn_free(key_state->g);
    key_state->g = NULL;

    return ret;
}



/* kex_method_diffie_hellman_group_exchange_sha1_key_exchange
 * Diffie-Hellman Group Exchange Key Exchange using SHA1
 * Negotiates random(ish) group for secret derivation
 */
static int
kex_method_diffie_hellman_group_exchange_sha1_key_exchange
(LIBSSH2_SESSION * session, key_exchange_state_low_t * key_state)
{
    unsigned long p_len, g_len;
    int ret = 0;
    int rc;

    if (key_state->state == libssh2_NB_state_idle) {
        key_state->p = _libssh2_bn_init();
        key_state->g = _libssh2_bn_init();
        /* Ask for a P and G pair */
#ifdef LIBSSH2_DH_GEX_NEW
        key_state->request[0] = SSH_MSG_KEX_DH_GEX_REQUEST;
        _libssh2_htonu32(key_state->request + 1, LIBSSH2_DH_GEX_MINGROUP);
        _libssh2_htonu32(key_state->request + 5, LIBSSH2_DH_GEX_OPTGROUP);
        _libssh2_htonu32(key_state->request + 9, LIBSSH2_DH_GEX_MAXGROUP);
        key_state->request_len = 13;
        _libssh2_debug(session, LIBSSH2_TRACE_KEX,
                       "Initiating Diffie-Hellman Group-Exchange (New Method)");
#else
        key_state->request[0] = SSH_MSG_KEX_DH_GEX_REQUEST_OLD;
        _libssh2_htonu32(key_state->request + 1, LIBSSH2_DH_GEX_OPTGROUP);
        key_state->request_len = 5;
        _libssh2_debug(session, LIBSSH2_TRACE_KEX,
                       "Initiating Diffie-Hellman Group-Exchange (Old Method)");
#endif

        key_state->state = libssh2_NB_state_created;
    }

    if (key_state->state == libssh2_NB_state_created) {
        rc = _libssh2_transport_send(session, key_state->request,
                                     key_state->request_len, NULL, 0);
        if (rc == LIBSSH2_ERROR_EAGAIN) {
            return rc;
        } else if (rc) {
            ret = _libssh2_error(session, rc,
                                 "Unable to send Group Exchange Request");
            goto dh_gex_clean_exit;
        }

        key_state->state = libssh2_NB_state_sent;
    }

    if (key_state->state == libssh2_NB_state_sent) {
        rc = _libssh2_packet_require(session, SSH_MSG_KEX_DH_GEX_GROUP,
                                     &key_state->data, &key_state->data_len,
                                     0, NULL, 0, &key_state->req_state);
        if (rc == LIBSSH2_ERROR_EAGAIN) {
            return rc;
        } else if (rc) {
            ret = _libssh2_error(session, rc,
                                 "Timeout waiting for GEX_GROUP reply");
            goto dh_gex_clean_exit;
        }

        key_state->state = libssh2_NB_state_sent1;
    }

    if (key_state->state == libssh2_NB_state_sent1) {
        unsigned char *s = key_state->data + 1;
        p_len = _libssh2_ntohu32(s);
        s += 4;
        _libssh2_bn_from_bin(key_state->p, p_len, s);
        s += p_len;

        g_len = _libssh2_ntohu32(s);
        s += 4;
        _libssh2_bn_from_bin(key_state->g, g_len, s);

        ret = diffie_hellman_sha1(session, key_state->g, key_state->p, p_len,
                                  SSH_MSG_KEX_DH_GEX_INIT,
                                  SSH_MSG_KEX_DH_GEX_REPLY,
                                  key_state->data + 1,
                                  key_state->data_len - 1,
                                  &key_state->exchange_state);
        if (ret == LIBSSH2_ERROR_EAGAIN) {
            return ret;
        }

        LIBSSH2_FREE(session, key_state->data);
    }

  dh_gex_clean_exit:
    key_state->state = libssh2_NB_state_idle;
    _libssh2_bn_free(key_state->g);
    key_state->g = NULL;
    _libssh2_bn_free(key_state->p);
    key_state->p = NULL;

    return ret;
}



#define LIBSSH2_KEX_METHOD_FLAG_REQ_ENC_HOSTKEY     0x0001
#define LIBSSH2_KEX_METHOD_FLAG_REQ_SIGN_HOSTKEY    0x0002

static const LIBSSH2_KEX_METHOD kex_method_diffie_helman_group1_sha1 = {
    "diffie-hellman-group1-sha1",
    kex_method_diffie_hellman_group1_sha1_key_exchange,
    LIBSSH2_KEX_METHOD_FLAG_REQ_SIGN_HOSTKEY,
};

static const LIBSSH2_KEX_METHOD kex_method_diffie_helman_group14_sha1 = {
    "diffie-hellman-group14-sha1",
    kex_method_diffie_hellman_group14_sha1_key_exchange,
    LIBSSH2_KEX_METHOD_FLAG_REQ_SIGN_HOSTKEY,
};

static const LIBSSH2_KEX_METHOD
kex_method_diffie_helman_group_exchange_sha1 = {
    "diffie-hellman-group-exchange-sha1",
    kex_method_diffie_hellman_group_exchange_sha1_key_exchange,
    LIBSSH2_KEX_METHOD_FLAG_REQ_SIGN_HOSTKEY,
};

static const LIBSSH2_KEX_METHOD *libssh2_kex_methods[] = {
    &kex_method_diffie_helman_group14_sha1,
    &kex_method_diffie_helman_group_exchange_sha1,
    &kex_method_diffie_helman_group1_sha1,
    NULL
};

typedef struct _LIBSSH2_COMMON_METHOD
{
    const char *name;
} LIBSSH2_COMMON_METHOD;

/* kex_method_strlen
 * Calculate the length of a particular method list's resulting string
 * Includes SUM(strlen() of each individual method plus 1 (for coma)) - 1 (because the last coma isn't used)
 * Another sign of bad coding practices gone mad.  Pretend you don't see this.
 */
static size_t
kex_method_strlen(LIBSSH2_COMMON_METHOD ** method)
{
    size_t len = 0;

    if (!method || !*method) {
        return 0;
    }

    while (*method && (*method)->name) {
        len += strlen((*method)->name) + 1;
        method++;
    }

    return len - 1;
}



/* kex_method_list
 * Generate formatted preference list in buf
 */
static size_t
kex_method_list(unsigned char *buf, size_t list_strlen,
                LIBSSH2_COMMON_METHOD ** method)
{
    _libssh2_htonu32(buf, list_strlen);
    buf += 4;

    if (!method || !*method) {
        return 4;
    }

    while (*method && (*method)->name) {
        int mlen = strlen((*method)->name);
        memcpy(buf, (*method)->name, mlen);
        buf += mlen;
        *(buf++) = ',';
        method++;
    }

    return list_strlen + 4;
}



#define LIBSSH2_METHOD_PREFS_LEN(prefvar, defaultvar)           \
    ((prefvar) ? strlen(prefvar) :                              \
     kex_method_strlen((LIBSSH2_COMMON_METHOD**)(defaultvar)))

#define LIBSSH2_METHOD_PREFS_STR(buf, prefvarlen, prefvar, defaultvar)  \
    if (prefvar) {                                                      \
        _libssh2_htonu32((buf), (prefvarlen));                          \
        buf += 4;                                                       \
        memcpy((buf), (prefvar), (prefvarlen));                         \
        buf += (prefvarlen);                                            \
    } else {                                                            \
        buf += kex_method_list((buf), (prefvarlen),                     \
                               (LIBSSH2_COMMON_METHOD**)(defaultvar));  \
    }

/* kexinit
 * Send SSH_MSG_KEXINIT packet
 */
static int kexinit(LIBSSH2_SESSION * session)
{
    /* 62 = packet_type(1) + cookie(16) + first_packet_follows(1) +
       reserved(4) + length longs(40) */
    size_t data_len = 62;
    size_t kex_len, hostkey_len = 0;
    size_t crypt_cs_len, crypt_sc_len;
    size_t comp_cs_len, comp_sc_len;
    size_t mac_cs_len, mac_sc_len;
    size_t lang_cs_len, lang_sc_len;
    unsigned char *data, *s;
    int rc;

    if (session->kexinit_state == libssh2_NB_state_idle) {
        kex_len =
            LIBSSH2_METHOD_PREFS_LEN(session->kex_prefs, libssh2_kex_methods);
        hostkey_len =
            LIBSSH2_METHOD_PREFS_LEN(session->hostkey_prefs,
                                     libssh2_hostkey_methods());
        crypt_cs_len =
            LIBSSH2_METHOD_PREFS_LEN(session->local.crypt_prefs,
                                     libssh2_crypt_methods());
        crypt_sc_len =
            LIBSSH2_METHOD_PREFS_LEN(session->remote.crypt_prefs,
                                     libssh2_crypt_methods());
        mac_cs_len =
            LIBSSH2_METHOD_PREFS_LEN(session->local.mac_prefs,
                                     _libssh2_mac_methods());
        mac_sc_len =
            LIBSSH2_METHOD_PREFS_LEN(session->remote.mac_prefs,
                                     _libssh2_mac_methods());
        comp_cs_len =
            LIBSSH2_METHOD_PREFS_LEN(session->local.comp_prefs,
                                     _libssh2_comp_methods(session));
        comp_sc_len =
            LIBSSH2_METHOD_PREFS_LEN(session->remote.comp_prefs,
                                     _libssh2_comp_methods(session));
        lang_cs_len =
            LIBSSH2_METHOD_PREFS_LEN(session->local.lang_prefs, NULL);
        lang_sc_len =
            LIBSSH2_METHOD_PREFS_LEN(session->remote.lang_prefs, NULL);

        data_len += kex_len + hostkey_len + crypt_cs_len + crypt_sc_len +
            comp_cs_len + comp_sc_len + mac_cs_len + mac_sc_len +
            lang_cs_len + lang_sc_len;

        s = data = LIBSSH2_ALLOC(session, data_len);
        if (!data) {
            return _libssh2_error(session, LIBSSH2_ERROR_ALLOC,
                                  "Unable to allocate memory");
        }

        *(s++) = SSH_MSG_KEXINIT;

        _libssh2_random(s, 16);
        s += 16;

        /* Ennumerating through these lists twice is probably (certainly?)
           inefficient from a CPU standpoint, but it saves multiple
           malloc/realloc calls */
        LIBSSH2_METHOD_PREFS_STR(s, kex_len, session->kex_prefs,
                                 libssh2_kex_methods);
        LIBSSH2_METHOD_PREFS_STR(s, hostkey_len, session->hostkey_prefs,
                                 libssh2_hostkey_methods());
        LIBSSH2_METHOD_PREFS_STR(s, crypt_cs_len, session->local.crypt_prefs,
                                 libssh2_crypt_methods());
        LIBSSH2_METHOD_PREFS_STR(s, crypt_sc_len, session->remote.crypt_prefs,
                                 libssh2_crypt_methods());
        LIBSSH2_METHOD_PREFS_STR(s, mac_cs_len, session->local.mac_prefs,
                                 _libssh2_mac_methods());
        LIBSSH2_METHOD_PREFS_STR(s, mac_sc_len, session->remote.mac_prefs,
                                 _libssh2_mac_methods());
        LIBSSH2_METHOD_PREFS_STR(s, comp_cs_len, session->local.comp_prefs,
                                 _libssh2_comp_methods(session));
        LIBSSH2_METHOD_PREFS_STR(s, comp_sc_len, session->remote.comp_prefs,
                                 _libssh2_comp_methods(session));
        LIBSSH2_METHOD_PREFS_STR(s, lang_cs_len, session->local.lang_prefs,
                                 NULL);
        LIBSSH2_METHOD_PREFS_STR(s, lang_sc_len, session->remote.lang_prefs,
                                 NULL);

        /* No optimistic KEX packet follows */
        /* Deal with optimistic packets
         * session->flags |= KEXINIT_OPTIMISTIC
         * session->flags |= KEXINIT_METHODSMATCH
         */
        *(s++) = 0;

        /* Reserved == 0 */
        _libssh2_htonu32(s, 0);

#ifdef LIBSSH2DEBUG
        {
            /* Funnily enough, they'll all "appear" to be '\0' terminated */
            unsigned char *p = data + 21;       /* type(1) + cookie(16) + len(4) */

            _libssh2_debug(session, LIBSSH2_TRACE_KEX, "Sent KEX: %s", p);
            p += kex_len + 4;
            _libssh2_debug(session, LIBSSH2_TRACE_KEX, "Sent HOSTKEY: %s", p);
            p += hostkey_len + 4;
            _libssh2_debug(session, LIBSSH2_TRACE_KEX, "Sent CRYPT_CS: %s", p);
            p += crypt_cs_len + 4;
            _libssh2_debug(session, LIBSSH2_TRACE_KEX, "Sent CRYPT_SC: %s", p);
            p += crypt_sc_len + 4;
            _libssh2_debug(session, LIBSSH2_TRACE_KEX, "Sent MAC_CS: %s", p);
            p += mac_cs_len + 4;
            _libssh2_debug(session, LIBSSH2_TRACE_KEX, "Sent MAC_SC: %s", p);
            p += mac_sc_len + 4;
            _libssh2_debug(session, LIBSSH2_TRACE_KEX, "Sent COMP_CS: %s", p);
            p += comp_cs_len + 4;
            _libssh2_debug(session, LIBSSH2_TRACE_KEX, "Sent COMP_SC: %s", p);
            p += comp_sc_len + 4;
            _libssh2_debug(session, LIBSSH2_TRACE_KEX, "Sent LANG_CS: %s", p);
            p += lang_cs_len + 4;
            _libssh2_debug(session, LIBSSH2_TRACE_KEX, "Sent LANG_SC: %s", p);
            p += lang_sc_len + 4;
        }
#endif /* LIBSSH2DEBUG */

        session->kexinit_state = libssh2_NB_state_created;
    } else {
        data = session->kexinit_data;
        data_len = session->kexinit_data_len;
        /* zap the variables to ensure there is NOT a double free later */
        session->kexinit_data = NULL;
        session->kexinit_data_len = 0;
    }

    rc = _libssh2_transport_send(session, data, data_len, NULL, 0);
    if (rc == LIBSSH2_ERROR_EAGAIN) {
        session->kexinit_data = data;
        session->kexinit_data_len = data_len;
        return rc;
    }
    else if (rc) {
        LIBSSH2_FREE(session, data);
        session->kexinit_state = libssh2_NB_state_idle;
        return _libssh2_error(session, rc,
                              "Unable to send KEXINIT packet to remote host");

    }

    if (session->local.kexinit) {
        LIBSSH2_FREE(session, session->local.kexinit);
    }

    session->local.kexinit = data;
    session->local.kexinit_len = data_len;

    session->kexinit_state = libssh2_NB_state_idle;

    return 0;
}

/* kex_agree_instr
 * Kex specific variant of strstr()
 * Needle must be preceed by BOL or ',', and followed by ',' or EOL
 */
static unsigned char *
kex_agree_instr(unsigned char *haystack, unsigned long haystack_len,
                const unsigned char *needle, unsigned long needle_len)
{
    unsigned char *s;

    /* Haystack too short to bother trying */
    if (haystack_len < needle_len) {
        return NULL;
    }

    /* Needle at start of haystack */
    if ((strncmp((char *) haystack, (char *) needle, needle_len) == 0) &&
        (needle_len == haystack_len || haystack[needle_len] == ',')) {
        return haystack;
    }

    s = haystack;
    /* Search until we run out of comas or we run out of haystack,
       whichever comes first */
    while ((s = (unsigned char *) strchr((char *) s, ','))
           && ((haystack_len - (s - haystack)) > needle_len)) {
        s++;
        /* Needle at X position */
        if ((strncmp((char *) s, (char *) needle, needle_len) == 0) &&
            (((s - haystack) + needle_len) == haystack_len
             || s[needle_len] == ',')) {
            return s;
        }
    }

    return NULL;
}



/* kex_get_method_by_name
 */
static const LIBSSH2_COMMON_METHOD *
kex_get_method_by_name(const char *name, size_t name_len,
                       const LIBSSH2_COMMON_METHOD ** methodlist)
{
    while (*methodlist) {
        if ((strlen((*methodlist)->name) == name_len) &&
            (strncmp((*methodlist)->name, name, name_len) == 0)) {
            return *methodlist;
        }
        methodlist++;
    }
    return NULL;
}



/* kex_agree_hostkey
 * Agree on a Hostkey which works with this kex
 */
static int kex_agree_hostkey(LIBSSH2_SESSION * session,
                             unsigned long kex_flags,
                             unsigned char *hostkey, unsigned long hostkey_len)
{
    const LIBSSH2_HOSTKEY_METHOD **hostkeyp = libssh2_hostkey_methods();
    unsigned char *s;

    if (session->hostkey_prefs) {
        s = (unsigned char *) session->hostkey_prefs;

        while (s && *s) {
            unsigned char *p = (unsigned char *) strchr((char *) s, ',');
            size_t method_len = (p ? (size_t)(p - s) : strlen((char *) s));
            if (kex_agree_instr(hostkey, hostkey_len, s, method_len)) {
                const LIBSSH2_HOSTKEY_METHOD *method =
                    (const LIBSSH2_HOSTKEY_METHOD *)
                    kex_get_method_by_name((char *) s, method_len,
                                           (const LIBSSH2_COMMON_METHOD **)
                                           hostkeyp);

                if (!method) {
                    /* Invalid method -- Should never be reached */
                    return -1;
                }

                /* So far so good, but does it suit our purposes? (Encrypting
                   vs Signing) */
                if (((kex_flags & LIBSSH2_KEX_METHOD_FLAG_REQ_ENC_HOSTKEY) ==
                     0) || (method->encrypt)) {
                    /* Either this hostkey can do encryption or this kex just
                       doesn't require it */
                    if (((kex_flags & LIBSSH2_KEX_METHOD_FLAG_REQ_SIGN_HOSTKEY)
                         == 0) || (method->sig_verify)) {
                        /* Either this hostkey can do signing or this kex just
                           doesn't require it */
                        session->hostkey = method;
                        return 0;
                    }
                }
            }

            s = p ? p + 1 : NULL;
        }
        return -1;
    }

    while (hostkeyp && (*hostkeyp) && (*hostkeyp)->name) {
        s = kex_agree_instr(hostkey, hostkey_len,
                            (unsigned char *) (*hostkeyp)->name,
                            strlen((*hostkeyp)->name));
        if (s) {
            /* So far so good, but does it suit our purposes? (Encrypting vs
               Signing) */
            if (((kex_flags & LIBSSH2_KEX_METHOD_FLAG_REQ_ENC_HOSTKEY) == 0) ||
                ((*hostkeyp)->encrypt)) {
                /* Either this hostkey can do encryption or this kex just
                   doesn't require it */
                if (((kex_flags & LIBSSH2_KEX_METHOD_FLAG_REQ_SIGN_HOSTKEY) ==
                     0) || ((*hostkeyp)->sig_verify)) {
                    /* Either this hostkey can do signing or this kex just
                       doesn't require it */
                    session->hostkey = *hostkeyp;
                    return 0;
                }
            }
        }
        hostkeyp++;
    }

    return -1;
}



/* kex_agree_kex_hostkey
 * Agree on a Key Exchange method and a hostkey encoding type
 */
static int kex_agree_kex_hostkey(LIBSSH2_SESSION * session, unsigned char *kex,
                                 unsigned long kex_len, unsigned char *hostkey,
                                 unsigned long hostkey_len)
{
    const LIBSSH2_KEX_METHOD **kexp = libssh2_kex_methods;
    unsigned char *s;

    if (session->kex_prefs) {
        s = (unsigned char *) session->kex_prefs;

        while (s && *s) {
            unsigned char *q, *p = (unsigned char *) strchr((char *) s, ',');
            size_t method_len = (p ? (size_t)(p - s) : strlen((char *) s));
            if ((q = kex_agree_instr(kex, kex_len, s, method_len))) {
                const LIBSSH2_KEX_METHOD *method = (const LIBSSH2_KEX_METHOD *)
                    kex_get_method_by_name((char *) s, method_len,
                                           (const LIBSSH2_COMMON_METHOD **)
                                           kexp);

                if (!method) {
                    /* Invalid method -- Should never be reached */
                    return -1;
                }

                /* We've agreed on a key exchange method,
                 * Can we agree on a hostkey that works with this kex?
                 */
                if (kex_agree_hostkey(session, method->flags, hostkey,
                                      hostkey_len) == 0) {
                    session->kex = method;
                    if (session->burn_optimistic_kexinit && (kex == q)) {
                        /* Server sent an optimistic packet,
                         * and client agrees with preference
                         * cancel burning the first KEX_INIT packet that comes in */
                        session->burn_optimistic_kexinit = 0;
                    }
                    return 0;
                }
            }

            s = p ? p + 1 : NULL;
        }
        return -1;
    }

    while (*kexp && (*kexp)->name) {
        s = kex_agree_instr(kex, kex_len,
                            (unsigned char *) (*kexp)->name,
                            strlen((*kexp)->name));
        if (s) {
            /* We've agreed on a key exchange method,
             * Can we agree on a hostkey that works with this kex?
             */
            if (kex_agree_hostkey(session, (*kexp)->flags, hostkey,
                                  hostkey_len) == 0) {
                session->kex = *kexp;
                if (session->burn_optimistic_kexinit && (kex == s)) {
                    /* Server sent an optimistic packet,
                     * and client agrees with preference
                     * cancel burning the first KEX_INIT packet that comes in */
                    session->burn_optimistic_kexinit = 0;
                }
                return 0;
            }
        }
        kexp++;
    }
    return -1;
}



/* kex_agree_crypt
 * Agree on a cipher algo
 */
static int kex_agree_crypt(LIBSSH2_SESSION * session,
                           libssh2_endpoint_data *endpoint,
                           unsigned char *crypt,
                           unsigned long crypt_len)
{
    const LIBSSH2_CRYPT_METHOD **cryptp = libssh2_crypt_methods();
    unsigned char *s;

    (void) session;

    if (endpoint->crypt_prefs) {
        s = (unsigned char *) endpoint->crypt_prefs;

        while (s && *s) {
            unsigned char *p = (unsigned char *) strchr((char *) s, ',');
            size_t method_len = (p ? (size_t)(p - s) : strlen((char *) s));

            if (kex_agree_instr(crypt, crypt_len, s, method_len)) {
                const LIBSSH2_CRYPT_METHOD *method =
                    (const LIBSSH2_CRYPT_METHOD *)
                    kex_get_method_by_name((char *) s, method_len,
                                           (const LIBSSH2_COMMON_METHOD **)
                                           cryptp);

                if (!method) {
                    /* Invalid method -- Should never be reached */
                    return -1;
                }

                endpoint->crypt = method;
                return 0;
            }

            s = p ? p + 1 : NULL;
        }
        return -1;
    }

    while (*cryptp && (*cryptp)->name) {
        s = kex_agree_instr(crypt, crypt_len,
                            (unsigned char *) (*cryptp)->name,
                            strlen((*cryptp)->name));
        if (s) {
            endpoint->crypt = *cryptp;
            return 0;
        }
        cryptp++;
    }

    return -1;
}



/* kex_agree_mac
 * Agree on a message authentication hash
 */
static int kex_agree_mac(LIBSSH2_SESSION * session,
                         libssh2_endpoint_data * endpoint, unsigned char *mac,
                         unsigned long mac_len)
{
    const LIBSSH2_MAC_METHOD **macp = _libssh2_mac_methods();
    unsigned char *s;
    (void) session;

    if (endpoint->mac_prefs) {
        s = (unsigned char *) endpoint->mac_prefs;

        while (s && *s) {
            unsigned char *p = (unsigned char *) strchr((char *) s, ',');
            size_t method_len = (p ? (size_t)(p - s) : strlen((char *) s));

            if (kex_agree_instr(mac, mac_len, s, method_len)) {
                const LIBSSH2_MAC_METHOD *method = (const LIBSSH2_MAC_METHOD *)
                    kex_get_method_by_name((char *) s, method_len,
                                           (const LIBSSH2_COMMON_METHOD **)
                                           macp);

                if (!method) {
                    /* Invalid method -- Should never be reached */
                    return -1;
                }

                endpoint->mac = method;
                return 0;
            }

            s = p ? p + 1 : NULL;
        }
        return -1;
    }

    while (*macp && (*macp)->name) {
        s = kex_agree_instr(mac, mac_len, (unsigned char *) (*macp)->name,
                            strlen((*macp)->name));
        if (s) {
            endpoint->mac = *macp;
            return 0;
        }
        macp++;
    }

    return -1;
}



/* kex_agree_comp
 * Agree on a compression scheme
 */
static int kex_agree_comp(LIBSSH2_SESSION *session,
                          libssh2_endpoint_data *endpoint, unsigned char *comp,
                          unsigned long comp_len)
{
    const LIBSSH2_COMP_METHOD **compp = _libssh2_comp_methods(session);
    unsigned char *s;
    (void) session;

    if (endpoint->comp_prefs) {
        s = (unsigned char *) endpoint->comp_prefs;

        while (s && *s) {
            unsigned char *p = (unsigned char *) strchr((char *) s, ',');
            size_t method_len = (p ? (size_t)(p - s) : strlen((char *) s));

            if (kex_agree_instr(comp, comp_len, s, method_len)) {
                const LIBSSH2_COMP_METHOD *method =
                    (const LIBSSH2_COMP_METHOD *)
                    kex_get_method_by_name((char *) s, method_len,
                                           (const LIBSSH2_COMMON_METHOD **)
                                           compp);

                if (!method) {
                    /* Invalid method -- Should never be reached */
                    return -1;
                }

                endpoint->comp = method;
                return 0;
            }

            s = p ? p + 1 : NULL;
        }
        return -1;
    }

    while (*compp && (*compp)->name) {
        s = kex_agree_instr(comp, comp_len, (unsigned char *) (*compp)->name,
                            strlen((*compp)->name));
        if (s) {
            endpoint->comp = *compp;
            return 0;
        }
        compp++;
    }

    return -1;
}



/* TODO: When in server mode we need to turn this logic on its head
 * The Client gets to make the final call on "agreed methods"
 */

/* kex_agree_methods
 * Decide which specific method to use of the methods offered by each party
 */
static int kex_agree_methods(LIBSSH2_SESSION * session, unsigned char *data,
                             unsigned data_len)
{
    unsigned char *kex, *hostkey, *crypt_cs, *crypt_sc, *comp_cs, *comp_sc,
        *mac_cs, *mac_sc;
    size_t kex_len, hostkey_len, crypt_cs_len, crypt_sc_len, comp_cs_len;
    size_t comp_sc_len, mac_cs_len, mac_sc_len;
    unsigned char *s = data;

    /* Skip packet_type, we know it already */
    s++;

    /* Skip cookie, don't worry, it's preserved in the kexinit field */
    s += 16;

    /* Locate each string */
    kex_len = _libssh2_ntohu32(s);
    kex = s + 4;
    s += 4 + kex_len;
    hostkey_len = _libssh2_ntohu32(s);
    hostkey = s + 4;
    s += 4 + hostkey_len;
    crypt_cs_len = _libssh2_ntohu32(s);
    crypt_cs = s + 4;
    s += 4 + crypt_cs_len;
    crypt_sc_len = _libssh2_ntohu32(s);
    crypt_sc = s + 4;
    s += 4 + crypt_sc_len;
    mac_cs_len = _libssh2_ntohu32(s);
    mac_cs = s + 4;
    s += 4 + mac_cs_len;
    mac_sc_len = _libssh2_ntohu32(s);
    mac_sc = s + 4;
    s += 4 + mac_sc_len;
    comp_cs_len = _libssh2_ntohu32(s);
    comp_cs = s + 4;
    s += 4 + comp_cs_len;
    comp_sc_len = _libssh2_ntohu32(s);
    comp_sc = s + 4;
#if 0
    s += 4 + comp_sc_len;
    lang_cs_len = _libssh2_ntohu32(s);
    lang_cs = s + 4;
    s += 4 + lang_cs_len;
    lang_sc_len = _libssh2_ntohu32(s);
    lang_sc = s + 4;
    s += 4 + lang_sc_len;
#endif
    /* If the server sent an optimistic packet, assume that it guessed wrong.
     * If the guess is determined to be right (by kex_agree_kex_hostkey)
     * This flag will be reset to zero so that it's not ignored */
    session->burn_optimistic_kexinit = *(s++);
    /* Next uint32 in packet is all zeros (reserved) */

    if (data_len < (unsigned) (s - data))
        return -1;              /* short packet */

    if (kex_agree_kex_hostkey(session, kex, kex_len, hostkey, hostkey_len)) {
        return -1;
    }

    if (kex_agree_crypt(session, &session->local, crypt_cs, crypt_cs_len)
        || kex_agree_crypt(session, &session->remote, crypt_sc, crypt_sc_len)) {
        return -1;
    }

    if (kex_agree_mac(session, &session->local, mac_cs, mac_cs_len) ||
        kex_agree_mac(session, &session->remote, mac_sc, mac_sc_len)) {
        return -1;
    }

    if (kex_agree_comp(session, &session->local, comp_cs, comp_cs_len) ||
        kex_agree_comp(session, &session->remote, comp_sc, comp_sc_len)) {
        return -1;
    }

#if 0
    if (libssh2_kex_agree_lang(session, &session->local, lang_cs, lang_cs_len)
        || libssh2_kex_agree_lang(session, &session->remote, lang_sc,
                                  lang_sc_len)) {
        return -1;
    }
#endif

    _libssh2_debug(session, LIBSSH2_TRACE_KEX, "Agreed on KEX method: %s",
                   session->kex->name);
    _libssh2_debug(session, LIBSSH2_TRACE_KEX, "Agreed on HOSTKEY method: %s",
                   session->hostkey->name);
    _libssh2_debug(session, LIBSSH2_TRACE_KEX, "Agreed on CRYPT_CS method: %s",
                   session->local.crypt->name);
    _libssh2_debug(session, LIBSSH2_TRACE_KEX, "Agreed on CRYPT_SC method: %s",
                   session->remote.crypt->name);
    _libssh2_debug(session, LIBSSH2_TRACE_KEX, "Agreed on MAC_CS method: %s",
                   session->local.mac->name);
    _libssh2_debug(session, LIBSSH2_TRACE_KEX, "Agreed on MAC_SC method: %s",
                   session->remote.mac->name);
    _libssh2_debug(session, LIBSSH2_TRACE_KEX, "Agreed on COMP_CS method: %s",
                   session->local.comp->name);
    _libssh2_debug(session, LIBSSH2_TRACE_KEX, "Agreed on COMP_SC method: %s",
                   session->remote.comp->name);

    return 0;
}



/* _libssh2_kex_exchange
 * Exchange keys
 * Returns 0 on success, non-zero on failure
 *
 * Returns some errors without _libssh2_error()
 */
int
_libssh2_kex_exchange(LIBSSH2_SESSION * session, int reexchange,
                     key_exchange_state_t * key_state)
{
    int rc = 0;
    int retcode;

    session->state |= LIBSSH2_STATE_KEX_ACTIVE;

    if (key_state->state == libssh2_NB_state_idle) {
        /* Prevent loop in packet_add() */
        session->state |= LIBSSH2_STATE_EXCHANGING_KEYS;

        if (reexchange) {
            session->kex = NULL;

            if (session->hostkey && session->hostkey->dtor) {
                session->hostkey->dtor(session,
                                       &session->server_hostkey_abstract);
            }
            session->hostkey = NULL;
        }

        key_state->state = libssh2_NB_state_created;
    }

    if (!session->kex || !session->hostkey) {
        if (key_state->state == libssh2_NB_state_created) {
            /* Preserve in case of failure */
            key_state->oldlocal = session->local.kexinit;
            key_state->oldlocal_len = session->local.kexinit_len;

            session->local.kexinit = NULL;

            key_state->state = libssh2_NB_state_sent;
        }

        if (key_state->state == libssh2_NB_state_sent) {
            retcode = kexinit(session);
            if (retcode == LIBSSH2_ERROR_EAGAIN) {
                session->state &= ~LIBSSH2_STATE_KEX_ACTIVE;
                return retcode;
            } else if (retcode) {
                session->local.kexinit = key_state->oldlocal;
                session->local.kexinit_len = key_state->oldlocal_len;
                key_state->state = libssh2_NB_state_idle;
                session->state &= ~LIBSSH2_STATE_KEX_ACTIVE;
                session->state &= ~LIBSSH2_STATE_EXCHANGING_KEYS;
                return -1;
            }

            key_state->state = libssh2_NB_state_sent1;
        }

        if (key_state->state == libssh2_NB_state_sent1) {
            retcode =
                _libssh2_packet_require(session, SSH_MSG_KEXINIT,
                                        &key_state->data,
                                        &key_state->data_len, 0, NULL, 0,
                                        &key_state->req_state);
            if (retcode == LIBSSH2_ERROR_EAGAIN) {
                session->state &= ~LIBSSH2_STATE_KEX_ACTIVE;
                return retcode;
            }
            else if (retcode) {
                if (session->local.kexinit) {
                    LIBSSH2_FREE(session, session->local.kexinit);
                }
                session->local.kexinit = key_state->oldlocal;
                session->local.kexinit_len = key_state->oldlocal_len;
                key_state->state = libssh2_NB_state_idle;
                session->state &= ~LIBSSH2_STATE_KEX_ACTIVE;
                session->state &= ~LIBSSH2_STATE_EXCHANGING_KEYS;
                return -1;
            }

            if (session->remote.kexinit) {
                LIBSSH2_FREE(session, session->remote.kexinit);
            }
            session->remote.kexinit = key_state->data;
            session->remote.kexinit_len = key_state->data_len;

            if (kex_agree_methods(session, key_state->data,
                                  key_state->data_len))
                rc = LIBSSH2_ERROR_KEX_FAILURE;

            key_state->state = libssh2_NB_state_sent2;
        }
    } else {
        key_state->state = libssh2_NB_state_sent2;
    }

    if (rc == 0) {
        if (key_state->state == libssh2_NB_state_sent2) {
            retcode = session->kex->exchange_keys(session,
                                                  &key_state->key_state_low);
            if (retcode == LIBSSH2_ERROR_EAGAIN) {
                session->state &= ~LIBSSH2_STATE_KEX_ACTIVE;
                return retcode;
            } else if (retcode) {
                rc = _libssh2_error(session, LIBSSH2_ERROR_KEY_EXCHANGE_FAILURE,
                                    "Unrecoverable error exchanging keys");
            }
        }
    }

    /* Done with kexinit buffers */
    if (session->local.kexinit) {
        LIBSSH2_FREE(session, session->local.kexinit);
        session->local.kexinit = NULL;
    }
    if (session->remote.kexinit) {
        LIBSSH2_FREE(session, session->remote.kexinit);
        session->remote.kexinit = NULL;
    }

    session->state &= ~LIBSSH2_STATE_KEX_ACTIVE;
    session->state &= ~LIBSSH2_STATE_EXCHANGING_KEYS;

    key_state->state = libssh2_NB_state_idle;

    return rc;
}



/* libssh2_session_method_pref
 * Set preferred method
 */
LIBSSH2_API int
libssh2_session_method_pref(LIBSSH2_SESSION * session, int method_type,
                            const char *prefs)
{
    char **prefvar, *s, *newprefs;
    int prefs_len = strlen(prefs);
    const LIBSSH2_COMMON_METHOD **mlist;

    switch (method_type) {
    case LIBSSH2_METHOD_KEX:
        prefvar = &session->kex_prefs;
        mlist = (const LIBSSH2_COMMON_METHOD **) libssh2_kex_methods;
        break;

    case LIBSSH2_METHOD_HOSTKEY:
        prefvar = &session->hostkey_prefs;
        mlist = (const LIBSSH2_COMMON_METHOD **) libssh2_hostkey_methods();
        break;

    case LIBSSH2_METHOD_CRYPT_CS:
        prefvar = &session->local.crypt_prefs;
        mlist = (const LIBSSH2_COMMON_METHOD **) libssh2_crypt_methods();
        break;

    case LIBSSH2_METHOD_CRYPT_SC:
        prefvar = &session->remote.crypt_prefs;
        mlist = (const LIBSSH2_COMMON_METHOD **) libssh2_crypt_methods();
        break;

    case LIBSSH2_METHOD_MAC_CS:
        prefvar = &session->local.mac_prefs;
        mlist = (const LIBSSH2_COMMON_METHOD **) _libssh2_mac_methods();
        break;

    case LIBSSH2_METHOD_MAC_SC:
        prefvar = &session->remote.mac_prefs;
        mlist = (const LIBSSH2_COMMON_METHOD **) _libssh2_mac_methods();
        break;

    case LIBSSH2_METHOD_COMP_CS:
        prefvar = &session->local.comp_prefs;
        mlist = (const LIBSSH2_COMMON_METHOD **)
            _libssh2_comp_methods(session);
        break;

    case LIBSSH2_METHOD_COMP_SC:
        prefvar = &session->remote.comp_prefs;
        mlist = (const LIBSSH2_COMMON_METHOD **)
            _libssh2_comp_methods(session);
        break;

    case LIBSSH2_METHOD_LANG_CS:
        prefvar = &session->local.lang_prefs;
        mlist = NULL;
        break;

    case LIBSSH2_METHOD_LANG_SC:
        prefvar = &session->remote.lang_prefs;
        mlist = NULL;
        break;

    default:
        return _libssh2_error(session, LIBSSH2_ERROR_INVAL,
                              "Invalid parameter specified for method_type");
    }

    s = newprefs = LIBSSH2_ALLOC(session, prefs_len + 1);
    if (!newprefs) {
        return _libssh2_error(session, LIBSSH2_ERROR_ALLOC,
                              "Error allocated space for method preferences");
    }
    memcpy(s, prefs, prefs_len + 1);

    while (s && *s) {
        char *p = strchr(s, ',');
        int method_len = p ? (p - s) : (int) strlen(s);

        if (!kex_get_method_by_name(s, method_len, mlist)) {
            /* Strip out unsupported method */
            if (p) {
                memcpy(s, p + 1, strlen(s) - method_len);
            } else {
                if (s > newprefs) {
                    *(--s) = '\0';
                } else {
                    *s = '\0';
                }
            }
        }

        s = p ? (p + 1) : NULL;
    }

    if (strlen(newprefs) == 0) {
        LIBSSH2_FREE(session, newprefs);
        return _libssh2_error(session, LIBSSH2_ERROR_METHOD_NOT_SUPPORTED,
                              "The requested method(s) are not currently "
                              "supported");
    }

    if (*prefvar) {
        LIBSSH2_FREE(session, *prefvar);
    }
    *prefvar = newprefs;

    return 0;
}

/*
 * libssh2_session_supported_algs()
 * returns a number of returned algorithms (a positive number) on success,
 * a negative number on failure
 */

LIBSSH2_API int libssh2_session_supported_algs(LIBSSH2_SESSION* session,
                                               int method_type,
                                               const char*** algs)
{
    unsigned int i;
    unsigned int j;
    unsigned int ialg;
    const LIBSSH2_COMMON_METHOD **mlist;

    /* to prevent coredumps due to dereferencing of NULL */
    if (NULL == algs)
        return _libssh2_error(session, LIBSSH2_ERROR_BAD_USE,
                              "algs must not be NULL");

    switch (method_type) {
    case LIBSSH2_METHOD_KEX:
        mlist = (const LIBSSH2_COMMON_METHOD **) libssh2_kex_methods;
        break;

    case LIBSSH2_METHOD_HOSTKEY:
        mlist = (const LIBSSH2_COMMON_METHOD **) libssh2_hostkey_methods();
        break;

    case LIBSSH2_METHOD_CRYPT_CS:
    case LIBSSH2_METHOD_CRYPT_SC:
        mlist = (const LIBSSH2_COMMON_METHOD **) libssh2_crypt_methods();
        break;

    case LIBSSH2_METHOD_MAC_CS:
    case LIBSSH2_METHOD_MAC_SC:
        mlist = (const LIBSSH2_COMMON_METHOD **) _libssh2_mac_methods();
        break;

    case LIBSSH2_METHOD_COMP_CS:
    case LIBSSH2_METHOD_COMP_SC:
        mlist = (const LIBSSH2_COMMON_METHOD **) _libssh2_comp_methods(session);
        break;

    default:
        return _libssh2_error(session, LIBSSH2_ERROR_METHOD_NOT_SUPPORTED,
                              "Unknown method type");
    }  /* switch */

    /* weird situation */
    if (NULL==mlist)
        return _libssh2_error(session, LIBSSH2_ERROR_INVAL,
                              "No algorithm found");

    /*
      mlist is looped through twice. The first time to find the number od
      supported algorithms (needed to allocate the proper size of array) and
      the second time to actually copy the pointers.  Typically this function
      will not be called often (typically at the beginning of a session) and
      the number of algorithms (i.e. niumber of iterations in one loop) will
      not be high (typically it will not exceed 20) for quite a long time.

      So double looping really shouldn't be an issue and it is definitely a
      better solution than reallocation several times.
    */

    /* count the number of supported algorithms */
    for ( i=0, ialg=0; NULL!=mlist[i]; i++) {
        /* do not count fields with NULL name */
        if (mlist[i]->name)
            ialg++;
    }

    /* weird situation, no algorithm found */
    if (0==ialg)
        return _libssh2_error(session, LIBSSH2_ERROR_INVAL,
                              "No algorithm found");

    /* allocate buffer */
    *algs = (const char**) LIBSSH2_ALLOC(session, ialg*sizeof(const char*));
    if ( NULL==*algs ) {
        return _libssh2_error(session, LIBSSH2_ERROR_ALLOC,
                              "Memory allocation failed");
    }
    /* Past this point *algs must be deallocated in case of an error!! */

    /* copy non-NULL pointers only */
    for ( i=0, j=0; NULL!=mlist[i] && j<ialg; i++ ) {
        if ( NULL==mlist[i]->name ){
            /* maybe a weird situation but if it occurs, do not include NULL
               pointers */
            continue;
        }

        /* note that [] has higher priority than * (dereferencing) */
        (*algs)[j++] = mlist[i]->name;
    }

    /* correct number of pointers copied? (test the code above) */
    if ( j!=ialg ) {
        /* deallocate buffer */
        LIBSSH2_FREE(session, (void *)*algs);
        *algs = NULL;

        return _libssh2_error(session, LIBSSH2_ERROR_BAD_USE,
                              "Internal error");
    }

    return ialg;
}
