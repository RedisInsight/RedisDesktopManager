AUTOMAKE_OPTIONS = foreign nostdinc

EXTRA_DIST = libssh2_config.h.in

# samples
noinst_PROGRAMS = direct_tcpip ssh2 scp scp_nonblock scp_write		\
 scp_write_nonblock sftp sftp_nonblock sftp_write sftp_write_nonblock	\
 sftp_mkdir sftp_mkdir_nonblock sftp_RW_nonblock sftp_write_sliding	\
 sftpdir sftpdir_nonblock ssh2_exec ssh2_agent ssh2_echo sftp_append	\
 subsystem_netconf tcpip-forward

if HAVE_SYS_UN_H
noinst_PROGRAMS += x11
endif

AM_CPPFLAGS = -I$(top_srcdir)/include -I$(top_builddir)/example
LDADD = $(top_builddir)/src/libssh2.la
