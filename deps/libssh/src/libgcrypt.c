/* Copyright (C) 2008, 2009, Simon Josefsson
 * Copyright (C) 2006, 2007, The Written Word, Inc.
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

#ifdef LIBSSH2_LIBGCRYPT /* compile only if we build with libgcrypt */

#include <string.h>

int
_libssh2_rsa_new(libssh2_rsa_ctx ** rsa,
                 const unsigned char *edata,
                 unsigned long elen,
                 const unsigned char *ndata,
                 unsigned long nlen,
                 const unsigned char *ddata,
                 unsigned long dlen,
                 const unsigned char *pdata,
                 unsigned long plen,
                 const unsigned char *qdata,
                 unsigned long qlen,
                 const unsigned char *e1data,
                 unsigned long e1len,
                 const unsigned char *e2data,
                 unsigned long e2len,
                 const unsigned char *coeffdata, unsigned long coefflen)
{
    int rc;
    (void) e1data;
    (void) e1len;
    (void) e2data;
    (void) e2len;

    if (ddata) {
        rc = gcry_sexp_build
            (rsa, NULL,
             "(private-key(rsa(n%b)(e%b)(d%b)(q%b)(p%b)(u%b)))",
             nlen, ndata, elen, edata, dlen, ddata, plen, pdata,
             qlen, qdata, coefflen, coeffdata);
    } else {
        rc = gcry_sexp_build(rsa, NULL, "(public-key(rsa(n%b)(e%b)))",
                             nlen, ndata, elen, edata);
    }
    if (rc) {
        *rsa = NULL;
        return -1;
    }

    return 0;
}

int
_libssh2_rsa_sha1_verify(libssh2_rsa_ctx * rsa,
                         const unsigned char *sig,
                         unsigned long sig_len,
                         const unsigned char *m, unsigned long m_len)
{
    unsigned char hash[SHA_DIGEST_LENGTH];
    gcry_sexp_t s_sig, s_hash;
    int rc = -1;

    libssh2_sha1(m, m_len, hash);

    rc = gcry_sexp_build(&s_hash, NULL,
                         "(data (flags pkcs1) (hash sha1 %b))",
                         SHA_DIGEST_LENGTH, hash);
    if (rc != 0) {
        return -1;
    }

    rc = gcry_sexp_build(&s_sig, NULL, "(sig-val(rsa(s %b)))", sig_len, sig);
    if (rc != 0) {
        gcry_sexp_release(s_hash);
        return -1;
    }

    rc = gcry_pk_verify(s_sig, s_hash, rsa);
    gcry_sexp_release(s_sig);
    gcry_sexp_release(s_hash);

    return (rc == 0) ? 0 : -1;
}

int
_libssh2_dsa_new(libssh2_dsa_ctx ** dsactx,
                 const unsigned char *p,
                 unsigned long p_len,
                 const unsigned char *q,
                 unsigned long q_len,
                 const unsigned char *g,
                 unsigned long g_len,
                 const unsigned char *y,
                 unsigned long y_len,
                 const unsigned char *x, unsigned long x_len)
{
    int rc;

    if (x_len) {
        rc = gcry_sexp_build
            (dsactx, NULL,
             "(private-key(dsa(p%b)(q%b)(g%b)(y%b)(x%b)))",
             p_len, p, q_len, q, g_len, g, y_len, y, x_len, x);
    } else {
        rc = gcry_sexp_build(dsactx, NULL,
                             "(public-key(dsa(p%b)(q%b)(g%b)(y%b)))",
                             p_len, p, q_len, q, g_len, g, y_len, y);
    }

    if (rc) {
        *dsactx = NULL;
        return -1;
    }

    return 0;
}

