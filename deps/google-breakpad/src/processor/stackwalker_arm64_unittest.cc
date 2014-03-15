// Copyright (c) 2010, Google Inc.
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

// Original author: Jim Blandy <jimb@mozilla.com> <jimb@red-bean.com>

// stackwalker_arm64_unittest.cc: Unit tests for StackwalkerARM64 class.

#include <string.h>
#include <string>
#include <vector>

#include "breakpad_googletest_includes.h"
#include "common/test_assembler.h"
#include "common/using_std_string.h"
#include "google_breakpad/common/minidump_format.h"
#include "google_breakpad/processor/basic_source_line_resolver.h"
#include "google_breakpad/processor/call_stack.h"
#include "google_breakpad/processor/code_module.h"
#include "google_breakpad/processor/source_line_resolver_interface.h"
#include "google_breakpad/processor/stack_frame_cpu.h"
#include "processor/stackwalker_unittest_utils.h"
#include "processor/stackwalker_arm64.h"
#include "processor/windows_frame_info.h"

using google_breakpad::BasicSourceLineResolver;
using google_breakpad::CallStack;
using google_breakpad::CodeModule;
using google_breakpad::StackFrameSymbolizer;
using google_breakpad::StackFrame;
using google_breakpad::StackFrameARM64;
using google_breakpad::Stackwalker;
using google_breakpad::StackwalkerARM64;
using google_breakpad::SystemInfo;
using google_breakpad::WindowsFrameInfo;
using google_breakpad::test_assembler::kLittleEndian;
using google_breakpad::test_assembler::Label;
using google_breakpad::test_assembler::Section;
using std::vector;
using testing::_;
using testing::AnyNumber;
using testing::Return;
using testing::SetArgumentPointee;
using testing::Test;

class StackwalkerARM64Fixture {
 public:
  StackwalkerARM64Fixture()
    : stack_section(kLittleEndian),
      // Give the two modules reasonable standard locations and names
      // for tests to play with.
      module1(0x40000000, 0x10000, "module1", "version1"),
      module2(0x50000000, 0x10000, "module2", "version2") {
    // Identify the system as an iOS system, since that is the only platform
    // for which ARM64 support is currently enabled.
    system_info.os = "iOS";
    system_info.os_short = "ios";
    system_info.cpu = "arm64";
    system_info.cpu_info = "";

    // Put distinctive values in the raw CPU context.
    BrandContext(&raw_context);

    // Create some modules with some stock debugging information.
    modules.Add(&module1);
    modules.Add(&module2);

    // By default, none of the modules have symbol info; call
    // SetModuleSymbols to override this.
    EXPECT_CALL(supplier, GetCStringSymbolData(_, _, _, _, _))
      .WillRepeatedly(Return(MockSymbolSupplier::NOT_FOUND));

    // Avoid GMOCK WARNING "Uninteresting mock function call - returning
    // directly" for FreeSymbolData().
    EXPECT_CALL(supplier, FreeSymbolData(_)).Times(AnyNumber());

    // Reset max_frames_scanned since it's static.
    Stackwalker::set_max_frames_scanned(1024);
  }

  // Set the Breakpad symbol information that supplier should return for
  // MODULE to INFO.
  void SetModuleSymbols(MockCodeModule *module, const string &info) {
    size_t buffer_size;
    char *buffer = supplier.CopySymbolDataAndOwnTheCopy(info, &buffer_size);
    EXPECT_CALL(supplier, GetCStringSymbolData(module, &system_info, _, _, _))
      .WillRepeatedly(DoAll(SetArgumentPointee<3>(buffer),
                            SetArgumentPointee<4>(buffer_size),
                            Return(MockSymbolSupplier::FOUND)));
  }

  // Populate stack_region with the contents of stack_section. Use
  // stack_section.start() as the region's starting address.
  void RegionFromSection() {
    string contents;
    ASSERT_TRUE(stack_section.GetContents(&contents));
    stack_region.Init(stack_section.start().Value(), contents);
  }

  // Fill RAW_CONTEXT with pseudo-random data, for round-trip checking.
  void BrandContext(MDRawContextARM64 *raw_context) {
    uint8_t x = 173;
    for (size_t i = 0; i < sizeof(*raw_context); i++)
      reinterpret_cast<uint8_t *>(raw_context)[i] = (x += 17);
  }

