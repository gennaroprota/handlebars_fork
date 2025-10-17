//
// Copyright (c) 2024 Alan de Freitas (alandefreitas@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/cppalliance/handlebars
//

#include <boost/handlebars/support/string.hpp>

namespace boost::handlebars {

void
replace(std::string& s, std::string_view from, std::string_view to)
{
    if (from.empty())
        return;

    size_t start_pos = 0;
    while ((start_pos = s.find(from, start_pos)) != std::string::npos) {
        s.replace(start_pos, from.length(), to);
        start_pos += to.length();
    }
}

} // boost::handlebars
