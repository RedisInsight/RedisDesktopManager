/*
 *
 * Sample showing how to makes SSH2 with X11 Forwarding works.
 *
 * Usage :
 * "ssh2 host user password [DEBUG]"
 */

#include <string.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/un.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <termios.h>

#include <libssh2.h>

#define _PATH_UNIX_X "/tmp/.X11-unix/X%d"

/*
 * Chained list that contains channels and associated X11 socket for each X11
 * connections
 */
struct chan_X11_list {
    LIBSSH2_CHANNEL  *chan;
    int               sock;
    struct chan_X11_list *next;
};

struct chan_X11_list * gp_x11_chan = NULL;
struct termios         _saved_tio;

/*
 * Utility function to remove a Node of the chained list
 */
static void remove_node(struct chan_X11_list *elem)
{
    struct chan_X11_list *current_node = NULL;

    current_node = gp_x11_chan;

    if (gp_x11_chan == elem) {
        /* Removing the only one element in the list */
        free(gp_x11_chan);
        gp_x11_chan = NULL;
    }

    while( current_node->next != NULL) {
        if (current_node->next ==elem) {
            current_node->next = current_node->next->next;
            current_node = current_node->next;
            free(current_node);
            break;
        }
    }
}


static void session_shutdown(LIBSSH2_SESSION *session)
{
    libssh2_session_disconnect(session,
                                "Session Shutdown, Thank you for playing");
    libssh2_session_free(session);
}

static int _raw_mode(void)
{
    int rc;
    struct termios tio;

    rc = tcgetattr(fileno(stdin), &tio);
    if (rc != -1) {
        _saved_tio = tio;
        /* do the equivalent of cfmakeraw() manually, to build on Solaris */
        tio.c_iflag &= ~(IGNBRK|BRKINT|PARMRK|ISTRIP|INLCR|IGNCR|ICRNL|IXON);
        tio.c_oflag &= ~OPOST;
        tio.c_lflag &= ~(ECHO|ECHONL|ICANON|ISIG|IEXTEN);
        tio.c_cflag &= ~(CSIZE|PARENB);
        tio.c_cflag |= CS8;
        rc = tcsetattr(fileno(stdin), TCSADRAIN, &tio);
    }
    return rc;
}

static int _normal_mode(void)
{
    int rc;
    rc = tcsetattr(fileno(stdin), TCSADRAIN, &_saved_tio);
    return rc;
}

/*
 * CallBack to initialize the forwarding.
 * Save the channel to loop on it, save the X11 forwarded socket to send
 * and receive info from our X server.
 */
static void x11_callback(LIBSSH2_SESSION *session, LIBSSH2_CHANNEL *channel,
                         char *shost, int sport, void **abstract)
{
    const char * display      = NULL;
    char * ptr          = NULL;
    char * temp_buff    = NULL;
    int   display_port = 0;
    int   sock         = 0;
    int   rc           = 0;
    struct sockaddr_un addr;
    struct chan_X11_list *new;
    struct chan_X11_list *chan_iter;

    /*
     * Connect to the display
     * Inspired by x11_connect_display in openssh
     */
    display = getenv("DISPLAY");
    if ( display != NULL) {
        if (strncmp( display, "unix:", 5) == 0 ||
            display[0] == ':') {
            /* Connect to the local unix domain */
            ptr = strrchr(display, ':');
            temp_buff = (char *) calloc(strlen(ptr+1), sizeof(char));
            if (!temp_buff) {
                perror("calloc");
                return;
            }
            memcpy(temp_buff, ptr+1, strlen(ptr+1));
            display_port = atoi(temp_buff);
            free(temp_buff);

            sock = socket(AF_UNIX, SOCK_STREAM, 0);
            if (sock < 0)
                return;
            memset(&addr, 0, sizeof(addr));
            addr.sun_family = AF_UNIX;
            snprintf(addr.sun_path, sizeof(addr.sun_path),
                     _PATH_UNIX_X, display_port);
            rc = connect(sock, (struct sockaddr *) &addr, sizeof(addr));

            if (rc != -1){
                /* Connection Successfull */
                if (gp_x11_chan == NULL) {
                    /* Calloc ensure that gp_X11_chan is full of 0 */
                    gp_x11_chan = (struct chan_X11_list *)
                        calloc(1, sizeof(struct chan_X11_list));
                    gp_x11_chan->sock = sock;
                    gp_x11_chan->chan = channel;
                    gp_x11_chan->next = NULL;
                }
                else {
                    chan_iter = gp_x11_chan;
                    while (chan_iter->next != NULL)
                        chan_iter = chan_iter->next;
                    /* Create the new Node */
                    new = (struct chan_X11_list *)
                        malloc(sizeof(struct chan_X11_list));
                    new->sock = sock;
                    new->chan = channel;
                    new->next = NULL;
                    chan_iter->next = new;
                }
            }
            else
                close(sock);
        }
    }
    return;
}

