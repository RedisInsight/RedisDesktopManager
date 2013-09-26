#########################################################################
#
## Makefile for building libssh2 (NetWare version - gnu make)
## Use: make [help|all|clean|dev|devclean|dist|distclean|lib|nlm|objclean]
##
## Hacked by: Guenter Knauf
#
#########################################################################

# Edit the path below to point to the base of your Novell NDK.
ifndef NDKBASE
NDKBASE	= c:/novell
endif

# Edit the path below to point to the base of your Zlib sources.
ifndef ZLIB_PATH
ZLIB_PATH = ../../zlib-1.2.7
endif

# Edit the path below to point to the base of your OpenSSL package.
ifndef OPENSSL_PATH
OPENSSL_PATH = ../../openssl-0.9.8x
endif

# Edit the path below to point to your Distribution folder.
ifndef DISTDIR
DISTDIR	= libssh2-$(LIBSSH2_VERSION_STR)-bin-nw
endif
DISTARC	= $(DISTDIR).zip

# Edit the path below to point to your Development folder.
ifndef DEVLDIR
DEVLDIR	= libssh2-$(LIBSSH2_VERSION_STR)-dev-nw
endif
DEVLARC	= $(DEVLDIR).zip

# Edit the vars below to change NLM target settings.
TARGET	= libssh2
VERSION	= $(LIBSSH2_VERSION)
COPYR	= Copyright (c) $(LIBSSH2_COPYRIGHT_STR)
WWWURL	= http://www.libssh2.org/
DESCR	= libssh2 $(LIBSSH2_VERSION_STR) ($(LIBARCH)) - $(WWWURL)
MTSAFE	= YES
STACK	= 64000
SCREEN	= none
EXPORTS	= @$(TARGET).imp

# Edit the var below to point to your lib architecture.
ifndef LIBARCH
LIBARCH = LIBC
endif

# must be equal to DEBUG or NDEBUG
ifndef DB
	DB	= NDEBUG
	# DB	= DEBUG
endif
# Optimization: -O<n> or debugging: -g
ifeq ($(DB),NDEBUG)
	OPT	= -O2
	OBJDIR	= release
else
	OPT	= -g
	OPT	+= -DLIBSSH2DEBUG
	OBJDIR	= debug
endif

# The following lines defines your compiler.
ifdef CWFolder
	METROWERKS = $(CWFolder)
endif
ifdef METROWERKS
	# MWCW_PATH = $(subst \,/,$(METROWERKS))/Novell Support
	MWCW_PATH = $(subst \,/,$(METROWERKS))/Novell Support/Metrowerks Support
	CC = mwccnlm
else
	CC = $(CROSSPREFIX)gcc
endif

# Here you can find a native Win32 binary of the original awk:
# http://www.gknw.net/development/prgtools/awk-20100523.zip
AWK	= awk
# If you want to mark the target as MTSAFE you will need a tool for
# generating the xdc data for the linker; here's a minimal tool:
# http://www.gknw.net/development/prgtools/mkxdc.zip
MPKXDC	= mkxdc
ZIP	= zip -qzr9

