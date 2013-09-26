
dnl **********************************************************************
dnl CURL_DETECT_ICC ([ACTION-IF-YES])
dnl
dnl check if this is the Intel ICC compiler, and if so run the ACTION-IF-YES
dnl sets the $ICC variable to "yes" or "no"
dnl **********************************************************************
AC_DEFUN([CURL_DETECT_ICC],
[
    ICC="no"
    AC_MSG_CHECKING([for icc in use])
    if test "$GCC" = "yes"; then
       dnl check if this is icc acting as gcc in disguise
       AC_EGREP_CPP([^__INTEL_COMPILER], [__INTEL_COMPILER],
         dnl action if the text is found, this it has not been replaced by the
         dnl cpp
         ICC="no",
         dnl the text was not found, it was replaced by the cpp
         ICC="yes"
         AC_MSG_RESULT([yes])
         [$1]
       )
    fi
    if test "$ICC" = "no"; then
        # this is not ICC
        AC_MSG_RESULT([no])
    fi
])

dnl We create a function for detecting which compiler we use and then set as
dnl pendantic compiler options as possible for that particular compiler. The
dnl options are only used for debug-builds.

AC_DEFUN([CURL_CC_DEBUG_OPTS],
[
    if test "z$ICC" = "z"; then
      CURL_DETECT_ICC
    fi

    if test "$GCC" = "yes"; then

       dnl figure out gcc version!
       AC_MSG_CHECKING([gcc version])
       gccver=`$CC -dumpversion`
       num1=`echo $gccver | cut -d . -f1`
       num2=`echo $gccver | cut -d . -f2`
       gccnum=`(expr $num1 "*" 100 + $num2) 2>/dev/null`
       AC_MSG_RESULT($gccver)

       if test "$ICC" = "yes"; then
         dnl this is icc, not gcc.

         dnl ICC warnings we ignore:
         dnl * 269 warns on our "%Od" printf formatters for curl_off_t output:
         dnl   "invalid format string conversion"
         dnl * 279 warns on static conditions in while expressions
         dnl * 981 warns on "operands are evaluated in unspecified order"
         dnl * 1418 "external definition with no prior declaration"
         dnl * 1419 warns on "external declaration in primary source file"
         dnl   which we know and do on purpose.

         WARN="-wd279,269,981,1418,1419"

         if test "$gccnum" -gt "600"; then
            dnl icc 6.0 and older doesn't have the -Wall flag
            WARN="-Wall $WARN"
         fi
       else dnl $ICC = yes
         dnl this is a set of options we believe *ALL* gcc versions support:
         WARN="-W -Wall -Wwrite-strings -pedantic -Wpointer-arith -Wnested-externs -Winline -Wmissing-prototypes"

         dnl -Wcast-align is a bit too annoying on all gcc versions ;-)

         if test "$gccnum" -ge "207"; then
           dnl gcc 2.7 or later
           WARN="$WARN -Wmissing-declarations"
         fi

         if test "$gccnum" -gt "295"; then
           dnl only if the compiler is newer than 2.95 since we got lots of
           dnl "`_POSIX_C_SOURCE' is not defined" in system headers with
           dnl gcc 2.95.4 on FreeBSD 4.9!
           WARN="$WARN -Wundef -Wno-long-long -Wsign-compare"
         fi

         if test "$gccnum" -ge "296"; then
           dnl gcc 2.96 or later
           WARN="$WARN -Wfloat-equal"
         fi

         if test "$gccnum" -gt "296"; then
           dnl this option does not exist in 2.96
           WARN="$WARN -Wno-format-nonliteral"
         fi

         dnl -Wunreachable-code seems totally unreliable on my gcc 3.3.2 on
         dnl on i686-Linux as it gives us heaps with false positives.
         dnl Also, on gcc 4.0.X it is totally unbearable and complains all
         dnl over making it unusable for generic purposes. Let's not use it.

         if test "$gccnum" -ge "303"; then
           dnl gcc 3.3 and later
           WARN="$WARN -Wendif-labels -Wstrict-prototypes"
         fi

         if test "$gccnum" -ge "304"; then
           # try these on gcc 3.4
           WARN="$WARN -Wdeclaration-after-statement"
         fi

         for flag in $CPPFLAGS; do
           case "$flag" in
            -I*)
              dnl Include path, provide a -isystem option for the same dir
              dnl to prevent warnings in those dirs. The -isystem was not very
              dnl reliable on earlier gcc versions.
              add=`echo $flag | sed 's/^-I/-isystem /g'`
              WARN="$WARN $add"
              ;;
           esac
         done

       fi dnl $ICC = no

       CFLAGS="$CFLAGS $WARN"

      AC_MSG_NOTICE([Added this set of compiler options: $WARN])

    else dnl $GCC = yes

      AC_MSG_NOTICE([Added no extra compiler options])

    fi dnl $GCC = yes

    dnl strip off optimizer flags
    NEWFLAGS=""
    for flag in $CFLAGS; do
      case "$flag" in
      -O*)
        dnl echo "cut off $flag"
        ;;
      *)
        NEWFLAGS="$NEWFLAGS $flag"
        ;;
      esac
    done
    CFLAGS=$NEWFLAGS

]) dnl end of AC_DEFUN()