/*
 * Send and receive Data for the X11 channel.
 * If the connection is closed, returns -1, 0 either.
 */
static int x11_send_receive(LIBSSH2_CHANNEL *channel, int sock)
{
    char * buf          = NULL;
    int    bufsize      = 8192;
    int    rc           = 0;
    int    nfds         = 1;
    LIBSSH2_POLLFD  *fds      = NULL;
    fd_set set;
    struct timeval timeval_out;
    timeval_out.tv_sec = 0;
    timeval_out.tv_usec = 0;


    FD_ZERO(&set);
    FD_SET(sock,&set);

    if ((buf = calloc (bufsize, sizeof(char))) == NULL)
        return 0;

    if ((fds = malloc (sizeof (LIBSSH2_POLLFD))) == NULL) {
        free(buf);
        return 0;
    }

    fds[0].type = LIBSSH2_POLLFD_CHANNEL;
    fds[0].fd.channel = channel;
    fds[0].events = LIBSSH2_POLLFD_POLLIN;
    fds[0].revents = LIBSSH2_POLLFD_POLLIN;

    rc = libssh2_poll(fds, nfds, 0);
    if (rc >0) {
        rc = libssh2_channel_read(channel, buf, bufsize);
        rc = write(sock, buf, rc);
    }

    rc = select(sock+1,&set,NULL,NULL,&timeval_out);
    if (rc > 0) {
        memset((void *)buf,0,bufsize);

        /* Data in sock*/
        rc = read(sock, buf, bufsize);
        if (rc > 0)
            rc = libssh2_channel_write(channel,buf, rc);
        else
            return -1;
    }

    free(fds);
    free(buf);
    if (libssh2_channel_eof (channel) == 1) {
        return -1;
    }
    return 0;
}

/*
 * Main, more than inspired by ssh2.c by Bagder
 */
