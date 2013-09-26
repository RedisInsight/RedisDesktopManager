AUTOMAKE_OPTIONS = foreign nostdinc

SUBDIRS = src tests docs
if BUILD_EXAMPLES
SUBDIRS += example
endif

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = libssh2.pc

include_HEADERS =			\
	include/libssh2.h		\
	include/libssh2_publickey.h	\
	include/libssh2_sftp.h

NETWAREFILES =  nw/keepscreen.c \
	nw/nwlib.c \
	nw/GNUmakefile \
	nw/test/GNUmakefile

DSP = win32/libssh2.dsp
VCPROJ = win32/libssh2.vcproj

DISTCLEANFILES = $(DSP)

VMSFILES = vms/libssh2_make_example.dcl vms/libssh2_make_help.dcl	\
vms/libssh2_make_kit.dcl vms/libssh2_make_lib.dcl vms/man2help.c	\
vms/readme.vms vms/libssh2_config.h

WIN32FILES = win32/GNUmakefile win32/test/GNUmakefile		\
win32/libssh2_config.h win32/config.mk win32/rules.mk		\
win32/Makefile.Watcom win32/libssh2.dsw win32/tests.dsp $(DSP)	\
win32/msvcproj.head win32/msvcproj.foot win32/libssh2.rc

EXTRA_DIST = $(WIN32FILES) buildconf $(NETWAREFILES) get_ver.awk HACKING \
 maketgz NMakefile TODO RELEASE-NOTES libssh2.pc.in $(VMSFILES) config.rpath

ACLOCAL_AMFLAGS = -I m4

.PHONY: ChangeLog
ChangeLog:
	echo "see NEWS" > ./ChangeLog
DISTCLEANFILES += ChangeLog

dist-hook:
	rm -rf $(top_builddir)/tests/log
	find $(distdir) -name "*.dist" -exec rm {} \;
	(distit=`find $(srcdir) -name "*.dist"`; \
	for file in $$distit; do \
	  strip=`echo $$file | sed -e s/^$(srcdir)// -e s/\.dist//`; \
	  cp $$file $(distdir)$$strip; \
	done)

# Code Coverage

init-coverage:
	make clean
	lcov --directory . --zerocounters

COVERAGE_CCOPTS ?= "-g --coverage"
COVERAGE_OUT ?= docs/coverage

build-coverage:
	make CFLAGS=$(COVERAGE_CCOPTS) check
	mkdir -p $(COVERAGE_OUT)
	lcov --directory . --output-file $(COVERAGE_OUT)/$(PACKAGE).info \
		--capture

gen-coverage:
	genhtml --output-directory $(COVERAGE_OUT) \
		$(COVERAGE_OUT)/$(PACKAGE).info \
		--highlight --frames --legend \
		--title "$(PACKAGE_NAME)"

coverage: init-coverage build-coverage gen-coverage

# DSP/VCPROJ generation adapted from libcurl
# Makefile.inc provides the CSOURCES and HHEADERS defines
include Makefile.inc

WIN32SOURCES = $(CSOURCES)
WIN32HEADERS = $(HHEADERS) libssh2_config.h

$(DSP): win32/msvcproj.head win32/msvcproj.foot Makefile.am
	echo "creating $(DSP)"
	@( (cat $(srcdir)/win32/msvcproj.head; \
	echo "# Begin Group \"Source Files\""; \
	echo ""; \
	echo "# PROP Default_Filter \"cpp;c;cxx\""; \
	win32_srcs='$(WIN32SOURCES)'; \
	sorted_srcs=`for file in $$win32_srcs; do echo $$file; done | sort`; \
	for file in $$sorted_srcs; do \
		echo "# Begin Source File"; \
		echo ""; \
		echo "SOURCE=..\\src\\"$$file; \
		echo "# End Source File"; \
	done; \
	echo "# End Group"; \
	echo "# Begin Group \"Header Files\""; \
	echo ""; \
	echo "# PROP Default_Filter \"h;hpp;hxx\""; \
	win32_hdrs='$(WIN32HEADERS)'; \
	sorted_hdrs=`for file in $$win32_hdrs; do echo $$file; done | sort`; \
	for file in $$sorted_hdrs; do \
		echo "# Begin Source File"; \
		echo ""; \
		if [ "$$file" == "libssh2_config.h" ]; \
		then \
			echo "SOURCE=.\\"$$file; \
		else \
			echo "SOURCE=..\\src\\"$$file; \
		fi; \
		echo "# End Source File"; \
	done; \
	echo "# End Group"; \
	cat $(srcdir)/win32/msvcproj.foot) | \
	awk '{printf("%s\r\n", gensub("\r", "", "g"))}' > $@ )

$(VCPROJ): win32/vc8proj.head win32/vc8proj.foot Makefile.am
	echo "creating $(VCPROJ)"
	@( (cat $(srcdir)/vc8proj.head; \
	win32_srcs='$(WIN32SOURCES)'; \
	sorted_srcs=`for file in $$win32_srcs; do echo $$file; done | sort`; \
	for file in $$sorted_srcs; do \
		echo "<File RelativePath=\""..\src\$$file"\"></File>"; \
	done; \
	echo "</Filter><Filter	Name=\"Header Files\">"; \
	win32_hdrs='$(WIN32HEADERS)'; \
	sorted_hdrs=`for file in $$win32_hdrs; do echo $$file; done | sort`; \
	for file in $$sorted_hdrs; do \
		echo "<File RelativePath=\""..\src\$$file"\"></File>"; \
	done; \
	cat $(srcdir)/vc8proj.foot) | \
	awk '{printf("%s\r\n", gensub("\r", "", "g"))}' > $@ )
