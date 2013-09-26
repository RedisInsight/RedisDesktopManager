/* Copyright (c) 2009, 2010 Simon Josefsson <simon@josefsson.org>
 * Copyright (c) 2004-2007, Sara Golemon <sarag@libssh2.org>
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

#ifdef LIBSSH2_CRYPT_NONE

/* crypt_none_crypt
 * Minimalist cipher: VERY secure *wink*
 */
static int
crypt_none_crypt(LIBSSH2_SESSION * session, unsigned char *buf,
                         void **abstract)
{
    /* Do nothing to the data! */
    return 0;
}

static const LIBSSH2_CRYPT_METHOD libssh2_crypt_method_none = {
    "none",
    8,                     /* blocksize (SSH2 defines minimum blocksize as 8) */
    0,                     /* iv_len */
    0,                     /* secret_len */
    0,                     /* flags */
    NULL,
    crypt_none_crypt,
    NULL
};
#endif /* LIBSSH2_CRYPT_NONE */

struct crypt_ctx
{
    int encrypt;
    _libssh2_cipher_type(algo);
    _libssh2_cipher_ctx h;
};

static int
crypt_init(LIBSSH2_SESSION * session,
           const LIBSSH2_CRYPT_METHOD * method,
           unsigned char *iv, int *free_iv,
           unsigned char *secret, int *free_secret,
           int encrypt, void **abstract)
{
    struct crypt_ctx *ctx = LIBSSH2_ALLOC(session,
                                          sizeof(struct crypt_ctx));
    if (!ctx)
        return LIBSSH2_ERROR_ALLOC;

    ctx->encrypt = encrypt;
    ctx->algo = method->algo;
    if (_libssh2_cipher_init(&ctx->h, ctx->algo, iv, secret, encrypt)) {
        LIBSSH2_FREE(session, ctx);
        return -1;
    }
    *abstract = ctx;
    *free_iv = 1;
    *free_secret = 1;
    return 0;
}

static int
crypt_encrypt(LIBSSH2_SESSION * session, unsigned char *block,
              size_t blocksize, void **abstract)
{
    struct crypt_ctx *cctx = *(struct crypt_ctx **) abstract;
    (void) session;
    return _libssh2_cipher_crypt(&cctx->h, cctx->algo, cctx->encrypt, block,
                                 blocksize);
}

static int
crypt_dtor(LIBSSH2_SESSION * session, void **abstract)
{
    struct crypt_ctx **cctx = (struct crypt_ctx **) abstract;
    if (cctx && *cctx) {
        _libssh2_cipher_dtor(&(*cctx)->h);
        LIBSSH2_FREE(session, *cctx);
        *abstract = NULL;
    }
    return 0;
}

#if LIBSSH2_AES_CTR
static const LIBSSH2_CRYPT_METHOD libssh2_crypt_method_aes128_ctr = {
    "aes128-ctr",
    16,                         /* blocksize */
    16,                         /* initial value length */
    16,                         /* secret length -- 16*8 == 128bit */
    0,                          /* flags */
    &crypt_init,
    &crypt_encrypt,
    &crypt_dtor,
    _libssh2_cipher_aes128ctr
};

static const LIBSSH2_CRYPT_METHOD libssh2_crypt_method_aes192_ctr = {
    "aes192-ctr",
    16,                         /* blocksize */
    16,                         /* initial value length */
    24,                         /* secret length -- 24*8 == 192bit */
    0,                          /* flags */
    &crypt_init,
    &crypt_encrypt,
    &crypt_dtor,
    _libssh2_cipher_aes192ctr
};

static const LIBSSH2_CRYPT_METHOD libssh2_crypt_method_aes256_ctr = {
    "aes256-ctr",
    16,                         /* blocksize */
    16,                         /* initial value length */
    32,                         /* secret length -- 32*8 == 256bit */
    0,                          /* flags */
    &crypt_init,
    &crypt_encrypt,
    &crypt_dtor,
    _libssh2_cipher_aes256ctr
};
#endif

#if LIBSSH2_AES
static const LIBSSH2_CRYPT_METHOD libssh2_crypt_method_aes128_cbc = {
    "aes128-cbc",
    16,                         /* blocksize */
    16,                         /* initial value length */
    16,                         /* secret length -- 16*8 == 128bit */
    0,                          /* flags */
    &crypt_init,
    &crypt_encrypt,
    &crypt_dtor,
    _libssh2_cipher_aes128
};

static const LIBSSH2_CRYPT_METHOD libssh2_crypt_method_aes192_cbc = {
    "aes192-cbc",
    16,                         /* blocksize */
    16,                         /* initial value length */
    24,                         /* secret length -- 24*8 == 192bit */
    0,                          /* flags */
    &crypt_init,
    &crypt_encrypt,
    &crypt_dtor,
    _libssh2_cipher_aes192
};

static const LIBSSH2_CRYPT_METHOD libssh2_crypt_method_aes256_cbc = {
    "aes256-cbc",
    16,                         /* blocksize */
    16,                         /* initial value length */
    32,                         /* secret length -- 32*8 == 256bit */
    0,                          /* flags */
    &crypt_init,
    &crypt_encrypt,
    &crypt_dtor,
    _libssh2_cipher_aes256
};

