/*
 * Sample showing how to do SFTP transfers in a non-blocking manner.
 *
 * It will first download a given source file, store it locally and then
 * upload the file again to a given destination file.
 *
 * Using the SFTP server running on 127.0.0.1
 */

#include "libssh2_config.h"
#include <libssh2.h>
#include <libssh2_sftp.h>

#ifdef HAVE_WINSOCK2_H
# include <winsock2.h>
#endif
#ifdef HAVE_SYS_SOCKET_H
# include <sys/socket.h>
#endif
#ifdef HAVE_NETINET_IN_H
# include <netinet/in.h>
#endif
#ifdef HAVE_SYS_SELECT_H
# include <sys/select.h>
#endif
# ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#ifdef HAVE_ARPA_INET_H
# include <arpa/inet.h>
#endif
#ifdef HAVE_SYS_TIME_H
# include <sys/time.h>
#endif

#include <sys/types.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <ctype.h>

#define STORAGE "/tmp/sftp-storage" /* this is the local file name this
                                       example uses to store the downloaded
                                       file in */

static int waitsocket(int socket_fd, LIBSSH2_SESSION *session)
{
    struct timeval timeout;
    int rc;
    fd_set fd;
    fd_set *writefd = NULL;
    fd_set *readfd = NULL;
    int dir;

    timeout.tv_sec = 10;
    timeout.tv_usec = 0;

    FD_ZERO(&fd);

    FD_SET(socket_fd, &fd);

    /* now make sure we wait in the correct direction */
    dir = libssh2_session_block_directions(session);

    if(dir & LIBSSH2_SESSION_BLOCK_INBOUND)
        readfd = &fd;

    if(dir & LIBSSH2_SESSION_BLOCK_OUTBOUND)
        writefd = &fd;

    rc = select(socket_fd + 1, readfd, writefd, NULL, &timeout);

    return rc;
}

