#########################################################################
#
## Makefile for building libssh2 (NetWare version - gnu make)
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
ZLIB_PATH = ../../../zlib-1.2.7
endif

# Edit the path below to point to the base of your OpenSSL package.
ifndef OPENSSL_PATH
OPENSSL_PATH = ../../../openssl-0.9.8x
endif

# Edit the var below to enable static linking of libssh2 and libz
LINK_STATIC = 1

# Edit the vars below to change NLM target settings.
SAMPLES	= ../../example
TARGETS := $(filter-out x11.nlm,$(patsubst $(SAMPLES)/%.c,%.nlm,$(strip $(wildcard $(SAMPLES)/*.c))))
VERSION	= $(LIBSSH2_VERSION)
COPYR   = Copyright (c) $(LIBSSH2_COPYRIGHT_STR)
WWWURL	= http://www.libssh2.org/
DESCR	= libssh2 $(notdir $(@:.def=)) $(LIBSSH2_VERSION_STR) ($(LIBARCH)) - $(WWWURL)
MTSAFE	= YES
STACK	= 64000
SCREEN	= NONE
#SCREEN	= libssh2 $(notdir $(@:.def=))
# Comment the line below if you dont want to load protected automatically.
#LDRING	= 3

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

ifeq ($(CC),mwccnlm)
LD	= mwldnlm
LDFLAGS	= -nostdlib $(<:.def=.o) $(PRELUDE) $(LDLIBS) -o $@ -commandfile
AR	= mwldnlm
ARFLAGS	= -type library -w nocmdline $(OBJS) -o
LIBEXT	= lib
CFLAGS	+= -msgstyle gcc -gccinc -inline off -opt nointrinsics -proc 586
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
LD	= nlmconv
LDFLAGS	= -UT
AR	= ar
ARFLAGS	= -cq
LIBEXT	= a
CFLAGS	+= -m32
CFLAGS	+= -fno-builtin -fpcc-struct-return
CFLAGS	+= -fno-strict-aliasing
CFLAGS	+= -Wall # -pedantic
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

NDK_ROOT = $(NDKBASE)/ndk
SDK_CLIB = $(NDK_ROOT)/nwsdk
SDK_LIBC = $(NDK_ROOT)/libc
SNPRINTF = $(NDKBASE)/snprintf

INCLUDES = -I.. -I../../include

LDLIBS =
 
ifdef LINK_STATIC
	LDLIBS += ../libssh2.$(LIBEXT)
else
	IMPORTS += @../libssh2.imp
	MODULES += libssh2.nlm
endif

INCLUDES += -I$(OPENSSL_PATH)/outinc_nw_$(LIBARCH_L)
LDLIBS += $(OPENSSL_PATH)/out_nw_$(LIBARCH_L)/ssl.$(LIBEXT)
LDLIBS += $(OPENSSL_PATH)/out_nw_$(LIBARCH_L)/crypto.$(LIBEXT)
IMPORTS += GetProcessSwitchCount RunningProcess

ifdef WITH_ZLIB
	INCLUDES += -I$(ZLIB_PATH)
ifdef LINK_STATIC
	LDLIBS += $(ZLIB_PATH)/nw/$(LIBARCH)/libz.$(LIBEXT)
else
	MODULES += libz.nlm
	IMPORTS += @$(ZLIB_PATH)/nw/$(LIBARCH)/libz.imp
endif
endif

ifeq ($(LIBARCH),LIBC)
	INCLUDES += -I$(SDK_LIBC)/include
	# INCLUDES += -I$(SDK_LIBC)/include/nks
	# INCLUDES += -I$(SDK_LIBC)/include/winsock
	CFLAGS += -D_POSIX_SOURCE
else
	INCLUDES += -I$(SDK_CLIB)/include/nlm
	# INCLUDES += -I$(SDK_CLIB)/include/nlm/obsolete
	# INCLUDES += -I$(SDK_CLIB)/include
	# CFLAGS += -DNETDB_USE_INTERNET
endif
CFLAGS	+= $(INCLUDES)

ifeq ($(MTSAFE),YES)
	XDCOPT = -n
endif
ifeq ($(MTSAFE),NO)
	XDCOPT = -u
endif

vpath %.c $(SAMPLES)

.PRECIOUS: $(OBJDIR)/%.o $(OBJDIR)/%.def $(OBJDIR)/%.xdc


all: prebuild $(TARGETS) 

prebuild: $(OBJDIR) $(OBJDIR)/version.inc

$(OBJDIR)/%.o: %.c
#	@echo Compiling $<
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR)/version.inc: ../../get_ver.awk ../../include/libssh2.h $(OBJDIR)
	@echo Creating $@
	@$(AWK) -f $^ > $@

objclean:
	$(call RMDIR, $(OBJDIR))

clean: objclean
	$(foreach f, $(TARGETS), $(call DEL, $(f)))

$(OBJDIR):
	@$(call MKDIR, $@)

%.nlm: $(OBJDIR)/%.def $(OBJDIR)/%.o $(OBJDIR)/%.xdc
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
	@echo $(DL)threadname "$(notdir $(@:.def=))"$(DL) >> $@
ifdef XDCOPT
	@echo $(DL)xdcdata $(@:.def=.xdc)$(DL) >> $@
endif
ifeq ($(LDRING),0)
	@echo $(DL)flag_on 16$(DL) >> $@
endif
ifeq ($(LDRING),3)
	@echo $(DL)flag_on 512$(DL) >> $@
endif
ifeq ($(LIBARCH),CLIB)
	@echo $(DL)start _Prelude$(DL) >> $@
	@echo $(DL)exit _Stop$(DL) >> $@
	@echo $(DL)import @$(SDK_CLIB)/imports/clib.imp$(DL) >> $@
	@echo $(DL)import @$(SDK_CLIB)/imports/threads.imp$(DL) >> $@
	@echo $(DL)import @$(SDK_CLIB)/imports/nlmlib.imp$(DL) >> $@
	@echo $(DL)import @$(SDK_CLIB)/imports/socklib.imp$(DL) >> $@
	@echo $(DL)module clib$(DL) >> $@
else
	@echo $(DL)flag_on 64$(DL) >> $@
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
	@echo $(DL)input $(@:.def=.o)$(DL) >> $@
	@echo $(DL)input $(PRELUDE)$(DL) >> $@
ifdef LDLIBS
	@echo $(DL)input $(LDLIBS)$(DL) >> $@
endif
	@echo $(DL)output $(notdir $(@:.def=.nlm))$(DL) >> $@
endif

