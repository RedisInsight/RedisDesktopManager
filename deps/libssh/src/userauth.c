/* Copyright (c) 2004-2007, Sara Golemon <sarag@libssh2.org>
 * Copyright (c) 2005 Mikhail Gusarov <dottedmag@dottedmag.net>
 * Copyright (c) 2009-2011 by Daniel Stenberg
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

#include <ctype.h>
#include <stdio.h>

#include <assert.h>

/* Needed for struct iovec on some platforms */
#ifdef HAVE_SYS_UIO_H
#include <sys/uio.h>
#endif

#include "transport.h"
#include "session.h"
#include "userauth.h"

/* libssh2_userauth_list
 *
 * List authentication methods
 * Will yield successful login if "none" happens to be allowable for this user
 * Not a common configuration for any SSH server though
 * username should be NULL, or a null terminated string
 */
static char *userauth_list(LIBSSH2_SESSION *session, const char *username,
                           unsigned int username_len)
{
    static const unsigned char reply_codes[3] =
        { SSH_MSG_USERAUTH_SUCCESS, SSH_MSG_USERAUTH_FAILURE, 0 };
    /* packet_type(1) + username_len(4) + service_len(4) +
       service(14)"ssh-connection" + method_len(4) = 27 */
    unsigned long methods_len;
    unsigned char *s;
    int rc;

    if (session->userauth_list_state == libssh2_NB_state_idle) {
        /* Zero the whole thing out */
        memset(&session->userauth_list_packet_requirev_state, 0,
               sizeof(session->userauth_list_packet_requirev_state));

        session->userauth_list_data_len = username_len + 27;

        s = session->userauth_list_data =
            LIBSSH2_ALLOC(session, session->userauth_list_data_len);
        if (!session->userauth_list_data) {
            _libssh2_error(session, LIBSSH2_ERROR_ALLOC,
                           "Unable to allocate memory for userauth_list");
            return NULL;
        }

        *(s++) = SSH_MSG_USERAUTH_REQUEST;
        _libssh2_store_str(&s, username, username_len);
        _libssh2_store_str(&s, "ssh-connection", 14);
        _libssh2_store_u32(&s, 4); /* send "none" separately */

        session->userauth_list_state = libssh2_NB_state_created;
    }

    if (session->userauth_list_state == libssh2_NB_state_created) {
        rc = _libssh2_transport_send(session, session->userauth_list_data,
                                     session->userauth_list_data_len,
                                     (unsigned char *)"none", 4);
        if (rc == LIBSSH2_ERROR_EAGAIN) {
            _libssh2_error(session, LIBSSH2_ERROR_EAGAIN,
                           "Would block requesting userauth list");
            return NULL;
        }
        /* now free the packet that was sent */
        LIBSSH2_FREE(session, session->userauth_list_data);
        session->userauth_list_data = NULL;

        if (rc) {
            _libssh2_error(session, LIBSSH2_ERROR_SOCKET_SEND,
                           "Unable to send userauth-none request");
            session->userauth_list_state = libssh2_NB_state_idle;
            return NULL;
        }

        session->userauth_list_state = libssh2_NB_state_sent;
    }

    if (session->userauth_list_state == libssh2_NB_state_sent) {
        rc = _libssh2_packet_requirev(session, reply_codes,
                                      &session->userauth_list_data,
                                      &session->userauth_list_data_len, 0,
                                      NULL, 0,
                                      &session->userauth_list_packet_requirev_state);
        if (rc == LIBSSH2_ERROR_EAGAIN) {
            _libssh2_error(session, LIBSSH2_ERROR_EAGAIN,
                           "Would block requesting userauth list");
            return NULL;
        } else if (rc) {
            _libssh2_error(session, rc, "Failed getting response");
            session->userauth_list_state = libssh2_NB_state_idle;
            return NULL;
        }

        if (session->userauth_list_data[0] == SSH_MSG_USERAUTH_SUCCESS) {
            /* Wow, who'dve thought... */
            _libssh2_error(session, LIBSSH2_ERROR_NONE, "No error");
            LIBSSH2_FREE(session, session->userauth_list_data);
            session->userauth_list_data = NULL;
            session->state |= LIBSSH2_STATE_AUTHENTICATED;
            session->userauth_list_state = libssh2_NB_state_idle;
            return NULL;
        }

        methods_len = _libssh2_ntohu32(session->userauth_list_data + 1);

        /* Do note that the memory areas overlap! */
        memmove(session->userauth_list_data, session->userauth_list_data + 5,
                methods_len);
        session->userauth_list_data[methods_len] = '\0';
        _libssh2_debug(session, LIBSSH2_TRACE_AUTH,
                       "Permitted auth methods: %s",
                       session->userauth_list_data);
    }

    session->userauth_list_state = libssh2_NB_state_idle;
    return (char *) session->userauth_list_data;
}

/* libssh2_userauth_list
 *
 * List authentication methods
 * Will yield successful login if "none" happens to be allowable for this user
 * Not a common configuration for any SSH server though
 * username should be NULL, or a null terminated string
 */
LIBSSH2_API char *
libssh2_userauth_list(LIBSSH2_SESSION * session, const char *user,
                      unsigned int user_len)
{
    char *ptr;
    BLOCK_ADJUST_ERRNO(ptr, session,
                       userauth_list(session, user, user_len));
    return ptr;
}

/*
 * libssh2_userauth_authenticated
 *
 * Returns: 0 if not yet authenticated
 *          1 if already authenticated
 */
LIBSSH2_API int
libssh2_userauth_authenticated(LIBSSH2_SESSION * session)
{
    return (session->state & LIBSSH2_STATE_AUTHENTICATED)?1:0;
}



/* userauth_password
 * Plain ol' login
 */
