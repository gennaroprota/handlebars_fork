//
// Copyright (c) 2023 Krystian Stasiowski (sdkrystian@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/cppalliance/handlebars
//

#include <boost/handlebars/support/assert.hpp>
#include <format>
#include <iostream>

namespace boost {
namespace handlebars {

void
assert_failed(
    char const* msg,
    char const* file,
    std::uint_least32_t line)
{
    std::cerr << std::format("assertion failed: {} on line {} in {}\n", msg, line, file);
}

} // handlebars
} // boost
