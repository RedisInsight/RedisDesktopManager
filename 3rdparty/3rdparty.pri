#-------------------------------------------------
#
# Redis Desktop Manager Dependencies
#
#-------------------------------------------------

BREAKPADDIR = $$PWD/breakpad/src

INCLUDEPATH += $$PWD/libssh2/include
DEPENDPATH += $$PWD/libssh2/include
DEPENDPATH += $$BREAKPADDIR

win32-msvc* {
    INCLUDEPATH += $$BREAKPADDIR\client\windows\handler\
    INCLUDEPATH += $$BREAKPADDIR\common\windows\
    INCLUDEPATH += $$BREAKPADDIR\client\windows\crash_generation
    INCLUDEPATH += $$BREAKPADDIR\client\windows\common\
    INCLUDEPATH += $$BREAKPADDIR\google_breakpad\common\
    INCLUDEPATH += $$BREAKPADDIR\processor
    INCLUDEPATH += $$BREAKPADDIR\

    CONFIG(release, debug|release) {
        WIN_DEPS_PATH = $$PWD/libs/win32/release/
    } else: CONFIG(debug, debug|release) {
        WIN_DEPS_PATH = $$PWD/libs/win32/debug/
    }

    LIBS += -L$$WIN_DEPS_PATH -llibssh2
    LIBS += -lcommon -lcrash_generation_client -lexception_handler

    CONFIG(debug, debug|release) {
        LIBS += -llibeay32 -lssleay32 -lzlib
    }

    PRE_TARGETDEPS += $$WIN_DEPS_PATH/libssh2.lib

    QMAKE_CXXFLAGS += /MP
    QMAKE_LFLAGS_RELEASE += /MAP
    QMAKE_CFLAGS_RELEASE += /Zi
    QMAKE_LFLAGS_RELEASE += /debug /opt:ref
}

unix:macx { # OSX

    PRE_TARGETDEPS += /usr/local/lib/libssh2.dylib \
                     $$BREAKPADDIR/client/mac/build/Release/Breakpad.framework

    LIBS += /usr/local/lib/libssh2.dylib
    LIBS += $$BREAKPADDIR/client/mac/build/Release/Breakpad.framework/Versions/A/Breakpad
    LIBS += /System/Library/Frameworks/CoreFoundation.framework/Versions/A/CoreFoundation
    LIBS += /System/Library/Frameworks/CoreServices.framework/Versions/A/CoreServices

    INCLUDEPATH += $$PWD/libssh/include
    INCLUDEPATH += $$BREAKPADDIR/ \
                    $$BREAKPADDIR/client/mac/handler/ \
                    $$BREAKPADDIR/client/mac/ \
                    $$BREAKPADDIR/client/ \
                    $$BREAKPADDIR/client/mac/crash_generation/ \
                    $$BREAKPADDIR/common/mac/ \
                    $$BREAKPADDIR/common/linux/ \
                    $$BREAKPADDIR/common/ \
                    $$BREAKPADDIR/processor/ \
                    $$BREAKPADDIR/google_breakpad/common/ \
                    $$BREAKPADDIR/third_party/lss/ \

    #breakpad app need debug info inside binaries
    QMAKE_CXXFLAGS+=-g

    #deployment
    APP_DATA_FILES.files = $$BREAKPADDIR/client/mac/build/Release/Breakpad.framework
    APP_DATA_FILES.path = Contents/Frameworks
    QMAKE_BUNDLE_DATA += APP_DATA_FILES
}

unix:!macx { # ubuntu & debian

    QMAKE_CXXFLAGS += -std=gnu++0x -g #workaround for google breakpad

    LIBS += -Wl,-rpath=\\\$$ORIGIN/../lib #don't remove!!!
    LIBS += /usr/local/lib/libssh2.a \

    PRE_TARGETDEPS +=/usr/local/lib/libssh2.a \

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
          $$BREAKPADDIR/third_party/lss/linux_syscall_support.h

    SOURCES += $$BREAKPADDIR/client/linux/crash_generation/crash_generation_client.cc \
         $$BREAKPADDIR/client/linux/handler/exception_handler.cc \
         $$BREAKPADDIR/client/linux/handler/minidump_descriptor.cc \
         $$BREAKPADDIR/client/linux/minidump_writer/minidump_writer.cc \
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