static int
userauth_password(LIBSSH2_SESSION *session,
                  const char *username, unsigned int username_len,
                  const unsigned char *password, unsigned int password_len,
                  LIBSSH2_PASSWD_CHANGEREQ_FUNC((*passwd_change_cb)))
{
    unsigned char *s;
    static const unsigned char reply_codes[4] =
        { SSH_MSG_USERAUTH_SUCCESS, SSH_MSG_USERAUTH_FAILURE,
          SSH_MSG_USERAUTH_PASSWD_CHANGEREQ, 0
        };
    int rc;

    if (session->userauth_pswd_state == libssh2_NB_state_idle) {
        /* Zero the whole thing out */
        memset(&session->userauth_pswd_packet_requirev_state, 0,
               sizeof(session->userauth_pswd_packet_requirev_state));

        /*
         * 40 = acket_type(1) + username_len(4) + service_len(4) +
         * service(14)"ssh-connection" + method_len(4) + method(8)"password" +
         * chgpwdbool(1) + password_len(4) */
        session->userauth_pswd_data_len = username_len + 40;

        session->userauth_pswd_data0 = ~SSH_MSG_USERAUTH_PASSWD_CHANGEREQ;

        /* TODO: remove this alloc with a fixed buffer in the session
           struct */
        s = session->userauth_pswd_data =
            LIBSSH2_ALLOC(session, session->userauth_pswd_data_len);
        if (!session->userauth_pswd_data) {
            return _libssh2_error(session, LIBSSH2_ERROR_ALLOC,
                                  "Unable to allocate memory for "
                                  "userauth-password request");
        }

        *(s++) = SSH_MSG_USERAUTH_REQUEST;
        _libssh2_store_str(&s, username, username_len);
        _libssh2_store_str(&s, "ssh-connection", sizeof("ssh-connection") - 1);
        _libssh2_store_str(&s, "password", sizeof("password") - 1);
        *s++ = '\0';
        _libssh2_store_u32(&s, password_len);
        /* 'password' is sent separately */

        _libssh2_debug(session, LIBSSH2_TRACE_AUTH,
                       "Attempting to login using password authentication");

        session->userauth_pswd_state = libssh2_NB_state_created;
    }

    if (session->userauth_pswd_state == libssh2_NB_state_created) {
        rc = _libssh2_transport_send(session, session->userauth_pswd_data,
                                     session->userauth_pswd_data_len,
                                     password, password_len);
        if (rc == LIBSSH2_ERROR_EAGAIN) {
            return _libssh2_error(session, LIBSSH2_ERROR_EAGAIN,
                                  "Would block writing password request");
        }

        /* now free the sent packet */
        LIBSSH2_FREE(session, session->userauth_pswd_data);
        session->userauth_pswd_data = NULL;

        if (rc) {
            session->userauth_pswd_state = libssh2_NB_state_idle;
            return _libssh2_error(session, LIBSSH2_ERROR_SOCKET_SEND,
                                  "Unable to send userauth-password request");
        }

        session->userauth_pswd_state = libssh2_NB_state_sent;
    }

  password_response:

    if ((session->userauth_pswd_state == libssh2_NB_state_sent)
        || (session->userauth_pswd_state == libssh2_NB_state_sent1)
        || (session->userauth_pswd_state == libssh2_NB_state_sent2)) {
        if (session->userauth_pswd_state == libssh2_NB_state_sent) {
            rc = _libssh2_packet_requirev(session, reply_codes,
                                          &session->userauth_pswd_data,
                                          &session->userauth_pswd_data_len,
                                          0, NULL, 0,
                                          &session->
                                          userauth_pswd_packet_requirev_state);
            if (rc == LIBSSH2_ERROR_EAGAIN) {
                return _libssh2_error(session, LIBSSH2_ERROR_EAGAIN,
                                      "Would block waiting");
            } else if (rc) {
                session->userauth_pswd_state = libssh2_NB_state_idle;
                return _libssh2_error(session, LIBSSH2_ERROR_TIMEOUT,
                                      "Would block waiting");
            }

            if (session->userauth_pswd_data[0] == SSH_MSG_USERAUTH_SUCCESS) {
                _libssh2_debug(session, LIBSSH2_TRACE_AUTH,
                               "Password authentication successful");
                LIBSSH2_FREE(session, session->userauth_pswd_data);
                session->userauth_pswd_data = NULL;
                session->state |= LIBSSH2_STATE_AUTHENTICATED;
                session->userauth_pswd_state = libssh2_NB_state_idle;
                return 0;
            } else if (session->userauth_pswd_data[0] == SSH_MSG_USERAUTH_FAILURE) {
                _libssh2_debug(session, LIBSSH2_TRACE_AUTH,
                               "Password authentication failed");
                LIBSSH2_FREE(session, session->userauth_pswd_data);
                session->userauth_pswd_data = NULL;
                session->userauth_pswd_state = libssh2_NB_state_idle;
                return _libssh2_error(session,
                                      LIBSSH2_ERROR_AUTHENTICATION_FAILED,
                                      "Authentication failed "
                                      "(username/password)");
            }

            session->userauth_pswd_newpw = NULL;
            session->userauth_pswd_newpw_len = 0;

            session->userauth_pswd_state = libssh2_NB_state_sent1;
        }

        if ((session->userauth_pswd_data[0] ==
             SSH_MSG_USERAUTH_PASSWD_CHANGEREQ)
            || (session->userauth_pswd_data0 ==
                SSH_MSG_USERAUTH_PASSWD_CHANGEREQ)) {
            session->userauth_pswd_data0 = SSH_MSG_USERAUTH_PASSWD_CHANGEREQ;

            if ((session->userauth_pswd_state == libssh2_NB_state_sent1) ||
                (session->userauth_pswd_state == libssh2_NB_state_sent2)) {
                if (session->userauth_pswd_state == libssh2_NB_state_sent1) {
                    _libssh2_debug(session, LIBSSH2_TRACE_AUTH,
                                   "Password change required");
                    LIBSSH2_FREE(session, session->userauth_pswd_data);
                    session->userauth_pswd_data = NULL;
                }
                if (passwd_change_cb) {
                    if (session->userauth_pswd_state == libssh2_NB_state_sent1) {
                        passwd_change_cb(session,
                                         &session->userauth_pswd_newpw,
                                         &session->userauth_pswd_newpw_len,
                                         &session->abstract);
                        if (!session->userauth_pswd_newpw) {
                            return _libssh2_error(session,
                                                  LIBSSH2_ERROR_PASSWORD_EXPIRED,
                                                  "Password expired, and "
                                                  "callback failed");
                        }

                        /* basic data_len + newpw_len(4) */
                        session->userauth_pswd_data_len =
                            username_len + password_len + 44;

                        s = session->userauth_pswd_data =
                            LIBSSH2_ALLOC(session,
                                          session->userauth_pswd_data_len);
                        if (!session->userauth_pswd_data) {
                            LIBSSH2_FREE(session,
                                         session->userauth_pswd_newpw);
                            session->userauth_pswd_newpw = NULL;
                            return _libssh2_error(session, LIBSSH2_ERROR_ALLOC,
                                                  "Unable to allocate memory "
                                                  "for userauth password "
                                                  "change request");
                        }

                        *(s++) = SSH_MSG_USERAUTH_REQUEST;
                        _libssh2_store_str(&s, username, username_len);
                        _libssh2_store_str(&s, "ssh-connection",
                                           sizeof("ssh-connection") - 1);
                        _libssh2_store_str(&s, "password",
                                           sizeof("password") - 1);
                        *s++ = 0x01;
                        _libssh2_store_str(&s, (char *)password, password_len);
                        _libssh2_store_u32(&s,
                                           session->userauth_pswd_newpw_len);
                        /* send session->userauth_pswd_newpw separately */

                        session->userauth_pswd_state = libssh2_NB_state_sent2;
                    }

                    if (session->userauth_pswd_state == libssh2_NB_state_sent2) {
                        rc = _libssh2_transport_send(session,
                                                     session->userauth_pswd_data,
                                                     session->userauth_pswd_data_len,
                                                     (unsigned char *)
                                                     session->userauth_pswd_newpw,
                                                     session->userauth_pswd_newpw_len);
                        if (rc == LIBSSH2_ERROR_EAGAIN) {
                            return _libssh2_error(session, LIBSSH2_ERROR_EAGAIN,
                                                  "Would block waiting");
                        }

                        /* free the allocated packets again */
                        LIBSSH2_FREE(session, session->userauth_pswd_data);
                        session->userauth_pswd_data = NULL;
                        LIBSSH2_FREE(session, session->userauth_pswd_newpw);
                        session->userauth_pswd_newpw = NULL;

                        if (rc) {
                            return _libssh2_error(session,
                                                  LIBSSH2_ERROR_SOCKET_SEND,
                                                  "Unable to send userauth "
                                                  "password-change request");
                        }

                        /*
                         * Ugliest use of goto ever.  Blame it on the
                         * askN => requirev migration.
                         */
                        session->userauth_pswd_state = libssh2_NB_state_sent;
                        goto password_response;
                    }
                }
            } else {
                session->userauth_pswd_state = libssh2_NB_state_idle;
                return _libssh2_error(session, LIBSSH2_ERROR_PASSWORD_EXPIRED,
                                      "Password Expired, and no callback "
                                      "specified");
            }
        }
    }

    /* FAILURE */
    LIBSSH2_FREE(session, session->userauth_pswd_data);
    session->userauth_pswd_data = NULL;
    session->userauth_pswd_state = libssh2_NB_state_idle;

    return _libssh2_error(session, LIBSSH2_ERROR_AUTHENTICATION_FAILED,
                          "Authentication failed");
}

/*
 * libssh2_userauth_password_ex
 *
 * Plain ol' login
 */

LIBSSH2_API int
libssh2_userauth_password_ex(LIBSSH2_SESSION *session, const char *username,
                             unsigned int username_len, const char *password,
                             unsigned int password_len,
                             LIBSSH2_PASSWD_CHANGEREQ_FUNC((*passwd_change_cb)))
{
    int rc;
    BLOCK_ADJUST(rc, session,
                 userauth_password(session, username, username_len,
                                   (unsigned char *)password, password_len,
                                   passwd_change_cb));
    return rc;
}

/*
 * file_read_publickey
 *
 * Read a public key from an id_???.pub style file
 *
 * Returns an allocated string containing the decoded key in *pubkeydata
 * on success.
 * Returns an allocated string containing the key method (e.g. "ssh-dss")
 * in method on success.
 */
