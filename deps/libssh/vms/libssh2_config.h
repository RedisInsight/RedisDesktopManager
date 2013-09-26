#ifndef LIBSSH2_CONFIG_H
#ifdef __VMS

#define LIBSSH2_CONFIG_H

/* VMS specific libssh2_config.h
 */

#define ssize_t SSIZE_T

typedef unsigned int uint32_t ;
typedef unsigned int socklen_t; /* missing in headers on VMS */

/* Have's */

#define HAVE_UNISTD_H
#define HAVE_INTTYPES_H
#define HAVE_SYS_TIME_H
#define HAVE_SELECT
#define HAVE_UIO

#define HAVE_SYS_SOCKET.H
#define HAVE_NETINET_IN_H
#define HAVE_ARPA_INET_H

#define POSIX_C_SOURCE

/* Enable the possibility of using tracing */
 
#define LIBSSH2DEBUG 1

/* For selection of proper block/unblock function in session.c */

#define HAVE_FIONBIO

#include <stropts.h>

/* In VMS TCP/IP Services and some BSD variants SO_STATE retrieves 
 * a bitmask revealing amongst others the blocking state of the 
 * socket. On VMS the bits are undocumented, but  SS_NBIO
 * works, I did not test the other bits. Below bitdefs are 
 * from Berkely source socketvar.h at   
 * http://ftp.fibranet.cat/UnixArchive/PDP-11/Trees/2.11BSD/sys/h/socketvar.h
 *  Socket state bits.
 *  #define SS_NOFDREF          0x001    no file table ref any more 
 *  #define SS_ISCONNECTED      0x002    socket connected to a peer 
 *  #define SS_ISCONNECTING     0x004    in process of connecting to peer 
 *  #define SS_ISDISCONNECTING  0x008    in process of disconnecting 
 *  #define SS_CANTSENDMORE     0x010    can't send more data to peer 
 *  #define SS_CANTRCVMORE      0x020    can't receive more data from peer 
 *  #define SS_RCVATMARK        0x040    at mark on input 
 *  #define SS_PRIV             0x080    privileged for broadcast, raw... 
 *  #define SS_NBIO             0x100    non-blocking ops 
 *  #define SS_ASYNC            0x200    async i/o notify 
 *
 */

#ifdef SO_STATE

/* SO_STATE is defined in stropts.h  by DECC
 * When running on Multinet, SO_STATE renders a protocol
 * not started error. Functionally this has no impact,
 * apart from libssh2 not being able to restore the socket
 * to the proper blocking/non-blocking state.  
 */

#define SS_NBIO         0x100 

#endif

/* Compile in zlib support. We link against gnv$libzshr, as available
 * on encompasserve.com.
 */

#define LIBSSH2_HAVE_ZLIB

/* Enable newer diffie-hellman-group-exchange-sha1 syntax */

#define LIBSSH2_DH_GEX_NEW 1

#endif /* __VMS */
#endif /* LIBSSH2_CONFIG_H */                             
