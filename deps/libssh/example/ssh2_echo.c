/*
 * Run it like this:
 *
 * $ ./ssh2_echo 127.0.0.1 user password
 *
 * The code sends a 'cat' command, and then writes a lot of data to it only to
 * check that reading the returned data sums up to the same amount.
 *
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

#include <sys/time.h>
#include <sys/types.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <ctype.h>

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

#define BUFSIZE 32000

int main(int argc, char *argv[])
{
    const char *hostname = "127.0.0.1";
    const char *commandline = "cat";
    const char *username    = "user";
    const char *password    = "password";
    unsigned long hostaddr;
    int sock;
    struct sockaddr_in sin;
    const char *fingerprint;
    LIBSSH2_SESSION *session;
    LIBSSH2_CHANNEL *channel;
    int rc;
    int exitcode = 0;
    char *exitsignal=(char *)"none";
    size_t len;
    LIBSSH2_KNOWNHOSTS *nh;
    int type;

#ifdef WIN32
    WSADATA wsadata;
    WSAStartup(MAKEWORD(2,0), &wsadata);
#endif
    if (argc > 1)
        /* must be ip address only */
        hostname = argv[1];

    if (argc > 2) {
        username = argv[2];
    }
    if (argc > 3) {
        password = argv[3];
    }

    rc = libssh2_init (0);
    if (rc != 0) {
        fprintf (stderr, "libssh2 initialization failed (%d)\n", rc);
        return 1;
    }

    hostaddr = inet_addr(hostname);

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

    /* Create a session instance */
    session = libssh2_session_init();
    if (!session)
        return -1;

    /* tell libssh2 we want it all done non-blocking */
    libssh2_session_set_blocking(session, 0);

    /* ... start it up. This will trade welcome banners, exchange keys,
     * and setup crypto, compression, and MAC layers
     */
    while ((rc = libssh2_session_handshake(session, sock)) ==
           LIBSSH2_ERROR_EAGAIN);
    if (rc) {
        fprintf(stderr, "Failure establishing SSH session: %d\n", rc);
        return -1;
    }

    nh = libssh2_knownhost_init(session);
    if(!nh) {
        /* eeek, do cleanup here */
        return 2;
    }

    /* read all hosts from here */
    libssh2_knownhost_readfile(nh, "known_hosts",
                               LIBSSH2_KNOWNHOST_FILE_OPENSSH);

    /* store all known hosts to here */
    libssh2_knownhost_writefile(nh, "dumpfile",
                                LIBSSH2_KNOWNHOST_FILE_OPENSSH);

    fingerprint = libssh2_session_hostkey(session, &len, &type);
    if(fingerprint) {
        struct libssh2_knownhost *host;
        int check = libssh2_knownhost_checkp(nh, hostname, 22,
                                             fingerprint, len,
                                             LIBSSH2_KNOWNHOST_TYPE_PLAIN|
                                             LIBSSH2_KNOWNHOST_KEYENC_RAW,
                                             &host);

        fprintf(stderr, "Host check: %d, key: %s\n", check,
                (check <= LIBSSH2_KNOWNHOST_CHECK_MISMATCH)?
                host->key:"<none>");

        /*****
         * At this point, we could verify that 'check' tells us the key is
         * fine or bail out.
         *****/
    }
    else {
        /* eeek, do cleanup here */
        return 3;
    }
    libssh2_knownhost_free(nh);

    if ( strlen(password) != 0 ) {
        /* We could authenticate via password */
        while ((rc = libssh2_userauth_password(session, username, password)) ==
               LIBSSH2_ERROR_EAGAIN);
        if (rc) {
            fprintf(stderr, "Authentication by password failed.\n");
            exit(1);
        }
    }

    libssh2_trace(session, LIBSSH2_TRACE_SOCKET);

    /* Exec non-blocking on the remove host */
    while( (channel = libssh2_channel_open_session(session)) == NULL &&
           libssh2_session_last_error(session,NULL,NULL,0) ==
           LIBSSH2_ERROR_EAGAIN ) {
        waitsocket(sock, session);
    }
    if( channel == NULL ) {
        fprintf(stderr,"Error\n");
        exit( 1 );
    }
    while( (rc = libssh2_channel_exec(channel, commandline)) ==
           LIBSSH2_ERROR_EAGAIN )
        waitsocket(sock, session);

    if( rc != 0 ) {
        fprintf(stderr, "exec error\n");
        exit( 1 );
    }
    else {
        LIBSSH2_POLLFD *fds = NULL;
        int running = 1;
        int bufsize = BUFSIZE;
        char buffer[BUFSIZE];
        int totsize = 1500000;
        int totwritten = 0;
        int totread = 0;
        int partials = 0;
        int rereads = 0;
        int rewrites = 0;
        int i;

        for (i = 0; i < BUFSIZE; i++)
            buffer[i] = 'A';

        if ((fds = malloc (sizeof (LIBSSH2_POLLFD))) == NULL) {
            fprintf(stderr, "malloc failed\n");
            exit(1);
        }

        fds[0].type = LIBSSH2_POLLFD_CHANNEL;
        fds[0].fd.channel = channel;
        fds[0].events = LIBSSH2_POLLFD_POLLIN | LIBSSH2_POLLFD_POLLOUT;

        do {
            int rc = (libssh2_poll(fds, 1, 10));
            int act = 0;

            if (rc < 1)
                continue;

            if (fds[0].revents & LIBSSH2_POLLFD_POLLIN) {
                int n = libssh2_channel_read(channel, buffer, sizeof(buffer));
                act++;

                if (n == LIBSSH2_ERROR_EAGAIN) {
                    rereads++;
                    fprintf(stderr, "will read again\n");
                }
                else if (n < 0) {
                    fprintf(stderr, "read failed\n");
                    exit(1);
                }
                else {
                    totread += n;
                    fprintf(stderr, "read %d bytes (%d in total)\n",
                            n, totread);
                }
            }

            if (fds[0].revents & LIBSSH2_POLLFD_POLLOUT) {
                act++;

                if (totwritten < totsize) {
                    /* we have not written all data yet */
                    int left = totsize - totwritten;
                    int size = (left < bufsize) ? left : bufsize;
                    int n = libssh2_channel_write_ex(channel, 0, buffer, size);

                    if (n == LIBSSH2_ERROR_EAGAIN) {
                        rewrites++;
                        fprintf(stderr, "will write again\n");
                    }
                    else if (n < 0) {
                        fprintf(stderr, "write failed\n");
                        exit(1);
                    }
                    else {
                        totwritten += n;
                        fprintf(stderr, "wrote %d bytes (%d in total)",
                                n, totwritten);
                        if (left >= bufsize && n != bufsize) {
                            partials++;
                            fprintf(stderr, " PARTIAL");
                        }
                        fprintf(stderr, "\n");
                    }
                } else {
                    /* all data written, send EOF */
                    rc = libssh2_channel_send_eof(channel);

                    if (rc == LIBSSH2_ERROR_EAGAIN) {
                        fprintf(stderr, "will send eof again\n");
                    }
                    else if (rc < 0) {
                        fprintf(stderr, "send eof failed\n");
                        exit(1);
                    }
                    else {
                        fprintf(stderr, "sent eof\n");
                        /* we're done writing, stop listening for OUT events */
                        fds[0].events &= ~LIBSSH2_POLLFD_POLLOUT;
                    }
                }
            }

            if (fds[0].revents & LIBSSH2_POLLFD_CHANNEL_CLOSED) {
                if (!act) /* don't leave loop until we have read all data */
                    running = 0;
            }
        } while(running);

        exitcode = 127;
        while( (rc = libssh2_channel_close(channel)) == LIBSSH2_ERROR_EAGAIN )
            waitsocket(sock, session);

        if( rc == 0 ) {
            exitcode = libssh2_channel_get_exit_status( channel );
            libssh2_channel_get_exit_signal(channel, &exitsignal,
                                            NULL, NULL, NULL, NULL, NULL);
        }

        if (exitsignal)
            fprintf(stderr, "\nGot signal: %s\n", exitsignal);

        libssh2_channel_free(channel);
        channel = NULL;

        fprintf(stderr, "\nrereads: %d rewrites: %d totwritten %d\n",
                rereads, rewrites, totwritten);

        if (totwritten != totread) {
            fprintf(stderr, "\n*** FAIL bytes written: %d bytes "
                    "read: %d ***\n", totwritten, totread);
            exit(1);
        }
    }

    libssh2_session_disconnect(session,
                               "Normal Shutdown, Thank you for playing");
    libssh2_session_free(session);

#ifdef WIN32
    closesocket(sock);
#else
    close(sock);
#endif
    fprintf(stderr, "all done\n");

    libssh2_exit();

    return exitcode;
}
