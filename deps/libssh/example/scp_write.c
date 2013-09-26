/*
 * Sample showing how to do an SCP upload.
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
    char mem[1024];
    size_t nread;
    char *ptr;
    struct stat fileinfo;

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
        fprintf(stderr, "Can't open local file %s\n", loclfile);
        return -1;
    }

    stat(loclfile, &fileinfo);

    /* Ultra basic "connect to port 22 on localhost"
     * Your code is responsible for creating the socket establishing the
     * connection
     */
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if(-1 == sock) {
        fprintf(stderr, "failed to create socket!\n");
        return -1;
    }

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

    /* ... start it up. This will trade welcome banners, exchange keys,
     * and setup crypto, compression, and MAC layers
     */
    rc = libssh2_session_handshake(session, sock);
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
        if (libssh2_userauth_password(session, username, password)) {
            fprintf(stderr, "Authentication by password failed.\n");
            goto shutdown;
        }
    } else {
        /* Or by public key */
        if (libssh2_userauth_publickey_fromfile(session, username,
                            "/home/username/.ssh/id_rsa.pub",
                            "/home/username/.ssh/id_rsa",
                            password)) {
            fprintf(stderr, "\tAuthentication by public key failed\n");
            goto shutdown;
        }
    }

    /* Send a file via scp. The mode parameter must only have permissions! */
    channel = libssh2_scp_send(session, scppath, fileinfo.st_mode & 0777,
                               (unsigned long)fileinfo.st_size);

    if (!channel) {
        char *errmsg;
        int errlen;
        int err = libssh2_session_last_error(session, &errmsg, &errlen, 0);
        fprintf(stderr, "Unable to open a session: (%d) %s\n", err, errmsg);
        goto shutdown;
    }

    fprintf(stderr, "SCP session waiting to send file\n");
    do {
        nread = fread(mem, 1, sizeof(mem), local);
        if (nread <= 0) {
            /* end of file */
            break;
        }
        ptr = mem;

        do {
            /* write the same data over and over, until error or completion */
            rc = libssh2_channel_write(channel, ptr, nread);
            if (rc < 0) {
                fprintf(stderr, "ERROR %d\n", rc);
                break;
            }
            else {
                /* rc indicates how many bytes were written this time */
                ptr += rc;
                nread -= rc;
            }
        } while (nread);

    } while (1);

    fprintf(stderr, "Sending EOF\n");
    libssh2_channel_send_eof(channel);

    fprintf(stderr, "Waiting for EOF\n");
    libssh2_channel_wait_eof(channel);

    fprintf(stderr, "Waiting for channel to close\n");
    libssh2_channel_wait_closed(channel);

    libssh2_channel_free(channel);
    channel = NULL;

 shutdown:

    if(session) {
        libssh2_session_disconnect(session, "Normal Shutdown, Thank you for playing");
        libssh2_session_free(session);
    }
#ifdef WIN32
    closesocket(sock);
#else
    close(sock);
#endif
    if (local)
        fclose(local);
    fprintf(stderr, "all done\n");

    libssh2_exit();

    return 0;
}