dnl CURL_CHECK_NONBLOCKING_SOCKET
dnl -------------------------------------------------
dnl Check for how to set a socket to non-blocking state. There seems to exist
dnl four known different ways, with the one used almost everywhere being POSIX
dnl and XPG3, while the other different ways for different systems (old BSD,
dnl Windows and Amiga).
dnl
dnl There are two known platforms (AIX 3.x and SunOS 4.1.x) where the
dnl O_NONBLOCK define is found but does not work. This condition is attempted
dnl to get caught in this script by using an excessive number of #ifdefs...
dnl
AC_DEFUN([CURL_CHECK_NONBLOCKING_SOCKET],
[
  AC_MSG_CHECKING([non-blocking sockets style])

  AC_TRY_COMPILE([
/* headers for O_NONBLOCK test */
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
],[
/* try to compile O_NONBLOCK */

#if defined(sun) || defined(__sun__) || defined(__SUNPRO_C) || defined(__SUNPRO_CC)
# if defined(__SVR4) || defined(__srv4__)
#  define PLATFORM_SOLARIS
# else
#  define PLATFORM_SUNOS4
# endif
#endif
#if (defined(_AIX) || defined(__xlC__)) && !defined(_AIX41)
# define PLATFORM_AIX_V3
#endif

#if defined(PLATFORM_SUNOS4) || defined(PLATFORM_AIX_V3) || defined(__BEOS__)
#error "O_NONBLOCK does not work on this platform"
#endif
  int socket;
  int flags = fcntl(socket, F_SETFL, flags | O_NONBLOCK);
],[
dnl the O_NONBLOCK test was fine
nonblock="O_NONBLOCK"
AC_DEFINE(HAVE_O_NONBLOCK, 1, [use O_NONBLOCK for non-blocking sockets])
],[
dnl the code was bad, try a different program now, test 2

  AC_TRY_COMPILE([
/* headers for FIONBIO test */
#include <unistd.h>
#include <stropts.h>
],[
/* FIONBIO source test (old-style unix) */
 int socket;
 int flags = ioctl(socket, FIONBIO, &flags);
],[
dnl FIONBIO test was good
nonblock="FIONBIO"
AC_DEFINE(HAVE_FIONBIO, 1, [use FIONBIO for non-blocking sockets])
],[
dnl FIONBIO test was also bad
dnl the code was bad, try a different program now, test 3

  AC_TRY_COMPILE([
/* headers for ioctlsocket test (Windows) */
#undef inline
#ifdef HAVE_WINDOWS_H
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#ifdef HAVE_WINSOCK2_H
#include <winsock2.h>
#else
#ifdef HAVE_WINSOCK_H
#include <winsock.h>
#endif
#endif
#endif
],[
/* ioctlsocket source code */
 SOCKET sd;
 unsigned long flags = 0;
 sd = socket(0, 0, 0);
 ioctlsocket(sd, FIONBIO, &flags);
],[
dnl ioctlsocket test was good
nonblock="ioctlsocket"
AC_DEFINE(HAVE_IOCTLSOCKET, 1, [use ioctlsocket() for non-blocking sockets])
],[
dnl ioctlsocket didnt compile!, go to test 4

  AC_TRY_LINK([
/* headers for IoctlSocket test (Amiga?) */
#include <sys/ioctl.h>
],[
/* IoctlSocket source code */
 int socket;
 int flags = IoctlSocket(socket, FIONBIO, (long)1);
],[
dnl ioctlsocket test was good
nonblock="IoctlSocket"
AC_DEFINE(HAVE_IOCTLSOCKET_CASE, 1, [use Ioctlsocket() for non-blocking sockets])
],[
dnl Ioctlsocket didnt compile, do test 5!
  AC_TRY_COMPILE([
/* headers for SO_NONBLOCK test (BeOS) */
#include <socket.h>
],[
/* SO_NONBLOCK source code */
 long b = 1;
 int socket;
 int flags = setsockopt(socket, SOL_SOCKET, SO_NONBLOCK, &b, sizeof(b));
],[
dnl the SO_NONBLOCK test was good
nonblock="SO_NONBLOCK"
AC_DEFINE(HAVE_SO_NONBLOCK, 1, [use SO_NONBLOCK for non-blocking sockets])
],[
dnl test 5 didnt compile!
nonblock="nada"
AC_DEFINE(HAVE_DISABLED_NONBLOCKING, 1, [disabled non-blocking sockets])
])
dnl end of fifth test

])
dnl end of forth test

])
dnl end of third test

])
dnl end of second test

])
dnl end of non-blocking try-compile test
  AC_MSG_RESULT($nonblock)

  if test "$nonblock" = "nada"; then
    AC_MSG_WARN([non-block sockets disabled])
  fi
])

