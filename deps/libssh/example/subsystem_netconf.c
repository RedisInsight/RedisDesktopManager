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
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

#ifdef HAVE_SYS_SELECT_H
#include <sys/select.h>
#endif

#ifndef INADDR_NONE
#define INADDR_NONE (in_addr_t)~0
#endif

const char *keyfile1 = "/home/username/.ssh/id_rsa.pub";
const char *keyfile2 = "/home/username/.ssh/id_rsa";
const char *username = "username";
const char *password = "";

const char *server_ip = "127.0.0.1";

enum {
    AUTH_NONE = 0,
    AUTH_PASSWORD,
    AUTH_PUBLICKEY
};

static int netconf_write(LIBSSH2_CHANNEL *channel, const char *buf, size_t len)
{
    int i;
    ssize_t wr = 0;

    do {
        i = libssh2_channel_write(channel, buf, len);
        if (i < 0) {
            fprintf(stderr, "libssh2_channel_write: %d\n", i);
            return -1;
        }
        wr += i;
    } while (i > 0 && wr < (ssize_t)len);

    return 0;
}

static int netconf_read_until(LIBSSH2_CHANNEL *channel, const char *endtag,
                              char *buf, size_t buflen)
{
    ssize_t len;
    size_t rd = 0;
    char *endreply = NULL, *specialsequence = NULL;

    memset(buf, 0, buflen);

    do {
        len = libssh2_channel_read(channel, buf + rd, buflen - rd);
        if (LIBSSH2_ERROR_EAGAIN == len)
            continue;
        else if (len < 0) {
            fprintf(stderr, "libssh2_channel_read: %d\n", (int)len);
            return -1;
        }
        rd += len;

        /* read more data until we see a rpc-reply closing tag followed by
         * the special sequence ]]>]]> */

        /* really, this MUST be replaced with proper XML parsing! */

        endreply = strstr(buf, endtag);
        if (endreply)
            specialsequence = strstr(endreply, "]]>]]>");

    } while (!specialsequence && rd < buflen);

    if (!specialsequence) {
        fprintf(stderr, "%s: ]]>]]> not found! read buffer too small?\n", __func__);
        return -1;
    }

    /* discard the special sequence so that only XML is returned */
    rd = specialsequence - buf;
    buf[rd] = 0;

    return rd;
}

int main(int argc, char *argv[])
{
    int rc, sock = -1, i, auth = AUTH_NONE;
    struct sockaddr_in sin;
    const char *fingerprint;
    char *userauthlist;
    LIBSSH2_SESSION *session;
    LIBSSH2_CHANNEL *channel = NULL;
    char buf[1048576]; /* avoid any buffer reallocation for simplicity */
    ssize_t len;

#ifdef WIN32
    WSADATA wsadata;

    WSAStartup(MAKEWORD(2,0), &wsadata);
#endif

    if (argc > 1)
        server_ip = argv[1];
    if (argc > 2)
        username = argv[2];
    if (argc > 3)
        password = argv[3];

    rc = libssh2_init (0);
    if (rc != 0) {
        fprintf (stderr, "libssh2 initialization failed (%d)\n", rc);
        return 1;
    }

    /* Connect to SSH server */
    sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    sin.sin_family = AF_INET;
    if (INADDR_NONE == (sin.sin_addr.s_addr = inet_addr(server_ip))) {
        fprintf(stderr, "inet_addr: Invalid IP address \"%s\"\n", server_ip);
        return -1;
    }
    sin.sin_port = htons(830);
    if (connect(sock, (struct sockaddr*)(&sin),
                sizeof(struct sockaddr_in)) != 0) {
        fprintf(stderr, "Failed to connect to %s!\n", inet_ntoa(sin.sin_addr));
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
    if(argc > 4) {
        if ((auth & AUTH_PASSWORD) && !strcasecmp(argv[4], "-p"))
            auth = AUTH_PASSWORD;
        if ((auth & AUTH_PUBLICKEY) && !strcasecmp(argv[4], "-k"))
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
            fprintf(stderr, "Authentication by public key failed!\n");
            goto shutdown;
        }
        fprintf(stderr, "Authentication by public key succeeded.\n");
    } else {
        fprintf(stderr, "No supported authentication methods found!\n");
        goto shutdown;
    }

    /* open a channel */
    channel = libssh2_channel_open_session(session);
    if (!channel) {
        fprintf(stderr, "Could not open the channel!\n"
                "(Note that this can be a problem at the server!"
                " Please review the server logs.)\n");
        goto shutdown;
    }

    /* execute the subsystem on our channel */
    if (libssh2_channel_subsystem(channel, "netconf")) {
        fprintf(stderr, "Could not execute the \"netconf\" subsystem!\n"
                "(Note that this can be a problem at the server!"
                " Please review the server logs.)\n");
        goto shutdown;
    }

    /* NETCONF: http://tools.ietf.org/html/draft-ietf-netconf-ssh-06 */

    fprintf(stderr, "Sending NETCONF client <hello>\n");
    snprintf(buf, sizeof(buf),
      "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
      "<hello>"
      "<capabilities>"
      "<capability>urn:ietf:params:xml:ns:netconf:base:1.0</capability>"
      "</capabilities>"
      "</hello>\n"
      "]]>]]>\n%n", (int *)&len);
    if (-1 == netconf_write(channel, buf, len))
        goto shutdown;

    fprintf(stderr, "Reading NETCONF server <hello>\n");
    len = netconf_read_until(channel, "</hello>", buf, sizeof(buf));
    if (-1 == len)
        goto shutdown;

    fprintf(stderr, "Got %d bytes:\n----------------------\n%s", (int)len, buf);

    fprintf(stderr, "Sending NETCONF <rpc>\n");
    snprintf(buf, sizeof(buf),
      "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
      "<rpc xmlns=\"urn:ietf:params:xml:ns:netconf:base:1.0\">"
      "<get-interface-information><terse/></get-interface-information>"
      "</rpc>\n"
      "]]>]]>\n%n", (int *)&len);
    if (-1 == netconf_write(channel, buf, len))
        goto shutdown;

    fprintf(stderr, "Reading NETCONF <rpc-reply>\n");
    len = netconf_read_until(channel, "</rpc-reply>", buf, sizeof(buf));
    if (-1 == len)
        goto shutdown;

    fprintf(stderr, "Got %d bytes:\n----------------------\n%s", (int)len, buf);

shutdown:
    if (channel)
        libssh2_channel_free(channel);
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
