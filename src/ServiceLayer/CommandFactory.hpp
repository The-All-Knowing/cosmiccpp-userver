#pragma once

#include "Domain/Commands.hpp"
#include "schemas/AddBatch.hpp"
#include "schemas/Allocate.hpp"


namespace Allocation::ServiceLayer
{
    template <class>
    inline constexpr bool always_false_v = false;

    template <typename T>
    Domain::CommandPtr MakeCommand(const T&)
    {
        static_assert(always_false_v<T>, "MakeCommand<T> must be specialized for the given type T");
    }

    template <>
    Domain::CommandPtr MakeCommand(const AddBatch::CreateBatch& commandDto);

    template <>
    Domain::CommandPtr MakeCommand(const Allocate::Allocate& commandDto);
}