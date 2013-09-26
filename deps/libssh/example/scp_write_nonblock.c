/*
 * Sample showing how to do an SCP non-blocking upload transfer.
 */

#include "libssh2_config.h"

#include <libssh2.h>

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
#include <time.h>

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
    LIBSSH2_SESSION *session = NULL;
    LIBSSH2_CHANNEL *channel;
    const char *username="username";
    const char *password="password";
    const char *loclfile="scp_write.c";
    const char *scppath="/tmp/TEST";
    FILE *local;
    int rc;
    char mem[1024*100];
    size_t nread;
    char *ptr;
    struct stat fileinfo;
    time_t start;
    long total = 0;
    int duration;
    size_t prev;

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
    if(argc > 4) {
        loclfile = argv[4];
    }
    if (argc > 5) {
        scppath = argv[5];
    }

    rc = libssh2_init (0);
    if (rc != 0) {
        fprintf (stderr, "libssh2 initialization failed (%d)\n", rc);
        return 1;
    }

    local = fopen(loclfile, "rb");
    if (!local) {
        fprintf(stderr, "Can't local file %s\n", loclfile);
        return -1;
    }

    stat(loclfile, &fileinfo);

    /* Ultra basic "connect to port 22 on localhost"
     * Your code is responsible for creating the socket establishing the
     * connection
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

    /* Create a session instance
     */
    session = libssh2_session_init();
    if(!session)
        return -1;

    /* Since we have set non-blocking, tell libssh2 we are non-blocking */
    libssh2_session_set_blocking(session, 0);

    /* ... start it up. This will trade welcome banners, exchange keys,
     * and setup crypto, compression, and MAC layers
     */
    while ((rc = libssh2_session_handshake(session, sock))
           == LIBSSH2_ERROR_EAGAIN);
    if(rc) {
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
        while ((rc = libssh2_userauth_password(session, username, password)) ==
               LIBSSH2_ERROR_EAGAIN);
        if (rc) {
            fprintf(stderr, "Authentication by password failed.\n");
            goto shutdown;
        }
    } else {
        /* Or by public key */
        while ((rc = libssh2_userauth_publickey_fromfile(session, username,
                                                         "/home/username/.ssh/id_rsa.pub",
                                                         "/home/username/.ssh/id_rsa",
                                                         password)) == LIBSSH2_ERROR_EAGAIN);
        if (rc) {
            fprintf(stderr, "\tAuthentication by public key failed\n");
            goto shutdown;
        }
    }

    /* Send a file via scp. The mode parameter must only have permissions! */
    do {
        channel = libssh2_scp_send(session, scppath, fileinfo.st_mode & 0777,
                                   (unsigned long)fileinfo.st_size);

        if ((!channel) && (libssh2_session_last_errno(session) !=
                           LIBSSH2_ERROR_EAGAIN)) {
            char *err_msg;

            libssh2_session_last_error(session, &err_msg, NULL, 0);
            fprintf(stderr, "%s\n", err_msg);
            goto shutdown;
        }
    } while (!channel);

    fprintf(stderr, "SCP session waiting to send file\n");
    start = time(NULL);
    do {
        nread = fread(mem, 1, sizeof(mem), local);
        if (nread <= 0) {
            /* end of file */
            break;
        }
        ptr = mem;

        total += nread;

        prev = 0;
        do {
            while ((rc = libssh2_channel_write(channel, ptr, nread)) ==
                   LIBSSH2_ERROR_EAGAIN) {
                waitsocket(sock, session);
                prev = 0;
            }
            if (rc < 0) {
                fprintf(stderr, "ERROR %d total %ld / %d prev %d\n", rc,
                        total, (int)nread, (int)prev);
                break;
            }
            else {
                prev = nread;

                /* rc indicates how many bytes were written this time */
                nread -= rc;
                ptr += rc;
            }
        } while (nread);
    } while (!nread); /* only continue if nread was drained */

    duration = (int)(time(NULL)-start);

    fprintf(stderr, "%ld bytes in %d seconds makes %.1f bytes/sec\n",
           total, duration, total/(double)duration);

    fprintf(stderr, "Sending EOF\n");
    while (libssh2_channel_send_eof(channel) == LIBSSH2_ERROR_EAGAIN);

    fprintf(stderr, "Waiting for EOF\n");
    while (libssh2_channel_wait_eof(channel) == LIBSSH2_ERROR_EAGAIN);

    fprintf(stderr, "Waiting for channel to close\n");
    while (libssh2_channel_wait_closed(channel) == LIBSSH2_ERROR_EAGAIN);

    libssh2_channel_free(channel);
    channel = NULL;

 shutdown:

    while (libssh2_session_disconnect(session,
                                      "Normal Shutdown, Thank you for playing") ==
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
