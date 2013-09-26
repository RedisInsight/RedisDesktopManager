#ifndef LIBSSH2_PACKET_H
#define LIBSSH2_PACKET_H
/*
 * Copyright (C) 2010 by Daniel Stenberg
 * Author: Daniel Stenberg <daniel@haxx.se>
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
 *
 */

int _libssh2_packet_read(LIBSSH2_SESSION * session);

int _libssh2_packet_ask(LIBSSH2_SESSION * session, unsigned char packet_type,
                        unsigned char **data, size_t *data_len,
                        int match_ofs,
                        const unsigned char *match_buf,
                        size_t match_len);

int _libssh2_packet_askv(LIBSSH2_SESSION * session,
                         const unsigned char *packet_types,
                         unsigned char **data, size_t *data_len,
                         int match_ofs,
                         const unsigned char *match_buf,
                         size_t match_len);
int _libssh2_packet_require(LIBSSH2_SESSION * session,
                            unsigned char packet_type, unsigned char **data,
                            size_t *data_len, int match_ofs,
                            const unsigned char *match_buf,
                            size_t match_len,
                            packet_require_state_t * state);
int _libssh2_packet_requirev(LIBSSH2_SESSION *session,
                             const unsigned char *packet_types,
                             unsigned char **data, size_t *data_len,
                             int match_ofs,
                             const unsigned char *match_buf,
                             size_t match_len,
                             packet_requirev_state_t * state);
int _libssh2_packet_burn(LIBSSH2_SESSION * session,
                         libssh2_nonblocking_states * state);
int _libssh2_packet_write(LIBSSH2_SESSION * session, unsigned char *data,
                          unsigned long data_len);
int _libssh2_packet_add(LIBSSH2_SESSION * session, unsigned char *data,
                        size_t datalen, int macstate);

#endif /* LIBSSH2_PACKET_H */
