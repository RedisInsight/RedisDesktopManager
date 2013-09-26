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
#ifdef LIBSSH2_HAVE_ZLIB
# include <zlib.h>
#endif

#include "comp.h"

/* ********
 * none *
 ******** */

/*
 * comp_method_none_comp
 *
 * Minimalist compression: Absolutely none
 */
static int
comp_method_none_comp(LIBSSH2_SESSION *session,
                      unsigned char *dest,
                      size_t *dest_len,
                      const unsigned char *src,
                      size_t src_len,
                      void **abstract)
{
    (void) session;
    (void) abstract;
    (void) dest;
    (void) dest_len;
    (void) src;
    (void) src_len;

    return 0;
}

/*
 * comp_method_none_decomp
 *
 * Minimalist decompression: Absolutely none
 */
static int
comp_method_none_decomp(LIBSSH2_SESSION * session,
                        unsigned char **dest,
                        size_t *dest_len,
                        size_t payload_limit,
                        const unsigned char *src,
                        size_t src_len, void **abstract)
{
    (void) session;
    (void) payload_limit;
    (void) abstract;
    *dest = (unsigned char *) src;
    *dest_len = src_len;
    return 0;
}



static const LIBSSH2_COMP_METHOD comp_method_none = {
    "none",
    0, /* not really compressing */
    0, /* isn't used in userauth, go figure */
    NULL,
    comp_method_none_comp,
    comp_method_none_decomp,
    NULL
};

#ifdef LIBSSH2_HAVE_ZLIB
/* ********
 * zlib *
 ******** */

/* Memory management wrappers
 * Yes, I realize we're doing a callback to a callback,
 * Deal...
 */

static voidpf
comp_method_zlib_alloc(voidpf opaque, uInt items, uInt size)
{
    LIBSSH2_SESSION *session = (LIBSSH2_SESSION *) opaque;

    return (voidpf) LIBSSH2_ALLOC(session, items * size);
}

static void
comp_method_zlib_free(voidpf opaque, voidpf address)
{
    LIBSSH2_SESSION *session = (LIBSSH2_SESSION *) opaque;

    LIBSSH2_FREE(session, address);
}



/* libssh2_comp_method_zlib_init
 * All your bandwidth are belong to us (so save some)
 */
static int
comp_method_zlib_init(LIBSSH2_SESSION * session, int compr,
                      void **abstract)
{
    z_stream *strm;
    int status;

    strm = LIBSSH2_ALLOC(session, sizeof(z_stream));
    if (!strm) {
        return _libssh2_error(session, LIBSSH2_ERROR_ALLOC,
                              "Unable to allocate memory for "
                              "zlib compression/decompression");
    }
    memset(strm, 0, sizeof(z_stream));

    strm->opaque = (voidpf) session;
    strm->zalloc = (alloc_func) comp_method_zlib_alloc;
    strm->zfree = (free_func) comp_method_zlib_free;
    if (compr) {
        /* deflate */
        status = deflateInit(strm, Z_DEFAULT_COMPRESSION);
    } else {
        /* inflate */
        status = inflateInit(strm);
    }

    if (status != Z_OK) {
        LIBSSH2_FREE(session, strm);
        _libssh2_debug(session, LIBSSH2_TRACE_TRANS,
                       "unhandled zlib error %d", status);
        return LIBSSH2_ERROR_COMPRESS;
    }
    *abstract = strm;

    return LIBSSH2_ERROR_NONE;
}

/*
 * libssh2_comp_method_zlib_comp
 *
 * Compresses source to destination. Without allocation.
 */
static int
comp_method_zlib_comp(LIBSSH2_SESSION *session,
                      unsigned char *dest,

                      /* dest_len is a pointer to allow this function to
                         update it with the final actual size used */
                      size_t *dest_len,
                      const unsigned char *src,
                      size_t src_len,
                      void **abstract)
{
    z_stream *strm = *abstract;
    int out_maxlen = *dest_len;
    int status;

    strm->next_in = (unsigned char *) src;
    strm->avail_in = src_len;
    strm->next_out = dest;
    strm->avail_out = out_maxlen;

    status = deflate(strm, Z_PARTIAL_FLUSH);

    if (status != Z_OK) {
        _libssh2_debug(session, LIBSSH2_TRACE_TRANS,
                       "unhandled zlib compression error %d", status);
        return _libssh2_error(session, LIBSSH2_ERROR_ZLIB,
                              "compression failure");
    }

    *dest_len = out_maxlen - strm->avail_out;
    return 0;
}

/*
 * libssh2_comp_method_zlib_decomp
 *
 * Decompresses source to destination. Allocates the output memory.
 */