  SystemInfo system_info;
  MDRawContextARM64 raw_context;
  Section stack_section;
  MockMemoryRegion stack_region;
  MockCodeModule module1;
  MockCodeModule module2;
  MockCodeModules modules;
  MockSymbolSupplier supplier;
  BasicSourceLineResolver resolver;
  CallStack call_stack;
  const vector<StackFrame *> *frames;
};

class SanityCheck: public StackwalkerARM64Fixture, public Test { };

TEST_F(SanityCheck, NoResolver) {
  // Since the context's frame pointer is garbage, the stack walk will end after
  // the first frame.
  StackFrameSymbolizer frame_symbolizer(NULL, NULL);
  StackwalkerARM64 walker(&system_info, &raw_context, &stack_region, &modules,
                          &frame_symbolizer);
  // This should succeed even without a resolver or supplier.
  vector<const CodeModule*> modules_without_symbols;
  vector<const CodeModule*> modules_with_corrupt_symbols;
  ASSERT_TRUE(walker.Walk(&call_stack, &modules_without_symbols,
                          &modules_with_corrupt_symbols));
  ASSERT_EQ(0U, modules_without_symbols.size());
  ASSERT_EQ(0U, modules_with_corrupt_symbols.size());
  frames = call_stack.frames();
  ASSERT_EQ(1U, frames->size());
  StackFrameARM64 *frame = static_cast<StackFrameARM64 *>(frames->at(0));
  // Check that the values from the original raw context made it
  // through to the context in the stack frame.
  EXPECT_EQ(0, memcmp(&raw_context, &frame->context, sizeof(raw_context)));
}

class GetContextFrame: public StackwalkerARM64Fixture, public Test { };

// The stackwalker should be able to produce the context frame even
// without stack memory present.
TEST_F(GetContextFrame, NoStackMemory) {
  StackFrameSymbolizer frame_symbolizer(&supplier, &resolver);
  StackwalkerARM64 walker(&system_info, &raw_context, NULL, &modules,
                          &frame_symbolizer);
  vector<const CodeModule*> modules_without_symbols;
  vector<const CodeModule*> modules_with_corrupt_symbols;
  ASSERT_TRUE(walker.Walk(&call_stack, &modules_without_symbols,
                          &modules_with_corrupt_symbols));
  ASSERT_EQ(0U, modules_without_symbols.size());
  ASSERT_EQ(0U, modules_with_corrupt_symbols.size());
  frames = call_stack.frames();
  ASSERT_EQ(1U, frames->size());
  StackFrameARM64 *frame = static_cast<StackFrameARM64 *>(frames->at(0));
  // Check that the values from the original raw context made it
  // through to the context in the stack frame.
  EXPECT_EQ(0, memcmp(&raw_context, &frame->context, sizeof(raw_context)));
}

class GetCallerFrame: public StackwalkerARM64Fixture, public Test { };

