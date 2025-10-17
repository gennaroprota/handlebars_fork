//
// Copyright (c) 2023 Krystian Stasiowski (sdkrystian@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/cppalliance/handlebars
//

#ifndef BOOST_HANDLEBARS_ASSERT_HPP
#define BOOST_HANDLEBARS_ASSERT_HPP

#include <cstdint>

namespace boost {
namespace handlebars {

#ifdef NDEBUG
    #ifdef __GNUC__
        #define BOOST_HANDLEBARS_UNREACHABLE() static_cast<void>(__builtin_unreachable())
    #elif defined(_MSC_VER)
        #define BOOST_HANDLEBARS_UNREACHABLE() static_cast<void>(__assume(false))
    #endif
    #define BOOST_HANDLEBARS_ASSERT(x) static_cast<void>(false)
#else
    #ifdef __GNUC__
        #define BOOST_HANDLEBARS_UNREACHABLE() static_cast<void>(__builtin_trap(), __builtin_unreachable())
    #elif defined(_MSC_VER)
        #define BOOST_HANDLEBARS_UNREACHABLE() static_cast<void>(__debugbreak(), __assume(false))
    #endif

    void
    assert_failed(
        char const* msg,
        char const* file,
        std::uint_least32_t line);

    #define BOOST_HANDLEBARS_ASSERT(x) static_cast<void>(!! (x) || \
        (assert_failed(#x, __builtin_FILE(), __builtin_LINE()), \
        BOOST_HANDLEBARS_UNREACHABLE(), true))
#endif

} // handlebars
} // boost

#endif