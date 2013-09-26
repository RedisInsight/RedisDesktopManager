/* Copyright (c) 2004-2006, Sara Golemon <sarag@libssh2.org>
 * Copyright (c) 2009 by Daniel Stenberg
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

/* Needed for struct iovec on some platforms */
#ifdef HAVE_SYS_UIO_H
#include <sys/uio.h>
#endif

#if LIBSSH2_RSA
/* ***********
 * ssh-rsa *
 *********** */

static int hostkey_method_ssh_rsa_dtor(LIBSSH2_SESSION * session,
                                       void **abstract);

/*
 * hostkey_method_ssh_rsa_init
 *
 * Initialize the server hostkey working area with e/n pair
 */
static int
hostkey_method_ssh_rsa_init(LIBSSH2_SESSION * session,
                            const unsigned char *hostkey_data,
                            size_t hostkey_data_len,
                            void **abstract)
{
    libssh2_rsa_ctx *rsactx;
    const unsigned char *s, *e, *n;
    unsigned long len, e_len, n_len;

    (void) hostkey_data_len;

    if (*abstract) {
        hostkey_method_ssh_rsa_dtor(session, abstract);
        *abstract = NULL;
    }

    s = hostkey_data;
    len = _libssh2_ntohu32(s);
    s += 4;

    if (len != 7 || strncmp((char *) s, "ssh-rsa", 7) != 0) {
        return -1;
    }
    s += 7;

    e_len = _libssh2_ntohu32(s);
    s += 4;

    e = s;
    s += e_len;
    n_len = _libssh2_ntohu32(s);
    s += 4;
    n = s;

    if (_libssh2_rsa_new(&rsactx, e, e_len, n, n_len, NULL, 0,
                         NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0))
        return -1;

    *abstract = rsactx;

    return 0;
}

/*
 * hostkey_method_ssh_rsa_initPEM
 *
 * Load a Private Key from a PEM file
 */
static int
hostkey_method_ssh_rsa_initPEM(LIBSSH2_SESSION * session,
                               const char *privkeyfile,
                               unsigned const char *passphrase,
                               void **abstract)
{
    libssh2_rsa_ctx *rsactx;
    int ret;

    if (*abstract) {
        hostkey_method_ssh_rsa_dtor(session, abstract);
        *abstract = NULL;
    }

    ret = _libssh2_rsa_new_private(&rsactx, session, privkeyfile, passphrase);
    if (ret) {
        return -1;
    }

    *abstract = rsactx;

    return 0;
}

/*
 * hostkey_method_ssh_rsa_sign
 *
 * Verify signature created by remote
 */
static int
hostkey_method_ssh_rsa_sig_verify(LIBSSH2_SESSION * session,
                                  const unsigned char *sig,
                                  size_t sig_len,
                                  const unsigned char *m,
                                  size_t m_len, void **abstract)
{
    libssh2_rsa_ctx *rsactx = (libssh2_rsa_ctx *) (*abstract);
    (void) session;

    /* Skip past keyname_len(4) + keyname(7){"ssh-rsa"} + signature_len(4) */
    sig += 15;
    sig_len -= 15;
    return _libssh2_rsa_sha1_verify(rsactx, sig, sig_len, m, m_len);
}

/*
 * hostkey_method_ssh_rsa_signv
 *
 * Construct a signature from an array of vectors
 */
static int
hostkey_method_ssh_rsa_signv(LIBSSH2_SESSION * session,
                             unsigned char **signature,
                             size_t *signature_len,
                             int veccount,
                             const struct iovec datavec[],
                             void **abstract)
{
    libssh2_rsa_ctx *rsactx = (libssh2_rsa_ctx *) (*abstract);
    int ret;
    int i;
    unsigned char hash[SHA_DIGEST_LENGTH];
    libssh2_sha1_ctx ctx;

    libssh2_sha1_init(&ctx);
    for(i = 0; i < veccount; i++) {
        libssh2_sha1_update(ctx, datavec[i].iov_base, datavec[i].iov_len);
    }
    libssh2_sha1_final(ctx, hash);

    ret = _libssh2_rsa_sha1_sign(session, rsactx, hash, SHA_DIGEST_LENGTH,
                                 signature, signature_len);
    if (ret) {
        return -1;
    }

    return 0;
}

/*
 * hostkey_method_ssh_rsa_dtor
 *
 * Shutdown the hostkey
 */
