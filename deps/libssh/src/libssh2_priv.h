/* Copyright (c) 2004-2008, 2010, Sara Golemon <sarag@libssh2.org>
 * Copyright (c) 2009-2011 by Daniel Stenberg
 * Copyright (c) 2010 Simon Josefsson
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

#ifndef LIBSSH2_PRIV_H
#define LIBSSH2_PRIV_H 1

#define LIBSSH2_LIBRARY
#include "libssh2_config.h"

#ifdef HAVE_WINDOWS_H
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#undef WIN32_LEAN_AND_MEAN
#endif

#ifdef HAVE_WS2TCPIP_H
#include <ws2tcpip.h>
#endif

#include <stdio.h>
#include <time.h>

/* The following CPP block should really only be in session.c and
   packet.c.  However, AIX have #define's for 'events' and 'revents'
   and we are using those names in libssh2.h, so we need to include
   the AIX headers first, to make sure all code is compiled with
   consistent names of these fields.  While arguable the best would to
   change libssh2.h to use other names, that would break backwards
   compatibility.  For more information, see:
   http://www.mail-archive.com/libssh2-devel%40lists.sourceforge.net/msg00003.html
   http://www.mail-archive.com/libssh2-devel%40lists.sourceforge.net/msg00224.html
*/
#ifdef HAVE_POLL
# include <sys/poll.h>
#else
# if defined(HAVE_SELECT) && !defined(WIN32)
# ifdef HAVE_SYS_SELECT_H
# include <sys/select.h>
# else
# include <sys/time.h>
# include <sys/types.h>
# endif
# endif
#endif

/* Needed for struct iovec on some platforms */
#ifdef HAVE_SYS_UIO_H
#include <sys/uio.h>
#endif

#ifdef HAVE_SYS_SOCKET_H
# include <sys/socket.h>
#endif
#ifdef HAVE_SYS_IOCTL_H
# include <sys/ioctl.h>
#endif
#ifdef HAVE_INTTYPES_H
#include <inttypes.h>
#endif

#include "libssh2.h"
#include "libssh2_publickey.h"
#include "libssh2_sftp.h"
#include "misc.h" /* for the linked list stuff */

#ifndef FALSE
#define FALSE 0
#endif
#ifndef TRUE
#define TRUE 1
#endif

/* Provide iovec / writev on WIN32 platform. */
#ifdef WIN32

struct iovec {
    size_t iov_len;
    void * iov_base;
};

#define inline __inline

static inline int writev(int sock, struct iovec *iov, int nvecs)
{
    DWORD ret;
    if (WSASend(sock, (LPWSABUF)iov, nvecs, &ret, 0, NULL, NULL) == 0) {
        return ret;
    }
    return -1;
}

#endif /* WIN32 */

#include "crypto.h"

#ifdef HAVE_WINSOCK2_H

#include <winsock2.h>
#include <mswsock.h>
#include <ws2tcpip.h>

#ifdef _MSC_VER
/* "inline" keyword is valid only with C++ engine! */
#define inline __inline
#endif

#endif

/* RFC4253 section 6.1 Maximum Packet Length says:
 *
 * "All implementations MUST be able to process packets with
 * uncompressed payload length of 32768 bytes or less and
 * total packet size of 35000 bytes or less (including length,
 * padding length, payload, padding, and MAC.)."
 */
#define MAX_SSH_PACKET_LEN 35000

#define LIBSSH2_ALLOC(session, count) \
  session->alloc((count), &(session)->abstract)
#define LIBSSH2_REALLOC(session, ptr, count) \
 ((ptr) ? session->realloc((ptr), (count), &(session)->abstract) : \
 session->alloc((count), &(session)->abstract))
#define LIBSSH2_FREE(session, ptr) \
 session->free((ptr), &(session)->abstract)
#define LIBSSH2_IGNORE(session, data, datalen) \
 session->ssh_msg_ignore((session), (data), (datalen), &(session)->abstract)
#define LIBSSH2_DEBUG(session, always_display, message, message_len, \
                      language, language_len)    \
    session->ssh_msg_debug((session), (always_display), (message), \
                           (message_len), (language), (language_len), \
                           &(session)->abstract)
#define LIBSSH2_DISCONNECT(session, reason, message, message_len, \
                           language, language_len)                \
    session->ssh_msg_disconnect((session), (reason), (message),   \
                                (message_len), (language), (language_len), \
                                &(session)->abstract)

#define LIBSSH2_MACERROR(session, data, datalen)         \
    session->macerror((session), (data), (datalen), &(session)->abstract)
#define LIBSSH2_X11_OPEN(channel, shost, sport)          \
    channel->session->x11(((channel)->session), (channel), \
                          (shost), (sport), (&(channel)->session->abstract))