# Platform-dependent helper tool macros
ifeq ($(findstring /sh,$(SHELL)),/sh)
DEL	= rm -f $1
RMDIR	= rm -fr $1
MKDIR	= mkdir -p $1
COPY	= -cp -afv $1 $2
#COPYR	= -cp -afr $1/* $2
COPYR	= -rsync -aC $1/* $2
TOUCH	= touch $1
CAT	= cat
ECHONL	= echo ""
DL	= '
else
ifeq "$(OS)" "Windows_NT"
DEL	= -del 2>NUL /q /f $(subst /,\,$1)
RMDIR	= -rd 2>NUL /q /s $(subst /,\,$1)
else
DEL	= -del 2>NUL $(subst /,\,$1)
RMDIR	= -deltree 2>NUL /y $(subst /,\,$1)
endif     
MKDIR	= -md 2>NUL $(subst /,\,$1)
COPY	= -copy 2>NUL /y $(subst /,\,$1) $(subst /,\,$2)
COPYR	= -xcopy 2>NUL /q /y /e $(subst /,\,$1) $(subst /,\,$2)
TOUCH	= copy 2>&1>NUL /b $(subst /,\,$1) +,,
CAT	= type
ECHONL	= $(ComSpec) /c echo.
endif

# LIBARCH_U = $(shell $(AWK) 'BEGIN {print toupper(ARGV[1])}' $(LIBARCH))
LIBARCH_L = $(shell $(AWK) 'BEGIN {print tolower(ARGV[1])}' $(LIBARCH))

# Include the version info retrieved from libssh2.h
-include $(OBJDIR)/version.inc

# Global flags for all compilers
CFLAGS	+= $(OPT) -D$(DB) -DNETWARE -nostdinc
#CFLAGS	+= -DHAVE_CONFIG_H

ifeq ($(CC),mwccnlm)
LD	= mwldnlm
LDFLAGS	= -nostdlib $(PRELUDE) $(OBJL) -o $@ -commandfile
AR	= mwldnlm
ARFLAGS	= -nostdlib -type library -o
LIBEXT	= lib
CFLAGS	+= -msgstyle gcc -gccinc -inline on -opt nointrinsics -proc 586
CFLAGS	+= -relax_pointers
#CFLAGS	+= -w on
ifeq ($(LIBARCH),LIBC)
	PRELUDE = $(SDK_LIBC)/imports/libcpre.o
	CFLAGS += -align 4
else
	# PRELUDE = $(SDK_CLIB)/imports/clibpre.o
	# to avoid the __init_* / __deinit_* whoes dont use prelude from NDK
	PRELUDE = "$(MWCW_PATH)/libraries/runtime/prelude.obj"
	# CFLAGS += -include "$(MWCW_PATH)/headers/nlm_clib_prefix.h"
	CFLAGS += -align 1
endif
else
LD	= $(CROSSPREFIX)nlmconv
LDFLAGS	= -UT
AR	= $(CROSSPREFIX)ar
ARFLAGS	= -cq
LIBEXT	= a
RANLIB	= $(CROSSPREFIX)ranlib
CFLAGS	+= -m32
CFLAGS	+= -fno-builtin -fpcc-struct-return
CFLAGS	+= -fno-strict-aliasing
CFLAGS	+= -Wall # -pedantic
#CFLAGS	+= -Wno-pointer-sign
ifeq ($(LIBARCH),LIBC)
	PRELUDE = $(SDK_LIBC)/imports/libcpre.gcc.o
else
	# PRELUDE = $(SDK_CLIB)/imports/clibpre.gcc.o
	# to avoid the __init_* / __deinit_* whoes dont use prelude from NDK
	# http://www.gknw.net/development/mk_nlm/gcc_pre.zip
	PRELUDE = $(NDK_ROOT)/pre/prelude.o
	CFLAGS += -include $(NDKBASE)/nlmconv/genlm.h
endif
endif

LDLIBS	=

NDK_ROOT = $(NDKBASE)/ndk
SDK_CLIB = $(NDK_ROOT)/nwsdk
SDK_LIBC = $(NDK_ROOT)/libc
SNPRINTF = $(NDKBASE)/snprintf

INCLUDES = -I. -I../include

ifdef WITH_ZLIB
	INCLUDES += -I$(ZLIB_PATH)
ifdef LINK_STATIC
	LDLIBS += $(ZLIB_PATH)/nw/$(LIBARCH)/libz.$(LIBEXT)
else
	MODULES += libz.nlm
	IMPORTS += @$(ZLIB_PATH)/nw/$(LIBARCH)/libz.imp
endif
endif
INCLUDES += -I$(OPENSSL_PATH)/outinc_nw_$(LIBARCH_L)
LDLIBS += $(OPENSSL_PATH)/out_nw_$(LIBARCH_L)/ssl.$(LIBEXT)
LDLIBS += $(OPENSSL_PATH)/out_nw_$(LIBARCH_L)/crypto.$(LIBEXT)
IMPORTS += GetProcessSwitchCount RunningProcess

ifeq ($(LIBARCH),LIBC)
	INCLUDES += -I$(SDK_LIBC)/include
	# INCLUDES += -I$(SDK_LIBC)/include/nks
	# INCLUDES += -I$(SDK_LIBC)/include/winsock
	CFLAGS += -D_POSIX_SOURCE
else
	INCLUDES += -I$(SDK_CLIB)/include/nlm
	# INCLUDES += -I$(SDK_CLIB)/include/nlm/obsolete
	# INCLUDES += -I$(SDK_CLIB)/include
endif
CFLAGS	+= $(INCLUDES)

ifeq ($(MTSAFE),YES)
	XDCOPT = -n
endif
ifeq ($(MTSAFE),NO)
	XDCOPT = -u
endif
ifdef XDCOPT
	XDCDATA = $(OBJDIR)/$(TARGET).xdc
endif

vpath %.c . ../src

# include Makefile.inc to get CSOURCES define
include ../Makefile.inc

OBJECTS := $(patsubst %.c,%.o,$(CSOURCES))
ifeq ($(LIBARCH),CLIB)
# CLIB lacks of snprint() function - here's a replacement:
# http://www.ijs.si/software/snprintf/
OBJECTS += snprintf.o
vpath %.c $(SNPRINTF)
endif

OBJS	:= $(addprefix $(OBJDIR)/,$(OBJECTS))
OBJL	= $(OBJS) $(OBJDIR)/nwlib.o $(LDLIBS)

all: lib nlm

nlm: prebuild $(TARGET).nlm

lib: prebuild $(TARGET).$(LIBEXT)

prebuild: $(OBJDIR) $(OBJDIR)/version.inc libssh2_config.h

test: all
	$(MAKE) -C test

$(OBJDIR)/%.o: %.c
#	@echo Compiling $<
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR)/version.inc: ../get_ver.awk ../include/libssh2.h $(OBJDIR)
	@echo Creating $@
	@$(AWK) -f $^ > $@

dist: all $(DISTDIR) $(DISTDIR)/readme.txt
	@$(call MD, $(DISTDIR)/bin)
	@$(call CP, ../AUTHORS, $(DISTDIR))
	@$(call CP, ../COPYING, $(DISTDIR))
	@$(call CP, ../INSTALL, $(DISTDIR))
	@$(call CP, ../README, $(DISTDIR))
	@$(call CP, ../RELEASE-NOTES, $(DISTDIR))
	@$(call CP, $(TARGET).nlm, $(DISTDIR)/bin)
	@echo Creating $(DISTARC)
	@$(ZIP) $(DISTARC) $(DISTDIR)/* < $(DISTDIR)/readme.txt

dev: all $(DEVLDIR) $(DEVLDIR)/readme.txt
	@$(call MD, $(DEVLDIR)/bin)
	@$(call MD, $(DEVLDIR)/include)
	@$(call MD, $(DEVLDIR)/nw)
	@$(call CP, ../AUTHORS, $(DEVLDIR))
	@$(call CP, ../COPYING, $(DEVLDIR))
	@$(call CP, ../INSTALL, $(DEVLDIR))
	@$(call CP, ../README, $(DEVLDIR))
	@$(call CP, ../RELEASE-NOTES, $(DEVLDIR))
	@$(call CP, ../include/*.h, $(DEVLDIR)/include)
	@$(call CP, libssh2_config.h, $(DEVLDIR)/include)
	@$(call CP, $(TARGET).nlm, $(DEVLDIR)/bin)
	@$(call CP, $(TARGET).imp, $(DEVLDIR)/nw)
	@$(call CP, $(TARGET).$(LIBEXT), $(DEVLDIR)/nw)
	@echo Creating $(DEVLARC)
	@$(ZIP) $(DEVLARC) $(DEVLDIR)/* < $(DEVLDIR)/readme.txt

distclean: clean
	$(call RMDIR, $(DISTDIR))
	$(call DEL, $(DISTARC))

devclean: clean
	$(call RMDIR, $(DEVLDIR))
	$(call DEL, $(DEVLARC))

objclean:
	$(call RMDIR, $(OBJDIR))

testclean: clean
	$(MAKE) -C test clean

clean: objclean
	$(call DEL, libssh2_config.h)
	$(call DEL, $(TARGET).*)

$(OBJDIR):
	@$(call MKDIR, $@)

$(DISTDIR):
	@$(call MKDIR, $@)

$(DEVLDIR):
	@$(call MKDIR, $@)

$(TARGET).$(LIBEXT): $(OBJS)
	@echo Creating $@
	@$(call DEL, $@)
	@$(AR) $(ARFLAGS) $@ $^
ifdef RANLIB
	@$(RANLIB) $@
endif

$(TARGET).nlm: $(OBJDIR)/$(TARGET).def $(TARGET).imp $(OBJL) $(XDCDATA)
	@echo Linking $@
	@$(call DEL, $@)
	@$(LD) $(LDFLAGS) $<

$(OBJDIR)/%.xdc: GNUmakefile
	@echo Creating $@
	@$(MPKXDC) $(XDCOPT) $@

$(OBJDIR)/%.def: GNUmakefile
	@echo $(DL)# DEF file for linking with $(LD)$(DL) > $@
	@echo $(DL)# Do not edit this file - it is created by make!$(DL) >> $@
	@echo $(DL)# All your changes will be lost!!$(DL) >> $@
	@echo $(DL)#$(DL) >> $@
	@echo $(DL)copyright "$(COPYR)"$(DL) >> $@
	@echo $(DL)description "$(DESCR)"$(DL) >> $@
	@echo $(DL)version $(VERSION)$(DL) >> $@
ifdef NLMTYPE
	@echo $(DL)type $(NLMTYPE)$(DL) >> $@
endif
ifdef STACK
	@echo $(DL)stack $(STACK)$(DL) >> $@
endif
ifdef SCREEN
	@echo $(DL)screenname "$(SCREEN)"$(DL) >> $@
else
	@echo $(DL)screenname "DEFAULT"$(DL) >> $@
endif
ifeq ($(DB),DEBUG)
	@echo $(DL)debug$(DL) >> $@
endif
	@echo $(DL)threadname "$(TARGET)"$(DL) >> $@
ifdef XDCDATA
	@echo $(DL)xdcdata $(XDCDATA)$(DL) >> $@
endif
	@echo $(DL)flag_on 64$(DL) >> $@
ifeq ($(LIBARCH),CLIB)
	@echo $(DL)start _Prelude$(DL) >> $@
	@echo $(DL)exit _Stop$(DL) >> $@
	@echo $(DL)import @$(SDK_CLIB)/imports/clib.imp$(DL) >> $@
	@echo $(DL)import @$(SDK_CLIB)/imports/threads.imp$(DL) >> $@
	@echo $(DL)import @$(SDK_CLIB)/imports/nlmlib.imp$(DL) >> $@
	@echo $(DL)import @$(SDK_CLIB)/imports/socklib.imp$(DL) >> $@
	@echo $(DL)module clib$(DL) >> $@
else
	@echo $(DL)pseudopreemption$(DL) >> $@
	@echo $(DL)start _LibCPrelude$(DL) >> $@
	@echo $(DL)exit _LibCPostlude$(DL) >> $@
	@echo $(DL)check _LibCCheckUnload$(DL) >> $@
	@echo $(DL)import @$(SDK_LIBC)/imports/libc.imp$(DL) >> $@
	@echo $(DL)import @$(SDK_LIBC)/imports/netware.imp$(DL) >> $@
	@echo $(DL)module libc$(DL) >> $@
endif
ifdef MODULES
	@echo $(DL)module $(MODULES)$(DL) >> $@
endif
ifdef EXPORTS
	@echo $(DL)export $(EXPORTS)$(DL) >> $@
endif
ifdef IMPORTS
	@echo $(DL)import $(IMPORTS)$(DL) >> $@
endif
ifeq ($(LD),nlmconv)
	@echo $(DL)input $(OBJL)$(DL) >> $@
	@echo $(DL)input $(PRELUDE)$(DL) >> $@
	@echo $(DL)output $(TARGET).nlm$(DL) >> $@
endif

libssh2_config.h: GNUmakefile
	@echo Creating $@
	@echo $(DL)/* $@ for NetWare target.$(DL) > $@
	@echo $(DL)** Do not edit this file - it is created by make!$(DL) >> $@
	@echo $(DL)** All your changes will be lost!!$(DL) >> $@
	@echo $(DL)*/$(DL) >> $@
	@echo $(DL)#define VERSION "$(LIBSSH2_VERSION_STR)"$(DL) >> $@
	@echo $(DL)#define PACKAGE_BUGREPORT "http://sourceforge.net/projects/libssh2"$(DL) >> $@
