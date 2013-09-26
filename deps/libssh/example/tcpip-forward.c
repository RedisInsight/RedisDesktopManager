#include "libssh2_config.h"
#include <libssh2.h>

#ifdef WIN32
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/time.h>
#endif

#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

#ifdef HAVE_SYS_SELECT_H
#include <sys/select.h>
#endif

#ifndef INADDR_NONE
#define INADDR_NONE (in_addr_t)-1
#endif

const char *keyfile1 = "/home/username/.ssh/id_rsa.pub";
const char *keyfile2 = "/home/username/.ssh/id_rsa";
const char *username = "username";
const char *password = "";

const char *server_ip = "127.0.0.1";

const char *remote_listenhost = "localhost"; /* resolved by the server */
unsigned int remote_wantport = 2222;
unsigned int remote_listenport;

const char *local_destip = "127.0.0.1";
unsigned int local_destport = 22;

enum {
    AUTH_NONE = 0,
    AUTH_PASSWORD,
    AUTH_PUBLICKEY
};

int main(int argc, char *argv[])
{
    int rc, sock = -1, forwardsock = -1, i, auth = AUTH_NONE;
    struct sockaddr_in sin;
    socklen_t sinlen = sizeof(sin);
    const char *fingerprint;
    char *userauthlist;
    LIBSSH2_SESSION *session;
    LIBSSH2_LISTENER *listener = NULL;
    LIBSSH2_CHANNEL *channel = NULL;
    const char *shost;
    unsigned int sport;
    fd_set fds;
    struct timeval tv;
    ssize_t len, wr;
    char buf[16384];

#ifdef WIN32
    char sockopt;
    WSADATA wsadata;

    WSAStartup(MAKEWORD(2,0), &wsadata);
#else
    int sockopt;
#endif

    if (argc > 1)
        server_ip = argv[1];
    if (argc > 2)
        username = argv[2];
    if (argc > 3)
        password = argv[3];
    if (argc > 4)
        remote_listenhost = argv[4];
    if (argc > 5)
        remote_wantport = atoi(argv[5]);
    if (argc > 6)
        local_destip = argv[6];
    if (argc > 7)
        local_destport = atoi(argv[7]);

    rc = libssh2_init (0);
    if (rc != 0) {
        fprintf (stderr, "libssh2 initialization failed (%d)\n", rc);
        return 1;
    }

    /* Connect to SSH server */
    sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    sin.sin_family = AF_INET;
    if (INADDR_NONE == (sin.sin_addr.s_addr = inet_addr(server_ip))) {
        perror("inet_addr");
        return -1;
    }
    sin.sin_port = htons(22);
    if (connect(sock, (struct sockaddr*)(&sin),
                sizeof(struct sockaddr_in)) != 0) {
        fprintf(stderr, "failed to connect!\n");
        return -1;
    }

    /* Create a session instance */
    session = libssh2_session_init();
    if(!session) {
        fprintf(stderr, "Could not initialize SSH session!\n");
        return -1;
    }

    /* ... start it up. This will trade welcome banners, exchange keys,
     * and setup crypto, compression, and MAC layers
     */
    rc = libssh2_session_handshake(session, sock);
    if(rc) {
        fprintf(stderr, "Error when starting up SSH session: %d\n", rc);
        return -1;
    }

    /* At this point we havn't yet authenticated.  The first thing to do
     * is check the hostkey's fingerprint against our known hosts Your app
     * may have it hard coded, may go to a file, may present it to the
     * user, that's your call
     */
    fingerprint = libssh2_hostkey_hash(session, LIBSSH2_HOSTKEY_HASH_SHA1);
    fprintf(stderr, "Fingerprint: ");
    for(i = 0; i < 20; i++)
        fprintf(stderr, "%02X ", (unsigned char)fingerprint[i]);
    fprintf(stderr, "\n");

    /* check what authentication methods are available */
    userauthlist = libssh2_userauth_list(session, username, strlen(username));
    fprintf(stderr, "Authentication methods: %s\n", userauthlist);
    if (strstr(userauthlist, "password"))
        auth |= AUTH_PASSWORD;
    if (strstr(userauthlist, "publickey"))
        auth |= AUTH_PUBLICKEY;

    /* check for options */
    if(argc > 8) {
        if ((auth & AUTH_PASSWORD) && !strcasecmp(argv[8], "-p"))
            auth = AUTH_PASSWORD;
        if ((auth & AUTH_PUBLICKEY) && !strcasecmp(argv[8], "-k"))
            auth = AUTH_PUBLICKEY;
    }

    if (auth & AUTH_PASSWORD) {
        if (libssh2_userauth_password(session, username, password)) {
            fprintf(stderr, "Authentication by password failed.\n");
            goto shutdown;
        }
    } else if (auth & AUTH_PUBLICKEY) {
        if (libssh2_userauth_publickey_fromfile(session, username, keyfile1,
                                                keyfile2, password)) {
            fprintf(stderr, "\tAuthentication by public key failed!\n");
            goto shutdown;
        }
        fprintf(stderr, "\tAuthentication by public key succeeded.\n");
    } else {
        fprintf(stderr, "No supported authentication methods found!\n");
        goto shutdown;
    }

    fprintf(stderr, "Asking server to listen on remote %s:%d\n",
        remote_listenhost, remote_wantport);

    listener = libssh2_channel_forward_listen_ex(session, remote_listenhost,
        remote_wantport, &remote_listenport, 1);
    if (!listener) {
        fprintf(stderr, "Could not start the tcpip-forward listener!\n"
                "(Note that this can be a problem at the server!"
                " Please review the server logs.)\n");
        goto shutdown;
    }

    fprintf(stderr, "Server is listening on %s:%d\n", remote_listenhost,
        remote_listenport);

    fprintf(stderr, "Waiting for remote connection\n");
    channel = libssh2_channel_forward_accept(listener);
    if (!channel) {
        fprintf(stderr, "Could not accept connection!\n"
                "(Note that this can be a problem at the server!"
                " Please review the server logs.)\n");
        goto shutdown;
    }

    fprintf(stderr,
        "Accepted remote connection. Connecting to local server %s:%d\n",
        local_destip, local_destport);
    forwardsock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    sin.sin_family = AF_INET;
    sin.sin_port = htons(local_destport);
    if (INADDR_NONE == (sin.sin_addr.s_addr = inet_addr(local_destip))) {
        perror("inet_addr");
        goto shutdown;
    }
    if (-1 == connect(forwardsock, (struct sockaddr *)&sin, sinlen)) {
        perror("connect");
        goto shutdown;
    }

    fprintf(stderr, "Forwarding connection from remote %s:%d to local %s:%d\n",
        remote_listenhost, remote_listenport, local_destip, local_destport);

    /* Must use non-blocking IO hereafter due to the current libssh2 API */
    libssh2_session_set_blocking(session, 0);

    while (1) {
        FD_ZERO(&fds);
        FD_SET(forwardsock, &fds);
        tv.tv_sec = 0;
        tv.tv_usec = 100000;
        rc = select(forwardsock + 1, &fds, NULL, NULL, &tv);
        if (-1 == rc) {
            perror("select");
            goto shutdown;
        }
        if (rc && FD_ISSET(forwardsock, &fds)) {
            len = recv(forwardsock, buf, sizeof(buf), 0);
            if (len < 0) {
                perror("read");
                goto shutdown;
            } else if (0 == len) {
                fprintf(stderr, "The local server at %s:%d disconnected!\n",
                    local_destip, local_destport);
                goto shutdown;
            }
            wr = 0;
            do {
                i = libssh2_channel_write(channel, buf, len);
                if (i < 0) {
                    fprintf(stderr, "libssh2_channel_write: %d\n", i);
                    goto shutdown;
                }
                wr += i;
            } while(i > 0 && wr < len);
        }
        while (1) {
            len = libssh2_channel_read(channel, buf, sizeof(buf));
            if (LIBSSH2_ERROR_EAGAIN == len)
                break;
            else if (len < 0) {
                fprintf(stderr, "libssh2_channel_read: %d", (int)len);
                goto shutdown;
            }
            wr = 0;
            while (wr < len) {
                i = send(forwardsock, buf + wr, len - wr, 0);
                if (i <= 0) {
                    perror("write");
                    goto shutdown;
                }
                wr += i;
            }
            if (libssh2_channel_eof(channel)) {
                fprintf(stderr, "The remote client at %s:%d disconnected!\n",
                    remote_listenhost, remote_listenport);
                goto shutdown;
            }
        }
    }

shutdown:
#ifdef WIN32
    closesocket(forwardsock);
#else
    close(forwardsock);
#endif
    if (channel)
        libssh2_channel_free(channel);
    if (listener)
        libssh2_channel_forward_cancel(listener);
    libssh2_session_disconnect(session, "Client disconnecting normally");
    libssh2_session_free(session);

#ifdef WIN32
    closesocket(sock);
#else
    close(sock);
#endif

    libssh2_exit();

    return 0;
}