static int
comp_method_zlib_decomp(LIBSSH2_SESSION * session,
                        unsigned char **dest,
                        size_t *dest_len,
                        size_t payload_limit,
                        const unsigned char *src,
                        size_t src_len, void **abstract)
{
    z_stream *strm = *abstract;
    /* A short-term alloc of a full data chunk is better than a series of
       reallocs */
    char *out;
    int out_maxlen = 8 * src_len;
    int limiter = 0;

    /* If strm is null, then we have not yet been initialized. */
    if (strm == NULL)
        return _libssh2_error(session, LIBSSH2_ERROR_COMPRESS,
                              "decompression unitilized");;

    /* In practice they never come smaller than this */
    if (out_maxlen < 25)
        out_maxlen = 25;

    if (out_maxlen > (int) payload_limit)
        out_maxlen = payload_limit;

    strm->next_in = (unsigned char *) src;
    strm->avail_in = src_len;
    strm->next_out = (unsigned char *) LIBSSH2_ALLOC(session, out_maxlen);
    out = (char *) strm->next_out;
    strm->avail_out = out_maxlen;
    if (!strm->next_out)
        return _libssh2_error(session, LIBSSH2_ERROR_ALLOC,
                              "Unable to allocate decompression buffer");

    /* Loop until it's all inflated or hit error */
    for (;;) {
        int status, grow_size;
        size_t out_ofs;
        char *newout;

        status = inflate(strm, Z_PARTIAL_FLUSH);

        if (status == Z_OK) {
            if (! strm->avail_in) {
                /* status is OK and input all used so we're done */
                break;
            }
        } else if (status == Z_BUF_ERROR) {
            /* This is OK, just drop through to grow the buffer */
        } else {
            /* error state */
            LIBSSH2_FREE(session, out);
            _libssh2_debug(session, LIBSSH2_TRACE_TRANS,
                           "unhandled zlib error %d", status);
            return _libssh2_error(session, LIBSSH2_ERROR_ZLIB,
                                  "decompression failure");
        }

        /* If we get here we need to grow the output buffer and try again */
        out_ofs = out_maxlen - strm->avail_out;
        if (strm->avail_in) {
            grow_size = strm->avail_in * 8;
        } else {
            /* Not sure how much to grow by */
            grow_size = 32;
        }
        out_maxlen += grow_size;

        if ((out_maxlen > (int) payload_limit) && limiter++) {
            LIBSSH2_FREE(session, out);
            return _libssh2_error(session, LIBSSH2_ERROR_ZLIB,
                                  "Excessive growth in decompression phase");
        }

        newout = LIBSSH2_REALLOC(session, out, out_maxlen);
        if (!newout) {
            LIBSSH2_FREE(session, out);
            return _libssh2_error(session, LIBSSH2_ERROR_ALLOC,
                                  "Unable to expand decompression buffer");
        }
        out = newout;
        strm->next_out = (unsigned char *) out + out_ofs;
        strm->avail_out += grow_size;
    }

    *dest = (unsigned char *) out;
    *dest_len = out_maxlen - strm->avail_out;

    return 0;
}


/* libssh2_comp_method_zlib_dtor
 * All done, no more compression for you
 */
static int
comp_method_zlib_dtor(LIBSSH2_SESSION *session, int compr, void **abstract)
{
    z_stream *strm = *abstract;

    if (strm) {
        if (compr)
            deflateEnd(strm);
        else
            inflateEnd(strm);
        LIBSSH2_FREE(session, strm);
    }

    *abstract = NULL;
    return 0;
}

static const LIBSSH2_COMP_METHOD comp_method_zlib = {
    "zlib",
    1, /* yes, this compresses */
    1, /* do compression during userauth */
    comp_method_zlib_init,
    comp_method_zlib_comp,
    comp_method_zlib_decomp,
    comp_method_zlib_dtor,
};

static const LIBSSH2_COMP_METHOD comp_method_zlib_openssh = {
    "zlib@openssh.com",
    1, /* yes, this compresses */
    0, /* don't use compression during userauth */
    comp_method_zlib_init,
    comp_method_zlib_comp,
    comp_method_zlib_decomp,
    comp_method_zlib_dtor,
};
#endif /* LIBSSH2_HAVE_ZLIB */

/* If compression is enabled by the API, then this array is used which then
   may allow compression if zlib is available at build time */
static const LIBSSH2_COMP_METHOD *comp_methods[] = {
#ifdef LIBSSH2_HAVE_ZLIB
    &comp_method_zlib,
    &comp_method_zlib_openssh,
#endif /* LIBSSH2_HAVE_ZLIB */
    &comp_method_none,
    NULL
};

/* If compression is disabled by the API, then this array is used */
static const LIBSSH2_COMP_METHOD *no_comp_methods[] = {
    &comp_method_none,
    NULL
};

const LIBSSH2_COMP_METHOD **
_libssh2_comp_methods(LIBSSH2_SESSION *session)
{
    if(session->flag.compress)
        return comp_methods;
    else
        return no_comp_methods;
}
