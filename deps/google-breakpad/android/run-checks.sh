#!/bin/sh
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

# Sanitize the environment
export LANG=C
export LC_ALL=C

if [ "$BASH_VERSION" ]; then
  set -o posix
fi

PROGDIR=$(dirname "$0")
PROGDIR=$(cd "$PROGDIR" && pwd)
PROGNAME=$(basename "$0")

. $PROGDIR/common-functions.sh

DEFAULT_ABI="armeabi"
VALID_ABIS="armeabi armeabi-v7a x86 mips"

ABI=
ADB=
ALL_TESTS=
ENABLE_M32=
HELP=
HELP_ALL=
NDK_DIR=
NO_CLEANUP=
NO_DEVICE=
NUM_JOBS=$(get_core_count)
TMPDIR=

for opt do
  # The following extracts the value if the option is like --name=<value>.
  optarg=$(expr -- $opt : '^--[^=]*=\(.*\)$')
  case $opt in
    --abi=*) ABI=$optarg;;
    --adb=*) ADB=$optarg;;
    --all-tests) ALL_TESTS=true;;
    --enable-m32) ENABLE_M32=true;;
    --help|-h|-?) HELP=TRUE;;
    --help-all) HELP_ALL=true;;
    --jobs=*) NUM_JOBS=$optarg;;
    --ndk-dir=*) NDK_DIR=$optarg;;
    --tmp-dir=*) TMPDIR=$optarg;;
    --no-cleanup) NO_CLEANUP=true;;
    --no-device) NO_DEVICE=true;;
    --quiet) decrease_verbosity;;
    --verbose) increase_verbosity;;
    -*) panic "Invalid option '$opt', see --help for details.";;
    *) panic "This script doesn't take any parameters. See --help for details."
       ;;
  esac
done

if [ "$HELP" -o "$HELP_ALL" ]; then
  echo "\
  Usage: $PROGNAME [options]

  This script is used to check that your Google Breakpad source tree can
  be properly built for Android, and that the client library and host tools
  work properly together.
"
  if [ "$HELP_ALL" ]; then
    echo "\
  In more details, this script will:

  - Rebuild the host version of Google Breakpad in a temporary
    directory (with the Auto-tools based build system).

  - Rebuild the Android client library with the Google Breakpad build
    system (using autotools/configure). This requires that you define
    ANDROID_NDK_ROOT in your environment to point to a valid Android NDK
    installation directory, or use the --ndk-dir=<path> option.

  - Rebuild the Android client library and a test crashing program with the
    Android NDK build system (ndk-build).

  - Require an Android device connected to your machine, and the 'adb'
    tool in your path. They are used to:

      - Install and  run a test crashing program.
      - Extract the corresponding minidump from the device.
      - Dump the symbols from the test program on the host with 'dump_syms'
      - Generate a stack trace with 'minidump_stackwalk'
      - Check the stack trace content for valid source file locations.

    You can however skip this requirement and only test the builds by using
    the --no-device flag.

    By default, all generated files will be created in a temporary directory
    that is removed when the script completion. If you want to inspect the
    files, use the --no-cleanup option.

    Finally, use --verbose to increase the verbosity level, this will help
    you see which exact commands are being issues and their result. Use the
    flag twice for even more output. Use --quiet to decrease verbosity
    instead and run the script silently.

    If you have a device connected, the script will probe it to determine
    its primary CPU ABI, and build the test program for it. You can however
    use the --abi=<name> option to override this (this can be useful to check
    the secondary ABI, e.g. using --abi=armeabi to check that such a program
    works correctly on an ARMv7-A device).

    If you don't have a device connected, the test program will be built (but
    not run) with the default '$DEFAULT_ABI' ABI. Again, you can use
    --abi=<name> to override this. Valid ABI names are:

        $VALID_ABIS

    The script will only run the client library unit test on the device
    by default. You can use --all-tests to also build and run the unit
    tests for the Breakpad tools and processor, but be warned that this
    adds several minutes of testing time. --all-tests will also run the
    host unit tests suite.
