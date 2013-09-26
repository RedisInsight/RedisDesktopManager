/* Copyright (c) 2004-2007, Sara Golemon <sarag@libssh2.org>
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
#include "mac.h"

#ifdef LIBSSH2_MAC_NONE
/* mac_none_MAC
 * Minimalist MAC: No MAC
 */
static int
mac_none_MAC(LIBSSH2_SESSION * session, unsigned char *buf,
             uint32_t seqno, const unsigned char *packet,
             uint32_t packet_len, const unsigned char *addtl,
             uint32_t addtl_len, void **abstract)
{
    return 0;
}




static LIBSSH2_MAC_METHOD mac_method_none = {
    "none",
    0,
    0,
    NULL,
    mac_none_MAC,
    NULL
};
#endif /* LIBSSH2_MAC_NONE */

/* mac_method_common_init
 * Initialize simple mac methods
 */
static int
mac_method_common_init(LIBSSH2_SESSION * session, unsigned char *key,
                       int *free_key, void **abstract)
{
    *abstract = key;
    *free_key = 0;
    (void) session;

    return 0;
}



/* mac_method_common_dtor
 * Cleanup simple mac methods
 */
static int
mac_method_common_dtor(LIBSSH2_SESSION * session, void **abstract)
{
    if (*abstract) {
        LIBSSH2_FREE(session, *abstract);
    }
    *abstract = NULL;

    return 0;
}



/* mac_method_hmac_sha1_hash
 * Calculate hash using full sha1 value
 */
static int
mac_method_hmac_sha1_hash(LIBSSH2_SESSION * session,
                          unsigned char *buf, uint32_t seqno,
                          const unsigned char *packet,
                          uint32_t packet_len,
                          const unsigned char *addtl,
                          uint32_t addtl_len, void **abstract)
{
    libssh2_hmac_ctx ctx;
    unsigned char seqno_buf[4];
    (void) session;

    _libssh2_htonu32(seqno_buf, seqno);

    libssh2_hmac_sha1_init(&ctx, *abstract, 20);
    libssh2_hmac_update(ctx, seqno_buf, 4);
    libssh2_hmac_update(ctx, packet, packet_len);
    if (addtl && addtl_len) {
        libssh2_hmac_update(ctx, addtl, addtl_len);
    }
    libssh2_hmac_final(ctx, buf);
    libssh2_hmac_cleanup(&ctx);

    return 0;
}



static const LIBSSH2_MAC_METHOD mac_method_hmac_sha1 = {
    "hmac-sha1",
    20,
    20,
    mac_method_common_init,
    mac_method_hmac_sha1_hash,
    mac_method_common_dtor,
};

/* mac_method_hmac_sha1_96_hash
 * Calculate hash using first 96 bits of sha1 value
 */
static int
mac_method_hmac_sha1_96_hash(LIBSSH2_SESSION * session,
                             unsigned char *buf, uint32_t seqno,
                             const unsigned char *packet,
                             uint32_t packet_len,
                             const unsigned char *addtl,
                             uint32_t addtl_len, void **abstract)
{
    unsigned char temp[SHA_DIGEST_LENGTH];

    mac_method_hmac_sha1_hash(session, temp, seqno, packet, packet_len,
                              addtl, addtl_len, abstract);
    memcpy(buf, (char *) temp, 96 / 8);

    return 0;
}



static const LIBSSH2_MAC_METHOD mac_method_hmac_sha1_96 = {
    "hmac-sha1-96",
    12,
    20,
    mac_method_common_init,
    mac_method_hmac_sha1_96_hash,
    mac_method_common_dtor,
};

#if LIBSSH2_MD5
/* mac_method_hmac_md5_hash
 * Calculate hash using full md5 value
 */
