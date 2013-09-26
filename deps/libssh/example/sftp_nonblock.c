/*
 * Sample showing how to do SFTP non-blocking transfers.
 *
 * The sample code has default values for host name, user name, password
 * and path to copy, but you can specify them on the command line like:
 *
 * "sftp_nonblock 192.168.0.1 user password /tmp/secrets"
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

/* diff in ms */
static long tvdiff(struct timeval newer, struct timeval older)
{
  return (newer.tv_sec-older.tv_sec)*1000+
      (newer.tv_usec-older.tv_usec)/1000;
}

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
    unsigned long hostaddr;
    int sock, i, auth_pw = 1;
    struct sockaddr_in sin;
    const char *fingerprint;
    LIBSSH2_SESSION *session;
    const char *username="username";
    const char *password="password";
    const char *sftppath="/tmp/TEST";
    struct timeval start;
    struct timeval end;
    int rc;
    int total = 0;
    long time_ms;
    int spin = 0;
    LIBSSH2_SFTP *sftp_session;
    LIBSSH2_SFTP_HANDLE *sftp_handle;

#ifdef WIN32
    WSADATA wsadata;

    WSAStartup(MAKEWORD(2,0), &wsadata);
#endif

    if (argc > 1) {
        hostaddr = inet_addr(argv[1]);
    } else {
        hostaddr = htonl(0x7F000001);
    }

    if (argc > 2) {
        username = argv[2];
    }
    if (argc > 3) {
        password = argv[3];
    }
    if (argc > 4) {
        sftppath = argv[4];
    }

    rc = libssh2_init (0);
    if (rc != 0) {
        fprintf (stderr, "libssh2 initialization failed (%d)\n", rc);
        return 1;
    }

    /*
     * The application code is responsible for creating the socket
     * and establishing the connection
     */
    sock = socket(AF_INET, SOCK_STREAM, 0);

    sin.sin_family = AF_INET;
    sin.sin_port = htons(22);
    sin.sin_addr.s_addr = hostaddr;
    if (connect(sock, (struct sockaddr*)(&sin),
                sizeof(struct sockaddr_in)) != 0) {
        fprintf(stderr, "failed to connect!\n");
        return -1;
    }

    /* Create a session instance */
    session = libssh2_session_init();
    if (!session)
        return -1;

    /* Since we have set non-blocking, tell libssh2 we are non-blocking */
    libssh2_session_set_blocking(session, 0);

    gettimeofday(&start, NULL);

    /* ... start it up. This will trade welcome banners, exchange keys,
        * and setup crypto, compression, and MAC layers
        */
    while ((rc = libssh2_session_handshake(session, sock)) ==
           LIBSSH2_ERROR_EAGAIN);
    if (rc) {
        fprintf(stderr, "Failure establishing SSH session: %d\n", rc);
        return -1;
    }

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
#if 0
    libssh2_trace(session, LIBSSH2_TRACE_CONN);
#endif
    fprintf(stderr, "libssh2_sftp_init()!\n");
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

    fprintf(stderr, "libssh2_sftp_open()!\n");
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
        char mem[1024*24];

        /* loop until we fail */
        while ((rc = libssh2_sftp_read(sftp_handle, mem,
                                       sizeof(mem))) == LIBSSH2_ERROR_EAGAIN) {
            spin++;
            waitsocket(sock, session); /* now we wait */
        }
        if (rc > 0) {
            total += rc;
            write(1, mem, rc);
        } else {
            break;
        }
    } while (1);

    gettimeofday(&end, NULL);
    time_ms = tvdiff(end, start);
    fprintf(stderr, "Got %d bytes in %ld ms = %.1f bytes/sec spin: %d\n", total,
           time_ms, total/(time_ms/1000.0), spin );

    libssh2_sftp_close(sftp_handle);
    libssh2_sftp_shutdown(sftp_session);

shutdown:

    fprintf(stderr, "libssh2_session_disconnect\n");
    while (libssh2_session_disconnect(session,
                                      "Normal Shutdown, Thank you") ==
           LIBSSH2_ERROR_EAGAIN);
    libssh2_session_free(session);

#ifdef WIN32
    closesocket(sock);
#else
    close(sock);
#endif
    fprintf(stderr, "all done\n");

    libssh2_exit();

    return 0;
}
