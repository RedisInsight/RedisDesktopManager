/*
 * Sample showing how to do SSH2 connect using ssh-agent.
 *
 * The sample code has default values for host name, user name:
 *
 * "ssh2_agent host user"
 */

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
#include <ctype.h>
#include <stdlib.h>

const char *username="username";

int main(int argc, char *argv[])
{
    unsigned long hostaddr;
    int sock = -1, i, rc;
    struct sockaddr_in sin;
    const char *fingerprint;
    char *userauthlist;
    LIBSSH2_SESSION *session = NULL;
    LIBSSH2_CHANNEL *channel;
    LIBSSH2_AGENT *agent = NULL;
    struct libssh2_agent_publickey *identity, *prev_identity = NULL;
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

    rc = libssh2_init (0);
    if (rc != 0) {
        fprintf (stderr, "libssh2 initialization failed (%d)\n", rc);
        return 1;
    }

    /* Ultra basic "connect to port 22 on localhost".  Your code is
     * responsible for creating the socket establishing the connection
     */
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        fprintf(stderr, "failed to create socket!\n");
        rc = 1;
        goto shutdown;
    }

    sin.sin_family = AF_INET;
    sin.sin_port = htons(22);
    sin.sin_addr.s_addr = hostaddr;
    if (connect(sock, (struct sockaddr*)(&sin),
                sizeof(struct sockaddr_in)) != 0) {
        fprintf(stderr, "failed to connect!\n");
        goto shutdown;
    }

    /* Create a session instance and start it up. This will trade welcome
     * banners, exchange keys, and setup crypto, compression, and MAC layers
     */
    session = libssh2_session_init();
    if (libssh2_session_handshake(session, sock)) {
        fprintf(stderr, "Failure establishing SSH session\n");
        return 1;
    }

    /* At this point we havn't authenticated. The first thing to do is check
     * the hostkey's fingerprint against our known hosts Your app may have it
     * hard coded, may go to a file, may present it to the user, that's your
     * call
     */
    fingerprint = libssh2_hostkey_hash(session, LIBSSH2_HOSTKEY_HASH_SHA1);
    fprintf(stderr, "Fingerprint: ");
    for(i = 0; i < 20; i++) {
        fprintf(stderr, "%02X ", (unsigned char)fingerprint[i]);
    }
    fprintf(stderr, "\n");

    /* check what authentication methods are available */
    userauthlist = libssh2_userauth_list(session, username, strlen(username));
    fprintf(stderr, "Authentication methods: %s\n", userauthlist);
    if (strstr(userauthlist, "publickey") == NULL) {
        fprintf(stderr, "\"publickey\" authentication is not supported\n");
        goto shutdown;
    }

    /* Connect to the ssh-agent */
    agent = libssh2_agent_init(session);
    if (!agent) {
        fprintf(stderr, "Failure initializing ssh-agent support\n");
        rc = 1;
        goto shutdown;
    }
    if (libssh2_agent_connect(agent)) {
        fprintf(stderr, "Failure connecting to ssh-agent\n");
        rc = 1;
        goto shutdown;
    }
    if (libssh2_agent_list_identities(agent)) {
        fprintf(stderr, "Failure requesting identities to ssh-agent\n");
        rc = 1;
        goto shutdown;
    }
    while (1) {
        rc = libssh2_agent_get_identity(agent, &identity, prev_identity);
        if (rc == 1)
            break;
        if (rc < 0) {
            fprintf(stderr,
                    "Failure obtaining identity from ssh-agent support\n");
            rc = 1;
            goto shutdown;
        }
        if (libssh2_agent_userauth(agent, username, identity)) {
            fprintf(stderr, "\tAuthentication with username %s and "
                   "public key %s failed!\n",
                   username, identity->comment);
        } else {
            fprintf(stderr, "\tAuthentication with username %s and "
                   "public key %s succeeded!\n",
                   username, identity->comment);
            break;
        }
        prev_identity = identity;
    }
    if (rc) {
        fprintf(stderr, "Couldn't continue authentication\n");
        goto shutdown;
    }

    /* We're authenticated now. */

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

    /* At this point the shell can be interacted with using
     * libssh2_channel_read()
     * libssh2_channel_read_stderr()
     * libssh2_channel_write()
     * libssh2_channel_write_stderr()
     *
     * Blocking mode may be (en|dis)abled with: libssh2_channel_set_blocking()
     * If the server send EOF, libssh2_channel_eof() will return non-0
     * To send EOF to the server use: libssh2_channel_send_eof()
     * A channel can be closed with: libssh2_channel_close()
     * A channel can be freed with: libssh2_channel_free()
     */

  skip_shell:
    if (channel) {
        libssh2_channel_free(channel);
        channel = NULL;
    }

    /* Other channel types are supported via:
     * libssh2_scp_send()
     * libssh2_scp_recv()
     * libssh2_channel_direct_tcpip()
     */

  shutdown:

    libssh2_agent_disconnect(agent);
    libssh2_agent_free(agent);

    if(session) {
        libssh2_session_disconnect(session,
                                   "Normal Shutdown, Thank you for playing");
        libssh2_session_free(session);
    }

    if (sock != -1) {
#ifdef WIN32
        closesocket(sock);
#else
        close(sock);
#endif
    }

    fprintf(stderr, "all done!\n");

    libssh2_exit();

    return rc;
}
