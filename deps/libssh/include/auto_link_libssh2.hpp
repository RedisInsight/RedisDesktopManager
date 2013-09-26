#pragma once
#define LIBSSH2_LIB_NAME "libssh2-1.4.3"
#define XML_STATIC

#if defined(XML_STATIC)
#  define LIBSSH2_LIB_PREFIX "lib"
#else
#  define LIBSSH2_LIB_PREFIX 
#endif

#  define LIBSSH2_LIB_PLATFORM "-x86"

#ifdef _DLL
#  if defined(_DEBUG)
// MDd
#    define LIBSSH2_LIB_RT_OPT "-gd"
#  else
// MD
#    define LIBSSH2_LIB_RT_OPT
#  endif
#else
#  if defined(_DEBUG)
// MTd
#    define LIBSSH2_LIB_RT_OPT "-sgd"
#  else
// MT
#    define LIBSSH2_LIB_RT_OPT "-s"
#  endif
#endif

#  pragma comment(lib, LIBSSH2_LIB_PREFIX LIBSSH2_LIB_NAME  LIBSSH2_LIB_RT_OPT LIBSSH2_LIB_PLATFORM ".lib")