static int
file_read_publickey(LIBSSH2_SESSION * session, unsigned char **method,
                    size_t *method_len,
                    unsigned char **pubkeydata,
                    size_t *pubkeydata_len,
                    const char *pubkeyfile)
{
    FILE *fd;
    char c;
    unsigned char *pubkey = NULL, *sp1, *sp2, *tmp;
    size_t pubkey_len = 0;
    unsigned int tmp_len;

    _libssh2_debug(session, LIBSSH2_TRACE_AUTH, "Loading public key file: %s",
                   pubkeyfile);
    /* Read Public Key */
    fd = fopen(pubkeyfile, "r");
    if (!fd) {
        return _libssh2_error(session, LIBSSH2_ERROR_FILE,
                              "Unable to open public key file");
    }
    while (!feof(fd) && 1 == fread(&c, 1, 1, fd) && c != '\r' && c != '\n')
        pubkey_len++;
    if (feof(fd)) {
        /* the last character was EOF */
        pubkey_len--;
    }
    rewind(fd);

    if (pubkey_len <= 1) {
        fclose(fd);
        return _libssh2_error(session, LIBSSH2_ERROR_FILE,
                              "Invalid data in public key file");
    }

    pubkey = LIBSSH2_ALLOC(session, pubkey_len);
    if (!pubkey) {
        fclose(fd);
        return _libssh2_error(session, LIBSSH2_ERROR_ALLOC,
                              "Unable to allocate memory for public key data");
    }
    if (fread(pubkey, 1, pubkey_len, fd) != pubkey_len) {
        LIBSSH2_FREE(session, pubkey);
        fclose(fd);
        return _libssh2_error(session, LIBSSH2_ERROR_FILE,
                              "Unable to read public key from file");
    }
    fclose(fd);
    /*
     * Remove trailing whitespace
     */
    while (pubkey_len && isspace(pubkey[pubkey_len - 1]))
        pubkey_len--;

    if (!pubkey_len) {
        LIBSSH2_FREE(session, pubkey);
        return _libssh2_error(session, LIBSSH2_ERROR_FILE,
                              "Missing public key data");
    }

    if ((sp1 = memchr(pubkey, ' ', pubkey_len)) == NULL) {
        LIBSSH2_FREE(session, pubkey);
        return _libssh2_error(session, LIBSSH2_ERROR_FILE,
                              "Invalid public key data");
    }

    sp1++;

    if ((sp2 = memchr(sp1, ' ', pubkey_len - (sp1 - pubkey - 1))) == NULL) {
        /* Assume that the id string is missing, but that it's okay */
        sp2 = pubkey + pubkey_len;
    }

    if (libssh2_base64_decode(session, (char **) &tmp, &tmp_len,
                              (char *) sp1, sp2 - sp1)) {
        LIBSSH2_FREE(session, pubkey);
        return _libssh2_error(session, LIBSSH2_ERROR_FILE,
                              "Invalid key data, not base64 encoded");
    }

    /* Wasting some bytes here (okay, more than some), but since it's likely
     * to be freed soon anyway, we'll just avoid the extra free/alloc and call
     * it a wash */
    *method = pubkey;
    *method_len = sp1 - pubkey - 1;

    *pubkeydata = tmp;
    *pubkeydata_len = tmp_len;

    return 0;
}



/* libssh2_file_read_privatekey
 * Read a PEM encoded private key from an id_??? style file
 */
static int
file_read_privatekey(LIBSSH2_SESSION * session,
                     const LIBSSH2_HOSTKEY_METHOD ** hostkey_method,
                     void **hostkey_abstract,
                     const unsigned char *method, int method_len,
                     const char *privkeyfile, const char *passphrase)
{
    const LIBSSH2_HOSTKEY_METHOD **hostkey_methods_avail =
        libssh2_hostkey_methods();

    _libssh2_debug(session, LIBSSH2_TRACE_AUTH, "Loading private key file: %s",
                   privkeyfile);
    *hostkey_method = NULL;
    *hostkey_abstract = NULL;
    while (*hostkey_methods_avail && (*hostkey_methods_avail)->name) {
        if ((*hostkey_methods_avail)->initPEM
            && strncmp((*hostkey_methods_avail)->name, (const char *) method,
                       method_len) == 0) {
            *hostkey_method = *hostkey_methods_avail;
            break;
        }
        hostkey_methods_avail++;
    }
    if (!*hostkey_method) {
        return _libssh2_error(session, LIBSSH2_ERROR_METHOD_NONE,
                              "No handler for specified private key");
    }

    if ((*hostkey_method)->
        initPEM(session, privkeyfile, (unsigned char *) passphrase,
                hostkey_abstract)) {
        return _libssh2_error(session, LIBSSH2_ERROR_FILE,
                              "Unable to initialize private key from file");
    }

    return 0;
}

struct privkey_file {
    const char *filename;
    const char *passphrase;
};

static int
sign_fromfile(LIBSSH2_SESSION *session, unsigned char **sig, size_t *sig_len,
              const unsigned char *data, size_t data_len, void **abstract)
{
    struct privkey_file *privkey_file = (struct privkey_file *) (*abstract);
    const LIBSSH2_HOSTKEY_METHOD *privkeyobj;
    void *hostkey_abstract;
    struct iovec datavec;
    int rc;

    rc = file_read_privatekey(session, &privkeyobj, &hostkey_abstract,
                              session->userauth_pblc_method,
                              session->userauth_pblc_method_len,
                              privkey_file->filename,
                              privkey_file->passphrase);
    if(rc)
        return rc;

    datavec.iov_base = (void *)data;
    datavec.iov_len  = data_len;

    if (privkeyobj->signv(session, sig, sig_len, 1, &datavec,
                          &hostkey_abstract)) {
        if (privkeyobj->dtor) {
            privkeyobj->dtor(session, abstract);
        }
        return -1;
    }

    if (privkeyobj->dtor) {
        privkeyobj->dtor(session, &hostkey_abstract);
    }
    return 0;
}



/* userauth_hostbased_fromfile
 * Authenticate using a keypair found in the named files
 */