#define LIBSSH2_CHANNEL_CLOSE(session, channel)          \
    channel->close_cb((session), &(session)->abstract, \
                      (channel), &(channel)->abstract)

#define LIBSSH2_SEND_FD(session, fd, buffer, length, flags) \
    session->send(fd, buffer, length, flags, &session->abstract)
#define LIBSSH2_RECV_FD(session, fd, buffer, length, flags) \
    session->recv(fd, buffer, length, flags, &session->abstract)

#define LIBSSH2_SEND(session, buffer, length, flags)  \
    LIBSSH2_SEND_FD(session, session->socket_fd, buffer, length, flags)
#define LIBSSH2_RECV(session, buffer, length, flags)                    \
    LIBSSH2_RECV_FD(session, session->socket_fd, buffer, length, flags)

typedef struct _LIBSSH2_KEX_METHOD LIBSSH2_KEX_METHOD;
typedef struct _LIBSSH2_HOSTKEY_METHOD LIBSSH2_HOSTKEY_METHOD;
typedef struct _LIBSSH2_CRYPT_METHOD LIBSSH2_CRYPT_METHOD;
typedef struct _LIBSSH2_COMP_METHOD LIBSSH2_COMP_METHOD;

typedef struct _LIBSSH2_PACKET LIBSSH2_PACKET;

typedef enum
{
    libssh2_NB_state_idle = 0,
    libssh2_NB_state_allocated,
    libssh2_NB_state_created,
    libssh2_NB_state_sent,
    libssh2_NB_state_sent1,
    libssh2_NB_state_sent2,
    libssh2_NB_state_sent3,
    libssh2_NB_state_sent4,
    libssh2_NB_state_sent5,
    libssh2_NB_state_sent6,
    libssh2_NB_state_sent7,
    libssh2_NB_state_jump1,
    libssh2_NB_state_jump2,
    libssh2_NB_state_jump3,
    libssh2_NB_state_jump4,
    libssh2_NB_state_jump5
} libssh2_nonblocking_states;

typedef struct packet_require_state_t
{
    libssh2_nonblocking_states state;
    time_t start;
} packet_require_state_t;

typedef struct packet_requirev_state_t
{
    time_t start;
} packet_requirev_state_t;

typedef struct kmdhgGPsha1kex_state_t
{
    libssh2_nonblocking_states state;
    unsigned char *e_packet;
    unsigned char *s_packet;
    unsigned char *tmp;
    unsigned char h_sig_comp[SHA_DIGEST_LENGTH];
    unsigned char c;
    size_t e_packet_len;
    size_t s_packet_len;
    size_t tmp_len;
    _libssh2_bn_ctx *ctx;
    _libssh2_bn *x;
    _libssh2_bn *e;
    _libssh2_bn *f;
    _libssh2_bn *k;
    unsigned char *s;
    unsigned char *f_value;
    unsigned char *k_value;
    unsigned char *h_sig;
    size_t f_value_len;
    size_t k_value_len;
    size_t h_sig_len;
    libssh2_sha1_ctx exchange_hash;
    packet_require_state_t req_state;
    libssh2_nonblocking_states burn_state;
} kmdhgGPsha1kex_state_t;

typedef struct key_exchange_state_low_t
{
    libssh2_nonblocking_states state;
    packet_require_state_t req_state;
    kmdhgGPsha1kex_state_t exchange_state;
    _libssh2_bn *p;             /* SSH2 defined value (p_value) */
    _libssh2_bn *g;             /* SSH2 defined value (2) */
    unsigned char request[13];
    unsigned char *data;
    size_t request_len;
    size_t data_len;
} key_exchange_state_low_t;

typedef struct key_exchange_state_t
{
    libssh2_nonblocking_states state;
    packet_require_state_t req_state;
    key_exchange_state_low_t key_state_low;
    unsigned char *data;
    size_t data_len;
    unsigned char *oldlocal;
    size_t oldlocal_len;
} key_exchange_state_t;

#define FwdNotReq "Forward not requested"

typedef struct packet_queue_listener_state_t
{
    libssh2_nonblocking_states state;
    unsigned char packet[17 + (sizeof(FwdNotReq) - 1)];
    unsigned char *host;
    unsigned char *shost;
    uint32_t sender_channel;
    uint32_t initial_window_size;
    uint32_t packet_size;
    uint32_t port;
    uint32_t sport;
    uint32_t host_len;
    uint32_t shost_len;
    LIBSSH2_CHANNEL *channel;
} packet_queue_listener_state_t;

#define X11FwdUnAvil "X11 Forward Unavailable"

