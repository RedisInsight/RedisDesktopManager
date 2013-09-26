#!/bin/sh

LIBTOOLIZE="libtoolize"

if [ "x`which $LIBTOOLIZE`" = "x" ]; then
    LIBTOOLIZE="glibtoolize"
fi

if [ "x`which $LIBTOOLIZE`" = "x" ]; then
    echo "Neither libtoolize nor glibtoolize could be found!"
    exit 1
fi

${LIBTOOLIZE} --copy --automake --force
${ACLOCAL:-aclocal} -I m4 $ACLOCAL_FLAGS
${AUTOHEADER:-autoheader}
# copy the private libssh2_config.h.in to the examples dir so that
# it can be included without pointing the include path to the private
# source dir
cp src/libssh2_config.h.in example/libssh2_config.h.in
${AUTOCONF:-autoconf}
${AUTOMAKE:-automake} --add-missing --copy