ifeq ($(LIBARCH),CLIB)
	@echo $(DL)#define OS "i586-pc-clib-NetWare"$(DL) >> $@
	@echo $(DL)#define NETDB_USE_INTERNET 1$(DL) >> $@
	@echo $(DL)#define HAVE_STRICMP 1$(DL) >> $@
	@echo $(DL)#define socklen_t int$(DL) >> $@
	@echo $(DL)#define sleep(s) delay(1000 * s)$(DL) >> $@
	@echo $(DL)#define strcasecmp stricmp$(DL) >> $@
	@echo $(DL)#define strncasecmp strnicmp$(DL) >> $@
else
	@echo $(DL)#define OS "i586-pc-libc-NetWare"$(DL) >> $@
	@echo $(DL)#define HAVE_DLFCN_H 1$(DL) >> $@
	@echo $(DL)#define HAVE_DLOPEN 1$(DL) >> $@
	@echo $(DL)#define HAVE_FTRUNCATE 1$(DL) >> $@
	@echo $(DL)#define HAVE_GETTIMEOFDAY 1$(DL) >> $@
	@echo $(DL)#define HAVE_INET_PTON 1$(DL) >> $@
	@echo $(DL)#define HAVE_INTTYPES_H 1$(DL) >> $@
	@echo $(DL)#define HAVE_LIMITS_H 1$(DL) >> $@
	@echo $(DL)#define HAVE_LONGLONG 1$(DL) >> $@
	@echo $(DL)#define HAVE_STDINT_H 1$(DL) >> $@
	@echo $(DL)#define HAVE_STRCASECMP 1$(DL) >> $@
	@echo $(DL)#define HAVE_STRLCAT 1$(DL) >> $@
	@echo $(DL)#define HAVE_STRLCPY 1$(DL) >> $@
	@echo $(DL)#define HAVE_STRTOLL 1$(DL) >> $@
	@echo $(DL)#define HAVE_SYS_PARAM_H 1$(DL) >> $@
	@echo $(DL)#define HAVE_SYS_SELECT_H 1$(DL) >> $@
	@echo $(DL)#define HAVE_TERMIOS_H 1$(DL) >> $@
	@echo $(DL)#define HAVE_AF_INET6 1$(DL) >> $@
	@echo $(DL)#define HAVE_PF_INET6 1$(DL) >> $@
	@echo $(DL)#define HAVE_STRUCT_IN6_ADDR 1$(DL) >> $@
	@echo $(DL)#define HAVE_STRUCT_SOCKADDR_IN6 1$(DL) >> $@
	@echo $(DL)#define SIZEOF_STRUCT_IN6_ADDR 16$(DL) >> $@