TEST_F(GetCallerFrame, ScanWithoutSymbols) {
  // When the stack walker resorts to scanning the stack,
  // only addresses located within loaded modules are
  // considered valid return addresses.
  // Force scanning through three frames to ensure that the
  // stack pointer is set properly in scan-recovered frames.
  stack_section.start() = 0x80000000;
  uint64_t return_address1 = 0x50000100;
  uint64_t return_address2 = 0x50000900;
  Label frame1_sp, frame2_sp;
  stack_section
    // frame 0
    .Append(16, 0)                      // space

    .D64(0x40090000)                    // junk that's not
    .D64(0x60000000)                    // a return address

    .D64(return_address1)               // actual return address
    // frame 1
    .Mark(&frame1_sp)
    .Append(16, 0)                      // space

    .D64(0xF0000000)                    // more junk
    .D64(0x0000000D)

    .D64(return_address2)               // actual return address
    // frame 2
    .Mark(&frame2_sp)
    .Append(64, 0);                     // end of stack
  RegionFromSection();

  raw_context.iregs[MD_CONTEXT_ARM64_REG_PC] = 0x40005510;
  raw_context.iregs[MD_CONTEXT_ARM64_REG_SP] = stack_section.start().Value();

  StackFrameSymbolizer frame_symbolizer(&supplier, &resolver);
  StackwalkerARM64 walker(&system_info, &raw_context, &stack_region, &modules,
                          &frame_symbolizer);
  vector<const CodeModule*> modules_without_symbols;
  vector<const CodeModule*> modules_with_corrupt_symbols;
  ASSERT_TRUE(walker.Walk(&call_stack, &modules_without_symbols,
                          &modules_with_corrupt_symbols));
  ASSERT_EQ(2U, modules_without_symbols.size());
  ASSERT_EQ("module1", modules_without_symbols[0]->debug_file());
  ASSERT_EQ("module2", modules_without_symbols[1]->debug_file());
  ASSERT_EQ(0U, modules_with_corrupt_symbols.size());
  frames = call_stack.frames();
  ASSERT_EQ(3U, frames->size());

  StackFrameARM64 *frame0 = static_cast<StackFrameARM64 *>(frames->at(0));
  EXPECT_EQ(StackFrame::FRAME_TRUST_CONTEXT, frame0->trust);
  ASSERT_EQ(StackFrameARM64::CONTEXT_VALID_ALL,
            frame0->context_validity);
  EXPECT_EQ(0, memcmp(&raw_context, &frame0->context, sizeof(raw_context)));

  StackFrameARM64 *frame1 = static_cast<StackFrameARM64 *>(frames->at(1));
  EXPECT_EQ(StackFrame::FRAME_TRUST_SCAN, frame1->trust);
  ASSERT_EQ((StackFrameARM64::CONTEXT_VALID_PC |
             StackFrameARM64::CONTEXT_VALID_SP),
            frame1->context_validity);
  EXPECT_EQ(return_address1, frame1->context.iregs[MD_CONTEXT_ARM64_REG_PC]);
  EXPECT_EQ(frame1_sp.Value(), frame1->context.iregs[MD_CONTEXT_ARM64_REG_SP]);

  StackFrameARM64 *frame2 = static_cast<StackFrameARM64 *>(frames->at(2));
  EXPECT_EQ(StackFrame::FRAME_TRUST_SCAN, frame2->trust);
  ASSERT_EQ((StackFrameARM64::CONTEXT_VALID_PC |
             StackFrameARM64::CONTEXT_VALID_SP),
            frame2->context_validity);
  EXPECT_EQ(return_address2, frame2->context.iregs[MD_CONTEXT_ARM64_REG_PC]);
  EXPECT_EQ(frame2_sp.Value(), frame2->context.iregs[MD_CONTEXT_ARM64_REG_SP]);
}

