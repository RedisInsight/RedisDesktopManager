
# Tweak these for your system
OPENSSLINC=..\openssl-0.9.8x\inc32
OPENSSLLIB=..\openssl-0.9.8x\out32dll

ZLIBINC=-DLIBSSH2_HAVE_ZLIB=1 /I..\zlib-1.2.7
ZLIBLIB=..\zlib-1.2.7

!if "$(TARGET)" == ""
TARGET=Release
!endif

!if "$(TARGET)" == "Debug"
SUFFIX=_debug
CPPFLAGS=/Od /MDd
DLLFLAGS=/DEBUG /LDd
!else
CPPFLAGS=/Og /Oi /O2 /Oy /GF /Y- /MD /DNDEBUG
DLLFLAGS=/DEBUG /LD
!endif

CPPFLAGS=/nologo /GL /Zi /EHsc $(CPPFLAGS) /Iwin32 /Iinclude /I$(OPENSSLINC) $(ZLIBINC) -DLIBSSH2_WIN32
CFLAGS=$(CPPFLAGS)
RCFLAGS=/Iinclude
DLLFLAGS=$(CFLAGS) $(DLLFLAGS)
LIBS=$(OPENSSLLIB)\libeay32.lib $(OPENSSLLIB)\ssleay32.lib ws2_32.lib user32.lib $(ZLIBLIB)\zlib.lib

INTDIR=$(TARGET)\$(SUBDIR)