ifdef ENABLE_IPV6
	@echo $(DL)#define ENABLE_IPV6 1$(DL) >> $@
endif
endif
	@echo $(DL)#define HAVE_ARPA_INET_H 1$(DL) >> $@
	@echo $(DL)#define HAVE_ASSERT_H 1$(DL) >> $@
	@echo $(DL)#define HAVE_CTYPE_H 1$(DL) >> $@
	@echo $(DL)#define HAVE_ERR_H 1$(DL) >> $@
	@echo $(DL)#define HAVE_ERRNO_H 1$(DL) >> $@
	@echo $(DL)#define HAVE_FCNTL_H 1$(DL) >> $@
	@echo $(DL)#define HAVE_FIONBIO 1$(DL) >> $@
	@echo $(DL)#define HAVE_GETHOSTBYADDR 1$(DL) >> $@
	@echo $(DL)#define HAVE_GETHOSTBYNAME 1$(DL) >> $@
	@echo $(DL)#define HAVE_GETPROTOBYNAME 1$(DL) >> $@
	@echo $(DL)#define HAVE_GMTIME_R 1$(DL) >> $@
	@echo $(DL)#define HAVE_INET_ADDR 1$(DL) >> $@
	@echo $(DL)#define HAVE_INET_NTOA 1$(DL) >> $@
	@echo $(DL)#define HAVE_LL 1$(DL) >> $@
	@echo $(DL)#define HAVE_LOCALTIME_R 1$(DL) >> $@
	@echo $(DL)#define HAVE_MALLOC_H 1$(DL) >> $@
	@echo $(DL)#define HAVE_NETINET_IN_H 1$(DL) >> $@
	@echo $(DL)#define HAVE_SELECT 1$(DL) >> $@
	@echo $(DL)#define HAVE_SETJMP_H 1$(DL) >> $@
	@echo $(DL)#define HAVE_SIGNAL 1$(DL) >> $@
	@echo $(DL)#define HAVE_SIGNAL_H 1$(DL) >> $@
	@echo $(DL)#define HAVE_SIG_ATOMIC_T 1$(DL) >> $@
	@echo $(DL)#define HAVE_SOCKET 1$(DL) >> $@
	@echo $(DL)#define HAVE_STDLIB_H 1$(DL) >> $@
	@echo $(DL)#define HAVE_STRDUP 1$(DL) >> $@
	@echo $(DL)#define HAVE_STRFTIME 1$(DL) >> $@
	@echo $(DL)#define HAVE_STRING_H 1$(DL) >> $@
	@echo $(DL)#define HAVE_STRSTR 1$(DL) >> $@
	@echo $(DL)#define HAVE_STRUCT_ADDRINFO 1$(DL) >> $@
	@echo $(DL)#define HAVE_STRUCT_TIMEVAL 1$(DL) >> $@
	@echo $(DL)#define HAVE_SYS_IOCTL_H 1$(DL) >> $@
	@echo $(DL)#define HAVE_SYS_STAT_H 1$(DL) >> $@
	@echo $(DL)#define HAVE_SYS_TIME_H 1$(DL) >> $@
	@echo $(DL)#define HAVE_TIME_H 1$(DL) >> $@
	@echo $(DL)#define HAVE_UNAME 1$(DL) >> $@
	@echo $(DL)#define HAVE_UNISTD_H 1$(DL) >> $@
	@echo $(DL)#define HAVE_UTIME 1$(DL) >> $@
	@echo $(DL)#define HAVE_UTIME_H 1$(DL) >> $@
	@echo $(DL)#define RETSIGTYPE void$(DL) >> $@
	@echo $(DL)#define SIZEOF_STRUCT_IN_ADDR 4$(DL) >> $@
	@echo $(DL)#define STDC_HEADERS 1$(DL) >> $@
	@echo $(DL)#define TIME_WITH_SYS_TIME 1$(DL) >> $@
	@echo $(DL)#define USE_SSLEAY 1$(DL) >> $@
	@echo $(DL)#define USE_OPENSSL 1$(DL) >> $@
	@echo $(DL)#define HAVE_OPENSSL_X509_H 1$(DL) >> $@
	@echo $(DL)#define HAVE_OPENSSL_SSL_H 1$(DL) >> $@
	@echo $(DL)#define HAVE_OPENSSL_RSA_H 1$(DL) >> $@
	@echo $(DL)#define HAVE_OPENSSL_PEM_H 1$(DL) >> $@
	@echo $(DL)#define HAVE_OPENSSL_ERR_H 1$(DL) >> $@
	@echo $(DL)#define HAVE_OPENSSL_CRYPTO_H 1$(DL) >> $@
	@echo $(DL)#define HAVE_OPENSSL_ENGINE_H 1$(DL) >> $@
	@echo $(DL)#define HAVE_O_NONBLOCK 1$(DL) >> $@
	@echo $(DL)#define HAVE_LIBSSL 1$(DL) >> $@
	@echo $(DL)#define HAVE_LIBCRYPTO 1$(DL) >> $@
	@echo $(DL)#define OPENSSL_NO_KRB5 1$(DL) >> $@