int
_libssh2_rsa_new_private(libssh2_rsa_ctx ** rsa,
                         LIBSSH2_SESSION * session,
                         const char *filename, unsigned const char *passphrase)
{
    FILE *fp;
    unsigned char *data, *save_data;
    unsigned int datalen;
    int ret;
    unsigned char *n, *e, *d, *p, *q, *e1, *e2, *coeff;
    unsigned int nlen, elen, dlen, plen, qlen, e1len, e2len, coefflen;

    (void) passphrase;

    fp = fopen(filename, "r");
    if (!fp) {
        return -1;
    }

    ret = _libssh2_pem_parse(session,
                             "-----BEGIN RSA PRIVATE KEY-----",
                             "-----END RSA PRIVATE KEY-----",
                             fp, &data, &datalen);
    fclose(fp);
    if (ret) {
        return -1;
    }

    save_data = data;

    if (_libssh2_pem_decode_sequence(&data, &datalen)) {
        ret = -1;
        goto fail;
    }
/* First read Version field (should be 0). */
    ret = _libssh2_pem_decode_integer(&data, &datalen, &n, &nlen);
    if (ret != 0 || (nlen != 1 && *n != '\0')) {
        ret = -1;
        goto fail;
    }

    ret = _libssh2_pem_decode_integer(&data, &datalen, &n, &nlen);
    if (ret != 0) {
        ret = -1;
        goto fail;
    }

    ret = _libssh2_pem_decode_integer(&data, &datalen, &e, &elen);
    if (ret != 0) {
        ret = -1;
        goto fail;
    }

    ret = _libssh2_pem_decode_integer(&data, &datalen, &d, &dlen);
    if (ret != 0) {
        ret = -1;
        goto fail;
    }

    ret = _libssh2_pem_decode_integer(&data, &datalen, &p, &plen);
    if (ret != 0) {
        ret = -1;
        goto fail;
    }

    ret = _libssh2_pem_decode_integer(&data, &datalen, &q, &qlen);
    if (ret != 0) {
        ret = -1;
        goto fail;
    }

    ret = _libssh2_pem_decode_integer(&data, &datalen, &e1, &e1len);
    if (ret != 0) {
        ret = -1;
        goto fail;
    }

    ret = _libssh2_pem_decode_integer(&data, &datalen, &e2, &e2len);
    if (ret != 0) {
        ret = -1;
        goto fail;
    }

    ret = _libssh2_pem_decode_integer(&data, &datalen, &coeff, &coefflen);
    if (ret != 0) {
        ret = -1;
        goto fail;
    }

    if (_libssh2_rsa_new(rsa, e, elen, n, nlen, d, dlen, p, plen,
                         q, qlen, e1, e1len, e2, e2len, coeff, coefflen)) {
        ret = -1;
        goto fail;
    }

    ret = 0;

  fail:
    LIBSSH2_FREE(session, save_data);
    return ret;
}

int
_libssh2_dsa_new_private(libssh2_dsa_ctx ** dsa,
                         LIBSSH2_SESSION * session,
                         const char *filename, unsigned const char *passphrase)
{
    FILE *fp;
    unsigned char *data, *save_data;
    unsigned int datalen;
    int ret;
    unsigned char *p, *q, *g, *y, *x;
    unsigned int plen, qlen, glen, ylen, xlen;

    (void) passphrase;

    fp = fopen(filename, "r");
    if (!fp) {
        return -1;
    }

    ret = _libssh2_pem_parse(session,
                             "-----BEGIN DSA PRIVATE KEY-----",
                             "-----END DSA PRIVATE KEY-----",
                             fp, &data, &datalen);
    fclose(fp);
    if (ret) {
        return -1;
    }

    save_data = data;

    if (_libssh2_pem_decode_sequence(&data, &datalen)) {
        ret = -1;
        goto fail;
    }

/* First read Version field (should be 0). */
    ret = _libssh2_pem_decode_integer(&data, &datalen, &p, &plen);
    if (ret != 0 || (plen != 1 && *p != '\0')) {
        ret = -1;
        goto fail;
    }

    ret = _libssh2_pem_decode_integer(&data, &datalen, &p, &plen);
    if (ret != 0) {
        ret = -1;
        goto fail;
    }

    ret = _libssh2_pem_decode_integer(&data, &datalen, &q, &qlen);
    if (ret != 0) {
        ret = -1;
        goto fail;
    }

    ret = _libssh2_pem_decode_integer(&data, &datalen, &g, &glen);
    if (ret != 0) {
        ret = -1;
        goto fail;
    }

    ret = _libssh2_pem_decode_integer(&data, &datalen, &y, &ylen);
    if (ret != 0) {
        ret = -1;
        goto fail;
    }

    ret = _libssh2_pem_decode_integer(&data, &datalen, &x, &xlen);
    if (ret != 0) {
        ret = -1;
        goto fail;
    }

    if (datalen != 0) {
        ret = -1;
        goto fail;
    }

    if (_libssh2_dsa_new(dsa, p, plen, q, qlen, g, glen, y, ylen, x, xlen)) {
        ret = -1;
        goto fail;
    }

    ret = 0;

  fail:
    LIBSSH2_FREE(session, save_data);
    return ret;
}