typedef struct packet_x11_open_state_t
{
    libssh2_nonblocking_states state;
    unsigned char packet[17 + (sizeof(X11FwdUnAvil) - 1)];
    unsigned char *shost;
    uint32_t sender_channel;
    uint32_t initial_window_size;
    uint32_t packet_size;
    uint32_t sport;
    uint32_t shost_len;
    LIBSSH2_CHANNEL *channel;
} packet_x11_open_state_t;

struct _LIBSSH2_PACKET
{
    struct list_node node; /* linked list header */

    /* the raw unencrypted payload */
    unsigned char *data;
    size_t data_len;

    /* Where to start reading data from,
     * used for channel data that's been partially consumed */
    size_t data_head;
};

typedef struct _libssh2_channel_data
{
    /* Identifier */
    uint32_t id;

    /* Limits and restrictions */
    uint32_t window_size_initial, window_size, packet_size;

    /* Set to 1 when CHANNEL_CLOSE / CHANNEL_EOF sent/received */
    char close, eof, extended_data_ignore_mode;
} libssh2_channel_data;

struct _LIBSSH2_CHANNEL
{
    struct list_node node;

    unsigned char *channel_type;
    unsigned channel_type_len;

    /* channel's program exit status */
    int exit_status;

    /* channel's program exit signal (without the SIG prefix) */
    char *exit_signal;

    libssh2_channel_data local, remote;
    /* Amount of bytes to be refunded to receive window (but not yet sent) */
    uint32_t adjust_queue;

    LIBSSH2_SESSION *session;

    void *abstract;
      LIBSSH2_CHANNEL_CLOSE_FUNC((*close_cb));

    /* State variables used in libssh2_channel_setenv_ex() */
    libssh2_nonblocking_states setenv_state;
    unsigned char *setenv_packet;
    size_t setenv_packet_len;
    unsigned char setenv_local_channel[4];
    packet_requirev_state_t setenv_packet_requirev_state;

    /* State variables used in libssh2_channel_request_pty_ex()
       libssh2_channel_request_pty_size_ex() */
    libssh2_nonblocking_states reqPTY_state;
    unsigned char reqPTY_packet[41 + 256];
    size_t reqPTY_packet_len;
    unsigned char reqPTY_local_channel[4];
    packet_requirev_state_t reqPTY_packet_requirev_state;

    /* State variables used in libssh2_channel_x11_req_ex() */
    libssh2_nonblocking_states reqX11_state;
    unsigned char *reqX11_packet;
    size_t reqX11_packet_len;
    unsigned char reqX11_local_channel[4];
    packet_requirev_state_t reqX11_packet_requirev_state;

    /* State variables used in libssh2_channel_process_startup() */
    libssh2_nonblocking_states process_state;
    unsigned char *process_packet;
    size_t process_packet_len;
    unsigned char process_local_channel[4];
    packet_requirev_state_t process_packet_requirev_state;

    /* State variables used in libssh2_channel_flush_ex() */
    libssh2_nonblocking_states flush_state;
    size_t flush_refund_bytes;
    size_t flush_flush_bytes;

    /* State variables used in libssh2_channel_receive_window_adjust() */
    libssh2_nonblocking_states adjust_state;
    unsigned char adjust_adjust[9];     /* packet_type(1) + channel(4) + adjustment(4) */

    /* State variables used in libssh2_channel_read_ex() */
    libssh2_nonblocking_states read_state;

    uint32_t read_local_id;

    /* State variables used in libssh2_channel_write_ex() */
    libssh2_nonblocking_states write_state;
    unsigned char write_packet[13];
    size_t write_packet_len;
    size_t write_bufwrite;

    /* State variables used in libssh2_channel_close() */
    libssh2_nonblocking_states close_state;
    unsigned char close_packet[5];

    /* State variables used in libssh2_channel_wait_closedeof() */
    libssh2_nonblocking_states wait_eof_state;

    /* State variables used in libssh2_channel_wait_closed() */
    libssh2_nonblocking_states wait_closed_state;

    /* State variables used in libssh2_channel_free() */
    libssh2_nonblocking_states free_state;

    /* State variables used in libssh2_channel_handle_extended_data2() */
    libssh2_nonblocking_states extData2_state;

};

struct _LIBSSH2_LISTENER
{
    struct list_node node; /* linked list header */

    LIBSSH2_SESSION *session;

    char *host;
    int port;

    /* a list of CHANNELs for this listener */
    struct list_head queue;

    int queue_size;
    int queue_maxsize;

    /* State variables used in libssh2_channel_forward_cancel() */
    libssh2_nonblocking_states chanFwdCncl_state;
    unsigned char *chanFwdCncl_data;
    size_t chanFwdCncl_data_len;
};