/* rijndael-cbc@lysator.liu.se == aes256-cbc */
static const LIBSSH2_CRYPT_METHOD
    libssh2_crypt_method_rijndael_cbc_lysator_liu_se = {
    "rijndael-cbc@lysator.liu.se",
    16,                         /* blocksize */
    16,                         /* initial value length */
    32,                         /* secret length -- 32*8 == 256bit */
    0,                          /* flags */
    &crypt_init,
    &crypt_encrypt,
    &crypt_dtor,
    _libssh2_cipher_aes256
};
#endif /* LIBSSH2_AES */

#if LIBSSH2_BLOWFISH
static const LIBSSH2_CRYPT_METHOD libssh2_crypt_method_blowfish_cbc = {
    "blowfish-cbc",
    8,                          /* blocksize */
    8,                          /* initial value length */
    16,                         /* secret length */
    0,                          /* flags */
    &crypt_init,
    &crypt_encrypt,
    &crypt_dtor,
    _libssh2_cipher_blowfish
};
#endif /* LIBSSH2_BLOWFISH */

#if LIBSSH2_RC4
static const LIBSSH2_CRYPT_METHOD libssh2_crypt_method_arcfour = {
    "arcfour",
    8,                          /* blocksize */
    8,                          /* initial value length */
    16,                         /* secret length */
    0,                          /* flags */
    &crypt_init,
    &crypt_encrypt,
    &crypt_dtor,
    _libssh2_cipher_arcfour
};

static int
crypt_init_arcfour128(LIBSSH2_SESSION * session,
                      const LIBSSH2_CRYPT_METHOD * method,
                      unsigned char *iv, int *free_iv,
                      unsigned char *secret, int *free_secret,
                      int encrypt, void **abstract)
{
    int rc;

    rc = crypt_init (session, method, iv, free_iv, secret, free_secret,
                     encrypt, abstract);
    if (rc == 0) {
        struct crypt_ctx *cctx = *(struct crypt_ctx **) abstract;
        unsigned char block[8];
        size_t discard = 1536;
        for (; discard; discard -= 8)
            _libssh2_cipher_crypt(&cctx->h, cctx->algo, cctx->encrypt, block,
                                  method->blocksize);
    }

    return rc;
}

static const LIBSSH2_CRYPT_METHOD libssh2_crypt_method_arcfour128 = {
    "arcfour128",
    8,                          /* blocksize */
    8,                          /* initial value length */
    16,                         /* secret length */
    0,                          /* flags */
    &crypt_init_arcfour128,
    &crypt_encrypt,
    &crypt_dtor,
    _libssh2_cipher_arcfour
};
#endif /* LIBSSH2_RC4 */

#if LIBSSH2_CAST
static const LIBSSH2_CRYPT_METHOD libssh2_crypt_method_cast128_cbc = {
    "cast128-cbc",
    8,                          /* blocksize */
    8,                          /* initial value length */
    16,                         /* secret length */
    0,                          /* flags */
    &crypt_init,
    &crypt_encrypt,
    &crypt_dtor,
    _libssh2_cipher_cast5
};
#endif /* LIBSSH2_CAST */

#if LIBSSH2_3DES
static const LIBSSH2_CRYPT_METHOD libssh2_crypt_method_3des_cbc = {
    "3des-cbc",
    8,                          /* blocksize */
    8,                          /* initial value length */
    24,                         /* secret length */
    0,                          /* flags */
    &crypt_init,
    &crypt_encrypt,
    &crypt_dtor,
    _libssh2_cipher_3des
};
#endif

static const LIBSSH2_CRYPT_METHOD *_libssh2_crypt_methods[] = {
#if LIBSSH2_AES_CTR
  &libssh2_crypt_method_aes128_ctr,
  &libssh2_crypt_method_aes192_ctr,
  &libssh2_crypt_method_aes256_ctr,
#endif /* LIBSSH2_AES */
#if LIBSSH2_AES
    &libssh2_crypt_method_aes256_cbc,
    &libssh2_crypt_method_rijndael_cbc_lysator_liu_se,  /* == aes256-cbc */
    &libssh2_crypt_method_aes192_cbc,
    &libssh2_crypt_method_aes128_cbc,
#endif /* LIBSSH2_AES */
#if LIBSSH2_BLOWFISH
    &libssh2_crypt_method_blowfish_cbc,
#endif /* LIBSSH2_BLOWFISH */
#if LIBSSH2_RC4
    &libssh2_crypt_method_arcfour128,
    &libssh2_crypt_method_arcfour,
#endif /* LIBSSH2_RC4 */
#if LIBSSH2_CAST
    &libssh2_crypt_method_cast128_cbc,
#endif /* LIBSSH2_CAST */
#if LIBSSH2_3DES
    &libssh2_crypt_method_3des_cbc,
#endif /*  LIBSSH2_DES */
#ifdef LIBSSH2_CRYPT_NONE
    &libssh2_crypt_method_none,
#endif
    NULL
};

/* Expose to kex.c */
const LIBSSH2_CRYPT_METHOD **
libssh2_crypt_methods(void)
{
    return _libssh2_crypt_methods;
}
