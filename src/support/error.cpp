//
//  Copyright (c) 2023 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/cppalliance/handlebars
//

#include <boost/handlebars/config.hpp>
#include <boost/handlebars/support/assert.hpp>
#include <boost/handlebars/support/path.hpp>
#include <cstdlib>
#include <format>


namespace SourceFileNames {
extern char const* getFileName(char const*) noexcept;
} // SourceFileNames


namespace boost::handlebars {

//------------------------------------------------
//
// Error
//
//------------------------------------------------

std::string
Error::
formatWhere(
    source_location const& loc)
{
  return std::format("{}:{}", ::SourceFileNames::getFileName(loc.file_name()),
                     loc.line());
#if 0
    return std::format(
        "{}@{}({})",
        loc.function_name(),
        ::SourceFileNames::getFileName(
            loc.file_name()),
        loc.line());
#endif
}

std::string
Error::
formatMessage(
    std::string_view const& reason,
    std::string_view const& where)
{
    std::string result;
    result = reason;
    result.append(" (");
    result.append(where);
    result.append(")");
    return result;
}

Error::
Error(
    std::string reason,
    source_location loc)
    : where_(formatWhere(loc))
    , reason_(std::move(reason))
    , message_(formatMessage(reason_, where_))
    , loc_(loc)
{
    BOOST_HANDLEBARS_ASSERT(! message_.empty());
}

Error::
Error(
    std::error_code const& ec,
    source_location loc)
{
    if(! ec)
        return;
    where_ = formatWhere(loc);
    reason_ = ec.message();
    message_ = formatMessage(reason_, where_);
    loc_ = loc;
}

Error::
Error(
    std::exception const& ex)
{
    std::string_view s(ex.what());
    if(s.empty())
        s = "unknown exception";
    reason_ = s;
    message_ = s;
}

Error::
Error(
    std::vector<Error> const& errors,
    source_location loc)
{
    BOOST_HANDLEBARS_ASSERT(errors.size() > 0);
    if(errors.size() == 1)
    {
        *this = errors.front();
        return;
    }

    where_ = formatWhere(loc);
    reason_ = std::format("{} errors occurred:\n", errors.size());
    for(auto const& err : errors)
    {
        reason_.append("    ");
        reason_.append(err.message());
        reason_.push_back('\n');
    }
    message_ = formatMessage(reason_, where_);
    loc_ = loc;
}

void Error::Throw() const&
{
    BOOST_HANDLEBARS_ASSERT(failed());
    throw Exception(*this);
}

void Error::Throw() &&
{
    BOOST_HANDLEBARS_ASSERT(failed());
    throw Exception(std::move(*this));
}

//------------------------------------------------

SourceLocation::
SourceLocation(
    source_location const& loc) noexcept
    : file_(files::getSourceFilename(loc.file_name()))
    , line_(loc.line())
    , col_(loc.column())
    , func_(loc.function_name())
{
}

}

