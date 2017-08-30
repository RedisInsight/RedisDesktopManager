// Copyright 2007-2010 Baptiste Lepilleur
// Distributed under MIT license, or public domain if desired and
// recognized in your jurisdiction.
// See file LICENSE for detail or copy at http://jsoncpp.sourceforge.net/LICENSE

#ifndef CPPTL_JSON_ASSERTIONS_H_INCLUDED
# define CPPTL_JSON_ASSERTIONS_H_INCLUDED

#include <stdlib.h>

#if !defined(JSON_IS_AMALGAMATION)
# include <json/config.h>
#endif // if !defined(JSON_IS_AMALGAMATION)

#if JSON_USE_EXCEPTION
#define JSON_ASSERT( condition ) assert( condition );  // @todo <= change this into an exception throw
#define JSON_FAIL_MESSAGE( message ) throw std::runtime_error( message );
#else  // JSON_USE_EXCEPTION
#define JSON_ASSERT( condition ) assert( condition );

// The call to assert() will show the failure message in debug builds. In
// release bugs we write to invalid memory in order to crash hard, so that a
// debugger or crash reporter gets the chance to take over. We still call exit()
// afterward in order to tell the compiler that this macro doesn't return.
#define JSON_FAIL_MESSAGE( message ) { assert(false && message); strcpy(reinterpret_cast<char*>(666), message); exit(123); }

#endif

#define JSON_ASSERT_MESSAGE( condition, message ) if (!( condition )) { JSON_FAIL_MESSAGE( message ) }

#endif // CPPTL_JSON_ASSERTIONS_H_INCLUDED
