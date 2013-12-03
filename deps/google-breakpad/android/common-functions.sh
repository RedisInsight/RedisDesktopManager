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

# Collection of common shell functions for 'run-checks.sh' et 'test-shell.sh'

# All internal variables and functions use an underscore as a prefix
# (e.g. _VERBOSE, _ALL_CLEANUPS, etc..).

# Sanitize the environment
export LANG=C
export LC_ALL=C

if [ "$BASH_VERSION" ]; then
  set -o posix
fi

# Utility functions

_ALL_CLEANUPS=

# Register a function to be called when the script exits, even in case of
# Ctrl-C, logout, etc.
# $1: function name.
atexit () {
  if [ -z "$_ALL_CLEANUPS" ]; then
    _ALL_CLEANUPS=$1
    # Ensure a clean exit when the script is:
    #  - Exiting normally (EXIT)
    #  - Interrupted by Ctrl-C (INT)
    #  - Interrupted by log out (HUP)
    #  - Being asked to quit nicely (TERM)
    #  - Being asked to quit and dump core (QUIT)
    trap "_exit_cleanups \$?" EXIT INT HUP QUIT TERM
  else
    _ALL_CLEANUPS="$_ALL_CLEANUPS $1"
  fi
}

# Called on exit if at least one function was registered with atexit
# $1: final exit status code
_exit_cleanups () {
  local CLEANUP CLEANUPS
  # Ignore calls to atexit during cleanups
  CLEANUPS=$_ALL_CLEANUPS
  _ALL_CLEANUPS=
  for CLEANUP in $CLEANUPS; do
    ($CLEANUP)
  done
  exit "$@"
}




# Dump a panic message then exit.
# $1+: message
panic () {
  echo "ERROR: $@" >&2
  exit 1
}

# If the previous command failed, dump a panic message then exit.
# $1+: message.
fail_panic () {
  if [ $? != 0 ]; then
    panic "$@"
  fi;
}

_VERBOSE=0

# Increase verbosity for dump/log/run/run2 functions
increase_verbosity () {
  _VERBOSE=$(( $_VERBOSE + 1 ))
}

# Decrease verbosity
decrease_verbosity () {
  _VERBOSE=$(( $_VERBOSE - 1 ))
}

# Returns success iff verbosity level is higher than a specific value
# $1: verbosity level
verbosity_is_higher_than () {
  [ "$_VERBOSE" -gt "$1" ]
}

# Returns success iff verbosity level is lower than a specific value
# $1: verbosity level
verbosity_is_lower_than () {
  [ "$_VERBOSE" -le "$1" ]
}

# Dump message to stdout, unless verbosity is < 0, i.e. --quiet was called
# $1+: message
dump () {
  if [ "$_VERBOSE" -ge 0 ]; then
    printf "%s\n" "$*"
  fi
}

# If --verbose was used, dump a message to stdout.
# $1+: message
log () {
  if [ "$_VERBOSE" -ge 1 ]; then
    printf "%s\n" "$*"
  fi
}

_RUN_LOG=

# Set a run log file that can be used to collect the output of commands that
# are not displayed.
set_run_log () {
  _RUN_LOG=$1
}

# Run a command. Output depends on $_VERBOSE:
#   $_VERBOSE <= 0:  Run command, store output into the run log
#   $_VERBOSE >= 1:  Dump command, run it, output goest to stdout
# Note: Ideally, the command's output would go to the run log for $_VERBOSE >= 1
#       but the 'tee' tool doesn't preserve the status code of its input pipe
#       in case of error.
run () {
  local LOGILE
  if [ "$_RUN_LOG" ]; then
    LOGFILE=$_RUN_LOG
  else
    LOGFILE=/dev/null
  fi

  if [ "$_VERBOSE" -ge 1 ]; then
    echo "COMMAND: $@"
    "$@"
  else
    "$@" >>$LOGFILE 2>&1
  fi
}

# Same as run(), but only dump command output for $_VERBOSE >= 2
run2 () {
  local LOGILE
  if [ "$_RUN_LOG" ]; then
    LOGFILE=$_RUN_LOG
  else
    LOGFILE=/dev/null
  fi

  if [ "$_VERBOSE" -ge 1 ]; then
    echo "COMMAND: $@"
  fi
  if [ "$_VERBOSE" -ge 2 ]; then
    "$@"
  else
    "$@" >>$LOGFILE 2>&1
  fi
}

# Extract number of cores to speed up the builds
# Out: number of CPU cores
get_core_count () {
  case $(uname -s) in
    Linux)
      grep -c -e '^processor' /proc/cpuinfo
      ;;
    Darwin)
      sysctl -n hw.ncpu
      ;;
    CYGWIN*|*_NT-*)
      echo $NUMBER_OF_PROCESSORS
      ;;
    *)
      echo 1
      ;;
  esac
}


# Check for the Android ADB program.
#
# On success, return nothing, but updates internal variables so later calls to
# adb_shell, adb_push, etc.. will work. You can get the path to the ADB program
# with adb_get_program if needed.
#
# On failure, returns 1, and updates the internal adb error message, which can
# be retrieved with adb_get_error.
#
# $1: optional ADB program path.
# Return: success or failure.
_ADB=
_ADB_STATUS=
_ADB_ERROR=