typedef struct _libssh2_endpoint_data
{
    unsigned char *banner;

    unsigned char *kexinit;
    size_t kexinit_len;

    const LIBSSH2_CRYPT_METHOD *crypt;
    void *crypt_abstract;

    const struct _LIBSSH2_MAC_METHOD *mac;
    uint32_t seqno;
    void *mac_abstract;

    const LIBSSH2_COMP_METHOD *comp;
    void *comp_abstract;

    /* Method Preferences -- NULL yields "load order" */
    char *crypt_prefs;
    char *mac_prefs;
    char *comp_prefs;
    char *lang_prefs;
} libssh2_endpoint_data;

#define PACKETBUFSIZE (1024*16)

struct transportpacket
{
    /* ------------- for incoming data --------------- */
    unsigned char buf[PACKETBUFSIZE];
    unsigned char init[5];  /* first 5 bytes of the incoming data stream,
                               still encrypted */
    size_t writeidx;        /* at what array index we do the next write into
                               the buffer */
    size_t readidx;         /* at what array index we do the next read from
                               the buffer */
    uint32_t packet_length; /* the most recent packet_length as read from the
                               network data */
    uint8_t padding_length; /* the most recent padding_length as read from the
                               network data */
    size_t data_num;        /* How much of the total package that has been read
                               so far. */
    size_t total_num;       /* How much a total package is supposed to be, in
                               number of bytes. A full package is
                               packet_length + padding_length + 4 +
                               mac_length. */
    unsigned char *payload; /* this is a pointer to a LIBSSH2_ALLOC()
                               area to which we write decrypted data */
    unsigned char *wptr;    /* write pointer into the payload to where we
                               are currently writing decrypted data */

    /* ------------- for outgoing data --------------- */
    unsigned char outbuf[MAX_SSH_PACKET_LEN]; /* area for the outgoing data */

    int ototal_num;         /* size of outbuf in number of bytes */
    const unsigned char *odata; /* original pointer to the data */
    size_t olen;            /* original size of the data we stored in
                               outbuf */
    size_t osent;           /* number of bytes already sent */
};

struct _LIBSSH2_PUBLICKEY
{
    LIBSSH2_CHANNEL *channel;
    uint32_t version;

    /* State variables used in libssh2_publickey_packet_receive() */
    libssh2_nonblocking_states receive_state;
    unsigned char *receive_packet;
    size_t receive_packet_len;

    /* State variables used in libssh2_publickey_add_ex() */
    libssh2_nonblocking_states add_state;
    unsigned char *add_packet;
    unsigned char *add_s;

    /* State variables used in libssh2_publickey_remove_ex() */
    libssh2_nonblocking_states remove_state;
    unsigned char *remove_packet;
    unsigned char *remove_s;

    /* State variables used in libssh2_publickey_list_fetch() */
    libssh2_nonblocking_states listFetch_state;
    unsigned char *listFetch_s;
    unsigned char listFetch_buffer[12];
    unsigned char *listFetch_data;
    size_t listFetch_data_len;
};

#define LIBSSH2_SCP_RESPONSE_BUFLEN     256

struct flags {
    int sigpipe;  /* LIBSSH2_FLAG_SIGPIPE */
    int compress; /* LIBSSH2_FLAG_COMPRESS */
};

struct _LIBSSH2_SESSION
{
    /* Memory management callbacks */
    void *abstract;
      LIBSSH2_ALLOC_FUNC((*alloc));
      LIBSSH2_REALLOC_FUNC((*realloc));
      LIBSSH2_FREE_FUNC((*free));

    /* Other callbacks */
      LIBSSH2_IGNORE_FUNC((*ssh_msg_ignore));
      LIBSSH2_DEBUG_FUNC((*ssh_msg_debug));
      LIBSSH2_DISCONNECT_FUNC((*ssh_msg_disconnect));
      LIBSSH2_MACERROR_FUNC((*macerror));
      LIBSSH2_X11_OPEN_FUNC((*x11));
      LIBSSH2_SEND_FUNC((*send));
      LIBSSH2_RECV_FUNC((*recv));

    /* Method preferences -- NULL yields "load order" */
    char *kex_prefs;
    char *hostkey_prefs;

    int state;

    /* Flag options */
    struct flags flag;

    /* Agreed Key Exchange Method */
    const LIBSSH2_KEX_METHOD *kex;
    int burn_optimistic_kexinit:1;

    unsigned char *session_id;
    uint32_t session_id_len;

    /* this is set to TRUE if a blocking API behavior is requested */
    int api_block_mode;

    /* Timeout used when blocking API behavior is active */
    long api_timeout;

    /* Server's public key */
    const LIBSSH2_HOSTKEY_METHOD *hostkey;
    void *server_hostkey_abstract;