static int
userauth_hostbased_fromfile(LIBSSH2_SESSION *session,
                            const char *username, size_t username_len,
                            const char *publickey, const char *privatekey,
                            const char *passphrase, const char *hostname,
                            size_t hostname_len,
                            const char *local_username,
                            size_t local_username_len)
{
    int rc;

    if (session->userauth_host_state == libssh2_NB_state_idle) {
        const LIBSSH2_HOSTKEY_METHOD *privkeyobj;
        unsigned char *pubkeydata, *sig;
        size_t pubkeydata_len;
        size_t sig_len;
        void *abstract;
        unsigned char buf[5];
        struct iovec datavec[4];

        /* Zero the whole thing out */
        memset(&session->userauth_host_packet_requirev_state, 0,
               sizeof(session->userauth_host_packet_requirev_state));

        if (publickey) {
            rc = file_read_publickey(session, &session->userauth_host_method,
                                     &session->userauth_host_method_len,
                                     &pubkeydata, &pubkeydata_len, publickey);
            if(rc)
                /* Note: file_read_publickey() calls _libssh2_error() */
                return rc;
        }
        else {
            /* Compute public key from private key. */
            rc = _libssh2_pub_priv_keyfile(session,
                                           &session->userauth_host_method,
                                           &session->userauth_host_method_len,
                                           &pubkeydata, &pubkeydata_len,
                                           privatekey, passphrase);
            if (rc)
                /* libssh2_pub_priv_keyfile calls _libssh2_error() */
                return rc;
        }

        /*
         * 52 = packet_type(1) + username_len(4) + servicename_len(4) +
         * service_name(14)"ssh-connection" + authmethod_len(4) +
         * authmethod(9)"hostbased" + method_len(4) + pubkeydata_len(4) +
         * hostname_len(4) + local_username_len(4)
         */
        session->userauth_host_packet_len =
            username_len + session->userauth_host_method_len + hostname_len +
            local_username_len + pubkeydata_len + 52;

        /*
         * Preallocate space for an overall length,  method name again,
         * and the signature, which won't be any larger than the size of
         * the publickeydata itself
         */
        session->userauth_host_s = session->userauth_host_packet =
            LIBSSH2_ALLOC(session,
                          session->userauth_host_packet_len + 4 +
                          (4 + session->userauth_host_method_len) +
                          (4 + pubkeydata_len));
        if (!session->userauth_host_packet) {
            LIBSSH2_FREE(session, session->userauth_host_method);
            session->userauth_host_method = NULL;
            LIBSSH2_FREE(session, pubkeydata);
            return _libssh2_error(session, LIBSSH2_ERROR_ALLOC,
                                  "Out of memory");
        }

        *(session->userauth_host_s++) = SSH_MSG_USERAUTH_REQUEST;
        _libssh2_store_str(&session->userauth_host_s, username, username_len);
        _libssh2_store_str(&session->userauth_host_s, "ssh-connection", 14);
        _libssh2_store_str(&session->userauth_host_s, "hostbased", 9);
        _libssh2_store_str(&session->userauth_host_s,
                           (const char *)session->userauth_host_method,
                           session->userauth_host_method_len);
        _libssh2_store_str(&session->userauth_host_s, (const char *)pubkeydata,
                           pubkeydata_len);
        LIBSSH2_FREE(session, pubkeydata);
        _libssh2_store_str(&session->userauth_host_s, hostname, hostname_len);
        _libssh2_store_str(&session->userauth_host_s, local_username,
                           local_username_len);

        rc = file_read_privatekey(session, &privkeyobj, &abstract,
                                  session->userauth_host_method,
                                  session->userauth_host_method_len,
                                  privatekey, passphrase);
        if(rc) {
            /* Note: file_read_privatekey() calls _libssh2_error() */
            LIBSSH2_FREE(session, session->userauth_host_method);
            session->userauth_host_method = NULL;
            LIBSSH2_FREE(session, session->userauth_host_packet);
            session->userauth_host_packet = NULL;
            return rc;
        }

        _libssh2_htonu32(buf, session->session_id_len);
        datavec[0].iov_base = (void *)buf;
        datavec[0].iov_len = 4;
        datavec[1].iov_base = (void *)session->session_id;
        datavec[1].iov_len = session->session_id_len;
        datavec[2].iov_base = (void *)session->userauth_host_packet;
        datavec[2].iov_len = session->userauth_host_packet_len;

        if (privkeyobj->signv(session, &sig, &sig_len, 3, datavec, &abstract)) {
            LIBSSH2_FREE(session, session->userauth_host_method);
            session->userauth_host_method = NULL;
            LIBSSH2_FREE(session, session->userauth_host_packet);
            session->userauth_host_packet = NULL;
            if (privkeyobj->dtor) {
                privkeyobj->dtor(session, &abstract);
            }
            return -1;
        }

        if (privkeyobj->dtor) {
            privkeyobj->dtor(session, &abstract);
        }

        if (sig_len > pubkeydata_len) {
            unsigned char *newpacket;
            /* Should *NEVER* happen, but...well.. better safe than sorry */
            newpacket = LIBSSH2_REALLOC(session, session->userauth_host_packet,
                                        session->userauth_host_packet_len + 4 +
                                        (4 + session->userauth_host_method_len)
                                        + (4 + sig_len)); /* PK sigblob */
            if (!newpacket) {
                LIBSSH2_FREE(session, sig);
                LIBSSH2_FREE(session, session->userauth_host_packet);
                session->userauth_host_packet = NULL;
                LIBSSH2_FREE(session, session->userauth_host_method);
                session->userauth_host_method = NULL;
                return _libssh2_error(session, LIBSSH2_ERROR_ALLOC,
                                      "Failed allocating additional space for "
                                      "userauth-hostbased packet");
            }
            session->userauth_host_packet = newpacket;
        }

        session->userauth_host_s =
            session->userauth_host_packet + session->userauth_host_packet_len;

        _libssh2_store_u32(&session->userauth_host_s,
                           4 + session->userauth_host_method_len + 4 + sig_len);
        _libssh2_store_str(&session->userauth_host_s,
                           (const char *)session->userauth_host_method,
                           session->userauth_host_method_len);
        LIBSSH2_FREE(session, session->userauth_host_method);
        session->userauth_host_method = NULL;

        _libssh2_store_str(&session->userauth_host_s, (const char *)sig,
                           sig_len);
        LIBSSH2_FREE(session, sig);

        _libssh2_debug(session, LIBSSH2_TRACE_AUTH,
                       "Attempting hostbased authentication");

        session->userauth_host_state = libssh2_NB_state_created;
    }

    if (session->userauth_host_state == libssh2_NB_state_created) {
        rc = _libssh2_transport_send(session, session->userauth_host_packet,
                                     session->userauth_host_s -
                                     session->userauth_host_packet,
                                     NULL, 0);
        if (rc == LIBSSH2_ERROR_EAGAIN) {
            return _libssh2_error(session, LIBSSH2_ERROR_EAGAIN, "Would block");
        }
        else if (rc) {
            LIBSSH2_FREE(session, session->userauth_host_packet);
            session->userauth_host_packet = NULL;
            session->userauth_host_state = libssh2_NB_state_idle;
            return _libssh2_error(session, LIBSSH2_ERROR_SOCKET_SEND,
                                  "Unable to send userauth-hostbased request");
        }
        LIBSSH2_FREE(session, session->userauth_host_packet);
        session->userauth_host_packet = NULL;

        session->userauth_host_state = libssh2_NB_state_sent;
    }

    if (session->userauth_host_state == libssh2_NB_state_sent) {
        static const unsigned char reply_codes[3] =
            { SSH_MSG_USERAUTH_SUCCESS, SSH_MSG_USERAUTH_FAILURE, 0 };
        size_t data_len;
        rc = _libssh2_packet_requirev(session, reply_codes,
                                      &session->userauth_host_data,
                                      &data_len, 0, NULL, 0,
                                      &session->
                                      userauth_host_packet_requirev_state);
        if (rc == LIBSSH2_ERROR_EAGAIN) {
            return _libssh2_error(session, LIBSSH2_ERROR_EAGAIN, "Would block");
        }

        session->userauth_host_state = libssh2_NB_state_idle;
        if (rc) {
            return _libssh2_error(session, LIBSSH2_ERROR_PUBLICKEY_UNVERIFIED,
                                  "Auth failed");
        }

        if (session->userauth_host_data[0] == SSH_MSG_USERAUTH_SUCCESS) {
            _libssh2_debug(session, LIBSSH2_TRACE_AUTH,
                           "Hostbased authentication successful");
            /* We are us and we've proved it. */
            LIBSSH2_FREE(session, session->userauth_host_data);
            session->userauth_host_data = NULL;
            session->state |= LIBSSH2_STATE_AUTHENTICATED;
            return 0;
        }
    }

    /* This public key is not allowed for this user on this server */
    LIBSSH2_FREE(session, session->userauth_host_data);
    session->userauth_host_data = NULL;
    return _libssh2_error(session, LIBSSH2_ERROR_PUBLICKEY_UNVERIFIED,
                          "Invalid signature for supplied public key, or bad "
                          "username/public key combination");
}

/* libssh2_userauth_hostbased_fromfile_ex
 * Authenticate using a keypair found in the named files
 */
LIBSSH2_API int
libssh2_userauth_hostbased_fromfile_ex(LIBSSH2_SESSION *session,
                                       const char *user,
                                       unsigned int user_len,
                                       const char *publickey,
                                       const char *privatekey,
                                       const char *passphrase,
                                       const char *host,
                                       unsigned int host_len,
                                       const char *localuser,
                                       unsigned int localuser_len)
{
    int rc;
    BLOCK_ADJUST(rc, session,
                 userauth_hostbased_fromfile(session, user, user_len,
                                             publickey, privatekey,
                                             passphrase, host, host_len,
                                             localuser, localuser_len));
    return rc;
}



