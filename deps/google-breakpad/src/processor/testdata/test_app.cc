// Copyright (c) 2006, Google Inc.
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

// This file is used to generate minidump2.dmp and minidump2.sym.
// cl /Zi test_app.cc /Fetest_app.exe /I google_breakpad/src \
//   google_breakpad/src/client/windows/releasestaticcrt/exception_handler.lib
// Then run test_app to generate a dump, and dump_syms to create the .sym file.

#include <stdio.h>

#include "client/windows/handler/exception_handler.h"

namespace {

static bool callback(const wchar_t *dump_path, const wchar_t *id,
                     void *context, EXCEPTION_POINTERS *exinfo,
                     MDRawAssertionInfo *assertion,
                     bool succeeded) {
  if (succeeded) {
    printf("dump guid is %ws\n", id);
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
  google_breakpad::ExceptionHandler eh(
      L".", NULL, callback, NULL,
      google_breakpad::ExceptionHandler::HANDLER_ALL);
  CrashFunction();
  printf("did not crash?\n");
  return 0;
}
