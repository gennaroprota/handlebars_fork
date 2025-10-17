//
//  Copyright (c) 2023 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/cppalliance/handlebars
//

#ifndef BOOST_HANDLEBARS_CONFIG_HPP
#define BOOST_HANDLEBARS_CONFIG_HPP

/*
    Platform-specific things, and stuff
    that is dependent on the toolchain.
*/

namespace boost {
namespace handlebars {

//------------------------------------------------
//
// Shared Libraries
//
//------------------------------------------------

// static linking
#if defined(BOOST_HANDLEBARS_STATIC_LINK)
# define BOOST_HANDLEBARS_DECL
# define BOOST_HANDLEBARS_VISIBLE

// MSVC
#elif defined(_MSC_VER)
# define BOOST_HANDLEBARS_SYMBOL_EXPORT __declspec(dllexport)
# define BOOST_HANDLEBARS_SYMBOL_IMPORT __declspec(dllimport)
# if defined(BOOST_HANDLEBARS_BUILD) // building the library
#  define BOOST_HANDLEBARS_DECL BOOST_HANDLEBARS_SYMBOL_EXPORT
# else
#  define BOOST_HANDLEBARS_DECL BOOST_HANDLEBARS_SYMBOL_IMPORT
# endif
# define BOOST_HANDLEBARS_VISIBLE

// (unknown)
#elif defined(__GNUC__)
# if defined(BOOST_HANDLEBARS_BUILD) // building the library
#   define BOOST_HANDLEBARS_DECL
# else
#   define BOOST_HANDLEBARS_DECL __attribute__((__visibility__("default")))
#endif
# define BOOST_HANDLEBARS_VISIBLE __attribute__((__visibility__("default")))
#else
# error unknown platform for dynamic linking
#endif

//------------------------------------------------

#ifndef FMT_CONSTEVAL
# if !defined(__GNUC__) && defined(_MSC_VER)
#  define FMT_CONSTEVAL
# endif
#endif

#if ! defined(__x86_64__) && ! defined(_WIN64) && ! defined(__aarch64__)
# error Handlebars requires a 64-bit architecture
#endif

#ifndef BOOST_HANDLEBARS_NO_UNIQUE_ADDRESS
# if defined(__cpp_lib_no_unique_address)
#  define BOOST_HANDLEBARS_NO_UNIQUE_ADDRESS [[no_unique_address]]
# else
#  define BOOST_HANDLEBARS_NO_UNIQUE_ADDRESS
# endif
#endif

} // handlebars
} // boost

#endif