static int
hostkey_method_ssh_rsa_dtor(LIBSSH2_SESSION * session, void **abstract)
{
    libssh2_rsa_ctx *rsactx = (libssh2_rsa_ctx *) (*abstract);
    (void) session;

    _libssh2_rsa_free(rsactx);

    *abstract = NULL;

    return 0;
}

static const LIBSSH2_HOSTKEY_METHOD hostkey_method_ssh_rsa = {
    "ssh-rsa",
    MD5_DIGEST_LENGTH,
    hostkey_method_ssh_rsa_init,
    hostkey_method_ssh_rsa_initPEM,
    hostkey_method_ssh_rsa_sig_verify,
    hostkey_method_ssh_rsa_signv,
    NULL,                       /* encrypt */
    hostkey_method_ssh_rsa_dtor,
};
#endif /* LIBSSH2_RSA */

#if LIBSSH2_DSA
/* ***********
 * ssh-dss *
 *********** */

static int hostkey_method_ssh_dss_dtor(LIBSSH2_SESSION * session,
                                       void **abstract);

/*
 * hostkey_method_ssh_dss_init
 *
 * Initialize the server hostkey working area with p/q/g/y set
 */
static int
hostkey_method_ssh_dss_init(LIBSSH2_SESSION * session,
                            const unsigned char *hostkey_data,
                            size_t hostkey_data_len,
                            void **abstract)
{
    libssh2_dsa_ctx *dsactx;
    const unsigned char *p, *q, *g, *y, *s;
    unsigned long p_len, q_len, g_len, y_len, len;
    (void) hostkey_data_len;

    if (*abstract) {
        hostkey_method_ssh_dss_dtor(session, abstract);
        *abstract = NULL;
    }

    s = hostkey_data;
    len = _libssh2_ntohu32(s);
    s += 4;
    if (len != 7 || strncmp((char *) s, "ssh-dss", 7) != 0) {
        return -1;
    }
    s += 7;

    p_len = _libssh2_ntohu32(s);
    s += 4;
    p = s;
    s += p_len;
    q_len = _libssh2_ntohu32(s);
    s += 4;
    q = s;
    s += q_len;
    g_len = _libssh2_ntohu32(s);
    s += 4;
    g = s;
    s += g_len;
    y_len = _libssh2_ntohu32(s);
    s += 4;
    y = s;
    /* s += y_len; */

    _libssh2_dsa_new(&dsactx, p, p_len, q, q_len, g, g_len, y, y_len, NULL, 0);

    *abstract = dsactx;

    return 0;
}

/*
 * hostkey_method_ssh_dss_initPEM
 *
 * Load a Private Key from a PEM file
 */
static int
hostkey_method_ssh_dss_initPEM(LIBSSH2_SESSION * session,
                               const char *privkeyfile,
                               unsigned const char *passphrase,
                               void **abstract)
{
    libssh2_dsa_ctx *dsactx;
    int ret;

    if (*abstract) {
        hostkey_method_ssh_dss_dtor(session, abstract);
        *abstract = NULL;
    }

    ret = _libssh2_dsa_new_private(&dsactx, session, privkeyfile, passphrase);
    if (ret) {
        return -1;
    }

    *abstract = dsactx;

    return 0;
}

/*
 * libssh2_hostkey_method_ssh_dss_sign
 *
 * Verify signature created by remote
 */
static int
hostkey_method_ssh_dss_sig_verify(LIBSSH2_SESSION * session,
                                  const unsigned char *sig,
                                  size_t sig_len,
                                  const unsigned char *m,
                                  size_t m_len, void **abstract)
{
    libssh2_dsa_ctx *dsactx = (libssh2_dsa_ctx *) (*abstract);

    /* Skip past keyname_len(4) + keyname(7){"ssh-dss"} + signature_len(4) */
    sig += 15;
    sig_len -= 15;
    if (sig_len != 40) {
        return _libssh2_error(session, LIBSSH2_ERROR_PROTO,
                              "Invalid DSS signature length");
    }
    return _libssh2_dsa_sha1_verify(dsactx, sig, m, m_len);
}

/*
 * hostkey_method_ssh_dss_signv
 *
 * Construct a signature from an array of vectors
 */