    /* Either set with libssh2_session_hostkey() (for server mode)
     * Or read from server in (eg) KEXDH_INIT (for client mode)
     */
    unsigned char *server_hostkey;
    uint32_t server_hostkey_len;
#if LIBSSH2_MD5
    unsigned char server_hostkey_md5[MD5_DIGEST_LENGTH];
    int server_hostkey_md5_valid;
#endif                          /* ! LIBSSH2_MD5 */
    unsigned char server_hostkey_sha1[SHA_DIGEST_LENGTH];

    /* (remote as source of data -- packet_read ) */
    libssh2_endpoint_data remote;

    /* (local as source of data -- packet_write ) */
    libssh2_endpoint_data local;

    /* Inbound Data linked list -- Sometimes the packet that comes in isn't the
       packet we're ready for */
    struct list_head packets;

    /* Active connection channels */
    struct list_head channels;

    uint32_t next_channel;

    struct list_head listeners; /* list of LIBSSH2_LISTENER structs */

    /* Actual I/O socket */
    libssh2_socket_t socket_fd;
    int socket_state;
    int socket_block_directions;
    int socket_prev_blockstate; /* stores the state of the socket blockiness
                                   when libssh2_session_startup() is called */

    /* Error tracking */
    const char *err_msg;
    int err_code;

    /* struct members for packet-level reading */
    struct transportpacket packet;
#ifdef LIBSSH2DEBUG
    int showmask;               /* what debug/trace messages to display */
    libssh2_trace_handler_func tracehandler; /* callback to display trace messages */
    void* tracehandler_context; /* context for the trace handler */
#endif

    /* State variables used in libssh2_banner_send() */
    libssh2_nonblocking_states banner_TxRx_state;
    char banner_TxRx_banner[256];
    ssize_t banner_TxRx_total_send;

    /* State variables used in libssh2_kexinit() */
    libssh2_nonblocking_states kexinit_state;
    unsigned char *kexinit_data;
    size_t kexinit_data_len;

    /* State variables used in libssh2_session_startup() */
    libssh2_nonblocking_states startup_state;
    unsigned char *startup_data;
    size_t startup_data_len;
    unsigned char startup_service[sizeof("ssh-userauth") + 5 - 1];
    size_t startup_service_length;
    packet_require_state_t startup_req_state;
    key_exchange_state_t startup_key_state;

    /* State variables used in libssh2_session_free() */
    libssh2_nonblocking_states free_state;

    /* State variables used in libssh2_session_disconnect_ex() */
    libssh2_nonblocking_states disconnect_state;
    unsigned char disconnect_data[256 + 13];
    size_t disconnect_data_len;

    /* State variables used in libssh2_packet_read() */
    libssh2_nonblocking_states readPack_state;
    int readPack_encrypted;

    /* State variables used in libssh2_userauth_list() */
    libssh2_nonblocking_states userauth_list_state;
    unsigned char *userauth_list_data;
    size_t userauth_list_data_len;
    packet_requirev_state_t userauth_list_packet_requirev_state;

    /* State variables used in libssh2_userauth_password_ex() */
    libssh2_nonblocking_states userauth_pswd_state;
    unsigned char *userauth_pswd_data;
    unsigned char userauth_pswd_data0;
    size_t userauth_pswd_data_len;
    char *userauth_pswd_newpw;
    int userauth_pswd_newpw_len;
    packet_requirev_state_t userauth_pswd_packet_requirev_state;

    /* State variables used in libssh2_userauth_hostbased_fromfile_ex() */
    libssh2_nonblocking_states userauth_host_state;
    unsigned char *userauth_host_data;
    size_t userauth_host_data_len;
    unsigned char *userauth_host_packet;
    size_t userauth_host_packet_len;
    unsigned char *userauth_host_method;
    size_t userauth_host_method_len;
    unsigned char *userauth_host_s;
    packet_requirev_state_t userauth_host_packet_requirev_state;

    /* State variables used in libssh2_userauth_publickey_fromfile_ex() */
    libssh2_nonblocking_states userauth_pblc_state;
    unsigned char *userauth_pblc_data;
    size_t userauth_pblc_data_len;
    unsigned char *userauth_pblc_packet;
    size_t userauth_pblc_packet_len;
    unsigned char *userauth_pblc_method;
    size_t userauth_pblc_method_len;
    unsigned char *userauth_pblc_s;
    unsigned char *userauth_pblc_b;
    packet_requirev_state_t userauth_pblc_packet_requirev_state;