TEST_F(GetCallerFrame, ScanWithFunctionSymbols) {
  // During stack scanning, if a potential return address
  // is located within a loaded module that has symbols,
  // it is only considered a valid return address if it
  // lies within a function's bounds.
  stack_section.start() = 0x80000000;
  uint64_t return_address = 0x50000200;
  Label frame1_sp;

  stack_section
    // frame 0
    .Append(16, 0)                      // space

    .D64(0x40090000)                    // junk that's not
    .D64(0x60000000)                    // a return address

    .D64(0x40001000)                    // a couple of plausible addresses
    .D64(0x5000F000)                    // that are not within functions

    .D64(return_address)                // actual return address
    // frame 1
    .Mark(&frame1_sp)
    .Append(64, 0);                     // end of stack
  RegionFromSection();

  raw_context.iregs[MD_CONTEXT_ARM64_REG_PC] = 0x40000200;
  raw_context.iregs[MD_CONTEXT_ARM64_REG_SP] = stack_section.start().Value();

  SetModuleSymbols(&module1,
                   // The youngest frame's function.
                   "FUNC 100 400 10 monotreme\n");
  SetModuleSymbols(&module2,
                   // The calling frame's function.
                   "FUNC 100 400 10 marsupial\n");

  StackFrameSymbolizer frame_symbolizer(&supplier, &resolver);
  StackwalkerARM64 walker(&system_info, &raw_context, &stack_region, &modules,
                          &frame_symbolizer);
  vector<const CodeModule*> modules_without_symbols;
  vector<const CodeModule*> modules_with_corrupt_symbols;
  ASSERT_TRUE(walker.Walk(&call_stack, &modules_without_symbols,
                          &modules_with_corrupt_symbols));
  ASSERT_EQ(0U, modules_without_symbols.size());
  ASSERT_EQ(0U, modules_with_corrupt_symbols.size());
  frames = call_stack.frames();
  ASSERT_EQ(2U, frames->size());

  StackFrameARM64 *frame0 = static_cast<StackFrameARM64 *>(frames->at(0));
  EXPECT_EQ(StackFrame::FRAME_TRUST_CONTEXT, frame0->trust);
  ASSERT_EQ(StackFrameARM64::CONTEXT_VALID_ALL,
            frame0->context_validity);
  EXPECT_EQ(0, memcmp(&raw_context, &frame0->context, sizeof(raw_context)));
  EXPECT_EQ("monotreme", frame0->function_name);
  EXPECT_EQ(0x40000100ULL, frame0->function_base);

  StackFrameARM64 *frame1 = static_cast<StackFrameARM64 *>(frames->at(1));
  EXPECT_EQ(StackFrame::FRAME_TRUST_SCAN, frame1->trust);
  ASSERT_EQ((StackFrameARM64::CONTEXT_VALID_PC |
             StackFrameARM64::CONTEXT_VALID_SP),
            frame1->context_validity);
  EXPECT_EQ(return_address, frame1->context.iregs[MD_CONTEXT_ARM64_REG_PC]);
  EXPECT_EQ(frame1_sp.Value(), frame1->context.iregs[MD_CONTEXT_ARM64_REG_SP]);
  EXPECT_EQ("marsupial", frame1->function_name);
  EXPECT_EQ(0x50000100ULL, frame1->function_base);
}

TEST_F(GetCallerFrame, ScanFirstFrame) {
  // If the stackwalker resorts to stack scanning, it will scan much
  // farther to find the caller of the context frame.
  stack_section.start() = 0x80000000;
  uint64_t return_address1 = 0x50000100;
  uint64_t return_address2 = 0x50000900;
  Label frame1_sp, frame2_sp;
  stack_section
    // frame 0
    .Append(32, 0)                      // space

    .D64(0x40090000)                    // junk that's not
    .D64(0x60000000)                    // a return address

    .Append(96, 0)                      // more space

    .D64(return_address1)               // actual return address
    // frame 1
    .Mark(&frame1_sp)
    .Append(32, 0)                      // space

    .D64(0xF0000000)                    // more junk
    .D64(0x0000000D)

    .Append(256, 0)                     // more space

    .D64(return_address2)               // actual return address
                                        // (won't be found)
    // frame 2
    .Mark(&frame2_sp)
    .Append(64, 0);                     // end of stack
  RegionFromSection();

  raw_context.iregs[MD_CONTEXT_ARM64_REG_PC] = 0x40005510;
  raw_context.iregs[MD_CONTEXT_ARM64_REG_SP] = stack_section.start().Value();

  StackFrameSymbolizer frame_symbolizer(&supplier, &resolver);
  StackwalkerARM64 walker(&system_info, &raw_context, &stack_region, &modules,
                          &frame_symbolizer);
  vector<const CodeModule*> modules_without_symbols;
  vector<const CodeModule*> modules_with_corrupt_symbols;
  ASSERT_TRUE(walker.Walk(&call_stack, &modules_without_symbols,
                          &modules_with_corrupt_symbols));
  ASSERT_EQ(2U, modules_without_symbols.size());
  ASSERT_EQ("module1", modules_without_symbols[0]->debug_file());
  ASSERT_EQ("module2", modules_without_symbols[1]->debug_file());
  ASSERT_EQ(0U, modules_with_corrupt_symbols.size());
  frames = call_stack.frames();
  ASSERT_EQ(2U, frames->size());

  StackFrameARM64 *frame0 = static_cast<StackFrameARM64 *>(frames->at(0));
  EXPECT_EQ(StackFrame::FRAME_TRUST_CONTEXT, frame0->trust);
  ASSERT_EQ(StackFrameARM64::CONTEXT_VALID_ALL,
            frame0->context_validity);
  EXPECT_EQ(0, memcmp(&raw_context, &frame0->context, sizeof(raw_context)));

  StackFrameARM64 *frame1 = static_cast<StackFrameARM64 *>(frames->at(1));
  EXPECT_EQ(StackFrame::FRAME_TRUST_SCAN, frame1->trust);
  ASSERT_EQ((StackFrameARM64::CONTEXT_VALID_PC |
             StackFrameARM64::CONTEXT_VALID_SP),
            frame1->context_validity);
  EXPECT_EQ(return_address1, frame1->context.iregs[MD_CONTEXT_ARM64_REG_PC]);
  EXPECT_EQ(frame1_sp.Value(), frame1->context.iregs[MD_CONTEXT_ARM64_REG_SP]);
}

