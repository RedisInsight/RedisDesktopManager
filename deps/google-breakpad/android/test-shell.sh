#!/bin/sh
#
# Copyright (c) 2012 Google Inc.
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are
# met:
#
#     * Redistributions of source code must retain the above copyright
# notice, this list of conditions and the following disclaimer.
#     * Redistributions in binary form must reproduce the above
# copyright notice, this list of conditions and the following disclaimer
# in the documentation and/or other materials provided with the
# distribution.
#     * Neither the name of Google Inc. nor the names of its
# contributors may be used to endorse or promote products derived from
# this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
# "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
# LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
# A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
# OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
# SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
# LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
# DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
# THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
# OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

# A special shell wrapper that can be used to run the Google Breakpad unit
# tests on a connected Android device.
#
# This is designed to be called from the Makefile during 'make check'
#

PROGDIR=$(dirname "$0")
PROGNAME=$(basename "$0")
. $PROGDIR/common-functions.sh

# Extract test program name first.
TEST_PROGRAM=$1
shift

if [ -z "$TEST_PROGRAM" ]; then
  panic "No test program/script name on the command-line!"
fi

if [ ! -f "$TEST_PROGRAM" ]; then
  panic "Can't find test program/script: $TEST_PROGRAM"
fi

# Create test directory on the device
TEST_DIR=/data/local/tmp/test-google-breakpad
adb_shell mkdir "$TEST_DIR" || panic "Can't create test directory on device"

# Ensure that it is always removed when the script exits.
clean_test_dir () {
  # Don't care about success/failure, use '$ADB shell' directly.
  adb_shell rm -r "$TEST_DIR"
}

atexit clean_test_dir

TEST_PROGRAM_NAME=$(basename "$TEST_PROGRAM")
TEST_PROGRAM_DIR=$(dirname "$TEST_PROGRAM")

# Handle special case(s) here.
DATA_FILES=
case $TEST_PROGRAM_NAME in
  linux_client_unittest)
    # linux_client_unittest will call another executable at runtime, ensure
    # it is installed too.
    adb_install "$TEST_PROGRAM_DIR/linux_dumper_unittest_helper" "$TEST_DIR"
    # linux_client_unittest loads a shared library at runtime, ensure it is
    # installed too.
    adb_install "$TEST_PROGRAM_DIR/linux_client_unittest_shlib" "$TEST_DIR"
    ;;
  basic_source_line_resolver_unittest)
    DATA_FILES="module1.out \
                module2.out \
                module3_bad.out \
                module4_bad.out"
    ;;
  exploitability_unittest)
    DATA_FILES="scii_read_av.dmp \
                ascii_read_av_block_write.dmp \
                ascii_read_av_clobber_write.dmp \
                ascii_read_av_conditional.dmp \
                ascii_read_av_non_null.dmp \
                ascii_read_av_then_jmp.dmp \
                ascii_read_av_xchg_write.dmp \
                ascii_write_av.dmp \
                ascii_write_av_arg_to_call.dmp \
                exec_av_on_stack.dmp \
                null_read_av.dmp \
                null_write_av.dmp \
                read_av.dmp \
                null_read_av.dmp \
                write_av_non_null.dmp"
    ;;
  fast_source_line_resolver_unittest)
    DATA_FILES="module0.out \
                module1.out \
                module2.out \
                module3_bad.out \
                module4_bad.out"
    ;;
  minidump_processor_unittest|minidump_unittest)
    DATA_FILES="src/processor/testdata/minidump2.dmp"
    ;;
esac

# Install the data files, their path is relative to the environment
# variable 'srcdir'
for FILE in $DATA_FILES; do
  FILEDIR=src/processor/testdata/$(dirname "$FILE")
  adb_shell mkdir -p "$TEST_DIR/$FILEDIR"
  adb_install "${srcdir:-.}/$FILE" "$TEST_DIR"/"$FILE"
done

# Copy test program to device
adb_install "$TEST_PROGRAM" "$TEST_DIR"

# Run it
adb_shell "cd $TEST_DIR && LD_LIBRARY_PATH=. ./$TEST_PROGRAM_NAME $@"

# Note: exiting here will call cleanup_exit which will remove the temporary
#       files from the device.
