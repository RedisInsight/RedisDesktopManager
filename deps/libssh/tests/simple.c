/* Copyright (C) 2007 The Written Word, Inc.
 * Copyright (C) 2008, 2010 Simon Josefsson
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

#include <stdio.h>
#include <stdlib.h>

#include "libssh2.h"

static int test_libssh2_base64_decode (LIBSSH2_SESSION *session)
{
    char *data;
    unsigned int datalen;
    const char *src = "Zm5vcmQ=";
    unsigned int src_len = strlen (src);
    int ret;

    ret = libssh2_base64_decode(session, &data, &datalen,
                                src, src_len);
    if (ret)
        return ret;

    if (datalen != 5 || strcmp (data, "fnord") != 0)
    {
        fprintf (stderr,
                 "libssh2_base64_decode() failed (%d, %.*s)\n",
                 datalen, datalen, data);
        return 1;
    }

    free (data);

    return 0;
}

int main(int argc, char *argv[])
{
    LIBSSH2_SESSION *session;
    int rc;
    (void)argv;
    (void)argc;

    rc = libssh2_init (LIBSSH2_INIT_NO_CRYPTO);
    if (rc != 0)
    {
        fprintf (stderr, "libssh2_init() failed: %d\n", rc);
        return 1;
    }

    session = libssh2_session_init();
    if (!session)
    {
        fprintf (stderr, "libssh2_session_init() failed\n");
        return 1;
    }

    test_libssh2_base64_decode (session);

    libssh2_session_free(session);

    libssh2_exit ();

    return 0;
}