adb_check () {
  # First, try to find the executable in the path, or the SDK install dir.
  _ADB=$1
  if [ -z "$_ADB" ]; then
    _ADB=$(which adb 2>/dev/null)
    if [ -z "$_ADB" -a "$ANDROID_SDK_ROOT" ]; then
      _ADB=$ANDROID_SDK_ROOT/platform-tools/adb
      if [ ! -f "$_ADB" ]; then
        _ADB=
      fi
    fi
    if [ -z "$_ADB" ]; then
      _ADB_STATUS=1
      _ADB_ERROR="The Android 'adb' tool is not in your path."
      return 1
    fi
  fi

  log "Found ADB program: $_ADB"

  # Check that it works correctly
  local ADB_VERSION
  ADB_VERSION=$("$_ADB" version 2>/dev/null)
  case $ADB_VERSION in
    "Android Debug Bridge "*) # Pass
      log "Found ADB version: $ADB_VERSION"
      ;;
    *) # Fail
      _ADB_ERROR="Your ADB binary reports a bad version ($ADB_VERSION): $_ADB"
      _ADB_STATUS=1
      return 1
  esac

  _ADB_STATUS=0
  return 0
}


# Return the path to the Android ADB program, if correctly detected.
# On failure, return the empty string.
# Out: ADB program path (or empty on failure)
# Return: success or failure.
adb_get_program () {
  # Return cached value as soon as possible.
  if [ -z "$_ADB_STATUS" ]; then
    adb_check $1
  fi
  echo "$_ADB"
  return $_ADB_STATUS
}

# Return the error corresponding to the last ADB function failure.
adb_get_error () {
  echo "$_ADB_ERROR"
}

# Check that there is one device connected through ADB.
# In case of failure, use adb_get_error to know why this failed.
# $1: Optional adb program path
# Return: success or failure.
_ADB_DEVICE=
_ADB_DEVICE_STATUS=
adb_check_device () {
  if [ "$_ADB_DEVICE_STATUS" ]; then
    return $_ADB_DEVICE_STATUS
  fi

  # Check for ADB.
  if ! adb_check $1; then
    _ADB_DEVICE_STATUS=$_ADB_STATUS
    return 1
  fi

  local ADB_DEVICES NUM_DEVICES FINGERPRINT

  # Count the number of connected devices.
  ADB_DEVICES=$("$_ADB" devices 2>/dev/null | awk '$2 == "device" { print $1; }')
  NUM_DEVICES=$(echo "$ADB_DEVICES" | wc -l)
  case $NUM_DEVICES in
    0)
      _ADB_ERROR="No Android device connected. Please connect one to your machine."
      _ADB_DEVICE_STATUS=1
      return 1
      ;;
    1) # Pass
      # Ensure the same device will be called in later adb_shell calls.
      export ANDROID_SERIAL=$ADB_DEVICES
      ;;
    *) # 2 or more devices.
      if [ "$ANDROID_SERIAL" ]; then
        ADB_DEVICES=$ANDROID_SERIAL
        NUM_DEVICES=1
      else
        _ADB_ERROR="More than one Android device connected. \
Please define ANDROID_SERIAL in your environment"
        _ADB_DEVICE_STATUS=1
        return 1
      fi
      ;;
  esac

  _ADB_DEVICE_STATUS=0
  _ADB_DEVICE=$ADB_DEVICES

  FINGERPRINT=$(adb_shell getprop ro.build.fingerprint)
  log "Using ADB device: $ANDROID_SERIAL ($FINGERPRINT)"
  return 0
}

# The 'adb shell' command is pretty hopeless, try to make sense of it by:
#   1/ Removing trailing \r from line endings.
#   2/ Ensuring the function returns the command's status code.
#
# $1+: Command
# Out: command output (stdout + stderr combined)
# Return: command exit status
adb_shell () {
  local RET ADB_LOG
  # Check for ADB device.
  adb_check_device || return 1
  ADB_LOG=$(mktemp "${TMPDIR:-/tmp}/adb-XXXXXXXX")
  "$_ADB" shell "$@" ";" echo \$? > "$ADB_LOG" 2>&1
  sed -i -e 's![[:cntrl:]]!!g' "$ADB_LOG"  # Remove \r.
  RET=$(sed -e '$!d' "$ADB_LOG")           # Last line contains status code.
  sed -e '$d' "$ADB_LOG"                   # Print everything except last line.
  rm -f "$ADB_LOG"
  return $RET
}

# Push a file to a device.
# $1: source file path
# $2: device target file path
# Return: success or failure.
adb_push () {
  adb_check_device || return 1
  run "$_ADB" push "$1" "$2"
}

# Pull a file from a device
# $1: device file path
# $2: target host file path
# Return: success or failure.
adb_pull () {
  adb_check_device || return 1
  run "$_ADB" pull "$1" "$2"
}

# Same as adb_push, but will panic if the operations didn't succeed.
adb_install () {
  adb_push "$@"
  fail_panic "Failed to install $1 to the Android device at $2"
}