    /* State variables used in libssh2_userauth_keyboard_interactive_ex() */
    libssh2_nonblocking_states userauth_kybd_state;
    unsigned char *userauth_kybd_data;
    size_t userauth_kybd_data_len;
    unsigned char *userauth_kybd_packet;
    size_t userauth_kybd_packet_len;
    unsigned int userauth_kybd_auth_name_len;
    char *userauth_kybd_auth_name;
    unsigned userauth_kybd_auth_instruction_len;
    char *userauth_kybd_auth_instruction;
    unsigned int userauth_kybd_num_prompts;
    int userauth_kybd_auth_failure;
    LIBSSH2_USERAUTH_KBDINT_PROMPT *userauth_kybd_prompts;
    LIBSSH2_USERAUTH_KBDINT_RESPONSE *userauth_kybd_responses;
    packet_requirev_state_t userauth_kybd_packet_requirev_state;

    /* State variables used in libssh2_channel_open_ex() */
    libssh2_nonblocking_states open_state;
    packet_requirev_state_t open_packet_requirev_state;
    LIBSSH2_CHANNEL *open_channel;
    unsigned char *open_packet;
    size_t open_packet_len;
    unsigned char *open_data;
    size_t open_data_len;
    uint32_t open_local_channel;

    /* State variables used in libssh2_channel_direct_tcpip_ex() */
    libssh2_nonblocking_states direct_state;
    unsigned char *direct_message;
    size_t direct_host_len;
    size_t direct_shost_len;
    size_t direct_message_len;

    /* State variables used in libssh2_channel_forward_listen_ex() */
    libssh2_nonblocking_states fwdLstn_state;
    unsigned char *fwdLstn_packet;
    uint32_t fwdLstn_host_len;
    uint32_t fwdLstn_packet_len;
    packet_requirev_state_t fwdLstn_packet_requirev_state;

    /* State variables used in libssh2_publickey_init() */
    libssh2_nonblocking_states pkeyInit_state;
    LIBSSH2_PUBLICKEY *pkeyInit_pkey;
    LIBSSH2_CHANNEL *pkeyInit_channel;
    unsigned char *pkeyInit_data;
    size_t pkeyInit_data_len;
    /* 19 = packet_len(4) + version_len(4) + "version"(7) + version_num(4) */
    unsigned char pkeyInit_buffer[19];
    size_t pkeyInit_buffer_sent; /* how much of buffer that has been sent */

    /* State variables used in libssh2_packet_add() */
    libssh2_nonblocking_states packAdd_state;
    LIBSSH2_CHANNEL *packAdd_channelp; /* keeper of the channel during EAGAIN
                                          states */
    packet_queue_listener_state_t packAdd_Qlstn_state;
    packet_x11_open_state_t packAdd_x11open_state;

    /* State variables used in fullpacket() */
    libssh2_nonblocking_states fullpacket_state;
    int fullpacket_macstate;
    size_t fullpacket_payload_len;
    int fullpacket_packet_type;

    /* State variables used in libssh2_sftp_init() */
    libssh2_nonblocking_states sftpInit_state;
    LIBSSH2_SFTP *sftpInit_sftp;
    LIBSSH2_CHANNEL *sftpInit_channel;
    unsigned char sftpInit_buffer[9];   /* sftp_header(5){excludes request_id}
                                           + version_id(4) */
    int sftpInit_sent; /* number of bytes from the buffer that have been
                          sent */

    /* State variables used in libssh2_scp_recv() */
    libssh2_nonblocking_states scpRecv_state;
    unsigned char *scpRecv_command;
    size_t scpRecv_command_len;
    unsigned char scpRecv_response[LIBSSH2_SCP_RESPONSE_BUFLEN];
    size_t scpRecv_response_len;
    long scpRecv_mode;
#if defined(HAVE_LONGLONG) && defined(HAVE_STRTOLL)
    /* we have the type and we can parse such numbers */
    long long scpRecv_size;
#define scpsize_strtol strtoll
#else
    long scpRecv_size;
#define scpsize_strtol strtol
#endif
    long scpRecv_mtime;
    long scpRecv_atime;
    LIBSSH2_CHANNEL *scpRecv_channel;

    /* State variables used in libssh2_scp_send_ex() */
    libssh2_nonblocking_states scpSend_state;
    unsigned char *scpSend_command;
    size_t scpSend_command_len;
    unsigned char scpSend_response[LIBSSH2_SCP_RESPONSE_BUFLEN];
    size_t scpSend_response_len;
    LIBSSH2_CHANNEL *scpSend_channel;

    /* Keepalive variables used by keepalive.c. */
    int keepalive_interval;
    int keepalive_want_reply;
    time_t keepalive_last_sent;
};

/* session.state bits */
#define LIBSSH2_STATE_EXCHANGING_KEYS   0x00000001
#define LIBSSH2_STATE_NEWKEYS           0x00000002
#define LIBSSH2_STATE_AUTHENTICATED     0x00000004
#define LIBSSH2_STATE_KEX_ACTIVE        0x00000008