ifdef WITH_ZLIB
	@echo $(DL)#define HAVE_ZLIB_H 1$(DL) >> $@
	@echo $(DL)#define HAVE_LIBZ 1$(DL) >> $@
	@echo $(DL)#define LIBSSH2_HAVE_ZLIB 1$(DL) >> $@
endif
ifdef NW_WINSOCK
	@echo $(DL)#define HAVE_CLOSESOCKET 1$(DL) >> $@
else
	@echo $(DL)#define HAVE_SYS_TYPES_H 1$(DL) >> $@
	@echo $(DL)#define HAVE_SYS_SOCKET_H 1$(DL) >> $@
	@echo $(DL)#define HAVE_SYS_SOCKIO_H 1$(DL) >> $@
	@echo $(DL)#define HAVE_NETDB_H 1$(DL) >> $@
endif
ifdef OLD_NOVELLSDK
	@echo $(DL)#define socklen_t int$(DL) >> $@
endif
	@echo $(DL)#define LIBSSH2_DH_GEX_NEW 1$(DL) >> $@
ifeq ($(DB),DEBUG)
	@echo $(DL)#define LIBSSH2_DEBUG_CONNECTION 1$(DL) >> $@
	@echo $(DL)#define LIBSSH2_DEBUG_ERRORS 1$(DL) >> $@
	@echo $(DL)#define LIBSSH2_DEBUG_KEX 1$(DL) >> $@
	@echo $(DL)#define LIBSSH2_DEBUG_PUBLICKEY 1$(DL) >> $@
	@echo $(DL)#define LIBSSH2_DEBUG_SCP 1$(DL) >> $@
	@echo $(DL)#define LIBSSH2_DEBUG_SFTP 1$(DL) >> $@
	@echo $(DL)#define LIBSSH2_DEBUG_TRANSPORT 1$(DL) >> $@
	@echo $(DL)#define LIBSSH2_DEBUG_USERAUTH 1$(DL) >> $@
