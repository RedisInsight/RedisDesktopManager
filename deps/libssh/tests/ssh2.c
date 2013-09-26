/* Self test, based on examples/ssh2.c. */

#include "libssh2_config.h"
#include <libssh2.h>
#include <libssh2_sftp.h>

#ifdef HAVE_WINDOWS_H
# include <windows.h>
#endif
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
# ifdef HAVE_ARPA_INET_H
#include <arpa/inet.h>
#endif

#include <sys/types.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

int main(int argc, char *argv[])
{
    unsigned long hostaddr;
    int sock, i, auth_pw = 0;
    struct sockaddr_in sin;
    const char *fingerprint;
    char *userauthlist;
    LIBSSH2_SESSION *session;
    LIBSSH2_CHANNEL *channel;
#ifdef WIN32
    WSADATA wsadata;

    WSAStartup(MAKEWORD(2,0), &wsadata);
#endif
    const char *pubkeyfile="etc/user.pub";
    const char *privkeyfile="etc/user";
    const char *username="username";
    const char *password="password";
    int ec = 1;

    (void)argc;
    (void)argv;

    if (getenv ("USER"))
      username = getenv ("USER");

    if (getenv ("PRIVKEY"))
      privkeyfile = getenv ("PRIVKEY");

    if (getenv ("PUBKEY"))
      pubkeyfile = getenv ("PUBKEY");

    hostaddr = htonl(0x7F000001);

    sock = socket(AF_INET, SOCK_STREAM, 0);
#ifndef WIN32
    fcntl(sock, F_SETFL, 0);
#endif
    sin.sin_family = AF_INET;
    sin.sin_port = htons(4711);
    sin.sin_addr.s_addr = hostaddr;
    if (connect(sock, (struct sockaddr*)(&sin),
                sizeof(struct sockaddr_in)) != 0) {
        fprintf(stderr, "failed to connect!\n");
        return -1;
    }

    /* Create a session instance and start it up
     * This will trade welcome banners, exchange keys, and setup crypto, compression, and MAC layers
     */
    session = libssh2_session_init();
    if (libssh2_session_startup(session, sock)) {
        fprintf(stderr, "Failure establishing SSH session\n");
        return -1;
    }

    /* At this point we havn't authenticated,
     * The first thing to do is check the hostkey's fingerprint against our known hosts
     * Your app may have it hard coded, may go to a file, may present it to the user, that's your call
     */
    fingerprint = libssh2_hostkey_hash(session, LIBSSH2_HOSTKEY_HASH_SHA1);
    printf("Fingerprint: ");
    for(i = 0; i < 20; i++) {
        printf("%02X ", (unsigned char)fingerprint[i]);
    }
    printf("\n");

    /* check what authentication methods are available */
    userauthlist = libssh2_userauth_list(session, username, strlen(username));
    printf("Authentication methods: %s\n", userauthlist);
    if (strstr(userauthlist, "password") != NULL) {
        auth_pw |= 1;
    }
    if (strstr(userauthlist, "keyboard-interactive") != NULL) {
        auth_pw |= 2;
    }
    if (strstr(userauthlist, "publickey") != NULL) {
        auth_pw |= 4;
    }

    if (auth_pw & 4) {
        /* Authenticate by public key */
        if (libssh2_userauth_publickey_fromfile(session, username, pubkeyfile, privkeyfile, password)) {
            printf("\tAuthentication by public key failed!\n");
            goto shutdown;
        } else {
            printf("\tAuthentication by public key succeeded.\n");
        }
    } else {
        printf("No supported authentication methods found!\n");
        goto shutdown;
    }

    /* Request a shell */
    if (!(channel = libssh2_channel_open_session(session))) {
        fprintf(stderr, "Unable to open a session\n");
        goto shutdown;
    }

    /* Some environment variables may be set,
     * It's up to the server which ones it'll allow though
     */
    libssh2_channel_setenv(channel, "FOO", "bar");

    /* Request a terminal with 'vanilla' terminal emulation
     * See /etc/termcap for more options
     */
    if (libssh2_channel_request_pty(channel, "vanilla")) {
        fprintf(stderr, "Failed requesting pty\n");
        goto skip_shell;
    }

    /* Open a SHELL on that pty */
    if (libssh2_channel_shell(channel)) {
        fprintf(stderr, "Unable to request shell on allocated pty\n");
        goto shutdown;
    }

    ec = 0;

  skip_shell:
    if (channel) {
        libssh2_channel_free(channel);
        channel = NULL;
    }

  shutdown:

    libssh2_session_disconnect(session, "Normal Shutdown");
    libssh2_session_free(session);

#ifdef WIN32
    Sleep(1000);
    closesocket(sock);
#else
    sleep(1);
    close(sock);
#endif

    return ec;
}
