//
// Copyright (c) 2023 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/cppalliance/handlebars
//

#include <boost/handlebars/dom/function.hpp>


namespace boost::handlebars::dom {

namespace {

struct NullFunction : public FunctionImpl
{
    Expected<Value>
    call(Array const&) const override
    {
        return dom::Value(dom::Kind::Undefined);
    }
};

static Function const nullFunction =
    newFunction<NullFunction>();

} // (anon)

Function::
~Function() = default;

Function::
Function() noexcept
    : Function(nullFunction)
{
}

Function::
Function(
    Function&& other) noexcept
    : Function()
{
    swap(other);
}

Function::
Function(
    Function const& other) noexcept
    : impl_(other.impl_)
{
}

Function&
Function::
operator=(
    Function&& other) noexcept
{
    Function temp(std::move(other));
    swap(temp);
    return *this;
}

Function&
Function::
operator=(
    Function const& other) noexcept
{
    if(this != &other)
        impl_ = other.impl_;
    return *this;
}

//------------------------------------------------
//
// FunctionImpl
//
//------------------------------------------------

char const*
FunctionImpl::
type_key() const noexcept
{
    return "Function";
}

}