"

  fi  # HELP_ALL

  echo "\
  Valid options:

      --help|-h|-?     Display this message.
      --help-all       Display extended help.
      --enable-m32     Build 32-bit version of host tools.
      --abi=<name>     Specify target CPU ABI [auto-detected].
      --jobs=<count>   Run <count> build tasks in parallel [$NUM_JOBS].
      --ndk-dir=<path> Specify NDK installation directory.
      --tmp-dir=<path> Specify temporary directory (will be wiped-out).
      --adb=<path>     Specify adb program path.
      --no-cleanup     Don't remove temporary directory after completion.
      --no-device      Do not try to detect devices, nor run crash test.
      --all-tests      Run all unit tests (i.e. tools and processor ones too).
      --verbose        Increase verbosity.
      --quiet          Decrease verbosity."

  exit 0
fi

TESTAPP_DIR=$PROGDIR/sample_app

# Select NDK install directory.
if [ -z "$NDK_DIR" ]; then
  if [ -z "$ANDROID_NDK_ROOT" ]; then
    panic "Please define ANDROID_NDK_ROOT in your environment, or use \
--ndk-dir=<path>."
  fi
  NDK_DIR="$ANDROID_NDK_ROOT"
  log "Found NDK directory: $NDK_DIR"
else
  log "Using NDK directory: $NDK_DIR"
fi
# Small sanity check.
NDK_BUILD="$NDK_DIR/ndk-build"
if [ ! -f "$NDK_BUILD" ]; then
  panic "Your NDK directory is not valid (missing ndk-build): $NDK_DIR"
fi

# Ensure the temporary directory is deleted on exit, except if the --no-cleanup
# option is used.

clean_tmpdir () {
  if [ "$TMPDIR" ]; then
    if [ -z "$NO_CLEANUP" ]; then
      log "Cleaning up: $TMPDIR"
      rm -rf "$TMPDIR"
    else
      dump "Temporary directory contents preserved: $TMPDIR"
    fi
  fi
  exit "$@"
}

atexit clean_tmpdir

# If --tmp-dir=<path> is not used, create a temporary directory.
# Otherwise, start by cleaning up the user-provided path.
if [ -z "$TMPDIR" ]; then
  TMPDIR=$(mktemp -d /tmp/$PROGNAME.XXXXXXXX)
  fail_panic "Can't create temporary directory!"
  log "Using temporary directory: $TMPDIR"