int
_libssh2_rsa_sha1_sign(LIBSSH2_SESSION * session,
                       libssh2_dsa_ctx * rsactx,
                       const unsigned char *hash,
                       size_t hash_len,
                       unsigned char **signature, size_t *signature_len)
{
    gcry_sexp_t sig_sexp;
    gcry_sexp_t data;
    int rc;
    const char *tmp;
    size_t size;

    if (hash_len != SHA_DIGEST_LENGTH) {
        return -1;
    }

    if (gcry_sexp_build(&data, NULL,
                        "(data (flags pkcs1) (hash sha1 %b))",
                        hash_len, hash)) {
        return -1;
    }

    rc = gcry_pk_sign(&sig_sexp, data, rsactx);

    gcry_sexp_release(data);

    if (rc != 0) {
        return -1;
    }

    data = gcry_sexp_find_token(sig_sexp, "s", 0);
    if (!data) {
        return -1;
    }

    tmp = gcry_sexp_nth_data(data, 1, &size);
    if (!tmp) {
        return -1;
    }

    if (tmp[0] == '\0') {
        tmp++;
        size--;
    }

    *signature = LIBSSH2_ALLOC(session, size);
    memcpy(*signature, tmp, size);
    *signature_len = size;

    return rc;
}

int
_libssh2_dsa_sha1_sign(libssh2_dsa_ctx * dsactx,
                       const unsigned char *hash,
                       unsigned long hash_len, unsigned char *sig)
{
    unsigned char zhash[SHA_DIGEST_LENGTH + 1];
    gcry_sexp_t sig_sexp;
    gcry_sexp_t data;
    int ret;
    const char *tmp;
    size_t size;

    if (hash_len != SHA_DIGEST_LENGTH) {
        return -1;
    }

    memcpy(zhash + 1, hash, hash_len);
    zhash[0] = 0;

    if (gcry_sexp_build(&data, NULL, "(data (value %b))", hash_len + 1, zhash)) {
        return -1;
    }

    ret = gcry_pk_sign(&sig_sexp, data, dsactx);

    gcry_sexp_release(data);

    if (ret != 0) {
        return -1;
    }

    memset(sig, 0, 40);

/* Extract R. */

    data = gcry_sexp_find_token(sig_sexp, "r", 0);
    if (!data)
        goto err;

    tmp = gcry_sexp_nth_data(data, 1, &size);
    if (!tmp)
        goto err;

    if (tmp[0] == '\0') {
        tmp++;
        size--;
    }

    if (size < 1 || size > 20)
        goto err;

    memcpy(sig + (20 - size), tmp, size);

    gcry_sexp_release(data);

/* Extract S. */

    data = gcry_sexp_find_token(sig_sexp, "s", 0);
    if (!data)
        goto err;

    tmp = gcry_sexp_nth_data(data, 1, &size);
    if (!tmp)
        goto err;

    if (tmp[0] == '\0') {
        tmp++;
        size--;
    }

    if (size < 1 || size > 20)
        goto err;

    memcpy(sig + 20 + (20 - size), tmp, size);
    goto out;

  err:
    ret = -1;

  out:
    if (sig_sexp) {
        gcry_sexp_release(sig_sexp);
    }
    if (data) {
        gcry_sexp_release(data);
    }
    return ret;
}

