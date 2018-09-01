// Copyright (c) 2009, Google Inc.
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
//     * Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above
// copyright notice, this list of conditions and the following disclaimer
// in the documentation and/or other materials provided with the
// distribution.
//     * Neither the name of Google Inc. nor the names of its
// contributors may be used to endorse or promote products derived from
// this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

// Breakpad test application for Linux. When run, it generates one on-demand
// minidump and then crashes, which should generate an on-crash minidump.
// dump_syms can be used to extract symbol information for use in processing.

// To build:
// g++ -g -o linux_test_app -I ../../ -L../../client/linux linux_test_app.cc \
//   -lbreakpad
// Add -m32 to build a 32-bit executable, or -m64 for a 64-bit one
// (assuming your environment supports it). Replace -g with -gstabs+ to
// generate an executable with STABS symbols (needs -m32), or -gdwarf-2 for one
// with DWARF symbols (32- or 64-bit)

#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

#include <string>

#include "client/linux/handler/exception_handler.h"
#include "third_party/lss/linux_syscall_support.h"

namespace {

// google_breakpad::MinidumpCallback to invoke after minidump generation.
static bool callback(const char *dump_path, const char *id,
                     void *context,
                     bool succeeded) {
  if (succeeded) {
    printf("dump guid is %s\n", id);
  } else {
    printf("dump failed\n");
  }
  fflush(stdout);

  return succeeded;
}

static void CrashFunction() {
  int *i = reinterpret_cast<int*>(0x45);
  *i = 5;  // crash!
}

}  // namespace

int main(int argc, char **argv) {
  google_breakpad::ExceptionHandler eh(".", NULL, callback, NULL, true);
  if (!eh.WriteMinidump()) {
    printf("Failed to generate on-demand minidump\n");
  }
  CrashFunction();
  printf("did not crash?\n");
  return 0;
}