/* session.flag helpers */
#ifdef MSG_NOSIGNAL
#define LIBSSH2_SOCKET_SEND_FLAGS(session)              \
    (((session)->flag.sigpipe) ? 0 : MSG_NOSIGNAL)
#define LIBSSH2_SOCKET_RECV_FLAGS(session)              \
    (((session)->flag.sigpipe) ? 0 : MSG_NOSIGNAL)
#else
/* If MSG_NOSIGNAL isn't defined we're SOL on blocking SIGPIPE */
#define LIBSSH2_SOCKET_SEND_FLAGS(session)      0
#define LIBSSH2_SOCKET_RECV_FLAGS(session)      0
#endif

/* --------- */

/* libssh2 extensible ssh api, ultimately I'd like to allow loading additional
   methods via .so/.dll */

struct _LIBSSH2_KEX_METHOD
{
    const char *name;

    /* Key exchange, populates session->* and returns 0 on success, non-0 on error */
    int (*exchange_keys) (LIBSSH2_SESSION * session,
                          key_exchange_state_low_t * key_state);

    long flags;
};

struct _LIBSSH2_HOSTKEY_METHOD
{
    const char *name;
    unsigned long hash_len;

    int (*init) (LIBSSH2_SESSION * session, const unsigned char *hostkey_data,
                 size_t hostkey_data_len, void **abstract);
    int (*initPEM) (LIBSSH2_SESSION * session, const char *privkeyfile,
                    unsigned const char *passphrase, void **abstract);
    int (*sig_verify) (LIBSSH2_SESSION * session, const unsigned char *sig,
                       size_t sig_len, const unsigned char *m,
                       size_t m_len, void **abstract);
    int (*signv) (LIBSSH2_SESSION * session, unsigned char **signature,
                  size_t *signature_len, int veccount,
                  const struct iovec datavec[], void **abstract);
    int (*encrypt) (LIBSSH2_SESSION * session, unsigned char **dst,
                    size_t *dst_len, const unsigned char *src,
                    size_t src_len, void **abstract);
    int (*dtor) (LIBSSH2_SESSION * session, void **abstract);
};

struct _LIBSSH2_CRYPT_METHOD
{
    const char *name;

    int blocksize;

    /* iv and key sizes (-1 for variable length) */
    int iv_len;
    int secret_len;

    long flags;

    int (*init) (LIBSSH2_SESSION * session,
                 const LIBSSH2_CRYPT_METHOD * method, unsigned char *iv,
                 int *free_iv, unsigned char *secret, int *free_secret,
                 int encrypt, void **abstract);
    int (*crypt) (LIBSSH2_SESSION * session, unsigned char *block,
                  size_t blocksize, void **abstract);
    int (*dtor) (LIBSSH2_SESSION * session, void **abstract);

      _libssh2_cipher_type(algo);
};

struct _LIBSSH2_COMP_METHOD
{
    const char *name;
    int compress; /* 1 if it does compress, 0 if it doesn't */
    int use_in_auth; /* 1 if compression should be used in userauth */
    int (*init) (LIBSSH2_SESSION *session, int compress, void **abstract);
    int (*comp) (LIBSSH2_SESSION *session,
                 unsigned char *dest,
                 size_t *dest_len,
                 const unsigned char *src,
                 size_t src_len,
                 void **abstract);
    int (*decomp) (LIBSSH2_SESSION *session,
                   unsigned char **dest,
                   size_t *dest_len,
                   size_t payload_limit,
                   const unsigned char *src,
                   size_t src_len,
                   void **abstract);
    int (*dtor) (LIBSSH2_SESSION * session, int compress, void **abstract);
};

#ifdef LIBSSH2DEBUG
void _libssh2_debug(LIBSSH2_SESSION * session, int context, const char *format,
                    ...);
#else
#if (defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901L)) || defined(__GNUC__)
/* C99 supported and also by older GCC */
#define _libssh2_debug(x,y,z,...) do {} while (0)
#else
/* no gcc and not C99, do static and hopefully inline */
static inline void
_libssh2_debug(LIBSSH2_SESSION * session, int context, const char *format, ...)
{
}
#endif
#endif

#define LIBSSH2_SOCKET_UNKNOWN                   1
#define LIBSSH2_SOCKET_CONNECTED                 0
#define LIBSSH2_SOCKET_DISCONNECTED             -1

/* Initial packet state, prior to MAC check */
#define LIBSSH2_MAC_UNCONFIRMED                  1
/* When MAC type is "none" (proto initiation phase) all packets are deemed "confirmed" */
#define LIBSSH2_MAC_CONFIRMED                    0
/* Something very bad is going on */
#define LIBSSH2_MAC_INVALID                     -1

