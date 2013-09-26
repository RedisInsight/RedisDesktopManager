libssh2 - SSH2 library
======================

libssh2 is a library implementing the SSH2 protocol, available under
the revised BSD license.

Web site: http://www.libssh2.org/

Mailing list: http://cool.haxx.se/mailman/listinfo/libssh2-devel

Generic installation instructions are in INSTALL.  Some ./configure
options deserve additional comments:

	* --enable-crypt-none

		The SSH2 Transport allows for unencrypted data
		transmission using the "none" cipher.  Because this is
		such a huge security hole, it is typically disabled on
		SSH2 implementations and is disabled in libssh2 by
		default as well.

		Enabling this option will allow for "none" as a
		negotiable method, however it still requires that the
		method be advertized by the remote end and that no
		more-preferable methods are available.

	* --enable-mac-none

		The SSH2 Transport also allows implementations to
		forego a message authentication code.  While this is
		less of a security risk than using a "none" cipher, it
		is still not recommended as disabling MAC hashes
		removes a layer of security.

		Enabling this option will allow for "none" as a
		negotiable method, however it still requires that the
		method be advertized by the remote end and that no
		more-preferable methods are available.

	* --disable-gex-new

		The diffie-hellman-group-exchange-sha1 (dh-gex) key
		exchange method originally defined an exchange
		negotiation using packet type 30 to request a
		generation pair based on a single target value.  Later
		refinement of dh-gex provided for range and target
		values.  By default libssh2 will use the newer range
		method.

		If you experience trouble connecting to an old SSH
		server using dh-gex, try this option to fallback on
		the older more reliable method.

  	* --with-libgcrypt
  	* --without-libgcrypt
	* --with-libgcrypt-prefix=DIR

		libssh2 can use the Libgcrypt library
		(http://www.gnupg.org/) for cryptographic operations.
		Either Libgcrypt or OpenSSL is required.

		Configure will attempt to locate Libgcrypt
		automatically.

 		If your installation of Libgcrypt is in another
		location, specify it using --with-libgcrypt-prefix.

	* --with-openssl
	* --without-openssl
	* --with-libssl-prefix=[DIR]

		libssh2 can use the OpenSSL library
		(http://www.openssl.org) for cryptographic operations.
		Either Libgcrypt or OpenSSL is required.

		Configure will attempt to locate OpenSSL in the
		default location.

		If your installation of OpenSSL is in another
		location, specify it using --with-libssl-prefix.

	* --with-libz
	* --without-libz
	* --with-libz-prefix=[DIR]

		If present, libssh2 will attempt to use the zlib
		(http://www.zlib.org) for payload compression, however
		zlib is not required.

		If your installation of Libz is in another location,
		specify it using --with-libz-prefix.

	* --enable-debug

		Will make the build use more pedantic and strict compiler
		options as well as enable the libssh2_trace() function (for
		showing debug traces).