else
  if [ ! -d "$TMPDIR" ]; then
    mkdir -p "$TMPDIR"
    fail_panic "Can't create temporary directory: $TMPDIR"
  else
    log "Cleaning up temporary directory: $TMPDIR"
    rm -rf "$TMPDIR"/*
    fail_panic "Cannot cleanup temporary directory!"
  fi
fi

if [ -z "$NO_DEVICE" ]; then
  if ! adb_check_device $ADB; then
    echo "$(adb_get_error)"
    echo "Use --no-device to build the code without running any tests."
    exit 1
  fi
fi

BUILD_LOG="$TMPDIR/build.log"
RUN_LOG="$TMPDIR/run.log"
CRASH_LOG="$TMPDIR/crash.log"

set_run_log "$RUN_LOG"

TMPHOST="$TMPDIR/host-local"

cd "$TMPDIR"

# Build host version of the tools
dump "Building host binaries."
CONFIGURE_FLAGS=
if [ "$ENABLE_M32" ]; then
  CONFIGURE_FLAGS="$CONFIGURE_FLAGS --enable-m32"
fi
(
  run mkdir "$TMPDIR/build-host" &&
  run cd "$TMPDIR/build-host" &&
  run2 "$PROGDIR/../configure" --prefix="$TMPHOST" $CONFIGURE_FLAGS &&
  run2 make -j$NUM_JOBS install
)
fail_panic "Can't build host binaries!"

if [ "$ALL_TESTS" ]; then
  dump "Running host unit tests."
  (
    run cd "$TMPDIR/build-host" &&
    run2 make -j$NUM_JOBS check
  )
  fail_panic "Host unit tests failed!!"
fi

TMPBIN=$TMPHOST/bin

# Generate a stand-alone NDK toolchain

# Extract CPU ABI and architecture from device, if any.
if adb_check_device; then
  DEVICE_ABI=$(adb_shell getprop ro.product.cpu.abi)
  DEVICE_ABI2=$(adb_shell getprop ro.product.cpu.abi2)
  if [ -z "$DEVICE_ABI" ]; then
    panic "Can't extract ABI from connected device!"
  fi
  if [ "$DEVICE_ABI2" ]; then
    dump "Found device ABIs: $DEVICE_ABI $DEVICE_ABI2"
  else
    dump "Found device ABI: $DEVICE_ABI"
    DEVICE_ABI2=$DEVICE_ABI
  fi

  # If --abi=<name> is used, check that the device supports it.
  if [ "$ABI" -a "$DEVICE_ABI" != "$ABI" -a "$DEVICE_ABI2" != "$ABI" ]; then
    dump  "ERROR: Device ABI(s) do not match --abi command-line value ($ABI)!"
    panic "Please use --no-device to skip device tests."
  fi

  if [ -z "$ABI" ]; then
    ABI=$DEVICE_ABI
    dump "Using CPU ABI: $ABI (device)"
  else
    dump "Using CPU ABI: $ABI (command-line)"
  fi
else
  if [ -z "$ABI" ]; then
    # No device connected, choose default ABI
    ABI=$DEFAULT_ABI
    dump "Using CPU ABI: $ABI (default)"
  else
    dump "Using CPU ABI: $ABI (command-line)"
  fi
fi

# Check the ABI value
VALID=
for VALID_ABI in $VALID_ABIS; do
  if [ "$ABI" = "$VALID_ABI" ]; then
    VALID=true
    break
  fi
done

if [ -z "$VALID" ]; then
  panic "Unknown CPU ABI '$ABI'. Valid values are: $VALID_ABIS"
fi

# Extract architecture name from ABI
case $ABI in
  armeabi*) ARCH=arm;;
  *) ARCH=$ABI;;
esac

# Extract GNU configuration name
case $ARCH in
  arm)
    GNU_CONFIG=arm-linux-androideabi
    ;;
  x86)
    GNU_CONFIG=i686-linux-android
    ;;
  mips)
    GNU_CONFIG=mipsel-linux-android
    ;;
  *)
    GNU_CONFIG="$ARCH-linux-android"
    ;;
esac

# Generate standalone NDK toolchain installation
NDK_STANDALONE="$TMPDIR/ndk-$ARCH-toolchain"
echo "Generating NDK standalone toolchain installation"
mkdir -p "$NDK_STANDALONE"
# NOTE: The --platform=android-9 is required to provide <regex.h> for GTest.
run "$NDK_DIR/build/tools/make-standalone-toolchain.sh" \
      --arch="$ARCH" \
      --platform=android-9 \
      --install-dir="$NDK_STANDALONE"
fail_panic "Can't generate standalone NDK toolchain installation!"

# Rebuild the client library, processor and tools with the auto-tools based
# build system. Even though it's not going to be used, this checks that this
# still works correctly.
echo "Building full Android binaries with configure/make"
TMPTARGET="$TMPDIR/target-local"
(
  PATH="$NDK_STANDALONE/bin:$PATH"
  run mkdir "$TMPTARGET" &&
  run mkdir "$TMPDIR"/build-target &&
  run cd "$TMPDIR"/build-target &&
  run2 "$PROGDIR"/../configure --prefix="$TMPTARGET" \
                               --host="$GNU_CONFIG" &&
  run2 make -j$NUM_JOBS install
)
fail_panic "Could not rebuild Android binaries!"

# Build and/or run unit test suite.
# If --no-device is used, only rebuild it, otherwise, run in on the
# connected device.
if [ "$NO_DEVICE" ]; then
  ACTION="Building"
  # This is a trick to force the Makefile to ignore running the scripts.
  TESTS_ENVIRONMENT="TESTS_ENVIRONMENT=true"
else
  ACTION="Running"
  TESTS_ENVIRONMENT=
fi

(
  PATH="$NDK_STANDALONE/bin:$PATH"
  run cd "$TMPDIR"/build-target &&
  # Reconfigure to only run the client unit test suite.
  # This one should _never_ fail.
  dump "$ACTION Android client library unit tests."
  run2 "$PROGDIR"/../configure --prefix="$TMPTARGET" \
                               --host="$GNU_CONFIG" \
                               --disable-tools \
                               --disable-processor &&
  run make -j$NUM_JOBS check $TESTS_ENVIRONMENT || exit $?

  if [ "$ALL_TESTS" ]; then
    dump "$ACTION Tools and processor unit tests."
    # Reconfigure to run the processor and tools tests.
    # Most of these fail for now, so do not worry about it.
    run2 "$PROGDIR"/../configure --prefix="$TMPTARGET" \
                                 --host="$GNU_CONFIG" &&
    run make -j$NUM_JOBS check $TESTS_ENVIRONMENT
    if [ $? != 0 ]; then
      dump "Tools and processor unit tests failed as expected. \
Use --verbose for results."
    fi                           
  fi
)
fail_panic "Client library unit test suite failed!"

# Copy sources to temporary directory
PROJECT_DIR=$TMPDIR/project
dump "Copying test program sources to: $PROJECT_DIR"
run cp -r "$TESTAPP_DIR" "$PROJECT_DIR" &&
run rm -rf "$PROJECT_DIR/obj" &&
run rm -rf "$PROJECT_DIR/libs"
fail_panic "Could not copy test program sources to: $PROJECT_DIR"

# Build the test program with ndk-build.
dump "Building test program with ndk-build"
export NDK_MODULE_PATH="$PROGDIR"
NDK_BUILD_FLAGS="-j$NUM_JOBS"
if verbosity_is_higher_than 1; then
  NDK_BUILD_FLAGS="$NDK_BUILD_FLAGS NDK_LOG=1 V=1"
fi
run "$NDK_DIR/ndk-build" -C "$PROJECT_DIR" $NDK_BUILD_FLAGS APP_ABI=$ABI
fail_panic "Can't build test program!"

# Unless --no-device was used, stop right here if ADB isn't in the path,
# or there is no connected device.
if [ "$NO_DEVICE" ]; then
  dump "Done. Please connect a device to run all tests!"
  clean_exit 0
fi

# Push the program to the device.
TESTAPP=test_google_breakpad
TESTAPP_FILE="$PROJECT_DIR/libs/$ABI/test_google_breakpad"
if [ ! -f "$TESTAPP_FILE" ]; then
  panic "Device requires '$ABI' binaries. None found!"
fi

# Run the program there
dump "Installing test program on device"
DEVICE_TMP=/data/local/tmp
adb_push "$TESTAPP_FILE" "$DEVICE_TMP/"
fail_panic "Cannot push test program to device!"

dump "Running test program on device"
adb_shell cd "$DEVICE_TMP" "&&" ./$TESTAPP > "$CRASH_LOG" 2>/dev/null
if [ $? = 0 ]; then
  panic "Test program did *not* crash as expected!"
fi
if verbosity_is_higher_than 0; then
  echo -n "Crash log: "
  cat "$CRASH_LOG"
fi

# Extract minidump from device
MINIDUMP_NAME=$(awk '$1 == "Dump" && $2 == "path:" { print $3; }' "$CRASH_LOG")
MINIDUMP_NAME=$(basename "$MINIDUMP_NAME")
if [ -z "$MINIDUMP_NAME" ]; then
  panic "Test program didn't write minidump properly!"
fi

dump "Extracting minidump: $MINIDUMP_NAME"
adb_pull "$DEVICE_TMP/$MINIDUMP_NAME" .
fail_panic "Can't extract minidump!"

dump "Parsing test program symbols"
if verbosity_is_higher_than 1; then
  log "COMMAND: $TMPBIN/dump_syms \
                $PROJECT_DIR/obj/local/$ABI/$TESTAPP >$TESTAPP.sym"
fi
"$TMPBIN/dump_syms" "$PROJECT_DIR/obj/local/$ABI/$TESTAPP" > $TESTAPP.sym
fail_panic "dump_syms doesn't work!"

VERSION=$(awk '$1 == "MODULE" { print $4; }' $TESTAPP.sym)
dump "Found module version: $VERSION"
if [ -z "$VERSION" ]; then
  echo "ERROR: Can't find proper module version from symbol dump!"
  head -n5 $TESTAPP.sym
  clean_exit 1
fi

run mkdir -p "$TMPDIR/symbols/$TESTAPP/$VERSION"
run mv $TESTAPP.sym "$TMPDIR/symbols/$TESTAPP/$VERSION/"

dump "Generating stack trace"
# Don't use 'run' to be able to send stdout and stderr to two different files.
log "COMMAND: $TMPBIN/minidump_stackwalk $MINIDUMP_NAME symbols"
"$TMPBIN/minidump_stackwalk" $MINIDUMP_NAME \
                             "$TMPDIR/symbols" \
                             > "$BUILD_LOG" 2>>"$RUN_LOG"
fail_panic "minidump_stackwalk doesn't work!"

dump "Checking stack trace content"

if verbosity_is_higher_than 1; then
  cat "$BUILD_LOG"
fi

# The generated stack trace should look like the following:
#
# Thread 0 (crashed)
#  0  test_google_breakpad!crash [test_breakpad.cpp : 17 + 0x4]
#      r4 = 0x00015530    r5 = 0xbea2cbe4    r6 = 0xffffff38    r7 = 0xbea2cb5c
#      r8 = 0x00000000    r9 = 0x00000000   r10 = 0x00000000    fp = 0x00000000
#      sp = 0xbea2cb50    lr = 0x00009025    pc = 0x00008f84
#     Found by: given as instruction pointer in context
#  1  test_google_breakpad!main [test_breakpad.cpp : 25 + 0x3]
#      r4 = 0x00015530    r5 = 0xbea2cbe4    r6 = 0xffffff38    r7 = 0xbea2cb5c
#      r8 = 0x00000000    r9 = 0x00000000   r10 = 0x00000000    fp = 0x00000000
#      sp = 0xbea2cb50    pc = 0x00009025
#     Found by: call frame info
#  2  libc.so + 0x164e5
#      r4 = 0x00008f64    r5 = 0xbea2cc34    r6 = 0x00000001    r7 = 0xbea2cc3c
#      r8 = 0x00000000    r9 = 0x00000000   r10 = 0x00000000    fp = 0x00000000
#      sp = 0xbea2cc18    pc = 0x400c34e7
#     Found by: call frame info
# ...
#
# The most important part for us is ensuring that the source location could
# be extracted, so look at the 'test_breakpad.cpp' references here.
#
# First, extract all the lines with test_google_breakpad! in them, and
# dump the corresponding crash location.
#
# Note that if the source location can't be extracted, the second field
# will only be 'test_google_breakpad' without the exclamation mark.
#
LOCATIONS=$(awk '$2 ~ "^test_google_breakpad!.*" { print $3; }' "$BUILD_LOG")

if [ -z "$LOCATIONS" ]; then
  if verbosity_is_lower_than 1; then
    cat "$BUILD_LOG"
  fi
  panic "No source location found in stack trace!"
fi

# Now check that they all match "[<source file>"
BAD_LOCATIONS=
for LOCATION in $LOCATIONS; do
  case $LOCATION in
    # Escape the opening bracket, or some shells like Dash will not
    # match them properly.
    \[*.cpp|\[*.cc|\[*.h) # These are valid source locations in our executable
      ;;
    *) # Everything else is not!
      BAD_LOCATIONS="$BAD_LOCATIONS $LOCATION"
      ;;
  esac
done

if [ "$BAD_LOCATIONS" ]; then
  dump "ERROR: Generated stack trace doesn't contain valid source locations:"
  cat "$BUILD_LOG"
  echo "Bad locations are: $BAD_LOCATIONS"
  exit 1
fi

echo "All clear! Congratulations."