// Test that set_max_frames_scanned prevents using stack scanning
// to find caller frames.
TEST_F(GetCallerFrame, ScanningNotAllowed) {
  // When the stack walker resorts to scanning the stack,
  // only addresses located within loaded modules are
  // considered valid return addresses.
  stack_section.start() = 0x80000000;
  uint64_t return_address1 = 0x50000100;
  uint64_t return_address2 = 0x50000900;
  Label frame1_sp, frame2_sp;
  stack_section
    // frame 0
    .Append(16, 0)                      // space

    .D64(0x40090000)                    // junk that's not
    .D64(0x60000000)                    // a return address

    .D64(return_address1)               // actual return address
    // frame 1
    .Mark(&frame1_sp)
    .Append(16, 0)                      // space

    .D64(0xF0000000)                    // more junk
    .D64(0x0000000D)

    .D64(return_address2)               // actual return address
    // frame 2
    .Mark(&frame2_sp)
    .Append(64, 0);                     // end of stack
  RegionFromSection();

  raw_context.iregs[MD_CONTEXT_ARM64_REG_PC] = 0x40005510;
  raw_context.iregs[MD_CONTEXT_ARM64_REG_SP] = stack_section.start().Value();

  StackFrameSymbolizer frame_symbolizer(&supplier, &resolver);
  StackwalkerARM64 walker(&system_info, &raw_context, &stack_region, &modules,
                          &frame_symbolizer);
  Stackwalker::set_max_frames_scanned(0);

  vector<const CodeModule*> modules_without_symbols;
  vector<const CodeModule*> modules_with_corrupt_symbols;
  ASSERT_TRUE(walker.Walk(&call_stack, &modules_without_symbols,
                          &modules_with_corrupt_symbols));
  ASSERT_EQ(1U, modules_without_symbols.size());
  ASSERT_EQ("module1", modules_without_symbols[0]->debug_file());
  ASSERT_EQ(0U, modules_with_corrupt_symbols.size());
  frames = call_stack.frames();
  ASSERT_EQ(1U, frames->size());

  StackFrameARM64 *frame0 = static_cast<StackFrameARM64 *>(frames->at(0));
  EXPECT_EQ(StackFrame::FRAME_TRUST_CONTEXT, frame0->trust);
  ASSERT_EQ(StackFrameARM64::CONTEXT_VALID_ALL,
            frame0->context_validity);
  EXPECT_EQ(0, memcmp(&raw_context, &frame0->context, sizeof(raw_context)));
}

class GetFramesByFramePointer: public StackwalkerARM64Fixture, public Test { };