int
_libssh2_userauth_publickey(LIBSSH2_SESSION *session,
                            const char *username,
                            unsigned int username_len,
                            const unsigned char *pubkeydata,
                            unsigned long pubkeydata_len,
                            LIBSSH2_USERAUTH_PUBLICKEY_SIGN_FUNC((*sign_callback)),
                            void *abstract)
{
    unsigned char reply_codes[4] =
        { SSH_MSG_USERAUTH_SUCCESS, SSH_MSG_USERAUTH_FAILURE,
          SSH_MSG_USERAUTH_PK_OK, 0
        };
    int rc;
    unsigned char *s;

    if (session->userauth_pblc_state == libssh2_NB_state_idle) {

        /*
         * The call to _libssh2_ntohu32 later relies on pubkeydata having at
         * least 4 valid bytes containing the length of the method name.
         */
        if (pubkeydata_len < 4)
            return _libssh2_error(session, LIBSSH2_ERROR_PUBLICKEY_UNVERIFIED,
                                  "Invalid public key, too short");

        /* Zero the whole thing out */
        memset(&session->userauth_pblc_packet_requirev_state, 0,
               sizeof(session->userauth_pblc_packet_requirev_state));

        /*
         * As an optimisation, userauth_publickey_fromfile reuses a
         * previously allocated copy of the method name to avoid an extra
         * allocation/free.
         * For other uses, we allocate and populate it here.
         */
        if (!session->userauth_pblc_method) {
            session->userauth_pblc_method_len = _libssh2_ntohu32(pubkeydata);

            if(session->userauth_pblc_method_len > pubkeydata_len)
                /* the method length simply cannot be longer than the entire
                   passed in data, so we use this to detect crazy input
                   data */
                return _libssh2_error(session,
                                      LIBSSH2_ERROR_PUBLICKEY_UNVERIFIED,
                                      "Invalid public key");

            session->userauth_pblc_method =
                LIBSSH2_ALLOC(session, session->userauth_pblc_method_len);
            if (!session->userauth_pblc_method) {
                return _libssh2_error(session, LIBSSH2_ERROR_ALLOC,
                                      "Unable to allocate memory for public key "
                                      "data");
            }
            memcpy(session->userauth_pblc_method, pubkeydata + 4,
                   session->userauth_pblc_method_len);
        }
        /*
         * The length of the method name read from plaintext prefix in the
         * file must match length embedded in the key.
         * TODO: The data should match too but we don't check that. Should we?
         */
        else if (session->userauth_pblc_method_len !=
                 _libssh2_ntohu32(pubkeydata))
            return _libssh2_error(session, LIBSSH2_ERROR_PUBLICKEY_UNVERIFIED,
                                  "Invalid public key");

        /*
         * 45 = packet_type(1) + username_len(4) + servicename_len(4) +
         * service_name(14)"ssh-connection" + authmethod_len(4) +
         * authmethod(9)"publickey" + sig_included(1)'\0' + algmethod_len(4) +
         * publickey_len(4)
         */
        session->userauth_pblc_packet_len =
            username_len + session->userauth_pblc_method_len + pubkeydata_len +
            45;

        /*
         * Preallocate space for an overall length, method name again, and the
         * signature, which won't be any larger than the size of the
         * publickeydata itself.
         *
         * Note that the 'pubkeydata_len' extra bytes allocated here will not
         * be used in this first send, but will be used in the later one where
         * this same allocation is re-used.
         */
        s = session->userauth_pblc_packet =
            LIBSSH2_ALLOC(session,
                          session->userauth_pblc_packet_len + 4 +
                          (4 + session->userauth_pblc_method_len)
                          + (4 + pubkeydata_len));
        if (!session->userauth_pblc_packet) {
            LIBSSH2_FREE(session, session->userauth_pblc_method);
            session->userauth_pblc_method = NULL;
            return _libssh2_error(session, LIBSSH2_ERROR_ALLOC,
                                  "Out of memory");
        }

        *s++ = SSH_MSG_USERAUTH_REQUEST;
        _libssh2_store_str(&s, username, username_len);
        _libssh2_store_str(&s, "ssh-connection", 14);
        _libssh2_store_str(&s, "publickey", 9);

        session->userauth_pblc_b = s;
        /* Not sending signature with *this* packet */
        *s++ = 0;

        _libssh2_store_str(&s, (const char *)session->userauth_pblc_method,
                           session->userauth_pblc_method_len);
        _libssh2_store_str(&s, (const char *)pubkeydata, pubkeydata_len);

        _libssh2_debug(session, LIBSSH2_TRACE_AUTH,
                       "Attempting publickey authentication");

        session->userauth_pblc_state = libssh2_NB_state_created;
    }

    if (session->userauth_pblc_state == libssh2_NB_state_created) {
        rc = _libssh2_transport_send(session, session->userauth_pblc_packet,
                                     session->userauth_pblc_packet_len,
                                     NULL, 0);
        if (rc == LIBSSH2_ERROR_EAGAIN)
            return _libssh2_error(session, LIBSSH2_ERROR_EAGAIN, "Would block");
        else if (rc) {
            LIBSSH2_FREE(session, session->userauth_pblc_packet);
            session->userauth_pblc_packet = NULL;
            LIBSSH2_FREE(session, session->userauth_pblc_method);
            session->userauth_pblc_method = NULL;
            session->userauth_pblc_state = libssh2_NB_state_idle;
            return _libssh2_error(session, LIBSSH2_ERROR_SOCKET_SEND,
                                  "Unable to send userauth-publickey request");
        }

        session->userauth_pblc_state = libssh2_NB_state_sent;
    }

    if (session->userauth_pblc_state == libssh2_NB_state_sent) {
        rc = _libssh2_packet_requirev(session, reply_codes,
                                      &session->userauth_pblc_data,
                                      &session->userauth_pblc_data_len, 0,
                                      NULL, 0,
                                      &session->
                                      userauth_pblc_packet_requirev_state);
        if (rc == LIBSSH2_ERROR_EAGAIN) {
            return _libssh2_error(session, LIBSSH2_ERROR_EAGAIN, "Would block");
        }
        else if (rc) {
            LIBSSH2_FREE(session, session->userauth_pblc_packet);
            session->userauth_pblc_packet = NULL;
            LIBSSH2_FREE(session, session->userauth_pblc_method);
            session->userauth_pblc_method = NULL;
            session->userauth_pblc_state = libssh2_NB_state_idle;
            return _libssh2_error(session, LIBSSH2_ERROR_PUBLICKEY_UNVERIFIED,
                                  "Waiting for USERAUTH response");
        }

        if (session->userauth_pblc_data[0] == SSH_MSG_USERAUTH_SUCCESS) {
            _libssh2_debug(session, LIBSSH2_TRACE_AUTH,
                           "Pubkey authentication prematurely successful");
            /*
             * God help any SSH server that allows an UNVERIFIED
             * public key to validate the user
             */
            LIBSSH2_FREE(session, session->userauth_pblc_data);
            session->userauth_pblc_data = NULL;
            LIBSSH2_FREE(session, session->userauth_pblc_packet);
            session->userauth_pblc_packet = NULL;
            LIBSSH2_FREE(session, session->userauth_pblc_method);
            session->userauth_pblc_method = NULL;
            session->state |= LIBSSH2_STATE_AUTHENTICATED;
            session->userauth_pblc_state = libssh2_NB_state_idle;
            return 0;
        }

        if (session->userauth_pblc_data[0] == SSH_MSG_USERAUTH_FAILURE) {
            /* This public key is not allowed for this user on this server */
            LIBSSH2_FREE(session, session->userauth_pblc_data);
            session->userauth_pblc_data = NULL;
            LIBSSH2_FREE(session, session->userauth_pblc_packet);
            session->userauth_pblc_packet = NULL;
            LIBSSH2_FREE(session, session->userauth_pblc_method);
            session->userauth_pblc_method = NULL;
            session->userauth_pblc_state = libssh2_NB_state_idle;
            return _libssh2_error(session, LIBSSH2_ERROR_AUTHENTICATION_FAILED,
                                  "Username/PublicKey combination invalid");
        }

        /* Semi-Success! */
        LIBSSH2_FREE(session, session->userauth_pblc_data);
        session->userauth_pblc_data = NULL;

        *session->userauth_pblc_b = 0x01;
        session->userauth_pblc_state = libssh2_NB_state_sent1;
    }

    if (session->userauth_pblc_state == libssh2_NB_state_sent1) {
        unsigned char *buf;
        unsigned char *sig;
        size_t sig_len;

        s = buf = LIBSSH2_ALLOC(session, 4 + session->session_id_len
                                + session->userauth_pblc_packet_len);
        if (!buf) {
            return _libssh2_error(session, LIBSSH2_ERROR_ALLOC,
                                  "Unable to allocate memory for "
                                  "userauth-publickey signed data");
        }

        _libssh2_store_str(&s, (const char *)session->session_id,
                           session->session_id_len);

        memcpy (s, session->userauth_pblc_packet,
                session->userauth_pblc_packet_len);
        s += session->userauth_pblc_packet_len;

        rc = sign_callback(session, &sig, &sig_len, buf, s - buf, abstract);
        LIBSSH2_FREE(session, buf);
        if (rc == LIBSSH2_ERROR_EAGAIN) {
            return _libssh2_error(session, LIBSSH2_ERROR_EAGAIN, "Would block");
        } else if (rc) {
            LIBSSH2_FREE(session, session->userauth_pblc_method);
            session->userauth_pblc_method = NULL;
            LIBSSH2_FREE(session, session->userauth_pblc_packet);
            session->userauth_pblc_packet = NULL;
            session->userauth_pblc_state = libssh2_NB_state_idle;
            return _libssh2_error(session, LIBSSH2_ERROR_PUBLICKEY_UNVERIFIED,
                                  "Callback returned error");
        }

        /*
         * If this function was restarted, pubkeydata_len might still be 0
         * which will cause an unnecessary but harmless realloc here.
         */
        if (sig_len > pubkeydata_len) {
            unsigned char *newpacket;
            /* Should *NEVER* happen, but...well.. better safe than sorry */
            newpacket = LIBSSH2_REALLOC(session,
                                        session->userauth_pblc_packet,
                                        session->userauth_pblc_packet_len + 4 +
                                        (4 + session->userauth_pblc_method_len)
                                        + (4 + sig_len)); /* PK sigblob */
            if (!newpacket) {
                LIBSSH2_FREE(session, sig);
                LIBSSH2_FREE(session, session->userauth_pblc_packet);
                session->userauth_pblc_packet = NULL;
                LIBSSH2_FREE(session, session->userauth_pblc_method);
                session->userauth_pblc_method = NULL;
                session->userauth_pblc_state = libssh2_NB_state_idle;
                return _libssh2_error(session, LIBSSH2_ERROR_ALLOC,
                                      "Failed allocating additional space for "
                                      "userauth-publickey packet");
            }
            session->userauth_pblc_packet = newpacket;
        }

        s = session->userauth_pblc_packet + session->userauth_pblc_packet_len;
        session->userauth_pblc_b = NULL;

        _libssh2_store_u32(&s,
                           4 + session->userauth_pblc_method_len + 4 + sig_len);
        _libssh2_store_str(&s, (const char *)session->userauth_pblc_method,
                           session->userauth_pblc_method_len);

        LIBSSH2_FREE(session, session->userauth_pblc_method);
        session->userauth_pblc_method = NULL;

        _libssh2_store_str(&s, (const char *)sig, sig_len);
        LIBSSH2_FREE(session, sig);

        _libssh2_debug(session, LIBSSH2_TRACE_AUTH,
                       "Attempting publickey authentication -- phase 2");

        session->userauth_pblc_s = s;
        session->userauth_pblc_state = libssh2_NB_state_sent2;
    }

    if (session->userauth_pblc_state == libssh2_NB_state_sent2) {
        rc = _libssh2_transport_send(session, session->userauth_pblc_packet,
                                     session->userauth_pblc_s -
                                     session->userauth_pblc_packet,
                                     NULL, 0);
        if (rc == LIBSSH2_ERROR_EAGAIN) {
            return _libssh2_error(session, LIBSSH2_ERROR_EAGAIN, "Would block");
        } else if (rc) {
            LIBSSH2_FREE(session, session->userauth_pblc_packet);
            session->userauth_pblc_packet = NULL;
            session->userauth_pblc_state = libssh2_NB_state_idle;
            return _libssh2_error(session, LIBSSH2_ERROR_SOCKET_SEND,
                                  "Unable to send userauth-publickey request");
        }
        LIBSSH2_FREE(session, session->userauth_pblc_packet);
        session->userauth_pblc_packet = NULL;

        session->userauth_pblc_state = libssh2_NB_state_sent3;
    }

    /* PK_OK is no longer valid */
    reply_codes[2] = 0;

    rc = _libssh2_packet_requirev(session, reply_codes,
                                  &session->userauth_pblc_data,
                                  &session->userauth_pblc_data_len, 0, NULL, 0,
                                  &session->userauth_pblc_packet_requirev_state);
    if (rc == LIBSSH2_ERROR_EAGAIN) {
        return _libssh2_error(session, LIBSSH2_ERROR_EAGAIN,
                              "Would block requesting userauth list");
    } else if (rc) {
        session->userauth_pblc_state = libssh2_NB_state_idle;
        return _libssh2_error(session, LIBSSH2_ERROR_PUBLICKEY_UNVERIFIED,
                              "Waiting for publickey USERAUTH response");
    }

    if (session->userauth_pblc_data[0] == SSH_MSG_USERAUTH_SUCCESS) {
        _libssh2_debug(session, LIBSSH2_TRACE_AUTH,
                       "Publickey authentication successful");
        /* We are us and we've proved it. */
        LIBSSH2_FREE(session, session->userauth_pblc_data);
        session->userauth_pblc_data = NULL;
        session->state |= LIBSSH2_STATE_AUTHENTICATED;
        session->userauth_pblc_state = libssh2_NB_state_idle;
        return 0;
    }

    /* This public key is not allowed for this user on this server */
    LIBSSH2_FREE(session, session->userauth_pblc_data);
    session->userauth_pblc_data = NULL;
    session->userauth_pblc_state = libssh2_NB_state_idle;
    return _libssh2_error(session, LIBSSH2_ERROR_PUBLICKEY_UNVERIFIED,
                          "Invalid signature for supplied public key, or bad "
                          "username/public key combination");
}

