Things TODO
===========

* Fix the numerous malloc+copy operations for sending data, see "Buffering
  Improvements" below for details

* make sure the windowing code adapts better to slow situations so that it
  doesn't then use as much memory as today. Possibly by an app-controllable
  "Window mode"?

* Decrease the number of mallocs. Everywhere. Will get easier once the
  buffering improvements have been done.

* Use SO_NOSIGPIPE for Mac OS/BSD systems where MSG_NOSIGNAL doesn't
  exist/work

* Extend the test suite to actually test lots of aspects of libssh2

* Fix all compiler warnings (some can't be done without API changes)

* Expose error messages sent by the server

* select() is troublesome with libssh2 when using multiple channels over
  the same session. See "New Transport API" below for more details.

At next SONAME bump
===================

* stop using #defined macros as part of the official API. The macros should
  either be turned into real functions or discarded from the API.

* fix the parts of the API where object pointers and function pointers are
  mixed like libssh2_session_callback_set()

* remove the following functions from the API/ABI

  libssh2_base64_decode()
  libssh2_session_flag()
  libssh2_channel_handle_extended_data()
  libssh2_channel_receive_window_adjust()
  libssh2_poll()
  libssh2_poll_channel_read()
  libssh2_session_startup() (libssh2_session_handshake() is the replacement)
  libssh2_banner_set() (libssh2_session_banner_set() is the repacement)

* Rename a few function:

  libssh2_hostkey_hash => libssh2_session_hostkey_hash
  libssh2_banner_set => libssh2_session_banner_set

* change 'int' to 'libssh2_socket_t' in the public API for sockets.

* Use 'size_t' for string lengths in all functions.

* Add a comment field to struct libssh2_knownhost.

* remove the existing libssh2_knownhost_add() function and rename
  libssh2_knownhost_addc to become the new libssh2_knownhost_add instead

* remove the existing libssh2_scp_send_ex() function and rename
  libssh2_scp_send64 to become the new libssh2_scp_send instead.

* remove the existing libssh2_knownhost_check() functin and rename
  libssh2_knownhost_checkp() to become the new libssh2_knownhost_check instead

Buffering Improvements
======================

transport_write

  - If this function gets called with a total packet size that is larger than
  32K, it should create more than one SSH packet so that it keeps the largest
  one below 32K

sftp_write

  - should not copy/allocate anything for the data, only create a header chunk
  and pass on the payload data to channel_write "pointed to"

New Transport API
=================

THE PROBLEM

The problem in a nutshell is that when an application opens up multiple
channels over a single session, those are all using the same socket. If the
application is then using select() to wait for traffic (like any sensible app
does) and wants to act on the data when select() tells there is something to
for example read, what does an application do?

With our current API, you have to loop over all the channels and read from
them to see if they have data. This effectively makes blocking reads
impossible. If the app has many channels in a setup like this, it even becomes
slow. (The original API had the libssh2_poll_channel_read() and libssh2_poll()
to somewhat overcome this hurdle, but they too have pretty much the same
problems plus a few others.)

Traffic in the other direction is similarly limited: the app has to try
sending to all channels, even though some of them may very well not accept any
data at that point.

A SOLUTION

I suggest we introduce two new helper functions:

 libssh2_transport_read()

 - Read "a bunch" of data from the given socket and returns information to the
   app about what channels that are now readable (ie they will not block when
   read from). The function can be called over and over and it will repeatedly
   return info about what channels that are readable at that moment.

 libssh2_transport_write()

 - Returns information about what channels that are writable, in the sense
   that they have windows set from the remote side that allows data to get
   sent. Writing to one of those channels will not block. Of course, the
   underlying socket may only accept a certain amount of data, so at the first
   short return, nothing more should be attempted to get sent until select()
   (or equivalent) has been used on the master socket again.

I haven't yet figured out a sensible API for how these functions should return
that info, but if we agree on the general principles I guess we can work that
out.

VOLUNTARY

  I wanted to mention that these two helper functions would not be mandatory
  in any way. They would just be there for those who want them, and existing
  programs can remain using the old functions only if they prefer to.

New SFTP API
============

PURPOSE

 Provide API functions that explicitly tells at once that a (full) SFTP file
 transfer is wanted, to allow libssh2 to leverage on that knowledge to speed
 up things internally. It can for example do read ahead, buffer writes (merge
 small writes into larger chunks), better tune the SSH window and more. This
 sort of API is already provided for SCP transfers.

API

 New functions:

    LIBSSH2_SFTP_HANDLE *libssh2_sftp_send(SFTP_SESSION *sftp,
                                           uint64_t filesize,
                                           char *remote_path,
                                           size_t remote_path_len,
                                           long mode);

 Tell libssh2 that a local file with a given size is about to get sent to
 the SFTP server.

    LIBSSH2_SFTP_HANDLE *libssh2_sftp_recv();

 Tell libssh2 that a remote file is requested to get downloaded from the SFTP
 server.

 Only the setup of the file transfer is different from an application's point
 of view. Depending on direction of the transfer(s), the following already
 existing functions should then be used until the transfer is complete:

 libssh2_sftp_read()
 libssh2_sftp_write()

HOW TO USE

 1. Setup the transfer using one of the two new functions.

 2. Loop through the reading or writing of data.

 3. Cleanup the transfer
