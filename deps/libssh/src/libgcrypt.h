/*
 * Copyright (C) 2008, 2009, 2010 Simon Josefsson
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

#include <gcrypt.h>

#define LIBSSH2_MD5 1

#define LIBSSH2_HMAC_RIPEMD 1

#define LIBSSH2_AES 1
#define LIBSSH2_AES_CTR 1
#define LIBSSH2_BLOWFISH 1
#define LIBSSH2_RC4 1
#define LIBSSH2_CAST 1
#define LIBSSH2_3DES 1

#define LIBSSH2_RSA 1
#define LIBSSH2_DSA 1

#define MD5_DIGEST_LENGTH 16
#define SHA_DIGEST_LENGTH 20

#define _libssh2_random(buf, len)                \
  (gcry_randomize ((buf), (len), GCRY_STRONG_RANDOM), 1)

#define libssh2_sha1_ctx gcry_md_hd_t
#define libssh2_sha1_init(ctx) gcry_md_open (ctx,  GCRY_MD_SHA1, 0);
#define libssh2_sha1_update(ctx, data, len) gcry_md_write (ctx, data, len)
#define libssh2_sha1_final(ctx, out) \
  memcpy (out, gcry_md_read (ctx, 0), SHA_DIGEST_LENGTH), gcry_md_close (ctx)
#define libssh2_sha1(message, len, out) \
  gcry_md_hash_buffer (GCRY_MD_SHA1, out, message, len)

#define libssh2_md5_ctx gcry_md_hd_t

/* returns 0 in case of failure */
#define libssh2_md5_init(ctx) \
  (GPG_ERR_NO_ERROR == gcry_md_open (ctx,  GCRY_MD_MD5, 0))

#define libssh2_md5_update(ctx, data, len) gcry_md_write (ctx, data, len)
#define libssh2_md5_final(ctx, out) \
  memcpy (out, gcry_md_read (ctx, 0), MD5_DIGEST_LENGTH), gcry_md_close (ctx)
#define libssh2_md5(message, len, out) \
  gcry_md_hash_buffer (GCRY_MD_MD5, out, message, len)

#define libssh2_hmac_ctx gcry_md_hd_t
#define libssh2_hmac_sha1_init(ctx, key, keylen) \
  gcry_md_open (ctx, GCRY_MD_SHA1, GCRY_MD_FLAG_HMAC), \
    gcry_md_setkey (*ctx, key, keylen)
#define libssh2_hmac_md5_init(ctx, key, keylen) \
  gcry_md_open (ctx, GCRY_MD_MD5, GCRY_MD_FLAG_HMAC), \
    gcry_md_setkey (*ctx, key, keylen)
#define libssh2_hmac_ripemd160_init(ctx, key, keylen) \
  gcry_md_open (ctx, GCRY_MD_RMD160, GCRY_MD_FLAG_HMAC), \
    gcry_md_setkey (*ctx, key, keylen)
#define libssh2_hmac_update(ctx, data, datalen) \
  gcry_md_write (ctx, data, datalen)
#define libssh2_hmac_final(ctx, data) \
  memcpy (data, gcry_md_read (ctx, 0), \
      gcry_md_get_algo_dlen (gcry_md_get_algo (ctx)))
#define libssh2_hmac_cleanup(ctx) gcry_md_close (*ctx);

#define libssh2_crypto_init() gcry_control (GCRYCTL_DISABLE_SECMEM)
#define libssh2_crypto_exit()

#define libssh2_rsa_ctx struct gcry_sexp

#define _libssh2_rsa_free(rsactx)  gcry_sexp_release (rsactx)

#define libssh2_dsa_ctx struct gcry_sexp

#define _libssh2_dsa_free(dsactx)  gcry_sexp_release (dsactx)

#define _libssh2_cipher_type(name) int name
#define _libssh2_cipher_ctx gcry_cipher_hd_t

#define _libssh2_gcry_ciphermode(c,m) ((c << 8) | m)
#define _libssh2_gcry_cipher(c) (c >> 8)
#define _libssh2_gcry_mode(m) (m & 0xFF)

#define _libssh2_cipher_aes256ctr \
  _libssh2_gcry_ciphermode(GCRY_CIPHER_AES256, GCRY_CIPHER_MODE_CTR)
#define _libssh2_cipher_aes192ctr \
  _libssh2_gcry_ciphermode(GCRY_CIPHER_AES192, GCRY_CIPHER_MODE_CTR)
#define _libssh2_cipher_aes128ctr \
  _libssh2_gcry_ciphermode(GCRY_CIPHER_AES128, GCRY_CIPHER_MODE_CTR)
#define _libssh2_cipher_aes256 \
  _libssh2_gcry_ciphermode(GCRY_CIPHER_AES256, GCRY_CIPHER_MODE_CBC)
#define _libssh2_cipher_aes192 \
  _libssh2_gcry_ciphermode(GCRY_CIPHER_AES192, GCRY_CIPHER_MODE_CBC)
#define _libssh2_cipher_aes128 \
  _libssh2_gcry_ciphermode(GCRY_CIPHER_AES128, GCRY_CIPHER_MODE_CBC)
#define _libssh2_cipher_blowfish \
  _libssh2_gcry_ciphermode(GCRY_CIPHER_BLOWFISH, GCRY_CIPHER_MODE_CBC)
#define _libssh2_cipher_arcfour \
  _libssh2_gcry_ciphermode(GCRY_CIPHER_ARCFOUR, GCRY_CIPHER_MODE_STREAM)
#define _libssh2_cipher_cast5 \
  _libssh2_gcry_ciphermode(GCRY_CIPHER_CAST5, GCRY_CIPHER_MODE_CBC)
#define _libssh2_cipher_3des \
  _libssh2_gcry_ciphermode(GCRY_CIPHER_3DES, GCRY_CIPHER_MODE_CBC)


#define _libssh2_cipher_dtor(ctx) gcry_cipher_close(*(ctx))

#define _libssh2_bn struct gcry_mpi
#define _libssh2_bn_ctx int
#define _libssh2_bn_ctx_new() 0
#define _libssh2_bn_ctx_free(bnctx) ((void)0)
#define _libssh2_bn_init() gcry_mpi_new(0)
#define _libssh2_bn_rand(bn, bits, top, bottom) gcry_mpi_randomize (bn, bits, GCRY_WEAK_RANDOM)
#define _libssh2_bn_mod_exp(r, a, p, m, ctx) gcry_mpi_powm (r, a, p, m)
#define _libssh2_bn_set_word(bn, val) gcry_mpi_set_ui(bn, val)
#define _libssh2_bn_from_bin(bn, len, val) gcry_mpi_scan(&((bn)), GCRYMPI_FMT_USG, val, len, NULL)
#define _libssh2_bn_to_bin(bn, val) gcry_mpi_print (GCRYMPI_FMT_USG, val, _libssh2_bn_bytes(bn), NULL, bn)
#define _libssh2_bn_bytes(bn) (gcry_mpi_get_nbits (bn) / 8 + ((gcry_mpi_get_nbits (bn) % 8 == 0) ? 0 : 1))
#define _libssh2_bn_bits(bn) gcry_mpi_get_nbits (bn)
#define _libssh2_bn_free(bn) gcry_mpi_release(bn)

