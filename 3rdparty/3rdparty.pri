#-------------------------------------------------
#
# Redis Desktop Manager Dependencies
#
#-------------------------------------------------

BREAKPADDIR = $$PWD/breakpad/src

INCLUDEPATH += $$PWD/libssh2/include

INCLUDEPATH += $$PWD/qtconsole/include
HEADERS += $$PWD/qtconsole/include/qconsole.h
SOURCES += $$PWD/qtconsole/src/qconsole.cpp

INCLUDEPATH += $$PWD/easyloggingpp/src
HEADERS += $$PWD/easyloggingpp/src/easylogging++.h

INCLUDEPATH += $$BREAKPADDIR/
INCLUDEPATH += $$BREAKPADDIR/src
DEPENDPATH += $$PWD/libssh2/include
DEPENDPATH += $$BREAKPADDIR

#breakpad app need debug info inside binaries
QMAKE_CXXFLAGS+=-g
QMAKE_CFLAGS_RELEASE+=-g


win32* {    
    # Workaround for mingw
    QMAKE_LFLAGS_RELEASE=
    LIBS += -lssh2 -lssl -lz

    HEADERS += $$BREAKPADDIR/common/windows/string_utils-inl.h
    HEADERS += $$BREAKPADDIR/common/windows/guid_string.h
    HEADERS += $$BREAKPADDIR/client/windows/handler/exception_handler.h
    HEADERS += $$BREAKPADDIR/client/windows/common/ipc_protocol.h
    HEADERS += $$BREAKPADDIR/google_breakpad/common/minidump_format.h
    HEADERS += $$BREAKPADDIR/google_breakpad/common/breakpad_types.h
    HEADERS += $$BREAKPADDIR/client/windows/crash_generation/crash_generation_client.h
    HEADERS += $$BREAKPADDIR/common/scoped_ptr.h
    SOURCES += $$BREAKPADDIR/client/windows/handler/exception_handler.cc
    SOURCES += $$BREAKPADDIR/common/windows/string_utils.cc
    SOURCES += $$BREAKPADDIR/common/windows/guid_string.cc
    SOURCES += $$BREAKPADDIR/client/windows/crash_generation/crash_generation_client.cc
}

unix:macx { # OSX
    PRE_TARGETDEPS += /usr/local/lib/libssh2.dylib \
                     $$BREAKPADDIR/client/mac/build/Release/Breakpad.framework

    LIBS += /usr/local/lib/libssh2.dylib
    LIBS += $$BREAKPADDIR/client/mac/build/Release/Breakpad.framework/Versions/A/Breakpad
    LIBS += /System/Library/Frameworks/CoreFoundation.framework/Versions/A/CoreFoundation
    LIBS += /System/Library/Frameworks/CoreServices.framework/Versions/A/CoreServices

    #deployment
    APP_DATA_FILES.files = $$BREAKPADDIR/client/mac/build/Release/Breakpad.framework
    APP_DATA_FILES.path = Contents/Frameworks
    QMAKE_BUNDLE_DATA += APP_DATA_FILES
}

unix:!macx { # ubuntu & debian
    QMAKE_CXXFLAGS += -std=gnu++0x #workaround for google breakpad

    # clean default flags
    QMAKE_LFLAGS_RPATH=

    LIBS += -Wl,-rpath=\\\$$ORIGIN/../lib #don't remove!!!
#    LIBS += /usr/local/lib/libssh2.a
#    PRE_TARGETDEPS +=/usr/local/lib/libssh2.a \
    LIBS += -lcrypto -lz -lssh2

    HEADERS += $$BREAKPADDIR/client/linux/minidump_writer/cpu_set.h \
          $$BREAKPADDIR/client/linux/minidump_writer/proc_cpuinfo_reader.h \
          $$BREAKPADDIR/client/linux/handler/exception_handler.h \
          $$BREAKPADDIR/client/linux/crash_generation/crash_generation_client.h \
          $$BREAKPADDIR/client/linux/handler/minidump_descriptor.h \
          $$BREAKPADDIR/client/linux/minidump_writer/minidump_writer.h \
          $$BREAKPADDIR/client/linux/minidump_writer/line_reader.h \
          $$BREAKPADDIR/client/linux/minidump_writer/linux_dumper.h \
          $$BREAKPADDIR/client/linux/minidump_writer/linux_ptrace_dumper.h \
          $$BREAKPADDIR/client/linux/minidump_writer/directory_reader.h \
          $$BREAKPADDIR/client/linux/log/log.h \
          $$BREAKPADDIR/client/minidump_file_writer-inl.h \
          $$BREAKPADDIR/client/minidump_file_writer.h \
          $$BREAKPADDIR/common/linux/linux_libc_support.h \
          $$BREAKPADDIR/common/linux/eintr_wrapper.h \
          $$BREAKPADDIR/common/linux/ignore_ret.h \
          $$BREAKPADDIR/common/linux/file_id.h \
          $$BREAKPADDIR/common/linux/memory_mapped_file.h \
          $$BREAKPADDIR/common/linux/safe_readlink.h \
          $$BREAKPADDIR/common/linux/guid_creator.h \
          $$BREAKPADDIR/common/linux/elfutils.h \
          $$BREAKPADDIR/common/linux/elfutils-inl.h \
          $$BREAKPADDIR/common/linux/elf_gnu_compat.h \
          $$BREAKPADDIR/common/using_std_string.h \
          $$BREAKPADDIR/common/memory.h \
          $$BREAKPADDIR/common/basictypes.h \
          $$BREAKPADDIR/common/memory_range.h \
          $$BREAKPADDIR/common/string_conversion.h \
          $$BREAKPADDIR/common/convert_UTF.h \
          $$BREAKPADDIR/google_breakpad/common/minidump_format.h \
          $$BREAKPADDIR/google_breakpad/common/minidump_size.h \
          $$BREAKPADDIR/google_breakpad/common/breakpad_types.h \
          $$BREAKPADDIR/common/scoped_ptr.h \
          $$BREAKPADDIR/src/third_party/lss/linux_syscall_support.h

    SOURCES += $$BREAKPADDIR/client/linux/crash_generation/crash_generation_client.cc \
         $$BREAKPADDIR/client/linux/handler/exception_handler.cc \
         $$BREAKPADDIR/client/linux/handler/minidump_descriptor.cc \
         $$BREAKPADDIR/client/linux/minidump_writer/minidump_writer.cc \
         $$BREAKPADDIR/client/linux/dump_writer_common/thread_info.cc \
         $$BREAKPADDIR/client/linux/dump_writer_common/seccomp_unwinder.cc \
         $$BREAKPADDIR/client/linux/dump_writer_common/ucontext_reader.cc \
         $$BREAKPADDIR/client/linux/microdump_writer/microdump_writer.cc \
         $$BREAKPADDIR/client/linux/minidump_writer/linux_dumper.cc \
         $$BREAKPADDIR/client/linux/minidump_writer/linux_ptrace_dumper.cc \
         $$BREAKPADDIR/client/linux/log/log.cc \
         $$BREAKPADDIR/client/minidump_file_writer.cc \
         $$BREAKPADDIR/common/linux/linux_libc_support.cc \
         $$BREAKPADDIR/common/linux/file_id.cc \
         $$BREAKPADDIR/common/linux/memory_mapped_file.cc \
         $$BREAKPADDIR/common/linux/safe_readlink.cc \
         $$BREAKPADDIR/common/linux/guid_creator.cc \
         $$BREAKPADDIR/common/linux/elfutils.cc \
         $$BREAKPADDIR/common/string_conversion.cc \
         $$BREAKPADDIR/common/convert_UTF.c
}