/* SSH Packet Types -- Defined by internet draft */
/* Transport Layer */
#define SSH_MSG_DISCONNECT                          1
#define SSH_MSG_IGNORE                              2
#define SSH_MSG_UNIMPLEMENTED                       3
#define SSH_MSG_DEBUG                               4
#define SSH_MSG_SERVICE_REQUEST                     5
#define SSH_MSG_SERVICE_ACCEPT                      6

#define SSH_MSG_KEXINIT                             20
#define SSH_MSG_NEWKEYS                             21

/* diffie-hellman-group1-sha1 */
#define SSH_MSG_KEXDH_INIT                          30
#define SSH_MSG_KEXDH_REPLY                         31

/* diffie-hellman-group-exchange-sha1 */
#define SSH_MSG_KEX_DH_GEX_REQUEST_OLD              30
#define SSH_MSG_KEX_DH_GEX_REQUEST                  34
#define SSH_MSG_KEX_DH_GEX_GROUP                    31
#define SSH_MSG_KEX_DH_GEX_INIT                     32
#define SSH_MSG_KEX_DH_GEX_REPLY                    33

/* User Authentication */
#define SSH_MSG_USERAUTH_REQUEST                    50
#define SSH_MSG_USERAUTH_FAILURE                    51
#define SSH_MSG_USERAUTH_SUCCESS                    52
#define SSH_MSG_USERAUTH_BANNER                     53

/* "public key" method */
#define SSH_MSG_USERAUTH_PK_OK                      60
/* "password" method */
#define SSH_MSG_USERAUTH_PASSWD_CHANGEREQ           60
/* "keyboard-interactive" method */
#define SSH_MSG_USERAUTH_INFO_REQUEST               60
#define SSH_MSG_USERAUTH_INFO_RESPONSE              61

/* Channels */
#define SSH_MSG_GLOBAL_REQUEST                      80
#define SSH_MSG_REQUEST_SUCCESS                     81
#define SSH_MSG_REQUEST_FAILURE                     82

#define SSH_MSG_CHANNEL_OPEN                        90
#define SSH_MSG_CHANNEL_OPEN_CONFIRMATION           91
#define SSH_MSG_CHANNEL_OPEN_FAILURE                92
#define SSH_MSG_CHANNEL_WINDOW_ADJUST               93
#define SSH_MSG_CHANNEL_DATA                        94
#define SSH_MSG_CHANNEL_EXTENDED_DATA               95
#define SSH_MSG_CHANNEL_EOF                         96
#define SSH_MSG_CHANNEL_CLOSE                       97
#define SSH_MSG_CHANNEL_REQUEST                     98
#define SSH_MSG_CHANNEL_SUCCESS                     99
#define SSH_MSG_CHANNEL_FAILURE                     100

/* Error codes returned in SSH_MSG_CHANNEL_OPEN_FAILURE message
   (see RFC4254) */
#define SSH_OPEN_ADMINISTRATIVELY_PROHIBITED 1
#define SSH_OPEN_CONNECT_FAILED              2
#define SSH_OPEN_UNKNOWN_CHANNELTYPE         3
#define SSH_OPEN_RESOURCE_SHORTAGE           4

ssize_t _libssh2_recv(libssh2_socket_t socket, void *buffer,
                      size_t length, int flags, void **abstract);
ssize_t _libssh2_send(libssh2_socket_t socket, const void *buffer,
                      size_t length, int flags, void **abstract);

#define LIBSSH2_READ_TIMEOUT 60 /* generic timeout in seconds used when
                                   waiting for more data to arrive */


int _libssh2_kex_exchange(LIBSSH2_SESSION * session, int reexchange,
                          key_exchange_state_t * state);

/* Let crypt.c/hostkey.c expose their method structs */
const LIBSSH2_CRYPT_METHOD **libssh2_crypt_methods(void);
const LIBSSH2_HOSTKEY_METHOD **libssh2_hostkey_methods(void);

/* pem.c */
int _libssh2_pem_parse(LIBSSH2_SESSION * session,
                       const char *headerbegin,
                       const char *headerend,
                       FILE * fp, unsigned char **data, unsigned int *datalen);
int _libssh2_pem_decode_sequence(unsigned char **data, unsigned int *datalen);
int _libssh2_pem_decode_integer(unsigned char **data, unsigned int *datalen,
                                unsigned char **i, unsigned int *ilen);

/* global.c */
void _libssh2_init_if_needed (void);


#define ARRAY_SIZE(a) (sizeof ((a)) / sizeof ((a)[0]))

/* define to output the libssh2_int64_t type in a *printf() */
#if defined( __BORLANDC__ ) || defined( _MSC_VER ) || defined( __MINGW32__ )
#define LIBSSH2_INT64_T_FORMAT "I64d"
#else
#define LIBSSH2_INT64_T_FORMAT "lld"
#endif

#endif /* LIBSSH2_H */
