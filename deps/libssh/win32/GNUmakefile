#########################################################################
#
## Makefile for building libssh2 (Win32 version - gnu make)
## Use: make -f GNUmakefile [help|all|clean|dev|devclean|dist|distclean|dll|objclean]
##
## Hacked by: Guenter Knauf
#
#########################################################################

# Edit the path below to point to the base of your Zlib sources.
ifndef ZLIB_PATH
ZLIB_PATH = ../../zlib-1.2.7
endif

# Edit the path below to point to the base of your OpenSSL package.
ifndef OPENSSL_PATH
OPENSSL_PATH = ../../openssl-0.9.8x
endif

# Edit the var below to set to your architecture or set environment var.
ifndef ARCH
ARCH = w32
endif

# Edit the path below to point to your Distribution folder.
ifndef DISTDIR
DISTDIR	= libssh2-$(LIBSSH2_VERSION_STR)-bin-$(ARCH)
endif
DISTARC	= $(DISTDIR).zip

# Edit the path below to point to your Development folder.
ifndef DEVLDIR
DEVLDIR	= libssh2-$(LIBSSH2_VERSION_STR)-dev-$(ARCH)
endif
DEVLARC	= $(DEVLDIR).zip

# Edit the vars below to change target settings.
TARGET	= libssh2
WWWURL	= http://www.libssh2.org/
DESCR	= libssh2 $(LIBSSH2_VERSION_STR)
#STACK	= 64000

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

# Here you can find a native Win32 binary of the original awk:
# http://www.gknw.net/development/prgtools/awk-20100523.zip
AWK	= awk
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

# The following line defines your compiler.
ifdef METROWERKS
	CC = mwcc
else
	CC = $(CROSSPREFIX)gcc
endif

# Include the version info retrieved from libssh2.h
-include $(OBJDIR)/version.inc

# Global flags for all compilers
CFLAGS	= $(OPT) -D$(DB) -DLIBSSH2_WIN32 # -DHAVE_CONFIG_H
ifeq ($(ARCH),w64)
CFLAGS	+= -D_AMD64_
endif

ifeq ($(CC),mwcc)
LD	= mwld
RC	= mwwinrc
LDFLAGS	= -nostdlib
AR	= $(LD)
ARFLAGS	= -nostdlib -library -o
LIBEXT	= lib
#RANLIB	=
LIBPATH	+= -lr "$(METROWERKS)/MSL" -lr "$(METROWERKS)/Win32-x86 Support"
LDLIBS	+= -lMSL_Runtime_x86.lib -lMSL_C_x86.lib -lMSL_Extras_x86.lib
LDLIBS	+= -lkernel32.lib -luser32.lib -lwsock32.lib
RCFLAGS	=
CFLAGS	+= -nostdinc -gccinc -msgstyle gcc -inline off -opt nointrinsics -proc 586
CFLAGS	+= -ir "$(METROWERKS)/MSL" -ir "$(METROWERKS)/Win32-x86 Support"
CFLAGS	+= -w on,nounused,nounusedexpr # -ansi strict
else
LD	= $(CROSSPREFIX)gcc
RC	= $(CROSSPREFIX)windres
LDFLAGS	= -s -shared -Wl,--output-def,$(TARGET).def,--out-implib,$(TARGET)dll.a
AR	= $(CROSSPREFIX)ar
ARFLAGS	= -cq
LIBEXT	= a
RANLIB	= $(CROSSPREFIX)ranlib
#LDLIBS	+= -lwsock32
LDLIBS	+= -lws2_32
RCFLAGS	= -I. -I ../include -O coff -i
CFLAGS	+= -fno-strict-aliasing
CFLAGS	+= -Wall # -pedantic
endif

INCLUDES = -I. -I../include