int
main (int argc, char *argv[])
{
    unsigned long hostaddr = 0;
    int sock = 0;
    int rc = 0;
    struct sockaddr_in sin;
    LIBSSH2_SESSION *session;
    LIBSSH2_CHANNEL *channel;
    char *username = NULL;
    char *password = NULL;
    size_t bufsiz = 8193;
    char *buf = NULL;
    int set_debug_on = 0;
    int nfds = 1;
    LIBSSH2_POLLFD *fds = NULL;

    /* Chan List struct */
    struct chan_X11_list *current_node = NULL;

    /* Struct winsize for term size */
    struct winsize w_size;
    struct winsize w_size_bck;

    /* For select on stdin */
    fd_set set;
    struct timeval timeval_out;
    timeval_out.tv_sec = 0;
    timeval_out.tv_usec = 10;


    if (argc > 3) {
      hostaddr = inet_addr(argv[1]);
      username = argv[2];
      password = argv[3];
    }
    else {
        fprintf(stderr, "Usage: %s destination username password",
                argv[0]);
        return -1;
    }

    if (argc > 4) {
        set_debug_on = 1;
        fprintf (stderr, "DEBUG is ON: %d\n", set_debug_on);
    }

    rc = libssh2_init (0);
    if (rc != 0) {
        fprintf (stderr, "libssh2 initialization failed (%d)\n", rc);
        return 1;
    }

    sock = socket (AF_INET, SOCK_STREAM, 0);

    sin.sin_family = AF_INET;
    sin.sin_port = htons (22);
    sin.sin_addr.s_addr = hostaddr;

    rc = connect(sock, (struct sockaddr *) &sin,
                 sizeof(struct sockaddr_in));
    if (rc != 0) {
        fprintf (stderr, "Failed to established connection!\n");
        return -1;
    }
    /* Open a session */
    session = libssh2_session_init();
    rc      = libssh2_session_handshake(session, sock);
    if (rc != 0) {
        fprintf(stderr, "Failed Start the SSH session\n");
        return -1;
    }

    if (set_debug_on == 1)
        libssh2_trace(session, LIBSSH2_TRACE_CONN);

    /* Set X11 Callback */
    libssh2_session_callback_set(session, LIBSSH2_CALLBACK_X11,
                                 (void *)x11_callback);

    /* Authenticate via password */
    rc = libssh2_userauth_password(session, username, password);
    if (rc != 0) {
        fprintf(stderr, "Failed to authenticate\n");
        session_shutdown(session);
        close(sock);
        return -1;
    }

    /* Open a channel */
    channel  = libssh2_channel_open_session(session);
    if ( channel == NULL ) {
        fprintf(stderr, "Failed to open a new channel\n");
        session_shutdown(session);
        close(sock);
        return -1;
    }


    /* Request a PTY */
    rc = libssh2_channel_request_pty( channel, "xterm");
    if (rc != 0) {
        fprintf(stderr, "Failed to request a pty\n");
        session_shutdown(session);
        close(sock);
        return -1;
    }

    /* Request X11 */
    rc = libssh2_channel_x11_req(channel,0);
    if(rc!=0) {
        fprintf(stderr, "Failed to request X11 forwarding\n");
        session_shutdown(session);
        close(sock);
        return -1;
    }

    /* Request a shell */
    rc = libssh2_channel_shell(channel);
    if (rc!=0) {
        fprintf(stderr, "Failed to open a shell\n");
        session_shutdown(session);
        close(sock);
        return -1;
    }

    rc = _raw_mode();
    if (rc != 0) {
        fprintf(stderr, "Failed to entered in raw mode\n");
        session_shutdown(session);
        close(sock);
        return -1;
    }

    while (1) {

        FD_ZERO(&set);
        FD_SET(fileno(stdin),&set);

        /* Search if a resize pty has to be send */
        ioctl(fileno(stdin), TIOCGWINSZ, &w_size);
        if ((w_size.ws_row != w_size_bck.ws_row) ||
            (w_size.ws_col != w_size_bck.ws_col))   {
            w_size_bck = w_size;

            libssh2_channel_request_pty_size(channel,
                                             w_size.ws_col,
                                             w_size.ws_row);
        }

        if ((buf = calloc (bufsiz, sizeof(char))) == NULL)
            break;

        if ((fds = malloc (sizeof (LIBSSH2_POLLFD))) == NULL) {
            free(buf);
            break;
        }

        fds[0].type = LIBSSH2_POLLFD_CHANNEL;
        fds[0].fd.channel = channel;
        fds[0].events = LIBSSH2_POLLFD_POLLIN;
        fds[0].revents = LIBSSH2_POLLFD_POLLIN;

        rc = libssh2_poll(fds, nfds, 0);
        if (rc >0) {
            rc = libssh2_channel_read(channel, buf,sizeof(buf));
            fprintf(stdout, "%s", buf);
            fflush(stdout);
        }

        /* Looping on X clients */
        if (gp_x11_chan != NULL) {
            current_node = gp_x11_chan;
        }
        else
            current_node = NULL;

        while (current_node != NULL) {
            struct chan_X11_list *next_node;
            rc = x11_send_receive(current_node->chan, current_node->sock);
            next_node = current_node->next;
            if (rc == -1){
                shutdown(current_node->sock,SHUT_RDWR);
                close(current_node->sock);
                remove_node(current_node);
            }

            current_node = next_node;
        }


        rc = select(fileno(stdin)+1,&set,NULL,NULL,&timeval_out);
        if (rc > 0) {
            /* Data in stdin*/
            rc = read(fileno(stdin), buf,1);
            if (rc > 0)
                libssh2_channel_write(channel,buf, sizeof(buf));
        }

        free (fds);
        free (buf);

        if (libssh2_channel_eof (channel) == 1) {
          break;
        }
    }

    if (channel) {
      libssh2_channel_free (channel);
      channel = NULL;
    }
    _normal_mode();

    libssh2_exit();

    return 0;
}