TEST_F(GetFramesByFramePointer, OnlyFramePointer) {
  stack_section.start() = 0x80000000;
  uint64_t return_address1 = 0x50000100;
  uint64_t return_address2 = 0x50000900;
  Label frame1_sp, frame2_sp;
  Label frame1_fp, frame2_fp;
  stack_section
    // frame 0
    .Append(64, 0)           // Whatever values on the stack.
    .D64(0x0000000D)         // junk that's not
    .D64(0xF0000000)         // a return address.

    .Mark(&frame1_fp)        // Next fp will point to the next value.
    .D64(frame2_fp)          // Save current frame pointer.
    .D64(return_address2)    // Save current link register.
    .Mark(&frame1_sp)

    // frame 1
    .Append(64, 0)           // Whatever values on the stack.
    .D64(0x0000000D)         // junk that's not
    .D64(0xF0000000)         // a return address.

    .Mark(&frame2_fp)
    .D64(0)
    .D64(0)
    .Mark(&frame2_sp)

    // frame 2
    .Append(64, 0)           // Whatever values on the stack.
    .D64(0x0000000D)         // junk that's not
    .D64(0xF0000000);        // a return address.
  RegionFromSection();


  raw_context.iregs[MD_CONTEXT_ARM64_REG_PC] = 0x40005510;
  raw_context.iregs[MD_CONTEXT_ARM64_REG_LR] = return_address1;
  raw_context.iregs[MD_CONTEXT_ARM64_REG_FP] = frame1_fp.Value();
  raw_context.iregs[MD_CONTEXT_ARM64_REG_SP] = stack_section.start().Value();

  StackFrameSymbolizer frame_symbolizer(&supplier, &resolver);
  StackwalkerARM64 walker(&system_info, &raw_context,
                          &stack_region, &modules, &frame_symbolizer);

  vector<const CodeModule*> modules_without_symbols;
  vector<const CodeModule*> modules_with_corrupt_symbols;
  ASSERT_TRUE(walker.Walk(&call_stack, &modules_without_symbols,
                          &modules_with_corrupt_symbols));
  ASSERT_EQ(2U, modules_without_symbols.size());
  ASSERT_EQ("module1", modules_without_symbols[0]->debug_file());
  ASSERT_EQ("module2", modules_without_symbols[1]->debug_file());
  ASSERT_EQ(0U, modules_with_corrupt_symbols.size());
  frames = call_stack.frames();
  ASSERT_EQ(3U, frames->size());

  StackFrameARM64 *frame0 = static_cast<StackFrameARM64 *>(frames->at(0));
  EXPECT_EQ(StackFrame::FRAME_TRUST_CONTEXT, frame0->trust);
  ASSERT_EQ(StackFrameARM64::CONTEXT_VALID_ALL,
            frame0->context_validity);
  EXPECT_EQ(0, memcmp(&raw_context, &frame0->context, sizeof(raw_context)));

  StackFrameARM64 *frame1 = static_cast<StackFrameARM64 *>(frames->at(1));
  EXPECT_EQ(StackFrame::FRAME_TRUST_FP, frame1->trust);
  ASSERT_EQ((StackFrameARM64::CONTEXT_VALID_PC |
             StackFrameARM64::CONTEXT_VALID_LR |
             StackFrameARM64::CONTEXT_VALID_FP |
             StackFrameARM64::CONTEXT_VALID_SP),
            frame1->context_validity);
  EXPECT_EQ(return_address1, frame1->context.iregs[MD_CONTEXT_ARM64_REG_PC]);
  EXPECT_EQ(return_address2, frame1->context.iregs[MD_CONTEXT_ARM64_REG_LR]);
  EXPECT_EQ(frame1_sp.Value(), frame1->context.iregs[MD_CONTEXT_ARM64_REG_SP]);
  EXPECT_EQ(frame2_fp.Value(),
            frame1->context.iregs[MD_CONTEXT_ARM64_REG_FP]);

  StackFrameARM64 *frame2 = static_cast<StackFrameARM64 *>(frames->at(2));
  EXPECT_EQ(StackFrame::FRAME_TRUST_FP, frame2->trust);
  ASSERT_EQ((StackFrameARM64::CONTEXT_VALID_PC |
             StackFrameARM64::CONTEXT_VALID_LR |
             StackFrameARM64::CONTEXT_VALID_FP |
             StackFrameARM64::CONTEXT_VALID_SP),
            frame2->context_validity);
  EXPECT_EQ(return_address2, frame2->context.iregs[MD_CONTEXT_ARM64_REG_PC]);
  EXPECT_EQ(0U, frame2->context.iregs[MD_CONTEXT_ARM64_REG_LR]);
  EXPECT_EQ(frame2_sp.Value(), frame2->context.iregs[MD_CONTEXT_ARM64_REG_SP]);
  EXPECT_EQ(0U, frame2->context.iregs[MD_CONTEXT_ARM64_REG_FP]);
}