ifndef OPENSSL_INCLUDE
	ifeq "$(wildcard $(OPENSSL_PATH)/outinc)" "$(OPENSSL_PATH)/outinc"
		OPENSSL_INCLUDE = $(OPENSSL_PATH)/outinc
	endif
	ifeq "$(wildcard $(OPENSSL_PATH)/include)" "$(OPENSSL_PATH)/include"
		OPENSSL_INCLUDE = $(OPENSSL_PATH)/include
	endif
endif
ifneq "$(wildcard $(OPENSSL_INCLUDE)/openssl/opensslv.h)" "$(OPENSSL_INCLUDE)/openssl/opensslv.h"
$(error Invalid OPENSSL_PATH: $(OPENSSL_PATH))
endif
INCLUDES += -I"$(OPENSSL_INCLUDE)"
ifndef OPENSSL_LIBPATH
	OPENSSL_LIBS_STAT = crypto ssl
	ifeq "$(wildcard $(OPENSSL_PATH)/out)" "$(OPENSSL_PATH)/out"
		OPENSSL_LIBPATH = $(OPENSSL_PATH)/out
		OPENSSL_LIBS_DYN = eay32 ssl32
	endif
	ifeq "$(wildcard $(OPENSSL_PATH)/lib)" "$(OPENSSL_PATH)/lib"
		OPENSSL_LIBPATH = $(OPENSSL_PATH)/lib
		OPENSSL_LIBS_DYN = crypto.dll ssl.dll
	endif
endif
ifdef LINK_OPENSSL_STATIC
	LDLIBS += $(patsubst %,$(OPENSSL_LIBPATH)/lib%.$(LIBEXT), $(OPENSSL_LIBS_STAT)) -lgdi32
else
	LDLIBS += $(patsubst %,$(OPENSSL_LIBPATH)/lib%.$(LIBEXT), $(OPENSSL_LIBS_DYN))
endif

ifdef WITH_ZLIB
	CFLAGS += -DLIBSSH2_HAVE_ZLIB
	INCLUDES += -I$(ZLIB_PATH)
	ifdef LINK_ZLIB_STATIC
		LDLIBS += $(ZLIB_PATH)/libz.$(LIBEXT)
	else
		LDLIBS += $(ZLIB_PATH)/libz.dll.$(LIBEXT)
	endif
endif

CFLAGS	+= $(INCLUDES)

vpath %.c . ../src

# include Makefile.inc to get CSOURCES define
include ../Makefile.inc

OBJECTS	:= $(patsubst %.c,%.o,$(CSOURCES))
OBJS	:= $(addprefix $(OBJDIR)/,$(OBJECTS))
OBJL	= $(OBJS) $(OBJDIR)/$(TARGET).res

all: lib dll

dll: prebuild $(TARGET).dll

lib: prebuild $(TARGET).$(LIBEXT)

prebuild: $(OBJDIR) $(OBJDIR)/version.inc
# libssh2_config.h

test: all
	$(MAKE) -C test -f GNUmakefile
 
$(OBJDIR)/%.o: %.c
#	@echo Compiling $<
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR)/version.inc: ../get_ver.awk ../include/libssh2.h $(OBJDIR)
	@echo Creating $@
	@$(AWK) -f $^ > $@