int main(int argc, char *argv[])
{
    int sock, i, auth_pw = 1;
    struct sockaddr_in sin;
    const char *fingerprint;
    LIBSSH2_SESSION *session;
    const char *username="username";
    const char *password="password";
    const char *sftppath="/tmp/TEST"; /* source path */
    const char *dest="/tmp/TEST2";    /* destination path */
    int rc;
    LIBSSH2_SFTP *sftp_session;
    LIBSSH2_SFTP_HANDLE *sftp_handle;
    FILE *tempstorage;
    char mem[1000];
    struct timeval timeout;
    fd_set fd;

#ifdef WIN32
    WSADATA wsadata;

    WSAStartup(MAKEWORD(2,0), &wsadata);
#endif

    rc = libssh2_init (0);
    if (rc != 0) {
        fprintf (stderr, "libssh2 initialization failed (%d)\n", rc);
        return 1;
    }

    /* Ultra basic "connect to port 22 on localhost"
     * The application is responsible for creating the socket establishing
     * the connection
     */
    sock = socket(AF_INET, SOCK_STREAM, 0);

    sin.sin_family = AF_INET;
    sin.sin_port = htons(22);
    sin.sin_addr.s_addr = htonl(0x7F000001);
    if (connect(sock, (struct sockaddr*)(&sin),
                sizeof(struct sockaddr_in)) != 0) {
        fprintf(stderr, "failed to connect!\n");
        return -1;
    }

    /* Create a session instance
     */
    session = libssh2_session_init();
    if(!session)
        return -1;

    /* ... start it up. This will trade welcome banners, exchange keys,
     * and setup crypto, compression, and MAC layers
     */
    rc = libssh2_session_handshake(session, sock);
    if(rc) {
        fprintf(stderr, "Failure establishing SSH session: %d\n", rc);
        return -1;
    }

    libssh2_session_set_blocking(session, 0);

    /* At this point we havn't yet authenticated.  The first thing to do
     * is check the hostkey's fingerprint against our known hosts Your app
     * may have it hard coded, may go to a file, may present it to the
     * user, that's your call
     */
    fingerprint = libssh2_hostkey_hash(session, LIBSSH2_HOSTKEY_HASH_SHA1);
    fprintf(stderr, "Fingerprint: ");
    for(i = 0; i < 20; i++) {
        fprintf(stderr, "%02X ", (unsigned char)fingerprint[i]);
    }
    fprintf(stderr, "\n");

    if(argc > 1) {
        username = argv[1];
    }
    if(argc > 2) {
        password = argv[2];
    }
    if(argc > 3) {
        sftppath = argv[3];
    }
    if(argc > 4) {
        dest = argv[4];
    }

    tempstorage = fopen(STORAGE, "wb");
    if(!tempstorage) {
        fprintf(stderr, "Can't open temp storage file %s\n", STORAGE);
        goto shutdown;
    }

    if (auth_pw) {
        /* We could authenticate via password */
        while ((rc = libssh2_userauth_password(session, username, password))
               == LIBSSH2_ERROR_EAGAIN);
        if (rc) {
            fprintf(stderr, "Authentication by password failed.\n");
            goto shutdown;
        }
    } else {
        /* Or by public key */
        while ((rc =
                libssh2_userauth_publickey_fromfile(session, username,
                                                    "/home/username/"
                                                    ".ssh/id_rsa.pub",
                                                    "/home/username/"
                                                    ".ssh/id_rsa",
                                                    password)) ==
               LIBSSH2_ERROR_EAGAIN);
        if (rc) {
            fprintf(stderr, "\tAuthentication by public key failed\n");
            goto shutdown;
        }
    }

    do {
        sftp_session = libssh2_sftp_init(session);

        if(!sftp_session) {
            if(libssh2_session_last_errno(session) ==
               LIBSSH2_ERROR_EAGAIN) {
                fprintf(stderr, "non-blocking init\n");
                waitsocket(sock, session); /* now we wait */
            }
            else {
                fprintf(stderr, "Unable to init SFTP session\n");
                goto shutdown;
            }
        }
    } while (!sftp_session);

    /* Request a file via SFTP */
    do {
        sftp_handle = libssh2_sftp_open(sftp_session, sftppath,
                                        LIBSSH2_FXF_READ, 0);

        if (!sftp_handle) {
            if (libssh2_session_last_errno(session) != LIBSSH2_ERROR_EAGAIN) {
                fprintf(stderr, "Unable to open file with SFTP\n");
                goto shutdown;
            }
            else {
                fprintf(stderr, "non-blocking open\n");
                waitsocket(sock, session); /* now we wait */
            }
        }
    } while (!sftp_handle);

    fprintf(stderr, "libssh2_sftp_open() is done, now receive data!\n");
    do {
        do {
            /* read in a loop until we block */
            rc = libssh2_sftp_read(sftp_handle, mem, sizeof(mem));
            fprintf(stderr, "libssh2_sftp_read returned %d\n",
                    rc);

            if(rc > 0) {
                /* write to stderr */
                write(2, mem, rc);
                /* write to temporary storage area */
                fwrite(mem, rc, 1, tempstorage);
            }
        } while (rc > 0);

        if(rc != LIBSSH2_ERROR_EAGAIN) {
            /* error or end of file */
            break;
        }

        timeout.tv_sec = 10;
        timeout.tv_usec = 0;

        FD_ZERO(&fd);

        FD_SET(sock, &fd);

        /* wait for readable or writeable */
        rc = select(sock+1, &fd, &fd, NULL, &timeout);
        if(rc <= 0) {
            /* negative is error
               0 is timeout */
            fprintf(stderr, "SFTP download timed out: %d\n", rc);
            break;
        }

    } while (1);

    libssh2_sftp_close(sftp_handle);
    fclose(tempstorage);

    tempstorage = fopen(STORAGE, "rb");
    if(!tempstorage) {
        /* weird, we can't read the file we just wrote to... */
        fprintf(stderr, "can't open %s for reading\n", STORAGE);
        goto shutdown;
    }

    /* we're done downloading, now reverse the process and upload the
       temporarily stored data to the destination path */
    sftp_handle =
        libssh2_sftp_open(sftp_session, dest,
                          LIBSSH2_FXF_WRITE|LIBSSH2_FXF_CREAT,
                          LIBSSH2_SFTP_S_IRUSR|LIBSSH2_SFTP_S_IWUSR|
                          LIBSSH2_SFTP_S_IRGRP|LIBSSH2_SFTP_S_IROTH);
    if(sftp_handle) {
        size_t nread;
        char *ptr;
        do {
            nread = fread(mem, 1, sizeof(mem), tempstorage);
            if(nread <= 0) {
                /* end of file */
                break;
            }
            ptr = mem;

            do {
                /* write data in a loop until we block */
                rc = libssh2_sftp_write(sftp_handle, ptr,
                                        nread);
                ptr += rc;
                nread -= nread;
            } while (rc >= 0);

            if(rc != LIBSSH2_ERROR_EAGAIN) {
                /* error or end of file */
                break;
            }

            timeout.tv_sec = 10;
            timeout.tv_usec = 0;

            FD_ZERO(&fd);

            FD_SET(sock, &fd);

            /* wait for readable or writeable */
            rc = select(sock+1, &fd, &fd, NULL, &timeout);
            if(rc <= 0) {
                /* negative is error
                   0 is timeout */
                fprintf(stderr, "SFTP upload timed out: %d\n",
                        rc);
                break;
            }
        } while (1);
        fprintf(stderr, "SFTP upload done!\n");
    }
    else {
        fprintf(stderr, "SFTP failed to open destination path: %s\n",
                dest);
    }

    libssh2_sftp_shutdown(sftp_session);

  shutdown:

    libssh2_session_disconnect(session, "Normal Shutdown, Thank you for playing");
    libssh2_session_free(session);

#ifdef WIN32
    closesocket(sock);
#else
    close(sock);
#endif
    if (tempstorage)
        fclose(tempstorage);
    fprintf(stderr, "all done\n");

    libssh2_exit();

    return 0;
}
