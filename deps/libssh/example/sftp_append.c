/*
 * Sample showing how to do SFTP append of a local file onto a remote one.
 *
 * The sample code has default values for host name, user name, password
 * and path to copy, but you can specify them on the command line like:
 *
 * sftp_append 192.168.0.1 user password localfile /tmp/remotefile
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
# ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#ifdef HAVE_ARPA_INET_H
# include <arpa/inet.h>
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
    LIBSSH2_SESSION *session;
    const char *username="username";
    const char *password="password";
    const char *loclfile="sftp_write.c";
    const char *sftppath="/tmp/TEST";
    int rc;
    FILE *local;
    LIBSSH2_SFTP *sftp_session;
    LIBSSH2_SFTP_HANDLE *sftp_handle;
    LIBSSH2_SFTP_ATTRIBUTES attrs;
    char mem[1024*100];
    size_t nread;
    char *ptr;

#ifdef WIN32
    WSADATA wsadata;

    WSAStartup(MAKEWORD(2,0), &wsadata);
#endif

    if (argc > 1) {
        hostaddr = inet_addr(argv[1]);
    } else {
        hostaddr = htonl(0x7F000001);
    }

    if(argc > 2) {
        username = argv[2];
    }
    if(argc > 3) {
        password = argv[3];
    }
    if(argc > 4) {
        loclfile = argv[4];
    }
    if(argc > 5) {
        sftppath = argv[5];
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

    /* Create a session instance
     */
    session = libssh2_session_init();
    if(!session)
        return -1;

    /* Since we have set non-blocking, tell libssh2 we are blocking */
    libssh2_session_set_blocking(session, 1);

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

    fprintf(stderr, "libssh2_sftp_init()!\n");
    sftp_session = libssh2_sftp_init(session);

    if (!sftp_session) {
        fprintf(stderr, "Unable to init SFTP session\n");
        goto shutdown;
    }

    fprintf(stderr, "libssh2_sftp_open() for READ and WRITE!\n");
    /* Request a file via SFTP */

    sftp_handle =
        libssh2_sftp_open(sftp_session, sftppath,
                          LIBSSH2_FXF_WRITE|LIBSSH2_FXF_READ,
                          LIBSSH2_SFTP_S_IRUSR|LIBSSH2_SFTP_S_IWUSR|
                          LIBSSH2_SFTP_S_IRGRP|LIBSSH2_SFTP_S_IROTH);
    if (!sftp_handle) {
        fprintf(stderr, "Unable to open file with SFTP\n");
        goto shutdown;
    }

    if(libssh2_sftp_fstat_ex(sftp_handle, &attrs, 0) < 0) {
        fprintf(stderr, "libssh2_sftp_fstat_ex failed\n");
        goto shutdown;
    }
    else
        libssh2_sftp_seek64(sftp_handle, attrs.filesize);
    fprintf(stderr, "Did a seek to position %ld\n", (long) attrs.filesize);

    fprintf(stderr, "libssh2_sftp_open() a handle for APPEND\n");

    if (!sftp_handle) {
        fprintf(stderr, "Unable to open file with SFTP\n");
        goto shutdown;
    }
    fprintf(stderr, "libssh2_sftp_open() is done, now send data!\n");
    do {
        nread = fread(mem, 1, sizeof(mem), local);
        if (nread <= 0) {
            /* end of file */
            break;
        }
        ptr = mem;

        do {
            /* write data in a loop until we block */
            rc = libssh2_sftp_write(sftp_handle, ptr, nread);
            if(rc < 0)
                break;
            ptr += rc;
            nread -= rc;
        } while (nread);

    } while (rc > 0);

    libssh2_sftp_close(sftp_handle);
    libssh2_sftp_shutdown(sftp_session);

shutdown:
    libssh2_session_disconnect(session,
                               "Normal Shutdown, Thank you for playing");
    libssh2_session_free(session);

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