static int
mac_method_hmac_md5_hash(LIBSSH2_SESSION * session, unsigned char *buf,
                         uint32_t seqno,
                         const unsigned char *packet,
                         uint32_t packet_len,
                         const unsigned char *addtl,
                         uint32_t addtl_len, void **abstract)
{
    libssh2_hmac_ctx ctx;
    unsigned char seqno_buf[4];
    (void) session;

    _libssh2_htonu32(seqno_buf, seqno);

    libssh2_hmac_md5_init(&ctx, *abstract, 16);
    libssh2_hmac_update(ctx, seqno_buf, 4);
    libssh2_hmac_update(ctx, packet, packet_len);
    if (addtl && addtl_len) {
        libssh2_hmac_update(ctx, addtl, addtl_len);
    }
    libssh2_hmac_final(ctx, buf);
    libssh2_hmac_cleanup(&ctx);

    return 0;
}



static const LIBSSH2_MAC_METHOD mac_method_hmac_md5 = {
    "hmac-md5",
    16,
    16,
    mac_method_common_init,
    mac_method_hmac_md5_hash,
    mac_method_common_dtor,
};

/* mac_method_hmac_md5_96_hash
 * Calculate hash using first 96 bits of md5 value
 */
static int
mac_method_hmac_md5_96_hash(LIBSSH2_SESSION * session,
                            unsigned char *buf, uint32_t seqno,
                            const unsigned char *packet,
                            uint32_t packet_len,
                            const unsigned char *addtl,
                            uint32_t addtl_len, void **abstract)
{
    unsigned char temp[MD5_DIGEST_LENGTH];
    mac_method_hmac_md5_hash(session, temp, seqno, packet, packet_len,
                             addtl, addtl_len, abstract);
    memcpy(buf, (char *) temp, 96 / 8);
    return 0;
}



static const LIBSSH2_MAC_METHOD mac_method_hmac_md5_96 = {
    "hmac-md5-96",
    12,
    16,
    mac_method_common_init,
    mac_method_hmac_md5_96_hash,
    mac_method_common_dtor,
};
#endif /* LIBSSH2_MD5 */

#if LIBSSH2_HMAC_RIPEMD
/* mac_method_hmac_ripemd160_hash
 * Calculate hash using ripemd160 value
 */
static int
mac_method_hmac_ripemd160_hash(LIBSSH2_SESSION * session,
                               unsigned char *buf, uint32_t seqno,
                               const unsigned char *packet,
                               uint32_t packet_len,
                               const unsigned char *addtl,
                               uint32_t addtl_len,
                               void **abstract)
{
    libssh2_hmac_ctx ctx;
    unsigned char seqno_buf[4];
    (void) session;

    _libssh2_htonu32(seqno_buf, seqno);

    libssh2_hmac_ripemd160_init(&ctx, *abstract, 20);
    libssh2_hmac_update(ctx, seqno_buf, 4);
    libssh2_hmac_update(ctx, packet, packet_len);
    if (addtl && addtl_len) {
        libssh2_hmac_update(ctx, addtl, addtl_len);
    }
    libssh2_hmac_final(ctx, buf);
    libssh2_hmac_cleanup(&ctx);

    return 0;
}



static const LIBSSH2_MAC_METHOD mac_method_hmac_ripemd160 = {
    "hmac-ripemd160",
    20,
    20,
    mac_method_common_init,
    mac_method_hmac_ripemd160_hash,
    mac_method_common_dtor,
};

static const LIBSSH2_MAC_METHOD mac_method_hmac_ripemd160_openssh_com = {
    "hmac-ripemd160@openssh.com",
    20,
    20,
    mac_method_common_init,
    mac_method_hmac_ripemd160_hash,
    mac_method_common_dtor,
};
#endif /* LIBSSH2_HMAC_RIPEMD */

static const LIBSSH2_MAC_METHOD *mac_methods[] = {
    &mac_method_hmac_sha1,
    &mac_method_hmac_sha1_96,
#if LIBSSH2_MD5
    &mac_method_hmac_md5,
    &mac_method_hmac_md5_96,
#endif
#if LIBSSH2_HMAC_RIPEMD
    &mac_method_hmac_ripemd160,
    &mac_method_hmac_ripemd160_openssh_com,
#endif /* LIBSSH2_HMAC_RIPEMD */
#ifdef LIBSSH2_MAC_NONE
    &mac_method_none,
#endif /* LIBSSH2_MAC_NONE */
    NULL
};

const LIBSSH2_MAC_METHOD **
_libssh2_mac_methods(void)
{
    return mac_methods;
}
