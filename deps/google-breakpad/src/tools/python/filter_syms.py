#!/usr/bin/env python
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

"""Normalizes and de-duplicates paths within Breakpad symbol files.

When using DWARF for storing debug symbols, some file information will be
stored relative to the current working directory of the current compilation
unit, and may be further relativized based upon how the file was #included.

This helper can be used to parse the Breakpad symbol file generated from such
DWARF files and normalize and de-duplicate the FILE records found within,
updating any references to the FILE records in the other record types.
"""

import macpath
import ntpath
import optparse
import os
import posixpath
import sys

class BreakpadParseError(Exception):
  """Unsupported Breakpad symbol record exception class."""
  pass

class SymbolFileParser(object):
  """Parser for Breakpad symbol files.

  The format of these files is documented at
  https://code.google.com/p/google-breakpad/wiki/SymbolFiles
  """

  def __init__(self, input_stream, output_stream, ignored_prefixes=None,
               path_handler=os.path):
    """Inits a SymbolFileParser to read symbol records from |input_stream| and
    write the processed output to |output_stream|.
    
    |ignored_prefixes| contains a list of optional path prefixes that
    should be stripped from the final, normalized path outputs.
    
    For example, if the Breakpad symbol file had all paths starting with a
    common prefix, such as:
      FILE 1 /b/build/src/foo.cc
      FILE 2 /b/build/src/bar.cc
    Then adding "/b/build/src" as an ignored prefix would result in an output
    file that contained:
      FILE 1 foo.cc
      FILE 2 bar.cc
    
    Note that |ignored_prefixes| does not necessarily contain file system
    paths, as the contents of the DWARF DW_AT_comp_dir attribute is dependent
    upon the host system and compiler, and may contain additional information
    such as hostname or compiler version.
    """

    self.unique_files = {}
    self.duplicate_files = {}
    self.input_stream = input_stream
    self.output_stream = output_stream
    self.ignored_prefixes = ignored_prefixes or []
    self.path_handler = path_handler

  def Process(self):
    """Processes the Breakpad symbol file."""
    for line in self.input_stream:
      parsed = self._ParseRecord(line.rstrip())
      if parsed:
        self.output_stream.write(parsed + '\n')

  def _ParseRecord(self, record):
    """Parses a single Breakpad symbol record - a single line from the symbol
    file.

    Returns:
        The modified string to write to the output file, or None if no line
        should be written.
    """
    record_type = record.partition(' ')[0]
    if record_type == 'FILE':
      return self._ParseFileRecord(record)
    elif self._IsLineRecord(record_type):
      return self._ParseLineRecord(record)
    else:
      # Simply pass the record through unaltered.
      return record

  def _NormalizePath(self, path):
    """Normalizes a file path to its canonical form.

    As this may not execute on the machine or file system originally
    responsible for compilation, it may be necessary to further correct paths
    for symlinks, junctions, or other such file system indirections.

    Returns:
        A unique, canonical representation for the the file path.
    """
    return self.path_handler.normpath(path)

  def _AdjustPath(self, path):
    """Adjusts the supplied path after performing path de-duplication.

    This may be used to perform secondary adjustments, such as removing a
    common prefix, such as "/D/build", or replacing the file system path with
    information from the version control system.

    Returns:
        The actual path to use when writing the FILE record.
    """
    return path[len(filter(path.startswith,
                           self.ignored_prefixes + [''])[0]):]

  def _ParseFileRecord(self, file_record):
    """Parses and corrects a FILE record."""
    file_info = file_record[5:].split(' ', 3)
    if len(file_info) > 2:
      raise BreakpadParseError('Unsupported FILE record: ' + file_record)
    file_index = int(file_info[0])
    file_name = self._NormalizePath(file_info[1])
    existing_file_index = self.unique_files.get(file_name)
    if existing_file_index is None:
      self.unique_files[file_name] = file_index
      file_info[1] = self._AdjustPath(file_name)
      return 'FILE ' + ' '.join(file_info)
    else:
      self.duplicate_files[file_index] = existing_file_index
      return None

  def _IsLineRecord(self, record_type):
    """Determines if the current record type is a Line record"""
    try:
      line = int(record_type, 16)
    except (ValueError, TypeError):
      return False
    return True

  def _ParseLineRecord(self, line_record):
    """Parses and corrects a Line record."""
    line_info = line_record.split(' ', 5)
    if len(line_info) > 4:
      raise BreakpadParseError('Unsupported Line record: ' + line_record)
    file_index = int(line_info[3])
    line_info[3] = str(self.duplicate_files.get(file_index, file_index))
    return ' '.join(line_info)

def main():
  option_parser = optparse.OptionParser()
  option_parser.add_option("-p", "--prefix",
                           action="append", dest="prefixes", type="string",
                           default=[],
                           help="A path prefix that should be removed from "
                                "all FILE lines. May be repeated to specify "
                                "multiple prefixes.")
  option_parser.add_option("-t", "--path_type",
                           action="store", type="choice", dest="path_handler",
                           choices=['win32', 'posix'],
                           help="Indicates how file paths should be "
                                "interpreted. The default is to treat paths "
                                "the same as the OS running Python (eg: "
                                "os.path)")
  options, args = option_parser.parse_args()
  if args:
    option_parser.error('Unknown argument: %s' % args)

  path_handler = { 'win32': ntpath,
                   'posix': posixpath }.get(options.path_handler, os.path)
  try:
    symbol_parser = SymbolFileParser(sys.stdin, sys.stdout, options.prefixes,
                                     path_handler)
    symbol_parser.Process()
  except BreakpadParseError, e:
    print >> sys.stderr, 'Got an error while processing symbol file'
    print >> sys.stderr, str(e)
    return 1
  return 0

if __name__ == '__main__':
  sys.exit(main())