/*
 * userauth_publickey_fromfile
 * Authenticate using a keypair found in the named files
 */
static int
userauth_publickey_fromfile(LIBSSH2_SESSION *session,
                            const char *username,
                            size_t username_len,
                            const char *publickey,
                            const char *privatekey,
                            const char *passphrase)
{
    unsigned char *pubkeydata = NULL;
    size_t pubkeydata_len = 0;
    struct privkey_file privkey_file;
    void *abstract = &privkey_file;
    int rc;

    privkey_file.filename = privatekey;
    privkey_file.passphrase = passphrase;

    if (session->userauth_pblc_state == libssh2_NB_state_idle) {
        if (publickey) {
            rc = file_read_publickey(session, &session->userauth_pblc_method,
                                     &session->userauth_pblc_method_len,
                                     &pubkeydata, &pubkeydata_len,publickey);
            if (rc)
                return rc;
        }
        else {
            /* Compute public key from private key. */
            rc = _libssh2_pub_priv_keyfile(session,
                                           &session->userauth_pblc_method,
                                           &session->userauth_pblc_method_len,
                                           &pubkeydata, &pubkeydata_len,
                                           privatekey, passphrase);

            /* _libssh2_pub_priv_keyfile calls _libssh2_error() */
            if (rc)
                return rc;
        }
    }

    rc = _libssh2_userauth_publickey(session, username, username_len,
                                     pubkeydata, pubkeydata_len,
                                     sign_fromfile, &abstract);
    if(pubkeydata)
        LIBSSH2_FREE(session, pubkeydata);

    return rc;
}

/* libssh2_userauth_publickey_fromfile_ex
 * Authenticate using a keypair found in the named files
 */
