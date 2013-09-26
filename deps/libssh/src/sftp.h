#ifndef _LIBSSH2_SFTP_H
#define _LIBSSH2_SFTP_H
/*
 * Copyright (C) 2010 - 2012 by Daniel Stenberg
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

/*
 * MAX_SFTP_OUTGOING_SIZE MUST not be larger than 32500 or so. This is the
 * amount of data sent in each FXP_WRITE packet
 */
#define MAX_SFTP_OUTGOING_SIZE 30000

/* MAX_SFTP_READ_SIZE is how much data is asked for at max in each FXP_READ
 * packets.
 */
#define MAX_SFTP_READ_SIZE 2000

struct sftp_pipeline_chunk {
    struct list_node node;
    size_t len; /* WRITE: size of the data to write
                   READ: how many bytes that was asked for */
    size_t sent;
    ssize_t lefttosend; /* if 0, the entire packet has been sent off */
    uint32_t request_id;
    unsigned char packet[1]; /* data */
};

struct sftp_zombie_requests {
    struct list_node node;
    uint32_t request_id;
};

#ifndef MIN
#define MIN(x,y) ((x)<(y)?(x):(y))
#endif

struct _LIBSSH2_SFTP_PACKET
{
    struct list_node node;   /* linked list header */
    uint32_t request_id;
    unsigned char *data;
    size_t data_len;              /* payload size */
};

typedef struct _LIBSSH2_SFTP_PACKET LIBSSH2_SFTP_PACKET;

#define SFTP_HANDLE_MAXLEN 256 /* according to spec! */

struct _LIBSSH2_SFTP_HANDLE
{
    struct list_node node;

    LIBSSH2_SFTP *sftp;

    char handle[SFTP_HANDLE_MAXLEN];
    size_t handle_len;

    enum {
        LIBSSH2_SFTP_HANDLE_FILE,
        LIBSSH2_SFTP_HANDLE_DIR
    } handle_type;

    union _libssh2_sftp_handle_data
    {
        struct _libssh2_sftp_handle_file_data
        {
            libssh2_uint64_t offset;
            libssh2_uint64_t offset_sent;
            size_t acked; /* container for acked data that hasn't been
                             returned to caller yet, used for sftp_write */

            /* 'data' is used by sftp_read() and is allocated data that has
               been received already from the server but wasn't returned to
               the caller yet. It is of size 'data_len' and 'data_left is the
               number of bytes not yet returned, counted from the end of the
               buffer. */
            unsigned char *data;
            size_t data_len;
            size_t data_left;

            char eof; /* we have read to the end */
        } file;
        struct _libssh2_sftp_handle_dir_data
        {
            uint32_t names_left;
            void *names_packet;
            char *next_name;
        } dir;
    } u;

    /* State variables used in libssh2_sftp_close_handle() */
    libssh2_nonblocking_states close_state;
    uint32_t close_request_id;
    unsigned char *close_packet;

    /* list of outstanding packets sent to server */
    struct list_head packet_list;

};

struct _LIBSSH2_SFTP
{
    LIBSSH2_CHANNEL *channel;

    uint32_t request_id, version;

    struct list_head packets;

    /* List of FXP_READ responses to ignore because EOF already received. */
    struct list_head zombie_requests;

    /* a list of _LIBSSH2_SFTP_HANDLE structs */
    struct list_head sftp_handles;

    uint32_t last_errno;

    /* Holder for partial packet, use in libssh2_sftp_packet_read() */
    unsigned char partial_size[4];      /* buffer for size field   */
    size_t partial_size_len;            /* size field length       */
    unsigned char *partial_packet;      /* The data                */
    uint32_t partial_len;               /* Desired number of bytes */
    size_t partial_received;            /* Bytes received so far   */

    /* Time that libssh2_sftp_packet_requirev() started reading */
    time_t requirev_start;

    /* State variables used in libssh2_sftp_open_ex() */
    libssh2_nonblocking_states open_state;
    unsigned char *open_packet;
    uint32_t open_packet_len; /* 32 bit on the wire */
    size_t open_packet_sent;
    uint32_t open_request_id;

    /* State variable used in sftp_read() */
    libssh2_nonblocking_states read_state;

    /* State variable used in sftp_packet_read() */
    libssh2_nonblocking_states packet_state;

    /* State variable used in sftp_write() */
    libssh2_nonblocking_states write_state;

    /* State variables used in libssh2_sftp_readdir() */
    libssh2_nonblocking_states readdir_state;
    unsigned char *readdir_packet;
    uint32_t readdir_request_id;

    /* State variables used in libssh2_sftp_fstat_ex() */
    libssh2_nonblocking_states fstat_state;
    unsigned char *fstat_packet;
    uint32_t fstat_request_id;

    /* State variables used in libssh2_sftp_unlink_ex() */
    libssh2_nonblocking_states unlink_state;
    unsigned char *unlink_packet;
    uint32_t unlink_request_id;

    /* State variables used in libssh2_sftp_rename_ex() */
    libssh2_nonblocking_states rename_state;
    unsigned char *rename_packet;
    unsigned char *rename_s;
    uint32_t rename_request_id;

    /* State variables used in libssh2_sftp_fstatvfs() */
    libssh2_nonblocking_states fstatvfs_state;
    unsigned char *fstatvfs_packet;
    uint32_t fstatvfs_request_id;

    /* State variables used in libssh2_sftp_statvfs() */
    libssh2_nonblocking_states statvfs_state;
    unsigned char *statvfs_packet;
    uint32_t statvfs_request_id;

    /* State variables used in libssh2_sftp_mkdir() */
    libssh2_nonblocking_states mkdir_state;
    unsigned char *mkdir_packet;
    uint32_t mkdir_request_id;

    /* State variables used in libssh2_sftp_rmdir() */
    libssh2_nonblocking_states rmdir_state;
    unsigned char *rmdir_packet;
    uint32_t rmdir_request_id;

    /* State variables used in libssh2_sftp_stat() */
    libssh2_nonblocking_states stat_state;
    unsigned char *stat_packet;
    uint32_t stat_request_id;

    /* State variables used in libssh2_sftp_symlink() */
    libssh2_nonblocking_states symlink_state;
    unsigned char *symlink_packet;
    uint32_t symlink_request_id;
};

#endif