dist: all $(DISTDIR) $(DISTDIR)/readme.txt
	@$(call MKDIR, $(DISTDIR)/bin)
	@$(call CP, ../AUTHORS, $(DISTDIR))
	@$(call CP, ../COPYING, $(DISTDIR))
	@$(call CP, ../INSTALL, $(DISTDIR))
	@$(call CP, ../README, $(DISTDIR))
	@$(call CP, ../RELEASE-NOTES, $(DISTDIR))
	@$(call CP, $(TARGET).dll, $(DISTDIR)/bin)
	@echo Creating $(DISTARC)
	@$(ZIP) $(DISTARC) $(DISTDIR)/* < $(DISTDIR)/readme.txt

dev: all $(DEVLDIR) $(DEVLDIR)/readme.txt
	@$(call MKDIR, $(DEVLDIR)/bin)
	@$(call MKDIR,$(DEVLDIR)/include)
	@$(call MKDIR, $(DEVLDIR)/win32)
	@$(call CP, ../AUTHORS, $(DEVLDIR))
	@$(call CP, ../COPYING, $(DEVLDIR))
	@$(call CP, ../INSTALL, $(DEVLDIR))
	@$(call CP, ../README, $(DEVLDIR))
	@$(call CP, ../RELEASE-NOTES, $(DEVLDIR))
	@$(call CP, $(TARGET).dll, $(DEVLDIR)/bin)
	@$(call CP, ../include/*.h, $(DEVLDIR)/include)
	@$(call CP, libssh2_config.h, $(DEVLDIR)/include)
	@$(call CP, *.$(LIBEXT), $(DEVLDIR)/win32)
	@echo Creating $(DEVLARC)
	@$(ZIP) $(DEVLARC) $(DEVLDIR)/* < $(DEVLDIR)/readme.txt

distclean: clean
	$(call RMDIR, $(DISTDIR))
	$(call DEL, $(DISTARC))

devclean: clean
	$(call RMDIR, $(DEVLDIR))
	$(call DEL, $(DEVLARC))

objclean: all
	$(call RMDIR, $(OBJDIR))

testclean: clean
	$(MAKE) -C test -f GNUmakefile clean

clean:
#	$(call DEL, libssh2_config.h)
	$(call DEL, $(TARGET).dll $(TARGET).def $(TARGET).$(LIBEXT) $(TARGET)dll.$(LIBEXT))
	$(call RMDIR, $(OBJDIR))

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

$(TARGET).dll $(TARGET)dll.a: $(OBJL)
	@echo Linking $@
	@$(call DEL, $@)
	@$(LD) $(LDFLAGS) $^ -o $@ $(LIBPATH) $(LDLIBS)


$(OBJDIR)/%.res: %.rc
	@echo Creating $@
	@$(RC) $(RCFLAGS) $< -o $@


$(DISTDIR)/readme.txt: GNUmakefile
	@echo Creating $@
	@echo $(DL)This is a binary distribution for Win32 platform.$(DL) > $@
	@echo $(DL)libssh version $(LIBSSH2_VERSION_STR)$(DL) >> $@
	@echo $(DL)Please download the complete libssh package for$(DL) >> $@
	@echo $(DL)any further documentation:$(DL) >> $@
	@echo $(DL)$(WWWURL)$(DL) >> $@

$(DEVLDIR)/readme.txt: GNUmakefile
	@echo Creating $@
	@echo $(DL)This is a development distribution for Win32 platform.$(DL) > $@
	@echo $(DL)libssh version $(LIBSSH2_VERSION_STR)$(DL) >> $@
	@echo $(DL)Please download the complete libssh package for$(DL) >> $@
	@echo $(DL)any further documentation:$(DL) >> $@
	@echo $(DL)$(WWWURL)$(DL) >> $@

help: $(OBJDIR)/version.inc
	@echo $(DL)===========================================================$(DL)
	@echo $(DL)OpenSSL path    = $(OPENSSL_PATH)$(DL)
	@echo $(DL)Zlib path       = $(ZLIB_PATH)$(DL)
	@echo $(DL)===========================================================$(DL)
	@echo $(DL)libssh $(LIBSSH2_VERSION_STR) - available targets are:$(DL)
	@echo $(DL)$(MAKE) all$(DL)
	@echo $(DL)$(MAKE) dll$(DL)
	@echo $(DL)$(MAKE) lib$(DL)
	@echo $(DL)$(MAKE) clean$(DL)
	@echo $(DL)$(MAKE) dev$(DL)
	@echo $(DL)$(MAKE) devclean$(DL)
	@echo $(DL)$(MAKE) dist$(DL)
	@echo $(DL)$(MAKE) distclean$(DL)
	@echo $(DL)$(MAKE) objclean$(DL)
	@echo $(DL)$(MAKE) test$(DL)
	@echo $(DL)===========================================================$(DL)