endif

libssh2.imp: GNUmakefile
	@echo Creating $@
	@echo $(DL)# $@ for NetWare target.$(DL) > $@
	@echo $(DL)# Do not edit this file - it is created by make!$(DL) >> $@
	@echo $(DL)# All your changes will be lost!!$(DL) >> $@
ifeq ($(LIBARCH),CLIB)
	@echo $(DL)  (CLIB_LIBSSH2)$(DL) >> $@
else
	@echo $(DL)  (LIBC_LIBSSH2)$(DL) >> $@
endif
	@echo $(DL)  libssh2_banner_set,$(DL) >> $@
	@echo $(DL)  libssh2_channel_close,$(DL) >> $@
	@echo $(DL)  libssh2_channel_direct_tcpip_ex,$(DL) >> $@
	@echo $(DL)  libssh2_channel_eof,$(DL) >> $@
	@echo $(DL)  libssh2_channel_flush_ex,$(DL) >> $@
	@echo $(DL)  libssh2_channel_free,$(DL) >> $@
	@echo $(DL)  libssh2_channel_get_exit_status,$(DL) >> $@
	@echo $(DL)  libssh2_channel_open_ex,$(DL) >> $@
	@echo $(DL)  libssh2_channel_process_startup,$(DL) >> $@
	@echo $(DL)  libssh2_channel_read_ex,$(DL) >> $@
	@echo $(DL)  libssh2_channel_request_pty_ex,$(DL) >> $@
	@echo $(DL)  libssh2_channel_send_eof,$(DL) >> $@
	@echo $(DL)  libssh2_channel_set_blocking,$(DL) >> $@
	@echo $(DL)  libssh2_channel_setenv_ex,$(DL) >> $@
	@echo $(DL)  libssh2_channel_wait_closed,$(DL) >> $@
	@echo $(DL)  libssh2_channel_wait_eof,$(DL) >> $@
	@echo $(DL)  libssh2_channel_write_ex,$(DL) >> $@
	@echo $(DL)  libssh2_hostkey_hash,$(DL) >> $@
	@echo $(DL)  libssh2_scp_recv,$(DL) >> $@
	@echo $(DL)  libssh2_scp_send64,$(DL) >> $@
	@echo $(DL)  libssh2_scp_send_ex,$(DL) >> $@
	@echo $(DL)  libssh2_knownhost_add,$(DL) >> $@
	@echo $(DL)  libssh2_knownhost_check,$(DL) >> $@
	@echo $(DL)  libssh2_knownhost_free,$(DL) >> $@
	@echo $(DL)  libssh2_knownhost_init,$(DL) >> $@
	@echo $(DL)  libssh2_knownhost_readfile,$(DL) >> $@
	@echo $(DL)  libssh2_knownhost_writefile,$(DL) >> $@
	@echo $(DL)  libssh2_session_abstract,$(DL) >> $@
	@echo $(DL)  libssh2_session_block_directions,$(DL) >> $@
	@echo $(DL)  libssh2_session_callback_set,$(DL) >> $@
	@echo $(DL)  libssh2_session_disconnect_ex,$(DL) >> $@
	@echo $(DL)  libssh2_session_free,$(DL) >> $@
	@echo $(DL)  libssh2_session_hostkey,$(DL) >> $@
	@echo $(DL)  libssh2_session_init_ex,$(DL) >> $@
	@echo $(DL)  libssh2_session_last_errno,$(DL) >> $@
	@echo $(DL)  libssh2_session_last_error,$(DL) >> $@
	@echo $(DL)  libssh2_session_method_pref,$(DL) >> $@
	@echo $(DL)  libssh2_session_methods,$(DL) >> $@
	@echo $(DL)  libssh2_session_set_blocking,$(DL) >> $@
	@echo $(DL)  libssh2_session_startup,$(DL) >> $@
	@echo $(DL)  libssh2_sftp_close_handle,$(DL) >> $@
	@echo $(DL)  libssh2_sftp_fstat_ex,$(DL) >> $@
	@echo $(DL)  libssh2_sftp_init,$(DL) >> $@
	@echo $(DL)  libssh2_sftp_last_error,$(DL) >> $@
	@echo $(DL)  libssh2_sftp_mkdir_ex,$(DL) >> $@
	@echo $(DL)  libssh2_sftp_open_ex,$(DL) >> $@
	@echo $(DL)  libssh2_sftp_read,$(DL) >> $@
	@echo $(DL)  libssh2_sftp_readdir_ex,$(DL) >> $@
	@echo $(DL)  libssh2_sftp_rename_ex,$(DL) >> $@
	@echo $(DL)  libssh2_sftp_rmdir_ex,$(DL) >> $@
	@echo $(DL)  libssh2_sftp_seek,$(DL) >> $@
	@echo $(DL)  libssh2_sftp_seek64,$(DL) >> $@
	@echo $(DL)  libssh2_sftp_shutdown,$(DL) >> $@
	@echo $(DL)  libssh2_sftp_stat_ex,$(DL) >> $@
	@echo $(DL)  libssh2_sftp_symlink_ex,$(DL) >> $@
	@echo $(DL)  libssh2_sftp_tell,$(DL) >> $@
	@echo $(DL)  libssh2_sftp_unlink_ex,$(DL) >> $@
	@echo $(DL)  libssh2_sftp_write,$(DL) >> $@
	@echo $(DL)  libssh2_trace,$(DL) >> $@
	@echo $(DL)  libssh2_userauth_authenticated,$(DL) >> $@
	@echo $(DL)  libssh2_userauth_keyboard_interactive_ex,$(DL) >> $@
	@echo $(DL)  libssh2_userauth_list,$(DL) >> $@
	@echo $(DL)  libssh2_userauth_password_ex,$(DL) >> $@
	@echo $(DL)  libssh2_userauth_publickey_fromfile_ex$(DL) >> $@