int
_libssh2_dsa_sha1_verify(libssh2_dsa_ctx * dsactx,
                         const unsigned char *sig,
                         const unsigned char *m, unsigned long m_len)
{
    unsigned char hash[SHA_DIGEST_LENGTH + 1];
    gcry_sexp_t s_sig, s_hash;
    int rc = -1;

    libssh2_sha1(m, m_len, hash + 1);
    hash[0] = 0;

    if (gcry_sexp_build(&s_hash, NULL, "(data(flags raw)(value %b))",
                        SHA_DIGEST_LENGTH + 1, hash)) {
        return -1;
    }

    if (gcry_sexp_build(&s_sig, NULL, "(sig-val(dsa(r %b)(s %b)))",
                        20, sig, 20, sig + 20)) {
        gcry_sexp_release(s_hash);
        return -1;
    }

    rc = gcry_pk_verify(s_sig, s_hash, dsactx);
    gcry_sexp_release(s_sig);
    gcry_sexp_release(s_hash);

    return (rc == 0) ? 0 : -1;
}

int
_libssh2_cipher_init(_libssh2_cipher_ctx * h,
                     _libssh2_cipher_type(algo),
                     unsigned char *iv, unsigned char *secret, int encrypt)
{
    int ret;
    int cipher = _libssh2_gcry_cipher (algo);
    int mode = _libssh2_gcry_mode (algo);
    int keylen = gcry_cipher_get_algo_keylen(cipher);

    (void) encrypt;

    ret = gcry_cipher_open(h, cipher, mode, 0);
    if (ret) {
        return -1;
    }

    ret = gcry_cipher_setkey(*h, secret, keylen);
    if (ret) {
        gcry_cipher_close(*h);
        return -1;
    }

    if (mode != GCRY_CIPHER_MODE_STREAM) {
        int blklen = gcry_cipher_get_algo_blklen(cipher);
        if (mode == GCRY_CIPHER_MODE_CTR)
            ret = gcry_cipher_setctr(*h, iv, blklen);
        else
            ret = gcry_cipher_setiv(*h, iv, blklen);
        if (ret) {
            gcry_cipher_close(*h);
            return -1;
        }
    }

    return 0;
}

int
_libssh2_cipher_crypt(_libssh2_cipher_ctx * ctx,
                      _libssh2_cipher_type(algo),
                      int encrypt, unsigned char *block, size_t blklen)
{
    int cipher = _libssh2_gcry_cipher (algo);
    int ret;

    if (encrypt) {
        ret = gcry_cipher_encrypt(*ctx, block, blklen, block, blklen);
    } else {
        ret = gcry_cipher_decrypt(*ctx, block, blklen, block, blklen);
    }
    return ret;
}

int
_libssh2_pub_priv_keyfile(LIBSSH2_SESSION *session,
                          unsigned char **method,
                          size_t *method_len,
                          unsigned char **pubkeydata,
                          size_t *pubkeydata_len,
                          const char *privatekey,
                          const char *passphrase)
{
    return _libssh2_error(session, LIBSSH2_ERROR_FILE,
                         "Unable to extract public key from private key file: "
                         "Method unimplemented in libgcrypt backend");
}

void _libssh2_init_aes_ctr(void)
{
    /* no implementation */
}
#endif /* LIBSSH2_LIBGCRYPT */
