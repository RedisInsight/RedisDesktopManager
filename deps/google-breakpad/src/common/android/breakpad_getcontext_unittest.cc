// Copyright (c) 2012, Google Inc.
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

#include <sys/ucontext.h>

#include "breakpad_googletest_includes.h"
#include "common/android/ucontext_constants.h"

TEST(AndroidUContext, GRegsOffset) {
#ifdef __arm__
  // There is no gregs[] array on ARM, so compare to the offset of
  // first register fields, since they're stored in order.
  ASSERT_EQ(static_cast<size_t>(MCONTEXT_GREGS_OFFSET),
            offsetof(ucontext_t,uc_mcontext.arm_r0));
#elif defined(__i386__)
  ASSERT_EQ(static_cast<size_t>(MCONTEXT_GREGS_OFFSET),
            offsetof(ucontext_t,uc_mcontext.gregs));
#define CHECK_REG(x) \
  ASSERT_EQ(static_cast<size_t>(MCONTEXT_##x##_OFFSET),         \
            offsetof(ucontext_t,uc_mcontext.gregs[REG_##x]))
  CHECK_REG(GS);
  CHECK_REG(FS);
  CHECK_REG(ES);
  CHECK_REG(DS);
  CHECK_REG(EDI);
  CHECK_REG(ESI);
  CHECK_REG(EBP);
  CHECK_REG(ESP);
  CHECK_REG(EBX);
  CHECK_REG(EDX);
  CHECK_REG(ECX);
  CHECK_REG(EAX);
  CHECK_REG(TRAPNO);
  CHECK_REG(ERR);
  CHECK_REG(EIP);
  CHECK_REG(CS);
  CHECK_REG(EFL);
  CHECK_REG(UESP);
  CHECK_REG(SS);

  ASSERT_EQ(static_cast<size_t>(UCONTEXT_FPREGS_OFFSET),
            offsetof(ucontext_t,uc_mcontext.fpregs));

  ASSERT_EQ(static_cast<size_t>(UCONTEXT_FPREGS_MEM_OFFSET),
            offsetof(ucontext_t,__fpregs_mem));
#elif defined(__mips__)
  ASSERT_EQ(static_cast<size_t>(MCONTEXT_GREGS_OFFSET),
            offsetof(ucontext_t,uc_mcontext.gregs));

  // PC for mips is not part of gregs.
  ASSERT_EQ(static_cast<size_t>(MCONTEXT_PC_OFFSET),
            offsetof(ucontext_t,uc_mcontext.pc));

  ASSERT_EQ(static_cast<size_t>(MCONTEXT_FPREGS_OFFSET),
            offsetof(ucontext_t,uc_mcontext.fpregs));

  ASSERT_EQ(static_cast<size_t>(MCONTEXT_FPC_CSR),
            offsetof(ucontext_t,uc_mcontext.fpc_csr));
#else
  ASSERT_EQ(static_cast<size_t>(MCONTEXT_GREGS_OFFSET),
            offsetof(ucontext_t,uc_mcontext.gregs));
#endif
}

TEST(AndroidUContext, SigmakOffset) {
  ASSERT_EQ(static_cast<size_t>(UCONTEXT_SIGMASK_OFFSET),
            offsetof(ucontext_t,uc_sigmask));
}