static int
hostkey_method_ssh_dss_signv(LIBSSH2_SESSION * session,
                             unsigned char **signature,
                             size_t *signature_len,
                             int veccount,
                             const struct iovec datavec[],
                             void **abstract)
{
    libssh2_dsa_ctx *dsactx = (libssh2_dsa_ctx *) (*abstract);
    unsigned char hash[SHA_DIGEST_LENGTH];
    libssh2_sha1_ctx ctx;
    int i;

    *signature = LIBSSH2_ALLOC(session, 2 * SHA_DIGEST_LENGTH);
    if (!*signature) {
        return -1;
    }

    *signature_len = 2 * SHA_DIGEST_LENGTH;
    memset(*signature, 0, 2 * SHA_DIGEST_LENGTH);

    libssh2_sha1_init(&ctx);
    for(i = 0; i < veccount; i++) {
        libssh2_sha1_update(ctx, datavec[i].iov_base, datavec[i].iov_len);
    }
    libssh2_sha1_final(ctx, hash);

    if (_libssh2_dsa_sha1_sign(dsactx, hash, SHA_DIGEST_LENGTH, *signature)) {
        LIBSSH2_FREE(session, *signature);
        return -1;
    }

    return 0;
}

/*
 * libssh2_hostkey_method_ssh_dss_dtor
 *
 * Shutdown the hostkey method
 */
static int
hostkey_method_ssh_dss_dtor(LIBSSH2_SESSION * session, void **abstract)
{
    libssh2_dsa_ctx *dsactx = (libssh2_dsa_ctx *) (*abstract);
    (void) session;

    _libssh2_dsa_free(dsactx);

    *abstract = NULL;

    return 0;
}

static const LIBSSH2_HOSTKEY_METHOD hostkey_method_ssh_dss = {
    "ssh-dss",
    MD5_DIGEST_LENGTH,
    hostkey_method_ssh_dss_init,
    hostkey_method_ssh_dss_initPEM,
    hostkey_method_ssh_dss_sig_verify,
    hostkey_method_ssh_dss_signv,
    NULL,                       /* encrypt */
    hostkey_method_ssh_dss_dtor,
};
#endif /* LIBSSH2_DSA */

static const LIBSSH2_HOSTKEY_METHOD *hostkey_methods[] = {
#if LIBSSH2_RSA
    &hostkey_method_ssh_rsa,
#endif /* LIBSSH2_RSA */
#if LIBSSH2_DSA
    &hostkey_method_ssh_dss,
#endif /* LIBSSH2_DSA */
    NULL
};

const LIBSSH2_HOSTKEY_METHOD **
libssh2_hostkey_methods(void)
{
    return hostkey_methods;
}

/*
 * libssh2_hostkey_hash
 *
 * Returns hash signature
 * Returned buffer should NOT be freed
 * Length of buffer is determined by hash type
 * i.e. MD5 == 16, SHA1 == 20
 */
LIBSSH2_API const char *
libssh2_hostkey_hash(LIBSSH2_SESSION * session, int hash_type)
{
    switch (hash_type) {
#if LIBSSH2_MD5
    case LIBSSH2_HOSTKEY_HASH_MD5:
        return (session->server_hostkey_md5_valid)
          ? (char *) session->server_hostkey_md5
          : NULL;
        break;
#endif /* LIBSSH2_MD5 */
    case LIBSSH2_HOSTKEY_HASH_SHA1:
        return (char *) session->server_hostkey_sha1;
        break;
    default:
        return NULL;
    }
}

static int hostkey_type(const unsigned char *hostkey, size_t len)
{
    const unsigned char rsa[] = {
        0, 0, 0, 0x07, 's', 's', 'h', '-', 'r', 's', 'a'
    };
    const unsigned char dss[] = {
        0, 0, 0, 0x07, 's', 's', 'h', '-', 'd', 's', 's'
    };

    if (len < 11)
        return LIBSSH2_HOSTKEY_TYPE_UNKNOWN;

    if (!memcmp(rsa, hostkey, 11))
        return LIBSSH2_HOSTKEY_TYPE_RSA;

    if (!memcmp(dss, hostkey, 11))
        return LIBSSH2_HOSTKEY_TYPE_DSS;

    return LIBSSH2_HOSTKEY_TYPE_UNKNOWN;
}

/*
 * libssh2_session_hostkey()
 *
 * Returns the server key and length.
 *
 */
LIBSSH2_API const char *
libssh2_session_hostkey(LIBSSH2_SESSION *session, size_t *len, int *type)
{
    if(session->server_hostkey_len) {
        if(len)
            *len = session->server_hostkey_len;
        if (type)
            *type = hostkey_type(session->server_hostkey,
                                 session->server_hostkey_len);
        return (char *) session->server_hostkey;
    }
    if(len)
        *len = 0;
    return NULL;
}

