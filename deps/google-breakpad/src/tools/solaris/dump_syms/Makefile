# Copyright (c) 2007, Google Inc.
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

# Author: Alfred Peng

CXX=CC
CC=cc

CXXFLAGS=-g -xs -xdebugformat=stabs -I../../.. -I../../../common/solaris -lelf -ldemangle -D_REENTRANT

.PHONY:all clean

BIN=dump_syms

all:$(BIN)

DUMP_OBJ=dump_symbols.o guid_creator.o dump_syms.o file_id.o md5.o

dump_syms:$(DUMP_OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^

dump_symbols.o:../../../common/solaris/dump_symbols.cc
	$(CXX) $(CXXFLAGS) -c $^

guid_creator.o:../../../common/solaris/guid_creator.cc
	$(CXX) $(CXXFLAGS) -c $^

file_id.o:../../../common/solaris/file_id.cc
	$(CXX) $(CXXFLAGS) -c $^

md5.o:../../../common/md5.cc
	$(CXX) $(CXXFLAGS) -c $^

test:all
	./run_regtest.sh

clean:
	rm -f $(BIN) $(DUMP_OBJ)