$(DISTDIR)/readme.txt: GNUmakefile
	@echo Creating $@
	@echo $(DL)This is a binary distribution for NetWare platform.$(DL) > $@
	@echo $(DL)libssh2 version $(LIBSSH2_VERSION_STR)$(DL) >> $@
	@echo $(DL)Please download the complete libssh package for$(DL) >> $@
	@echo $(DL)any further documentation:$(DL) >> $@
	@echo $(DL)$(WWWURL)$(DL) >> $@

$(DEVLDIR)/readme.txt: GNUmakefile
	@echo Creating $@
	@echo $(DL)This is a development distribution for NetWare platform.$(DL) > $@
	@echo $(DL)libssh2 version $(LIBSSH2_VERSION_STR)$(DL) >> $@
	@echo $(DL)Please download the complete libssh2 package for$(DL) >> $@
	@echo $(DL)any further documentation:$(DL) >> $@
	@echo $(DL)$(WWWURL)$(DL) >> $@

help: $(OBJDIR)/version.inc
	@echo $(DL)===========================================================$(DL)
ifeq ($(LIBARCH),LIBC)
	@echo $(DL)Novell LibC NDK = $(SDK_LIBC)$(DL)
else
	@echo $(DL)Novell CLib NDK = $(SDK_CLIB)$(DL)
endif
	@echo $(DL)OpenSSL path    = $(OPENSSL_PATH)$(DL)
	@echo $(DL)Zlib path       = $(ZLIB_PATH)$(DL)
	@echo $(DL)===========================================================$(DL)
	@echo $(DL)libssh $(LIBSSH2_VERSION_STR) - available targets are:$(DL)
	@echo $(DL)$(MAKE) all$(DL)
	@echo $(DL)$(MAKE) nlm$(DL)
	@echo $(DL)$(MAKE) lib$(DL)
	@echo $(DL)$(MAKE) clean$(DL)
	@echo $(DL)$(MAKE) dev$(DL)
	@echo $(DL)$(MAKE) devclean$(DL)
	@echo $(DL)$(MAKE) dist$(DL)
	@echo $(DL)$(MAKE) distclean$(DL)
	@echo $(DL)$(MAKE) objclean$(DL)
	@echo $(DL)$(MAKE) test$(DL)
	@echo $(DL)===========================================================$(DL)