LIBSSH2_API int
libssh2_userauth_publickey_fromfile_ex(LIBSSH2_SESSION *session,
                                       const char *user,
                                       unsigned int user_len,
                                       const char *publickey,
                                       const char *privatekey,
                                       const char *passphrase)
{
    int rc;

    if(NULL == passphrase)
        /* if given a NULL pointer, make it point to a zero-length
           string to save us from having to check this all over */
        passphrase="";

    BLOCK_ADJUST(rc, session,
                 userauth_publickey_fromfile(session, user, user_len,
                                             publickey, privatekey,
                                             passphrase));
    return rc;
}

/* libssh2_userauth_publickey_ex
 * Authenticate using an external callback function
 */
LIBSSH2_API int
libssh2_userauth_publickey(LIBSSH2_SESSION *session,
                           const char *user,
                           const unsigned char *pubkeydata,
                           size_t pubkeydata_len,
                           LIBSSH2_USERAUTH_PUBLICKEY_SIGN_FUNC((*sign_callback)),
                           void **abstract)
{
    int rc;

    if(!session)
        return LIBSSH2_ERROR_BAD_USE;

    BLOCK_ADJUST(rc, session,
                 _libssh2_userauth_publickey(session, user, strlen(user),
                                             pubkeydata, pubkeydata_len,
                                             sign_callback, abstract));
    return rc;
}



/*
 * userauth_keyboard_interactive
 *
 * Authenticate using a challenge-response authentication
 */
