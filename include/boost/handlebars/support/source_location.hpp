//
// Copyright (c) 2023 Krystian Stasiowski (sdkrystian@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/cppalliance/handlebars
//

#ifndef BOOST_HANDLEBARS_SUPPORT_SOURCE_LOCATION_HPP
#define BOOST_HANDLEBARS_SUPPORT_SOURCE_LOCATION_HPP

#include <version>

#if __cpp_lib_source_location >= 201907L && \
    __has_include(<source_location>)
    #include <source_location>

    namespace boost {
    namespace handlebars {

    using std::source_location;

    } // handlebars
    } // boost
#else
    #include <cstdint>

    namespace boost {
    namespace handlebars {

    struct source_location
    {
        static
        constexpr
        source_location
        current(
            char const* const file = __builtin_FILE(),
            char const* const function = __builtin_FUNCTION(),
            const std::uint_least32_t line = __builtin_LINE(),
            const std::uint_least32_t column = __builtin_COLUMN()) noexcept

        {
            source_location result;
            result.file_ = file;
            result.function_ = function;
            result.line_ = line;
            result.column_ = column;
            return result;
        }

        constexpr
        char const*
        file_name() const noexcept
        {
            return file_;
        }
        constexpr
        char const*
        function_name() const noexcept
        {
            return function_;
        }

        constexpr
        std::uint_least32_t
        line() const noexcept
        {
            return line_;
        }
        constexpr
        std::uint_least32_t
        column() const noexcept
        {
            return column_;
        }

    private:
        char const* file_ = "";
        char const* function_ = "";
        std::uint_least32_t line_ = 0;
        std::uint_least32_t column_ = 0;
    };

} // handlebars
} // boost
#endif
#endif