dnl CURL_CHECK_NEED_REENTRANT_SYSTEM
dnl -------------------------------------------------
dnl Checks if the preprocessor _REENTRANT definition
dnl must be unconditionally done for this platform.
dnl Internal macro for CURL_CONFIGURE_REENTRANT.

AC_DEFUN([CURL_CHECK_NEED_REENTRANT_SYSTEM], [
  case $host in
    *-*-solaris* | *-*-hpux*)
      tmp_need_reentrant="yes"
      ;;
    *)
      tmp_need_reentrant="no"
      ;;
  esac
])


dnl CURL_CONFIGURE_FROM_NOW_ON_WITH_REENTRANT
dnl -------------------------------------------------
dnl This macro ensures that configuration tests done
dnl after this will execute with preprocessor symbol
dnl _REENTRANT defined. This macro also ensures that
dnl the generated config file defines NEED_REENTRANT
dnl and that in turn setup.h will define _REENTRANT.
dnl Internal macro for CURL_CONFIGURE_REENTRANT.

AC_DEFUN([CURL_CONFIGURE_FROM_NOW_ON_WITH_REENTRANT], [
AC_DEFINE(NEED_REENTRANT, 1,
  [Define to 1 if _REENTRANT preprocessor symbol must be defined.])
cat >>confdefs.h <<_EOF
#ifndef _REENTRANT
#  define _REENTRANT
#endif
_EOF
])


dnl CURL_CONFIGURE_REENTRANT
dnl -------------------------------------------------
dnl This first checks if the preprocessor _REENTRANT
dnl symbol is already defined. If it isn't currently
dnl defined a set of checks are performed to verify
dnl if its definition is required to make visible to
dnl the compiler a set of *_r functions. Finally, if
dnl _REENTRANT is already defined or needed it takes
dnl care of making adjustments necessary to ensure
dnl that it is defined equally for further configure
dnl tests and generated config file.

AC_DEFUN([CURL_CONFIGURE_REENTRANT], [
  AC_PREREQ([2.50])dnl
  #
  AC_MSG_CHECKING([if _REENTRANT is already defined])
  AC_COMPILE_IFELSE([
    AC_LANG_PROGRAM([[
    ]],[[
#ifdef _REENTRANT
      int dummy=1;
#else
      force compilation error
#endif
    ]])
  ],[
    AC_MSG_RESULT([yes])
    tmp_reentrant_initially_defined="yes"
  ],[
    AC_MSG_RESULT([no])
    tmp_reentrant_initially_defined="no"
  ])
  #
  if test "$tmp_reentrant_initially_defined" = "no"; then
    AC_MSG_CHECKING([if _REENTRANT is actually needed])
    CURL_CHECK_NEED_REENTRANT_SYSTEM

    if test "$tmp_need_reentrant" = "yes"; then
      AC_MSG_RESULT([yes])
    else
      AC_MSG_RESULT([no])
    fi
  fi
  #
  AC_MSG_CHECKING([if _REENTRANT is onwards defined])
  if test "$tmp_reentrant_initially_defined" = "yes" ||
    test "$tmp_need_reentrant" = "yes"; then
    CURL_CONFIGURE_FROM_NOW_ON_WITH_REENTRANT
    AC_MSG_RESULT([yes])
  else
    AC_MSG_RESULT([no])
  fi
  #
])