static int
userauth_keyboard_interactive(LIBSSH2_SESSION * session,
                              const char *username,
                              unsigned int username_len,
                              LIBSSH2_USERAUTH_KBDINT_RESPONSE_FUNC((*response_callback)))
{
    unsigned char *s;
    int rc;

    static const unsigned char reply_codes[4] = {
        SSH_MSG_USERAUTH_SUCCESS,
        SSH_MSG_USERAUTH_FAILURE, SSH_MSG_USERAUTH_INFO_REQUEST, 0
    };
    unsigned int language_tag_len;
    unsigned int i;

    if (session->userauth_kybd_state == libssh2_NB_state_idle) {
        session->userauth_kybd_auth_name = NULL;
        session->userauth_kybd_auth_instruction = NULL;
        session->userauth_kybd_num_prompts = 0;
        session->userauth_kybd_auth_failure = 1;
        session->userauth_kybd_prompts = NULL;
        session->userauth_kybd_responses = NULL;

        /* Zero the whole thing out */
        memset(&session->userauth_kybd_packet_requirev_state, 0,
               sizeof(session->userauth_kybd_packet_requirev_state));

        session->userauth_kybd_packet_len =
            1                   /* byte    SSH_MSG_USERAUTH_REQUEST */
            + 4 + username_len  /* string  user name (ISO-10646 UTF-8, as
                                   defined in [RFC-3629]) */
            + 4 + 14            /* string  service name (US-ASCII) */
            + 4 + 20            /* string  "keyboard-interactive" (US-ASCII) */
            + 4 + 0             /* string  language tag (as defined in
                                   [RFC-3066]) */
            + 4 + 0             /* string  submethods (ISO-10646 UTF-8) */
            ;

        session->userauth_kybd_data = s =
            LIBSSH2_ALLOC(session, session->userauth_kybd_packet_len);
        if (!s) {
            return _libssh2_error(session, LIBSSH2_ERROR_ALLOC,
                                  "Unable to allocate memory for "
                                  "keyboard-interactive authentication");
        }

        *s++ = SSH_MSG_USERAUTH_REQUEST;

        /* user name */
        _libssh2_store_str(&s, username, username_len);

        /* service name */
        _libssh2_store_str(&s, "ssh-connection", sizeof("ssh-connection") - 1);

        /* "keyboard-interactive" */
        _libssh2_store_str(&s, "keyboard-interactive",
                           sizeof("keyboard-interactive") - 1);
        /* language tag */
        _libssh2_store_u32(&s, 0);

        /* submethods */
        _libssh2_store_u32(&s, 0);

        _libssh2_debug(session, LIBSSH2_TRACE_AUTH,
                       "Attempting keyboard-interactive authentication");

        session->userauth_kybd_state = libssh2_NB_state_created;
    }

    if (session->userauth_kybd_state == libssh2_NB_state_created) {
        rc = _libssh2_transport_send(session, session->userauth_kybd_data,
                                     session->userauth_kybd_packet_len,
                                     NULL, 0);
        if (rc == LIBSSH2_ERROR_EAGAIN) {
            return _libssh2_error(session, LIBSSH2_ERROR_EAGAIN, "Would block");
        } else if (rc) {
            LIBSSH2_FREE(session, session->userauth_kybd_data);
            session->userauth_kybd_data = NULL;
            session->userauth_kybd_state = libssh2_NB_state_idle;
            return _libssh2_error(session, LIBSSH2_ERROR_SOCKET_SEND,
                                  "Unable to send keyboard-interactive request");
        }
        LIBSSH2_FREE(session, session->userauth_kybd_data);
        session->userauth_kybd_data = NULL;

        session->userauth_kybd_state = libssh2_NB_state_sent;
    }

    for(;;) {
        if (session->userauth_kybd_state == libssh2_NB_state_sent) {
            rc = _libssh2_packet_requirev(session, reply_codes,
                                          &session->userauth_kybd_data,
                                          &session->userauth_kybd_data_len,
                                          0, NULL, 0,
                                          &session->
                                          userauth_kybd_packet_requirev_state);
            if (rc == LIBSSH2_ERROR_EAGAIN) {
                return _libssh2_error(session, LIBSSH2_ERROR_EAGAIN,
                                      "Would block");
            } else if (rc) {
                session->userauth_kybd_state = libssh2_NB_state_idle;
                return _libssh2_error(session,
                                      LIBSSH2_ERROR_AUTHENTICATION_FAILED,
                                      "Waiting for keyboard USERAUTH response");
            }

            if (session->userauth_kybd_data[0] == SSH_MSG_USERAUTH_SUCCESS) {
                _libssh2_debug(session, LIBSSH2_TRACE_AUTH,
                               "Keyboard-interactive authentication successful");
                LIBSSH2_FREE(session, session->userauth_kybd_data);
                session->userauth_kybd_data = NULL;
                session->state |= LIBSSH2_STATE_AUTHENTICATED;
                session->userauth_kybd_state = libssh2_NB_state_idle;
                return 0;
            }

            if (session->userauth_kybd_data[0] == SSH_MSG_USERAUTH_FAILURE) {
                _libssh2_debug(session, LIBSSH2_TRACE_AUTH,
                               "Keyboard-interactive authentication failed");
                LIBSSH2_FREE(session, session->userauth_kybd_data);
                session->userauth_kybd_data = NULL;
                session->userauth_kybd_state = libssh2_NB_state_idle;
                return _libssh2_error(session,
                                      LIBSSH2_ERROR_AUTHENTICATION_FAILED,
                                      "Authentication failed "
                                      "(keyboard-interactive)");
            }

            /* server requested PAM-like conversation */
            s = session->userauth_kybd_data + 1;

            /* string    name (ISO-10646 UTF-8) */
            session->userauth_kybd_auth_name_len = _libssh2_ntohu32(s);
            s += 4;
            if(session->userauth_kybd_auth_name_len) {
                session->userauth_kybd_auth_name =
                    LIBSSH2_ALLOC(session,
                                  session->userauth_kybd_auth_name_len);
                if (!session->userauth_kybd_auth_name) {
                    _libssh2_error(session, LIBSSH2_ERROR_ALLOC,
                                   "Unable to allocate memory for "
                                   "keyboard-interactive 'name' "
                                   "request field");
                    goto cleanup;
                }
                memcpy(session->userauth_kybd_auth_name, s,
                       session->userauth_kybd_auth_name_len);
                s += session->userauth_kybd_auth_name_len;
            }

            /* string    instruction (ISO-10646 UTF-8) */
            session->userauth_kybd_auth_instruction_len = _libssh2_ntohu32(s);
            s += 4;
            if(session->userauth_kybd_auth_instruction_len) {
                session->userauth_kybd_auth_instruction =
                    LIBSSH2_ALLOC(session,
                                  session->userauth_kybd_auth_instruction_len);
                if (!session->userauth_kybd_auth_instruction) {
                    _libssh2_error(session, LIBSSH2_ERROR_ALLOC,
                                   "Unable to allocate memory for "
                                   "keyboard-interactive 'instruction' "
                                   "request field");
                    goto cleanup;
                }
                memcpy(session->userauth_kybd_auth_instruction, s,
                       session->userauth_kybd_auth_instruction_len);
                s += session->userauth_kybd_auth_instruction_len;
            }

            /* string    language tag (as defined in [RFC-3066]) */
            language_tag_len = _libssh2_ntohu32(s);
            s += 4;

            /* ignoring this field as deprecated */
            s += language_tag_len;

            /* int       num-prompts */
            session->userauth_kybd_num_prompts = _libssh2_ntohu32(s);
            s += 4;

            if(session->userauth_kybd_num_prompts) {
                session->userauth_kybd_prompts =
                    LIBSSH2_ALLOC(session,
                                  sizeof(LIBSSH2_USERAUTH_KBDINT_PROMPT) *
                                  session->userauth_kybd_num_prompts);
                if (!session->userauth_kybd_prompts) {
                    _libssh2_error(session, LIBSSH2_ERROR_ALLOC,
                                   "Unable to allocate memory for "
                                   "keyboard-interactive prompts array");
                    goto cleanup;
                }
                memset(session->userauth_kybd_prompts, 0,
                       sizeof(LIBSSH2_USERAUTH_KBDINT_PROMPT) *
                       session->userauth_kybd_num_prompts);

                session->userauth_kybd_responses =
                    LIBSSH2_ALLOC(session,
                                  sizeof(LIBSSH2_USERAUTH_KBDINT_RESPONSE) *
                                  session->userauth_kybd_num_prompts);
                if (!session->userauth_kybd_responses) {
                    _libssh2_error(session, LIBSSH2_ERROR_ALLOC,
                                   "Unable to allocate memory for "
                                   "keyboard-interactive responses array");
                    goto cleanup;
                }
                memset(session->userauth_kybd_responses, 0,
                       sizeof(LIBSSH2_USERAUTH_KBDINT_RESPONSE) *
                       session->userauth_kybd_num_prompts);

                for(i = 0; i != session->userauth_kybd_num_prompts; ++i) {
                    /* string    prompt[1] (ISO-10646 UTF-8) */
                    session->userauth_kybd_prompts[i].length =
                        _libssh2_ntohu32(s);
                    s += 4;
                    session->userauth_kybd_prompts[i].text =
                        LIBSSH2_ALLOC(session,
                                      session->userauth_kybd_prompts[i].length);
                    if (!session->userauth_kybd_prompts[i].text) {
                        _libssh2_error(session, LIBSSH2_ERROR_ALLOC,
                                       "Unable to allocate memory for "
                                       "keyboard-interactive prompt message");
                        goto cleanup;
                    }
                    memcpy(session->userauth_kybd_prompts[i].text, s,
                           session->userauth_kybd_prompts[i].length);
                    s += session->userauth_kybd_prompts[i].length;

                    /* boolean   echo[1] */
                    session->userauth_kybd_prompts[i].echo = *s++;
                }
            }

            response_callback(session->userauth_kybd_auth_name,
                              session->userauth_kybd_auth_name_len,
                              session->userauth_kybd_auth_instruction,
                              session->userauth_kybd_auth_instruction_len,
                              session->userauth_kybd_num_prompts,
                              session->userauth_kybd_prompts,
                              session->userauth_kybd_responses,
                              &session->abstract);

            _libssh2_debug(session, LIBSSH2_TRACE_AUTH,
                           "Keyboard-interactive response callback function"
                           " invoked");

            session->userauth_kybd_packet_len =
                1 /* byte      SSH_MSG_USERAUTH_INFO_RESPONSE */
                + 4             /* int       num-responses */
                ;

            for(i = 0; i != session->userauth_kybd_num_prompts; ++i) {
                /* string    response[1] (ISO-10646 UTF-8) */
                session->userauth_kybd_packet_len +=
                    4 + session->userauth_kybd_responses[i].length;
            }

            /* A new userauth_kybd_data area is to be allocated, free the
               former one. */
            LIBSSH2_FREE(session, session->userauth_kybd_data);

            session->userauth_kybd_data = s =
                LIBSSH2_ALLOC(session, session->userauth_kybd_packet_len);
            if (!s) {
                _libssh2_error(session, LIBSSH2_ERROR_ALLOC,
                               "Unable to allocate memory for keyboard-"
                               "interactive response packet");
                goto cleanup;
            }

            *s = SSH_MSG_USERAUTH_INFO_RESPONSE;
            s++;
            _libssh2_store_u32(&s, session->userauth_kybd_num_prompts);

            for(i = 0; i != session->userauth_kybd_num_prompts; ++i) {
                _libssh2_store_str(&s,
                                   session->userauth_kybd_responses[i].text,
                                   session->userauth_kybd_responses[i].length);
            }

            session->userauth_kybd_state = libssh2_NB_state_sent1;
        }

        if (session->userauth_kybd_state == libssh2_NB_state_sent1) {
            rc = _libssh2_transport_send(session, session->userauth_kybd_data,
                                         session->userauth_kybd_packet_len,
                                         NULL, 0);
            if (rc == LIBSSH2_ERROR_EAGAIN)
                return _libssh2_error(session, LIBSSH2_ERROR_EAGAIN,
                                      "Would block");
            if (rc) {
                _libssh2_error(session, LIBSSH2_ERROR_SOCKET_SEND,
                               "Unable to send userauth-keyboard-interactive"
                               " request");
                goto cleanup;
            }

            session->userauth_kybd_auth_failure = 0;
        }

      cleanup:
        /*
         * It's safe to clean all the data here, because unallocated pointers
         * are filled by zeroes
         */

        LIBSSH2_FREE(session, session->userauth_kybd_data);
        session->userauth_kybd_data = NULL;

        if (session->userauth_kybd_prompts) {
            for(i = 0; i != session->userauth_kybd_num_prompts; ++i) {
                LIBSSH2_FREE(session, session->userauth_kybd_prompts[i].text);
                session->userauth_kybd_prompts[i].text = NULL;
            }
        }

        if (session->userauth_kybd_responses) {
            for(i = 0; i != session->userauth_kybd_num_prompts; ++i) {
                LIBSSH2_FREE(session,
                             session->userauth_kybd_responses[i].text);
                session->userauth_kybd_responses[i].text = NULL;
            }
        }

        if(session->userauth_kybd_prompts) {
            LIBSSH2_FREE(session, session->userauth_kybd_prompts);
            session->userauth_kybd_prompts = NULL;
        }
        if(session->userauth_kybd_responses) {
            LIBSSH2_FREE(session, session->userauth_kybd_responses);
            session->userauth_kybd_responses = NULL;
        }
        if(session->userauth_kybd_auth_name) {
            LIBSSH2_FREE(session, session->userauth_kybd_auth_name);
            session->userauth_kybd_auth_name = NULL;
        }
        if(session->userauth_kybd_auth_instruction) {
            LIBSSH2_FREE(session, session->userauth_kybd_auth_instruction);
            session->userauth_kybd_auth_instruction = NULL;
        }

        if (session->userauth_kybd_auth_failure) {
            session->userauth_kybd_state = libssh2_NB_state_idle;
            return -1;
        }

        session->userauth_kybd_state = libssh2_NB_state_sent;
    }
}

/*
 * libssh2_userauth_keyboard_interactive_ex
 *
 * Authenticate using a challenge-response authentication
 */
LIBSSH2_API int
libssh2_userauth_keyboard_interactive_ex(LIBSSH2_SESSION *session,
                                         const char *user,
                                         unsigned int user_len,
                                         LIBSSH2_USERAUTH_KBDINT_RESPONSE_FUNC((*response_callback)))
{
    int rc;
    BLOCK_ADJUST(rc, session,
                 userauth_keyboard_interactive(session, user, user_len,
                                               response_callback));
    return rc;
}
